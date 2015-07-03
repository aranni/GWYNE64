#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"
#include "float.h"

#include "Commdef.h"
#include "pHitSJ300.h"
#include "Global.h"

#pragma MESSAGE DISABLE C5919 //WARNING C5919: Conversion of floating to unsigned int

///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_HIT_SJ300
///////////////////////////////////////////////////////////////////////////////
//
const EVENT_FUNCTION	HitSJ300Proc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_HitSJ300IdleTimeout},
{EVT_SIE_RXFRAME 	    , f_HitSJ300AnalizeMsg },
{EVT_VAR_CMDSTART	    , f_HitSJ300CmdStart   },
{EVT_VAR_CMDSTOP	    , f_HitSJ300CmdStop    },
{EVT_VAR_CMDSETFREC   , f_HitSJ300CmdSetFrec },
{EVT_VAR_CMDDEBUG     , f_HitSJ300CmdDebug   },
{EVT_OTHER_MSG     		, f_Consume           }};



///////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_HIT_SJ300
///////////////////////////////////////////////////////////////////////////////
//
void f_HitSJ300IdleTimeout (void)
{
  
  if (!HSJ_CmpPending){
    HSJ_BuildBasicRequest(HSJ_PollingIdx);
    HSJ_BuildBCC(); 
  }
  else{
    HSJ_PollingIdx = IDX_HSJ_MAX_PARAM;
  }
  
  HSJ_CmpPending = 0x00;   

  PutEventMsg (EVT_485_TXREQ, PROC_SERIAL, PROC_HITACHI, 0x00);
	return;
}


void f_HitSJ300AnalizeMsg (void)
{
  UINT16  TmpValue;
  
//  if ((!(CurrentMsg.Param)) && (ReplyId != IDX_HSJ_MAX_PARAM)){				 // Recibido sin error
  if (!(CurrentMsg.Param)){				 // Recibido sin error
    
    switch (HSJ_PollingIdx){

      case (IDX_HSJ_PAR_STATUS):
      	InverterParam.StatusWord = 0x00;
      	TmpValue = SYS_StrToInt  (2,&(pHitSJ300InMsg->Data[2]));     // Status Word B
      	if (TmpValue == 0x01)
      	   InverterParam.StatusWord |= STATUS_RUN_BIT;
      	if (TmpValue == 0x02)
      	   InverterParam.StatusWord |= STATUS_FAULT_BIT;     	
      	break;

      case (IDX_HSJ_PAR_MAXFREC):
      	InverterParam.MaxFrec    = SYS_StrToFloat(5,2,&(pHitSJ300InMsg->Data[2]));
      	break;
      	
       case (IDX_HSJ_PAR_MINFREC):
      	InverterParam.MinFrec    = SYS_StrToFloat(5,2,&(pHitSJ300InMsg->Data[2]));
      	break;
     	

      case (IDX_HSJ_PAR_READPARAMS):
      	InverterParam.RealFrec    = SYS_StrToFloat(5,2,&(pHitSJ300InMsg->Data[1]));
      	InverterParam.OutCurrent  = SYS_StrToFloat(4,1,&(pHitSJ300InMsg->Data[11]));
        InverterParam.OutVoltage  = SYS_StrToFloat(4,1,&(pHitSJ300InMsg->Data[67]));
        InverterParam.ParReal     = SYS_StrToFloat(3,0,&(pHitSJ300InMsg->Data[61]));
        InverterParam.PotenciaReal= SYS_StrToFloat(4,1,&(pHitSJ300InMsg->Data[75]));
      	InverterParam.HorasRun    = SYS_StrToInt  (5,&(pHitSJ300InMsg->Data[91]));
        break;

       case (IDX_HSJ_PAR_POLOS):
      	InverterParam.Polos       = SYS_StrToInt  (2,&(pHitSJ300InMsg->Data[7]));
      	break;
      	
       case (IDX_HSJ_PAR_POTNOM):
      	TmpValue = SYS_StrToInt(2,&(pHitSJ300InMsg->Data[7]));
      	if ( TmpValue < HSJ_MAX_POTCODES )
      	  InverterParam.Potencia  =  MotorPotCodes[(UINT8)TmpValue];
      	break;
     	
      case (IDX_HSJ_PAR_READTRIP):
      	InverterParam.UltimaFalla = SYS_StrxToInt  (2,&(pHitSJ300InMsg->Data[22]));
      	InverterParam.ValorFalla  = SYS_StrxToInt  (2,&(pHitSJ300InMsg->Data[14]));
      	InverterParam.ValorFalla  <<= 8;
      	InverterParam.ValorFalla  |= SYS_StrxToInt  (2,&(pHitSJ300InMsg->Data[30]));
      	InverterParam.HoraFalla    = SYS_StrToInt  (5,&(pHitSJ300InMsg->Data[51]));
      	break;

      case (IDX_HSJ_MAX_PARAM):						// fue un comando..
      	if (pHitSJ300InMsg->Data[0] != 0x06)
            InverterParam.CmdPending = 0xFF;
      	
        break;
        
      default:
      			//----
          break;
    }    
  } 
  else{														 // Timeout en RS485
    SRL_ReInit();    
  }

  HSJ_PollingIdx++;
  if (HSJ_PollingIdx >= IDX_HSJ_MAX_PARAM){
    HSJ_PollingIdx =  IDX_HSJ_FirstCommand;
    PeriodicTimer = SYS_ConfigParam.ScanRate;
//	DummyVar =  TMR_SetTimer ( SYS_ConfigParam.ScanRate , PROC_SIEMENS, 0x00, FALSE);
  } 
  else{
    PeriodicTimer = HSJ_TMR_TXREQ_DELAY;
//	DummyVar =  TMR_SetTimer ( TMR_TXREQ_DELAY , PROC_SIEMENS, 0x00, FALSE);
  }
   
	return;
	
}

