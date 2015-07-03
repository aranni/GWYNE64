#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"
#include "float.h"

#include "Commdef.h"
#include "pDnp.h"
#include "Global.h"


#pragma MESSAGE DISABLE C5919 //WARNING C5919: Conversion of floating to unsigned int

///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_DNP
///////////////////////////////////////////////////////////////////////////////
//
const EVENT_FUNCTION	DnpProc_INIT [] =
{
{EVT_TMR_TIMEOUT 	    , f_DnpInitTimeout        },
{EVT_DVR_EXCEPTION    , f_DnpInitDrvException   },				 
{EVT_OTHER_MSG     		, f_Consume               }};


const EVENT_FUNCTION	DnpProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_DnpIdleTimeout        },
{EVT_DNP_RXFRAME 	    , f_DnpAnalizeMsg         },				 
{EVT_DNP_POLLREQ      , f_DnpPollingMsg         },				 
{EVT_DNP_SETOUTON     , f_DnpSetOutputOn        },				 
{EVT_DNP_SETOUTOFF    , f_DnpSetOutputOff       },				 
{EVT_OTHER_MSG     		, f_Consume               }};

													
			
///////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_DNP estado INIT
///////////////////////////////////////////////////////////////////////////////
//
void f_DnpInitTimeout (void)
{
  
  DNP_CmdPendingFlag = 0x00;  
  PeriodicTimer = 1000;

	return;
}

