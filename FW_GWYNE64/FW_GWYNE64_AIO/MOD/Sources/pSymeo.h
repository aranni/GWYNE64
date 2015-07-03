
#ifndef pSymeoH
#define pSymeoH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//

#define TMR_SYM_RESETLINKWAIT  5000		// Tiempo de espera entre cada comando
#define MAX_SYM_RESETATTEMP		   60   // Cantidad de resets sin respuesta antes de ResetBoard
#define TIMEOUT_WAIT_ANSWER    5000   // Tiempo de espera de respuesta al comando


//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
SYMEO_BANK_ALLOC  SYM_BankMemManager[MAX_SYMEO_BANKS];
UINT8             SYM_RxMsgToken;    
/*
UINT8   SYM_PollingNode;	
UINT8   SYM_EnablePolling;				  // Habilitar polling
UINT16  SYM_PollingPeriod;				  // Periodo de polling en milisegundos
UINT8   SYM_CtrlFlags;				      // Habilitar polling
*/
UINT8   SYM_WaitAnswerTimerId;
UINT16  SYM_PollingTimeout;				  // Timeout en TX en milisegundos


UINT8   SYM_IdxPoll;				        // Index  polling

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//

// IDLE
void f_MOD_SymeoIdleTimeout  (void);
void f_MOD_SymeoTcpIdleAnalizeMsg (void);

// Funciones Auxiliares
void MOD_SymeoInit          (void);
//void MOD_SymeoResetBanks    (void); 
//void MOD_SymeoSaveBanks     (void);

#endif				 // pSymeoH