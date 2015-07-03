#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"
#include "Commdef.h"

#ifdef  MOD_INPUT_OUTPUT_ENABLED

#include "pInputOutput.h"
#include "Global.h"
#include "system.h"
#include "tcp_ip.h"

///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_PIO
///////////////////////////////////////////////////////////////////////////////
//
const EVENT_FUNCTION	PioProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	      , f_PioIdleTimeout      },
{EVT_OTHER_MSG     		  , f_Consume             }};



///////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_PIO
///////////////////////////////////////////////////////////////////////////////
//
//-------------- estado : IDLE ----------------------------------------------------------------
//
//-----------------------------------------------------------------------------
// Lectura periodica de estados de entradas salidas
//-----------------------------------------------------------------------------
//
void f_PioIdleTimeout (void)
{
    BYTE bTmpCntr;
    BYTE bTmpIdx;
    
    UINT8*  pTxBuffer;
    UINT8   bTmpToken; 

    GpioTimerId=0xFF;

    GpioControl.PreviousVal  = GpioControl.CurrentVal ;
    GpioControl.CurrentVal  = GIO_GetInpVal();
    
    GpioControl.PeriodCounter--;
    bTmpIdx =0x01;
    
    for (bTmpCntr=0 ; bTmpCntr < MAX_DIO_QTTY ; bTmpCntr++)
    {		     
       																 
    	 switch ( GpioControl.CfgIo[bTmpCntr] & CFG_DIO_TRIGGER_MASK )	 
    	 {
    	    case CFG_DIO_TRIGGER_LEVEL:
    	    
    	      if ( (GpioControl.PreviousVal & bTmpIdx) !=  (GpioControl.CurrentVal  & bTmpIdx) ) 
    	      {    	      
      	        if  (GpioControl.CurrentVal  & bTmpIdx)
      	        {
      	            if (GpioControl.CfgIo[bTmpCntr] & CFG_DIO_REPORT_MESSAGE)
                        PutEventMsg (EVT_DIO_CHANGEON, PROC_NEXO, PROC_PIO, bTmpCntr);
      	            
      	            if (GpioControl.CfgIo[bTmpCntr] & CFG_DIO_REPORT_EVENT) 
      	            {
                        if ( OK == MemGetBuffer ( (void**)(&pTxBuffer), &bTmpToken)) 
                        {
                            (void)sprintf((char *)&(((LOG_EVENT_FORMAT*)pTxBuffer)->Data[0]),"GPIO_NIVEL_ON: %02X \0",
                                          (UINT8)bTmpCntr);
                            ((LOG_EVENT_FORMAT*)pTxBuffer)->EventSource =  PROC_PIO;
                            ((LOG_EVENT_FORMAT*)pTxBuffer)->EventLastState =  EVENT_STATE_PEND;    
                            ((LOG_EVENT_FORMAT*)pTxBuffer)->EventCode =  EVENT_CODE_DIO_LEVEL_CHANGE;    
                            ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamByteA =  bTmpCntr;  // entrada que cambió   
                            ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamByteB =  0x01;      // nuevo valor de la entrada    
                            ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamWord  =  0x00;      // no usado 
                            ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamDword =  0x00;      // no usado 
                                                                                                   
                            PutEventMsg (EVT_LOG_EVENT_SAVE, PROC_DEBUG, PROC_PIO, (UINT16)bTmpToken);    
                        }      	            
      	            }
      	            
      	        }
                else 
                {
       	            if (GpioControl.CfgIo[bTmpCntr] & CFG_DIO_REPORT_MESSAGE)
                        PutEventMsg (EVT_DIO_CHANGEOFF, PROC_NEXO, PROC_PIO, bTmpCntr);
       	            
      	            if (GpioControl.CfgIo[bTmpCntr] & CFG_DIO_REPORT_EVENT) 
      	            {
                        if ( OK == MemGetBuffer ( (void**)(&pTxBuffer), &bTmpToken)) 
                        {
                            (void)sprintf((char *)&(((LOG_EVENT_FORMAT*)pTxBuffer)->Data[0]),"GPIO_NIVEL_OFF: %02X \0",
                                          (UINT8)bTmpCntr);
                            ((LOG_EVENT_FORMAT*)pTxBuffer)->EventSource =  PROC_PIO;
                            ((LOG_EVENT_FORMAT*)pTxBuffer)->EventLastState =  EVENT_STATE_PEND;                           
                            ((LOG_EVENT_FORMAT*)pTxBuffer)->EventCode =  EVENT_CODE_DIO_LEVEL_CHANGE;    
                            ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamByteA =  bTmpCntr;  // entrada que cambió   
                            ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamByteB =  0x00;      // nuevo valor de la entrada    
                            ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamWord  =  0x00;      // no usado 
                            ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamDword =  0x00;      // no usado 
                            PutEventMsg (EVT_LOG_EVENT_SAVE, PROC_DEBUG, PROC_PIO, (UINT16)bTmpToken);    
                        }      	            
      	            }
      	            
               }    	      		  
    	      }    	              	   
    	      break;
    	      
    	    
     	    case CFG_DIO_TRIGGER_PSLOPE:
     	    
    	      if ( (!(GpioControl.PreviousVal & bTmpIdx)) && (GpioControl.CurrentVal & bTmpIdx) ) 
    	      {
    	      
   	            if (GpioControl.CfgIo[bTmpCntr] & CFG_DIO_REPORT_MESSAGE)
                    PutEventMsg (EVT_DIO_PULSE, PROC_NEXO, PROC_PIO, bTmpCntr);  
            
  	            if (GpioControl.CfgIo[bTmpCntr] & CFG_DIO_REPORT_EVENT) 
  	            {
                    if ( OK == MemGetBuffer ( (void**)(&pTxBuffer), &bTmpToken)) 
                    {
                        (void)sprintf((char *)&(((LOG_EVENT_FORMAT*)pTxBuffer)->Data[0]),"GPIO_PULSO_ON: %02X \0",
                                      (UINT8)bTmpCntr);
                        ((LOG_EVENT_FORMAT*)pTxBuffer)->EventSource =  PROC_PIO;
                        ((LOG_EVENT_FORMAT*)pTxBuffer)->EventLastState =  EVENT_STATE_PEND;                           
                        ((LOG_EVENT_FORMAT*)pTxBuffer)->EventCode =  EVENT_CODE_DIO_PULSE_ON;    
                        ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamByteA =  bTmpCntr;  // entrada que cambió   
                        ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamByteB =  0x00;      // no usado   
                        ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamWord  =  0x00;      // no usado 
                        ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamDword =  0x00;      // no usado 
                        PutEventMsg (EVT_LOG_EVENT_SAVE, PROC_DEBUG, PROC_PIO, (UINT16)bTmpToken);    
                    }      	            
  	            }              
                  	      		  
    	      }    	              	   
    	      break;
   	    
     	    default:
    	      break;
    	      
   	   }
   	    
       if ( (!GpioControl.PeriodCounter) && (GpioControl.CfgIo[bTmpCntr] & CFG_DIO_REPORT_PERIODIC_MASK) )
       {       
   	            if (GpioControl.CfgIo[bTmpCntr] & CFG_DIO_REPORT_PERIODIC_MESSAGE)
                    PutEventMsg ((GpioControl.CurrentVal & bTmpIdx)?EVT_DIO_STATEON:EVT_DIO_STATEOFF, PROC_NEXO, PROC_PIO, bTmpCntr);  
            
  	            if (GpioControl.CfgIo[bTmpCntr] & CFG_DIO_REPORT_PERIODIC_EVENT) 
  	            {       
                    if ( OK == MemGetBuffer ( (void**)(&pTxBuffer), &bTmpToken)) 
                    {
                        (void)sprintf((char *)&(((LOG_EVENT_FORMAT*)pTxBuffer)->Data[0]),"GPIO_NIVEL_#%02X = %02X \0",
                                      (UINT8)bTmpCntr,
                                      (UINT8)(GpioControl.CurrentVal & bTmpIdx)?0x01:0x00 );
                        ((LOG_EVENT_FORMAT*)pTxBuffer)->EventSource =  PROC_PIO;
                        ((LOG_EVENT_FORMAT*)pTxBuffer)->EventLastState =  EVENT_STATE_PEND;    
                        ((LOG_EVENT_FORMAT*)pTxBuffer)->EventCode =  EVENT_CODE_DIO_LEVEL_STATE;    
                        ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamByteA =  bTmpCntr;  // entrada reportada   
                        ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamByteB =  (GpioControl.CurrentVal & bTmpIdx)?0x01:0x00;    
                        ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamWord  =  0x00;      // no usado 
                        ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamDword =  0x00;      // no usado 
                                                                                               
                        PutEventMsg (EVT_LOG_EVENT_SAVE, PROC_DEBUG, PROC_PIO, (UINT16)bTmpToken);    
                    } 
  	            }
       }
   	    
   	   bTmpIdx *= 2; 
    }
    
    if (!GpioControl.PeriodCounter)
      GpioControl.PeriodCounter	 = GpioControl.PeriodSetPoint;
    
	  GpioTimerId = TMR_SetTimer ( PIO_PERIODIC_TIMERVAL , PROC_PIO, 0x00, FALSE);
    
	  return;
}