void f_DnpInitDrvException (void)
{

  DNP_BinaryInputs[11] &= ~(SYS_FLAG_SCIADOWN); 
  if ( CurrentMsg.Param == DRV_CHANNEL_UP ){
    PutEventMsg (EVT_DNP_POLLREQ, PROC_DNP, PROC_DNP, 0x00);
    FirstEventInCurrentState = (EVENT_FUNCTION*)DnpProc_IDLE;	
  }
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_DNP
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// Encola periodicamente un mensaje de consulta al switch
//
void f_DnpIdleTimeout (void)
{
  UINT8 bTmpCntr;  

  
  // Si se acumulan muchas transmisiones sin respuesta reiniciar la comunicacion..
  if ( ++DNP_TxNoAnswerAcc >= MAX_DNP_TXATTEMP){
      // Si hay perdida de comunicacion Borrar todas las variables..
      for ( bTmpCntr = 0 ; bTmpCntr <= 106 ; bTmpCntr++ ){
	      DNP_AnalogInputs[bTmpCntr] =  0;
      }
      for ( bTmpCntr = 0 ; bTmpCntr < 8 ; bTmpCntr++ ){
	      DNP_BinaryInputs[bTmpCntr] =  0x00;
      }
      for ( bTmpCntr = 0 ; bTmpCntr < 16 ; bTmpCntr++ ){
	      DNP_BinaryOutput[bTmpCntr] =  0x00;
      }
      
      DNP_BinaryInputs[11] |= SYS_FLAG_SCIADOWN; 
  
      // Intentar establecer nuevamente la comunicacion
      DNPL_ReInit();
      DNP_ReInit();
      FirstEventInCurrentState = (EVENT_FUNCTION*)DnpProc_INIT;	
  } 
  else{
      // ABR: volver al polling
      PutEventMsg (EVT_DNP_POLLREQ, PROC_DNP, PROC_DNP, 0x00);
  }
	return;
}

//-----------------------------------------------------------------------------
// Busca permanentemente mensajes encolados para enviar..
//
void f_DnpPollingMsg (void)
{
  void*               vpAuxBuffer;
  LPAPU_MSG_REQUEST   pAppDataBuffer;

  UINT8   bTmpTxToken;
  UINT8   bTmpPolIdx;
  
//-- Pedir un buffer de transmision.
  (void)MemGetBuffer ( (void**)(&vpAuxBuffer), &bTmpTxToken);  // Pedir buffer para próxima TX

  pAppDataBuffer =  (LPAPU_MSG_REQUEST)(&(((LPRTS_MSG_FORMAT)vpAuxBuffer)->bDataBlock1[0]));

//-- Armar el mensaje de request.
  pAppDataBuffer->TransportHeader		 =  (TPDU_SeqNumber & TPDU_BITMASK_TH_SEQ);
  pAppDataBuffer->TransportHeader		 |= (TPDU_BITMASK_TH_FIN | TPDU_BITMASK_TH_FIR);
  
  pAppDataBuffer->ApplicationControl =  (APDU_SeqNumber & APDU_BITMASK_AC_SEQ);
  pAppDataBuffer->ApplicationControl |= (APDU_BITMASK_AC_FIN | APDU_BITMASK_AC_FIR);


  if (DNP_CmdPendingFlag & DNP_CMDMASK_INS){ //

//    pAppDataBuffer->FunctionCode       = APDU_FC_REQ_DIROPERATE;  // FC;
    pAppDataBuffer->FunctionCode       = (DNP_CmdPendingFlag & DNP_CMDFLAG_I0)?APDU_FC_REQ_SELECT:APDU_FC_REQ_OPERATE;  // FC;
    pAppDataBuffer->ObjectGroup        = 12;      // OG = 12 Binary Output;
    pAppDataBuffer->ObjectVariation    = 1;       // OV =  1 Control Relay Output Block;
    pAppDataBuffer->Qualifier          = 0x17;    // QL;
    pAppDataBuffer->bData[0]           = 1;       // Range, cantidad de objetos = 1  ;
    pAppDataBuffer->bData[1]           = DNP_CmdPendingIdx;     // Indice del objeto;

    // ControlCode : 3=LatchOn, 4=LatchOff  
    ((LPAPU_FOMRAT_CRTLRELAY)(&(pAppDataBuffer->bData[2])))->ControlCode = (DNP_CmdPendingFlag & DNP_CMDFLAG_ON)?3:4;
		((LPAPU_FOMRAT_CRTLRELAY)(&(pAppDataBuffer->bData[2])))->Count    = 1;
		((LPAPU_FOMRAT_CRTLRELAY)(&(pAppDataBuffer->bData[2])))->OnTime   = 0;
		((LPAPU_FOMRAT_CRTLRELAY)(&(pAppDataBuffer->bData[2])))->OffTime  = 0;
		((LPAPU_FOMRAT_CRTLRELAY)(&(pAppDataBuffer->bData[2])))->Status   = 0;
 		((LPAPU_FOMRAT_CRTLRELAY)(&(pAppDataBuffer->bData[2])))->CRC_Place = 0; // ABR !!
   									 
    //-- Iniciar timer de supervision.
	  APDU_WaitResponseTmrId = TMR_SetTimer ( 5000 , PROC_DNP, 0x00, FALSE);  //Por ahora..
    //-- Enviar mensaje..
    DNPL_SendAppMsg ( 19, bTmpTxToken);
    
//    DNP_CmdPendingFlag = 0x00;
    if (DNP_CmdPendingFlag & DNP_CMDFLAG_I0){
        DNP_CmdPendingFlag &= ~(DNP_CMDFLAG_I0);       // Borro la instacia 0     
        DNP_CmdPendingFlag |=  (DNP_CMDFLAG_I1);
    }
    else{
        DNP_CmdPendingFlag = 0x00;
    }
  } 
  else{
    pAppDataBuffer->FunctionCode       = APDU_FC_REQ_READ;  // FC;

    // Usar el SeqNumber de LinkLayer como indice para la tabla de polling.
    bTmpPolIdx = (UINT8)( TPDU_SeqNumber % DNP_POLIDX_MAX_QTTY);
 
    pAppDataBuffer->ObjectGroup        = DNP_PolingTable[DNP_PolingTableIdx].OG;   // OG;
    pAppDataBuffer->ObjectVariation    = DNP_PolingTable[DNP_PolingTableIdx].OV;   // OV;
    pAppDataBuffer->Qualifier          = DNP_PolingTable[DNP_PolingTableIdx].QL;   // QL;
    pAppDataBuffer->bData[0]           = DNP_PolingTable[DNP_PolingTableIdx].I1;   // I1;
    pAppDataBuffer->bData[1]           = DNP_PolingTable[DNP_PolingTableIdx].I2;   // I2;

//ABR: DEBUG. version vieja
//    if (DNP_PolingTableIdx==DNP_POLIDX_AAGINP)
//      pAppDataBuffer->bData[1] = (UINT8)SYS_ConfigParam.RelacionTx;   // I2;
//    if (DNP_PolingTableIdx==DNP_POLIDX_BINOUT){      
//      pAppDataBuffer->bData[0] = (UINT8)SYS_ConfigParam.RelacionTx;   // I1;
//      pAppDataBuffer->bData[1] = (UINT8)SYS_ConfigParam.CoefResbal;   // I2;
//    }
//ABR: DEBUG. version vieja

    //-- Iniciar timer de supervision.
	  APDU_WaitResponseTmrId = TMR_SetTimer ( 5000 , PROC_DNP, 0x00, FALSE);  //Por ahora..
    //-- Enviar mensaje..
    DNPL_SendAppMsg ( 8, bTmpTxToken);
  }

  return; 
}


//-----------------------------------------------------------------------------
// Analiza la respuesta del driver por el envio del mensaje
//
void f_DnpAnalizeMsg (void)
{
  UINT8   RxEndToken;
  UINT8   RxMsgUsrDataLen;
  INT16   wTmpSWordConv;
  UINT8   bTmpCntr, bTmpByte;
  UINT8   TmpTimerId;  
  UINT8   bTmpBitCntr;
  void*   vpAuxBuffer;
  LPAPU_MSG_RESPONSE  pAppDataBuffer;
  
  TMR_FreeTimer (APDU_WaitResponseTmrId);
  APDU_WaitResponseTmrId = 0xFF;
  
// Recuperar el puntero al buffer utilizado para recibir el mensaje
// El token del buffer está en la parte baja del parámetro del mensaje
  RxEndToken = (UINT8)CurrentMsg.Param;
  
  // Si no se puede obtener el puntero ERROR DE CONSISTENCIA..Volver a las seq. de poling
  if (MemGetBufferPtr( RxEndToken , &vpAuxBuffer ) != OK ){    
  // ABR: control de errores
	  (void)TMR_SetTimer ( SYS_ConfigParam.ScanRate , PROC_DNP, f_DnpReinitPoll, FALSE);
    return;   
  }
  
  DNP_TxNoAnswerAcc = 0x00;   // Resetear el contador de mensajes sin respuesta.
  
// Obtener el valor de la longitud de datos recibidos  
   RxMsgUsrDataLen = ((LPRTS_MSG_FORMAT)vpAuxBuffer)->bLenght;
   RxMsgUsrDataLen -= 5;                      // Data Bytes sin CRC

//--ABR: DEBUG...
//  StatisticData.SerialStat.MsgTxErr   = (RxMsgUsrDataLen);
//  StatisticData.SerialStat.MsgRxErr   = (DNP_PolingTableIdx);
//--ABR: DEBUG...
 
  pAppDataBuffer =  (LPAPU_MSG_RESPONSE)(&(((LPRTS_MSG_FORMAT)vpAuxBuffer)->bDataBlock1[0]));
//  pAppDataBuffer->TransportHeader  
//  pAppDataBuffer->ApplicationControl
//  pAppDataBuffer->FunctionCode 
//  pAppDataBuffer->ObjectGroup 
//  pAppDataBuffer->ObjectVariation
//  pAppDataBuffer->Qualifier
  
  // Guardar el estado reportado por el esclavo en al final de las entradas digitales
  DNP_BinaryInputs[8] = pAppDataBuffer->IIN_1; 
  DNP_BinaryInputs[9] = pAppDataBuffer->IIN_2; 
  
//  if ( RxMsgUsrDataLen < 10 ){        //  Bytes del header de AppLayer de una respuesta
    //- No es un mensaje con la informacion requerida..
    // ABR: ToDo
//  }
  
  // Iniciar timer para preparar el envio del proximo comando
	TmpTimerId = TMR_SetTimer ( SYS_ConfigParam.ScanRate , PROC_DNP, f_DnpReinitPoll, FALSE);

  //-- Analizar el mensaje de respuesta
  switch ( pAppDataBuffer->ObjectGroup ){
    
    case 30:			                              // Entradas analogicas OG=30 OV=4
#ifdef  UDP_DEBUG_PRINTFON
		  (void)sprintf((char *)udp_buf,"DNP_OG30: DataLen =%d\n", RxMsgUsrDataLen);
      udp_data_to_send = TRUE;
#endif    
      bTmpByte = 2;
      for ( bTmpCntr = 0 ; bTmpCntr < DNP_PolingTable[DNP_POLIDX_AAGINP].DataObjToRx ; bTmpCntr++ ){ //106
        DNPL_ChgWordEndian (((UINT16*)(&(pAppDataBuffer->bData[bTmpByte]))));
        wTmpSWordConv = *((INT16*)(&(pAppDataBuffer->bData[bTmpByte])));
	      DNP_AnalogInputs[bTmpCntr] =  (float)wTmpSWordConv;
	  
	      if (AagInpScale[bTmpCntr] != 1)
  	      DNP_AnalogInputs[bTmpCntr] *= AagInpScale[bTmpCntr];
	      bTmpByte += 2;
      }
      break;
      
    case 1:																			// Entadas binarias   OG=1 OV=1
      bTmpByte = 2;															
      for ( bTmpCntr = 0 ; bTmpCntr < DNP_PolingTable[DNP_POLIDX_BININP].DataObjToRx  ; bTmpCntr++ ){		//8
	      DNP_BinaryInputs[bTmpCntr] =  pAppDataBuffer->bData[bTmpByte];
	      bTmpByte ++;
      }
//    for ( bTmpCntr = 0 ; bTmpCntr < 4 ; bTmpCntr++ ){
//	    DNP_BinaryInputs[bTmpCntr] =  *((UINT16*)(&(pAppDataBuffer->bData[bTmpByte])));
//      DNPL_ChgWordEndian (&(DNP_BinaryInputs[bTmpCntr]));     // porque se leera desde modbus
//	    bTmpByte +=2;
//    }
//  }
  		break;
  		
    case 10:																	 // Estado de salidas digitales OG=10 0V=2
      bTmpByte = 2;														 
      for ( bTmpCntr = 0 ; bTmpCntr < DNP_PolingTable[DNP_POLIDX_BINOUT].DataObjToRx ; bTmpCntr++ ){ //16
        DNP_BinaryOutput[bTmpCntr] = 0x00;
        for ( bTmpBitCntr = 1 ; bTmpBitCntr != 0x00 ; bTmpBitCntr <<= 1 ){
          if ( pAppDataBuffer->bData[bTmpByte] & 0x80)		// Bit 7 de Status (Flag2) = State
	          DNP_BinaryOutput[bTmpCntr] |= bTmpBitCntr ;
	        bTmpByte ++;
        }	        
      }
  		break;
  		
    case 12:																	 // Respuesta a comando  OG=12 0V=1
      DNP_PolingTableIdx--;
      DNP_BinaryInputs[10] = ((LPAPU_FOMRAT_CRTLRELAY)(&(pAppDataBuffer->bData[2])))->Status; 
      TMR_ReloadTimer (TmpTimerId, 100);  // Adelantar el timer para continuar en 100 mseg
  	  break;

    case 20:			                              // Contadores 16bits OG=20 OV=6 (sin flags)
      bTmpByte = 2;
      for ( bTmpCntr = 0 ; bTmpCntr < DNP_PolingTable[DNP_POLIDX_COUNTR].DataObjToRx ; bTmpCntr++ ){ //29
        DNPL_ChgWordEndian (((UINT16*)(&(pAppDataBuffer->bData[bTmpByte]))));
        DNP_CountersWord[bTmpCntr] = *((UINT16*)(&(pAppDataBuffer->bData[bTmpByte])));      	  
	      bTmpByte += 2;
      }
      break;

  	default:  
  	  break;
  }
  		

  (void)MemFreeBuffer ( RxEndToken);                          // Liberar el buffer de recepcion

//	(void)TMR_SetTimer ( SYS_ConfigParam.ScanRate , PROC_DNP, f_DnpReinitPoll, FALSE);
  
  TPDU_SeqNumber++;  
//  TPDU_SeqNumber = (UINT8)(TPDU_SeqNumber % (TPDU_BITMASK_TH_SEQ+1));  
  TPDU_SeqNumber &= TPDU_BITMASK_TH_SEQ;  
	
  APDU_SeqNumber++;  
//  APDU_SeqNumber = (UINT8)(APDU_SeqNumber % (APDU_BITMASK_AC_SEQ+1));  
  APDU_SeqNumber &= APDU_BITMASK_AC_SEQ;  
  
  DNP_PolingTableIdx++;
  DNP_PolingTableIdx = (UINT8)(DNP_PolingTableIdx % (DNP_POLIDX_MAX_QTTY));  
	
	return;
	
}

//-----------------------------------------------------------------------------
// Retomar la tarea de polling
//
void f_DnpReinitPoll (void)
{

  PutEventMsg (EVT_DNP_POLLREQ, PROC_DNP, PROC_DNP, 0x00);
  return;
}


//-----------------------------------------------------------------------------
// Encolar comando de Modificacion de salida a ON
//
void f_DnpSetOutputOn (void)
{

  DNP_CmdPendingFlag = 0x00;
  DNP_CmdPendingFlag |= DNP_CMDFLAG_ON;			        // Comando de llevar a estado ON
  DNP_CmdPendingFlag |= DNP_CMDFLAG_I0;							// Comenzar con instacia 0= SELECT

  DNP_CmdPendingIdx = (UINT8)CurrentMsg.Param;       // En el parametro del msg. esta el idx
  return;
}


//-----------------------------------------------------------------------------
// Encolar comando de Modificacion de salida a OFF
//
void f_DnpSetOutputOff (void)
{

  DNP_CmdPendingFlag = 0x00;
  DNP_CmdPendingFlag |= DNP_CMDFLAG_OFF;			      // Comando de llevar a estado OFF
  DNP_CmdPendingFlag |= DNP_CMDFLAG_I0;							// Comenzar con instacia 0= SELECT

  DNP_CmdPendingIdx = (UINT8)CurrentMsg.Param;      // En el parametro del msg. esta el idx
  return;
}




///////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// DNP_Init : inicializacion de variables del proceso
//-----------------------------------------------------------------------------
//
void DNP_Init(void)
{
  UINT8   bTmpCntr;
   
  DNP_CmdPendingFlag = 0x00;
  DNP_CmdPendingIdx  = 0x00;
  DNP_PolingTableIdx = 0x00;
  
  TPDU_SeqNumber = 0x00;
  APDU_SeqNumber = 0x01;

  DNP_TxNoAnswerAcc = 0x00;

  // Inicializar todas las variables
  for ( bTmpCntr = 0 ; bTmpCntr <= 106 ; bTmpCntr++ ){
	  DNP_AnalogInputs[bTmpCntr] =  0;
  }
  for ( bTmpCntr = 0 ; bTmpCntr < 11 ; bTmpCntr++ ){
	  DNP_BinaryInputs[bTmpCntr] =  0x00;
  }
  for ( bTmpCntr = 0 ; bTmpCntr < 16 ; bTmpCntr++ ){
	  DNP_BinaryOutput[bTmpCntr] =  0x00;
  }
      
  DNP_BinaryInputs[11] |= SYS_FLAG_SCIADOWN; 


  
  DNPL_Init();

//	DnpQueueIdx.Counter 	= 0x00;
//	DnpQueueIdx.IndexOut	= 0x00;
//	DnpQueueIdx.IndexIn	= 0x00;  

//  PeriodicTimer = SYS_ConfigParam.ScanRate;
//  DnpPutMsg (0x02, 0x05);

}

void DNP_ReInit(void)
{
   
  DNP_CmdPendingFlag = 0x00;
  DNP_CmdPendingIdx  = 0x00;
  DNP_PolingTableIdx = 0x00;
  
  TPDU_SeqNumber = 0x00;
  APDU_SeqNumber = 0x01;

  DNP_TxNoAnswerAcc = 0x00;
      
}


/*
//-----------------------------------------------------------------------------
// Sacar un mensaje de la cola 
//-----------------------------------------------------------------------------
//
UINT8 DnpGetMsgAndQuit (void)
{
	if (DnpQueueIdx.Counter) {
		pDnpOutMsg->START     = DnpMsgQueue [DnpQueueIdx.IndexOut].Data_0;
		pDnpOutMsg->Data[0] 	= DnpMsgQueue [DnpQueueIdx.IndexOut].Data_1;
		
		DnpQueueIdx.IndexOut++;										
		if (DnpQueueIdx.IndexOut == MAX_DNPMSG_QUEUE) DnpQueueIdx.IndexOut = 0x00;				
		DnpQueueIdx.Counter--;							    
		return TRUE;
	}
	return FALSE;
}

UINT8 DnpGetMsg (void)
{
	if (DnpQueueIdx.Counter) {
		pDnpOutMsg->START     = DnpMsgQueue [DnpQueueIdx.IndexOut].Data_0;
		pDnpOutMsg->Data[0] 	= DnpMsgQueue [DnpQueueIdx.IndexOut].Data_1;
	
		return TRUE;
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
// Guardar un mensaje en la cola 
//-----------------------------------------------------------------------------
//
void DnpPutMsg (UINT8 Data_0, UINT8 Data_1)
{
	if (DnpQueueIdx.Counter < MAX_DNPMSG_QUEUE) {
		DnpMsgQueue [DnpQueueIdx.IndexIn].Data_0  = Data_0;
		DnpMsgQueue [DnpQueueIdx.IndexIn].Data_1  = Data_1;
		
		DnpQueueIdx.IndexIn++;
		if (DnpQueueIdx.IndexIn == MAX_DNPMSG_QUEUE) DnpQueueIdx.IndexIn = 0x00;
		DnpQueueIdx.Counter++;
	}
	return;
}	
*/


#pragma CODE_SEG DEFAULT

