#include "MC9S12NE64.h"
#include "datatypes.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "stdlib.h"
#include "MATH.h"

#include "Commdef.h"
#include "pBac.h"
#include "global.h"

extern unsigned short CalcCrc(unsigned char * pData, unsigned short length);


///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_BAC
///////////////////////////////////////////////////////////////////////////////
//

const EVENT_FUNCTION	BACProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_BACIdleTimeout },
{EVT_232_RXMSG 	      , f_BACIdleRxMsgEnd},
{EVT_232_TXMSG 	      , f_BACIdleTxMsgEnd},
{EVT_CPY_TXMSG 	      , f_BACIdleCpyTxMsgEnd},
{EVT_OTHER_MSG     		, f_Consume         }};


////////////////////////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_BAC
////////////////////////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------------------------------
//-------------- estado : IDLE ----------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//--Timeout, se envía periódicamente un comando de consulta
//
void f_BACIdleTimeout (void) {
  UINT8   bRetCode; 

//  UINT8*  pTxBuffer;
//  UINT8   bTmpToken; 
//  (void)MemGetBuffer ( (void**)(&pTxBuffer), &bTmpToken);
//  wMsgParam = bTmpToken; // Token en la parte baja.
//  wMsgParam |= 0x0200;    // LEn en la parte alta del parametro
//  pTxBuffer[0]=0x10;    // 
//  pTxBuffer[1]=0x06;    // 
//  PutEventMsg (EVT_485_TXREQ, PROC_SCIA, PROC_BAC, wMsgParam);     // Enviar requerimiento de TX.
//  bRetCode = SCIA_TransmitFrame  (2,bTmpToken);

  BAC_WaitAnswerTimerId = (UINT8)(-1);
  
  BAC_TxBuff[0]=0x10;    // 
  BAC_TxBuff[1]=0x06;    // 
  bRetCode = SCIA_TransmitBuffer  (2,BAC_TxBuff);
  if (bRetCode == ERROR)
  {
    #if DEBUG_AT(3)
  		  (void)sprintf((char *)udp_buf,"BAC_TxErr: Transmitiendo..\n");
        udp_data_to_send = TRUE;	  
    #endif  		
    StatisticData.SerialStat.MsgTxErr++;
	  BAC_WaitAnswerTimerId =  TMR_SetTimer ( BAC_PollingPeriod , PROC_BAC, 0x00, FALSE);  
  }

  return; 
}


