#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "MC9S12NE64.h"
#include "datatypes.h"

#include "Commdef.h"
#include "dSCIA.h"
#include "global.h"

//#define  DEBUG_SIM_BACFRAME 

#pragma CODE_SEG NON_BANKED        

///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_SCIA
///////////////////////////////////////////////////////////////////////////////
//
#ifdef SCIA_RS485_MODE        // Si se configura al puerto en modo RS485

const EVENT_FUNCTION	SCIAProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_SCIAIdleTimeout },
{EVT_485_TXREQ 	      , f_SCIASendRequest	},
{EVT_485_RXREQ 	      , f_SCIAReceiveMsg 	},
{EVT_OTHER_MSG     		, f_Consume         }};

const EVENT_FUNCTION	SCIAProc_TOTRANSMIT [] =
{
{EVT_TMR_TIMEOUT 	    , f_SCIAToTxTimeout	},
{EVT_OTHER_MSG     		, f_Consume         }};

const EVENT_FUNCTION	SCIAProc_TRANSMITING [] =
{
{EVT_TMR_TIMEOUT 	    , f_SCIATxTimeout	  },
{EVT_485_TXMSG 	      , f_SCIATxFrame	    },
{EVT_OTHER_MSG     		, f_Consume         }};

const EVENT_FUNCTION	SCIAProc_RECEIVING [] =
{
{EVT_TMR_TIMEOUT 	    , f_SCIARxTimeout	  },
{EVT_485_RXMSG 	      , f_SCIARxFrame	    },
{EVT_OTHER_MSG     		, f_Consume         }};

#else		  //  SCIA_RS485_MODE

const EVENT_FUNCTION	SCIAProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_SCIAIdleTimeout },
{EVT_OTHER_MSG     		, f_Consume         }};

#endif		//  SCIA_RS485_MODE

////////////////////////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_SCIA
////////////////////////////////////////////////////////////////////////////////////////////////

//-------------- estado : IDLE ----------------------------------------------------------------
//
void f_SCIAIdleTimeout (void) {

    //PutEventMsg (EVT_232_RXMSG, PROC_BAC, PROC_SCIA, (0x0000 | SCIA_CurrentRxToken));     // Informar error en recepcion

  return; 
}


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>> Funciones y Estados adicionales para el caso half-duplex RS485
//
#ifdef SCIA_RS485_MODE        // Si se configura al puerto en modo RS485

void f_SCIASendRequest (void)
{
  UINT16  wTmpWordCalc;   

  RS485_A_RE_ = 1; 				        // TX Mode
  RS485_A_DE  = 1;				        // TX Mode
  
  LED_GREEN_ON;
  
  wTmpWordCalc = (CurrentMsg.Param & 0xFF00);   // La longitud en la parte alta del parámetro
  wTmpWordCalc >>= 8;
  SCIA_TxByteCount = (UINT8) wTmpWordCalc;
  
  // El token del buffer está en la parte baja del parámetro del mensaje
  SCIA_CurrentTxToken = (UINT8)CurrentMsg.Param;
  (void)MemGetBufferPtr ( SCIA_CurrentTxToken, (void**) &(SCIA_FrameToTx));

	SCIA_TX_TimerID =  TMR_SetTimer (20, PROC_SCIA, 0x00, FALSE);															 
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIAProc_TOTRANSMIT;  // Esperar establecimiento de TX
 
	return;
}


