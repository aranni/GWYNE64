#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"

#include "Commdef.h"
#include "pModbusTCP.h"
#include "Global.h"


///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_MODBUS
///////////////////////////////////////////////////////////////////////////////
//
const EVENT_FUNCTION	ModbusProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_ModbusIdleTimeout	},
{EVT_MOD_RXMSG 	      , f_ModbusAnalizeMsg  },
{EVT_DBG_TXMSG 	      , f_ModbusSendDbgMsg  },
{EVT_OTHER_MSG     		, f_Consume           }};


///////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_MODBUS
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// Reintentar envio de mensaje modbus
//-----------------------------------------------------------------------------
//
void f_ModbusIdleTimeout (void)
{
  if (TCP_SendModbusMsg()){				  // Error en envio de respuesta
    // Error en reintento, acciones correctivas
    DummyVar = 0x00;
  }
	return;
}


//-----------------------------------------------------------------------------
// Enviar informacion de debug para protocolo Modbus
//-----------------------------------------------------------------------------
//
void f_ModbusSendDbgMsg (void)
{
  DummyVar = sprintf(DebugBuffer, "MOD_InMSG: TransID=%X, UI=%X, FC=%X \r\0",
                     ModbusInMsg.TransactionID0,
                     ModbusInMsg.UI,
                     ModbusInMsg.FC);
                     
  PutEventMsg (EVT_DBG_TXPRN, PROC_DEBUG, PROC_MODBUS, DBG_BY_SERIAL);
	return;
}


