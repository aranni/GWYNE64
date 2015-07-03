#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "MC9S12NE64.h"
#include "datatypes.h"

#include "Commdef.h"
#include "dSCIA.h"
#include "global.h"

#pragma CODE_SEG NON_BANKED        

////////////////////////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_SCIA
////////////////////////////////////////////////////////////////////////////////////////////////
//
const EVENT_FUNCTION	SCIAProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_SCIAIdleTimeout },
{EVT_SCI_TXCMD        , f_SCIASendCommand	},
{EVT_SCI_TXREQ 	      , f_SCIASendRequest	},
{EVT_SCI_RXREQ 	      , f_SCIAReceiveMsg 	},
{EVT_SCI_TXMSG 	      , f_SCIAIdleFrameTx	},
{EVT_SCI_RXMSG 	      , f_SCIAIdleFrameRx },
{EVT_OTHER_MSG     		, f_Consume         }};

const EVENT_FUNCTION	SCIAProc_TOTRANSMIT [] =
{
{EVT_TMR_TIMEOUT 	    , f_SCIAToTxTimeout	},
{EVT_SCI_TXREQ 	      , f_SCIATxNotReady  },
{EVT_OTHER_MSG     		, f_Consume         }};

const EVENT_FUNCTION	SCIAProc_TRANSMITING [] =
{
{EVT_TMR_TIMEOUT 	    , f_SCIATxTimeout	  },
{EVT_SCI_TXMSG 	      , f_SCIATxFrame	    },
{EVT_SCI_TXREQ 	      , f_SCIATxNotReady  },
{EVT_OTHER_MSG     		, f_Consume         }};

const EVENT_FUNCTION	SCIAProc_RECEIVING [] =
{
{EVT_TMR_TIMEOUT 	    , f_SCIARxTimeout	  },
{EVT_SCI_RXMSG 	      , f_SCIARxFrame	    },
{EVT_SCI_TXREQ 	      , f_SCIATxNotReady  },
{EVT_OTHER_MSG     		, f_Consume         }};


////////////////////////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_SCIA
////////////////////////////////////////////////////////////////////////////////////////////////
//
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//-------------- estado : IDLE ---------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//
void f_SCIAIdleTimeout (void) 
{
/*UINT8*  pTmpRxBuffer;
  UINT8   bTmpToken;    
  (void)MemFreeBuffer (SCIA_CurrentRxToken);  
  (void)MemGetBuffer ( (void**)(&pTmpRxBuffer), &bTmpToken);  
  SCIA_ReceiveFrame (3, SCIA_CurrentRxToken);	 */  
  
  SCIA_RX_TimerID = 0xFF;
  // Informar error en recepcion
  PutEventMsg (EVT_SCI_RXERR, SCIA_TargetProc, PROC_SCIA, SCIA_CurrentRxToken); 
  StatisticData.SerialStat.MsgRxErr++;
  return; 
  
}


/********************************************************************************************	 
 *  Atención al requerimiento de envío de mensaje. 																					*
 *  Responde a los mensajes:																																*
 *  - EVT_SCI_TXREQ : transmitir mensaje sin esperar respuesta.															*
 *  - EVT_SCI_TXCMD : transmitir comando y esperar una respuesta (via f_SCIASendCommand)		*
 *	Parámetros del mensaje:																																	*
 *	- LOW BYTE: Token del buffer de transmisión (TxBuffer)																	*
 *  - HIGH BYTE: offset en el TxBuffer desde donde comenzar a transmitir										*
 *  - La longitud a transmitir está en el parámetro wUsedLen asociado al buffer de memoria. *
 *  En modo RS485 pasa al estado TOTRANSMIT para esperar el establecimiento de la linea     *
 ********************************************************************************************/
void f_SCIASendCommand (void)
{
    SCIA_StateFlags |= FLG_TX_COMMAND;
    f_SCIASendRequest();
  	return;
}