void f_SCIAReceiveMsg (void)
{
  UINT16  wTmpWordCalc;   

  RS485_A_RE_ = 0; 				    // RX Mode
  RS485_A_DE  = 0;				    // RX Mode

  wTmpWordCalc = (CurrentMsg.Param & 0xFF00);   // La longitud en la parte alta del parámetro
  wTmpWordCalc <<= 8;
  SCIA_RxByteCount = (UINT8) wTmpWordCalc;			// Cantidad de bytes pendientes de recepcion
	SCIA_RxByteIdx = 0x00;                        // Indice contador de Bytes recibidos
  
  // El token del buffer está en la parte baja del parámetro del mensaje
  SCIA_CurrentRxToken = (UINT8)CurrentMsg.Param;
  (void)MemGetBufferPtr ( SCIA_CurrentRxToken, (void**) &(SCIA_FrameToRx));

  SCIA_TmpVar  = SCIA_SCISR1;	          // Clear pending flag
  SCIA_TmpVar  = SCIA_SCIDRL;	          // Flush buffer
  SCIA_SCICR2 |=  SCI1CR2_RE_MASK ;			// Rx Enable
  SCIA_SCICR2 |=	SCI1CR2_RIE_MASK;		  // Rx Interrupt Enable
	
	SCIA_RX_TimerID =  TMR_SetTimer ( TMR_RX_TIMEOUT , PROC_SCIA, 0x00, FALSE);
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIAProc_RECEIVING; 

	return;
}

//---------- estado : TOTRANSMIT  ----------------------------------------------------------
//
void f_SCIAToTxTimeout (void)
{

  SCIA_TmpVar  = SCIA_SCIDRL;	    // Flush buffer
  SCIA_TmpVar  = SCIA_SCISR1;	    // Clear pending flag

  SCIA_SCIDRL = *SCIA_FrameToTx;
  SCIA_TxByteCount--;      
  SCIA_FrameToTx++;
  
//  SCIA_SCICR2 |=  SCI1CR2_SCTIE_MASK;	      // Tx Interrupt Enable
  SCIA_SCICR2 |=  SCI1CR2_TCIE_MASK;          // TxC Interrupt Enable
  SCIA_SCICR2 |=  SCI1CR2_TE_MASK ; 				  // Tx Enable

	SCIA_TX_TimerID =  TMR_SetTimer ( TMR_TX_TIMEOUT , PROC_SCIA, 0x00, FALSE);
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIAProc_TRANSMITING; 
  
  return;										
}

//---------- estado : TRANSMITTING  ----------------------------------------------------------
//
void f_SCIATxTimeout (void)								
{
  LED_GREEN_OFF;

  RS485_A_RE_ = 1; 				    // TX Mode
  RS485_A_DE  = 1;				    // TX Mode

  SCIA_SCICR2 &= ~(SCI1CR2_TE_MASK ); 			  // Tx Disable
//  SCIA_SCICR2 &= ~(SCI1CR2_SCTIE_MASK);	    // Tx Interrupt Disable
  SCIA_SCICR2 &= ~(SCI1CR2_TCIE_MASK);        // TxC Interrupt Disable      
  SCIA_TxByteCount = 0x00;   
  
// Acciones correctivas.. error de transmision  
  if (  ++SCIA_TxErrAcc >= MAX_SERIAL_TXERR){
#ifdef MON_SERIAL_ERR_RESET_ENABLE
        MON_ResetIfNotConnected();
#endif
  }

  PutEventMsg (EVT_485_TXMSG, PROC_BAC, PROC_SCIA, (0x0200 | SCIA_CurrentTxToken));     // Informar error en transmisión
  StatisticData.SerialStat.MsgTxErr++;
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIAProc_IDLE;  // Volver a estado IDLE
}

