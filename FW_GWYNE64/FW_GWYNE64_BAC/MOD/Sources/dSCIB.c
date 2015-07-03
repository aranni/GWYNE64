#include <stdio.h>

#include "MC9S12NE64.h"
#include "datatypes.h"

#include "Commdef.h"
#include "dSCIB.h"
#include "global.h"


#pragma CODE_SEG NON_BANKED        

///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_SCIB
///////////////////////////////////////////////////////////////////////////////
//
#ifdef SCIB_RS485_MODE        // Si se configura al puerto en modo RS485

const EVENT_FUNCTION	SCIBProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_SCIBIdleTimeout },
{EVT_485_TXREQ 	      , f_SCIBSendRequest	},
{EVT_485_RXREQ 	      , f_SCIBReceiveMsg 	},
{EVT_OTHER_MSG     		, f_Consume         }};

const EVENT_FUNCTION	SCIBProc_TOTRANSMIT [] =
{
{EVT_TMR_TIMEOUT 	    , f_SCIBToTxTimeout	},
{EVT_OTHER_MSG     		, f_Consume         }};

const EVENT_FUNCTION	SCIBProc_TRANSMITING [] =
{
{EVT_TMR_TIMEOUT 	    , f_SCIBTxTimeout	  },
{EVT_485_TXMSG 	      , f_SCIBTxFrame	    },
{EVT_OTHER_MSG     		, f_Consume         }};

const EVENT_FUNCTION	SCIBProc_RECEIVING [] =
{
{EVT_TMR_TIMEOUT 	    , f_SCIBRxTimeout	  },
{EVT_485_RXMSG 	      , f_SCIBRxFrame	    },
{EVT_OTHER_MSG     		, f_Consume         }};

#else		  //  SCIB_RS485_MODE

const EVENT_FUNCTION	SCIBProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_SCIBIdleTimeout },
{EVT_OTHER_MSG     		, f_Consume         }};

#endif		//  SCIB_RS485_MODE

////////////////////////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_SCIB
////////////////////////////////////////////////////////////////////////////////////////////////

//-------------- estado : IDLE ----------------------------------------------------------------
//
void f_SCIBIdleTimeout (void) {

// UINT8*  pTmpRxBuffer;
//  UINT8   bTmpToken;    
  
  PutEventMsg (EVT_232_RXMSG, PROC_BAC, PROC_SCIB, (0x0000 | SCIB_CurrentRxToken));     // Informar error en recepcion

//  (void)MemFreeBuffer (SCIB_CurrentRxToken);  
//  (void)MemGetBuffer ( (void**)(&pTmpRxBuffer), &bTmpToken);  
//  SCIB_ReceiveFrame (250, bTmpToken);
  
//  SCIB_ReceiveFrame (250, SCIB_CurrentRxToken);
																											 
  return; 
}


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>> Funciones y Estados adicionales para el caso half-duplex RS485
//
#ifdef SCIB_RS485_MODE        // Si se configura al puerto en modo RS485

void f_SCIBSendRequest (void)
{
  UINT16  wTmpWordCalc;   

  RS485_B_RE_ = 1; 				        // TX Mode
  RS485_B_DE  = 1;				        // TX Mode
  
  LED_GREEN_ON;
  
  wTmpWordCalc = (CurrentMsg.Param & 0xFF00);   // La longitud en la parte alta del parámetro
  wTmpWordCalc >>= 8;
  SCIB_TxByteCount = (UINT8) wTmpWordCalc;
  
  // El token del buffer está en la parte baja del parámetro del mensaje
  SCIB_CurrentTxToken = (UINT8)CurrentMsg.Param;
  (void)MemGetBufferPtr ( SCIB_CurrentTxToken, (void**) &(SCIB_FrameToTx));

	SCIB_TX_TimerID =  TMR_SetTimer (20, PROC_SCIB, 0x00, FALSE);															 
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIBProc_TOTRANSMIT;  // Esperar establecimiento de TX
 
	return;
}