void f_SCIASendRequest (void)
{
    // Si hay transmisión en curso rechazar el requerimiento.
    if ( SCIA_TxByteCount ) {
      PutEventMsg (EVT_SCI_TXERR, SCIA_TargetProc, PROC_SCIA, SCIA_CurrentTxToken);
      return;                     
    }
    
    if (SCIA_ConfigByte & CFG_SCI_PHYMODE_MASK){            // modo RS485
      RS485_A_RE_ = 1; 				                              // TX Mode
      RS485_A_DE  = 1;				                              // TX Mode
    }
    LED_GREEN_ON;
   
    // Offset desde donde transmitir, en la parte alta del parámetro
    SCIA_TxOffset = (UINT8)((CurrentMsg.Param)>>8);
    
    // El token del buffer está en la parte baja del parámetro del mensaje
    SCIA_CurrentTxToken = (UINT8)CurrentMsg.Param;
    if ( MemGetBufferPtr( SCIA_CurrentTxToken, (void**) &(SCIA_FrameToTx)) != OK ){    
      // ABR: control de errores
      #if DEBUG_AT(DBG_LEVEL_2)
        if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_2)
        {                    
      	  (void)sprintf((char *)udp_buf,"SCIA Err:0x0A \n");
          udp_data_to_send = TRUE;
        }
      #endif  	
      PutEventMsg (EVT_SCI_TXERR, SCIA_TargetProc, PROC_SCIA, SCIA_CurrentTxToken);
      return;   
    }

    if ( MemGetUsedLen(SCIA_CurrentTxToken, &SCIA_TxByteCount) != OK)     // Bytes a transmitir
    {  
      //MON_ResetIfNotConnected(0x11);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      #if DEBUG_AT(DBG_LEVEL_2)
        if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_2)
        {                    
      	  (void)sprintf((char *)udp_buf,"SCIA Err:0x0B \n");
          udp_data_to_send = TRUE;
        }
      #endif  	
      PutEventMsg (EVT_SCI_TXERR, SCIA_TargetProc, PROC_SCIA, SCIA_CurrentTxToken);
      return;
      
    }
    
    if (SCIA_ConfigByte & CFG_SCI_PHYMODE_MASK)                           // modo RS485
    {         
        // ABR_2012-03-22: Revisar valor de timer de espera para transmitir, parametrizarlo dinamicamente   
      	SCIA_TX_TimerID =  TMR_SetTimer (SCIA_TMR_DRIVE_TIMEOUT, PROC_SCIA, 0x00, FALSE);															 
        FirstEventInCurrentState = (EVENT_FUNCTION*)SCIAProc_TOTRANSMIT;  // Esperar establecimiento de TX
    } 
    else
    {
    
        SCIA_TmpVar  = SCIA_SCIDRL;	                  // Flush buffer
        SCIA_TmpVar  = SCIA_SCISR1;	                  // Clear pending flag
    
        SCIA_FrameToTx += SCIA_TxOffset;
        SCIA_SCIDRL = *SCIA_FrameToTx;
//  ABR: 30-NOV-2010 TIP: revisar por qué se comporta distinto bajo algunas condiciones..
//  En SPA no se transmite este primer byte ?, entonces se comenta la siguiente linea
        SCIA_FrameToTx++;
        SCIA_TxByteCount--; 
        SCIA_SCICR2 |=  SCI1CR2_TCIE_MASK;            // TxC Interrupt Enable
        SCIA_SCICR2 |=  SCI1CR2_TE_MASK ; 				    // Tx Enable
        // SCIA_SCICR2 |=  SCI1CR2_SCTIE_MASK;	      // Tx Interrupt Enable
    }
  	return;
}



/********************************************************************************************	 
 *  Atención al requerimiento de recepción de mensaje.  																		*
 *  Responde a los mensajes:																																*
 *  - EVT_SCI_RXREQ : recibir mensaje por puerto serie.															        * 
 *	Parámetros del mensaje:																																	*
 *	- LOW BYTE: Token del buffer de recepción (RxBuffer)  																	*
 *  - HIGH BYTE: offset en el RxBuffer desde donde comenzar a escribir la respuesta					*
 *  - La longitud a transmitir está en el parámetro wUsedLen asociado al buffer de memoria. *
 *  En modo RS485 pasa al estado RECEIVING e inicia temporización                           *
 ********************************************************************************************/
void f_SCIAReceiveMsg (void)
{

  // Si hay recepción en curso rechazar el requerimiento.
  if ( SCIA_RxByteCount ) {
    PutEventMsg (EVT_SCI_RXERR, SCIA_TargetProc, PROC_SCIA, SCIA_CurrentTxToken);
    return;                     
  }

  if (SCIA_ConfigByte & CFG_SCI_PHYMODE_MASK){            // modo RS485
    RS485_A_RE_ = 0; 				    // RX Mode
    RS485_A_DE  = 0;				    // RX Mode
  }
 
  // Offset donde escribir la respuesta, en la parte alta del parámetro
  SCIA_RxOffset = (UINT8)((CurrentMsg.Param)>>8);
  // El token del buffer está en la parte baja del parámetro del mensaje
  SCIA_CurrentRxToken = (UINT8)CurrentMsg.Param;
  (void)MemGetBufferPtr ( SCIA_CurrentRxToken, (void**) &(SCIA_FrameToRx));
  SCIA_FrameToRx += SCIA_RxOffset;
  
  if ( MemGetUsedLen(SCIA_CurrentRxToken, &SCIA_RxByteCount) != OK)   // Bytes a recibir
        MON_ResetIfNotConnected(0x12);    // Error de SW, Reset (TODO: revisar acciones a tomar)

	SCIA_RxByteIdx = 0x00;                        // Indice contador de Bytes recibidos

  SCIA_TmpVar  = SCIA_SCISR1;	                  // Clear pending flag
  SCIA_TmpVar  = SCIA_SCIDRL;	                  // Flush buffer
  SCIA_SCICR2 |=  SCI1CR2_RE_MASK ;			        // Rx Enable
  SCIA_SCICR2 |=	SCI1CR2_RIE_MASK;		          // Rx Interrupt Enable
	
  if (SCIA_ConfigByte & CFG_SCI_PHYMODE_MASK)	   // modo RS485
  {          
  	SCIA_RX_TimerID =  TMR_SetTimer ( SCIA_TMR_RX_TIMEOUT , PROC_SCIA, 0x00, FALSE);
    FirstEventInCurrentState = (EVENT_FUNCTION*)SCIAProc_RECEIVING; 
  } 
	return;
}


/********************************************************************************************	 
 *  Atención al evento de mensaje transmitido. (para modo RS232, siempre esta en IDLE)      *
 *  Responde a los mensajes:																																*
 *  - EVT_SCI_TXMSG : Mensaje transmitido OK.                 															*
 *	Se envía evento EVT_SCI_TXMSG al proceso que requirió la transmisión del mensaje con el *
 *	token del TxBuffer como parámetro.                                                  		*
 *  En caso de haber sido una transmisión de comando (EVT_SCI_TXCMD) además se realizan los *
 *  preparativos para recibir la respuesta en el mismo buffer, con el mismo offset y len    *
 ********************************************************************************************/
