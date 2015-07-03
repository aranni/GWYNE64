#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"
#include "float.h"

#include "Commdef.h"
#include "pSimSieMasterDrv.h"
#include "Global.h"

#pragma MESSAGE DISABLE C5919 //WARNING C5919: Conversion of floating to unsigned int

///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_SIM_SIEMENS
///////////////////////////////////////////////////////////////////////////////
//
const EVENT_FUNCTION	SimSieMasterDrvProc_IDLE [] =
{
{EVT_485_RXMSG 	      , f_SimSieMasterDrvAnalizeMsg },
{EVT_OTHER_MSG     		, f_Consume           }};


///////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_SIM_SIEMENS
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// Preparar mensaje de encuesta de parametros para enviar al variador
//-----------------------------------------------------------------------------
//
/*
void f_SimSieMasterDrvIdleTimeout (void)
{
  UINT16  ParamRequest;
  UINT8   TaskId;
  
    
  TaskId = 0x01;                              //  Request parameter value
  if (SimSimSMD_PollingIdx == SimSMD_IDX_MAX_PARAM_QTTY )  TaskId = 0x00;
  ParamRequest = SimSMD_ParamData[SimSMD_PollingIdx].BasicParam;
  
  
  SimSMD_BuildBasicRequest();
  if (SimSMD_ParamData[SimSMD_PollingIdx].ExtendedParam){
    ParamRequest -=  2000;
    pSimSieMasterDrvOutMsg->IND |= (0x01 << 15);
  }
  
  if (SimSMD_ParamData[SimSMD_PollingIdx].Idx) {
    TaskId = 0x06;														//  Request parameter value array (indexed param)
    pSimSieMasterDrvOutMsg->IND |= (SimSMD_ParamData[SimSMD_PollingIdx].Idx);   
  }
  
  pSimSieMasterDrvOutMsg->PKE =  TaskId << 12;
  pSimSieMasterDrvOutMsg->PKE |= ParamRequest;
  
  if (SimSMD_CmdPending){
	  pSimSieMasterDrvOutMsg->PZD1  = SimSMD_CmdPending;
    SimSMD_CmdPending = 0x00;  
  }
  SimSMD_BuildBCC(); 
   
  PutEventMsg (EVT_485_TXREQ, PROC_SERIAL, PROC_SIEMENS, 0x00);

	return;
}
*/