//-----------------------------------------------------------------------------
// Analizar mensaje Modbus recibido
//-----------------------------------------------------------------------------
//
void f_ModbusAnalizeMsg (void)
{

  UINT8   ValuesRequired;                         // cantidad de WORDS solicitadas
  UINT8   BytesRequired;
  UINT8   * pInDataMsg;
  UINT8   * pOutDataMsg;
  UINT8   * pInverterParam;
  UINT16  ModbusOffset;
  UINT8   ModbusOffsetIdx;
  
  switch(ModbusInMsg.FC){
    case MOD_FC_RD_MUL_REG:										            // Read Multiple Register
        ValuesRequired = ModbusInMsg.Data[3];
        if ( ValuesRequired > MAX_PARAM_QTTY ) return;
        BytesRequired = ( 2 * ValuesRequired );
        
//        GIO_GetInpVal();          // ABR:leer periódicamente las entradas..     
        MOD_BuildBasicResponse();
        ModbusOutMsg.LengthLow = 3 + BytesRequired;  
        ModbusOutMsg.Data[0] = BytesRequired;  
        
        pInverterParam = (UINT8 *)(&InverterParam);  
        pOutDataMsg = (UINT8 *)(&(ModbusOutMsg.Data[1]));
        
        ModbusOffset   = *((UINT16*)&(ModbusInMsg.Data[0]));
        
        ModbusOffsetIdx = (UINT8)(ModbusOffset / 100);
        ModbusOffset   *= 2;

//        ModbusOffset   = ( 2 * (ModbusInMsg.Data[1]) );      // Offset
       
        switch ( ModbusOffsetIdx ){
        
          case MOD_SYS_OFFSET_IDX:
            pInverterParam = (UINT8 *)(&SYS_ConfigParam);  
            ModbusOffset -= (MOD_SYS_OFFSET_BY);          
            break;
            
          case MOD_STA_OFFSET_IDX:
            pInverterParam = (UINT8 *)(&StatisticData);  
            ModbusOffset -= (MOD_STA_OFFSET_BY);
            break;
            
          case MOD_GIO_OFFSET_IDX:
            pInverterParam = (UINT8 *)(&InputOutputData);  
            ModbusOffset -= (MOD_GIO_OFFSET_BY);
            break;
            
          default:
            break;          
        }
        
        pInverterParam += ModbusOffset;
        DummyPtr = memcpy (pOutDataMsg, pInverterParam, BytesRequired);  
        break;
        
    case MOD_FC_WR_SIN_REG:                               // Write Single Register
        if ( ModbusInMsg.Data[1] >= MAX_PARAM_QTTY ) return;    // offset en WORDS
             
        pInDataMsg = (UINT8 *)(&(ModbusInMsg.Data[2]));
        pInverterParam = (UINT8 *)(&InverterParam);  
        pInverterParam += ( 2 * (ModbusInMsg.Data[1]) );        // Offset 
            
    		*pInverterParam = *pInDataMsg;
    		ModbusOutMsg.Data[2] =  *pInverterParam;
	      pInverterParam++;
	      pInDataMsg++;
     		*pInverterParam = *pInDataMsg;
    		ModbusOutMsg.Data[3] =  *pInverterParam;		
        MOD_BuildBasicResponse();
        ModbusOutMsg.LengthLow = 5;  
     		ModbusOutMsg.Data[0] = ModbusInMsg.Data[0];
     		ModbusOutMsg.Data[1] = ModbusInMsg.Data[1];
     		
     		MOD_AnalizeValChanges();
        break;
  
    case MOD_FC_WR_MUL_REG:										          // Write Multiple Register
        if ( ModbusInMsg.Data[1] >= MAX_PARAM_QTTY ) return;    // offset en WORDS

        ValuesRequired = ModbusInMsg.Data[3];
        if ( ValuesRequired > MAX_PARAM_QTTY ) return;
        BytesRequired = ModbusInMsg.Data[4];
        
        pInDataMsg = (UINT8 *)(&(ModbusInMsg.Data[5]));
        pInverterParam = (UINT8 *)(&InverterParam);  
        pInverterParam += ( 2 * (ModbusInMsg.Data[1]) );        // Offset 
        
        DummyPtr = memcpy (pInverterParam, pInDataMsg, BytesRequired);  
                
        MOD_BuildBasicResponse();
        ModbusOutMsg.LengthLow = 6;  
        
     		ModbusOutMsg.Data[0] = ModbusInMsg.Data[0];		          // Reference number
     		ModbusOutMsg.Data[1] = ModbusInMsg.Data[1];
        
     		ModbusOutMsg.Data[2] = ModbusInMsg.Data[2];		          // Word count
     		ModbusOutMsg.Data[3] = ModbusInMsg.Data[3];
      	
      	MOD_AnalizeValChanges();
        break;
        
        
    default:
        MOD_BuildBasicResponse();
        ModbusOutMsg.LengthLow = 3;  
        ModbusOutMsg.FC = 0x83;  
        ModbusOutMsg.Data[0] = 1;  
        break;
  }
  
  if (TCP_SendModbusMsg()){				                    // Error en envio de respuesta
	  (void)TMR_SetTimer ( TCP_TIMEOUT_SENDRETRY , PROC_MODBUS, 0x00, FALSE);	            // Reintentar
  }
	return;
}



///////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// MOD_Init : inicializacion de variables del proceso
//-----------------------------------------------------------------------------
//
void MOD_Init(void)
{
//  InverterParam.ConsignaFloat = 0;
//	InverterParam.RealFrec      = 0;
	  return;
}


//-----------------------------------------------------------------------------
// Cargar valores comunes en la trama de salida Modbus
//-----------------------------------------------------------------------------
//
void MOD_BuildBasicResponse (void)
{
  ModbusOutMsg.TransactionID0 = ModbusInMsg.TransactionID0;  
  ModbusOutMsg.TransactionID1 = ModbusInMsg.TransactionID1; 
   
  ModbusOutMsg.ProtocolID0 = ModbusInMsg.ProtocolID0;  
  ModbusOutMsg.ProtocolID1 = ModbusInMsg.ProtocolID1; 
  
  ModbusOutMsg.LengthHigh = 0x00;  
//  ModbusOutMsg.LengthLow = ;  
  ModbusOutMsg.UI = ModbusInMsg.UI;  
  ModbusOutMsg.FC = ModbusInMsg.FC;  
}


