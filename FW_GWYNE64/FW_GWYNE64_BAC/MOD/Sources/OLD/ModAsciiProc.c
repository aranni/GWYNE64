#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"

#include "Commdef.h"
#include "ModAsciiProc.h"
#include "Global.h"


///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_ModAscii
///////////////////////////////////////////////////////////////////////////////
//
EVENT_FUNCTION	ModAsciiProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_ModAsciiIdleTimeout	},
{EVT_232_RXMSG 	      , f_ModAsciiAnalizeMsg  },
{EVT_DBG_TXMSG 	      , f_ModAsciiSendDbgMsg  },
{EVT_OTHER_MSG     		, f_Consume           }};


///////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_ModAscii
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// Reintentar envio de mensaje modbus
//-----------------------------------------------------------------------------
//
void f_ModAsciiIdleTimeout (void)
{
	return;
}


//-----------------------------------------------------------------------------
// Analizar mensaje ModAscii recibido
//-----------------------------------------------------------------------------
//
void f_ModAsciiAnalizeMsg (void)
{

  UINT8   ValuesRequired;                         // cantidad de WORDS solicitadas
  UINT8   BytesRequired;
  UINT8   * pInDataMsg;
  UINT8   * pOutDataMsg;
  UINT8   * pInverterParam;
  UINT16  ModAsciiOffset;
  
  switch(pModAsciiInMsg->FC){
    case MOD_FC_RD_MUL_REG:										            // Read Multiple Register
        ValuesRequired = pModAsciiInMsg->Data[3];
        if ( ValuesRequired > MAX_PARAM_QTTY ) return;
        BytesRequired = ( 2 * ValuesRequired );
        
        GIO_GetInpVal();        
        MRT_BuildBasicResponse();
        pModAsciiOutMsg->Data[0] = BytesRequired;  				    // Len
        
        pInverterParam = (UINT8 *)(&InverterParam);  
        pOutDataMsg = (UINT8 *)(&(pModAsciiOutMsg->Data[1]));
        ModAsciiOffset   = ( 2 * (pModAsciiInMsg->Data[1]) );   // Offset
        
        if (ModAsciiOffset >= MOD_SYS_OFFSET_BY){
          if (ModAsciiOffset >= MOD_STA_OFFSET_BY){
            pInverterParam = (UINT8 *)(&StatisticData);  
            ModAsciiOffset -= (MOD_STA_OFFSET_BY);
          }
          else{            
            pInverterParam = (UINT8 *)(&SYS_ConfigParam);  
            ModAsciiOffset -= (MOD_SYS_OFFSET_BY);
          }
        }

        pInverterParam += ModAsciiOffset;
        DummyPtr = memcpy (pOutDataMsg, pInverterParam, BytesRequired);
        break;
        
    case MOD_FC_WR_SIN_REG:                               // Write Single Register
        if ( pModAsciiInMsg->Data[1] >= MAX_PARAM_QTTY ) return;    // offset en WORDS
             
        pInDataMsg = (UINT8 *)(&(pModAsciiInMsg->Data[2]));
        pInverterParam = (UINT8 *)(&InverterParam);  
        pInverterParam += ( 2 * (pModAsciiInMsg->Data[1]) );        // Offset 
            
    		*pInverterParam = *pInDataMsg;
    		pModAsciiOutMsg->Data[2] =  *pInverterParam;
	      pInverterParam++;
	      pInDataMsg++;
     		*pInverterParam = *pInDataMsg;
    		pModAsciiOutMsg->Data[3] =  *pInverterParam;		
        MRT_BuildBasicResponse();
     		pModAsciiOutMsg->Data[0] = pModAsciiInMsg->Data[0];
     		pModAsciiOutMsg->Data[1] = pModAsciiInMsg->Data[1];
     		
     		BytesRequired = 0x03;
     		MRT_AnalizeValChanges();
        break;
  
    case MOD_FC_WR_MUL_REG:										          // Write Multiple Register
        if ( pModAsciiInMsg->Data[1] >= MAX_PARAM_QTTY ) return;    // offset en WORDS

        ValuesRequired = pModAsciiInMsg->Data[3];
        if ( ValuesRequired > MAX_PARAM_QTTY ) return;
        BytesRequired = pModAsciiInMsg->Data[4];
        
        pInDataMsg = (UINT8 *)(&(pModAsciiInMsg->Data[5]));
        pInverterParam = (UINT8 *)(&InverterParam);  
        pInverterParam += ( 2 * (pModAsciiInMsg->Data[1]) );        // Offset 
        
        DummyPtr = memcpy (pInverterParam, pInDataMsg, BytesRequired);  
                
        MRT_BuildBasicResponse();
        
     		pModAsciiOutMsg->Data[0] = pModAsciiInMsg->Data[0];		          // Reference number
     		pModAsciiOutMsg->Data[1] = pModAsciiInMsg->Data[1];
        
     		pModAsciiOutMsg->Data[2] = pModAsciiInMsg->Data[2];		          // Word count
     		pModAsciiOutMsg->Data[3] = pModAsciiInMsg->Data[3];
     		
      	BytesRequired = 0x03;
      	MRT_AnalizeValChanges();
        break;
        
        
    default:
        MRT_BuildBasicResponse();
        pModAsciiOutMsg->FC = 0x83;  
        pModAsciiOutMsg->Data[0] = 1; 
        BytesRequired = 0x00; 
        break;
  }
  
  ModAsciiOffset = MRT_BuildCRC16((UINT8 *)pModAsciiOutMsg, (BytesRequired+3));
  *((UINT16 *)(&(pModAsciiOutMsg->Data[BytesRequired+1]))) = ModAsciiOffset;
  RTU_TransmitFrame ((BytesRequired+5),(UINT8 *)pModAsciiOutMsg);
//  RTU_ReceiveFrame (8, (UINT8 *)pModAsciiInMsg);
//	DummyVar =  TMR_SetTimer ( 20 , PROC_ModAscii, 0x00, FALSE);

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
    pModAsciiInMsg  = (MODBUS_RTU_MSG*)SRL2_InBuff;
    pModAsciiOutMsg = (MODBUS_RTU_MSG*)SRL2_OutBuff;  

    RTU_ReceiveFrame (8, (UINT8 *)pModAsciiInMsg);
	  return;
}


//-----------------------------------------------------------------------------
// Cargar valores comunes en la trama de salida Modbus
//-----------------------------------------------------------------------------
//
void MRT_BuildBasicResponse (void)
{
  pModAsciiOutMsg->UI = pModAsciiInMsg->UI;  
  pModAsciiOutMsg->FC = pModAsciiInMsg->FC;  
}

//-----------------------------------------------------------------------------
// Analizar comandos recibidos y cambios en variables 
//-----------------------------------------------------------------------------
//
void MRT_AnalizeValChanges (void)
{
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