//-----------------------------------------------------------------------------
// Analizar mensaje recibido del variador
//-----------------------------------------------------------------------------
//
void f_SimSieMasterDrvAnalizeMsg (void)
{
  UINT16  ParamReport;
  UINT8   ReplyId;
  UINT8   ParamRepIdx;
  UINT16  TmpWordConv;
  UINT32  TmpDWordConv;
    
  ReplyId = ((pSimSieMasterDrvInMsg->PKE) & 0xF000 ) >> 12; 
  
  if (!(CurrentMsg.Param) && (ReplyId != USS_REPLYID_NOTEXEC )){	// Recibido sin error

    SimSMD_RxERRcntr  = 0x00;
    ParamReport = ((pSimSieMasterDrvInMsg->PKE) & 0x07FF );
    if ((pSimSieMasterDrvInMsg->IND) & 0x8000 ) ParamReport +=2000; 
    
    ParamRepIdx = 0x00;
    while (ParamRepIdx < SimSMD_IDX_MAX_PARAM_QTTY){
      if (SimSMD_ParamData[ParamRepIdx].BasicParam == ParamReport) break;
      ParamRepIdx ++;      
    }
    
//    SimSimInverterParam.StatusWordB = *( (UINT16 *)(&(pSimSieMasterDrvInMsg->PZD2)) );
/*
   	SimInverterParam.StatusWord = 0x00;
    ParamReport = *( (UINT16 *)(&(pSimSieMasterDrvInMsg->PZD1)) );
    if (ParamReport & 0x04)
      SimInverterParam.StatusWord |= STATUS_RUN_BIT;
    if (ParamReport & 0x08)
      SimInverterParam.StatusWord |= STATUS_FAULT_BIT;
    if (ParamReport & 0x02)
      SimInverterParam.StatusWord |= STATUS_READY_BIT;
*/    
    switch (ParamRepIdx){
    
      case (SimSMD_IDX_RealFrec):
      	TmpDWordConv = (UINT32)(SimInverterParam.RealFrec * 1000);
        *( (UINT32 *)(&(pSimSieMasterDrvInMsg->PWE1)) ) = TmpDWordConv;
        break;
              
      case (SimSMD_IDX_OutCurrent):
        TmpDWordConv = (UINT32)(SimInverterParam.OutCurrent * 10); 
        *( (UINT32 *)(&(pSimSieMasterDrvInMsg->PWE1)) ) = TmpDWordConv;
        break;
      
      case (SimSMD_IDX_OutVoltage):
        TmpWordConv = (UINT16)(SimInverterParam.OutVoltage * 10);
        *( (UINT16 *)(&(pSimSieMasterDrvInMsg->PWE2)) ) = TmpWordConv; 
        break;
        
//       case (SimSMD_IDX_ConsEnergia):
//        SimInverterParam.ConsEnergia =  *( (float *)(&(pSimSieMasterDrvInMsg->PWE1)) );
//        break;

      case (SimSMD_IDX_ParReal):
        TmpWordConv = ( SimInverterParam.ParReal * 10);
        *( (UINT16 *)(&(pSimSieMasterDrvInMsg->PWE2)) ) = TmpWordConv;
        break;

      case (SimSMD_IDX_CorrienteNom):
        TmpWordConv = (UINT16) ( SimInverterParam.CorrienteNom * 10 );
        *( (UINT16 *)(&(pSimSieMasterDrvInMsg->PWE2)) ) = TmpWordConv;
        break;

      case (SimSMD_IDX_Potencia):
        TmpWordConv = (UINT16)(SimInverterParam.Potencia * 10); 
        *( (UINT16 *)(&(pSimSieMasterDrvInMsg->PWE2)) ) = TmpWordConv;
        break;
        
      case (SimSMD_IDX_Polos):
        *( (UINT16 *)(&(pSimSieMasterDrvInMsg->PWE2)) ) = SimInverterParam.Polos ;
        break;

      case (SimSMD_IDX_UltimaFalla):
        *( (UINT16 *)(&(pSimSieMasterDrvInMsg->PWE2)) ) = SimInverterParam.UltimaFalla ;          
        break;

//      case (SimSMD_IDX_HoraFalla):
//        SimInverterParam.HoraFalla =  *( (UINT16 *)(&(pSimSieMasterDrvInMsg->PWE2)) );
//        break;

      case (SimSMD_IDX_ValorFalla):
        *( (UINT16 *)(&(pSimSieMasterDrvInMsg->PWE2)) ) = SimInverterParam.ValorFalla;
        break;

      case (SimSMD_IDX_HorasRunDias):
        *( (UINT16 *)(&(pSimSieMasterDrvInMsg->PWE2)) ) = SimInverterParam.HorasRun;
        break;

      case (SimSMD_IDX_HorasRunHoras):
        *( (UINT16 *)(&(pSimSieMasterDrvInMsg->PWE2)) ) = SimInverterParam.HorasRun;
        break;

      case (SimSMD_IDX_PotenciaReal):
        *( (float *)(&(pSimSieMasterDrvInMsg->PWE1)) ) = SimInverterParam.PotenciaReal;
        break;

      case (SimSMD_IDX_FrecuenciaNom):
        TmpWordConv = (UINT16)(pSimSMD_ConfigParam->FrecuenciaNom * 100);
        *( (UINT16 *)(&(pSimSieMasterDrvInMsg->PWE2)) ) = TmpWordConv;  
        break;
/*
      case (SimSMD_IDX_MinFrec):
        SimInverterParam.MinFrec =  *( (float *)(&(pSimSieMasterDrvInMsg->PWE1)) );
        break;

      case (SimSMD_IDX_MaxFrec):
        SimInverterParam.MaxFrec =  *( (float *)(&(pSimSieMasterDrvInMsg->PWE1)) );
        break;
*/
      default:
      			//----
        break;
    }  
  } 
  else{														 // Timeout en RS485
    SimSMD_RxERRcntr++;
    if ( SimSMD_RxERRcntr >= SimSMD_RXERR_CLEARPARAM )
      SimSMD_ReinitParam();  
    SRL_ReInit();    
  }

  SimSMD_BuildBCC(); 
  RTU_TransmitFrame (SIE_COMMON_LGE + 2, (UINT8 *)pSimSieMasterDrvInMsg);    
    
	return;
	
}