void f_SCIAIdleFrameTx (void)
{

    LED_GREEN_OFF;
    LED_RED_ON;
    StatisticData.SerialStat.MsgTxOk++;
  
    if ( SCIA_StateFlags & FLG_TX_COMMAND )   // comando transmitido, entonces esperar respuesta
    {
        SCIA_StateFlags &= ~FLG_TX_COMMAND;
      	SCIA_RxOffset = SCIA_TxOffset;          // Tomar el mismo offset para escribir la respuesta
      	
        // Recuperar puntero al buffer de transmisión que se reutilizará para recepción 
        SCIA_CurrentRxToken = SCIA_CurrentTxToken;  
        if ( MemGetBufferPtr( SCIA_CurrentRxToken , (void**)&(SCIA_FrameToRx)) != OK ){    
          // ABR: control de errores
          PutEventMsg (EVT_SCI_TXERR, SCIA_TargetProc, PROC_SCIA, SCIA_CurrentTxToken);
          return;   
        }
        // Preparar para leer la misma cantidad de bytes que se envió en el comando
        if ( MemGetUsedLen(SCIA_CurrentRxToken, &SCIA_RxByteCount) != OK)   // Bytes a recibir
          MON_ResetIfNotConnected(0x14);    // Error de SW, Reset (TODO: revisar acciones a tomar)
        // Preparar para leer la maxima cantidad de bytes que soporta el buffer
        //SCIA_RxByteCount = (MEM_MAX_BUF_LEN - SCIA_RxOffset);
        
      	SCIA_RxByteIdx = 0x00;                      // Indice contador de Bytes recibidos
        
        SCIA_FrameToRx += SCIA_RxOffset;
        m_RxMsgLen = 0x00;
        SCIA_TmpVar  = SCIA_SCISR1;	          // Clear pending flag
        SCIA_TmpVar  = SCIA_SCIDRL;	          // Flush buffer
        SCIA_SCICR2 |=  SCI1CR2_RE_MASK ;			// Rx Enable
        SCIA_SCICR2 |=	SCI1CR2_RIE_MASK;			// Rx Interrupt Enable
        
      	// Si es espera de respuesta a comando, el timer se inicializa aquí aún para RS232
      	SCIA_RX_TimerID =  TMR_SetTimer ( SCIA_TMR_RX_TIMEOUT , PROC_SCIA, 0x00, FALSE);
        
    } 
    
    PutEventMsg (EVT_SCI_TXMSG, SCIA_TargetProc, PROC_SCIA, SCIA_CurrentTxToken );
  
}

/********************************************************************************************	 
 *  Atención al evento de mensaje recibido. (para modo RS232, siempre esta en IDLE)         *
 *  Responde a los mensajes:																																*
 *  - EVT_SCI_RXMSG : Mensaje recibido OK.                     															*
 *	Se envía evento EVT_SCI_RXMSG al proceso que requirió la recepción del mensaje con el   *
 *	token del RxBuffer como parámetro.  																										*
 *  La longitud recibida se graba en el parámetro wUsedLen asociado al buffer de memoria    *
 ********************************************************************************************/
void f_SCIAIdleFrameRx (void)
{
  TMR_FreeTimer (SCIA_RX_TimerID);
  SCIA_RX_TimerID = 0xFF;

  LED_RED_OFF;
  SCIA_RxErrAcc = 0x00;
  if ( MemSetUsedLen(SCIA_CurrentRxToken, m_RxMsgLen) != OK)        // Bytes recibidos
  {  
      #if DEBUG_AT(DBG_LEVEL_2)
        if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_2)
        {                    
      	  (void)sprintf((char *)udp_buf,"SCIA Err:0x02 \n");
          udp_data_to_send = TRUE;
        }
    #endif  	
    PutEventMsg (EVT_SCI_RXERR, SCIA_TargetProc, PROC_SCIA, SCIA_CurrentRxToken); 
    StatisticData.SerialStat.MsgRxErr++;
    return;
    
  }
  
  if ((UINT8)CurrentMsg.Param) {
      StatisticData.SerialStat.MsgRxErr++;
  } 
  else
  {
      PutEventMsg (EVT_SCI_RXMSG, SCIA_TargetProc, PROC_SCIA, SCIA_CurrentRxToken);
      StatisticData.SerialStat.MsgRxOk++;
  }
}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//---------- estado : TOTRANSMIT  ------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//
void f_SCIAToTxTimeout (void)
{
    SCIA_TX_TimerID = 0xFF;

    SCIA_TmpVar  = SCIA_SCIDRL;	                  // Flush buffer
    SCIA_TmpVar  = SCIA_SCISR1;	                  // Clear pending flag
  
    SCIA_FrameToTx += SCIA_TxOffset;
    SCIA_SCIDRL = *SCIA_FrameToTx;
    SCIA_TxByteCount--;      
    SCIA_FrameToTx++;

    // SCIA_SCICR2 |=  SCI1CR2_SCTIE_MASK;	      // Tx Interrupt Enable
    SCIA_SCICR2 |=  SCI1CR2_TCIE_MASK;            // TxC Interrupt Enable
    SCIA_SCICR2 |=  SCI1CR2_TE_MASK ; 				    // Tx Enable

   	SCIA_TX_TimerID =  TMR_SetTimer ( SCIA_TMR_TX_TIMEOUT , PROC_SCIA, 0x00, FALSE);
    FirstEventInCurrentState = (EVENT_FUNCTION*)SCIAProc_TRANSMITING; 
}