void f_SCIBReceiveMsg (void)
{
  UINT16  wTmpWordCalc;   

  RS485_B_RE_ = 0; 				    // RX Mode
  RS485_B_DE  = 0;				    // RX Mode

  wTmpWordCalc = (CurrentMsg.Param & 0xFF00);   // La longitud en la parte alta del parámetro
  wTmpWordCalc <<= 8;
  SCIB_RxByteCount = (UINT8) wTmpWordCalc;			// Cantidad de bytes pendientes de recepcion
	SCIB_RxByteIdx = 0x00;                        // Indice contador de Bytes recibidos
  
  // El token del buffer está en la parte baja del parámetro del mensaje
  SCIB_CurrentRxToken = (UINT8)CurrentMsg.Param;
  (void)MemGetBufferPtr ( SCIB_CurrentRxToken, (void**) &(SCIB_FrameToRx));

  SCIB_TmpVar  = SCIB_SCISR1;	          // Clear pending flag
  SCIB_TmpVar  = SCIB_SCIDRL;	          // Flush buffer
  SCIB_SCICR2 |=  SCI1CR2_RE_MASK ;			// Rx Enable
  SCIB_SCICR2 |=	SCI1CR2_RIE_MASK;		  // Rx Interrupt Enable
	
	SCIB_RX_TimerID =  TMR_SetTimer ( TMR_RX_TIMEOUT , PROC_SCIB, 0x00, FALSE);
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIBProc_RECEIVING; 

	return;
}

//---------- estado : TOTRANSMIT  ----------------------------------------------------------
//
void f_SCIBToTxTimeout (void)
{

  SCIB_TmpVar  = SCIB_SCIDRL;	    // Flush buffer
  SCIB_TmpVar  = SCIB_SCISR1;	    // Clear pending flag

  SCIB_SCIDRL = *SCIB_FrameToTx;
  SCIB_TxByteCount--;      
  SCIB_FrameToTx++;
  
//  SCIB_SCICR2 |=  SCI1CR2_SCTIE_MASK;	      // Tx Interrupt Enable
  SCIB_SCICR2 |=  SCI1CR2_TCIE_MASK;          // TxC Interrupt Enable
  SCIB_SCICR2 |=  SCI1CR2_TE_MASK ; 				  // Tx Enable

	SCIB_TX_TimerID =  TMR_SetTimer ( TMR_TX_TIMEOUT , PROC_SCIB, 0x00, FALSE);
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIBProc_TRANSMITING; 
  
  return;										
}

//---------- estado : TRANSMITTING  ----------------------------------------------------------
//
void f_SCIBTxTimeout (void)								
{
  LED_GREEN_OFF;

  RS485_B_RE_ = 1; 				    // TX Mode
  RS485_B_DE  = 1;				    // TX Mode

  SCIB_SCICR2 &= ~(SCI1CR2_TE_MASK ); 			  // Tx Disable
//  SCIB_SCICR2 &= ~(SCI1CR2_SCTIE_MASK);	    // Tx Interrupt Disable
  SCIB_SCICR2 &= ~(SCI1CR2_TCIE_MASK);        // TxC Interrupt Disable      
  SCIB_TxByteCount = 0x00;   
  
// Acciones correctivas.. error de transmision  
  if (  ++SCIB_TxErrAcc >= MAX_SERIAL_TXERR){
#ifdef MON_SERIAL_ERR_RESET_ENABLE
        MON_ResetIfNotConnected();
#endif
  }

  PutEventMsg (EVT_485_TXMSG, PROC_BAC, PROC_SCIB, (0x0200 | SCIB_CurrentTxToken));     // Informar error en transmisión
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIBProc_IDLE;  // Volver a estado IDLE
}