void f_SCIATxFrame (void)
{
  TMR_FreeTimer (SCIA_TX_TimerID);
  StatisticData.SerialStat.MsgTxOk++;
//
  SCIA_RxByteCount = (UINT8) MSG_MAX_BAC_LEN;	// Cantidad de bytes pendientes de recepcion
	SCIA_RxByteIdx = 0x00;                      // Indice contador de Bytes recibidos
	
//---------------------------  
// Pedir un buffer de recepción 
//  (void)MemGetBuffer ( (void**) &(SCIA_FrameToRx), &SCIA_CurrentRxToken);
//---------------------------  
// Recuperar puntero al buffer de transmisión que se reutilizará para recepción 
  SCIA_CurrentRxToken = SCIA_CurrentTxToken;  
  if (MemGetBufferPtr( SCIA_CurrentRxToken ,  (void**) &(SCIA_FrameToRx) ) != OK ){    
  // ABR: control de errores
//    return;   
  }
//---------------------------  
  
//
  SCIA_TmpVar  = SCIA_SCISR1;	          // Clear pending flag
  SCIA_TmpVar  = SCIA_SCIDRL;	          // Flush buffer
  SCIA_SCICR2 |=  SCI1CR2_RE_MASK ;			// Rx Enable
  SCIA_SCICR2 |=	SCI1CR2_RIE_MASK;			// Rx Interrupt Enable
	
//  No envío mensaje ahora porque se reutiliza el buffer de transmisión.
//  PutEventMsg (EVT_485_TXMSG, PROC_BAC, PROC_SCIA, SCIA_CurrentTxToken);     // Informar error en transmisión

	SCIA_RX_TimerID =  TMR_SetTimer ( TMR_RX_TIMEOUT , PROC_SCIA, 0x00, FALSE);

  LED_GREEN_OFF;
  LED_RED_ON;
// Pasar a modo recepción..
  RS485_A_DE  = 0;				    // RX Mode
  RS485_A_RE_ = 0; 				    // RX Mode  
//
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIAProc_RECEIVING; 
//  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIAProc_IDLE; 
  
}

//---------- estado : RECEIVING  ------------------------------------------------------------

void f_SCIARxTimeout (void)
{
  LED_RED_OFF;
  
  RS485_A_RE_ = 1; 				    // TX Mode
  RS485_A_DE  = 1;				    // TX Mode
  
  SCIA_TmpVar  = SCIA_SCISR1;	              // Clear pending flag
  SCIA_TmpVar  = SCIA_SCIDRL;	              // Flush buffer
  SCIA_SCICR2  &= ~(SCI1CR2_RE_MASK) ;			// Rx Disable
  SCIA_SCICR2  &= ~(SCI1CR2_RIE_MASK);			// Rx Interrupt Disable
  SCIA_RxByteCount = 0;   
  
// Acciones correctivas.. error de recepcion
  if (  ++SCIA_RxErrAcc >= MAX_SERIAL_RXERR){
#ifdef MON_SERIAL_ERR_RESET_ENABLE
        MON_ResetIfNotConnected();
#endif																				
  }

  PutEventMsg (EVT_485_RXMSG, PROC_BAC, PROC_SCIA, (0x0100 | SCIA_CurrentRxToken));     // Informar error en recepcion
  StatisticData.SerialStat.MsgRxErr++;
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIAProc_IDLE;  // volver a estado IDLE

}

void f_SCIARxFrame (void)
{
  LED_RED_OFF;

  RS485_A_RE_ = 1; 				    // TX Mode
  RS485_A_DE  = 1;				    // TX Mode

  TMR_FreeTimer (SCIA_RX_TimerID);
  SCIA_RxErrAcc = 0x00;

  PutEventMsg (EVT_485_RXMSG, PROC_BAC, PROC_SCIA, SCIA_CurrentRxToken);     // Informar recepción OK

  StatisticData.SerialStat.MsgRxOk++;
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIAProc_IDLE;  // volver a estado IDLE

}

