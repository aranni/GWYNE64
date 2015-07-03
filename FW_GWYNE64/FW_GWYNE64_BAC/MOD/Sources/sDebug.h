
#ifndef sDebugH
#define sDebugH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define   DBG_PERIODIC_TIMERVAL   1000        // 1 segundo
#define   DBG_CHECKINTEG_TICKS    60*20       // 20 Minutos


//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
UINT8   DebugCntr;
//INT8    DebugBuffer [MAX_DEBUGBUF_LEN + 1];
UINT16  wDebugCheckIntegCounter;


//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
#pragma CODE_SEG NON_BANKED

void f_DebugPeriodic    (void);
void f_DebugsSendDbgMsg (void);
void f_DebugsSendDbgPrn (void);
void f_DebugsSaveLog    (void);
void f_DebugsSaveLogId  (void);

void MON_ResetIfNotConnected (void);
void MON_ResetBoard (void);
void DBG_Init(void);

#pragma CODE_SEG DEFAULT

#endif      // sDebugH
