#include "MC9S12NE64.h"
#include "datatypes.h"

#include "Commdef.h"
#include "dSCIB.h"
#include "Global.h"


//-----------------------------------------------------------------------------
//  VARIABLES LOCALES
//-----------------------------------------------------------------------------
//
UINT8 RTU_tmp_var;
UINT8 RTU_RxByteIdx;
UINT8 RTU_TimerID;

UINT8 RTU_rx_bytecount;
UINT8 RTU_tx_bytecount;
UINT8 * RTU_frame_to_rx; 
UINT8 * RTU_frame_to_tx; 
MODBUS_RTU_MSG *    RTU_frame_to_tfr; 

#pragma CODE_SEG NON_BANKED        

///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_RTU
///////////////////////////////////////////////////////////////////////////////
//
const EVENT_FUNCTION	RtuProc_IDLE [] =
{
{EVT_TMR_TIMEOUT      , f_RtuIdleTimeout	},
{EVT_OTHER_MSG     		, f_Consume         }};




///////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_RTU
///////////////////////////////////////////////////////////////////////////////

void f_RtuIdleTimeout (void) {

//ABR  RTU_ReceiveFrame (8, (UINT8 *)pModRtuInMsg);
  return; 
}


///////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
///////////////////////////////////////////////////////////////////////////////
//


void RTU_TransmitFrame ( UINT8 Len, UINT8 * MsgPtr) 
{

  LED_RED_ON;

  RTU_tx_bytecount = Len;
  RTU_frame_to_tx = (UINT8 *)MsgPtr;
  
  RTU_tmp_var  = RTU_SCIDRL;	 // Flush buffer
  RTU_tmp_var  = RTU_SCISR1;	 // Clear pending flag

  RTU_SCICR2 |=  SCI1CR2_TE_MASK ; 				// Tx Enable
  RTU_SCIDRL = *RTU_frame_to_tx;
  RTU_frame_to_tx++;
  RTU_tx_bytecount--;    
  
// RTU_SCICR2 |=  SCI1CR2_SCTIE_MASK;	    // Tx Interrupt Enable
  RTU_SCICR2 |=  SCI1CR2_TCIE_MASK;       // TxC Interrupt Enable
  
}


void RTU_ReceiveFrame ( UINT8 Len, UINT8 * MsgPtr)
{
  LED_RED_OFF;
  RTU_rx_bytecount = Len;
  RTU_frame_to_rx = (UINT8 *)MsgPtr;
  RTU_tmp_var  = RTU_SCISR1;	          // Clear pending flag
  RTU_tmp_var  = RTU_SCIDRL;	          // Flush buffer
  RTU_SCICR2 |=  SCI1CR2_RE_MASK ;			// Rx Enable
  RTU_SCICR2 |=	 SCI1CR2_RIE_MASK;			// Rx Interrupt Enable
	RTU_RxByteIdx = 0x00;
	
}



///////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
///////////////////////////////////////////////////////////////////////////////
//
void RTU_Init (void)
{
 
  RTU_SCIBD  = RTU_BAUD_DIV;
  RTU_SCICR1 = SCI1CR1_M_MASK | SCI1CR1_PE_MASK;      // 0x12 8-P-1
  RTU_SCICR2 = 0x00;

  RS485CFG_B_DE   = 1;        // Output DE, RS485_B
  RS485CFG_B_RE_  = 1;        // Output RE#, RS485_B
  RS485_B_RE_     = 0; 				// RX Mode
  RS485_B_DE      = 0;				// RX Mode

  RS485CFG_B_SEL  = 1;        // Salida para seleccionar el MUX
  RS485_B_SEL     = MUX_SEL_RS232;       

  RTU_rx_bytecount = 0x00;   
  RTU_tx_bytecount = 0x00;
  RTU_RxByteIdx    = 0x00;  
     
}