#endif		//  SCIA_RS485_MODE
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
RETCODE SCIA_TransmitFrame ( UINT16 TxLen, UINT8 TxMsgToken) 
{
 
  if ( SCIA_TxByteCount ) return ERROR;   // Si hay transmisión en curso rechazar..

#ifdef SCIA_RS485_MODE        // Si se configura al puerto en modo RS485
  RS485_A_RE_ = 1; 				    // TX Mode
  RS485_A_DE  = 1;				    // TX Mode
#endif

  LED_GREEN_ON;
  
// ABR : 28-SEP-2007
//  SCIA_TmpVar  = SCIA_SCIDRL;	    // Flush buffer
//  SCIA_TmpVar  = SCIA_SCISR1;	    // Clear pending flag
  
  SCIA_TxByteCount = TxLen;
  SCIA_CurrentTxToken = TxMsgToken;
  (void)MemGetBufferPtr ( SCIA_CurrentTxToken, (void**) &(SCIA_FrameToTx));
  
//  SCIA_SCICR2 |=  SCI1CR2_TE_MASK ; 				// Tx Enable

#if DEBUG_AT(0)
		  (void)sprintf((char *)udp_buf,"SCIA_Data Len: %d, Last:%X \n",
		                (BYTE)SCIA_TxByteCount,
		                (BYTE)SCIA_FrameToTx[SCIA_TxByteCount-1]);
      udp_data_to_send = TRUE;	  
#endif  		

#ifdef SCIA_RS485_MODE        // Si se configura al puerto en modo RS485
	SCIA_TX_TimerID =  TMR_SetTimer ( TMR_TX_TIMEOUT , PROC_SCIA, 0x00, FALSE);
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIAProc_IDLE; 
#endif		//  SCIA_RS485_MODE
										
//  SCIA_CurrentTxAddres = *SCIA_FrameToTx;

  SCIA_TmpVar  = SCIA_SCIDRL;	    // Flush buffer
  SCIA_TmpVar  = SCIA_SCISR1;	    // Clear pending flag

  SCIA_SCIDRL = *SCIA_FrameToTx;
  SCIA_TxByteCount--;      
  SCIA_FrameToTx++;
  
  SCIA_SCICR2 |=  SCI1CR2_SCTIE_MASK;	      // Tx Interrupt Enable
  SCIA_SCICR2 |=  SCI1CR2_TCIE_MASK;          // TxC Interrupt Enable
  SCIA_SCICR2 |=  SCI1CR2_TE_MASK ; 				  // Tx Enable
										
	return OK; 
  
}

//--------------------------------------------------------------------------------------
//-- Funcion de Requerimiento de Transmisión Inmediato a partir del puntero al mensaje
//
RETCODE SCIA_TransmitBuffer ( UINT16 TxLen, UINT8* TxMsgBuffer) 
{
 
  if ( SCIA_TxByteCount ) return ERROR;   // Si hay transmisión en curso rechazar..

#ifdef SCIA_RS485_MODE        // Si se configura al puerto en modo RS485
  RS485_A_RE_ = 1; 				    // TX Mode
  RS485_A_DE  = 1;				    // TX Mode
#endif

  LED_GREEN_ON;
   
  SCIA_TxByteCount = TxLen;
  SCIA_CurrentTxToken = 0xFF;
  SCIA_FrameToTx = TxMsgBuffer;
  
#if DEBUG_AT(0)
		  (void)sprintf((char *)udp_buf,"SCIA_Data Len: %d, Last:%X \n",
		                (BYTE)SCIA_TxByteCount,
		                (BYTE)SCIA_FrameToTx[SCIA_TxByteCount-1]);
      udp_data_to_send = TRUE;	  
#endif  		

#ifdef SCIA_RS485_MODE        // Si se configura al puerto en modo RS485
	SCIA_TX_TimerID =  TMR_SetTimer ( TMR_TX_TIMEOUT , PROC_SCIA, 0x00, FALSE);
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIAProc_IDLE; 
#endif		//  SCIA_RS485_MODE
										
  SCIA_TmpVar  = SCIA_SCIDRL;	    // Flush buffer
  SCIA_TmpVar  = SCIA_SCISR1;	    // Clear pending flag

  SCIA_SCIDRL = *SCIA_FrameToTx;
  SCIA_TxByteCount--;      
  SCIA_FrameToTx++;
  
  SCIA_SCICR2 |=  SCI1CR2_SCTIE_MASK;	      // Tx Interrupt Enable
  SCIA_SCICR2 |=  SCI1CR2_TCIE_MASK;          // TxC Interrupt Enable
  SCIA_SCICR2 |=  SCI1CR2_TE_MASK ; 				  // Tx Enable
										
	return OK; 
  
}


