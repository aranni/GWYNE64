#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"
#include "float.h"

#include "Commdef.h"
#include "pSieMicroMst.h"
#include "Global.h"

#pragma MESSAGE DISABLE C5919 //WARNING C5919: Conversion of floating to unsigned int

///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_SIEMENS
///////////////////////////////////////////////////////////////////////////////
//
const EVENT_FUNCTION	SiemensProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_SiemensIdleTimeout},
{EVT_SIE_RXFRAME 	    , f_SiemensAnalizeMsg },
{EVT_VAR_CMDSTART	    , f_SiemensCmdStart   },
{EVT_VAR_CMDSTOP	    , f_SiemensCmdStop    },
{EVT_VAR_CMDSETFREC   , f_SiemensCmdSetFrec },
{EVT_VAR_CMDRESETFAULT, f_SiemensCmdResFault},
{EVT_OTHER_MSG     		, f_Consume           }};


///////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_SIEMENS
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// Preparar mensaje de encuesta de parametros para enviar al variador
//-----------------------------------------------------------------------------
//
void f_SiemensIdleTimeout (void)
{
  UINT16  ParamRequest;
  UINT8   TaskId;
  
    
  TaskId = 0x01;  
  if (SIE_PollingIdx == IDX_MAX_PARAM_QTTY )  TaskId = 0x00;
  ParamRequest = ParamData[SIE_PollingIdx].BasicParam;

  SIE_BuildBasicRequest();
  if (ParamData[SIE_PollingIdx].ExtendedParam){
    ParamRequest -=  2000;
    pSiemensOutMsg->IND |= (0x01 << 15);
  }
  pSiemensOutMsg->PKE =  TaskId << 12;
  pSiemensOutMsg->PKE |= ParamRequest;
  
  if (SIE_CmdPending){
	  pSiemensOutMsg->PZD1  = SIE_CmdPending;
    SIE_CmdPending = 0x00;  
  }
  SIE_BuildBCC(); 
   
  PutEventMsg (EVT_485_TXREQ, PROC_SERIAL, PROC_SIEMENS, 0x00);

	return;
}

