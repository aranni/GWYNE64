#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"
#include "float.h"

#include "Commdef.h"
#include "pSieMasterDrv.h"
#include "Global.h"

#pragma MESSAGE DISABLE C5919 //WARNING C5919: Conversion of floating to unsigned int

///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_SIEMENS
///////////////////////////////////////////////////////////////////////////////
//
const EVENT_FUNCTION	SieMasterDrvProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_SieMasterDrvIdleTimeout},
{EVT_SIE_RXFRAME 	    , f_SieMasterDrvAnalizeMsg },
{EVT_VAR_CMDSTART	    , f_SieMasterDrvCmdStart   },
{EVT_VAR_CMDSTOP	    , f_SieMasterDrvCmdStop    },
{EVT_VAR_CMDSETFREC   , f_SieMasterDrvCmdSetFrec },
{EVT_VAR_CMDRESETFAULT, f_SieMasterDrvCmdResFault},
{EVT_OTHER_MSG     		, f_Consume           }};


///////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_SIEMENS
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// Preparar mensaje de encuesta de parametros para enviar al variador
//-----------------------------------------------------------------------------
//
void f_SieMasterDrvIdleTimeout (void)
{
  UINT16  ParamRequest;
  UINT8   TaskId;
  
    
  TaskId = 0x01;                              //  Request parameter value
  if (SMD_PollingIdx == SMD_IDX_MAX_PARAM_QTTY )  TaskId = 0x00;
  ParamRequest = SMD_ParamData[SMD_PollingIdx].BasicParam;
  
  
  SMD_BuildBasicRequest();
  if (SMD_ParamData[SMD_PollingIdx].ExtendedParam){
    ParamRequest -=  2000;
    pSieMasterDrvOutMsg->IND |= (0x01 << 15);
  }
  
  if (SMD_ParamData[SMD_PollingIdx].Idx) {
    TaskId = 0x06;														//  Request parameter value array (indexed param)
    pSieMasterDrvOutMsg->IND |= (SMD_ParamData[SMD_PollingIdx].Idx);   
  }
  
  pSieMasterDrvOutMsg->PKE =  TaskId << 12;
  pSieMasterDrvOutMsg->PKE |= ParamRequest;
  
  if (SMD_CmdPending){
	  pSieMasterDrvOutMsg->PZD1  = SMD_CmdPending;
    SMD_CmdPending = 0x00;  
  }
  SMD_BuildBCC(); 
   
  PutEventMsg (EVT_485_TXREQ, PROC_SERIAL, PROC_SIEMENS, 0x00);

	return;
}