void f_SCIBTxFrame (void)
{
  TMR_FreeTimer (SCIB_TX_TimerID);
//
  SCIB_RxByteCount = (UINT8) MSG_MAX_BAC_LEN;	// Cantidad de bytes pendientes de recepcion
	SCIB_RxByteIdx = 0x00;                      // Indice contador de Bytes recibidos
	
//---------------------------  
// Pedir un buffer de recepción 
//  (void)MemGetBuffer ( (void**) &(SCIB_FrameToRx), &SCIB_CurrentRxToken);
//---------------------------  
// Recuperar puntero al buffer de transmisión que se reutilizará para recepción 
  SCIB_CurrentRxToken = SCIB_CurrentTxToken;  
  if (MemGetBufferPtr( SCIB_CurrentRxToken ,  (void**) &(SCIB_FrameToRx) ) != OK ){    
  // ABR: control de errores
//    return;   
  }
//---------------------------  
  
//
  SCIB_TmpVar  = SCIB_SCISR1;	          // Clear pending flag
  SCIB_TmpVar  = SCIB_SCIDRL;	          // Flush buffer
  SCIB_SCICR2 |=  SCI1CR2_RE_MASK ;			// Rx Enable
  SCIB_SCICR2 |=	SCI1CR2_RIE_MASK;			// Rx Interrupt Enable
	
//  No envío mensaje ahora porque se reutiliza el buffer de transmisión.
//  PutEventMsg (EVT_485_TXMSG, PROC_BAC, PROC_SCIB, SCIB_CurrentTxToken);     // Informar error en transmisión

	SCIB_RX_TimerID =  TMR_SetTimer ( TMR_RX_TIMEOUT , PROC_SCIB, 0x00, FALSE);

  LED_GREEN_OFF;
  LED_RED_ON;
// Pasar a modo recepción..
  RS485_B_DE  = 0;				    // RX Mode
  RS485_B_RE_ = 0; 				    // RX Mode  
//
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIBProc_RECEIVING; 
//  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIBProc_IDLE; 
  
}

//---------- estado : RECEIVING  ------------------------------------------------------------

void f_SCIBRxTimeout (void)
{
  LED_RED_OFF;
  
  RS485_B_RE_ = 1; 				    // TX Mode
  RS485_B_DE  = 1;				    // TX Mode
  
  SCIB_TmpVar  = SCIB_SCISR1;	              // Clear pending flag
  SCIB_TmpVar  = SCIB_SCIDRL;	              // Flush buffer
  SCIB_SCICR2  &= ~(SCI1CR2_RE_MASK) ;			// Rx Disable
  SCIB_SCICR2  &= ~(SCI1CR2_RIE_MASK);			// Rx Interrupt Disable
  SCIB_RxByteCount = 0;   
  
// Acciones correctivas.. error de recepcion
  if (  ++SCIB_RxErrAcc >= MAX_SERIAL_RXERR){
#ifdef MON_SERIAL_ERR_RESET_ENABLE
        MON_ResetIfNotConnected();
#endif																				
  }

  PutEventMsg (EVT_485_RXMSG, PROC_BAC, PROC_SCIB, (0x0100 | SCIB_CurrentRxToken));     // Informar error en recepcion
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIBProc_IDLE;  // volver a estado IDLE

}

void f_SCIBRxFrame (void)
{
  LED_RED_OFF;

  RS485_B_RE_ = 1; 				    // TX Mode
  RS485_B_DE  = 1;				    // TX Mode

  TMR_FreeTimer (SCIB_RX_TimerID);
  SCIB_RxErrAcc = 0x00;

  PutEventMsg (EVT_485_RXMSG, PROC_BAC, PROC_SCIB, SCIB_CurrentRxToken);     // Informar recepción OK

  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIBProc_IDLE;  // volver a estado IDLE

}

#endif		//  SCIB_RS485_MODE
//
//>> Funciones y Estados adicionales para el caso half-duplex RS485
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>



////////////////////////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
////////////////////////////////////////////////////////////////////////////////////////////////
//

