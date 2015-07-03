#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"

#include "Commdef.h"
#include "pModbusRTU.h"
#include "Global.h"


///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_MODRTU
///////////////////////////////////////////////////////////////////////////////
//
const EVENT_FUNCTION	ModRtuProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_ModRtuIdleTimeout	},
{EVT_232_RXMSG 	      , f_ModRtuAnalizeMsg  },
{EVT_DBG_TXMSG 	      , f_ModRtuSendDbgMsg  },
{EVT_OTHER_MSG     		, f_Consume           }};


///////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_MODRTU
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// Reintentar envio de mensaje modbus
//-----------------------------------------------------------------------------
//
void f_ModRtuIdleTimeout (void)
{
	return;
}


//-----------------------------------------------------------------------------
// Enviar informacion de debug para protocolo Modbus
//-----------------------------------------------------------------------------
//
void f_ModRtuSendDbgMsg (void)
{
  DummyVar = sprintf(DebugBuffer, "MOD_InMSG: UI=%X, FC=%X \r\0",
                     pModRtuInMsg->UI,
                     pModRtuInMsg->FC);
                     
  PutEventMsg (EVT_DBG_TXPRN, PROC_DEBUG, PROC_MODBUS, DBG_BY_SERIAL);
	return;
}


//-----------------------------------------------------------------------------
// Analizar mensaje ModRtu recibido
//-----------------------------------------------------------------------------
//
void f_ModRtuAnalizeMsg (void)
{

  UINT8   ValuesRequired;                         // cantidad de WORDS solicitadas
  UINT8   BytesRequired;
  UINT8   * pInDataMsg;
  UINT8   * pOutDataMsg;
  UINT8   * pInverterParam;
  UINT16  ModRtuOffset;
  
  switch(pModRtuInMsg->FC){
    case MOD_FC_RD_MUL_REG:										            // Read Multiple Register
        ValuesRequired = pModRtuInMsg->Data[3];
        if ( ValuesRequired > MAX_PARAM_QTTY ) return;
        BytesRequired = ( 2 * ValuesRequired );
        
        GIO_GetInpVal();        
        MRT_BuildBasicResponse();
        pModRtuOutMsg->Data[0] = BytesRequired;  				    // Len
        
        pInverterParam = (UINT8 *)(&InverterParam);  
        pOutDataMsg = (UINT8 *)(&(pModRtuOutMsg->Data[1]));
        ModRtuOffset   = ( 2 * (pModRtuInMsg->Data[1]) );   // Offset
        
        if (ModRtuOffset >= MOD_SYS_OFFSET_BY){
//          if (ModRtuOffset >= MOD_STA_OFFSET_BY){
//            pInverterParam = (UINT8 *)(&StatisticData);  
//            ModRtuOffset -= (MOD_STA_OFFSET_BY);
//          }
//          else{            
            pInverterParam = (UINT8 *)(&SYS_ConfigParam);  
            ModRtuOffset -= (MOD_SYS_OFFSET_BY);
//          }
        }

        pInverterParam += ModRtuOffset;
        DummyPtr = memcpy (pOutDataMsg, pInverterParam, BytesRequired);
        break;
        
    case MOD_FC_WR_SIN_REG:                               // Write Single Register
        if ( pModRtuInMsg->Data[1] >= MAX_PARAM_QTTY ) return;    // offset en WORDS
             
        pInDataMsg = (UINT8 *)(&(pModRtuInMsg->Data[2]));
        pInverterParam = (UINT8 *)(&InverterParam);  
        pInverterParam += ( 2 * (pModRtuInMsg->Data[1]) );        // Offset 
            
    		*pInverterParam = *pInDataMsg;
    		pModRtuOutMsg->Data[2] =  *pInverterParam;
	      pInverterParam++;
	      pInDataMsg++;
     		*pInverterParam = *pInDataMsg;
    		pModRtuOutMsg->Data[3] =  *pInverterParam;		
        MRT_BuildBasicResponse();
     		pModRtuOutMsg->Data[0] = pModRtuInMsg->Data[0];
     		pModRtuOutMsg->Data[1] = pModRtuInMsg->Data[1];
     		
     		BytesRequired = 0x03;
     		MRT_AnalizeValChanges();
        break;
  
    case MOD_FC_WR_MUL_REG:										          // Write Multiple Register
        if ( pModRtuInMsg->Data[1] >= MAX_PARAM_QTTY ) return;    // offset en WORDS

        ValuesRequired = pModRtuInMsg->Data[3];
        if ( ValuesRequired > MAX_PARAM_QTTY ) return;
        BytesRequired = pModRtuInMsg->Data[4];
        
        pInDataMsg = (UINT8 *)(&(pModRtuInMsg->Data[5]));
        pInverterParam = (UINT8 *)(&InverterParam);  
        pInverterParam += ( 2 * (pModRtuInMsg->Data[1]) );        // Offset 
        
        DummyPtr = memcpy (pInverterParam, pInDataMsg, BytesRequired);  
                
        MRT_BuildBasicResponse();
        
     		pModRtuOutMsg->Data[0] = pModRtuInMsg->Data[0];		          // Reference number
     		pModRtuOutMsg->Data[1] = pModRtuInMsg->Data[1];
        
     		pModRtuOutMsg->Data[2] = pModRtuInMsg->Data[2];		          // Word count
     		pModRtuOutMsg->Data[3] = pModRtuInMsg->Data[3];
     		
      	BytesRequired = 0x03;
      	MRT_AnalizeValChanges();
        break;
        
        
    default:
        MRT_BuildBasicResponse();
        pModRtuOutMsg->FC = 0x83;  
        pModRtuOutMsg->Data[0] = 1; 
        BytesRequired = 0x00; 
        break;
  }
  
  ModRtuOffset = MRT_BuildCRC16((UINT8 *)pModRtuOutMsg, (BytesRequired+3));
  *((UINT16 *)(&(pModRtuOutMsg->Data[BytesRequired+1]))) = ModRtuOffset;
  RTU_TransmitFrame ((BytesRequired+5),(UINT8 *)pModRtuOutMsg);
//  RTU_ReceiveFrame (8, (UINT8 *)pModRtuInMsg);  // ABR:12-JUN-06
//	DummyVar =  TMR_SetTimer ( 20 , PROC_MODRTU, 0x00, FALSE);

	return;
}



