#include "MC9S12NE64.h"
#include "datatypes.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "stdlib.h"
#include "MATH.h"
#include "Commdef.h"

#ifdef  MOD_MODBUSRTU_AUTOQUERY_ENABLED   // Generar aplicación que utiliza PROC_MODBUS_AUTOQUERY

#include "pModbusAutoQuery.h"
#include "global.h"


///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_MODBUS_AUTOQUERY
///////////////////////////////////////////////////////////////////////////////
//

const EVENT_FUNCTION	MAQProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_MOD_ModbusRtuAutoQueryIdleTimeout },
{EVT_MODRTU_TXMSG     , f_MOD_ModbusRtuAutoQueryIdleRxMsgEnd},
{EVT_MODRTU_ERR       , f_MOD_ModbusRtuAutoQueryIdleRxMsgErr},
{EVT_OTHER_MSG     		, f_Consume         }};


////////////////////////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_MODBUS_AUTOQUERY
////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------
//-------------- estado : IDLE ----------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//--Timeout, se envía periódicamente un comando de consulta
//
void f_MOD_ModbusRtuAutoQueryIdleTimeout (void) {

    UINT8   bRetCode; 
    UINT16  wSenRequestParam;
    UINT8*  pModbusMsg;
		 
    MAQ_WaitAnswerTimerId = (UINT8)(-1);

    bRetCode = 0x00;
    while (bRetCode < MAX_AUTOQUERY_BANKS){ 
       
      bRetCode++;
      MAQ_IdxPoll++; 
      if (MAQ_IdxPoll >= MAX_AUTOQUERY_BANKS) MAQ_IdxPoll=0x00; 
                
      if (MAQ_BankMemManager[MAQ_IdxPoll].BankStatus) break; // si esta ocupado enviar encuesta
    }
    
    if (MAQ_BankMemManager[MAQ_IdxPoll].BankStatus)
    {
        
         // Recuperar el puntero al buffer utilizado para recibir el mensaje
        if (MemGetBufferPtr( MAQ_RxMsgToken , &((void*)pModbusMsg) ) != OK ){    
            MON_ResetIfNotConnected(0XA4);    // Error de SW, Reset (TODO: revisar acciones a tomar)
            return;   
        }
        
        pModbusMsg[0] =  MAQ_PollingNode;  // node
        pModbusMsg[1] =  MAQ_BankMemManager[MAQ_IdxPoll].ModbusQueryOpCode;  // func code
        pModbusMsg[2] =  (UINT8)((MAQ_BankMemManager[MAQ_IdxPoll].ModbusQueryAddress & 0xFF00)>>8);   
        pModbusMsg[3] =  (UINT8)(MAQ_BankMemManager[MAQ_IdxPoll].ModbusQueryAddress & 0x00FF);
        pModbusMsg[4] =  0x00;
        pModbusMsg[5] =  MAQ_BankMemManager[MAQ_IdxPoll].ModbusQueryLen;
        

        NEX_ModbusRtuBuildCRC16((UINT8 *)(&(pModbusMsg[0])), 6);

        wSenRequestParam = 0;     // En la parte alta del parámetro se pasa el offset de transmisión
        wSenRequestParam <<= 8;
        wSenRequestParam |= MAQ_RxMsgToken;

        (void)MemSetUsedLen(MAQ_RxMsgToken, 0x08);   // encuesta modbus 8 bytes
      
        // Marcar el buffer con el ID del proceso..
        (void)MemSetRouteMark(MAQ_RxMsgToken, PROC_MODBUS_AUTOQUERY);
        
        // Enviar mensaje al proceso NEXO para que redireccione la encuesta
        PutEventMsg (EVT_MODRTU_MSG, PROC_NEXO, PROC_MODBUS_AUTOQUERY, MAQ_RxMsgToken);    
    
#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
		  (void)sprintf((char *)udp_buf,"MAQ_MsgTxed: %X, %X, %X, %X ,%X, %X \n",
		                ((BYTE*)pModbusMsg)[0],  // 
		                ((BYTE*)pModbusMsg)[1],  // 
		                ((BYTE*)pModbusMsg)[2],  // 
		                ((BYTE*)pModbusMsg)[3],  // 
		                ((BYTE*)pModbusMsg)[4],  // 
		                ((BYTE*)pModbusMsg)[5] ); 
      udp_data_to_send = TRUE;
  }
#endif    
    
    }
    else            
    {
   	  // Iniciar un timer para enviar comando..
    	MAQ_WaitAnswerTimerId =  TMR_SetTimer ( TMR_MAQ_RESETLINKWAIT , PROC_MODBUS_AUTOQUERY, 0x00, FALSE); 
    }
                  
    return; 
}




