#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"

#include "Commdef.h"
#include "sDebug.h"
#include "Global.h"

#define DBG_WATCH_LED

//-----------------------------------------------------------------------------
// Estados del proceso PROC_DEBBUG
//-----------------------------------------------------------------------------
//
const EVENT_FUNCTION	DebugProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_DebugPeriodic	},
{EVT_DBG_TXMSG 	      , f_DebugsSendDbgMsg},
{EVT_DBG_TXPRN 	      , f_DebugsSendDbgPrn},
{EVT_OTHER_MSG     		, f_Consume         }};
	 
//-----------------------------------------------------------------------------
// Transiciones para el proceso PROC_DEBUG
//-----------------------------------------------------------------------------
void f_DebugPeriodic (void)
{

//  IIC_RtcGetDateTime();	
  GIO_GetTemp();
  GIO_GetAAGInp();
  GIO_GetInpVal();        
  GIO_SetOutVal((UINT8)InverterParam.OutVal);
//  IIC_DacSetValues ();
   
	return;
}


void f_DebugsSendDbgMsg (void)
{
                     
  switch ( CurrentMsg.Param ){
  
    case DBG_BY_SERIAL:
//      SCI_SendMsg((UINT8 *)DebugBuffer);   
      break;
      
    case DBG_BY_TCP:
//      TCP_SendDebugMsg((UINT8 *)DebugBuffer);   
      break;
      
    default:
      break;
  }
  
	return;
}


void f_DebugsSendDbgPrn (void)
{

  switch ( CurrentMsg.Param ){
  
    case DBG_BY_SERIAL:
//      SCI_SendMsg((UINT8 *)DebugBuffer);   
      break;
      
    case DBG_BY_TCP:
//      TCP_SendDebugMsg((UINT8 *)DebugBuffer);   
      break;
      
    default:
      break;
  }

  return;
}

void MON_ResetIfNotConnected (void){
  if (!TcpConfigState)    // resetear si no hay conexion con IMPConfig
		 MON_ResetBoard();		 
}

void MON_ResetBoard(void){
  UINT8   ResetFlag;
  UINT16  ResetCntr;

  ResetFlag = 0x01;  
  eeprom_write_buffer(EE_ADDR_RST_FLAG, &ResetFlag, 1); 
  eeprom_read_buffer(EE_ADDR_RST_CNTR,(UINT8 *)&ResetCntr, 2); 
  ResetCntr++;
  eeprom_write_buffer(EE_ADDR_RST_CNTR, (UINT8 *)&ResetCntr, 2); 
            
  __asm SEI;                         // Deshabilitar interrupciones (RESET)
  FOREVER;    
  return;
}

//-----------------------------------------------------------------------------
// Funciones Auxiliares del Proceso
//-----------------------------------------------------------------------------
//
void DBG_Init(void)
{
  DDRL_DDRL2 = 1;
  DebugCntr = 0x00;

#ifdef  DBG_WATCH_LED
	(void)TMR_SetTimer ( DBG_PERIODIC_TIMERVAL , PROC_DEBUG, 0x00, TRUE);
#endif

}

#pragma CODE_SEG DEFAULT