//-----------------------------------------------------------------------------
//--RxAtEnd se analiza el mensaje recibido como respuesta a un comando
//
void f_BACIdleRxMsgEnd (void) {
  UINT8   RxEndToken;
  void*   vpAuxBuffer;
  WORD    wCalcCrc;
  WORD    wValCrc;
  BYTE    bac_len;
  BYTE*   bac_buff;
  UINT8   bRetCode; 

  SETBIT(dwIntegrityMonitor,PROC_BAC);
  
  TMR_FreeTimer (BAC_WaitAnswerTimerId);
  
// Recuperar el puntero al buffer utilizado para recibir el mensaje
// El token del buffer está en la parte baja del parámetro del mensaje
  RxEndToken = (UINT8)CurrentMsg.Param;
  if (MemGetBufferPtr( RxEndToken , &vpAuxBuffer ) != OK ){    
  // ABR: control de errores
    return;   
  }
  bac_buff = (BYTE*)vpAuxBuffer;
#if DEBUG_AT(2)
		  (void)sprintf((char *)udp_buf,"BAC_MsgRx: %X, %X, %X, %X ,%X, %X, %X, %X, %X \n",
		                ((BYTE*)vpAuxBuffer)[0],  // 
		                ((BYTE*)vpAuxBuffer)[1],  // 
		                ((BYTE*)vpAuxBuffer)[10],  // 
		                ((BYTE*)vpAuxBuffer)[11],  // 
		                ((BYTE*)vpAuxBuffer)[12],  // 
		                ((BYTE*)vpAuxBuffer)[13],  // 
		                ((BYTE*)vpAuxBuffer)[14],  // 
		                ((BYTE*)vpAuxBuffer)[15],  // 
		                ((BYTE*)vpAuxBuffer)[16] ); 
      udp_data_to_send = TRUE;
#endif    

  wValCrc = CurrentMsg.Param & 0xFF00;    // Len del msg en parte alta del parametro
  wValCrc >>= 8;
  bac_len = (BYTE)wValCrc;
  
  if ( !bac_len )     // si hubo error de recepcion, llega len=0...
  {
      StatisticData.SerialStat.MsgRxErr++; 
      SeguimosVivos(0);
      
  } 
  else if(bac_len == 2) // Un Ack ?
	{
		  if((bac_buff[0]==DLE_CHAR) && (bac_buff[2]==ACK_CHAR)) // Es un ACK
			SeguimosVivos(1);
	}
	else if(bac_len < 14) // Mensaje trunco
	{	  
		TramaMal();	
	}
	else
	{
		// Vemos el CRC
		wCalcCrc=CalcCrc(bac_buff+2,bac_len-4);
		
		if ( *((WORD*)(&bac_buff[bac_len-2])) != wCalcCrc) // Crc ok ?
    {      
			TramaMal();
#if DEBUG_AT(3)
		  (void)sprintf((char *)udp_buf,"BAD_CrcRxMsg: CalcCrc=%X, ValCrc=%X\n",
		                wCalcCrc,
		                *((WORD*)(&bac_buff[bac_len-2])) );
      udp_data_to_send = TRUE;
#endif    
    }
		else			
   
		{
      StatisticData.SerialStat.MsgRxOk++;
#if DEBUG_AT(2)
		  (void)sprintf((char *)udp_buf,"RxMsg_OK: bac_len=%d\n",
		                bac_len);
      udp_data_to_send = TRUE;
#endif    
			/* El mensaje que llego esta OK, saco los DLEs repetidos*/
			ClearDLEsFromMsg(bac_buff,&bac_len);
			StoreData(bac_buff,bac_len);
       
			SeguimosVivos(1);
			
      if ( BAC_CtrlFlags & BAC_MASK_MIRR_FRAME) 
      {    
          bRetCode = SCIB_TransmitBuffer  (bac_len,(BYTE*)vpAuxBuffer);
          if (bRetCode == ERROR)
          {
          	BAC_WaitAnswerTimerId =  TMR_SetTimer ( BAC_PollingPeriod , PROC_BAC, 0x00, FALSE);  
          }
      }
      else        // Iniciar un timer para enviar comando..
      	  BAC_WaitAnswerTimerId =  TMR_SetTimer ( BAC_PollingPeriod , PROC_BAC, 0x00, FALSE);  

      return; 
			
		}
     
  }
  
  BAC_WaitAnswerTimerId =  TMR_SetTimer ( BAC_PollingPeriod , PROC_BAC, 0x00, FALSE);  
  return; 
}

//-----------------------------------------------------------------------------
//--TxAtEnd libera el buffer de transmisión cuando se terminó de enviar un msg
//
void f_BACIdleTxMsgEnd (void) {

  // Habilitar la recepción para recibir la respuesta al comando.
  SCIA_ReceiveFrame (MSG_MAX_BAC_LEN, BAC_RxMsgToken);
  
#if DEBUG_AT(1)
		  (void)sprintf((char *)udp_buf,"BAC_MsgTx: %X,%X\n",
		                BAC_TxBuff[0],
		                BAC_TxBuff[1]);	
      udp_data_to_send = TRUE;
#endif   
 
  StatisticData.SerialStat.MsgTxOk++;  

	BAC_WaitAnswerTimerId =  TMR_SetTimer ( BAC_PollingTimeout, PROC_BAC, 0x00, FALSE);  
//	BAC_WaitAnswerTimerId =  TMR_SetTimer ( TIMEOUT_WAIT_ANSWER , PROC_BAC, 0x00, FALSE);  
 
  return; 
  
}