//-----------------------------------------------------------------------------
// Analizar mensaje recibido del variador
//-----------------------------------------------------------------------------
//
void f_SiemensAnalizeMsg (void)
{
  UINT16  ParamReport;
  UINT8   ReplyId;
  UINT8   ParamRepIdx;
    
  if (!(CurrentMsg.Param)){				 // Recibido sin error

    ParamReport = ((pSiemensInMsg->PKE) & 0x07FF );
    if ((pSiemensInMsg->IND) & 0x8000 ) ParamReport +=2000; 
    
    ReplyId = ((pSiemensInMsg->PKE) & 0xF000 ) >> 12; 

    ParamRepIdx = 0x00;
    while (ParamRepIdx < IDX_MAX_PARAM_QTTY){
      if (ParamData[ParamRepIdx].BasicParam == ParamReport) break;
      ParamRepIdx ++;      
    }
    
//    InverterParam.StatusWordB = *( (UINT16 *)(&(pSiemensInMsg->PZD2)) );
   	InverterParam.StatusWord = 0x00;
    ParamReport = *( (UINT16 *)(&(pSiemensInMsg->PZD1)) );
    if (ParamReport & 0x04)
      InverterParam.StatusWord |= STATUS_RUN_BIT;
    if (ParamReport & 0x08)
      InverterParam.StatusWord |= STATUS_FAULT_BIT;
    if (ParamReport & 0x02)
      InverterParam.StatusWord |= STATUS_READY_BIT;
    
    switch (ParamRepIdx){
    
      case (IDX_RealFrec):
      	InverterParam.RealFrec = *( (float *)(&(pSiemensInMsg->PWE1)) );
        break;
              
      case (IDX_OutCurrent):
        InverterParam.OutCurrent =  *( (float *)(&(pSiemensInMsg->PWE1)) );
        break;
      
      case (IDX_OutVoltage):
        InverterParam.OutVoltage =  *( (float *)(&(pSiemensInMsg->PWE1)) );
        break;
        
       case (IDX_ConsEnergia):
        InverterParam.ConsEnergia =  *( (float *)(&(pSiemensInMsg->PWE1)) );
        break;

      case (IDX_ParReal):
        InverterParam.ParReal =  *( (float *)(&(pSiemensInMsg->PWE1)) );
        break;

      case (IDX_CorrienteNom):
        InverterParam.CorrienteNom =  *( (float *)(&(pSiemensInMsg->PWE1)) );
        break;

      case (IDX_Potencia):
        InverterParam.Potencia =  *( (float *)(&(pSiemensInMsg->PWE1)) );
        break;
        
      case (IDX_Polos):
        InverterParam.Polos =  *( (UINT16 *)(&(pSiemensInMsg->PWE2)) );
        break;

      case (IDX_UltimaFalla):
        InverterParam.UltimaFalla =  *( (UINT16 *)(&(pSiemensInMsg->PWE2)) );          
        break;

      case (IDX_HoraFalla):
        InverterParam.HoraFalla =  *( (UINT16 *)(&(pSiemensInMsg->PWE2)) );
        break;

      case (IDX_ValorFalla):
        InverterParam.ValorFalla =  *( (UINT16 *)(&(pSiemensInMsg->PWE2)) );
        break;

      case (IDX_HorasRun):
        InverterParam.HorasRun =  *( (UINT16 *)(&(pSiemensInMsg->PWE2)) );
        break;

      case (IDX_PotenciaReal):
        InverterParam.PotenciaReal =  *( (float *)(&(pSiemensInMsg->PWE1)) );
        break;

      case (IDX_FrecuenciaNom):
        pSIE_ConfigParam->FrecuenciaNom =  *( (float *)(&(pSiemensInMsg->PWE1)) );
        break;

      case (IDX_MinFrec):
        InverterParam.MinFrec =  *( (float *)(&(pSiemensInMsg->PWE1)) );
        break;

      case (IDX_MaxFrec):
        InverterParam.MaxFrec =  *( (float *)(&(pSiemensInMsg->PWE1)) );
        break;

      default:
      			//----
        break;
    }  
  } 
  else{														 // Timeout en RS485
    SIE_ReinitParam();  
    SRL_ReInit();    
  }

  SIE_PollingIdx++;
  if (SIE_PollingIdx > IDX_MAX_PARAM_QTTY){
    SIE_PollingIdx =  IDX_RealFrec;
    PeriodicTimer = SYS_ConfigParam.ScanRate;
//	DummyVar =  TMR_SetTimer ( SYS_ConfigParam.ScanRate , PROC_SIEMENS, 0x00, FALSE);
  } 
  else{
    PeriodicTimer = TMR_TXREQ_DELAY;
//	DummyVar =  TMR_SetTimer ( TMR_TXREQ_DELAY , PROC_SIEMENS, 0x00, FALSE);
  }
    
	return;
	
}

//-----------------------------------------------------------------------------
// Ejecutar comando recibido: START
//-----------------------------------------------------------------------------
//
void f_SiemensCmdStart (void)
{
  float TmpFloat;
  
  if ((SYS_ConfigParam.ConsignaFrec) && (pSIE_ConfigParam->FrecuenciaNom)){
    TmpFloat =  SYS_ConfigParam.ConsignaFrec/pSIE_ConfigParam->FrecuenciaNom;
    TmpFloat *= 16384;
    pSIE_ConfigParam->CmdWordB = (UINT16)TmpFloat;
    SIE_CmdPending = (pSIE_ConfigParam->CmdWordA | 0x0001);
  }
  return;

}

//-----------------------------------------------------------------------------
// Ejecutar comando recibido: STOP
//-----------------------------------------------------------------------------
//
void f_SiemensCmdStop (void)
{

  SIE_CmdPending = ( (pSIE_ConfigParam->CmdWordA) & (~(0x0001)) );
  return;

}

