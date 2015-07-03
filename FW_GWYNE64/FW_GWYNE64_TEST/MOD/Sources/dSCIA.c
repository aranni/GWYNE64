#include "MC9S12NE64.h"
#include "datatypes.h"

#include "Commdef.h"
#include "dSCIA.h"
#include "global.h"


///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_SERIAL
///////////////////////////////////////////////////////////////////////////////
//
const EVENT_FUNCTION	SerialProc_IDLE [] =
{
{EVT_485_TXREQ 	      , f_SerialSendRequest	},
{EVT_OTHER_MSG     		, f_Consume         }};

const EVENT_FUNCTION	SerialProc_TRANSMITING [] =
{
{EVT_TMR_TIMEOUT 	    , f_SerialTxTimeout	},
{EVT_485_TXMSG 	      , f_SerialTxFrame	},
{EVT_OTHER_MSG     		, f_Consume         }};

const EVENT_FUNCTION	SerialProc_RECEIVING [] =
{
{EVT_TMR_TIMEOUT 	    , f_SerialRxTimeout	},
{EVT_485_RXMSG 	      , f_SerialRxFrame	},
{EVT_OTHER_MSG     		, f_Consume         }};



///////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_SERIAL
///////////////////////////////////////////////////////////////////////////////

//-------------- estado : IDLE --------------------------

void f_SerialSendRequest (void)
{

  RS485_A_RE_ = 1; 				// TX Mode
  RS485_A_DE  = 1;				// TX Mode
  SCI_tmp_var  = SCIDRL;	 // Flush buffer
  SCI_tmp_var  = SCISR1;	 // Clear pending flag
  LED_RED_ON;

  switch (SYS_ConfigParam.InvCode){
    
    case (PROC_SIEMENS ):
      SCI_tx_bytecount = SIE_COMMON_LGE + 1;
      frame_to_tx = (UINT8 *)(&(pSiemensOutMsg->LGE));

      SCICR2 |=  SCI1CR2_TE_MASK ; 				// Tx Enable
      SCIDRL = pSiemensOutMsg->IniFlag;
      break;

    case (PROC_SIE_MASTERDRV ):
      SCI_tx_bytecount = SIE_COMMON_LGE + 1;
      frame_to_tx = (UINT8 *)(&(pSieMasterDrvOutMsg->LGE));

      SCICR2 |=  SCI1CR2_TE_MASK ; 				// Tx Enable
      SCIDRL = pSieMasterDrvOutMsg->IniFlag;
      break;      
      
/*      
    case (PROC_HITACHI ):
      SCI_tx_bytecount = ((pHitachiOutMsg->TxLen) - 1);
      frame_to_tx = (UINT8 *)(&(pHitachiOutMsg->Addr[0]));
      
      SCICR2 |=  SCI1CR2_TE_MASK ; 				// Tx Enable
      SCIDRL = pHitachiOutMsg->IniFlag;
      break;

    case (PROC_HIT_SJ300 ):
      SCI_tx_bytecount = ((pHitSJ300OutMsg->TxLen) - 1);
      frame_to_tx = (UINT8 *)(&(pHitSJ300OutMsg->Addr[0]));
      
      SCICR2 |=  SCI1CR2_TE_MASK ; 				// Tx Enable
      SCIDRL = pHitSJ300OutMsg->IniFlag;
      break;
*/      
    default:
        MON_ResetBoard();    
      break;
  }
  
//  SCICR2 |=  SCI1CR2_SCTIE_MASK;	    // Tx Interrupt Enable
  SCICR2 |=  SCI1CR2_TCIE_MASK;       // TxC Interrupt Enable
	SCI_TX_TimerID =  TMR_SetTimer ( TMR_TX_TIMEOUT , PROC_SERIAL, 0x00, FALSE);
  FirstEventInCurrentState = (EVENT_FUNCTION*)SerialProc_TRANSMITING; 

	return;
}

//---------- estado : TRANSMITTING  ----------------------

void f_SerialTxTimeout (void)
{
  LED_RED_OFF;
  RS485_A_RE_ = 1; 				// TX Mode
  RS485_A_DE  = 1;				// TX Mode
  SCICR2 &= ~(SCI1CR2_TE_MASK ); 			  // Tx Disable
//  SCICR2 &= ~(SCI1CR2_SCTIE_MASK);	  // Tx Interrupt Disable
  SCICR2 &= ~(SCI1CR2_TCIE_MASK);       // TxC Interrupt Disable      
  SCI_tx_bytecount = 0x00;   
  
  // Acciones correctivas.. error de transmision  
  if (  ++SCI_TxErrAcc >= MAX_SERIAL_TXERR){
   	    InverterParam.StatusWord |= STATUS_CX_FAIL_BIT;     	
#ifdef MON_SERIAL_ERR_RESET_ENABLE
        MON_ResetIfNotConnected();
#endif
  }

  PutEventMsg (EVT_SIE_RXFRAME, SYS_ConfigParam.InvCode, PROC_SERIAL, 0x02);
  StatisticData.SerialStat.MsgTxErr++;
  FirstEventInCurrentState = (EVENT_FUNCTION*)SerialProc_IDLE; 
}