//-----------------------------------------------------------------------------
//--TxAtEnd libera el buffer de transmisión cuando se terminó de enviar la copia de un msg
//
void f_BACIdleCpyTxMsgEnd (void) {

 
#if DEBUG_AT(3)
		  (void)sprintf((char *)udp_buf,"BAC_CpyMsgTx: %X,%X\n",
		                BAC_TxBuff[0],			 
		                BAC_TxBuff[1]);	
      udp_data_to_send = TRUE;
#endif   
 
 	BAC_WaitAnswerTimerId =  TMR_SetTimer ( BAC_PollingPeriod , PROC_BAC, 0x00, FALSE);  
 
  return; 
  
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Funciones Específicas de capa de red de protocolo DNP3 (BAC)
////////////////////////////////////////////////////////////////////////////////////////////////
//


/* Inidicamos que la RTU esta viva */
void SeguimosVivos(char vive)
{
	if(vive)
	  BAC_Diag.BitDiag.alive = 1;	
	else
	  BAC_Diag.BitDiag.alive = 0;	

}

/*No hay mas bancos de datos */
void NoHayMasBancos(void)
{
	  BAC_Diag.BitDiag.noBank = 1;	
}

void NoHayMasMemoria(void)
{
	  BAC_Diag.BitDiag.noMem = 1;	
}

/* Reporta una trama con errores */
void TramaMal()
{
  StatisticData.SerialStat.MsgRxErr++;
}


/* Saca los DLEs de un mensage */
void ClearDLEsFromMsg(BYTE* buff, BYTE* bytes)
{
	UINT8 j,k;
	UINT8 MaxIdxBytesToTest;

  MaxIdxBytesToTest = *bytes;

	for(j=0 ; j<(MaxIdxBytesToTest-4) ; j++)  // 4 = 1 por Idx[0], 1 porque el último y 2 por CRC
	{
			if((buff[j]==DLE_CHAR) && (buff[j+1]==DLE_CHAR)) /* no es DLE; es dato */
			{
  				for(k=j;k < (MaxIdxBytesToTest+2);k++)    // también desplazo el CRC
    					buff[k]=buff[k+1];
  				MaxIdxBytesToTest--;										  // uno menos porque ya eliminé el duplicado
			}
	}
	
	*bytes=MaxIdxBytesToTest;	
	
}


/* Guarda los datos en el array de lectura Modbus */
void StoreData(BYTE* data, BYTE len)
{
	UINT8   bIdx;		  
	UINT16  wLastOffsetAlloc;
	UINT8   bLastSizeAlloc;
	UINT16  wEEIndexAddr;
	
	//-------------------------------------------------------------------------------------
	// Veo que el dato sea el correspondiente a un banco
	for(bIdx=0 ; bIdx<MAX_BAC_BANKS ; bIdx++) 
	{
  		// Si la direccion del banco de datos coincide
  		if(BAC_BankMemManager[bIdx].BacNodeAddress == *(WORD*)(data+10)) 
  		{
  		    if (!bIdx) 
  		      BAC_CtrlFlags |= BAC_MASK_INIT_SEQUENCE; // Comienza nueva secuencia
  		    
  		    if (BAC_BankMemManager[bIdx].BacNodeAddress == BAC_DOMAIN_ADDRESS) 
  		    {
  		      if (BAC_CtrlFlags & BAC_MASK_INIT_SEQUENCE) 
  		      {
      		      BAC_CtrlFlags &= ~BAC_MASK_INIT_SEQUENCE;   // ya encontré el primero
  		      } 
  		      else{
       		      BAC_CtrlFlags |= BAC_MASK_INIT_SEQUENCE; // Comienza nueva secuencia
  		          continue;   // es el segundo, seguir buscando coincidencias o asignar nuevo idx   
 		      }
  		      
  		    }
  		    // Si viene el mismo banco con una longitud mayor que la que tiene reservada
     	    if (BAC_BankMemManager[bIdx].BankSize < (len-10-4)) 
     	    {
#if DEBUG_AT(3)
        		  (void)sprintf((char *)udp_buf,"BAC_BadLen: Addr=%X, Len=%d, WaitLen=%d, Mod=%d\n",
        		                BAC_BankMemManager[bIdx].BacNodeAddress,
        		                (len-10-4),
        		                BAC_BankMemManager[bIdx].BankSize,
        		                BAC_BankMemManager[bIdx].BankModbusAddress);
              udp_data_to_send = TRUE;
#endif        			     	      
         			TramaMal();
        			return;
     	    }


    			// Limpio el buffer del banco correspondiente
    			(void)memset(&bac_data[BAC_BankMemManager[bIdx].BankOffsetStart], 0x00, BAC_BankMemManager[bIdx].BankSize);
    			// Copio los datos menos el CRC, los caracteres de final y los 10 del principio
    			(void)memcpy(&bac_data[BAC_BankMemManager[bIdx].BankOffsetStart], (data+10), (len-10-4)); 
    			return;
  		}
	}
	
	
	//-------------------------------------------------------------------------------------
	// No encontre el banco de datos correspondiente. Busco uno vacio
	wLastOffsetAlloc  = 0x00;
  bLastSizeAlloc    = 0x00;
//  wLastModbusAlloc  = 0x00;
	for(bIdx=0 ; bIdx<MAX_BAC_BANKS ; bIdx++) // Busco un banco disponible
	{
  		if( !BAC_BankMemManager[bIdx].BankStatus) // Banco disponible
  		{		
  		  		    
  		    if ((wLastOffsetAlloc + bLastSizeAlloc + (len-10-4+BAC_EXTRA_LEN)) > MAX_BAC_BUF){  
  		        // No hay espacio.!!.. Inidicar condición de error..  
#if DEBUG_AT(3)
        		  (void)sprintf((char *)udp_buf,"BAC_Msg: No hay memoria suficiente\n");
              udp_data_to_send = TRUE;
#endif    
              NoHayMasMemoria();
  		        return;
  		    }
    		  BAC_BankMemManager[bIdx].BacNodeAddress     = *(WORD*)(data+10);
    	    BAC_BankMemManager[bIdx].BankOffsetStart    = wLastOffsetAlloc + bLastSizeAlloc;
//    	    BAC_BankMemManager[bIdx].BankModbusAddress  = wLastModbusAlloc + 1000;
    	    BAC_BankMemManager[bIdx].BankSize           = (len-10-4+BAC_EXTRA_LEN); //
    	    BAC_BankMemManager[bIdx].BankStatus         = 0x01;         // Ocupado

          wEEIndexAddr = sizeof(BAC_BANK_ALLOC)*bIdx;
          wEEIndexAddr += EE_ADDR_BAC_BANKS;
          eeprom_write_buffer(wEEIndexAddr, 
         	                  (UINT8 *)&(BAC_BankMemManager[bIdx]), 
         	                  sizeof(BAC_BANK_ALLOC)); 
    		       		      		  
    			// Copio los datos menos el CRC, los caracteres de final y los 10 del principio
    			(void)memcpy(&bac_data[BAC_BankMemManager[bIdx].BankOffsetStart], (data+10), (len-10-4)); 
#if DEBUG_AT(3)
    		  (void)sprintf((char *)udp_buf,"BAC_NewBank: Addr=%X, Off=%d, Mod=%d, Len=%d\n",
    		                BAC_BankMemManager[bIdx].BacNodeAddress,
    		                BAC_BankMemManager[bIdx].BankOffsetStart,
    		                BAC_BankMemManager[bIdx].BankModbusAddress,
    		                BAC_BankMemManager[bIdx].BankSize);
          udp_data_to_send = TRUE;
#endif        			
    			return;
  		}
  		wLastOffsetAlloc  = BAC_BankMemManager[bIdx].BankOffsetStart;
  		bLastSizeAlloc    = BAC_BankMemManager[bIdx].BankSize;		
//  		wLastModbusAlloc  = BAC_BankMemManager[bIdx].BankModbusAddress;		
	}
	
	// Si llego hasta aca es porque no hay mas bancos disponibles
	NoHayMasBancos(); // reporto
#if DEBUG_AT(3)
        		  (void)sprintf((char *)udp_buf,"BAC_Msg: No hay mas bancos disponibles\n");
              udp_data_to_send = TRUE;
#endif    
	
	return ;

}



////////////////////////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
////////////////////////////////////////////////////////////////////////////////////////////////
//

#define DNP_REMOTE_ADDR     (WORD)1       //11
#define DNP_LOCAL_ADDR      (WORD)100
//--------------------------------------------------------------------------------------
//-- Funcion de Inicialización
//
void BAC_Init (void)
{
// Las direcciones son WORD, que ya están grabada en EEPROM con formato LSB:MSB
//  eeprom_read_buffer(EE_ADDR_DNP_SLAVE, (UINT8 *)&BAC_wRemoteAddr, 2); 
//  eeprom_read_buffer(EE_ADDR_DNP_MASTER, (UINT8 *)&BAC_wLocalAddr, 2);		 
  UINT8*  pTmpRxBuffer;
  UINT8   bTmpConter;    

  BAC_Diag.WordDiag = 0;	
  BAC_CtrlFlags = 0x00;

  eeprom_read_buffer(EE_ADDR_BAC_STATE, (UINT8 *)&bTmpConter, 1);	
  if (bTmpConter == BAC_BANKS_INI) 
  {
      eeprom_read_buffer(EE_ADDR_BAC_BANKS, 
     	                  (UINT8 *)BAC_BankMemManager, 
     	                  sizeof(BAC_BANK_ALLOC)*MAX_BAC_BANKS);         
  } 
  else
  {
      BAC_ResetBanks();    	
  }
	
  // Se utiliza NODE_SCIB para habilitar reenvío de mensajes por puerto 2
 	eeprom_read_buffer(EE_ADDR_NODE_SCIB, &BAC_EnablePolling,1);           
	if (BAC_EnablePolling) 
	  BAC_CtrlFlags |= BAC_MASK_MIRR_FRAME; 
	else
	  BAC_CtrlFlags &= ~BAC_MASK_MIRR_FRAME; 
	
  eeprom_read_buffer(EE_ADDR_INV_POLL, 
 	                  (UINT8 *)&BAC_PollingPeriod,2); 

  eeprom_read_buffer(EE_ADDR_INV_POLL_B, 
 	                  (UINT8 *)&BAC_PollingTimeout,2); 

 	eeprom_read_buffer(EE_ADDR_NODE_SCIA, &BAC_EnablePolling,1);           
      
	if (BAC_EnablePolling) 
	{
	  BAC_CtrlFlags |= BAC_MASK_POL_ENABLE; 
	  
	  SCIA_Init();            // Driver de comunicación serie (SCI1) 
    // Iniciar un timer para enviar comando..
  	BAC_WaitAnswerTimerId =  TMR_SetTimer ( TMR_BAC_RESETLINKWAIT , PROC_BAC, 0x00, FALSE);  
  	//Reservar un buffer para la recepción de mensajes
    (void)MemGetBuffer ( (void**)(&pTmpRxBuffer), &BAC_RxMsgToken); 
    
    // ABR: chequear bit de config...
    if ( BAC_CtrlFlags & BAC_MASK_MIRR_FRAME) 
  	    SCIB_Init();            // Driver de comunicación serie (SCI1)     
	}
	else
	  BAC_CtrlFlags &= ~BAC_MASK_POL_ENABLE; 
	
   
}


//--------------------------------------------------------------------------------------
//-- Funcion de Reinicialización en caso de fallas 
//
void BAC_ReInit (void)
{
  UINT8*  pTmpRxBuffer;
  
  TMR_FreeTimer (BAC_WaitAnswerTimerId);
	CurrentProcessState [PROC_BAC] = (EVENT_FUNCTION*)BACProc_IDLE;	

  // Temporariamente para resetear estructura de bancos almacenada en EEPROM
  // Ahora se utiliza para habilitar reenvío de mensajes por puerto 2
 	eeprom_read_buffer(EE_ADDR_NODE_SCIB, &BAC_EnablePolling,1);           
	if (BAC_EnablePolling) 
	  BAC_CtrlFlags |= BAC_MASK_MIRR_FRAME; 
	else
	  BAC_CtrlFlags &= ~BAC_MASK_MIRR_FRAME; 
	
  // Frecuencia de Polling 
  eeprom_read_buffer(EE_ADDR_INV_POLL, 
 	                  (UINT8 *)&BAC_PollingPeriod,2); 
	// Para activar/desactivar polling..
 	eeprom_read_buffer(EE_ADDR_NODE_SCIA, &BAC_EnablePolling,1);           

	if (BAC_EnablePolling) 
	{	  
	  BAC_CtrlFlags |= BAC_MASK_POL_ENABLE; 
	  
    SCIA_ReInit();            // Driver de comunicación serie (SCI1) 
    // Iniciar un timer para enviar comando..
  	BAC_WaitAnswerTimerId =  TMR_SetTimer ( TMR_BAC_RESETLINKWAIT , PROC_BAC, 0x00, FALSE);  
    // Liberar buffer anterior.
    (void)MemFreeBuffer (BAC_RxMsgToken);  	 
  	//Reservar un buffer para la recepción de mensajes
    (void)MemGetBuffer ( (void**)(&pTmpRxBuffer), &BAC_RxMsgToken); 
    
    if ( BAC_CtrlFlags & BAC_MASK_MIRR_FRAME) 
  	    SCIB_ReInit();            // Driver de comunicación serie (SCI1) 
	}	  
	else
	  BAC_CtrlFlags &= ~BAC_MASK_POL_ENABLE; 
	
}


void BAC_ResetBanks(void) 
{
  UINT8   bTmpConter;

      // inicializar indices de administrador de bancos	
    	for(bTmpConter=0 ; bTmpConter<MAX_BAC_BANKS ; bTmpConter++)
    	{	  	  
    	    BAC_BankMemManager[bTmpConter].BacNodeAddress     = 0xFFFF;
    	    BAC_BankMemManager[bTmpConter].BankOffsetStart    = 0x0000;
    	    BAC_BankMemManager[bTmpConter].BankModbusAddress  = (bTmpConter+1)*1000;
    	    BAC_BankMemManager[bTmpConter].BankSize           = 0x00;
    	    BAC_BankMemManager[bTmpConter].BankStatus         = 0x00;         // libre
    	}
    	
      eeprom_write_buffer(EE_ADDR_BAC_BANKS, 
     	                  (UINT8 *)BAC_BankMemManager, 
     	                  sizeof(BAC_BANK_ALLOC)*MAX_BAC_BANKS); 
     	bTmpConter = BAC_BANKS_INI;
      eeprom_write_buffer(EE_ADDR_BAC_STATE, (UINT8 *)&bTmpConter, 1);	     	                         
  
      MON_ResetBoard(); // REsetear para comenzar un ciclo de encuestas desde el principio.
}

#pragma CODE_SEG DEFAULT



//--------------------------------------------BUFFER_GARBAGE_INI------------------------------------------


//---------------------------------------------------------------------------
//--Cambia el orden de los bytes de una WORD (entre little endian y big endian)
//
/*
void BAC_ChgWordEndian ( UINT16* DataWord)
{  
  UINT16 TmpEndian;
  
  TmpEndian = (*DataWord & 0xFF00)>>8;
  *DataWord = (*DataWord & 0x00FF)<<8;
  *DataWord |= TmpEndian;
  
}

#define REVPOLY 0x0811    // Polinomio generador para desplazamiento hacia la derecha
//-----------------------------------------------------------------------------
// Calcular CRC
//-----------------------------------------------------------------------------
//
//  The function returns the CRC as a unsigned short type 
//  puchMsg   =  message to calculate CRC upon
//  usDataLen =  quantity of bytes in message


UINT16 BAC_BuildCRC16 ( UINT8 * puchMsg, UINT16 usDataLen )
{
  UINT16 CRC_Outcome;
  UINT8  TmpByte;
  UINT8  TmpIdx;
  UINT16 TmpWord;
  
//  CRC_Outcome = 0xFFFF;
  CRC_Outcome = 0x00;
  
  while (usDataLen--){                          // pass through message buffer
    TmpByte = (*puchMsg);
    puchMsg++;
    for (TmpIdx = 0; TmpIdx < 8; TmpIdx++){
      TmpWord = (CRC_Outcome ^ TmpByte) & 1;
      CRC_Outcome >>= 1;
      TmpByte >>= 1;
      if (TmpWord)
        CRC_Outcome ^= REVPOLY;
    }
  }
  
  TmpByte = (CRC_Outcome & 0xFF00)>>8;
  CRC_Outcome <<= 8;
  CRC_Outcome |= TmpByte;
  
  return CRC_Outcome;  
}
*/
//--------------------------------------------BUFFER_GARBAGE_END------------------------------------------


