#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"

#include "Commdef.h"
#include "sDebug.h"
#include "Global.h"

#define DBG_WATCH_LED

#pragma CODE_SEG NON_BANKED

//-----------------------------------------------------------------------------
// Estados del proceso PROC_DEBBUG
//-----------------------------------------------------------------------------
//
const EVENT_FUNCTION	DebugProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_DebugPeriodic	      },
{EVT_DBG_TXMSG 	      , f_DebugsSendDbgMsg    },
{EVT_LOG_SAVE         , f_DebugsSaveLog       },
{EVT_LOG_EVENT_SAVE   , f_DebugsSaveLogEvent  },
{EVT_LOG_EVENT_ID     , f_DebugsSaveLogEventId},
{EVT_LOG_ID           , f_DebugsSaveLogId     },
{EVT_OTHER_MSG     		, f_Consume             }};

	 
//-----------------------------------------------------------------------------
// Transiciones para el proceso PROC_DEBUG
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
// Registro de logs de debug 
//-----------------------------------------------------------------------------
//

void f_DebugsSaveLog (void)
{
  UINT8   MsgToken;

// El token del buffer está en la parte baja del parámetro del mensaje
  MsgToken  = (UINT8)CurrentMsg.Param;
  FLA_SaveLog(MsgToken);
  
  //(void)MemFreeBuffer (MsgToken);   // TODO: agregar tratamiento de logs, y eliminar esto...

  return;
}

void f_DebugsSaveLogId (void)
{
  UINT8   LogId;
  UINT8   SourceId;

// El id del log está en la parte baja del parámetro del mensaje
  LogId  = (UINT8)CurrentMsg.Param;
  SourceId  = CurrentMsg.Source;
  FLA_SaveLogFixed(LogId, SourceId);   // TODO: agregar tratamiento de logs y descomentar

  return;
}

//-----------------------------------------------------------------------------
// Registro de logs de eventos 
//-----------------------------------------------------------------------------
//
void f_DebugsSaveLogEvent (void)
{
  UINT8   MsgToken;

// El token del buffer está en la parte baja del parámetro del mensaje
  MsgToken  = (UINT8)CurrentMsg.Param;
  FLA_SaveLogEvent(MsgToken);
  
  return;
}


void f_DebugsSaveLogEventId (void)
{
  UINT8   LogId;
  UINT8   SourceId;

// El id del log está en la parte baja del parámetro del mensaje
  LogId  = (UINT8)CurrentMsg.Param;
  SourceId  = CurrentMsg.Source;
  FLA_SaveLogEventFixed(LogId, SourceId);   // TODO: agregar tratamiento de logs y descomentar

  return;
}


//-----------------------------------------------------------------------------
// Actividades periodica: lectura de entradas, chequeo de integridad, etc 
//-----------------------------------------------------------------------------
//
void f_DebugPeriodic (void)
{

  UINT8 procIdxCntr;
  
  
//  IIC_RtcGetDateTime();	
  GIO_GetTemp();
  GIO_GetAAGInp();
  (void)GIO_GetInpVal();        
  GIO_SetOutVal((UINT8)InputOutputData.DigOutVal);
  IIC_DacSetValues ();

  dwUpTimeHalfSeg++;
  wDebugCheckIntegCounter++;
  if (wDebugCheckIntegCounter > DBG_CHECKINTEG_TICKS) 
  {
      // Chequear variable de integridad de procesos..     
      for ( procIdxCntr=1; procIdxCntr<PROCESS_QTTY ; procIdxCntr++) 
      {
        if ( ProcDebugFlag[procIdxCntr] & DBGFLAG_ACTIVITY_CHECK ){
        
          if ((ProcDebugFlag[procIdxCntr] & DBGFLAG_ACTIVITY_SIGN_MASK ) != DBGFLAG_ACTIVITY_SIGN ) MON_ResetIfNotConnected(procIdxCntr);
          
          ProcDebugFlag[procIdxCntr] |= DBGFLAG_ACTIVITY_SIGN_MASK;      
        }      
      }        
      wDebugCheckIntegCounter = 0x00;
  }
  
  //LED_GREEN_TOG;
  
#ifdef   TCP_SLAVE_MODE
  (void)TCP_VerifyCliTcpConnection();
#endif   
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


///////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// DBG_Init: inicializacion de variables del proceso
//-----------------------------------------------------------------------------
//
void DBG_Init(void)
{
  DDRL_DDRL2 = 1;
  DebugCntr = 0x00;
  wDebugCheckIntegCounter = 0x00;
  
#ifdef  DBG_WATCH_LED
	(void)TMR_SetTimer ( DBG_PERIODIC_TIMERVAL , PROC_DEBUG, 0x00, TRUE);
	dwUpTimeHalfSeg= 0;
#endif

}

//-----------------------------------------------------------------------------
// MON_ResetIfNotConnected: RESET DEL EQUIPO si no hay conexión en TCP_Config
//-----------------------------------------------------------------------------
//
void MON_ResetIfNotConnected (BYTE bResetReason){
  // resetear si no hay conexion con IMPConfig
  if (!TcpConfigState)
  {    
		 MON_ResetBoard(bResetReason);		 
  }
  else 
  {
#if DEBUG_AT(DBG_LEVEL_1)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_1)
  {                    
		  (void)sprintf((char *)udp_buf,">>>RESET !! Code: %X \n",
		                bResetReason );		                
      udp_data_to_send = TRUE;
  }
#endif  		                                
  }
}

//-----------------------------------------------------------------------------
// MON_ResetIfNotConnected: RESET DEL EQUIPO
//-----------------------------------------------------------------------------
//
void MON_ResetBoard(BYTE bResetReason){
  UINT8   ResetFlag;
  UINT16  ResetCntr;

  ResetFlag = bResetReason;  
  eeprom_write_buffer(EE_ADDR_RST_FLAG, &ResetFlag, 1); 
  eeprom_read_buffer(EE_ADDR_RST_CNTR,(UINT8 *)&ResetCntr, 2); 
  ResetCntr++;
  eeprom_write_buffer(EE_ADDR_RST_CNTR, (UINT8 *)&ResetCntr, 2); 
            
  __asm SEI;                         // Deshabilitar interrupciones (RESET)
  FOREVER;    
  return;
}

#pragma CODE_SEG DEFAULT