//--------------------------------------------------------------------------------------
//-- Funcion de acondicionamiento de estado de recepción
//
void SCIA_ReceiveFrame (  UINT16 RxLen, UINT8 RxMsgToken)
{
  
#ifdef SCIA_RS485_MODE        // Si se configura al puerto en modo RS485
  RS485_A_RE_ = 0; 				    // RX Mode
  RS485_A_DE  = 0;				    // RX Mode
#endif

  SCIA_RxByteCount = RxLen;			    // Cantidad de bytes pendientes de recepcion
	SCIA_RxByteIdx = 0x00;            // Indice contador de Bytes recibidos
  
  SCIA_CurrentRxToken = RxMsgToken;
  (void)MemGetBufferPtr ( SCIA_CurrentRxToken, (void**) &(SCIA_FrameToRx));


  SCIA_TmpVar  = SCIA_SCISR1;	          // Clear pending flag
  SCIA_TmpVar  = SCIA_SCIDRL;	          // Flush buffer
  SCIA_SCICR2 |=  SCI1CR2_RE_MASK ;			// Rx Enable
  SCIA_SCICR2 |=	SCI1CR2_RIE_MASK;		  // Rx Interrupt Enable

	SCIA_RxByteIdx = 0x00;
	SCIA_LastRxByte = 0x00;
	
//  SCIA_RX_FrameTimerID =  TMR_SetTimer ( 10000 , PROC_SCIA, 0x00, FALSE);
	
#ifdef SCIA_RS485_MODE        // Si se configura al puerto en modo RS485
	SCIA_RX_TimerID =  TMR_SetTimer ( TMR_RX_TIMEOUT , PROC_SCIA, 0x00, FALSE);
  FirstEventInCurrentState = (EVENT_FUNCTION*)SCIAProc_IDLE; 
#endif		//  SCIA_RS485_MODE

	return;
	
}


//--------------------------------------------------------------------------------------
//-- Funcion de Inicialización
//
void SCIA_Init (void)
{
  BYTE    tmp_cfgCode;	
  BYTE    tmp_byteIdx;	

//  SCIA_SCIBD  = SCIA_BAUD_DIV;
//  SCIA_SCICR1 = SCI1CR1_M_MASK | SCI1CR1_PE_MASK;      // 0x12 8-P-1

 // leer byte de configuración para la comunicacion serie
  eeprom_read_buffer(EE_ADDR_CFG_SCIA, &tmp_cfgCode, 1);
  tmp_byteIdx = tmp_cfgCode & CFG_SCI_BAUDRATE_MASK;
  SCIA_SCIBD  = BaudCode2RegVal[tmp_byteIdx];
  if ((tmp_cfgCode & CFG_SCI_PARITY_NONE) != CFG_SCI_PARITY_NONE){
    SCIA_SCICR1 = SCI1CR1_M_MASK | SCI1CR1_PE_MASK;      // 8 bits de datos, paridad habilitada
    if (tmp_cfgCode & CFG_SCI_PARITY_ODD){
      SCIA_SCICR1 |= SCI1CR1_PT_MASK;                    //  paridad IMPAR
    }
  }

  SCIA_SCICR2 = 0x00;
  
  RS485CFG_A_DE   = 1;              // control DE como salida, RS485_A
  RS485CFG_A_RE_  = 1;              // control RE# como salida, RS485_A
  
#ifdef SCIA_RS485_MODE              // Si se configura al puerto en modo RS485
  RS485_A_RE_     = 1; 				      // RE# = 1 incializado para transmitir
  RS485_A_DE      = 1;				      // DE = 1  incializado para transmitir
#endif

  RS485CFG_A_SEL  = 1;              // control del MUX para selección de modo, como salida  
#ifdef SCIA_RS485_MODE              // Si se configura al puerto en modo RS485
  RS485_A_SEL     = MUX_SEL_RS485;  // MUX seleccionado para tomar señales del DS3695       
  RS485_A_RE_     = 1; 				      // RE# = 1 incializado para transmitir
  RS485_A_DE      = 1;				      // DE = 1  incializado para transmitir
#else  
  RS485_A_SEL     = MUX_SEL_RS232;  // MUX seleccionado para tomar señales del MAX232     
#endif

  SCIA_RxByteCount = 0x00;   
  SCIA_TxByteCount = 0x00;   
  
  StatisticData.SerialStat.MsgTxOk    = 0x0000;
  StatisticData.SerialStat.MsgRxOk    = 0x0000;
  StatisticData.SerialStat.MsgTxErr   = 0x0000;
  StatisticData.SerialStat.MsgRxErr   = 0x0000;
  
  SCIA_TxErrAcc = 0x00;
  SCIA_RxErrAcc = 0x00;
  SCIA_CurrentTxToken = 0xFF;       // Ningún buffer transmitiéndose, habilitado para TX      

  SCIA_SCICR2 = 0x00;  
  SCIA_SCICR2 |=  SCI1CR2_RE_MASK ;			// Rx Enable
  SCIA_SCICR2 |=  SCI1CR2_TE_MASK ; 		// Tx Enable
   
}