//--------------------------------------------------------------------------------------
//-- Funcion de Requerimiento de Transmisión Inmediato a partir del token del buffer
//
RETCODE SCIB_TransmitFrame ( UINT16 TxLen, UINT8 TxMsgToken) 
{
 
  if ( SCIB_TxByteCount ) return ERROR;   // Si hay transmisión en curso rechazar..

#ifdef SCIB_RS485_MODE        // Si se configura al puerto en modo RS485
  RS485_B_RE_ = 1; 				    // TX Mode
  RS485_B_DE  = 1;				    // TX Mode
#endif

  LED_GREEN_ON;
  
// ABR : 28-SEP-2007
//  SCIB_TmpVar  = SCIB_SCIDRL;	    // Flush buffer
//  SCIB_TmpVar  = SCIB_SCISR1;	    // Clear pending flag
  
  SCIB_TxByteCount = TxLen;
  SCIB_CurrentTxToken = TxMsgToken;
  (void)MemGetBufferPtr ( SCIB_CurrentTxToken, (void**) &(SCIB_FrameToTx));
  
//  SCIB_SCICR2 |=  SCI1CR2_TE_MASK ; 				// Tx Enable

#if DEBUG_AT(0)
		  (void)sprintf((char *)udp_buf,"SCIB_Data Len: %d, Last:%X \n",
		                (BYTE)SCIB_TxByteCount,
		                (BYTE)SCIB_FrameToTx[SCIB_TxByteCount-1]);
      udp_data_to_send = TRUE;	  
#endif  		

#ifdef SCIB_RS485_MODE        // Si se configura al puerto en modo RS485
	SCIB_TX_TimerID =  TMR_SetTimer ( TMR_TX_TIMEOUT , PROC_SCIB, 0x00, FALSE);
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIBProc_IDLE; 
#endif		//  SCIB_RS485_MODE
										
//  SCIB_CurrentTxAddres = *SCIB_FrameToTx;

  SCIB_TmpVar  = SCIB_SCIDRL;	    // Flush buffer
  SCIB_TmpVar  = SCIB_SCISR1;	    // Clear pending flag

  SCIB_SCIDRL = *SCIB_FrameToTx;
  SCIB_TxByteCount--;      
  SCIB_FrameToTx++;
  
  SCIB_SCICR2 |=  SCI1CR2_SCTIE_MASK;	      // Tx Interrupt Enable
  SCIB_SCICR2 |=  SCI1CR2_TCIE_MASK;          // TxC Interrupt Enable
  SCIB_SCICR2 |=  SCI1CR2_TE_MASK ; 				  // Tx Enable
										
	return OK; 
  
}

//--------------------------------------------------------------------------------------
//-- Funcion de Requerimiento de Transmisión Inmediato a partir del puntero al mensaje
//
RETCODE SCIB_TransmitBuffer ( UINT16 TxLen, UINT8* TxMsgBuffer) 
{
 
  if ( SCIB_TxByteCount ) return ERROR;   // Si hay transmisión en curso rechazar..

#ifdef SCIB_RS485_MODE        // Si se configura al puerto en modo RS485
  RS485_B_RE_ = 1; 				    // TX Mode
  RS485_B_DE  = 1;				    // TX Mode
#endif

  LED_GREEN_ON;
   
  SCIB_TxByteCount = TxLen;
  SCIB_CurrentTxToken = 0xFF;
  SCIB_FrameToTx = TxMsgBuffer;
  
#if DEBUG_AT(0)
		  (void)sprintf((char *)udp_buf,"SCIB_Data Len: %d, Last:%X \n",
		                (BYTE)SCIB_TxByteCount,
		                (BYTE)SCIB_FrameToTx[SCIB_TxByteCount-1]);
      udp_data_to_send = TRUE;	  
#endif  		

#ifdef SCIB_RS485_MODE        // Si se configura al puerto en modo RS485
	SCIB_TX_TimerID =  TMR_SetTimer ( TMR_TX_TIMEOUT , PROC_SCIB, 0x00, FALSE);
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIBProc_IDLE; 
#endif		//  SCIB_RS485_MODE
										
  SCIB_TmpVar  = SCIB_SCIDRL;	    // Flush buffer
  SCIB_TmpVar  = SCIB_SCISR1;	    // Clear pending flag

  SCIB_SCIDRL = *SCIB_FrameToTx;
  SCIB_TxByteCount--;      
  SCIB_FrameToTx++;
  
  SCIB_SCICR2 |=  SCI1CR2_SCTIE_MASK;	      // Tx Interrupt Enable
  SCIB_SCICR2 |=  SCI1CR2_TCIE_MASK;          // TxC Interrupt Enable
  SCIB_SCICR2 |=  SCI1CR2_TE_MASK ; 				  // Tx Enable
										
	return OK; 
  
}


