#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "MC9S12NE64.h"
#include "datatypes.h"

#include "Commdef.h"
#include "dSCIB.h"
#include "global.h"

#pragma CODE_SEG NON_BANKED        

////////////////////////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_SCIB
////////////////////////////////////////////////////////////////////////////////////////////////
//
const EVENT_FUNCTION	SCIBProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_SCIBIdleTimeout },
{EVT_SCI_TXCMD        , f_SCIBSendCommand	},
{EVT_SCI_TXREQ 	      , f_SCIBSendRequest	},
{EVT_SCI_RXALL 	      , f_SCIBReceiveAll	},
{EVT_SCI_RXREQ 	      , f_SCIBReceiveMsg 	},
{EVT_SCI_TXMSG 	      , f_SCIBIdleFrameTx	},
{EVT_SCI_RXMSG 	      , f_SCIBIdleFrameRx },
{EVT_OTHER_MSG     		, f_Consume         }};

const EVENT_FUNCTION	SCIBProc_TOTRANSMIT [] =
{
{EVT_TMR_TIMEOUT 	    , f_SCIBToTxTimeout	},
{EVT_SCI_TXREQ 	      , f_SCIBTxNotReady  },
{EVT_OTHER_MSG     		, f_Consume         }};

const EVENT_FUNCTION	SCIBProc_TRANSMITING [] =
{
{EVT_TMR_TIMEOUT 	    , f_SCIBTxTimeout	  },
{EVT_SCI_TXMSG 	      , f_SCIBTxFrame	    },
{EVT_SCI_TXREQ 	      , f_SCIBTxNotReady  },
{EVT_OTHER_MSG     		, f_Consume         }};

const EVENT_FUNCTION	SCIBProc_RECEIVING [] =
{
{EVT_TMR_TIMEOUT 	    , f_SCIBRxTimeout	  },
{EVT_SCI_RXMSG 	      , f_SCIBRxFrame	    },
{EVT_SCI_TXREQ 	      , f_SCIBTxNotReady  },
{EVT_OTHER_MSG     		, f_Consume         }};


////////////////////////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_SCIB
////////////////////////////////////////////////////////////////////////////////////////////////
//
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//-------------- estado : IDLE ---------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//
void f_SCIBIdleTimeout (void) 
{
  
  SCIB_RX_TimerID = 0xFF;
  
  // Si es Modbus SLAVE, reinsertar el ciclo de espera de respuesta
  if (SCIB_ProtocolFrame == SCI_FRAME_MODBUSRTU_SLV)
  {
      (void)MemSetUsedLen(SCIB_CurrentRxToken, 0x08);   // Recibir por lo menos 8 bytes
      // En la parte alta del parámetro se pasa el offset de transmisión: CERO. 
      PutEventMsg (EVT_SCI_RXALL, PROC_SCIB, PROC_SCIB, (UINT16)SCIB_CurrentRxToken);
  } 
  else
  {
  // Informar error en recepcion
    PutEventMsg (SCIB_EVT_SCI_RXERR, SCIB_TargetProc, PROC_SCIB, SCIB_CurrentRxToken); 
  }
  return; 
  
}


/********************************************************************************************	 
 *  Atención al requerimiento de envío de mensaje. 																					*
 *  Responde a los mensajes:																																*
 *  - EVT_SCI_TXREQ : transmitir mensaje sin esperar respuesta.															*
 *  - EVT_SCI_TXCMD : transmitir comando y esperar una respuesta (via f_SCIBSendCommand)		*
 *	Parámetros del mensaje:																																	*
 *	- LOW BYTE: Token del buffer de transmisión (TxBuffer)																	*
 *  - HIGH BYTE: offset en el TxBuffer desde donde comenzar a transmitir										*
 *  - La longitud a transmitir está en el parámetro wUsedLen asociado al buffer de memoria. *
 *  En modo RS485 pasa al estado TOTRANSMIT para esperar el establecimiento de la linea     *
 ********************************************************************************************/
void f_SCIBSendCommand (void)
{
    SCIB_StateFlags |= FLG_TX_COMMAND;
    f_SCIBSendRequest();
  	return;
}

