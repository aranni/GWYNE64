#include "MC9S12NE64.h"
#include "datatypes.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "stdlib.h"
#include "MATH.h"
#include "Commdef.h"

#ifdef  MOD_SYMEO_ENABLED   // Generar aplicación que utiliza PROC_MODBUS_SYMEO

#include "pSymeo.h"
#include "global.h"


///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_SYMEO
///////////////////////////////////////////////////////////////////////////////
//

const EVENT_FUNCTION	SYMProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_MOD_SymeoIdleTimeout },
{EVT_SYMEOTCP_RXMSG 	, f_MOD_SymeoTcpIdleAnalizeMsg  },
{EVT_SYMEOUDP_RXMSG 	, f_MOD_SymeoTcpIdleAnalizeMsg  },
{EVT_OTHER_MSG     		, f_Consume         }};


////////////////////////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_SYMEO
////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------
//-------------- estado : IDLE ----------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//--Timeout, se envía periódicamente un comando de consulta
//
void f_MOD_SymeoIdleTimeout (void) {

/*    
    UINT8   bRetCode; 
    UINT16  wSenRequestParam;
    UINT8*  pModbusMsg;
		 
    SYM_WaitAnswerTimerId = (UINT8)(-1);

    bRetCode = 0x00;
    while (bRetCode < MAX_AUTOQUERY_BANKS){ 
       
      bRetCode++;
      SYM_IdxPoll++; 
      if (SYM_IdxPoll >= MAX_AUTOQUERY_BANKS) SYM_IdxPoll=0x00; 
                
      if (SYM_BankMemManager[SYM_IdxPoll].BankStatus) break; // si esta ocupado enviar encuesta
    }
    
    if (SYM_BankMemManager[SYM_IdxPoll].BankStatus)
    {
        
         // Recuperar el puntero al buffer utilizado para recibir el mensaje
        if (MemGetBufferPtr( SYM_RxMsgToken , &((void*)pModbusMsg) ) != OK ){    
            MON_ResetIfNotConnected(0XA4);    // Error de SW, Reset (TODO: revisar acciones a tomar)
            return;   
        }
        
        pModbusMsg[0] =  SYM_PollingNode;  // node
        pModbusMsg[1] =  SYM_BankMemManager[SYM_IdxPoll].ModbusQueryOpCode;  // func code
        pModbusMsg[2] =  (UINT8)((SYM_BankMemManager[SYM_IdxPoll].ModbusQueryAddress & 0xFF00)>>8);   
        pModbusMsg[3] =  (UINT8)(SYM_BankMemManager[SYM_IdxPoll].ModbusQueryAddress & 0x00FF);
        pModbusMsg[4] =  0x00;
        pModbusMsg[5] =  SYM_BankMemManager[SYM_IdxPoll].ModbusQueryLen;
        

        NEX_ModbusRtuBuildCRC16((UINT8 *)(&(pModbusMsg[0])), 6);

        wSenRequestParam = 0;     // En la parte alta del parámetro se pasa el offset de transmisión
        wSenRequestParam <<= 8;
        wSenRequestParam |= SYM_RxMsgToken;

        (void)MemSetUsedLen(SYM_RxMsgToken, 0x08);   // encuesta modbus 8 bytes
      
        // Marcar el buffer con el ID del proceso..
        (void)MemSetRouteMark(SYM_RxMsgToken, PROC_SYMEO);
        
        // Enviar mensaje al proceso NEXO para que redireccione la encuesta
        PutEventMsg (EVT_MODRTU_MSG, PROC_NEXO, PROC_SYMEO, SYM_RxMsgToken);    
    
#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
		  (void)sprintf((char *)udp_buf,"SYM_MsgTxed: %X, %X, %X, %X ,%X, %X \n",
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
    	SYM_WaitAnswerTimerId =  TMR_SetTimer ( TMR_SYM_RESETLINKWAIT , PROC_SYMEO, 0x00, FALSE); 
    }
*/                  
    return; 
}