//-----------------------------------------------------------------------------
// Ejecutar comando recibido: SET_FREC
//-----------------------------------------------------------------------------
//
void f_SiemensCmdSetFrec (void)
{
  float TmpFloat;
  
  if ((SYS_ConfigParam.ConsignaFrec) && (pSIE_ConfigParam->FrecuenciaNom)){
    TmpFloat =  SYS_ConfigParam.ConsignaFrec/pSIE_ConfigParam->FrecuenciaNom;
    TmpFloat *= 16384;
    pSIE_ConfigParam->CmdWordB = (UINT16)TmpFloat;
  }

  if (InverterParam.StatusWord & STATUS_RUN_BIT)        // Si esta funcionando..
    SIE_CmdPending = (pSIE_ConfigParam->CmdWordA | 0x0001);
  else 
    SIE_CmdPending = ( (pSIE_ConfigParam->CmdWordA) & (~(0x0001)) );
    
  return;

}


//-----------------------------------------------------------------------------
// Ejecutar comando recibido: RESET_FAULT
//-----------------------------------------------------------------------------
//
void f_SiemensCmdResFault (void)
{

  SIE_CmdPending = (pSIE_ConfigParam->CmdWordA | 0x0080);
  return;  
  
}


///////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// SIE_Init : inicializacion de variables del proceso
//-----------------------------------------------------------------------------
//
void SIE_Init(void)
{

  SIE_PollingIdx =  IDX_RealFrec;

  pSIE_ConfigParam  = &INV_ConfigParam;
  SIE_ReinitParam();
  SIE_CmdPending = 0x00;

  pSiemensInMsg     = (SIEMENS_MSG*)SCI1_InBuff;
  pSiemensOutMsg    = (SIEMENS_MSG*)SCI1_OutBuff;  

  
//	DummyVar =  TMR_SetTimer ( 1000 , PROC_SIEMENS, 0x00, FALSE);
		
}

//---------------------------------------------------------------------------
// SIE_ReinitParam : reinicializar valores de los parametros
//---------------------------------------------------------------------------
//
void SIE_ReinitParam(void){
  
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
        
        pSIE_ConfigParam->CmdWordB  = 0x00;
          
}

//---------------------------------------------------------------------------
// SIE_BuildBasicRequest : cargar los campos comunes de la trama de salida
//---------------------------------------------------------------------------
//
void SIE_BuildBasicRequest (void)
{
	pSiemensOutMsg->IniFlag = SIE_START_FLAG;
	pSiemensOutMsg->LGE     = SIE_COMMON_LGE;                   
	pSiemensOutMsg->Addr    = SIE_DEFAULT_SLAVE;     
	pSiemensOutMsg->PKE   = 0x0000;
	pSiemensOutMsg->IND   = 0x0000;
	pSiemensOutMsg->PWE1  = 0x0000;
  pSiemensOutMsg->PWE2  = 0x0000;
	pSiemensOutMsg->PZD1  = 0x0000;
	pSiemensOutMsg->PZD2  = pSIE_ConfigParam->CmdWordB;
  
  return;
}

//---------------------------------------------------------------------------
// SIE_BuildBCC : preparar el campo de checksum para la trama de salida
//---------------------------------------------------------------------------
//
void SIE_BuildBCC (void) {

  UINT8 UsefulDataCounter;
  UINT8 * pOutDataMsg;

  pSiemensOutMsg->BCC = pSiemensOutMsg->IniFlag;  
  pSiemensOutMsg->BCC ^= pSiemensOutMsg->LGE;
  
  UsefulDataCounter = pSiemensOutMsg->LGE;
  pOutDataMsg = (UINT8 *)(&(pSiemensOutMsg->Addr));
  UsefulDataCounter--;
  
  while(UsefulDataCounter--){
    pSiemensOutMsg->BCC ^= *pOutDataMsg;
    pOutDataMsg++;
  }   
}


#pragma CODE_SEG DEFAULT