//-----------------------------------------------------------------------------------------
// Se recibió un requerimiento de transmisión pero no se puede realizar en el estado actual
// -> RESPONDER CON UNA EXCEPCIÓN  
//
void f_SCIATxNotReady (void)
{
  // Informar con una excepción la imposibilidad de transmitir mensaje.
  PutEventMsg (EVT_SCI_TXBUSY, SCIA_TargetProc, PROC_SCIA, CurrentMsg.Param);
  
  return;
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//---------- estado : TRANSMITTING  ----------------------------------------------------------
//--------------------------------------------------------------------------------------------
//
void f_SCIATxTimeout (void)								
{
    SCIA_TX_TimerID = 0xFF;
    
    // Dejar en modo recepción..
    RS485_A_DE  = 0;				    // RX Mode
    RS485_A_RE_ = 0; 				    // RX Mode  
    
    LED_GREEN_OFF;

    // SCIA_SCICR2 &= ~(SCI1CR2_SCTIE_MASK);	    // Tx Interrupt Disable
    SCIA_SCICR2 &= ~(SCI1CR2_TE_MASK ); 			    // Tx Disable
    SCIA_SCICR2 &= ~(SCI1CR2_TCIE_MASK);          // TxC Interrupt Disable      
    SCIA_TxByteCount = 0x00;   
    
#ifdef MON_SERIAL_ERR_RESET_ENABLE
    // Acciones correctivas.. errores de transmision acumulados
    if ( ++SCIA_TxErrAcc >= MAX_SERIAL_TXERR){
        MON_ResetIfNotConnected(0x13);
    }
#endif
    
    PutEventMsg (EVT_SCI_TXERR, SCIA_TargetProc, PROC_SCIA, SCIA_CurrentTxToken);
    StatisticData.SerialStat.MsgTxErr++;
    FirstEventInCurrentState = (EVENT_FUNCTION*)SCIAProc_IDLE;  // Volver a estado IDLE
    
}


void f_SCIATxFrame (void)
{
    TMR_FreeTimer (SCIA_TX_TimerID);
    SCIA_TX_TimerID = 0xFF;

    // Pasar a modo recepción..
    RS485_A_DE  = 0;				    // RX Mode
    RS485_A_RE_ = 0; 				    // RX Mode  

    LED_GREEN_OFF;
    LED_RED_ON;
  
    if ( SCIA_StateFlags & FLG_TX_COMMAND )   // comando transmitido, entonces esperar respuesta
    {
        SCIA_StateFlags &= ~FLG_TX_COMMAND;
      	SCIA_RxOffset = SCIA_TxOffset;          // Tomar el mismo offset para escribir la respuesta
      	
        // Recuperar puntero al buffer de transmisión que se reutilizará para recepción 
        SCIA_CurrentRxToken = SCIA_CurrentTxToken;  
        if ( MemGetBufferPtr( SCIA_CurrentRxToken , (void**)&(SCIA_FrameToRx)) != OK ){    
          // ABR: control de errores
          PutEventMsg (EVT_SCI_TXERR, SCIA_TargetProc, PROC_SCIA, SCIA_CurrentTxToken);
          
          StatisticData.SerialStat.MsgTxErr++;
          //Volver a IDLE
          FirstEventInCurrentState = (EVENT_FUNCTION*)SCIAProc_IDLE; 
          return;   
        }
        // Preparar para leer la misma cantidad de bytes que se envió en el comando
        if ( MemGetUsedLen(SCIA_CurrentRxToken, &SCIA_RxByteCount) != OK)   // Bytes a recibir
          MON_ResetIfNotConnected(0x14);    // Error de SW, Reset (TODO: revisar acciones a tomar)
  
      	SCIA_RxByteIdx = 0x00;                      // Indice contador de Bytes recibidos
        
        SCIA_FrameToRx += SCIA_RxOffset;
        m_RxMsgLen = 0x00;
        SCIA_TmpVar  = SCIA_SCISR1;	          // Clear pending flag
        SCIA_TmpVar  = SCIA_SCIDRL;	          // Flush buffer
        SCIA_SCICR2 |=  SCI1CR2_RE_MASK ;			// Rx Enable
        SCIA_SCICR2 |=	SCI1CR2_RIE_MASK;			// Rx Interrupt Enable
      		
      	SCIA_RX_TimerID =  TMR_SetTimer ( SCIA_TMR_RX_TIMEOUT , PROC_SCIA, 0x00, FALSE);
        FirstEventInCurrentState = (EVENT_FUNCTION*)SCIAProc_RECEIVING; 
    } 
    else
    {
        FirstEventInCurrentState = (EVENT_FUNCTION*)SCIAProc_IDLE; 
    }
    
    StatisticData.SerialStat.MsgTxOk++;
    PutEventMsg (EVT_SCI_TXMSG, SCIA_TargetProc, PROC_SCIA, SCIA_CurrentTxToken );
    
}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//---------- estado : RECEIVING  ------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//
void f_SCIARxTimeout (void)
{
  
  SCIA_RX_TimerID = 0xFF;
  
//  RS485_A_RE_ = 1; 				    // TX Mode
//  RS485_A_DE  = 1;				    // TX Mode

  LED_RED_OFF;
  
  SCIA_TmpVar  = SCIA_SCISR1;	              // Clear pending flag
  SCIA_TmpVar  = SCIA_SCIDRL;	              // Flush buffer
  SCIA_SCICR2  &= ~(SCI1CR2_RE_MASK) ;			// Rx Disable
  SCIA_SCICR2  &= ~(SCI1CR2_RIE_MASK);			// Rx Interrupt Disable
  SCIA_RxByteCount = 0;   
  
// Acciones correctivas.. error de recepcion
  if (  ++SCIA_RxErrAcc >= MAX_SERIAL_RXERR){
#ifdef MON_SERIAL_ERR_RESET_ENABLE
        MON_ResetIfNotConnected(0x15);
#endif																				
  }

  // Informar error en recepcion
  PutEventMsg (EVT_SCI_RXERR, SCIA_TargetProc, PROC_SCIA, SCIA_CurrentRxToken); 
  StatisticData.SerialStat.MsgRxErr++;
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIAProc_IDLE;  // volver a estado IDLE

}

void f_SCIARxFrame (void)
{

//  RS485_A_RE_ = 1; 				    // TX Mode
//  RS485_A_DE  = 1;				    // TX Mode

  LED_RED_OFF;
  
  TMR_FreeTimer (SCIA_RX_TimerID);
  SCIA_RX_TimerID = 0xFF;
  SCIA_RxErrAcc = 0x00;

  if ( MemSetUsedLen(SCIA_CurrentRxToken, m_RxMsgLen) != OK)        // Bytes recibidos
  {  
      #if DEBUG_AT(DBG_LEVEL_2)
        if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_2)
        {                    
      	  (void)sprintf((char *)udp_buf,"SCIA Err:0x02 \n");
          udp_data_to_send = TRUE;
        }
    #endif  	
    PutEventMsg (EVT_SCI_RXERR, SCIA_TargetProc, PROC_SCIA, SCIA_CurrentRxToken); 
    StatisticData.SerialStat.MsgRxErr++;
    FirstEventInCurrentState = (EVENT_FUNCTION*)SCIAProc_IDLE;  // volver a estado IDLE
    return;    
  }
  
      #if DEBUG_AT(DBG_LEVEL_4)
        if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_4)
        {                    
          SCIA_FrameToRx -= m_RxMsgLen;
      	  (void)sprintf((char *)udp_buf,"SCIA Rx: %d, %d, %d, %d, %d, %d, %d, %d, \n",
      	      m_RxMsgLen,
      	      SCIA_FrameToRx[0],
      	      SCIA_FrameToRx[1],
      	      SCIA_FrameToRx[2],
      	      SCIA_FrameToRx[3],
      	      SCIA_FrameToRx[4],
      	      SCIA_FrameToRx[5],
      	      SCIA_FrameToRx[6]);
          udp_data_to_send = TRUE;
        }
    #endif  	

  
  if ( (UINT8)CurrentMsg.Param ){
    
    PutEventMsg (EVT_SCI_RXERR, SCIA_TargetProc, PROC_SCIA, SCIA_CurrentRxToken); 
    StatisticData.SerialStat.MsgRxErr++;
  }
  else{
    
    PutEventMsg (EVT_SCI_RXMSG, SCIA_TargetProc, PROC_SCIA, SCIA_CurrentRxToken);
    StatisticData.SerialStat.MsgRxOk++;
  }
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIAProc_IDLE;  // volver a estado IDLE

}