//--------------------------------------------------------------------------------------
//-- Funcion de acondicionamiento de estado de recepción
//
void SCIB_ReceiveFrame (  UINT16 RxLen, UINT8 RxMsgToken)
{
  
#ifdef SCIB_RS485_MODE        // Si se configura al puerto en modo RS485
  RS485_B_RE_ = 0; 				    // RX Mode
  RS485_B_DE  = 0;				    // RX Mode
#endif

  SCIB_RxByteCount = RxLen;			    // Cantidad de bytes pendientes de recepcion
	SCIB_RxByteIdx = 0x00;            // Indice contador de Bytes recibidos
  
  SCIB_CurrentRxToken = RxMsgToken;
  (void)MemGetBufferPtr ( SCIB_CurrentRxToken, (void**) &(SCIB_FrameToRx));


  SCIB_TmpVar  = SCIB_SCISR1;	          // Clear pending flag
  SCIB_TmpVar  = SCIB_SCIDRL;	          // Flush buffer
  SCIB_SCICR2 |=  SCI1CR2_RE_MASK ;			// Rx Enable
  SCIB_SCICR2 |=	SCI1CR2_RIE_MASK;		  // Rx Interrupt Enable

	SCIB_RxByteIdx = 0x00;
	SCIB_LastRxByte = 0x00;
	
//  SCIB_RX_FrameTimerID =  TMR_SetTimer ( 10000 , PROC_SCIB, 0x00, FALSE);
	
#ifdef SCIB_RS485_MODE        // Si se configura al puerto en modo RS485
	SCIB_RX_TimerID =  TMR_SetTimer ( TMR_RX_TIMEOUT , PROC_SCIB, 0x00, FALSE);
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIBProc_IDLE; 
#endif		//  SCIB_RS485_MODE

	return;
	
}


//--------------------------------------------------------------------------------------
//-- Funcion de Inicialización
//
void SCIB_Init (void)
{
  BYTE    tmp_cfgCode;	
  BYTE    tmp_byteIdx;	

//  SCIB_SCIBD  = SCIB_BAUD_DIV;
//  SCIB_SCICR1 = SCI1CR1_M_MASK | SCI1CR1_PE_MASK;      // 0x12 8-P-1

 // leer byte de configuración para la comunicacion serie
  eeprom_read_buffer(EE_ADDR_CFG_SCIB, &tmp_cfgCode, 1);
  tmp_byteIdx = tmp_cfgCode & CFG_SCI_BAUDRATE_MASK;
  SCIB_SCIBD  = BaudCode2RegVal[tmp_byteIdx];
  if ((tmp_cfgCode & CFG_SCI_PARITY_NONE) != CFG_SCI_PARITY_NONE){
    SCIB_SCICR1 = SCI1CR1_M_MASK | SCI1CR1_PE_MASK;      // 8 bits de datos, paridad habilitada
    if (tmp_cfgCode & CFG_SCI_PARITY_ODD){
      SCIB_SCICR1 |= SCI1CR1_PT_MASK;                    //  paridad IMPAR
    }
  }

  SCIB_SCICR2 = 0x00;
  
  RS485CFG_B_DE   = 1;              // control DE como salida, RS485_A
  RS485CFG_B_RE_  = 1;              // control RE# como salida, RS485_A
  
#ifdef SCIB_RS485_MODE              // Si se configura al puerto en modo RS485
  RS485_B_RE_     = 1; 				      // RE# = 1 incializado para transmitir
  RS485_B_DE      = 1;				      // DE = 1  incializado para transmitir
#endif

  RS485CFG_B_SEL  = 1;              // control del MUX para selección de modo, como salida  
#ifdef SCIB_RS485_MODE              // Si se configura al puerto en modo RS485
  RS485_B_SEL     = MUX_SEL_RS485;  // MUX seleccionado para tomar señales del DS3695       
  RS485_B_RE_     = 1; 				      // RE# = 1 incializado para transmitir
  RS485_B_DE      = 1;				      // DE = 1  incializado para transmitir
#else  
  RS485_B_SEL     = MUX_SEL_RS232;  // MUX seleccionado para tomar señales del MAX232     
#endif

  SCIB_RxByteCount = 0x00;   
  SCIB_TxByteCount = 0x00;   
  
  SCIB_TxErrAcc = 0x00;
  SCIB_RxErrAcc = 0x00;
  SCIB_CurrentTxToken = 0xFF;       // Ningún buffer transmitiéndose, habilitado para TX      

  SCIB_SCICR2 = 0x00;  
  SCIB_SCICR2 |=  SCI1CR2_RE_MASK ;			// Rx Enable
  SCIB_SCICR2 |=  SCI1CR2_TE_MASK ; 		// Tx Enable
   
}


