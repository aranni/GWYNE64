
#ifndef pModbusAutoQueryH
#define pModbusAutoQueryH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//

#define TMR_MAQ_RESETLINKWAIT  5000		// Tiempo de espera entre cada comando
#define MAX_MAQ_RESETATTEMP		   60   // Cantidad de resets sin respuesta antes de ResetBoard
#define TIMEOUT_WAIT_ANSWER    5000   // Tiempo de espera de respuesta al comando


//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
//UINT8 MAQ_len;
AUTOQUERY_BANK_ALLOC  MAQ_BankMemManager[MAX_AUTOQUERY_BANKS];
//AUTOQUERY_PROC_DIAG   MAQ_Diag;
UINT8                 MAQ_RxMsgToken;    

UINT8   MAQ_PollingNode;	
UINT8   MAQ_EnablePolling;				  // Habilitar polling
UINT16  MAQ_PollingPeriod;				  // Periodo de polling en milisegundos
UINT8   MAQ_CtrlFlags;				      // Habilitar polling

UINT8   MAQ_WaitAnswerTimerId;
UINT16  MAQ_PollingTimeout;				  // Timeout en TX en milisegundos


UINT8   MAQ_IdxPoll;				        // Index  polling

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//

// IDLE
void f_MOD_ModbusRtuAutoQueryIdleTimeout  (void);
void f_MOD_ModbusRtuAutoQueryIdleRxMsgEnd (void);
void f_MOD_ModbusRtuAutoQueryIdleRxMsgErr (void);

// Funciones Auxiliares
void MOD_ModbusRtuAutoQueryInit          (void);
void MOD_ModbusRtuAutoQueryResetBanks    (void); 
void MOD_ModbusRtuAutoQuerySaveBanks     (void);

#endif				 // pModbusAutoQueryH