void f_SerialTxFrame (void)
{
  TMR_FreeTimer (SCI_TX_TimerID);
  LED_RED_OFF;
  StatisticData.SerialStat.MsgTxOk++;
  SCI_TxErrAcc = 0x00;
  LED_GREEN_ON;
  
  switch (SYS_ConfigParam.InvCode){
    
    case (PROC_SIEMENS ):
      SCI_rx_bytecount = SIE_COMMON_LGE + 2;
      frame_to_rx = (UINT8 *)(&(pSiemensInMsg->IniFlag));
      break;

    case (PROC_SIE_MASTERDRV ):
      SCI_rx_bytecount = SIE_COMMON_LGE + 2;
      frame_to_rx = (UINT8 *)(&(pSieMasterDrvInMsg->IniFlag));
      break;
/*
    case (PROC_HITACHI ):
      SCI_rx_bytecount = pHitachiOutMsg->RxLen;
      frame_to_rx = (UINT8 *)(&(pHitachiInMsg->IniFlag));
      break;

    case (PROC_HIT_SJ300 ):
      SCI_rx_bytecount = pHitSJ300OutMsg->RxLen;
      frame_to_rx = (UINT8 *)(&(pHitSJ300InMsg->IniFlag));
      break;
*/      
    default:
        MON_ResetBoard();    
      break;
  }
  
//  SCI_RxWaitFirst = 0x01;

  RS485_A_DE  = 0;				// RX Mode
  RS485_A_RE_ = 0; 				// RX Mode  

  SCI_tmp_var  = SCISR1;	          // Clear pending flag
  SCI_tmp_var  = SCIDRL;	          // Flush buffer
  SCICR2 |=  SCI1CR2_RE_MASK ;			// Rx Enable
  SCICR2 |=	 SCI1CR2_RIE_MASK;			// Rx Interrupt Enable
	
	SCI_RX_TimerID =  TMR_SetTimer ( TMR_RX_TIMEOUT , PROC_SERIAL, 0x00, FALSE);
  FirstEventInCurrentState = (EVENT_FUNCTION*)SerialProc_RECEIVING; 
  
}

//---------- estado : RECEIVING  ------------------------

void f_SerialRxTimeout (void)
{

  LED_GREEN_OFF;
  RS485_A_RE_ = 1; 				// TX Mode
  RS485_A_DE  = 1;				// TX Mode
  
  SCI_tmp_var  = SCISR1;	 // Clear pending flag
  SCI_tmp_var  = SCIDRL;	 // Flush buffer
  SCICR2  &= ~(SCI1CR2_RE_MASK) ;			// Rx Disable
  SCICR2  &= ~(SCI1CR2_RIE_MASK);			// Rx Interrupt Disable
  SCI_rx_bytecount = 0x00;   
  
  // Acciones correctivas.. error de recepcion
  if (  ++SCI_RxErrAcc >= MAX_SERIAL_RXERR){
   	    InverterParam.StatusWord |= STATUS_CX_FAIL_BIT;     	
#ifdef MON_SERIAL_ERR_RESET_ENABLE
        MON_ResetIfNotConnected();
#endif																				
  }

  PutEventMsg (EVT_SIE_RXFRAME, SYS_ConfigParam.InvCode, PROC_SERIAL, 0x01);
  StatisticData.SerialStat.MsgRxErr++;
  
  FirstEventInCurrentState = (EVENT_FUNCTION*)SerialProc_IDLE; 

}

void f_SerialRxFrame (void)
{

  LED_GREEN_OFF;
  RS485_A_RE_ = 1; 				// TX Mode
  RS485_A_DE  = 1;				// TX Mode

  TMR_FreeTimer (SCI_RX_TimerID);
  SCI_RxErrAcc = 0x00;

  PutEventMsg (EVT_SIE_RXFRAME, SYS_ConfigParam.InvCode, PROC_SERIAL, 0x00);
  
	InverterParam.StatusWord &= ~(STATUS_CX_FAIL_BIT);     	
  StatisticData.SerialStat.MsgRxOk++;
  FirstEventInCurrentState = (EVENT_FUNCTION*)SerialProc_IDLE; 

}