////////////////////////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
////////////////////////////////////////////////////////////////////////////////////////////////
//
//--------------------------------------------------------------------------------------
//-- Funcion de Inicialización
//
void SCIA_Init (BYTE TargetProc, BYTE  ProtocolFrame)
{
  BYTE    tmp_cfgCode;	
  BYTE    tmp_byteIdx;	

  SCIA_TargetProc = TargetProc;
  SCIA_ProtocolFrame =  ProtocolFrame;
  
  SCIA_RX_TimerID = 0xFF;
  SCIA_TX_TimerID = 0xFF;
  
 // leer byte de configuración para la comunicacion serie
  eeprom_read_buffer(EE_ADDR_CFG_SCIA, &tmp_cfgCode, 1);
  tmp_byteIdx = tmp_cfgCode & CFG_SCI_BAUDRATE_MASK;
  SCIA_SCIBD  = BaudCode2RegVal[tmp_byteIdx];
  SCIA_SCICR1 = 0x00;
  if ((tmp_cfgCode & CFG_SCI_PARITY_NONE) != CFG_SCI_PARITY_NONE){	  // Habilitar paridad
    SCIA_SCICR1 = SCI1CR1_PE_MASK;                    // 7 bits de datos + paridad = 8 bits
    if (!(tmp_cfgCode & CFG_SCI_DATABITS_MASK)){      // 8 bits de datos
      SCIA_SCICR1 |= SCI1CR1_M_MASK;                  // total 9 bits: 8 bits de datos, + paridad 
    }
    if (tmp_cfgCode & CFG_SCI_PARITY_ODD){
      SCIA_SCICR1 |= SCI1CR1_PT_MASK;                 //  paridad IMPAR
    }
  }
  
  SCIA_ConfigByte = tmp_cfgCode;
  SCIA_SCICR2 = 0x00; 
  
  eeprom_read_buffer(EE_ADDR_TMR_TXWAIT_SCIA, (UINT8 *)&SCIA_TMR_TX_TIMEOUT, 2);			// Tiempo de espera para transmitir una trama entera (2 BYTES)
  eeprom_read_buffer(EE_ADDR_TMR_RXWAIT_SCIA, (UINT8 *)&SCIA_TMR_RX_TIMEOUT, 2);			// Tiempo de espera para recibir una trama entera (2 BYTES)
  eeprom_read_buffer(EE_ADDR_TMR_DRIVE_SCIA, (UINT8 *)&SCIA_TMR_DRIVE_TIMEOUT, 2); 	// Tiempo antes de enviar datos despues de establecer DRIVE 485 (2 BYTES)
  
 
  RS485CFG_A_DE   = 1;              // control DE como salida, RS485_A
  RS485CFG_A_RE_  = 1;              // control RE# como salida, RS485_A
  RS485CFG_A_SEL  = 1;              // control del MUX para selección de modo, como salida  
  
  if (SCIA_ConfigByte & CFG_SCI_PHYMODE_MASK){            // modo RS485
    RS485_A_SEL     = MUX_SEL_RS485;  // MUX seleccionado para tomar señales del DS3695       
    RS485_A_RE_     = 0; 				      // RE# = 0 incializado para recibir
    RS485_A_DE      = 0;				      // DE = 0  incializado para recibir
  } 
  else{
    RS485_A_SEL     = MUX_SEL_RS232;  // MUX seleccionado para tomar señales del MAX232     
  }

  SCIA_CurrentTxToken = 0xFF;       // Ningún buffer transmitiéndose, habilitado para TX      
  StatisticData.SerialStat.MsgTxOk    = 0x0000;
  StatisticData.SerialStat.MsgRxOk    = 0x0000;
  StatisticData.SerialStat.MsgTxErr   = 0x0000;
  StatisticData.SerialStat.MsgRxErr   = 0x0000;
  SCIA_TxErrAcc = 0x00;
  SCIA_RxErrAcc = 0x00;
  SCIA_RxByteCount = 0x00;   
  SCIA_TxByteCount = 0x00;   
  SCIA_StateFlags  = 0x00;

  SCIA_SCICR2 = 0x00;  
//  SCIA_SCICR2 |=  SCI1CR2_RE_MASK ;			// Rx Enable
//  SCIA_SCICR2 |=  SCI1CR2_TE_MASK ; 		// Tx Enable
   
}