//-----------------------------------------------------------------------------
//--RxAtEnd se analiza el mensaje recibido como respuesta a un comando
//
void f_MOD_ModbusRtuAutoQueryIdleRxMsgEnd (void) {

  UINT8   RxEndToken;
  void*   vpAuxBuffer;
//  WORD    wCalcCrc;
//  WORD    wValCrc;
  BYTE*   MAQ_buff;
  UINT16  wRxBuffLen; 

  
  TMR_FreeTimer (MAQ_WaitAnswerTimerId);
  MAQ_WaitAnswerTimerId = 0xFF;
  
// Recuperar el puntero al buffer utilizado para recibir el mensaje
// El token del buffer está en la parte baja del parámetro del mensaje
  RxEndToken = (UINT8)CurrentMsg.Param;
  if (MemGetBufferPtr( RxEndToken , &vpAuxBuffer ) != OK ){    
  // ABR: control de errores
    MAQ_WaitAnswerTimerId =  TMR_SetTimer ( MAQ_PollingPeriod , PROC_MODBUS_AUTOQUERY, 0x00, FALSE);  
    return;   
  }
  MAQ_buff = (BYTE*)vpAuxBuffer;
  
  if ( MemGetUsedLen(RxEndToken, &wRxBuffLen) != OK)     // Bytes recibidos
      MON_ResetIfNotConnected(0x21);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      
      
#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
		  (void)sprintf((char *)udp_buf,"MAQ_MsgRxOk: %X, %X, %X, %X ,%X, %X, %X \n",
		                ((BYTE*)vpAuxBuffer)[0],  // 
		                ((BYTE*)vpAuxBuffer)[1],  // 
		                ((BYTE*)vpAuxBuffer)[2],  // 
		                ((BYTE*)vpAuxBuffer)[3],  // 
		                ((BYTE*)vpAuxBuffer)[4],  // 
		                ((BYTE*)vpAuxBuffer)[5],  // 
		                ((BYTE*)vpAuxBuffer)[6] ); 
      udp_data_to_send = TRUE;
  }
#endif    


 /*
		// Vemos el CRC
				wCalcCrc = MAQ_buff[88];
        NEX_ModbusRtuBuildCRC16((UINT8 *)(&(pModbusMsg[0])), 6);
		
		if ( *((WORD*)(&MAQ_buff[MAQ_len-2])) != wCalcCrc) // Crc ok ?
    {      
#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
		  (void)sprintf((char *)udp_buf,"BAD_CrcRxMsg: CalcCrc=%X, ValCrc=%X\n",
		                wCalcCrc,
		                *((WORD*)(&MAQ_buff[MAQ_len-2])) );
      udp_data_to_send = TRUE;
   }   
#endif    
    }
*/    
  
		// Limpio el buffer del banco correspondiente
		(void)memset(&autoQueryModbusData[MAQ_BankMemManager[MAQ_IdxPoll].BankOffsetStart], 0x00, (wRxBuffLen-3-2));//(2*MAQ_BankMemManager[MAQ_IdxPoll].ModbusQueryLen));
		
		// Copio los datos menos el CRC,
		(void)memcpy(&autoQueryModbusData[MAQ_BankMemManager[MAQ_IdxPoll].BankOffsetStart], (MAQ_buff+3), (wRxBuffLen-3-2)); 
 
  
    MAQ_WaitAnswerTimerId =  TMR_SetTimer ( MAQ_PollingPeriod , PROC_MODBUS_AUTOQUERY, 0x00, FALSE);  
    return; 
}