///////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// GPIO_Init : inicializacion de variables del proceso
//-----------------------------------------------------------------------------
//
void PIO_Init(void)
{

    GpioControl.CurrentVal  = GIO_GetInpVal();
    GpioControl.PreviousVal  = GpioControl.CurrentVal ;

    GpioControl.CfgIo[0]= ( CFG_DIO_TRIGGER_LEVEL | CFG_DIO_REPORT_EVENT | CFG_DIO_REPORT_PERIODIC_MESSAGE );
    GpioControl.CfgIo[1]= ( CFG_DIO_TRIGGER_LEVEL | CFG_DIO_REPORT_EVENT | CFG_DIO_REPORT_PERIODIC_MESSAGE );
    GpioControl.CfgIo[2]= ( CFG_DIO_TRIGGER_LEVEL | CFG_DIO_REPORT_EVENT | CFG_DIO_REPORT_PERIODIC_MESSAGE );
    GpioControl.CfgIo[3]= ( CFG_DIO_TRIGGER_LEVEL | CFG_DIO_REPORT_EVENT | CFG_DIO_REPORT_PERIODIC_MESSAGE );
    GpioControl.CfgIo[4]= ( CFG_DIO_TRIGGER_LEVEL | CFG_DIO_REPORT_EVENT | CFG_DIO_REPORT_PERIODIC_MESSAGE );
    GpioControl.CfgIo[5]= ( CFG_DIO_TRIGGER_LEVEL | CFG_DIO_REPORT_EVENT | CFG_DIO_REPORT_PERIODIC_MESSAGE );
    
    
    
    GpioControl.PeriodSetPoint = PIO_PERIODIC_REPORT;
    GpioControl.PeriodCounter	 = GpioControl.PeriodSetPoint;
    
	  GpioTimerId = TMR_SetTimer ( PIO_PERIODIC_TIMERVAL , PROC_PIO, 0x00, FALSE);
    
	  return;
}

#pragma CODE_SEG DEFAULT

#endif 	// MOD_INPUT_OUTPUT_ENABLED