///////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// MRT_Init : inicializacion de variables del proceso
//-----------------------------------------------------------------------------
//
void MRT_Init(void)
{
//  InverterParam.ConsignaFloat = 0;
//	InverterParam.RealFrec      = 0;
    pModRtuInMsg  = (MODBUS_RTU_MSG*)SCI2_InBuff;
    pModRtuOutMsg = (MODBUS_RTU_MSG*)SCI2_OutBuff;  

    RTU_ReceiveFrame (8, (UINT8 *)pModRtuInMsg);
	  return;
}


//-----------------------------------------------------------------------------
// Cargar valores comunes en la trama de salida Modbus
//-----------------------------------------------------------------------------
//
void MRT_BuildBasicResponse (void)
{
  pModRtuOutMsg->UI = pModRtuInMsg->UI;  
  pModRtuOutMsg->FC = pModRtuInMsg->FC;  
}

//-----------------------------------------------------------------------------
// Analizar comandos recibidos y cambios en variables 
//-----------------------------------------------------------------------------
//
void MRT_AnalizeValChanges (void)
{

  GIO_SetOutVal((UINT8)InverterParam.OutVal);  
  return;
}


//-----------------------------------------------------------------------------
// Calcular CRC
//-----------------------------------------------------------------------------
//
//  The function returns the CRC as a unsigned short type 
//  puchMsg   =  message to calculate CRC upon
//  usDataLen =  quantity of bytes in message


UINT16 MRT_BuildCRC16 ( UINT8 * puchMsg, UINT16 usDataLen )
{
  UINT16 CRC_Outcome;
  UINT8  TmpByte;
  UINT8  TmpIdx;
  UINT16 TmpWord;
  
  CRC_Outcome = 0xFFFF;
  
  while (usDataLen--){                          // pass through message buffer
    TmpByte = (*puchMsg);
    puchMsg++;
    for (TmpIdx = 0; TmpIdx < 8; TmpIdx++){
      TmpWord = (CRC_Outcome ^ TmpByte) & 1;
      CRC_Outcome >>= 1;
      TmpByte >>= 1;
      if (TmpWord)
        CRC_Outcome ^= REVPOLY;
    }
  }
  
  TmpByte = (CRC_Outcome & 0xFF00)>>8;
  CRC_Outcome <<= 8;
  CRC_Outcome |= TmpByte;
  
  return CRC_Outcome;  
}

#pragma CODE_SEG DEFAULT

