
#ifndef sTimerH
#define sTimerH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define 	NULL_NODE		(UINT8)(-1)


//------------------------------------------------------------------------------------
// Estructuras de DATOS
//------------------------------------------------------------------------------------
//
typedef struct {
	UINT8		Prev;
	UINT8		Next;
	UINT16	Value;
	UINT16	Reload;
	UINT8		OwnerTask;
	UINT8		Used;
	void    (*TimeoutFuncPrt)(void);
} TMR_NODES;


//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
TMR_NODES	  TmrArray[TMR_MAX_NODES];
UINT8		    TmrAvailable;
UINT8		    TmrInUse;
UINT16	    TmrWaitVar;


//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
#pragma CODE_SEG NON_BANKED
void 	TMR_Init        (void);
UINT8	TMR_SetTimer 	  ( UINT16 Interval, UINT8 Owner, void(*FuncPrt)(void), UINT8 Periodic);
void 	TMR_FreeTimer   (UINT8 TimerId);

void 	TMR_ReloadTimer (UINT8 TimerId, UINT16 Interval);
void 	TMR_Wait 		    (UINT16 DelayMS);

void 	TMR_Polling (void);


interrupt void tim0_ch7_isr(void);
#pragma CODE_SEG DEFAULT

#endif          // sTimerH