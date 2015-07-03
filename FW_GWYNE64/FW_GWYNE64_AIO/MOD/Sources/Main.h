#ifndef mainH
#define mainH


#define MAX_RESTORESWITCH_CNTR    10

//-----------------------------------------------------------------------------
// Declaraciones de Estructuras de mensajes
//-----------------------------------------------------------------------------
EVENT_MSG	      EventMsgQueue [MAX_EVENTMSG_QUEUE];		// Cola de Mensajes de EVENTOS
FIFO_CNTRL			EventMsgCtrl;		 					// Variables para control de EventMsgQueue
EVENT_MSG	   	 	CurrentMsg;								// Mensaje actual analizandose.

//-----------------------------------------------------------------------------
// Declaraciones de Punteros a estructuras
//-----------------------------------------------------------------------------
EVENT_FUNCTION *  CurrentProcessState [PROCESS_QTTY];	// Array de Punteros a Estados de los procesos
EVENT_FUNCTION *  EventInCurrentState;            // EVENTO en Estado Actual de Cureent Process.
EVENT_FUNCTION *  FirstEventInCurrentState;				// 1er EVENTO en Estado Actual de Cureent Process.


//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
UINT16 	          TimerVal;
UINT16	          DummyVar;
void  *           DummyPtr;

UINT8             RestoreSwitchCounter;
SYS_CONFIG_PARAM  SYS_ConfigParam;			    // Parametros de configuracion del sistema
STATISTIC_MSG     StatisticData;				    // Variables de Estadisticas
//UINT32            dwIntegrityMonitor;
UINT8             ProcDebugFlag[PROCESS_QTTY];
	
#ifdef MOD_MODBUSRTU_AUTOQUERY_ENABLED		
  UINT8             autoQueryModbusData[MAX_AUTOQUERY_BUF];
#endif	

#ifdef MOD_SYMEO_ENABLED		
//  UINT8             symeoData[MAX_SYMEO_BUF];
#endif														
													
//-----------------------------------------------------------------------------
// Declaraciones de Punteros a funciones
//-----------------------------------------------------------------------------
void (*EventFunction)  (void);

#pragma CODE_SEG NON_BANKED        

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void  main          (void);
UINT8 GetEventMsg   (void);
void  PutEventMsg   (UINT8 PutEvent, UINT8 PutTarget, UINT8 PutSource, UINT16 PutParam);
void  f_Consume     (void);
void  RestoreConfig (void);

#pragma CODE_SEG DEFAULT

#endif			//  mainH