//-----------------------------------------------------------------------------
//--RxAtEnd se analiza el mensaje recibido como respuesta con error
//
void f_MOD_ModbusRtuAutoQueryIdleRxMsgErr (void) {

  UINT8   RxEndToken;
  void*   vpAuxBuffer;
  UINT16  wRxBuffLen; 

  
  TMR_FreeTimer (MAQ_WaitAnswerTimerId);
  MAQ_WaitAnswerTimerId = 0xFF;
  
// Recuperar el puntero al buffer utilizado para recibir el mensaje
// El token del buffer está en la parte baja del parámetro del mensaje
  RxEndToken = (UINT8)CurrentMsg.Param;
  if (MemGetBufferPtr( RxEndToken , &vpAuxBuffer ) != OK ){    
  // ABR: control de errores
    MAQ_WaitAnswerTimerId =  TMR_SetTimer ( MAQ_PollingPeriod , PROC_MODBUS_AUTOQUERY, 0x00, FALSE);  
    return;   
  }
  
  if ( MemGetUsedLen(RxEndToken, &wRxBuffLen) != OK)     // Bytes recibidos
      MON_ResetIfNotConnected(0x21);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      
      
#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
		  (void)sprintf((char *)udp_buf,"MAQ_MsgRxERR: %X, %X, %X, %X ,%X, %X, %X \n",
		                ((BYTE*)vpAuxBuffer)[0],  // 
		                ((BYTE*)vpAuxBuffer)[1],  // 
		                ((BYTE*)vpAuxBuffer)[2],  // 
		                ((BYTE*)vpAuxBuffer)[3],  // 
		                ((BYTE*)vpAuxBuffer)[4],  // 
		                ((BYTE*)vpAuxBuffer)[5],  // 
		                ((BYTE*)vpAuxBuffer)[6] ); 
      udp_data_to_send = TRUE;
  }
#endif    

  
    MAQ_WaitAnswerTimerId =  TMR_SetTimer ( MAQ_PollingPeriod , PROC_MODBUS_AUTOQUERY, 0x00, FALSE);  
    return; 
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
////////////////////////////////////////////////////////////////////////////////////////////////
//

//--------------------------------------------------------------------------------------
//-- Funcion de Inicialización
//
void MOD_ModbusRtuAutoQueryInit (void)
{
  UINT8*  pTmpRxBuffer;
  UINT8   bTmpConter;    

  MAQ_PollingNode = 0x00;

// Las direcciones son WORD, que ya están grabada en EEPROM con formato LSB:MSB
//  eeprom_read_buffer(EE_ADDR_DNP_SLAVE, (UINT8 *)&BAC_wRemoteAddr, 2); 
//  eeprom_read_buffer(EE_ADDR_DNP_MASTER, (UINT8 *)&BAC_wLocalAddr, 2);		 


  //MAQ_Diag.WordDiag = 0;	
  MAQ_CtrlFlags = 0x00;

  eeprom_read_buffer(EE_ADDR_MAQ_STATE, (UINT8 *)&bTmpConter, 1);	
  if (bTmpConter == AUTOQUERY_BANKS_INI) 
  {
      eeprom_read_buffer(EE_ADDR_MAQ_BANKS, 
     	                  (UINT8 *)MAQ_BankMemManager, 
     	                  sizeof(AUTOQUERY_BANK_ALLOC)*MAX_AUTOQUERY_BANKS);         
  } 
  else
  {
      MOD_ModbusRtuAutoQueryResetBanks();    	
  }
	
	
  eeprom_read_buffer(EE_ADDR_NODE_SCIB, 
 	                  (UINT8 *)&MAQ_PollingNode,2); 
	
  eeprom_read_buffer(EE_ADDR_INV_POLL, 
 	                  (UINT8 *)&MAQ_PollingPeriod,2); 

 	//eeprom_read_buffer(EE_ADDR_NODE_SCIA, &MAQ_EnablePolling,1);
 	
 	MAQ_EnablePolling = SYS_ConfigParam.RunConfig & CFG_RTU_RUNSTATE_MASK;          
      
	if (MAQ_EnablePolling) 
	{
	  MAQ_CtrlFlags |= AUTOQUERY_MASK_POL_ENABLE; 
	  
  	 
  	//Reservar un buffer para la recepción de mensajes
    if ( ERROR == MemGetBuffer ( (void**)(&pTmpRxBuffer), &MAQ_RxMsgToken)){
        // TODO: ABR, implementar logs en FLASH
        //PutEventMsg (EVT_LOG_ID, PROC_DEBUG, OS_NOT_PROC, (UINT16)LOG_ID_TCP_NOBUFF);    
        MON_ResetIfNotConnected(0x26);
        return; 
    }     
    
     MAQ_IdxPoll = 0x00;
 	  // Iniciar un timer para enviar comando..
  	MAQ_WaitAnswerTimerId =  TMR_SetTimer ( TMR_MAQ_RESETLINKWAIT , PROC_MODBUS_AUTOQUERY, 0x00, FALSE); 
   
	}
	else
	  MAQ_CtrlFlags &= ~AUTOQUERY_MASK_POL_ENABLE; 
	
   
}