void f_SCIBSendRequest (void)
{
    // Si hay transmisión en curso rechazar el requerimiento.
    if ( SCIB_TxByteCount ) {
      PutEventMsg (SCIB_EVT_SCI_TXERR, SCIB_TargetProc, PROC_SCIB, SCIB_CurrentTxToken);
      return;                     
    }
    
    if (SCIB_ConfigByte & CFG_SCI_PHYMODE_MASK){            // modo RS485
      RS485_B_RE_ = 1; 				                              // TX Mode
      RS485_B_DE  = 1;				                              // TX Mode
    }
    LED_GREEN_ON;
   
    // Offset desde donde transmitir, en la parte alta del parámetro
    SCIB_TxOffset = (UINT8)((CurrentMsg.Param)>>8);
    
    // El token del buffer está en la parte baja del parámetro del mensaje
    SCIB_CurrentTxToken = (UINT8)CurrentMsg.Param;
    if ( MemGetBufferPtr( SCIB_CurrentTxToken, (void**) &(SCIB_FrameToTx)) != OK ){    
      // ABR: control de errores
      PutEventMsg (SCIB_EVT_SCI_TXERR, SCIB_TargetProc, PROC_SCIB, SCIB_CurrentTxToken);
      return;   
    }

    if ( MemGetUsedLen(SCIB_CurrentTxToken, &SCIB_TxByteCount) != OK)     // Bytes a transmitir
        MON_ResetIfNotConnected(0x21);    // Error de SW, Reset (TODO: revisar acciones a tomar)
    
    if (SCIB_ConfigByte & CFG_SCI_PHYMODE_MASK)                           // modo RS485
    {            
      	SCIB_TX_TimerID =  TMR_SetTimer (10, PROC_SCIB, 0x00, FALSE);															 
        FirstEventInCurrentState = (EVENT_FUNCTION*)SCIBProc_TOTRANSMIT;  // Esperar establecimiento de TX
    } 
    else
    {
        SCIB_FrameToTx += SCIB_TxOffset;
        
      #if DEBUG_AT(DBG_LEVEL_4)
        if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_4)
        {                    
      	  (void)sprintf((char *)udp_buf,"SCIB Tx: %d, %d, %d, %d, %d, %d, %d, %d, \n",
      	      SCIB_TxByteCount,
      	      SCIB_FrameToTx[0],
      	      SCIB_FrameToTx[1],
      	      SCIB_FrameToTx[2],
      	      SCIB_FrameToTx[3],
      	      SCIB_FrameToTx[4],
      	      SCIB_FrameToTx[5]);
          udp_data_to_send = TRUE;
        }
    #endif  	        
    
        SCIB_SCIDRL = *SCIB_FrameToTx;
//  ABR: 30-NOV-2010 TIP: revisar por qué se comporta distinto bajo algunas condiciones..
//  En SPA no se transmite este primer byte ?, entonces se comenta la siguiente linea
        SCIB_FrameToTx++;
        SCIB_TxByteCount--; 
        SCIB_SCICR2 |=  SCI1CR2_TCIE_MASK;            // TxC Interrupt Enable
        SCIB_SCICR2 |=  SCI1CR2_TE_MASK ; 				    // Tx Enable
        //SCIB_SCICR2 |=  SCI1CR2_SCTIE_MASK;	      // Tx Interrupt Enable
        
        
        
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
void f_SCIBReceiveAll (void)
{
    SCIB_StateFlags |= FLG_RX_RELOAD;
    f_SCIBReceiveMsg();
    return;
}

void f_SCIBReceiveMsg (void)
{

  // Si hay recepción en curso rechazar el requerimiento.
  if ( SCIB_RxByteCount ) {
    PutEventMsg (SCIB_EVT_SCI_RXERR, SCIB_TargetProc, PROC_SCIB, SCIB_CurrentTxToken);
    return;                     
  }

  if (SCIB_ConfigByte & CFG_SCI_PHYMODE_MASK){            // modo RS485
    RS485_B_RE_ = 0; 				    // RX Mode
    RS485_B_DE  = 0;				    // RX Mode
  }
 
  // Offset donde escribir la respuesta, en la parte alta del parámetro
  SCIB_RxOffset = (UINT8)((CurrentMsg.Param)>>8);
  // El token del buffer está en la parte baja del parámetro del mensaje
  SCIB_CurrentRxToken = (UINT8)CurrentMsg.Param;
  (void)MemGetBufferPtr ( SCIB_CurrentRxToken, (void**) &(SCIB_FrameToRx));
  SCIB_FrameToRx += SCIB_RxOffset;
  
  if ( MemGetUsedLen(SCIB_CurrentRxToken, &SCIB_RxByteCount) != OK)   // Bytes a recibir
        MON_ResetIfNotConnected(0x22);    // Error de SW, Reset (TODO: revisar acciones a tomar)
        
	SCIB_RxByteIdx = 0x00;                        // Indice contador de Bytes recibidos
  m_SCIB_RxMsgLen = 0x00;
  
  SCIB_TmpVar  = SCIB_SCISR1;	                  // Clear pending flag
  SCIB_TmpVar  = SCIB_SCIDRL;	                  // Flush buffer
  SCIB_SCICR2 |=  SCI1CR2_RE_MASK ;			        // Rx Enable
  SCIB_SCICR2 |=	SCI1CR2_RIE_MASK;		          // Rx Interrupt Enable
	
  if (SCIB_ConfigByte & CFG_SCI_PHYMODE_MASK)	   // modo RS485
  {          
    if (!(SCIB_StateFlags & FLG_RX_RELOAD))     // Si no hay que esperar indefinidamente
  	  SCIB_RX_TimerID =  TMR_SetTimer ( SCIB_TMR_RX_TIMEOUT , PROC_SCIB, 0x00, FALSE);
    FirstEventInCurrentState = (EVENT_FUNCTION*)SCIBProc_RECEIVING; 
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
void f_SCIBIdleFrameTx (void)
{

    LED_GREEN_OFF;
    LED_RED_ON;
    StatisticData.SerialStat.MsgTxOk++;
  
    if ( SCIB_StateFlags & FLG_TX_COMMAND )   // comando transmitido, entonces esperar respuesta
    {
        SCIB_StateFlags &= ~FLG_TX_COMMAND;
      	SCIB_RxOffset = SCIB_TxOffset;          // Tomar el mismo offset para escribir la respuesta
      	
        // Recuperar puntero al buffer de transmisión que se reutilizará para recepción 
        SCIB_CurrentRxToken = SCIB_CurrentTxToken;  
        if ( MemGetBufferPtr( SCIB_CurrentRxToken , (void**)&(SCIB_FrameToRx)) != OK ){    
          // ABR: control de errores . TODO: REsetear?
          PutEventMsg (SCIB_EVT_SCI_TXERR, SCIB_TargetProc, PROC_SCIB, SCIB_CurrentTxToken);
          return;   
        }
        // Preparar para leer la misma cantidad de bytes que se envió en el comando
//        if ( MemGetUsedLen(SCIB_CurrentRxToken, &SCIB_RxByteCount) != OK)   // Bytes a recibir
//            return;      
        // Preparar para leer la maxima cantidad de bytes que soporta el buffer
        SCIB_RxByteCount = (MEM_MAX_BUF_LEN - SCIB_RxOffset);
        
      	SCIB_RxByteIdx = 0x00;                      // Indice contador de Bytes recibidos
        
        SCIB_FrameToRx += SCIB_RxOffset;
        m_SCIB_RxMsgLen = 0x00;
        SCIB_TmpVar  = SCIB_SCISR1;	          // Clear pending flag
        SCIB_TmpVar  = SCIB_SCIDRL;	          // Flush buffer
        SCIB_SCICR2 |=  SCI1CR2_RE_MASK ;			// Rx Enable
        SCIB_SCICR2 |=	SCI1CR2_RIE_MASK;			// Rx Interrupt Enable
        
      	// Si es espera de respuesta a comando, el timer se inicializa aquí aún para RS232
      	SCIB_RX_TimerID =  TMR_SetTimer ( SCIB_TMR_RX_TIMEOUT , PROC_SCIB, 0x00, FALSE);
        
    } 
    
    PutEventMsg (SCIB_EVT_SCI_TXMSG, SCIB_TargetProc, PROC_SCIB, SCIB_CurrentTxToken );
  
}

/********************************************************************************************	 
 *  Atención al evento de mensaje recibido. (para modo RS232, siempre esta en IDLE)         *
 *  Responde a los mensajes:																																*
 *  - EVT_SCI_RXMSG : Mensaje recibido OK.                     															*
 *	Se envía evento EVT_SCI_RXMSG al proceso que requirió la recepción del mensaje con el   *
 *	token del RxBuffer como parámetro.  																										*
 *  La longitud recibida se graba en el parámetro wUsedLen asociado al buffer de memoria    *
 ********************************************************************************************/
void f_SCIBIdleFrameRx (void)
{

  TMR_FreeTimer (SCIB_RX_TimerID);
  SCIB_RX_TimerID = 0xFF;

  LED_RED_OFF;
  SCIB_RxErrAcc = 0x00;
  (void)MemSetUsedLen(SCIB_CurrentRxToken, m_SCIB_RxMsgLen);        // Bytes recibidos
  
  if ((UINT8)CurrentMsg.Param) {
      StatisticData.SerialStat.MsgRxErr++;
      if (SCIB_ProtocolFrame == SCI_FRAME_MODBUSRTU_SLV)
      {
          (void)MemSetUsedLen(SCIB_CurrentRxToken, 0x08);   // Recibir por lo menos 8 bytes
          // En la parte alta del parámetro se pasa el offset de transmisión: CERO. 
          PutEventMsg (EVT_SCI_RXALL, PROC_SCIB, PROC_SCIB, (UINT16)SCIB_CurrentRxToken);
      }   
  } 
  else
  {																   
      #if DEBUG_AT(DBG_LEVEL_4)
        if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_4)
        {                    
          SCIB_FrameToRx -= m_SCIB_RxMsgLen;
      	  (void)sprintf((char *)udp_buf,"SCIB Rx: %d, %d, %d, %d, %d, %d, %d, %d, \n",
      	      m_SCIB_RxMsgLen,
      	      SCIB_FrameToRx[0],
      	      SCIB_FrameToRx[1],
      	      SCIB_FrameToRx[2],
      	      SCIB_FrameToRx[3],
      	      SCIB_FrameToRx[4],
      	      SCIB_FrameToRx[5],
      	      SCIB_FrameToRx[6]);
          udp_data_to_send = TRUE;
        }
    #endif  	
  
      PutEventMsg (SCIB_EVT_SCI_RXMSG, SCIB_TargetProc, PROC_SCIB, SCIB_CurrentRxToken);
      StatisticData.SerialStat.MsgRxOk++;
  }
  
  
}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//---------- estado : TOTRANSMIT  ------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//
void f_SCIBToTxTimeout (void)
{
    SCIB_TX_TimerID = 0xFF;

    SCIB_TmpVar  = SCIB_SCIDRL;	                  // Flush buffer
    SCIB_TmpVar  = SCIB_SCISR1;	                  // Clear pending flag
  
    SCIB_FrameToTx += SCIB_TxOffset;
    SCIB_SCIDRL = *SCIB_FrameToTx;
    SCIB_TxByteCount--;      
    SCIB_FrameToTx++;

    // SCIB_SCICR2 |=  SCI1CR2_SCTIE_MASK;	      // Tx Interrupt Enable
    SCIB_SCICR2 |=  SCI1CR2_TCIE_MASK;            // TxC Interrupt Enable
    SCIB_SCICR2 |=  SCI1CR2_TE_MASK ; 				    // Tx Enable

   	SCIB_TX_TimerID =  TMR_SetTimer ( SCIB_TMR_TX_TIMEOUT , PROC_SCIB, 0x00, FALSE);
    FirstEventInCurrentState = (EVENT_FUNCTION*)SCIBProc_TRANSMITING; 
}


//-----------------------------------------------------------------------------------------
// Se recibió un requerimiento de transmisión pero no se puede realizar en el estado actual
// -> RESPONDER CON UNA EXCEPCIÓN  
//
void f_SCIBTxNotReady (void)
{
  // Informar con una excepción la imposibilidad de transmitir mensaje.
  PutEventMsg (SCIB_EVT_SCI_TXBUSY, SCIB_TargetProc, PROC_SCIB, CurrentMsg.Param);
  
  return;
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//---------- estado : TRANSMITTING  ----------------------------------------------------------
//--------------------------------------------------------------------------------------------
//
void f_SCIBTxTimeout (void)								
{
    SCIB_TX_TimerID = 0xFF;
    
    // Dejar en modo recepción..
    RS485_B_DE  = 0;				    // RX Mode
    RS485_B_RE_ = 0; 				    // RX Mode  
    
    LED_GREEN_OFF;

    // SCIB_SCICR2 &= ~(SCI1CR2_SCTIE_MASK);	    // Tx Interrupt Disable
    SCIB_SCICR2 &= ~(SCI1CR2_TE_MASK ); 			    // Tx Disable
    SCIB_SCICR2 &= ~(SCI1CR2_TCIE_MASK);          // TxC Interrupt Disable      
    SCIB_TxByteCount = 0x00;   
    
#ifdef MON_SERIAL_ERR_RESET_ENABLE
    // Acciones correctivas.. errores de transmision acumulados
    if ( ++SCIB_TxErrAcc >= MAX_SERIAL_TXERR){
        MON_ResetIfNotConnected(0x23);
    }
#endif
    
    PutEventMsg (SCIB_EVT_SCI_TXERR, SCIB_TargetProc, PROC_SCIB, SCIB_CurrentTxToken);
    StatisticData.SerialStat.MsgTxErr++;
    FirstEventInCurrentState = (EVENT_FUNCTION*)SCIBProc_IDLE;  // Volver a estado IDLE
    
}


void f_SCIBTxFrame (void)
{
    TMR_FreeTimer (SCIB_TX_TimerID);
    SCIB_TX_TimerID = 0xFF;

    // Pasar a modo recepción..
    RS485_B_DE  = 0;				    // RX Mode
    RS485_B_RE_ = 0; 				    // RX Mode  

    LED_GREEN_OFF;
    LED_RED_ON;
  
    if ( SCIB_StateFlags & FLG_TX_COMMAND )   // comando transmitido, entonces esperar respuesta
    {
        SCIB_StateFlags &= ~FLG_TX_COMMAND;
      	SCIB_RxOffset = SCIB_TxOffset;          // Tomar el mismo offset para escribir la respuesta
      	
        // Recuperar puntero al buffer de transmisión que se reutilizará para recepción 
        SCIB_CurrentRxToken = SCIB_CurrentTxToken;  
        if ( MemGetBufferPtr( SCIB_CurrentRxToken , (void**)&(SCIB_FrameToRx)) != OK ){    
          // ABR: control de errores. TODO: Resetear ?
          PutEventMsg (SCIB_EVT_SCI_TXERR, SCIB_TargetProc, PROC_SCIB, SCIB_CurrentTxToken);
          StatisticData.SerialStat.MsgTxErr++;
          //Volver a IDLE
          FirstEventInCurrentState = (EVENT_FUNCTION*)SCIBProc_IDLE; 
          return;   
        }
        // Preparar para leer la misma cantidad de bytes que se envió en el comando
        if ( MemGetUsedLen(SCIB_CurrentRxToken, &SCIB_RxByteCount) != OK)   // Bytes a recibir
          MON_ResetIfNotConnected(0x24);    // Error de SW, Reset (TODO: revisar acciones a tomar)
          
      	SCIB_RxByteIdx = 0x00;                      // Indice contador de Bytes recibidos
        
        SCIB_FrameToRx += SCIB_RxOffset;
        m_SCIB_RxMsgLen = 0x00;
        SCIB_TmpVar  = SCIB_SCISR1;	          // Clear pending flag
        SCIB_TmpVar  = SCIB_SCIDRL;	          // Flush buffer
        SCIB_SCICR2 |=  SCI1CR2_RE_MASK ;			// Rx Enable
        SCIB_SCICR2 |=	SCI1CR2_RIE_MASK;			// Rx Interrupt Enable
      		
      	SCIB_RX_TimerID =  TMR_SetTimer ( SCIB_TMR_RX_TIMEOUT , PROC_SCIB, 0x00, FALSE);
        FirstEventInCurrentState = (EVENT_FUNCTION*)SCIBProc_RECEIVING; 
    } 
    else
    {
        FirstEventInCurrentState = (EVENT_FUNCTION*)SCIBProc_IDLE; 
    }
    
    StatisticData.SerialStat.MsgTxOk++;
    PutEventMsg (SCIB_EVT_SCI_TXMSG, SCIB_TargetProc, PROC_SCIB, SCIB_CurrentTxToken );
    
}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//---------- estado : RECEIVING  ------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//
void f_SCIBRxTimeout (void)
{
  
  SCIB_RX_TimerID = 0xFF;
  
//  RS485_B_RE_ = 1; 				    // TX Mode
//  RS485_B_DE  = 1;				    // TX Mode

  LED_RED_OFF;
  
  SCIB_TmpVar  = SCIB_SCISR1;	              // Clear pending flag
  SCIB_TmpVar  = SCIB_SCIDRL;	              // Flush buffer
  SCIB_SCICR2  &= ~(SCI1CR2_RE_MASK) ;			// Rx Disable
  SCIB_SCICR2  &= ~(SCI1CR2_RIE_MASK);			// Rx Interrupt Disable
  SCIB_RxByteCount = 0;   
  
// Acciones correctivas.. error de recepcion
  if (  ++SCIB_RxErrAcc >= MAX_SERIAL_RXERR){
#ifdef MON_SERIAL_ERR_RESET_ENABLE
        MON_ResetIfNotConnected(0x25);
#endif																				
  }
  // Si es espera indefinida, seguir recibiendo.
  if (SCIB_ProtocolFrame == SCI_FRAME_MODBUSRTU_SLV)
  {
      (void)MemSetUsedLen(SCIB_CurrentRxToken, 0x08);   // Recibir por lo menos 8 bytes
      // En la parte alta del parámetro se pasa el offset de transmisión: CERO. 
      PutEventMsg (EVT_SCI_RXALL, PROC_SCIB, PROC_SCIB, (UINT16)SCIB_CurrentRxToken);
  } 
  else
  {

    // Informar error en recepcion
    PutEventMsg (SCIB_EVT_SCI_RXERR, SCIB_TargetProc, PROC_SCIB, SCIB_CurrentRxToken); 
    StatisticData.SerialStat.MsgRxErr++;
  }
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIBProc_IDLE;  // volver a estado IDLE
  
}

void f_SCIBRxFrame (void)
{

//  RS485_B_RE_ = 1; 				    // TX Mode
//  RS485_B_DE  = 1;				    // TX Mode

  LED_RED_OFF;
  
  TMR_FreeTimer (SCIB_RX_TimerID);
  SCIB_RX_TimerID = 0xFF;
  SCIB_RxErrAcc = 0x00;

  (void)MemSetUsedLen(SCIB_CurrentRxToken, m_SCIB_RxMsgLen);        // Bytes recibidos
  
  if ( (UINT8)CurrentMsg.Param ) 
  {   
      StatisticData.SerialStat.MsgRxErr++;
      if (SCIB_ProtocolFrame == SCI_FRAME_MODBUSRTU_SLV)
      {
          (void)MemSetUsedLen(SCIB_CurrentRxToken, 0x08);   // Recibir por lo menos 8 bytes
          // En la parte alta del parámetro se pasa el offset de transmisión: CERO. 
          PutEventMsg (EVT_SCI_RXALL, PROC_SCIB, PROC_SCIB, (UINT16)SCIB_CurrentRxToken);
      }   
      
      PutEventMsg (SCIB_EVT_SCI_RXERR, SCIB_TargetProc, PROC_SCIB, SCIB_CurrentRxToken); 
  }
  else 
  {    
      StatisticData.SerialStat.MsgRxOk++;
      PutEventMsg (SCIB_EVT_SCI_RXMSG, SCIB_TargetProc, PROC_SCIB, SCIB_CurrentRxToken);
  }
  
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIBProc_IDLE;  // volver a estado IDLE

}


////////////////////////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
////////////////////////////////////////////////////////////////////////////////////////////////
//
//--------------------------------------------------------------------------------------
//-- Funcion de Inicialización
//
void SCIB_Init (BYTE TargetProc, BYTE  ProtocolFrame, BYTE _SCI_RX_MSG, BYTE _SCI_RX_ERR, BYTE _SCI_TX_MSG, BYTE _SCI_TX_ERR, BYTE _SCI_TX_BUSY)
{
  BYTE    tmp_cfgCode;	
  BYTE    tmp_byteIdx;	
  UINT8*  pTmpRxBuffer;
  UINT8   bTmpToken; 
  UINT16  wSenRequestParam;

  SCIB_TargetProc = TargetProc;
  SCIB_ProtocolFrame =  ProtocolFrame;

  SCIB_RX_TimerID = 0xFF;
  SCIB_TX_TimerID = 0xFF;


  SCIB_EVT_SCI_RXMSG = _SCI_RX_MSG;
  SCIB_EVT_SCI_RXERR = _SCI_RX_ERR;
  SCIB_EVT_SCI_TXMSG = _SCI_TX_MSG;
  SCIB_EVT_SCI_TXERR = _SCI_TX_ERR;
  SCIB_EVT_SCI_TXBUSY = _SCI_TX_BUSY;
  
 // leer byte de configuración para la comunicacion serie
  eeprom_read_buffer(EE_ADDR_CFG_SCIB, &tmp_cfgCode, 1);
  tmp_byteIdx = tmp_cfgCode & CFG_SCI_BAUDRATE_MASK;
  SCIB_SCIBD  = BaudCode2RegVal[tmp_byteIdx];
  SCIB_SCICR1 = 0x00;
  if ((tmp_cfgCode & CFG_SCI_PARITY_NONE) != CFG_SCI_PARITY_NONE){	  // Habilitar paridad
    SCIB_SCICR1 = SCI1CR1_PE_MASK;                    // 7 bits de datos + paridad = 8 bits
    if (!(tmp_cfgCode & CFG_SCI_DATABITS_MASK)){      // 8 bits de datos
      SCIB_SCICR1 |= SCI1CR1_M_MASK;                  // total 9 bits: 8 bits de datos, + paridad 
    }
    if (tmp_cfgCode & CFG_SCI_PARITY_ODD){
      SCIB_SCICR1 |= SCI1CR1_PT_MASK;                 //  paridad IMPAR
    }
  }
  
  SCIB_ConfigByte = tmp_cfgCode;
  SCIB_SCICR2 = 0x00; 
  
  eeprom_read_buffer(EE_ADDR_TMR_TXWAIT_SCIB, (UINT8 *)&SCIB_TMR_TX_TIMEOUT, 2);			// Tiempo de espera para transmitir una trama entera (2 BYTES)
  eeprom_read_buffer(EE_ADDR_TMR_RXWAIT_SCIB, (UINT8 *)&SCIB_TMR_RX_TIMEOUT, 2);			// Tiempo de espera para recibir una trama entera (2 BYTES)
  eeprom_read_buffer(EE_ADDR_TMR_DRIVE_SCIB, (UINT8 *)&SCIB_TMR_DRIVE_TIMEOUT, 2); 	// Tiempo antes de enviar datos despues de establecer DRIVE 485 (2 BYTES)
  
   
  RS485CFG_B_DE   = 1;              // control DE como salida, RS485_A
  RS485CFG_B_RE_  = 1;              // control RE# como salida, RS485_A
  RS485CFG_B_SEL  = 1;              // control del MUX para selección de modo, como salida  
  
  if (SCIB_ConfigByte & CFG_SCI_PHYMODE_MASK){            // modo RS485
    RS485_B_SEL     = MUX_SEL_RS485;  // MUX seleccionado para tomar señales del DS3695       
    RS485_B_RE_     = 0; 				      // RE# = 0 incializado para recibir
    RS485_B_DE      = 0;				      // DE = 0  incializado para recibir
  } 
  else{
    RS485_B_SEL     = MUX_SEL_RS232;  // MUX seleccionado para tomar señales del MAX232     
  }

  SCIB_CurrentTxToken = 0xFF;       // Ningún buffer transmitiéndose, habilitado para TX      
  StatisticData.SerialStat.MsgTxOk    = 0x0000;
  StatisticData.SerialStat.MsgRxOk    = 0x0000;
  StatisticData.SerialStat.MsgTxErr   = 0x0000;
  StatisticData.SerialStat.MsgRxErr   = 0x0000;
  SCIB_TxErrAcc = 0x00;
  SCIB_RxErrAcc = 0x00;
  SCIB_RxByteCount = 0x00;   
  SCIB_TxByteCount = 0x00;   
  SCIB_StateFlags  = 0x00;

  SCIB_SCICR2 = 0x00;  
//  SCIB_SCICR2 |=  SCI1CR2_RE_MASK ;			// Rx Enable
//  SCIB_SCICR2 |=  SCI1CR2_TE_MASK ; 		// Tx Enable


  // Si es Modbus SLAVE, pedir un buffer y quedar en recepcion
  if (SCIB_ProtocolFrame == SCI_FRAME_MODBUSRTU_SLV)
  {
      if ( ERROR == MemGetBuffer ( (void**)(&pTmpRxBuffer), &bTmpToken)){
          // TODO: ABR, implementar logs en FLASH
          //PutEventMsg (EVT_LOG_ID, PROC_DEBUG, OS_NOT_PROC, (UINT16)LOG_ID_TCP_NOBUFF);    
          MON_ResetIfNotConnected(0x26);
          return; 
      }  
      wSenRequestParam = 0;     // En la parte alta del parámetro se pasa el offset de transmisión
      wSenRequestParam <<= 8;
      wSenRequestParam |= bTmpToken;
      
      (void)MemSetUsedLen(bTmpToken, 0x08);   // Recibir por lo menos 8 bytes
      
      PutEventMsg (EVT_SCI_RXALL, PROC_SCIB, PROC_SCIB, wSenRequestParam );
  }
  
}


//--------------------------------------------------------------------------------------
//-- Funcion de Reinicialización en caso de fallas 
//
void SCIB_ReInit (BYTE TargetProc, BYTE  ProtocolFrame, BYTE _SCI_RX_MSG, BYTE _SCI_RX_ERR, BYTE _SCI_TX_MSG, BYTE _SCI_TX_ERR, BYTE _SCI_TX_BUSY)
{
  BYTE    tmp_cfgCode;	
  BYTE    tmp_byteIdx;	

  SCIB_TargetProc = TargetProc;
  SCIB_ProtocolFrame =  ProtocolFrame;

  SCIB_EVT_SCI_RXMSG = _SCI_RX_MSG;
  SCIB_EVT_SCI_RXERR = _SCI_RX_ERR;
  SCIB_EVT_SCI_TXMSG = _SCI_TX_MSG;
  SCIB_EVT_SCI_TXERR = _SCI_TX_ERR;
  SCIB_EVT_SCI_TXBUSY = _SCI_TX_BUSY;
  
 // leer byte de configuración para la comunicacion serie
  eeprom_read_buffer(EE_ADDR_CFG_SCIB, &tmp_cfgCode, 1);
  tmp_byteIdx = tmp_cfgCode & CFG_SCI_BAUDRATE_MASK;
  SCIB_SCIBD  = BaudCode2RegVal[tmp_byteIdx];

 // leer byte de configuración para la comunicacion serie
  eeprom_read_buffer(EE_ADDR_CFG_SCIB, &tmp_cfgCode, 1);
  tmp_byteIdx = tmp_cfgCode & CFG_SCI_BAUDRATE_MASK;
  SCIB_SCIBD  = BaudCode2RegVal[tmp_byteIdx];
  if ((tmp_cfgCode & CFG_SCI_PARITY_NONE) != CFG_SCI_PARITY_NONE){	  // Habilitar paridad
    SCIB_SCICR1 = SCI1CR1_PE_MASK;                    // 7 bits de datos + paridad = 8 bits
    if (!(tmp_cfgCode & CFG_SCI_DATABITS_MASK)){      // 8 bits de datos
      SCIB_SCICR1 |= SCI1CR1_M_MASK;                  // total 9 bits: 8 bits de datos, + paridad 
    }
    if (tmp_cfgCode & CFG_SCI_PARITY_ODD){
      SCIB_SCICR1 |= SCI1CR1_PT_MASK;                 //  paridad IMPAR
    }
  }
  
  SCIB_ConfigByte = tmp_cfgCode;
  SCIB_SCICR2 = 0x00;

  eeprom_read_buffer(EE_ADDR_TMR_TXWAIT_SCIB, (UINT8 *)&SCIB_TMR_TX_TIMEOUT, 2);			// Tiempo de espera para transmitir una trama entera (2 BYTES)
  eeprom_read_buffer(EE_ADDR_TMR_RXWAIT_SCIB, (UINT8 *)&SCIB_TMR_RX_TIMEOUT, 2);			// Tiempo de espera para recibir una trama entera (2 BYTES)
  eeprom_read_buffer(EE_ADDR_TMR_DRIVE_SCIB, (UINT8 *)&SCIB_TMR_DRIVE_TIMEOUT, 2); 	// Tiempo antes de enviar datos despues de establecer DRIVE 485 (2 BYTES)
  
  RS485CFG_B_DE   = 1;              // control DE como salida, RS485_A
  RS485CFG_B_RE_  = 1;              // control RE# como salida, RS485_A
  RS485CFG_B_SEL  = 1;              // control del MUX para selección de modo, como salida  
  
  if (SCIB_ConfigByte & CFG_SCI_PHYMODE_MASK){            // modo RS485
    RS485_B_SEL     = MUX_SEL_RS485;  // MUX seleccionado para tomar señales del DS3695       
    RS485_B_RE_     = 0; 				      // RE# = 0 incializado para recibir
    RS485_B_DE      = 0;				      // DE = 0  incializado para recibir
  } 
  else{
    RS485_B_SEL     = MUX_SEL_RS232;  // MUX seleccionado para tomar señales del MAX232     
  }

  StatisticData.SerialStat.MsgTxOk    = 0x0000;
  StatisticData.SerialStat.MsgRxOk    = 0x0000;
  StatisticData.SerialStat.MsgTxErr   = 0x0000;
  StatisticData.SerialStat.MsgRxErr   = 0x0000;

  SCIB_RxByteCount = 0x00;   
  SCIB_TxByteCount = 0x00;   
  SCIB_StateFlags  = 0x00;
  SCIB_CurrentTxToken = 0xFF;       // Ningún buffer transmitiéndose, habilitado para TX      
    
	CurrentProcessState [PROC_SCIB] = (EVENT_FUNCTION*)SCIBProc_IDLE;	
  
//  SCIB_SCICR2 |=  SCI1CR2_RE_MASK ;			// Rx Enable
//  SCIB_SCICR2 |=  SCI1CR2_TE_MASK ; 		// Tx Enable
  
}


////////////////////////////////////////////////////////////////////////////////////////////////
// SCI1 ISR
////////////////////////////////////////////////////////////////////////////////////////////////
//#pragma CODE_SEG NON_BANKED        

interrupt void near SCI0_isr (void)
{
  m_SCIB_SCISR1 = SCIB_SCISR1;
         
//--------------TRANSMISION-------------------------------------------------------   
  if (( m_SCIB_SCISR1 & SCI1SR1_TC_MASK ) && (SCIB_SCICR2 & SCI1CR2_TCIE_MASK))	        // si es interrupcion de TXC
  {
      if (!SCIB_TxByteCount)
      {
         SCIB_SCICR2 &= ~(SCI1CR2_SCTIE_MASK);	    // TIE Interrupt Disable
         SCIB_SCICR2 &= ~(SCI1CR2_TE_MASK ); 			  // TE Disable
         SCIB_SCICR2 &= ~(SCI1CR2_TCIE_MASK);       // TCIE Interrupt Disable      
         PutEventMsg (EVT_SCI_TXMSG, PROC_SCIB, PROC_SCIB, 0x00);    
      }
      else
      {
        SCIB_SCIDRL = *SCIB_FrameToTx;
        SCIB_FrameToTx++;
        SCIB_TxByteCount--; 
      }
      
  }
         
//--------------RECEPCION-------------------------------------------------------   
//		 
  if ((m_SCIB_SCISR1 & SCI1SR1_RDRF_MASK) && (SCIB_SCICR2 & SCI1CR2_RIE_MASK)){
       
    if (SCIB_RxByteCount)		                       // si quedan bytes por recibir
    {                       
      *SCIB_FrameToRx = SCIB_SCIDRL; 
      m_SCIB_RxMsgLen++;

      if ( SCIB_ProtocolFrame == SCI_FRAME_MODBUSRTU ) 	 // Delimitar frame ModbusRTU
      {
          if ( SCIB_RxByteIdx == 5 ) 
          {
              if ( m_SCIB_RxMsg_FC == 16 )		            // Write Multiple Registers
              {            
                SCIB_RxByteCount = *SCIB_FrameToRx;			  // Word count 
                SCIB_RxByteCount <<= 1;                   // x2 byte count
                SCIB_RxByteCount += 0x03;                 // CRC + bound effects
                SCIB_RxByteIdx = 10;                      // Para que no entre en los demás if's
              }
          }

          if ( SCIB_RxByteIdx == 2 ) 
          {
              if (( m_SCIB_RxMsg_FC == 4)||									 // Read Input Register
                  ( m_SCIB_RxMsg_FC == 3)||                  // Read Multiple Register
                  ( m_SCIB_RxMsg_FC == 2)||                  // Read Input Discrete
                  ( m_SCIB_RxMsg_FC == 100)||                // User defined FC
                  ( m_SCIB_RxMsg_FC == 1))                   // Read Coils
              {
                SCIB_RxByteCount = (UINT16)*SCIB_FrameToRx;		  // Byte count 
                SCIB_RxByteCount += 0x03;               // CRC + bound effects 
                SCIB_RxByteIdx = 10;                    // Para que no entre en los demás if's
              }
          }
          
          if ( SCIB_RxByteIdx == 1 )		              // Segundo Byte: Function Code
          {        
              m_SCIB_RxMsg_FC =  *SCIB_FrameToRx;
              if (( m_SCIB_RxMsg_FC == 5)||                // Write Coil
                  ( m_SCIB_RxMsg_FC == 6))									// Write Single Register
              {
                  SCIB_RxByteCount = 4;               // Bytes restantes de msg. de len fija 
                  SCIB_RxByteCount += 0x03;               // CRC + bound effects 
                  SCIB_RxByteIdx = 10;                // Para que no entre en los demás if's
              } 
              else if(m_SCIB_RxMsg_FC > 0x80)
              {
                  SCIB_RxByteCount = 1;               // Bytes restantes de msg. de len fija 
                  SCIB_RxByteCount += 0x03;               // CRC + bound effects 
                  SCIB_RxByteIdx = 10;                // Para que no entre en los demás if's              
              }

          }
          if ( SCIB_RxByteIdx == 0 )
          {	
            if ( SCIB_RX_TimerID == 0xFF){
            	// Si no se había incializado un timer de recepción, hacerlo al ingresar el primer byte
            	SCIB_RX_TimerID =  TMR_SetTimer ( SCIB_TMR_RX_TIMEOUT , PROC_SCIB, 0x00, FALSE);
            }
          }
      } 
      else if ( SCIB_ProtocolFrame  == SCI_FRAME_UNITELWAY )    // Delimitar Frame UNITELWAY
      {
      
          if ( ( SCIB_RxByteIdx > 3 ) && (SCIB_RxByteCount > 1) )
          {
              if ( ( *SCIB_FrameToRx == DLE_CHAR) && (SCIB_FrameToRx[m_SCIB_RxMsgLen-2] != DLE_CHAR) )
              {
                  SCIB_RxByteCount++;
              } 
          
          }
          
          if ( SCIB_RxByteIdx == 3 )		              // Cuarto Byte: LEN
          {        
              SCIB_RxByteCount = *SCIB_FrameToRx; 
              SCIB_RxByteCount++;                     //            
              if ( *SCIB_FrameToRx == DLE_CHAR)           
              {
                  SCIB_RxByteCount++;
              } 
          }      

          if ( SCIB_RxByteIdx == 1 )		              // Segundo Byte: STX
          {        
              if ( *SCIB_FrameToRx != STX_CHAR)           
              {
                  SCIB_SCICR2  &= ~(SCI1CR2_RE_MASK) ;		// Rx Disable
                  SCIB_SCICR2  &= ~(SCI1CR2_RIE_MASK);	  // Rx Interrupt Disable
                  PutEventMsg (EVT_SCI_RXMSG, PROC_SCIB, PROC_SCIB, 0x01);  // TERMINAR CON ERROR                                     
              } 
          }      
          if ( SCIB_RxByteIdx == 0 )
          {	
              if ( SCIB_RX_TimerID == 0xFF){
              	// Si no se había incializado un timer de recepción, hacerlo al ingresar el primer byte
              	SCIB_RX_TimerID =  TMR_SetTimer ( SCIB_TMR_RX_TIMEOUT , PROC_SCIB, 0x00, FALSE);
              }

              if ( *SCIB_FrameToRx != DLE_CHAR)           // Primer Byte debe ser DLE descartar previos
              {
                  SCIB_RxByteCount++;    
                  SCIB_RxByteIdx--;
                  SCIB_FrameToRx--; 
                  m_SCIB_RxMsgLen--;                                           
              } 
              
          }      
      }
      else if ( SCIB_ProtocolFrame  == SCI_FRAME_SPACOM )    // Delimitar Frame SPACOM
      {
      
          *SCIB_FrameToRx &= 0x7F;                      // SPA es siempre en 7 bits de datos
          
          if ( SCIB_RxByteIdx != 0 )
          {           
              if ( (*SCIB_FrameToRx == 0x0A)&&              //  cr lf 0x0D 0x0A final del frame
                   ( SCIB_RxByteIdx) )       
              {
                  SCIB_RxByteCount = 0x01;                  // terminar OK
              }
          }
/*
          if ( SCIB_RxByteIdx == 1 )		                // Segundo Byte: debe ser '<'
          {        
              if ( *SCIB_FrameToRx != '<')              // 
              {
                  SCIB_SCICR2  &= ~(SCI1CR2_RE_MASK) ;		// Rx Disable
                  SCIB_SCICR2  &= ~(SCI1CR2_RIE_MASK);	  // Rx Interrupt Disable
                  PutEventMsg (EVT_SCI_RXMSG, PROC_SCIB, PROC_SCIB, 0xFF);    // ERROR    
              }
          }
*/          
          if ( SCIB_RxByteIdx == 0 )
          {	
              if ( SCIB_RX_TimerID == 0xFF){
              	// Si no se había incializado un timer de recepción, hacerlo al ingresar el primer byte
              	SCIB_RX_TimerID =  TMR_SetTimer ( SCIB_TMR_RX_TIMEOUT , PROC_SCIB, 0x00, FALSE);
              }

              if ( *SCIB_FrameToRx != '<')           // Primer Byte debe ser '<' descartar previos
              {
                  SCIB_RxByteCount++;    
                  SCIB_RxByteIdx--;
                  SCIB_FrameToRx--;
              } 
              
          }

      } 
      else if ( SCIB_ProtocolFrame  == SCI_FRAME_MODBUSRTU_SLV ) 	 // Delimitar frame ModbusRTU Slave
      {
          if ( SCIB_RxByteIdx == 1 ) 
          {
              m_SCIB_RxMsg_FC =  *SCIB_FrameToRx;
              // TODO: analizar demas peticiones de escrituras, y rechazarlas   
              if ( m_SCIB_RxMsg_FC <= 6)				
              {
                  SCIB_RxByteCount = 0x07;                 // CRC + 6 data - 1 Rxed
                  SCIB_RxByteIdx = 10;                     // Para que no entre en los demás if's
              } 
              else
              {               
                  SCIB_RxByteCount = 1;               // Bytes restantes de msg. de len fija 
                  SCIB_RxByteIdx = 10;                // Para que no entre en los demás if's
                  SCIB_SCICR2  &= ~(SCI1CR2_RE_MASK) ;		// Rx Disable
                  SCIB_SCICR2  &= ~(SCI1CR2_RIE_MASK);	  // Rx Interrupt Disable
                  // TODO: cambiar por evento nuevo RXEND o RXERR
                  PutEventMsg (EVT_SCI_RXMSG, PROC_SCIB, PROC_SCIB, 0x01);    
                  return;
              }
          }

          if ( SCIB_RxByteIdx == 0 )
          {	
            if ( SCIB_RX_TimerID == 0xFF){
            	// Si no se había incializado un timer de recepción, hacerlo al ingresar el primer byte
            	SCIB_RX_TimerID =  TMR_SetTimer ( SCIB_TMR_RX_TIMEOUT , PROC_SCIB, 0x00, FALSE);
            }
          }
      }             
      
      SCIB_RxByteCount--;    
      SCIB_RxByteIdx++;
      SCIB_FrameToRx++;
                      
      if (!SCIB_RxByteCount)
      {
          SCIB_SCICR2  &= ~(SCI1CR2_RE_MASK) ;		// Rx Disable
          SCIB_SCICR2  &= ~(SCI1CR2_RIE_MASK);	  // Rx Interrupt Disable
          PutEventMsg (EVT_SCI_RXMSG, PROC_SCIB, PROC_SCIB, 0x00);    
      }
    }
    else{
      SCIB_TmpVar  = SCIB_SCIDRL;	                // Flush buffer
    } 
  }
      
   
//--------------ERROR DE COMUNICACION--------------------------------------------   
//
//  SCIB_SCICR2 &= ~(SCI1CR2_RE_MASK) ;			            // Rx Disable
//  SCIB_SCICR2 &= ~(SCI1CR2_RIE_MASK);			            // Rx Interrupt Disable
//  SCIB_SCICR2 &= ~(SCI1CR2_TE_MASK ); 			          // Tx Disable
//  SCIB_SCICR2 &= ~(SCI1CR2_TCIE_MASK);                // TxC Interrupt Disable      
//  SCIB_ReceiveFrame (3, );

}

#pragma CODE_SEG DEFAULT



