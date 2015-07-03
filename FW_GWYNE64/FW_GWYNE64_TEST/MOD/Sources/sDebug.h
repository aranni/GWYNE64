
#ifndef sDebugH
#define sDebugH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define   DBG_PERIODIC_TIMERVAL   1000


//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
INT8  DebugBuffer [MAX_DEBUGBUF_LEN + 1];
UINT8 DebugCntr;


//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void f_DebugPeriodic    (void);
void f_DebugsSendDbgMsg (void);
void f_DebugsSendDbgPrn (void);

void MON_ResetIfNotConnected (void);
void MON_ResetBoard (void);
void DBG_Init(void);

#endif      // sDebugH