//-----------------------------------------------------------------------------
//--RxAtEnd se analiza el mensaje recibido como respuesta a un comando
//
void f_MOD_SymeoTcpIdleAnalizeMsg  (void) {

  UINT8   RxEndToken;
  void*   vpAuxBuffer;

  BYTE*   SYM_buff;
  UINT16  wRxBuffLen; 

  
  TMR_FreeTimer (SYM_WaitAnswerTimerId);
  SYM_WaitAnswerTimerId = 0xFF;
  
// Recuperar el puntero al buffer utilizado para recibir el mensaje
// El token del buffer está en la parte baja del parámetro del mensaje
  RxEndToken = (UINT8)CurrentMsg.Param;
  if (MemGetBufferPtr( RxEndToken , &vpAuxBuffer ) != OK ){    
  // ABR: control de errores
  // OjO_REVISAR !!!!!!!!!!!!!
    (void)MemFreeBuffer (RxEndToken);   // Liberar el buffer
    //SYM_WaitAnswerTimerId =  TMR_SetTimer ( SYM_PollingPeriod , PROC_SYMEO, 0x00, FALSE);  
    return;   
  }
  SYM_buff = (BYTE*)vpAuxBuffer;
  
  if ( MemGetUsedLen(RxEndToken, &wRxBuffLen) != OK)     // Bytes recibidos
      MON_ResetIfNotConnected(0x21);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      
      
#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
		  (void)sprintf((char *)udp_buf,"SYM_MsgRx: Vel:[%X,%X,%X,%X] Lev:%d, Err:%d, Stat:%d\n",
		                ((BYTE*)vpAuxBuffer)[7],  // 
		                ((BYTE*)vpAuxBuffer)[8],  // 
		                ((BYTE*)vpAuxBuffer)[9],  // 
		                ((BYTE*)vpAuxBuffer)[10],  // 
		                ((BYTE*)vpAuxBuffer)[15],  // 
		                ((BYTE*)vpAuxBuffer)[16],  // 
		                ((BYTE*)vpAuxBuffer)[17] ); 
      udp_data_to_send = TRUE;
  }
#endif    

		if ( ((BYTE*)vpAuxBuffer)[1] == 0x00) {
	
       	SYM_BankMemManager[0].SourceGroupId   = ((SYMEO_LPR_MSG*)vpAuxBuffer)->SourceAddr.AddrFields.groupID ;     // Id de grupo del transmisor
      	SYM_BankMemManager[0].SourceStationId = (BYTE)((SYMEO_LPR_MSG*)vpAuxBuffer)->SourceAddr.AddrFields.stationID;    // Id de estacion del transmisor
      	SYM_BankMemManager[0].SourceBBt       = (BYTE)((SYMEO_LPR_MSG*)vpAuxBuffer)->SourceAddr.AddrFields.BB;           // Base station Bit del transmisor 1:master 0:slave
      	SYM_BankMemManager[0].TargetGroupId   = ((SYMEO_LPR_MSG*)vpAuxBuffer)->TargetAddr.AddrFields.groupID ;     // Id de grupo del receptor
      	SYM_BankMemManager[0].TargetStationId = (BYTE)((SYMEO_LPR_MSG*)vpAuxBuffer)->TargetAddr.AddrFields.stationID;    // Id de estacion del receptor
      	SYM_BankMemManager[0].TargetBBt       = (BYTE)((SYMEO_LPR_MSG*)vpAuxBuffer)->TargetAddr.AddrFields.BB;           // Base station Bit del receptor 1:master 0:slave
      	SYM_BankMemManager[0].Distance        = ((SYMEO_LPR_MSG*)vpAuxBuffer)->Distance;                // Distancia mm
      	SYM_BankMemManager[0].Velocity        = ((SYMEO_LPR_MSG*)vpAuxBuffer)->Velocity;                // Velocidad mm/s
      	SYM_BankMemManager[0].Level           = ((SYMEO_LPR_MSG*)vpAuxBuffer)->Level;                   // Señal en dB
      	SYM_BankMemManager[0].Error           = ((SYMEO_LPR_MSG*)vpAuxBuffer)->Error;                   // error
      	SYM_BankMemManager[0].Status          = ((SYMEO_LPR_MSG*)vpAuxBuffer)->Status;                  // estado
      	SYM_BankMemManager[0].WatchDog++;             // byte check
		}

    (void)MemFreeBuffer (RxEndToken);   // Liberar el buffer
  
		// Limpio el buffer del banco correspondiente
		//(void)memset(&autoQueryModbusData[SYM_BankMemManager[SYM_IdxPoll].BankOffsetStart], 0x00, (wRxBuffLen-3-2));//(2*SYM_BankMemManager[SYM_IdxPoll].ModbusQueryLen));
		
		// Copio los datos menos el CRC,
		//(void)memcpy(&autoQueryModbusData[SYM_BankMemManager[SYM_IdxPoll].BankOffsetStart], (SYM_buff+3), (wRxBuffLen-3-2)); 
 
  
    //SYM_WaitAnswerTimerId =  TMR_SetTimer ( SYM_PollingPeriod , PROC_SYMEO, 0x00, FALSE);  
    return; 
}



////////////////////////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
////////////////////////////////////////////////////////////////////////////////////////////////
//