void RTU_ReInit (void)
{
 
  RTU_SCIBD  = RTU_BAUD_DIV;
  RTU_SCICR1 = SCI1CR1_M_MASK | SCI1CR1_PE_MASK;      // 0x12 8-P-1
  RTU_SCICR2 = 0x00;

  RS485CFG_B_DE   = 1;        // Output DE, RS485_B
  RS485CFG_B_RE_  = 1;        // Output RE#, RS485_B
  RS485_B_RE_     = 0; 				// RX Mode
  RS485_B_DE      = 0;				// RX Mode

  RS485CFG_B_SEL  = 1;        // Salida para seleccionar el MUX
  RS485_B_SEL     = MUX_SEL_RS232;       
  
  RTU_rx_bytecount = 0x00;   
  RTU_tx_bytecount = 0x00; 
  RTU_RxByteIdx    = 0x00;   
      
}



///////////////////////////////////////////////////////////////////////////////
// SCI1 ISR
///////////////////////////////////////////////////////////////////////////////
//#pragma CODE_SEG NON_BANKED        

interrupt void near SCI0_isr (void){

  RTU_tmp_var	 = RTU_SCISR1;
   
  if (RTU_tmp_var & SCI1SR1_RDRF_MASK){	      // RX interrupt
    
    if (RTU_rx_bytecount){
      *RTU_frame_to_rx = RTU_SCIDRL; 
      if (!RTU_RxByteIdx){
	         RTU_TimerID =  TMR_SetTimer ( 30 , PROC_RTU, 0x00, FALSE);
      }
//ABR      if ( (RTU_RxByteIdx == 0x06) && ( pModRtuInMsg->FC == 16) ){		 // FC = WR_MUL
//        RTU_rx_bytecount = *RTU_frame_to_rx;			// Byte count 
//        RTU_rx_bytecount += 0x03;                 // CRC  + bound effects
//      }
      RTU_RxByteIdx++;
      RTU_frame_to_rx++;
      RTU_rx_bytecount--;    
      if (!RTU_rx_bytecount){
          RTU_SCICR2  &= ~(SCI1CR2_RE_MASK) ;			  // Rx Disable
          RTU_SCICR2  &= ~(SCI1CR2_RIE_MASK);			  // Rx Interrupt Disable
          TMR_FreeTimer (RTU_TimerID);                    
//ABR          PutEventMsg (EVT_232_RXMSG, PROC_MODRTU, OS_NOT_PROC, 0x00);    
//ABR          RTU_ReceiveFrame (8, (UINT8 *)pModRtuInMsg);
      }
    }
    else{
      RTU_tmp_var  = RTU_SCIDRL;	 // Flush buffer
    } 
    return;
  }
  
  if (RTU_tmp_var & SCI1SR1_TDRE_MASK){	    // TX interrupt
//   else{
    
    if (!RTU_tx_bytecount){
//      RTU_SCICR2 &= ~(SCI1CR2_SCTIE_MASK);	    // Tx Interrupt Disable
      RTU_SCICR2 &= ~(SCI1CR2_TE_MASK ); 			  // Tx Disable
      RTU_SCICR2 &= ~(SCI1CR2_TCIE_MASK);       // TxC Interrupt Disable      
//ABR      PutEventMsg (EVT_232_TXMSG, PROC_MODRTU, OS_NOT_PROC, 0x00);    
    } 
    else{
      RTU_SCIDRL = *RTU_frame_to_tx;
      RTU_frame_to_tx++;
      RTU_tx_bytecount--;    
    }
    return;
  }
     
  RTU_SCICR2  &= ~(SCI1CR2_RE_MASK) ;			  // Rx Disable
  RTU_SCICR2  &= ~(SCI1CR2_RIE_MASK);			  // Rx Interrupt Disable
  RTU_SCICR2 &= ~(SCI1CR2_TE_MASK ); 			  // Tx Disable
  TMR_FreeTimer (RTU_TimerID);
//ABR  RTU_ReceiveFrame (8, (UINT8 *)pModRtuInMsg);

}

#pragma CODE_SEG DEFAULT