//-----------------------------------------------------------------------------
// Analizar mensaje recibido del variador
//-----------------------------------------------------------------------------
//
void f_SieMasterDrvAnalizeMsg (void)
{
  UINT16  ParamReport;
  UINT8   ReplyId;
  UINT8   ParamRepIdx;
  UINT16  TmpWordConv;
  UINT32  TmpDWordConv;
    
  ReplyId = ((pSieMasterDrvInMsg->PKE) & 0xF000 ) >> 12; 
  
  if (!(CurrentMsg.Param) && (ReplyId != USS_REPLYID_NOTEXEC )){	// Recibido sin error

    SMD_RxERRcntr  = 0x00;
    ParamReport = ((pSieMasterDrvInMsg->PKE) & 0x07FF );
    if ((pSieMasterDrvInMsg->IND) & 0x8000 ) ParamReport +=2000; 
    
    ParamRepIdx = 0x00;
    while (ParamRepIdx < SMD_IDX_MAX_PARAM_QTTY){
      if (SMD_ParamData[ParamRepIdx].BasicParam == ParamReport) break;
      ParamRepIdx ++;      
    }
    
//    InverterParam.StatusWordB = *( (UINT16 *)(&(pSieMasterDrvInMsg->PZD2)) );
   	InverterParam.StatusWord = 0x00;
    ParamReport = *( (UINT16 *)(&(pSieMasterDrvInMsg->PZD1)) );
    if (ParamReport & 0x04)
      InverterParam.StatusWord |= STATUS_RUN_BIT;
    if (ParamReport & 0x08)
      InverterParam.StatusWord |= STATUS_FAULT_BIT;
    if (ParamReport & 0x02)
      InverterParam.StatusWord |= STATUS_READY_BIT;
    
    switch (ParamRepIdx){
    
      case (SMD_IDX_RealFrec):
        TmpDWordConv = *( (UINT32 *)(&(pSieMasterDrvInMsg->PWE1)) );
      	InverterParam.RealFrec = (float) TmpDWordConv; 
        InverterParam.RealFrec /= 1000;
        break;
              
      case (SMD_IDX_OutCurrent):
        TmpDWordConv = *( (UINT32 *)(&(pSieMasterDrvInMsg->PWE1)) );
        InverterParam.OutCurrent = (float) TmpDWordConv; 
        InverterParam.OutCurrent /= 10; 
        break;
      
      case (SMD_IDX_OutVoltage):
        TmpWordConv = *( (UINT16 *)(&(pSieMasterDrvInMsg->PWE2)) ); 
        InverterParam.OutVoltage = (float) TmpWordConv;
        InverterParam.OutVoltage /= 10;  
        break;
        
//       case (SMD_IDX_ConsEnergia):
//        InverterParam.ConsEnergia =  *( (float *)(&(pSieMasterDrvInMsg->PWE1)) );
//        break;

      case (SMD_IDX_ParReal):
        TmpWordConv = *( (UINT16 *)(&(pSieMasterDrvInMsg->PWE2)) );
        InverterParam.ParReal = (float) TmpWordConv;
        InverterParam.ParReal /= 10;
        break;

      case (SMD_IDX_CorrienteNom):
        TmpWordConv =  *( (UINT16 *)(&(pSieMasterDrvInMsg->PWE2)) );
        InverterParam.CorrienteNom = (float) TmpWordConv;
        InverterParam.CorrienteNom /= 10;
        break;

      case (SMD_IDX_Potencia):
        TmpWordConv = *( (UINT16 *)(&(pSieMasterDrvInMsg->PWE2)) );
        InverterParam.Potencia = (float) TmpWordConv; 
        InverterParam.Potencia /= 10;
        break;
        
      case (SMD_IDX_Polos):
        InverterParam.Polos =  *( (UINT16 *)(&(pSieMasterDrvInMsg->PWE2)) );
        break;

      case (SMD_IDX_UltimaFalla):
        InverterParam.UltimaFalla =  *( (UINT16 *)(&(pSieMasterDrvInMsg->PWE2)) );          
        break;

//      case (SMD_IDX_HoraFalla):
//        InverterParam.HoraFalla =  *( (UINT16 *)(&(pSieMasterDrvInMsg->PWE2)) );
//        break;

      case (SMD_IDX_ValorFalla):
        InverterParam.ValorFalla =  *( (UINT16 *)(&(pSieMasterDrvInMsg->PWE2)) );
        break;

      case (SMD_IDX_HorasRunDias):
        InverterParam.HorasRun =  *( (UINT16 *)(&(pSieMasterDrvInMsg->PWE2)) );
        InverterParam.HorasRun *= 24; 
        break;

      case (SMD_IDX_HorasRunHoras):
        InverterParam.HorasRun +=  *( (UINT16 *)(&(pSieMasterDrvInMsg->PWE2)) );
        break;

      case (SMD_IDX_PotenciaReal):
        InverterParam.PotenciaReal =  *( (float *)(&(pSieMasterDrvInMsg->PWE1)) );
        break;

      case (SMD_IDX_FrecuenciaNom):
        TmpWordConv = *( (UINT16 *)(&(pSieMasterDrvInMsg->PWE2)) ); 
        pSMD_ConfigParam->FrecuenciaNom = (float) TmpWordConv;
        pSMD_ConfigParam->FrecuenciaNom /= 100; 
        break;
/*
      case (SMD_IDX_MinFrec):
        InverterParam.MinFrec =  *( (float *)(&(pSieMasterDrvInMsg->PWE1)) );
        break;

      case (SMD_IDX_MaxFrec):
        InverterParam.MaxFrec =  *( (float *)(&(pSieMasterDrvInMsg->PWE1)) );
        break;
*/
      default:
      			//----
        break;
    }  
  } 
  else{														 // Timeout en RS485
    SMD_RxERRcntr++;
    if ( SMD_RxERRcntr >= SMD_RXERR_CLEARPARAM )
      SMD_ReinitParam();  
    SRL_ReInit();    
  }

  SMD_PollingIdx++;
  if (SMD_PollingIdx > SMD_IDX_MAX_PARAM_QTTY){
    SMD_PollingIdx =  SMD_IDX_RealFrec;
    PeriodicTimer = SYS_ConfigParam.ScanRate;
//	DummyVar =  TMR_SetTimer ( SYS_ConfigParam.ScanRate , PROC_SIEMENS, 0x00, FALSE);
  } 
  else{
    PeriodicTimer = SMD_TMR_TXREQ_DELAY;
//	DummyVar =  TMR_SetTimer ( TMR_TXREQ_DELAY , PROC_SIEMENS, 0x00, FALSE);
  }
    
	return;
	
}

//-----------------------------------------------------------------------------
// Ejecutar comando recibido: START
//-----------------------------------------------------------------------------
//
void f_SieMasterDrvCmdStart (void)
{
/*  
  float TmpFloat;
  if ((SYS_ConfigParam.ConsignaFrec) && (pSMD_ConfigParam->FrecuenciaNom)){
    TmpFloat =  SYS_ConfigParam.ConsignaFrec/pSMD_ConfigParam->FrecuenciaNom;
    TmpFloat *= 16384;
    pSMD_ConfigParam->CmdWordB = (UINT16)TmpFloat;
    SMD_CmdPending = (pSMD_ConfigParam->CmdWordA | 0x0001);
  }
*/  
  pSMD_ConfigParam->CmdWordA |= 0x0001;
  return;

}

//-----------------------------------------------------------------------------
// Ejecutar comando recibido: STOP
//-----------------------------------------------------------------------------
//
void f_SieMasterDrvCmdStop (void)
{

//  SMD_CmdPending = ( (pSMD_ConfigParam->CmdWordA) & (~(0x0001)) );
  pSMD_ConfigParam->CmdWordA &= (~(0x0001) );
  return;

}

