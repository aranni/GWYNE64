#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"
#include "Commdef.h"

#ifdef  MOD_MODBUSRTU_SLAVE_ENABLED   // Generar aplicación que utiliza MODBUSRTU_SLAVE

#include "pModbusSlaveRtu.h"
#include "Global.h"

///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_MODBUS_SLAVE_RTU
///////////////////////////////////////////////////////////////////////////////
//
const EVENT_FUNCTION	ModbusSlaveRtuProc_IDLE [] =
{
//{EVT_TMR_TIMEOUT 	    , f_ModbusSlaveRtuIdleTimeout	    },
//{EVT_MODRTU_ERR 	    , f_ModbusSlaveRtuIdleTimeout	    },
{EVT_SCI_RXMSG 	      , f_ModbusSlaveRtuIdleAnalizeMsg  },
{EVT_SCI_TXMSG 	      , f_ModbusSlaveRtuIdleTxedMsg     },
{EVT_SCI_TXERR 	      , f_ModbusSlaveRtuIdleTxedMsg     },
{EVT_MODRTU_ERR	      , f_ModbusSlaveRtuIdleTxedMsg     },
{EVT_MODRTU_TXMSG 	  , f_ModbusSlaveRtuIdleSendMsg     },
{EVT_OTHER_MSG     		, f_Consume               }};

  
///////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_MODBUS_SLAVE_RTU
///////////////////////////////////////////////////////////////////////////////
//
//-------------- estado : IDLE ----------------------------------------------------------------
//
//-----------------------------------------------------------------------------
// Analizar mensaje Modbus recibido
//-----------------------------------------------------------------------------
//
void f_ModbusSlaveRtuIdleAnalizeMsg (void)
{
    
    ModbusRtuSlaveMsgToken = (UINT8)CurrentMsg.Param;

    // Marcar el buffer con el ID del proceso..
    (void)MemSetRouteMark(ModbusRtuSlaveMsgToken, PROC_MODBUS_SLAVE_RTU);
    
    // Enviar mensaje al proceso NEXO para que redireccione la encuesta
    PutEventMsg (EVT_MODRTU_MSG, PROC_NEXO, PROC_MODBUS_SLAVE_RTU, ModbusRtuSlaveMsgToken);          

    //ModbusRtuSlaveTimerId = TMR_SetTimer ( 10000 , PROC_MODBUS_SLAVE_RTU, 0x00, FALSE );	
 
 	  return;
}


//-----------------------------------------------------------------------------
// Respuesta modbus transmitida a su encuestador, reutilizar el buffer para esperar recepcion de la proxima encuesta
//-----------------------------------------------------------------------------
//
void f_ModbusSlaveRtuIdleTxedMsg (void)
{
    
    UINT16  wSenRequestParam;
    UINT8   RxModBusRtuAnswerToken;
    
    RxModBusRtuAnswerToken = (UINT8)CurrentMsg.Param;
    
    //(void)MemFreeBuffer (RxModBusRtuAnswerToken);   // Liberar el buffer

    // Reutilizar el mismo buffer para seguir recibiendo encuestas
    wSenRequestParam = 0;     // En la parte alta del parámetro se pasa el offset de transmisión
    wSenRequestParam <<= 8;
    wSenRequestParam |= RxModBusRtuAnswerToken;
    
    (void)MemSetUsedLen(RxModBusRtuAnswerToken, 0x08);   // Recibir por lo menos 8 bytes
      
    
    PutEventMsg (EVT_SCI_RXALL, MODRTU_SLV_TargetSci, PROC_MODBUS_SLAVE_RTU, wSenRequestParam );
  
 	  return;
}
		

//-----------------------------------------------------------------------------
// Timeout en intento de transmisión de respuesta
//-----------------------------------------------------------------------------
//
//void f_ModbusSlaveRtuIdleTimeout (void) 
//{
//    UINT16  wSenRequestParam;
//
    // TODO: modificar mensaje, agregar excepcion y devolver al puerto serie...
  
//    wSenRequestParam = 0;     // En la parte alta del parámetro se pasa el offset de transmisión
//    wSenRequestParam <<= 8;
//    wSenRequestParam |= ModbusRtuSlaveMsgToken;

    // Transmitir respuesta 
//    PutEventMsg (EVT_SCI_TXREQ, MODRTU_SLV_TargetSci, PROC_MODBUS_SLAVE_RTU, wSenRequestParam );
  
//	return;
//}


//-----------------------------------------------------------------------------
// Transmitir o Retransmitir mensaje Modbus recibido
//-----------------------------------------------------------------------------
//
void f_ModbusSlaveRtuIdleSendMsg (void)
{

    UINT8   RxModBusRtuAnswerToken;
    UINT16  wSenRequestParam;
    
    //SETBIT(dwIntegrityMonitor,PROC_MODBUS_SLAVE_RTU);
    ProcDebugFlag[PROC_MODBUS_SLAVE_RTU] &= ~DBGFLAG_ACTIVITY_SIGN_MASK;
    ProcDebugFlag[PROC_MODBUS_SLAVE_RTU] |= DBGFLAG_ACTIVITY_SIGN;
    
    RxModBusRtuAnswerToken = (UINT8)CurrentMsg.Param;
  
    wSenRequestParam = 0;     // En la parte alta del parámetro se pasa el offset de transmisión
    wSenRequestParam <<= 8;
    wSenRequestParam |= RxModBusRtuAnswerToken;

    // Transmitir respuesta como comando, para volver a esperar encuesta
    PutEventMsg (EVT_SCI_TXREQ, MODRTU_SLV_TargetSci, PROC_MODBUS_SLAVE_RTU, wSenRequestParam );
      
 	return;
}
		


///////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// MOD_ModbusSlaveRtuInit : inicializacion de variables del proceso
//-----------------------------------------------------------------------------
//
void MOD_ModbusSlaveRtuInit(void)
{
    UINT8 ModbusSlaveRtuEnableWatchdog;
    
    ModbusRtuSlaveTimerId = 0xFF;
    MODRTU_SLV_TargetSci  = PROC_SCIB;

 	  ModbusSlaveRtuEnableWatchdog  = SYS_ConfigParam.RunConfig & CFG_RTU_RUNSTATE_MASK;                
    ProcDebugFlag[PROC_MODBUS_SLAVE_RTU] = 0x00;
  	if (ModbusSlaveRtuEnableWatchdog)
      ProcDebugFlag[PROC_MODBUS_SLAVE_RTU] |= DBGFLAG_ACTIVITY_CHECK;  
	  
    // ABR_TIP: agregar luego seleccion de puerto dinámicamente, para cada solución...
    SCIB_Init (PROC_MODBUS_SLAVE_RTU, SCI_FRAME_MODBUSRTU_SLV, EVT_SCI_RXMSG, EVT_SCI_RXERR, EVT_SCI_TXMSG, EVT_SCI_TXERR, EVT_SCI_TXBUSY);
    //SCIB_Init(PROC_MODBUS_SLAVE_RTU, SCI_FRAME_MODBUSRTU_SLV);
    
	  return;
}



#pragma CODE_SEG DEFAULT

#endif  //  (MOD_MODBUSRTU_ENABLED)   Generar aplicación que utiliza MODBUSRTU_SLAVE