//--------------------------------------------------------------------------------------
//-- Funcion de Reinicialización en caso de fallas 
//
void SCIB_ReInit (void)
{
  BYTE    tmp_cfgCode;	
  BYTE    tmp_byteIdx;	

//  SCIB_SCIBD  = SCIB_BAUD_DIV;
//  SCIB_SCICR1 = SCI1CR1_M_MASK | SCI1CR1_PE_MASK;      // 0x12 8-P-1

 // leer byte de configuración para la comunicacion serie
  eeprom_read_buffer(EE_ADDR_CFG_SCIB, &tmp_cfgCode, 1);
  tmp_byteIdx = tmp_cfgCode & CFG_SCI_BAUDRATE_MASK;
  SCIB_SCIBD  = BaudCode2RegVal[tmp_byteIdx];
  if ((tmp_cfgCode & CFG_SCI_PARITY_NONE) != CFG_SCI_PARITY_NONE){
    SCIB_SCICR1 = SCI1CR1_M_MASK | SCI1CR1_PE_MASK;      // 8 bits de datos, paridad habilitada
    if (tmp_cfgCode & CFG_SCI_PARITY_ODD){
      SCIB_SCICR1 |= SCI1CR1_PT_MASK;                    //  paridad IMPAR
    }
  }

  SCIB_SCICR2 = 0x00;
  
  RS485CFG_B_DE   = 1;              // control DE como salida, RS485_A
  RS485CFG_B_RE_  = 1;              // control RE# como salida, RS485_A
  
#ifdef SCIB_RS485_MODE              // Si se configura al puerto en modo RS485
  RS485_B_RE_     = 1; 				      // RE# = 1 incializado para transmitir
  RS485_B_DE      = 1;				      // DE = 1  incializado para transmitir
#endif

  RS485CFG_B_SEL  = 1;              // control del MUX para selección de modo, como salida  
#ifdef SCIB_RS485_MODE              // Si se configura al puerto en modo RS485
  RS485_B_SEL     = MUX_SEL_RS485;  // MUX seleccionado para tomar señales del DS3695       
  RS485_B_RE_     = 1; 				      // RE# = 1 incializado para transmitir
  RS485_B_DE      = 1;				      // DE = 1  incializado para transmitir
#else  
  RS485_B_SEL     = MUX_SEL_RS232;  // MUX seleccionado para tomar señales del MAX232     
#endif


  SCIB_RxByteCount = 0x00;   
  SCIB_TxByteCount = 0x00;   
  SCIB_CurrentTxToken = 0xFF;       // Ningún buffer transmitiéndose, habilitado para TX      
    
	CurrentProcessState [PROC_SCIB] = (EVENT_FUNCTION*)SCIBProc_IDLE;	
	
  SCIB_TmpVar  = SCIB_SCIDRL;	                // Flush buffer  
//  SCIB_SCICR2 |=  SCI1CR2_RE_MASK ;			// Rx Enable
//  SCIB_SCICR2 |=  SCI1CR2_TE_MASK ; 		// Tx Enable
  
}



////////////////////////////////////////////////////////////////////////////////////////////////
// SCI1 ISR
////////////////////////////////////////////////////////////////////////////////////////////////
//#pragma CODE_SEG NON_BANKED        