//-----------------------------------------------------------------------------
// Ejecutar comando recibido: SET_FREC
//-----------------------------------------------------------------------------
//
void f_SieMasterDrvCmdSetFrec (void)
{
  float TmpFloat;
  
  if ((SYS_ConfigParam.ConsignaFrec) && (pSMD_ConfigParam->FrecuenciaNom)){
    TmpFloat =  SYS_ConfigParam.ConsignaFrec/pSMD_ConfigParam->FrecuenciaNom;
    TmpFloat *= 16384;
    pSMD_ConfigParam->CmdWordB = (UINT16)TmpFloat;
  }

  if (InverterParam.StatusWord & STATUS_RUN_BIT)        // Si esta funcionando..
    SMD_CmdPending = (pSMD_ConfigParam->CmdWordA | 0x0001);
  else 
    SMD_CmdPending = ( (pSMD_ConfigParam->CmdWordA) & (~(0x0001)) );
    
  return;

}


//-----------------------------------------------------------------------------
// Ejecutar comando recibido: RESET_FAULT
//-----------------------------------------------------------------------------
//
void f_SieMasterDrvCmdResFault (void)
{

  SMD_CmdPending = (pSMD_ConfigParam->CmdWordA | 0x0080);
  return;  
  
}


///////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// SMD_Init : inicializacion de variables del proceso
//-----------------------------------------------------------------------------
//
void SMD_Init(void)
{

  SMD_PollingIdx =  SMD_IDX_RealFrec;

  pSMD_ConfigParam    = &INV_ConfigParam;
  SMD_ReinitParam();
  SMD_CmdPending = 0x00;

  pSieMasterDrvInMsg  = (SIEMENS_MSG*)SCI1_InBuff;
  pSieMasterDrvOutMsg = (SIEMENS_MSG*)SCI1_OutBuff;  

  SMD_RxERRcntr  = 0x00;
//	DummyVar =  TMR_SetTimer ( 1000 , PROC_SIEMENS, 0x00, FALSE);
		
}

//---------------------------------------------------------------------------
// SMD_ReinitParam : reinicializar valores de los parametros
//---------------------------------------------------------------------------
//
void SMD_ReinitParam(void){
  
      	InverterParam.RealFrec    = 0x00;
        InverterParam.OutCurrent  = 0x00;
        InverterParam.OutVoltage  = 0x00;
        InverterParam.ConsEnergia = 0x00;
        InverterParam.ParReal     = 0x00;
        InverterParam.CorrienteNom= 0x00;
        InverterParam.Potencia    = 0x00;
        InverterParam.Polos       = 0x00;
        InverterParam.UltimaFalla = 0x00;
        InverterParam.HoraFalla   = 0x00;
        InverterParam.ValorFalla  = 0x00;
        InverterParam.HorasRun    = 0x00;
        InverterParam.PotenciaReal= 0x00;
        
        pSMD_ConfigParam->CmdWordB  = 0x00;
          
}

//---------------------------------------------------------------------------
// SMD_BuildBasicRequest : cargar los campos comunes de la trama de salida
//---------------------------------------------------------------------------
//
void SMD_BuildBasicRequest (void)
{
	pSieMasterDrvOutMsg->IniFlag = SIE_START_FLAG;
	pSieMasterDrvOutMsg->LGE     = SIE_COMMON_LGE;                   
	pSieMasterDrvOutMsg->Addr    = SIE_DEFAULT_SLAVE;     
	pSieMasterDrvOutMsg->PKE   = 0x0000;
	pSieMasterDrvOutMsg->IND   = 0x0000;
	pSieMasterDrvOutMsg->PWE1  = 0x0000;
  pSieMasterDrvOutMsg->PWE2  = 0x0000;
	pSieMasterDrvOutMsg->PZD1  = pSMD_ConfigParam->CmdWordA; //0x0000;
	pSieMasterDrvOutMsg->PZD2  = pSMD_ConfigParam->CmdWordB;
  
  return;
}

//---------------------------------------------------------------------------
// SMD_BuildBCC : preparar el campo de checksum para la trama de salida
//---------------------------------------------------------------------------
//
void SMD_BuildBCC (void) {

  UINT8 UsefulDataCounter;
  UINT8 * pOutDataMsg;

  pSieMasterDrvOutMsg->BCC = pSieMasterDrvOutMsg->IniFlag;  
  pSieMasterDrvOutMsg->BCC ^= pSieMasterDrvOutMsg->LGE;
  
  UsefulDataCounter = pSieMasterDrvOutMsg->LGE;
  pOutDataMsg = (UINT8 *)(&(pSieMasterDrvOutMsg->Addr));
  UsefulDataCounter--;
  
  while(UsefulDataCounter--){
    pSieMasterDrvOutMsg->BCC ^= *pOutDataMsg;
    pOutDataMsg++;
  }   
}


#pragma CODE_SEG DEFAULT

//	DummyVar = sprintf(Message,"%#5.2f",NominalCurrent);