void MOD_ModbusRtuAutoQueryResetBanks(void) 
{
  UINT8   bTmpConter;

      // inicializar indices de administrador de bancos	
    	for(bTmpConter=0 ; bTmpConter<MAX_AUTOQUERY_BANKS ; bTmpConter++)
    	{	  	    	    
    	    MAQ_BankMemManager[bTmpConter].ModbusQueryAddress = 0x0;
    	    MAQ_BankMemManager[bTmpConter].ModbusQueryOpCode  = 0x03;
    	    MAQ_BankMemManager[bTmpConter].ModbusQueryLen     = 0x00;
    	    MAQ_BankMemManager[bTmpConter].BankOffsetStart    = 0x0000;
    	    MAQ_BankMemManager[bTmpConter].BankModbusAddress  = (bTmpConter+1)*1000;
    	    MAQ_BankMemManager[bTmpConter].BankStatus         = 0x00;         // libre
    	}
    	
      eeprom_write_buffer(EE_ADDR_MAQ_BANKS, 
     	                  (UINT8 *)MAQ_BankMemManager, 
     	                  sizeof(AUTOQUERY_BANK_ALLOC)*MAX_AUTOQUERY_BANKS); 
     	bTmpConter = AUTOQUERY_BANKS_INI;
      eeprom_write_buffer(EE_ADDR_MAQ_STATE, (UINT8 *)&bTmpConter, 1);	     	                         
  
      MON_ResetIfNotConnected(0xFF); // REsetear para comenzar un ciclo de encuestas desde el principio.
}

void MOD_ModbusRtuAutoQuerySaveBanks(void) 
{
    
     UINT8   bTmpConter;
     eeprom_write_buffer(EE_ADDR_MAQ_BANKS, 
     	                  (UINT8 *)MAQ_BankMemManager, 
     	                  sizeof(AUTOQUERY_BANK_ALLOC)*MAX_AUTOQUERY_BANKS); 
     	bTmpConter = AUTOQUERY_BANKS_INI;
      eeprom_write_buffer(EE_ADDR_MAQ_STATE, (UINT8 *)&bTmpConter, 1);
      
}

#pragma CODE_SEG DEFAULT


#endif		//  MOD_MODBUSRTU_AUTOQUERY_ENABLED  - Generar aplicación que utiliza PROC_MODBUS_AUTOQUERY


//--------------------------------------------BUFFER_GARBAGE_INI------------------------------------------

//--------------------------------------------BUFFER_GARBAGE_END------------------------------------------