//--------------------------------------------------------------------------------------
//-- Funcion de Reinicialización en caso de fallas 
//
void SCIA_ReInit (void)
{
  BYTE    tmp_cfgCode;	
  BYTE    tmp_byteIdx;	

//  SCIA_SCIBD  = SCIA_BAUD_DIV;
//  SCIA_SCICR1 = SCI1CR1_M_MASK | SCI1CR1_PE_MASK;      // 0x12 8-P-1

 // leer byte de configuración para la comunicacion serie
  eeprom_read_buffer(EE_ADDR_CFG_SCIA, &tmp_cfgCode, 1);
  tmp_byteIdx = tmp_cfgCode & CFG_SCI_BAUDRATE_MASK;
  SCIA_SCIBD  = BaudCode2RegVal[tmp_byteIdx];
  if ((tmp_cfgCode & CFG_SCI_PARITY_NONE) != CFG_SCI_PARITY_NONE){
    SCIA_SCICR1 = SCI1CR1_M_MASK | SCI1CR1_PE_MASK;      // 8 bits de datos, paridad habilitada
    if (tmp_cfgCode & CFG_SCI_PARITY_ODD){
      SCIA_SCICR1 |= SCI1CR1_PT_MASK;                    //  paridad IMPAR
    }
  }

  SCIA_SCICR2 = 0x00;
  
  RS485CFG_A_DE   = 1;              // control DE como salida, RS485_A
  RS485CFG_A_RE_  = 1;              // control RE# como salida, RS485_A
  
#ifdef SCIA_RS485_MODE              // Si se configura al puerto en modo RS485
  RS485_A_RE_     = 1; 				      // RE# = 1 incializado para transmitir
  RS485_A_DE      = 1;				      // DE = 1  incializado para transmitir
#endif

  RS485CFG_A_SEL  = 1;              // control del MUX para selección de modo, como salida  
#ifdef SCIA_RS485_MODE              // Si se configura al puerto en modo RS485
  RS485_A_SEL     = MUX_SEL_RS485;  // MUX seleccionado para tomar señales del DS3695       
  RS485_A_RE_     = 1; 				      // RE# = 1 incializado para transmitir
  RS485_A_DE      = 1;				      // DE = 1  incializado para transmitir
#else  
  RS485_A_SEL     = MUX_SEL_RS232;  // MUX seleccionado para tomar señales del MAX232     
#endif

  StatisticData.SerialStat.MsgTxOk    = 0x0000;
  StatisticData.SerialStat.MsgRxOk    = 0x0000;
  StatisticData.SerialStat.MsgTxErr   = 0x0000;
  StatisticData.SerialStat.MsgRxErr   = 0x0000;

  SCIA_RxByteCount = 0x00;   
  SCIA_TxByteCount = 0x00;   
  SCIA_CurrentTxToken = 0xFF;       // Ningún buffer transmitiéndose, habilitado para TX      
    
	CurrentProcessState [PROC_SCIA] = (EVENT_FUNCTION*)SCIAProc_IDLE;	
	
  SCIA_TmpVar  = SCIA_SCIDRL;	                // Flush buffer  
//  SCIA_SCICR2 |=  SCI1CR2_RE_MASK ;			// Rx Enable
//  SCIA_SCICR2 |=  SCI1CR2_TE_MASK ; 		// Tx Enable
  
}