interrupt void near SCI0_isr (void){

//  SCIB_TmpVar	 = SCIB_SCISR1;  
  m_SCIB_SCISR1 = SCIB_SCISR1;
  
         
//--------------RECEPCION-------------------------------------------------------   
		 
  if ((m_SCIB_SCISR1 & SCI1SR1_RDRF_MASK) && (SCIB_SCICR2 & SCI1CR2_RIE_MASK)){	        // si es interrupcion de RX 
       
    if (SCIB_RxByteCount){                        // si quedan bytes por recibir
      *SCIB_FrameToRx = SCIB_SCIDRL; 
/*
      if ( SCIB_RxByteIdx == 1 ){		              // segundo byte: verificar START_L word
         if ( *SCIB_FrameToRx != 0x10){           // Verificar START_L word = 0x64
            SCIB_SCICR2  &= ~(SCI1CR2_RE_MASK) ;  // Rx Disable
            SCIB_SCICR2  &= ~(SCI1CR2_RIE_MASK);	// Rx Interrupt Disable            
         } 
         SCIB_RxByteIdx++;
         LED_RED_ON;
      }
*/
      if (SCIB_RxByteIdx == 0x00){                // primer byte recibido ?
         if ( *SCIB_FrameToRx != DLE_CHAR){       // Verificar START = DLE
            return;                               // Descartar y seguir esperando
         } 
         else{            
              LED_RED_ON;
              SCIB_RX_FrameTimerID =  TMR_SetTimer ( 5000 , PROC_SCIB, 0x00, FALSE);
//            SCIB_RxByteIdx++;
         }  	         
      }

      if ((SCIB_LastRxByte==0x10)&&(*SCIB_FrameToRx == 0x03)){               // Flag ETX : End of Transmission
          SCIB_RxByteCount = 3;                   // Recibir 2 bytes más del checksum
      }
      
       
      SCIB_LastRxByte = *SCIB_FrameToRx;
       
      SCIB_FrameToRx++;
      SCIB_RxByteCount--;    
      SCIB_RxByteIdx++;
                
      if (!SCIB_RxByteCount){
          SCIB_SCICR2  &= ~(SCI1CR2_RE_MASK) ;		// Rx Disable
          SCIB_SCICR2  &= ~(SCI1CR2_RIE_MASK);	  // Rx Interrupt Disable
          TMR_FreeTimer (SCIB_RX_FrameTimerID);    
          
          PutEventMsg (EVT_232_RXMSG, PROC_BAC, PROC_SCIB, (UINT16)((SCIB_RxByteIdx<<8)|SCIB_CurrentRxToken));    
          LED_RED_OFF;
      }
    }
    else{
      SCIB_TmpVar  = SCIB_SCIDRL;	                // Flush buffer
    } 
//    return;   // ABR : 28-SEP-2007
  }
      
//--------------TRANSMISION-------------------------------------------------------   
/*     
      if (( m_SCIB_SCISR1 & SCI1SR1_TC_MASK ) && (SCIB_SCICR2 & SCI1CR2_TCIE_MASK))	        // si es interrupcion de TXC
      {
         SCIB_SCICR2 &= ~(SCI1CR2_SCTIE_MASK);	    // TIE Interrupt Disable
         SCIB_SCICR2 &= ~(SCI1CR2_TE_MASK ); 			  // TE Disable
         SCIB_SCICR2 &= ~(SCI1CR2_TCIE_MASK);        // TCIE Interrupt Disable      
         PutEventMsg (EVT_232_TXMSG, PROC_DNPLOW, PROC_SCIB, (UINT16)SCIB_CurrentTxToken);    
         LED_GREEN_OFF;
      } 
      SCIB_TmpVar  = SCIB_SCIDRL;	                // Flush buffer
*/      
//  if (( m_SCIB_SCISR1 & SCI1SR1_TDRE_MASK ) && (SCIB_SCICR2 & SCI1CR2_SCTIE_MASK))	  // si es interrupcion de TX
  if (( m_SCIB_SCISR1 & SCI1SR1_TC_MASK ) && (SCIB_SCICR2 & SCI1CR2_TCIE_MASK))	        // si es interrupcion de TXC
  {
      if (!SCIB_TxByteCount)
      {
         SCIB_SCICR2 &= ~(SCI1CR2_SCTIE_MASK);	    // TIE Interrupt Disable
         SCIB_SCICR2 &= ~(SCI1CR2_TE_MASK ); 			  // TE Disable
         SCIB_SCICR2 &= ~(SCI1CR2_TCIE_MASK);       // TCIE Interrupt Disable      
         PutEventMsg (EVT_CPY_TXMSG, PROC_BAC, PROC_SCIB, SCIB_CurrentTxToken);    
         LED_GREEN_OFF;
      }
      else
      {
        SCIB_SCIDRL = *SCIB_FrameToTx;
        SCIB_FrameToTx++;
        SCIB_TxByteCount--; 
      }
  }
  
   
//--------------ERROR DE COMUNICACION--------------------------------------------   

//  SCIB_SCICR2 &= ~(SCI1CR2_RE_MASK) ;			            // Rx Disable
//  SCIB_SCICR2 &= ~(SCI1CR2_RIE_MASK);			            // Rx Interrupt Disable
//  SCIB_SCICR2 &= ~(SCI1CR2_TE_MASK ); 			          // Tx Disable
//  SCIB_SCICR2 &= ~(SCI1CR2_TCIE_MASK);                // TxC Interrupt Disable      

//  TMR_FreeTimer (SCIB_RX_FrameTimerID);                    
//  SCIB_ReceiveFrame (3, );

}

#pragma CODE_SEG DEFAULT