void f_HitSJ300CmdStart (void)
{
  
  HSJ_CmpPending = 0x01;
  
  HSJ_BuildBasicRequest(IDX_HSJ_PAR_FORWARD);
  pHitSJ300OutMsg->Data[0] = HSJ_ParamData[IDX_HSJ_PAR_FORWARD].CmdPar[0];//  CMD_RUN_FORWARD;
  HSJ_BuildBCC();
  return;
}
													

void f_HitSJ300CmdStop (void)
{

  HSJ_CmpPending = 0x01;
  
  HSJ_BuildBasicRequest(IDX_HSJ_PAR_STOP);
  pHitSJ300OutMsg->Data[0] = HSJ_ParamData[IDX_HSJ_PAR_STOP].CmdPar[0]; //CMD_RUN_STOP;
  HSJ_BuildBCC();
  return;
}


void f_HitSJ300CmdSetFrec (void)
{
  UINT16  TmpWord;
  float   TmpFloat;

  HSJ_CmpPending = 0x01;
  HSJ_BuildBasicRequest(IDX_HSJ_PAR_SETFREC);

  TmpFloat = SYS_ConfigParam.ConsignaFrec;
  TmpWord = (UINT16)(SYS_ConfigParam.ConsignaFrec);
  SYS_IntToStr ( TmpWord , 4, &(pHitSJ300OutMsg->Data[0]) );
  
  TmpFloat -= TmpWord;
  TmpFloat *= 100;
  TmpWord = (UINT16)(TmpFloat);
  SYS_IntToStr ( TmpWord , 2, &(pHitSJ300OutMsg->Data[4]) );
  
  HSJ_BuildBCC();

  return;
}


void f_HitSJ300CmdDebug (void)
{

/*
  UINT16  TmpWord;
  float   TmpFloat;

  HSJ_CmpPending = 0x01;
  HSJ_BuildBasicRequest(IDX_HSJ_PAR_SETF001);
  (void)memcpy (pHitSJ300OutMsg->Data, HSJ_ParamData[IDX_HSJ_PAR_SETF001].CmdPar,HSJ_CMD_PARLEN);

  pHitSJ300OutMsg->Data[4] = '0';
  pHitSJ300OutMsg->Data[5] = '0';
  TmpFloat = SYS_ConfigParam.ConsignaFrec;
  TmpWord = (UINT16)(SYS_ConfigParam.ConsignaFrec);
  SYS_IntToStr ( TmpWord , 4, &(pHitSJ300OutMsg->Data[6]) ); //0
  
  TmpFloat -= TmpWord;
  TmpFloat *= 100;
  TmpWord = (UINT16)(TmpFloat);
  SYS_IntToStr ( TmpWord , 2, &(pHitSJ300OutMsg->Data[10]) ); //4
  
  HSJ_BuildBCC();
*/
  return;
}


///////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// HSJ_Init : inicializacion de variables del proceso
//-----------------------------------------------------------------------------
//
void HSJ_Init(void)
{

  HSJ_PollingIdx =  IDX_HSJ_FirstCommand;
  HSJ_CmpPending = 0x00;

  pHitSJ300InMsg  = (HITSJ300_IN_MSG*)SCI1_InBuff;
  pHitSJ300OutMsg = (HITSJ300_OUT_MSG*)SCI1_OutBuff;  
  
//	DummyVar =  TMR_SetTimer ( 1000 , PROC_SIEMENS, 0x00, FALSE);
		
}

//---------------------------------------------------------------------------
// HSJ_BuildBasicRequest : cargar los campos comunes de la trama de salida
//---------------------------------------------------------------------------
//
void HSJ_BuildBasicRequest (UINT8 ListIdx)
{
  pHitSJ300OutMsg->TxLen = HSJ_ParamData[ListIdx].TransmitLen;
  pHitSJ300OutMsg->RxLen = HSJ_ParamData[ListIdx].ResponseLen;
  
	pHitSJ300OutMsg->IniFlag = HSJ_START_FLAG;
	
	DummyPtr = memcpy (pHitSJ300OutMsg->Addr, HSJ_DEFAULT_SLAVE,2);
	DummyPtr = memcpy (pHitSJ300OutMsg->Command, HSJ_ParamData[ListIdx].Command,HSJ_CMD_LEN);
	if ( pHitSJ300OutMsg->TxLen > 8 )
	  DummyPtr = memcpy (pHitSJ300OutMsg->Data, HSJ_ParamData[ListIdx].CmdPar,HSJ_CMD_PARLEN);
  
  return;
}

//---------------------------------------------------------------------------
// HSJ_BuildBCC : preparar el campo de checksum para la trama de salida
//---------------------------------------------------------------------------
//
void HSJ_BuildBCC (void) {

  UINT8 UsefulDataCounter;
  UINT8 * pOutDataMsg;
  UINT8 TmpXor;
  UINT8 TmpXorHex;

  
  TmpXor = 0x00;
  UsefulDataCounter = pHitSJ300OutMsg->TxLen - 4;
  pOutDataMsg = (UINT8 *)(pHitSJ300OutMsg->Addr);
  
  while(UsefulDataCounter--){
    TmpXor ^= *pOutDataMsg;
    pOutDataMsg++;
  }  
  
  TmpXorHex = (UINT8)(TmpXor / 16);
  *pOutDataMsg = SYS_HexChar[TmpXorHex]; 
  pOutDataMsg++;
  
  TmpXor -= ( TmpXorHex * 16 );
  *pOutDataMsg = SYS_HexChar[TmpXor]; 
  
  pOutDataMsg++;
  *pOutDataMsg = HSJ_END_FLAG; 

}


#pragma CODE_SEG DEFAULT

