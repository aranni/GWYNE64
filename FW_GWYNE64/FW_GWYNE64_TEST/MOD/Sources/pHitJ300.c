#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"
#include "float.h"

#include "Commdef.h"
#include "pHitJ300.h"
#include "Global.h"

#pragma MESSAGE DISABLE C5919 //WARNING C5919: Conversion of floating to unsigned int

///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_HITACHI
///////////////////////////////////////////////////////////////////////////////
//
const EVENT_FUNCTION	HitachiProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_HitachiIdleTimeout},
{EVT_SIE_RXFRAME 	    , f_HitachiAnalizeMsg },
{EVT_VAR_CMDSTART	    , f_HitachiCmdStart   },
{EVT_VAR_CMDSTOP	    , f_HitachiCmdStop    },
{EVT_VAR_CMDSETFREC   , f_HitachiCmdSetFrec },
{EVT_VAR_CMDDEBUG     , f_HitachiCmdDebug   },
{EVT_OTHER_MSG     		, f_Consume           }};



///////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_HITACHI
///////////////////////////////////////////////////////////////////////////////
//
void f_HitachiIdleTimeout (void)
{
  
  if (!HIT_CmpPending){
    HIT_BuildBasicRequest(HIT_PollingIdx);
    HIT_BuildBCC(); 
  }
  HIT_CmpPending = 0x00;   

  PutEventMsg (EVT_485_TXREQ, PROC_SERIAL, PROC_HITACHI, 0x00);
	return;
}


void f_HitachiAnalizeMsg (void)
{
  UINT8   ReplyId;
  UINT16  TmpValue;
  
  ReplyId = LookForCmdId();
  
//  if ((!(CurrentMsg.Param)) && (ReplyId != IDX_HIT_MAX_PARAM)){				 // Recibido sin error
  if (!(CurrentMsg.Param)){				 // Recibido sin error
    
    switch (ReplyId){
    
      case (IDX_HIT_Monitor):
      	InverterParam.RealFrec    = SYS_StrToFloat(4,2,&(pHitachiInMsg->Data[6]));
      	InverterParam.OutCurrent  = SYS_StrToFloat(4,1,&(pHitachiInMsg->Data[19]));
      	InverterParam.ParReal     = SYS_StrToFloat(3,0,&(pHitachiInMsg->Data[39]));
      	InverterParam.Polos       = SYS_StrToInt  (2,&(pHitachiInMsg->Data[12]));
      	
      	InverterParam.StatusWord = 0x00;
      	TmpValue = SYS_StrToInt  (2,&(pHitachiInMsg->Data[0]));     // Status Word 1
      	if (TmpValue & 0x02)
      	   InverterParam.StatusWord |= STATUS_RUN_BIT;
      	if (TmpValue & 0x04)
      	   InverterParam.StatusWord |= STATUS_FAULT_BIT;
      	
//      	TmpValue = SYS_StrToInt  (2,&(pHitachiInMsg->Data[2]));     // Status Word 2
//      	if (TmpValue & 0x02)
//      	   InverterParam.StatusWord |= STATUS_READY_BIT;
      	
        break;
        
      case (IDX_HIT_MinMaxFrec):
      	InverterParam.MaxFrec    = SYS_StrToFloat(4,1,&(pHitachiInMsg->Data[0]));
      	InverterParam.MinFrec    = SYS_StrToFloat(4,1,&(pHitachiInMsg->Data[5]));
        break;

      case (IDX_HIT_READTIME):
      	InverterParam.HorasRun   = SYS_StrToInt  (2,&(pHitachiInMsg->Data[13]));
      	InverterParam.HorasRun   *= 365;         // anios a dias
      	InverterParam.HorasRun   *= 24;					// dias a horas
      	InverterParam.HorasRun   += ((SYS_StrToInt(2,&(pHitachiInMsg->Data[16]))) * 24);
        break;
     
      case (IDX_HIT_READTRIP):
      	InverterParam.UltimaFalla = SYS_StrxToInt  (2,&(pHitachiInMsg->Data[3]));
      	InverterParam.ValorFalla  = 0x00;
      	InverterParam.HoraFalla   = SYS_StrToInt  (2,&(pHitachiInMsg->Data[20]));
      	InverterParam.HoraFalla   *= 365;         // anios a dias
      	InverterParam.HoraFalla   *= 24;					// dias a horas
      	InverterParam.HoraFalla   += ((SYS_StrToInt(2,&(pHitachiInMsg->Data[23]))) * 24);
        break;
              
      default:
      			//----
 //     	InverterParam.StatusWordB = SYS_StrToInt  (2,(UINT8 *)&(pHitachiInMsg->RC));
        break;
    }  
  } 
  else{														 // Timeout en RS485
    SRL_ReInit();    
  }

  HIT_PollingIdx++;
  if (HIT_PollingIdx >= IDX_HIT_MAX_PARAM){
    HIT_PollingIdx =  IDX_HIT_Monitor;
    PeriodicTimer = SYS_ConfigParam.ScanRate;
//	DummyVar =  TMR_SetTimer ( SYS_ConfigParam.ScanRate , PROC_SIEMENS, 0x00, FALSE);
  } 
  else{
    PeriodicTimer = HIT_TMR_TXREQ_DELAY;
//	DummyVar =  TMR_SetTimer ( TMR_TXREQ_DELAY , PROC_SIEMENS, 0x00, FALSE);
  }
   
	return;
	
}