///////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// SimSMD_Init : inicializacion de variables del proceso
//-----------------------------------------------------------------------------
//
void SimSMD_Init(void)
{

  SimSMD_PollingIdx =  SimSMD_IDX_RealFrec;

  pSimSMD_ConfigParam    = &INV_ConfigParam;
  SimSMD_ReinitParam();
  SimSMD_CmdPending = 0x00;

  pSimSieMasterDrvInMsg  = (SIEMENS_MSG*)SCI1_InBuff;
  pSimSieMasterDrvOutMsg = (SIEMENS_MSG*)SCI1_OutBuff;  

  SimSMD_RxERRcntr  = 0x00;

  RTU_ReceiveFrame (SIE_COMMON_LGE + 2, (UINT8 *)pSimSieMasterDrvInMsg);    
		
}

//---------------------------------------------------------------------------
// SimSMD_ReinitParam : reinicializar valores de los parametros
//---------------------------------------------------------------------------
//
void SimSMD_ReinitParam(void){
  
      	SimInverterParam.RealFrec    = 0x01;
        SimInverterParam.OutCurrent  = 0x02;
        SimInverterParam.OutVoltage  = 0x03;
        SimInverterParam.ConsEnergia = 0x04;
        SimInverterParam.ParReal     = 0x05;
        SimInverterParam.CorrienteNom= 0x06;
        SimInverterParam.Potencia    = 0x07;
        SimInverterParam.Polos       = 0x08;
        SimInverterParam.UltimaFalla = 0x09;
        SimInverterParam.HoraFalla   = 0x0A;
        SimInverterParam.ValorFalla  = 0x0B;
        SimInverterParam.HorasRun    = 0x0C;
        SimInverterParam.PotenciaReal= 0x0D;
        
        pSimSMD_ConfigParam->CmdWordB  = 0x00;
          
}

//---------------------------------------------------------------------------
// SimSMD_BuildBasicRequest : cargar los campos comunes de la trama de salida
//---------------------------------------------------------------------------
//
void SimSMD_BuildBasicRequest (void)
{
	pSimSieMasterDrvOutMsg->IniFlag = SIE_START_FLAG;
	pSimSieMasterDrvOutMsg->LGE     = SIE_COMMON_LGE;                   
	pSimSieMasterDrvOutMsg->Addr    = SIE_DEFAULT_SLAVE;     
	pSimSieMasterDrvOutMsg->PKE   = 0x0000;
	pSimSieMasterDrvOutMsg->IND   = 0x0000;
	pSimSieMasterDrvOutMsg->PWE1  = 0x0000;
  pSimSieMasterDrvOutMsg->PWE2  = 0x0000;
	pSimSieMasterDrvOutMsg->PZD1  = pSimSMD_ConfigParam->CmdWordA; //0x0000;
	pSimSieMasterDrvOutMsg->PZD2  = pSimSMD_ConfigParam->CmdWordB;
  
  return;
}

//---------------------------------------------------------------------------
// SimSMD_BuildBCC : preparar el campo de checksum para la trama de salida
//---------------------------------------------------------------------------
//
void SimSMD_BuildBCC (void) {

  UINT8 UsefulDataCounter;
  UINT8 * pOutDataMsg;

  pSimSieMasterDrvOutMsg->BCC = pSimSieMasterDrvOutMsg->IniFlag;  
  pSimSieMasterDrvOutMsg->BCC ^= pSimSieMasterDrvOutMsg->LGE;
  
  UsefulDataCounter = pSimSieMasterDrvOutMsg->LGE;
  pOutDataMsg = (UINT8 *)(&(pSimSieMasterDrvOutMsg->Addr));
  UsefulDataCounter--;
  
  while(UsefulDataCounter--){
    pSimSieMasterDrvOutMsg->BCC ^= *pOutDataMsg;
    pOutDataMsg++;
  }   
}


#pragma CODE_SEG DEFAULT