//--------------------------------------------------------------------------------------
//-- Funcion de Reinicialización en caso de fallas 
//
void SCIA_ReInit (BYTE TargetProc, BYTE  ProtocolFrame)
{
  BYTE    tmp_cfgCode;	
  BYTE    tmp_byteIdx;	

  SCIA_TargetProc = TargetProc;
  SCIA_ProtocolFrame =  ProtocolFrame;

 // leer byte de configuración para la comunicacion serie
  eeprom_read_buffer(EE_ADDR_CFG_SCIA, &tmp_cfgCode, 1);
  tmp_byteIdx = tmp_cfgCode & CFG_SCI_BAUDRATE_MASK;
  SCIA_SCIBD  = BaudCode2RegVal[tmp_byteIdx];

 // leer byte de configuración para la comunicacion serie
  eeprom_read_buffer(EE_ADDR_CFG_SCIA, &tmp_cfgCode, 1);
  tmp_byteIdx = tmp_cfgCode & CFG_SCI_BAUDRATE_MASK;
  SCIA_SCIBD  = BaudCode2RegVal[tmp_byteIdx];
  if ((tmp_cfgCode & CFG_SCI_PARITY_NONE) != CFG_SCI_PARITY_NONE){	  // Habilitar paridad
    SCIA_SCICR1 = SCI1CR1_PE_MASK;                    // 7 bits de datos + paridad = 8 bits
    if (!(tmp_cfgCode & CFG_SCI_DATABITS_MASK)){      // 8 bits de datos
      SCIA_SCICR1 |= SCI1CR1_M_MASK;                  // total 9 bits: 8 bits de datos, + paridad 
    }
    if (tmp_cfgCode & CFG_SCI_PARITY_ODD){
      SCIA_SCICR1 |= SCI1CR1_PT_MASK;                 //  paridad IMPAR
    }
  }
  
  SCIA_ConfigByte = tmp_cfgCode;
  SCIA_SCICR2 = 0x00;
  
  eeprom_read_buffer(EE_ADDR_TMR_TXWAIT_SCIA, (UINT8 *)&SCIA_TMR_TX_TIMEOUT, 2);			// Tiempo de espera para transmitir una trama entera (2 BYTES)
  eeprom_read_buffer(EE_ADDR_TMR_RXWAIT_SCIA, (UINT8 *)&SCIA_TMR_RX_TIMEOUT, 2);			// Tiempo de espera para recibir una trama entera (2 BYTES)
  eeprom_read_buffer(EE_ADDR_TMR_DRIVE_SCIA, (UINT8 *)&SCIA_TMR_DRIVE_TIMEOUT, 2); 	// Tiempo antes de enviar datos despues de establecer DRIVE 485 (2 BYTES)
  

  RS485CFG_A_DE   = 1;              // control DE como salida, RS485_A
  RS485CFG_A_RE_  = 1;              // control RE# como salida, RS485_A
  RS485CFG_A_SEL  = 1;              // control del MUX para selección de modo, como salida  
  
  if (SCIA_ConfigByte & CFG_SCI_PHYMODE_MASK){            // modo RS485
    RS485_A_SEL     = MUX_SEL_RS485;  // MUX seleccionado para tomar señales del DS3695       
    RS485_A_RE_     = 0; 				      // RE# = 0 incializado para recibir
    RS485_A_DE      = 0;				      // DE = 0  incializado para recibir
  } 
  else{
    RS485_A_SEL     = MUX_SEL_RS232;  // MUX seleccionado para tomar señales del MAX232     
  }

  StatisticData.SerialStat.MsgTxOk    = 0x0000;
  StatisticData.SerialStat.MsgRxOk    = 0x0000;
  StatisticData.SerialStat.MsgTxErr   = 0x0000;
  StatisticData.SerialStat.MsgRxErr   = 0x0000;

  SCIA_RxByteCount = 0x00;   
  SCIA_TxByteCount = 0x00;   
  SCIA_StateFlags  = 0x00;
  SCIA_CurrentTxToken = 0xFF;       // Ningún buffer transmitiéndose, habilitado para TX      
    
	CurrentProcessState [PROC_SCIA] = (EVENT_FUNCTION*)SCIAProc_IDLE;	
  
//  SCIA_SCICR2 |=  SCI1CR2_RE_MASK ;			// Rx Enable
//  SCIA_SCICR2 |=  SCI1CR2_TE_MASK ; 		// Tx Enable
  
}


////////////////////////////////////////////////////////////////////////////////////////////////
// SCI1 ISR
////////////////////////////////////////////////////////////////////////////////////////////////
//#pragma CODE_SEG NON_BANKED        

