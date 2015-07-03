
#ifndef sDebugH
#define sDebugH

//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define   DBG_PERIODIC_TIMERVAL   500        // 0.5 segundo
#define   DBG_CHECKINTEG_TICKS    120*5        // 5 Minutos

//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
UINT8   DebugCntr;
UINT16  wDebugCheckIntegCounter;
UINT32  dwUpTimeHalfSeg;

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
#pragma CODE_SEG NON_BANKED

void f_DebugPeriodic    (void);
void f_DebugsSendDbgMsg (void);

void f_DebugsSaveLog    (void);
void f_DebugsSaveLogId  (void);

void f_DebugsSaveLogEvent    (void);
void f_DebugsSaveLogEventId  (void);

void DBG_Init(void);
void MON_ResetIfNotConnected (BYTE bResetReason);
void MON_ResetBoard (BYTE bResetReason);

#pragma CODE_SEG DEFAULT

#endif      // sDebugH