//--------------------------------------------------------------------------------------
//-- Funcion de Inicialización
//
void MOD_SymeoInit (void)
{
/*
  UINT8*  pTmpRxBuffer;
  UINT8   bTmpConter;    

  SYM_PollingNode = 0x00;

  SYM_CtrlFlags = 0x00;

  eeprom_read_buffer(EE_ADDR_SYM_STATE, (UINT8 *)&bTmpConter, 1);	
  if (bTmpConter == AUTOQUERY_BANKS_INI) 
  {
      eeprom_read_buffer(EE_ADDR_SYM_BANKS, 
     	                  (UINT8 *)SYM_BankMemManager, 
     	                  sizeof(AUTOQUERY_BANK_ALLOC)*MAX_AUTOQUERY_BANKS);         
  } 
  else
  {
      MOD_SymeoResetBanks();    	
  }
	
	
  eeprom_read_buffer(EE_ADDR_NODE_SCIB, 
 	                  (UINT8 *)&SYM_PollingNode,2); 
	
  eeprom_read_buffer(EE_ADDR_INV_POLL, 
 	                  (UINT8 *)&SYM_PollingPeriod,2); 

 	//eeprom_read_buffer(EE_ADDR_NODE_SCIA, &SYM_EnablePolling,1);
 	
 	SYM_EnablePolling = SYS_ConfigParam.RunConfig & CFG_RTU_RUNSTATE_MASK;          
      
	if (SYM_EnablePolling) 
	{
	  SYM_CtrlFlags |= AUTOQUERY_MASK_POL_ENABLE; 
	  
  	 
  	//Reservar un buffer para la recepción de mensajes
    if ( ERROR == MemGetBuffer ( (void**)(&pTmpRxBuffer), &SYM_RxMsgToken)){
        // TODO: ABR, implementar logs en FLASH
        //PutEventMsg (EVT_LOG_ID, PROC_DEBUG, OS_NOT_PROC, (UINT16)LOG_ID_TCP_NOBUFF);    
        MON_ResetIfNotConnected(0x26);
        return; 
    }     
    
     SYM_IdxPoll = 0x00;
 	  // Iniciar un timer para enviar comando..
  	SYM_WaitAnswerTimerId =  TMR_SetTimer ( TMR_SYM_RESETLINKWAIT , PROC_SYMEO, 0x00, FALSE); 
   
	}
	else
	  SYM_CtrlFlags &= ~AUTOQUERY_MASK_POL_ENABLE; 
*/	
  return;
   
}

/*

void MOD_SymeoResetBanks(void) 
{
  UINT8   bTmpConter;

      // inicializar indices de administrador de bancos	
    	for(bTmpConter=0 ; bTmpConter<MAX_AUTOQUERY_BANKS ; bTmpConter++)
    	{	  	    	    
    	    SYM_BankMemManager[bTmpConter].ModbusQueryAddress = 0x0;
    	    SYM_BankMemManager[bTmpConter].ModbusQueryOpCode  = 0x03;
    	    SYM_BankMemManager[bTmpConter].ModbusQueryLen     = 0x00;
    	    SYM_BankMemManager[bTmpConter].BankOffsetStart    = 0x0000;
    	    SYM_BankMemManager[bTmpConter].BankModbusAddress  = (bTmpConter+1)*1000;
    	    SYM_BankMemManager[bTmpConter].BankStatus         = 0x00;         // libre
    	}
    	
      eeprom_write_buffer(EE_ADDR_SYM_BANKS, 
     	                  (UINT8 *)SYM_BankMemManager, 
     	                  sizeof(AUTOQUERY_BANK_ALLOC)*MAX_AUTOQUERY_BANKS); 
     	bTmpConter = AUTOQUERY_BANKS_INI;
      eeprom_write_buffer(EE_ADDR_SYM_STATE, (UINT8 *)&bTmpConter, 1);	     	                         
  
      MON_ResetIfNotConnected(0xFF); // REsetear para comenzar un ciclo de encuestas desde el principio.
}

void MOD_SymeoSaveBanks(void) 
{
    
     UINT8   bTmpConter;
     eeprom_write_buffer(EE_ADDR_SYM_BANKS, 
     	                  (UINT8 *)SYM_BankMemManager, 
     	                  sizeof(AUTOQUERY_BANK_ALLOC)*MAX_AUTOQUERY_BANKS); 
     	bTmpConter = AUTOQUERY_BANKS_INI;
      eeprom_write_buffer(EE_ADDR_SYM_STATE, (UINT8 *)&bTmpConter, 1);
      
}

*/

#pragma CODE_SEG DEFAULT


#endif		//  MOD_MODBUSRTU_AUTOQUERY_ENABLED  - Generar aplicación que utiliza PROC_SYMEO


//--------------------------------------------BUFFER_GARBAGE_INI------------------------------------------

//--------------------------------------------BUFFER_GARBAGE_END------------------------------------------