interrupt void near SCI1_isr (void)
{
  m_SCIA_SCISR1 = SCIA_SCISR1;
         
//--------------RECEPCION-------------------------------------------------------   
//		 
  if ((m_SCIA_SCISR1 & SCI1SR1_RDRF_MASK) && (SCIA_SCICR2 & SCI1CR2_RIE_MASK)){
       
    if (SCIA_RxByteCount)		                       // si quedan bytes por recibir
    {                       
      *SCIA_FrameToRx = SCIA_SCIDRL; 
      m_RxMsgLen++;

      if ( SCIA_ProtocolFrame  == SCI_FRAME_MODBUSRTU ) 	 // Delimitar frame ModbusRTU
      {
          if ( SCIA_RxByteIdx == 5 ) 
          {
              if ( m_RxMsg_FC == 16 )		                  // Write Multiple Registers
              {            
                SCIA_RxByteCount = *SCIA_FrameToRx;			  // Byte count 
                //SCIA_RxByteCount <<= 1;                   // x2 byte count
                SCIA_RxByteCount += 0x03;                 // CRC + bound effects
                SCIA_RxByteIdx = 10;                      // Para que no entre en los demás if's
              }
          }

          if ( SCIA_RxByteIdx == 2 ) 
          {
              if (( m_RxMsg_FC == 4)||									// Read Input Register
                  ( m_RxMsg_FC == 3)||                  // Read Multiple Register
                  ( m_RxMsg_FC == 2)||                  // Read Input Discrete
                  ( m_RxMsg_FC == 100)||                // User defined FC
                  ( m_RxMsg_FC == 1))                   // Read Coils
              {
                SCIA_RxByteCount = (UINT16)*SCIA_FrameToRx;		  // Byte count 
                SCIA_RxByteCount += 0x03;               // CRC + bound effects 
                SCIA_RxByteIdx = 10;                    // Para que no entre en los demás if's
              }
          }
          
          if ( SCIA_RxByteIdx == 1 )		              // Segundo Byte: Function Code
          {        
              m_RxMsg_FC =  *SCIA_FrameToRx;
              if (( m_RxMsg_FC == 5)||                // Write Coil
                  ( m_RxMsg_FC == 6)||								// Write Single Register
                  ( m_RxMsg_FC == 16))								// Write Multiple Register
              {
                  SCIA_RxByteCount = 4;               // Bytes restantes de msg. de len fija 
                  SCIA_RxByteCount += 0x03;               // CRC + bound effects 
                  SCIA_RxByteIdx = 10;                // Para que no entre en los demás if's
              } 
              else if(m_RxMsg_FC > 0x80)
              {
                  SCIA_RxByteCount = 1;               // Bytes restantes de msg. de len fija 
                  SCIA_RxByteCount += 0x03;               // CRC + bound effects 
                  SCIA_RxByteIdx = 10;                // Para que no entre en los demás if's              
              }
          }
          
          if ( SCIA_RxByteIdx == 0 )
          {	
            if ( SCIA_RX_TimerID == 0xFF){
            	// Si no se había incializado un timer de recepción, hacerlo al ingresar el primer byte
            	SCIA_RX_TimerID =  TMR_SetTimer ( SCIA_TMR_RX_TIMEOUT , PROC_SCIA, 0x00, FALSE);
            }
          }
      } 
      else if ( SCIA_ProtocolFrame  == SCI_FRAME_SPACOM )    // Delimitar Frame SPACOM
      {
      
          *SCIA_FrameToRx &= 0x7F;                      // SPA es siempre en 7 bits de datos
          
          if ( SCIA_RxByteIdx != 0 )
          {           
              if ( (*SCIA_FrameToRx == 0x0A)&&              //  cr lf 0x0D 0x0A final del frame
                   ( SCIA_RxByteIdx) )       
              {
                  SCIA_RxByteCount = 0x01;                  // terminar OK
              }
          }
/*
          if ( SCIA_RxByteIdx == 1 )		                // Segundo Byte: debe ser '<'
          {        
              if ( *SCIA_FrameToRx != '<')              // 
              {
                  SCIA_SCICR2  &= ~(SCI1CR2_RE_MASK) ;		// Rx Disable
                  SCIA_SCICR2  &= ~(SCI1CR2_RIE_MASK);	  // Rx Interrupt Disable
                  PutEventMsg (EVT_SCI_RXMSG, PROC_SCIA, PROC_SCIA, 0xFF);    // ERROR    
              }
          }
*/          
          if ( SCIA_RxByteIdx == 0 )
          {	
              if ( SCIA_RX_TimerID == 0xFF){
              	// Si no se había incializado un timer de recepción, hacerlo al ingresar el primer byte
              	SCIA_RX_TimerID =  TMR_SetTimer ( SCIA_TMR_RX_TIMEOUT , PROC_SCIA, 0x00, FALSE);
              }

              if ( *SCIA_FrameToRx != '<')           // Primer Byte debe ser '<' descartar previos
              {
                  SCIA_RxByteCount++;    
                  SCIA_RxByteIdx--;
                  SCIA_FrameToRx--;             
              } 
              
          }

      } 
      else if ( SCIA_ProtocolFrame  == SCI_FRAME_MODBUSRTU_SLV ) 	 // Delimitar frame ModbusRTU Slave
      {
          if ( SCIA_RxByteIdx == 1 ) 
          {
              m_RxMsg_FC =  *SCIA_FrameToRx;
              // TODO: analizar demas peticiones de escrituras, y rechazarlas   
              if (( m_RxMsg_FC == 5)||                // Write Coil
                  ( m_RxMsg_FC == 6))									// Write Single Register
              {
                  SCIA_RxByteCount = 1;               // Bytes restantes de msg. de len fija 
                  SCIA_RxByteIdx = 10;                // Para que no entre en los demás if's
                  SCIA_SCICR2  &= ~(SCI1CR2_RE_MASK) ;		// Rx Disable
                  SCIA_SCICR2  &= ~(SCI1CR2_RIE_MASK);	  // Rx Interrupt Disable
                  // TODO: cambiar por evento nuevo RXEND o RXERR
                  PutEventMsg (EVT_SCI_RXMSG, PROC_SCIA, PROC_SCIA, 0x01);    
                  return;
              } 
              else
              {
                
                SCIA_RxByteCount = 0x09;                 // CRC + bound effects + 6 data
                SCIA_RxByteIdx = 10;                     // Para que no entre en los demás if's
              }
          }

          if ( SCIA_RxByteIdx == 0 )
          {	
            if ( SCIA_RX_TimerID == 0xFF){
            	// Si no se había incializado un timer de recepción, hacerlo al ingresar el primer byte
            	SCIA_RX_TimerID =  TMR_SetTimer ( SCIA_TMR_RX_TIMEOUT , PROC_SCIA, 0x00, FALSE);
            }
          }
      }
      else if ( SCIA_ProtocolFrame  == SCI_FRAME_BIOMETRIC_ANVIZ ) 	 // Delimitar frame Anviz lector Biometrico     
      {
          if ( SCIA_RxByteIdx == 5 ) 
          {
              m_RxMsg_FC =  *SCIA_FrameToRx; 
              
              if ( m_RxMsg_FC == 0xBC )								   // Respuesta a primera consulta general ( 29 Bytes )
              {
                  SCIA_RxByteCount = 24;                 // bound effects + 23 data
                  SCIA_RxByteIdx = 10;                   // Para que no entre en los demás if's
              } 
              else if( m_RxMsg_FC == 0xC0 )							 // Respuesta a segunda consulta si hay regs. ( 26 Bytes )
              {                
                SCIA_RxByteCount = 21;                   // bound effects + 20 data
                SCIA_RxByteIdx = 10;                     // Para que no entre en los demás if's
              }
              else if( m_RxMsg_FC == 0xCE )							 // Respuesta a tercera consulta si hay regs. ( 14 Bytes )
              {                
                SCIA_RxByteCount = 9;                    // bound effects + 8 data
                SCIA_RxByteIdx = 10;                     // Para que no entre en los demás if's
              }
              else
              {
                  SCIA_SCICR2  &= ~(SCI1CR2_RE_MASK) ;		// Rx Disable
                  SCIA_SCICR2  &= ~(SCI1CR2_RIE_MASK);	  // Rx Interrupt Disable
                  // TODO: cambiar por evento nuevo RXEND o RXERR
                  PutEventMsg (EVT_SCI_RXMSG, PROC_SCIA, PROC_SCIA, 0x01);    
                  return;                
              }
          }

          if ( SCIA_RxByteIdx == 0 )
          {	
            if ( SCIA_RX_TimerID == 0xFF){
            	// Si no se había incializado un timer de recepción, hacerlo al ingresar el primer byte
            	SCIA_RX_TimerID =  TMR_SetTimer ( SCIA_TMR_RX_TIMEOUT , PROC_SCIA, 0x00, FALSE);
            }
          }
      }  
        
                                   
      SCIA_RxByteCount--;    
      SCIA_RxByteIdx++;
      SCIA_FrameToRx++;
                      
      if (!SCIA_RxByteCount)
      {
          SCIA_SCICR2  &= ~(SCI1CR2_RE_MASK) ;		// Rx Disable
          SCIA_SCICR2  &= ~(SCI1CR2_RIE_MASK);	  // Rx Interrupt Disable
          PutEventMsg (EVT_SCI_RXMSG, PROC_SCIA, PROC_SCIA, 0x00);    
      }
    }
    else{
      SCIA_TmpVar  = SCIA_SCIDRL;	                // Flush buffer
    } 
  }
      
//--------------TRANSMISION-------------------------------------------------------   
  if (( m_SCIA_SCISR1 & SCI1SR1_TC_MASK ) && (SCIA_SCICR2 & SCI1CR2_TCIE_MASK))	        // si es interrupcion de TXC
  {
      if (!SCIA_TxByteCount)
      {
         SCIA_SCICR2 &= ~(SCI1CR2_SCTIE_MASK);	    // TIE Interrupt Disable
         SCIA_SCICR2 &= ~(SCI1CR2_TE_MASK ); 			  // TE Disable
         SCIA_SCICR2 &= ~(SCI1CR2_TCIE_MASK);       // TCIE Interrupt Disable      
         PutEventMsg (EVT_SCI_TXMSG, PROC_SCIA, PROC_SCIA, 0x00);    
      }
      else
      {
        SCIA_SCIDRL = *SCIA_FrameToTx;
        SCIA_FrameToTx++;
        SCIA_TxByteCount--; 
      }
  }
   
//--------------ERROR DE COMUNICACION--------------------------------------------   
//
//  SCIA_SCICR2 &= ~(SCI1CR2_RE_MASK) ;			            // Rx Disable
//  SCIA_SCICR2 &= ~(SCI1CR2_RIE_MASK);			            // Rx Interrupt Disable
//  SCIA_SCICR2 &= ~(SCI1CR2_TE_MASK ); 			          // Tx Disable
//  SCIA_SCICR2 &= ~(SCI1CR2_TCIE_MASK);                // TxC Interrupt Disable      
//  SCIA_ReceiveFrame (3, );

}

#pragma CODE_SEG DEFAULT