////////////////////////////////////////////////////////////////////////////////////////////////
// SCI1 ISR
////////////////////////////////////////////////////////////////////////////////////////////////
//#pragma CODE_SEG NON_BANKED        

interrupt void near SCI1_isr (void){

//  SCIA_TmpVar	 = SCIA_SCISR1;  
  m_SCIA_SCISR1 = SCIA_SCISR1;
  
         
//--------------RECEPCION-------------------------------------------------------   
		 
  if ((m_SCIA_SCISR1 & SCI1SR1_RDRF_MASK) && (SCIA_SCICR2 & SCI1CR2_RIE_MASK)){	        // si es interrupcion de RX 
       
    if (SCIA_RxByteCount){                        // si quedan bytes por recibir
      *SCIA_FrameToRx = SCIA_SCIDRL; 
/*
      if ( SCIA_RxByteIdx == 1 ){		              // segundo byte: verificar START_L word
         if ( *SCIA_FrameToRx != 0x10){           // Verificar START_L word = 0x64
            SCIA_SCICR2  &= ~(SCI1CR2_RE_MASK) ;  // Rx Disable
            SCIA_SCICR2  &= ~(SCI1CR2_RIE_MASK);	// Rx Interrupt Disable            
         } 
         SCIA_RxByteIdx++;
         LED_RED_ON;
      }
*/
      if (SCIA_RxByteIdx == 0x00){                // primer byte recibido ?
         if ( *SCIA_FrameToRx != DLE_CHAR){       // Verificar START = DLE
            return;                               // Descartar y seguir esperando
         } 
         else{            
              LED_RED_ON;
              //ABR: 2011-05-02 : superposicion de timers con capa más alta. CMD_TX duplicados ?
              //SCIA_RX_FrameTimerID =  TMR_SetTimer ( 5000 , PROC_SCIA, 0x00, FALSE);
//            SCIA_RxByteIdx++;
         }  	         
      }

      if ((SCIA_LastRxByte==0x10)&&(*SCIA_FrameToRx == 0x03)){               // Flag ETX : End of Transmission
          SCIA_RxByteCount = 3;                   // Recibir 2 bytes más del checksum
      }
      
       
      SCIA_LastRxByte = *SCIA_FrameToRx;
       
      SCIA_FrameToRx++;
      SCIA_RxByteCount--;    
      SCIA_RxByteIdx++;
                
      if (!SCIA_RxByteCount){
          SCIA_SCICR2  &= ~(SCI1CR2_RE_MASK) ;		// Rx Disable
          SCIA_SCICR2  &= ~(SCI1CR2_RIE_MASK);	  // Rx Interrupt Disable
          TMR_FreeTimer (SCIA_RX_FrameTimerID);    
          
          PutEventMsg (EVT_232_RXMSG, PROC_BAC, PROC_SCIA, (UINT16)((SCIA_RxByteIdx<<8)|SCIA_CurrentRxToken));    
//          StatisticData.SerialStat.MsgRxOk++;
          LED_RED_OFF;
      }
            
    }
    else{
      SCIA_TmpVar  = SCIA_SCIDRL;	                // Flush buffer
    } 
//    return;   // ABR : 28-SEP-2007
  }
      
//--------------TRANSMISION-------------------------------------------------------   
/*     
      if (( m_SCIA_SCISR1 & SCI1SR1_TC_MASK ) && (SCIA_SCICR2 & SCI1CR2_TCIE_MASK))	        // si es interrupcion de TXC
      {
         SCIA_SCICR2 &= ~(SCI1CR2_SCTIE_MASK);	    // TIE Interrupt Disable
         SCIA_SCICR2 &= ~(SCI1CR2_TE_MASK ); 			  // TE Disable
         SCIA_SCICR2 &= ~(SCI1CR2_TCIE_MASK);        // TCIE Interrupt Disable      
         PutEventMsg (EVT_232_TXMSG, PROC_DNPLOW, PROC_SCIA, (UINT16)SCIA_CurrentTxToken);    
         LED_GREEN_OFF;
      } 
      SCIA_TmpVar  = SCIA_SCIDRL;	                // Flush buffer
*/      
//  if (( m_SCIA_SCISR1 & SCI1SR1_TDRE_MASK ) && (SCIA_SCICR2 & SCI1CR2_SCTIE_MASK))	  // si es interrupcion de TX
  if (( m_SCIA_SCISR1 & SCI1SR1_TC_MASK ) && (SCIA_SCICR2 & SCI1CR2_TCIE_MASK))	        // si es interrupcion de TXC
  {
      if (!SCIA_TxByteCount)
      {
         SCIA_SCICR2 &= ~(SCI1CR2_SCTIE_MASK);	    // TIE Interrupt Disable
         SCIA_SCICR2 &= ~(SCI1CR2_TE_MASK ); 			  // TE Disable
         SCIA_SCICR2 &= ~(SCI1CR2_TCIE_MASK);       // TCIE Interrupt Disable      
         PutEventMsg (EVT_232_TXMSG, PROC_BAC, PROC_SCIA, SCIA_CurrentTxToken);    
         LED_GREEN_OFF;
#ifdef  DEBUG_SIM_BACFRAME  // Simular bac frame       
  SCIA_FrameToRx[0] = 0x10;
  SCIA_FrameToRx[1] = 0x02;
  SCIA_FrameToRx[2] = 0x00;
  SCIA_FrameToRx[3] = 0x00;
  SCIA_FrameToRx[4] = 0x00;
  SCIA_FrameToRx[5] = 0x00;
  SCIA_FrameToRx[6] = 0x34;
  SCIA_FrameToRx[7] = 0x35;
  SCIA_FrameToRx[8] = 0x36;
  SCIA_FrameToRx[9] = 0x00;
  SCIA_FrameToRx[10] = 0xF0;
  SCIA_FrameToRx[11] = 0x30;
  SCIA_FrameToRx[12] = 0xEA;
  SCIA_FrameToRx[13] = 0x3A;
  SCIA_FrameToRx[14] = 0x00;
  SCIA_FrameToRx[15] = 0x2C;
  SCIA_FrameToRx[16] = 0x00;
  SCIA_FrameToRx[17] = 0x00;
  SCIA_FrameToRx[18] = 0x00;
  SCIA_FrameToRx[19] = 0x00;
  SCIA_FrameToRx[20] = 0x10;
  SCIA_FrameToRx[21] = 0x03;
  SCIA_FrameToRx[22] = 0xAD;
  SCIA_FrameToRx[23] = 0xE0;
  SCIA_RxByteIdx = 24;
  PutEventMsg (EVT_232_RXMSG, PROC_BAC, PROC_SCIA, (UINT16)((SCIA_RxByteIdx<<8)|SCIA_CurrentRxToken));    
#endif    // Simular bac frame  
      }
      else
      {
        SCIA_SCIDRL = *SCIA_FrameToTx;
        SCIA_FrameToTx++;
        SCIA_TxByteCount--; 
      }
  }
  
   
//--------------ERROR DE COMUNICACION--------------------------------------------   

//  SCIA_SCICR2 &= ~(SCI1CR2_RE_MASK) ;			            // Rx Disable
//  SCIA_SCICR2 &= ~(SCI1CR2_RIE_MASK);			            // Rx Interrupt Disable
//  SCIA_SCICR2 &= ~(SCI1CR2_TE_MASK ); 			          // Tx Disable
//  SCIA_SCICR2 &= ~(SCI1CR2_TCIE_MASK);                // TxC Interrupt Disable      

//  TMR_FreeTimer (SCIA_RX_FrameTimerID);                    
//  SCIA_ReceiveFrame (3, );

}

#pragma CODE_SEG DEFAULT