void f_HitachiCmdStart (void)
{
  
  HIT_CmpPending = 0x01;
  
  HIT_BuildBasicRequest(IDX_HIT_CMD_RUN);
  pHitachiOutMsg->Data[0] = CMD_RUN_FORWARD;
  HIT_BuildBCC();
  return;
}


void f_HitachiCmdStop (void)
{

  HIT_CmpPending = 0x01;
  
  HIT_BuildBasicRequest(IDX_HIT_CMD_RUN);
  pHitachiOutMsg->Data[0] = CMD_RUN_STOP;
  HIT_BuildBCC();
  return;
}


void f_HitachiCmdSetFrec (void)
{
  UINT16  TmpWord;
  float   TmpFloat;

  HIT_CmpPending = 0x01;
  
  TmpFloat = SYS_ConfigParam.ConsignaFrec;
  TmpWord = (UINT16)(SYS_ConfigParam.ConsignaFrec);
  SYS_IntToStr ( TmpWord , 4, &(pHitachiOutMsg->Data[0]) );
  
  TmpFloat -= TmpWord;
  TmpFloat *= 100;
  TmpWord = (UINT16)(TmpFloat);
  SYS_IntToStr ( TmpWord , 2, &(pHitachiOutMsg->Data[4]) );
  
  HIT_BuildBasicRequest(IDX_HIT_CMD_SETFREC);
  HIT_BuildBCC();
  return;
}


void f_HitachiCmdDebug (void)
{

  HIT_CmpPending = 0x01;
  
  HIT_BuildBasicRequest(IDX_HIT_CMD_DEBUG);
  HIT_BuildBCC();
  return;
}



//    if (HIT_PollingIdx == IDX_HIT_CMD_SETMODE){
//	      memcpy (pHitachiOutMsg->Data, "OP1OP1",6);
//    }



///////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// HIT_Init : inicializacion de variables del proceso
//-----------------------------------------------------------------------------
//
void HIT_Init(void)
{

  HIT_PollingIdx =  IDX_HIT_Monitor;
  HIT_CmpPending = 0x00;

  pHitachiInMsg  = (HITACHI_IN_MSG*)SCI1_InBuff;
  pHitachiOutMsg = (HITACHI_OUT_MSG*)SCI1_OutBuff;  
  
//	DummyVar =  TMR_SetTimer ( 1000 , PROC_SIEMENS, 0x00, FALSE);
		
}

//---------------------------------------------------------------------------
// HIT_BuildBasicRequest : cargar los campos comunes de la trama de salida
//---------------------------------------------------------------------------
//
void HIT_BuildBasicRequest (UINT8 ListIdx)
{
  pHitachiOutMsg->TxLen = HIT_ParamData[ListIdx].TransmitLen;
  pHitachiOutMsg->RxLen = HIT_ParamData[ListIdx].ResponseLen;
  
	pHitachiOutMsg->IniFlag = HIT_START_FLAG;
	
	DummyPtr = memcpy (pHitachiOutMsg->Addr, HIT_DEFAULT_SLAVE,2);
	DummyPtr = memcpy (pHitachiOutMsg->Command, HIT_ParamData[ListIdx].Command,HIT_CMD_LEN);
  
  return;
}

//---------------------------------------------------------------------------
// HIT_BuildBCC : preparar el campo de checksum para la trama de salida
//---------------------------------------------------------------------------
//
void HIT_BuildBCC (void) {

  UINT8 UsefulDataCounter;
  UINT8 * pOutDataMsg;
  UINT8 TmpSum;
  UINT8 TmpSumHex;

  
  TmpSum = 0x00;
  UsefulDataCounter = pHitachiOutMsg->TxLen - 4;
  pOutDataMsg = (UINT8 *)(pHitachiOutMsg->Addr);
  
  while(UsefulDataCounter--){
    TmpSum += *pOutDataMsg;
    pOutDataMsg++;
  }  
  
  TmpSumHex = (UINT8)(TmpSum / 16);
  *pOutDataMsg = SYS_HexChar[TmpSumHex]; 
  pOutDataMsg++;
  
  TmpSum -= ( TmpSumHex * 16 );
  *pOutDataMsg = SYS_HexChar[TmpSum]; 
  
  pOutDataMsg++;
  *pOutDataMsg = HIT_END_FLAG; 

}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

UINT8 LookForCmdId(void)
{
  UINT8 IdxCntr;
  UINT8 * pInMsg;
  pInMsg = (UINT8 *)(&(pHitachiInMsg->Command));

 for(IdxCntr=0 ; IdxCntr < IDX_HIT_MAX_PARAM ; IdxCntr++ ){
	if(strncmp((INT8 *)(HIT_ParamData[IdxCntr].Command), (INT8 *)pInMsg, HIT_CMD_LEN) == 0)  
		return  (HIT_ParamData[IdxCntr].CmdId);
	}
  return IDX_HIT_MAX_PARAM;
}

#pragma CODE_SEG DEFAULT

//	DummyVar = sprintf(Message,"%#5.2f",NominalCurrent);