//-----------------------------------------------------------------------------
// Analizar comandos recibidos y cambios en variables 
//-----------------------------------------------------------------------------
//
void MOD_AnalizeValChanges (void)
{
    UINT8 TmpByte;
        
    TmpByte = (UINT8)(InverterParam.CmdPending & 0x00FF);
    if ( TmpByte){
      switch (TmpByte){
        case CMD_VAR_START:
          PutEventMsg (EVT_VAR_CMDSTART, SYS_ConfigParam.InvCode, PROC_MODBUS, 0x00);
          break;

        case CMD_VAR_STOP:
          PutEventMsg (EVT_VAR_CMDSTOP, SYS_ConfigParam.InvCode, PROC_MODBUS, 0x00);
          break;

        case CMD_VAR_SETFREC:
          SYS_ConfigParam.ConsignaFrec = InverterParam.CmdParam;
          eeprom_write_buffer(EE_ADDR_INV_FREC, (UINT8 *)&SYS_ConfigParam.ConsignaFrec, 4); 

          PutEventMsg (EVT_VAR_CMDSETFREC, SYS_ConfigParam.InvCode, PROC_MODBUS, 0x00);
          break;

        case CMD_VAR_SETRELTX:
          SYS_ConfigParam.RelacionTx = InverterParam.CmdParam;
          eeprom_write_buffer(EE_ADDR_PAR_RELTX, (UINT8 *)&SYS_ConfigParam.RelacionTx, 4); 
          break;

        case CMD_VAR_SETCRESB:
          SYS_ConfigParam.CoefResbal = InverterParam.CmdParam;
          eeprom_write_buffer(EE_ADDR_PAR_CRESB, (UINT8 *)&SYS_ConfigParam.CoefResbal, 4); 
          break;
          
        case CMD_VAR_SETRPMOT:
          SYS_ConfigParam.RpmEnMotor = InverterParam.CmdParam;
          eeprom_write_buffer(EE_ADDR_PAR_RPMOT, (UINT8 *)&SYS_ConfigParam.RpmEnMotor, 4); 
          break;
          
        case CMD_VAR_RESETFAULT:
          PutEventMsg (EVT_VAR_CMDRESETFAULT, SYS_ConfigParam.InvCode, PROC_MODBUS, 0x00);        
          break;

        case CMD_VAR_CLEARSTAT:
//          PutEventMsg (EVT_VAR_CMDDEBUG, SYS_ConfigParam.InvCode, PROC_MODBUS, 0x00); 
          StatisticData.OsStat.MsgRstCntr = 0x00;
          StatisticData.OsStat.MsgDogCntr = 0x00;
          eeprom_write_buffer(EE_ADDR_DOG_CNTR, (UINT8 *)&(StatisticData.OsStat.MsgDogCntr), 2);     
          eeprom_write_buffer(EE_ADDR_RST_CNTR, (UINT8 *)&(StatisticData.OsStat.MsgRstCntr), 2);     
          break;

        case CMD_VAR_SETAAG_A:
          InputOutputData.AAG_Output_A = InverterParam.CmdParam;
          GIO_SetAAGOut (0x00);
          break;

        case CMD_VAR_SETAAG_B:
          InputOutputData.AAG_Output_B = InverterParam.CmdParam;
          GIO_SetAAGOut (0x01);
          break;

        case CMD_VAR_DEBUG:
          PutEventMsg (EVT_VAR_CMDDEBUG, SYS_ConfigParam.InvCode, PROC_MODBUS, 0x00); 
          break;
          
        default:
          break;
      }
      InverterParam.CmdPending = 0x00;
    }  
    
//    GIO_SetOutVal((UINT8)InverterParam.OutVal);     // escribir periódicamente..

}

#pragma CODE_SEG DEFAULT

//    if (InverterParam.ConsignaFloat != SYS_ConfigParam.ConsignaFrec){
//      if (( InverterParam.ConsignaFloat >= InverterParam.MinFrec ) &&
//          ( InverterParam.ConsignaFloat <= InverterParam.MaxFrec ) ){                 
//              SYS_ConfigParam.ConsignaFrec = InverterParam.ConsignaFloat;
//              eeprom_write_buffer(EE_ADDR_INV_FREC, (UINT8 *)&SYS_ConfigParam.ConsignaFrec, 4); 
//          }
//    }
        
