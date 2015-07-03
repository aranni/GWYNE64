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
UINT16            PeriodicTimer;
UINT16	          DummyVar;
void  *           DummyPtr;

UINT8             RestoreSwitchCounter;

//UINT8             SCI1_InBuff [MAX_SERIAL_IN_BUFLEN];
//UINT8             SCI1_OutBuff[MAX_SERIAL_OUT_BUFLEN];
//UINT8             SCI2_InBuff [MAX_SERIAL_IN_BUFLEN];
//UINT8             SCI2_OutBuff[MAX_SERIAL_OUT_BUFLEN];

//INVERTER_PARAM    InverterParam;        // Parametros del inversor
SYS_CONFIG_PARAM  SYS_ConfigParam;			// Parametros de configuracion del sistema
STATISTIC_MSG     StatisticData;				// Variables de Estadisticas
INV_CONFIG_PARAM  INV_ConfigParam;      // Parametros de configuracion del variador.
UINT32            dwIntegrityMonitor;

UINT8 bac_data[MAX_BAC_BUF];

UINT8  HwVer[SIZE_HW_VER+1];
UINT8  HwSerial[SIZE_SERIAL+1];
														
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