///////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
///////////////////////////////////////////////////////////////////////////////
//
void SRL_Init (void)
{
 
  SCIBD  = BAUD_DIV;
  SCICR1 = SCI1CR1_M_MASK | SCI1CR1_PE_MASK;      // 0x12 8-P-1
  SCICR2 = 0x00;
  
  RS485CFG_A_DE   = 1;        // Output DE, RS485_A
  RS485CFG_A_RE_  = 1;        // Output RE#, RS485_A
  RS485_A_RE_     = 1; 				// TX Mode
  RS485_A_DE      = 1;				// TX Mode

  RS485CFG_A_SEL  = 1;        // Salida para seleccionar el MUX
  RS485_A_SEL     = MUX_SEL_RS485;       
  
  SCI_rx_bytecount = 0x00;   
  SCI_tx_bytecount = 0x00;   
  
  StatisticData.SerialStat.MsgTxOk    = 0x0000;
  StatisticData.SerialStat.MsgRxOk    = 0x0000;
  StatisticData.SerialStat.MsgTxErr   = 0x0000;
  StatisticData.SerialStat.MsgRxErr   = 0x0000;
  
  SCI_TxErrAcc = 0x00;
  SCI_RxErrAcc = 0x00;
  
}

void SRL_ReInit (void)
{
 
  SCIBD  = BAUD_DIV;
  SCICR1 = SCI1CR1_M_MASK | SCI1CR1_PE_MASK;      // 0x12 8-P-1
  SCICR2 = 0x00;
 
  RS485CFG_A_DE   = 1;        // Output DE, RS485_A
  RS485CFG_A_RE_  = 1;        // Output RE#, RS485_A
  RS485_A_RE_     = 1; 				// TX Mode
  RS485_A_DE      = 1;				// TX Mode

  RS485CFG_A_SEL  = 1;        // Salida para seleccionar el MUX
  RS485_A_SEL     = MUX_SEL_RS485;       
  
  SCI_rx_bytecount = 0x00;   
  SCI_tx_bytecount = 0x00; 
    
	CurrentProcessState [PROC_SERIAL] = (EVENT_FUNCTION*)SerialProc_IDLE;	
  
}



///////////////////////////////////////////////////////////////////////////////
// SCI1 ISR
///////////////////////////////////////////////////////////////////////////////
#pragma CODE_SEG NON_BANKED        

interrupt void near SCI1_isr (void){

   SCI_tmp_var	 = SCISR1;
   
   if (SCI_tmp_var & SCI1SR1_RDRF_MASK){	      // RX interrupt
    
    if (SCI_rx_bytecount){
      *frame_to_rx = SCIDRL; 
//      if (SCI_RxWaitFirst){
//        if ( *frame_to_rx == STX )
//           SCI_RxWaitFirst = 0x00;
//        else
//            SCI_rx_bytecount++;
//      }
      frame_to_rx++;
      SCI_rx_bytecount--;    
      if (!SCI_rx_bytecount){
          SCICR2  &= ~(SCI1CR2_RE_MASK) ;			  // Rx Disable
          SCICR2  &= ~(SCI1CR2_RIE_MASK);			  // Rx Interrupt Disable
          PutEventMsg (EVT_485_RXMSG, PROC_SERIAL, OS_NOT_PROC, 0x00);    
      }
    }
    else{
      SCI_tmp_var  = SCIDRL;	 // Flush buffer
    } 
    return;
   }
   
   if (SCI_tmp_var & SCI1SR1_TDRE_MASK){	    // TX interrupt
//   else{
    
    if (!SCI_tx_bytecount){
//      SCICR2 &= ~(SCI1CR2_SCTIE_MASK);	    // Tx Interrupt Disable
      SCICR2 &= ~(SCI1CR2_TE_MASK ); 			  // Tx Disable
      SCICR2 &= ~(SCI1CR2_TCIE_MASK);       // TxC Interrupt Disable      
      PutEventMsg (EVT_485_TXMSG, PROC_SERIAL, OS_NOT_PROC, 0x00);    
    } 
    else{
      SCIDRL = *frame_to_tx;
      frame_to_tx++;
      SCI_tx_bytecount--;    
    }
    return;
   }
   
   SCICR2  &= ~(SCI1CR2_RE_MASK) ;			  // Rx Disable
   SCICR2  &= ~(SCI1CR2_RIE_MASK);			  // Rx Interrupt Disable
   SCICR2 &= ~(SCI1CR2_TE_MASK ); 			  // Tx Disable
   SCICR2 &= ~(SCI1CR2_TCIE_MASK);       // TxC Interrupt Disable      

}

#pragma CODE_SEG DEFAULT


