#include "MC9S12NE64.h"
#include "datatypes.h"

#include "Commdef.h"
#include "sTimer.h"
#include "global.h"

//----------------------------------------------------------------------
// Los timers se implementan con una lista doblemente encadenada. 
// Cada nodo en la lista de timers en uso representa un timer corriendo.
//----------------------------------------------------------------------


void TMR_Init (void)
{
	UINT8	TmpCntr;

	TmrAvailable = 0;
	TmrInUse = NULL_NODE;
#ifdef  DBG_WATCH_OS
	StatisticData.OsStat.TmrInUseCntr = 0x00;
#endif
  StatisticData.OsStat.WatchRstTmr = 0x00;
  
	for (TmpCntr=0 ; TmpCntr < TMR_MAX_NODES ; TmpCntr++){
			TmrArray[TmpCntr].Prev = TmpCntr - 1;
			TmrArray[TmpCntr].Next = TmpCntr + 1;
			TmrArray[TmpCntr].Used = FALSE;
			TmrArray[TmpCntr].Value = 0x00;
			TmrArray[TmpCntr].OwnerTask = 0x00;

	}
	TmrArray[0].Prev = NULL_NODE;
	TmrArray[TMR_MAX_NODES - 1].Next = NULL_NODE;
	
	PeriodicTimer = 0x00;
  
}

UINT8 TMR_SetTimer ( UINT16 Interval, UINT8 Owner, 
                    void(*FuncPrt)(void), UINT8 Periodic )
{
	UINT8	TmrNew;

	if (TmrArray[TmrAvailable].Next == NULL_NODE)			// Si queda un solo timer libre return ERROR
		return NULL_NODE;
	
  __asm SEI;                         // Disable Interrupts
	TmrNew = TmrAvailable;
	TmrAvailable = TmrArray[TmrAvailable].Next;
	TmrArray[TmrAvailable].Prev = NULL_NODE;

	if (TmrInUse == NULL_NODE){	  							// Sera el primer timer en uso de la lista
		TmrInUse = TmrNew;							
		TmrArray[TmrInUse].Prev = NULL_NODE;
		TmrArray[TmrInUse].Next = NULL_NODE;
	}
	else{
		TmrArray[TmrInUse].Prev = TmrNew;
		TmrArray[TmrNew].Prev = NULL_NODE;
		TmrArray[TmrNew].Next = TmrInUse;
		TmrInUse = TmrNew;
	}

	TmrArray[TmrInUse].Value =  Interval;  // / RTD_TICKTIME );
	TmrArray[TmrInUse].OwnerTask = Owner;
	if (Periodic)
		TmrArray[TmrInUse].Reload =  Interval;
	else
		TmrArray[TmrInUse].Reload =  0x00;
	TmrArray[TmrInUse].TimeoutFuncPrt = FuncPrt;
	TmrArray[TmrInUse].Used = TRUE;

  __asm CLI;                         // Enable Interrupts

#ifdef  DBG_WATCH_OS
  StatisticData.OsStat.TmrInUseCntr++;
#endif
	return TmrInUse;											// Retorn el indice del array como timer ID
}  													


void TMR_FreeTimer (UINT8 TimerId)
{
	if ( TimerId == (UINT8)(-1) ) return;
	if ( TmrArray[TimerId].Used == FALSE ) return;

	TmrArray[TimerId].Used = FALSE;
#ifdef  DBG_WATCH_OS
  StatisticData.OsStat.TmrInUseCntr--;
#endif

  __asm SEI;                         // Disable Interrupts
	if (( TmrArray[TimerId].Prev == NULL_NODE ) && 				// Es el unico nodo en uso
		( TmrArray[TimerId].Next == NULL_NODE )) { 
		TmrInUse = NULL_NODE;
		TmrArray[TmrAvailable].Prev = TimerId;
		TmrArray[TimerId].Next = TmrAvailable;
		TmrAvailable = TimerId;
    __asm CLI;                         // Enable Interrupts
		return;
	}

	if ( TmrArray[TimerId].Prev == NULL_NODE ) {				// Es el primer nodo en uso de la lista
		TmrInUse = TmrArray[TimerId].Next;
		TmrArray[TmrInUse].Prev = NULL_NODE;
		TmrArray[TmrAvailable].Prev = TimerId;
		TmrArray[TimerId].Next = TmrAvailable;
		TmrAvailable = TimerId;
    __asm CLI;                         // Enable Interrupts
		return;
	}

	if ( TmrArray[TimerId].Next == NULL_NODE ) {				// Es el ultimo nodo en uso de la lista
		TmrArray[TmrArray[TimerId].Prev].Next = NULL_NODE;
		TmrArray[TmrAvailable].Prev = TimerId;
		TmrArray[TimerId].Next = TmrAvailable;
		TmrArray[TimerId].Prev = NULL_NODE;
		TmrAvailable = TimerId;
    __asm CLI;                         // Enable Interrupts
		return;
	}

	// Si esta en otra posicion dentro de la lista
	TmrArray[TmrArray[TimerId].Prev].Next = TmrArray[TimerId].Next;
	TmrArray[TmrArray[TimerId].Next].Prev = TmrArray[TimerId].Prev;
  TmrArray[TmrAvailable].Prev = TimerId;
	TmrArray[TimerId].Next = TmrAvailable;
	TmrArray[TimerId].Prev = NULL_NODE;
  TmrAvailable = TimerId;
    __asm CLI;                         // Enable Interrupts
  return;
}

void TMR_ReloadTimer (UINT8 TimerId, UINT16 Interval)
{
	if ( TimerId == (UINT8)(-1) ) return;
	if ( TmrArray[TimerId].Used == FALSE ) return;

  __asm SEI;                         // Disable Interrupts
	TmrArray[TimerId].Value =  Interval;
  __asm CLI;                         // Enable Interrupts
  	return;
}


void TMR_Polling (void)
{
	UINT8	TmrIdxTmp;
	UINT8	TmrIdx = TmrInUse;

	if ( TmrInUse == NULL_NODE) return;

	while ( TmrArray[TmrIdx].Next != NULL_NODE ){
		TmrIdxTmp = TmrIdx;
		TmrIdx = TmrArray[TmrIdx].Next;
//		if (!(--TmrArray[TmrIdxTmp].Value)){
//			PutEventMsg (EVT_TMR_TIMEOUT, TmrArray[TmrIdxTmp].OwnerTask, OS_NOT_PROC, TmrIdxTmp);
// 			TMR_FreeTimer (TmrIdxTmp);
//		}
		if (!(--TmrArray[TmrIdxTmp].Value)){
			if (TmrArray[TmrIdxTmp].TimeoutFuncPrt) 
				TmrArray[TmrIdxTmp].TimeoutFuncPrt();
			else
				PutEventMsg (EVT_TMR_TIMEOUT, TmrArray[TmrIdxTmp].OwnerTask, OS_NOT_PROC, TmrIdxTmp);

			if (TmrArray[TmrIdxTmp].Reload){
				TmrArray[TmrIdxTmp].Value = TmrArray[TmrIdxTmp].Reload;
			}
			else{
	 			TMR_FreeTimer (TmrIdxTmp);
			}
		}
	}
	
//if (!(--TmrArray[TmrIdx].Value)){ 
//		PutEventMsg (EVT_TMR_TIMEOUT, TmrArray[TmrIdx].OwnerTask, OS_NOT_PROC, TmrIdx);
// 		TMR_FreeTimer (TmrIdx);
//	}
	if (!(--TmrArray[TmrIdx].Value)){ 
		if (TmrArray[TmrIdx].TimeoutFuncPrt) 
 			TmrArray[TmrIdx].TimeoutFuncPrt();
		else
			PutEventMsg (EVT_TMR_TIMEOUT, TmrArray[TmrIdx].OwnerTask, OS_NOT_PROC, TmrIdx);

		if (TmrArray[TmrIdx].Reload)
			TmrArray[TmrIdx].Value = TmrArray[TmrIdx].Reload;
		else
	 		TMR_FreeTimer (TmrIdx);
	}
	
	
}

void TMR_Wait (UINT16 DelayMS){

	TmrWaitVar = DelayMS;
	while(TmrWaitVar){
    REFRESH_WATCHDOG;
	}
}


// ---------------------------------------------------------------------------
//    TIM0 CH7 ISR
// ---------------------------------------------------------------------------
#pragma CODE_SEG NON_BANKED        
interrupt void tim0_ch7_isr(void)
{

  TFLG1 = 0x80;
  
	TMR_Polling();
	if (TmrWaitVar)	TmrWaitVar--;
	
	if (PeriodicTimer){
	  PeriodicTimer--;  
	  if (PeriodicTimer == 5000){
	    StatisticData.OsStat.WatchRstTmr++;
      SRL_ReInit();	    
	  }
	  
	  if (!PeriodicTimer){
	    PeriodicTimer = 10000;
	    PutEventMsg (EVT_TMR_TIMEOUT, SYS_ConfigParam.InvCode, OS_NOT_PROC, 0x00);
	  }
	}		
        
}

#pragma CODE_SEG DEFAULT


