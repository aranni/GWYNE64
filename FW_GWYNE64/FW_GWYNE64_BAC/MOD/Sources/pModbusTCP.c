#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"

#include "Commdef.h"
#include "pModbusTCP.h"
#include "Global.h"
#include "system.h"
#include "tcp_ip.h"


///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_MODBUS
///////////////////////////////////////////////////////////////////////////////
//
const EVENT_FUNCTION	ModbusProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_ModbusIdleTimeout	},
{EVT_MOD_RXMSG 	      , f_ModbusAnalizeMsg  },
{EVT_DBG_TXMSG 	      , f_ModbusSendDbgMsg  },
{EVT_ETH_RETRY        , f_ModbusRetryTxMsg  },
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

  UINT8 TmpIdx;
  UINT8 CurrentTimerId;

  CurrentTimerId = (UINT8)CurrentMsg.Param;

  // Buscar con el id del timer a qué socket modbus tcp pertenece el timeout
  TmpIdx=0; 
  while (CurrentTimerId != TcpSockSrv[TmpIdx].TimerId )
  {   
    TmpIdx++;
    if ( TmpIdx >= MAX_SRV_TCPSOCH ) {
        return;
    }
  }

  TcpSockSrv[TmpIdx].TimerId = 0xFF;
  PutEventMsg (EVT_MOD_RXMSG, PROC_MODBUS, PROC_MODBUS, TmpIdx);
	
	return;
}



//-----------------------------------------------------------------------------
// Retransmitir mensaje Modbus recibido
//-----------------------------------------------------------------------------
//
void f_ModbusRetryTxMsg (void)
{

  UINT8   CurrentParam;  
  UINT8   TcpMsgLen; 
  UINT8   RxModBusTcpToken;
  void*   vpAuxBuffer;
  UINT8*	pModbusInMsg;					// Mensaje recibido por ModbusTCP.
  
  CurrentParam = (UINT8)CurrentMsg.Param;
  if (CurrentParam >= MAX_SRV_TCPSOCH)  
      return;       // ABR_ : (SW_ERR) analizar acciones a tomar     
  
  RxModBusTcpToken = TcpSockSrv[CurrentParam].BufferToken;
  if (MemGetBufferPtr( RxModBusTcpToken , &vpAuxBuffer ) != OK ){    
  // ABR: control de errores
    return;   
  }
  pModbusInMsg = (UINT8*)vpAuxBuffer;

  // Verificar si es posible enviar datos (puede haber un msg. previo aun sin ACK)
//  if (tcp_checksend(tcp_soch) < 0){
  if (tcp_checksend(TcpSockSrv[CurrentParam].TcpSrvTcpSoch) < 0){
    if (TcpSockSrv[CurrentParam].TxRetries) // si quedan reintentos, reintentar luego de un tiempo
    {      
      TcpSockSrv[CurrentParam].TxRetries--;
	    TcpSockSrv[CurrentParam].TimerId = TMR_SetTimer ( TCP_TIMEOUT_BUSYSENDRETRY , PROC_MODBUS, 0x00, FALSE );	// Reintentar
    } 
    return;
  }

  // recupero el mensaje, del buffer asociado al socket
  TcpMsgLen = *((UINT8*)pModbusInMsg);
  (void)memcpy ((UINT8*)(&net_buf[TCP_APP_OFFSET]), &((UINT8*)pModbusInMsg)[1], TcpMsgLen);
  
  if (tcp_send(TcpSockSrv[CurrentParam].TcpSrvTcpSoch, &net_buf[TCP_APP_OFFSET], NETWORK_TX_BUFFER_SIZE - TCP_APP_OFFSET, TcpMsgLen) != TcpMsgLen)
  { 
    if (TcpSockSrv[CurrentParam].TxRetries) // si quedan reintentos, reintentar luego de un tiempo
    {      
        TcpSockSrv[CurrentParam].TxRetries--;
  	    TcpSockSrv[CurrentParam].TimerId = TMR_SetTimer ( TCP_TIMEOUT_SENDRETRY , PROC_MODBUS, 0x00, FALSE );	// Reintentar
    }
	}
	else
	{
      TcpSockSrv[CurrentParam].TxRetries = 0;
	}
 	return;
}
		


//-----------------------------------------------------------------------------
// Enviar informacion de debug para protocolo Modbus
//-----------------------------------------------------------------------------
//
void f_ModbusSendDbgMsg (void)
{
/*
  DummyVar = sprintf(DebugBuffer, "MOD_InMSG: TransID=%X, UI=%X, FC=%X \r\0",
                     pModbusInMsg->TransactionID0,
                     pModbusInMsg->UI,
                     pModbusInMsg->FC);
                     
  PutEventMsg (EVT_DBG_TXPRN, PROC_DEBUG, PROC_MODBUS, DBG_BY_SERIAL);
*/
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
  UINT8   * pDataToSend;
  UINT8   * pDataToChange;
  UINT16  ModbusOffset;
  UINT8   ModbusOffsetIdx;
  UINT8   CurrentParam;  
  UINT8   TcpMsgLen; 
  UINT8   TmpCntr;
  UINT8   RxModBusTcpToken;
  void*   vpAuxBuffer;
  MODBUS_MSG*	   	 	pModbusInMsg;					// Mensaje recibido por ModbusTCP.
  
  CurrentParam = (UINT8)CurrentMsg.Param;
  if (CurrentParam >= MAX_SRV_TCPSOCH)  
      return;       // ABR_ : (SW_ERR) analizar acciones a tomar     
  
  RxModBusTcpToken = TcpSockSrv[CurrentParam].BufferToken;
  if (MemGetBufferPtr( RxModBusTcpToken , &vpAuxBuffer ) != OK ){    
  // ABR: control de errores
//    (void)MemFreeBuffer (RxModBusTcpToken);   // Liberar por las dudas ?
    return;   
  }
  pModbusInMsg = (MODBUS_MSG*)vpAuxBuffer;
  
  // Verificar si es posible enviar datos (puede haber un msg. previo aun sin ACK)
//  if (tcp_checksend(tcp_soch) < 0){
  if (tcp_checksend(TcpSockSrv[CurrentParam].TcpSrvTcpSoch) < 0){
    if (TcpSockSrv[CurrentParam].TxRetries) // si quedan reintentos, reintentar luego de un tiempo
    {      
      TcpSockSrv[CurrentParam].TxRetries--;
	    TcpSockSrv[CurrentParam].TimerId = TMR_SetTimer ( TCP_TIMEOUT_BUSYSENDRETRY , PROC_MODBUS, 0x00, FALSE );	// Reintentar
    } 
//    else
//    {
//        (void)MemFreeBuffer (RxModBusTcpToken);   // Si no se reintenta libero el buffer	  
//    }
//    return;
  }
  
  SETBIT(dwIntegrityMonitor,PROC_MODBUS);
  
  TcpMsgLen = 0x00;
  
  switch(pModbusInMsg->FC){
    

    case MOD_FC_RD_MUL_REG:										            // Read Multiple Register
        ValuesRequired = pModbusInMsg->Data[3];
        if ( ValuesRequired > MAX_MODBUS_REG ) return;		// Data[3]:0-125 Word Count FC=3
        BytesRequired = ( 2 * ValuesRequired );
        
//        MOD_BuildBasicResponse();
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->TransactionID0;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->TransactionID1;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->ProtocolID0;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->ProtocolID1;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = 0x00;	                // LengthHigh
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = 3 + BytesRequired;    // LengthLow
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->UI;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->FC;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = BytesRequired;  // Data[0] = BytesRequired
	      TcpMsgLen++;
//        ModbusOutMsg.Data[0] = BytesRequired;  
        
//        pDataToSend = (UINT8 *)(&(bac_data[0]));
        pDataToSend = (UINT8 *)(&(BAC_BankMemManager[0]));       
               
        ModbusOffset   = *((UINT16*)&(pModbusInMsg->Data[0]));        
        ModbusOffsetIdx = (UINT8)(ModbusOffset / 100);
//        ModbusOffset   *= 2;

        if (ModbusOffsetIdx >= 10) 
        {
            ModbusOffsetIdx /=  10;
            if (ModbusOffsetIdx > MAX_BAC_BANKS) return;   // Direccion fuera de rango..
            pDataToSend = (UINT8 *)(&(bac_data[BAC_BankMemManager[ModbusOffsetIdx-1].BankOffsetStart]));    
//            ModbusOffset -= (WORD)(ModbusOffsetIdx*2000);            
            ModbusOffset -= (WORD)(ModbusOffsetIdx*1000);            

            pDataToSend += ModbusOffset;        
            for ( TmpCntr=0 ; TmpCntr < ValuesRequired ; TmpCntr++ ){
    	        net_buf[TCP_APP_OFFSET+TcpMsgLen] = 0x00;
    	        TcpMsgLen++;
    	        net_buf[TCP_APP_OFFSET+TcpMsgLen] = pDataToSend[TmpCntr];
    	        TcpMsgLen++;
    	      }
            break;        
        } 
        else 
        {          
            switch ( ModbusOffsetIdx )
            {            
              case MOD_REG_SYS_OFFSET_IDX:
                pDataToSend = (UINT8 *)(&SYS_ConfigParam);  
                ModbusOffset -= (MOD_REG_SYS_OFFSET);          
                break;
                
              case MOD_REG_STA_OFFSET_IDX:
                pDataToSend = (UINT8 *)(&StatisticData);  
                ModbusOffset -= (MOD_REG_STA_OFFSET);
                break;
                
              case MOD_REG_GIO_OFFSET_IDX:
                pDataToSend = (UINT8 *)(&InputOutputData);  
                ModbusOffset -= (MOD_REG_GIO_OFFSET);
                break;

//              case MOD_REG_VAL_OFFSET_IDX:
//              pDataToSend = (UINT8 *)(&(bac_data[BAC_BankMemManager[0].BankOffsetStart]));    
//              ModbusOffset -= (MOD_REG_VAL_OFFSET_BY);
//              break;
               
              default:
                break;          
            }        
        }
        
        pDataToSend += ModbusOffset;   
        
#if DEBUG_AT(2)
		  (void)sprintf((char *)udp_buf,"ModbusTx: IdxOff=%d, Sock:%d, %d,%d,%d,%d\n",
		                ModbusOffsetIdx,
		                CurrentParam,
		                pDataToSend[0],
		                pDataToSend[1],
		                pDataToSend[2],
		                pDataToSend[3]);
      udp_data_to_send = TRUE;
#endif    

        // ABR: cambiar por memcpy..
//        DummyPtr = memcpy (pOutDataMsg, pInverterParam, BytesRequired); 
        for ( TmpCntr=0 ; TmpCntr < BytesRequired ; TmpCntr++ ){
	        net_buf[TCP_APP_OFFSET+TcpMsgLen] = pDataToSend[TmpCntr];
	        TcpMsgLen++;
	      }
        break;
        
        
    case MOD_FC_WR_SIN_REG:                               // Write Single Register
        if ( pModbusInMsg->Data[1] >= MAX_DNP_MODBUS ) return;    // offset en WORDS
             
        pInDataMsg = (UINT8 *)(&(pModbusInMsg->Data[2]));
        
//        pDataToChange = (UINT8 *)(&InverterParam);  
        pDataToChange = (UINT8 *)(&(bac_data[0]));
        pDataToChange += ( 2 * (pModbusInMsg->Data[1]) );        // Offset 

//        MOD_BuildBasicResponse();
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->TransactionID0;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->TransactionID1;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->ProtocolID0;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->ProtocolID1;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = 0x00;	                // LengthHigh
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = 5;                    // LengthLow
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->UI;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->FC;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->Data[0];  // Data[0]
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->Data[1];  // Data[1]
	      TcpMsgLen++;

    		*pDataToChange = *pInDataMsg;    		
//     		ModbusOutMsg.Data[2] =  *pDataToChange;
 	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = *pDataToChange;       // Data[2]
	      TcpMsgLen++;
	      pDataToChange++;
	      pInDataMsg++;
	      
     		*pDataToChange = *pInDataMsg;
//    		ModbusOutMsg.Data[3] =  *pDataToChange;		
 	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = *pDataToChange;       // Data[3]
	      TcpMsgLen++; 
     		
     		MOD_AnalizeValChanges();
        break;
        
  
    case MOD_FC_WR_MUL_REG:										          // Write Multiple Register
        if ( pModbusInMsg->Data[1] >= MAX_DNP_MODBUS ) return;    // offset en WORDS

        ValuesRequired = pModbusInMsg->Data[3];
        if ( ValuesRequired > 100 ) return;		          // Data[3]:0-100 Word Count FC=16
        BytesRequired = pModbusInMsg->Data[4];
        
        pInDataMsg = (UINT8 *)(&(pModbusInMsg->Data[5]));
        
//        pDataToChange = (UINT8 *)(&InverterParam);  
        pDataToChange = (UINT8 *)(&(bac_data[0]));
        pDataToChange += ( 2 * (pModbusInMsg->Data[1]) );        // Offset 
        
        DummyPtr = memcpy (pDataToChange, pInDataMsg, BytesRequired);  
                
//        MOD_BuildBasicResponse();
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->TransactionID0;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->TransactionID1;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->ProtocolID0;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->ProtocolID1;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = 0x00;	                // LengthHigh
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = 6;                    // LengthLow
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->UI;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->FC;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->Data[0];  // Data[0]
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->Data[1];  // Data[1]
	      TcpMsgLen++;
 	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->Data[2];  // Data[2]	 Word Count
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->Data[3];  // Data[3]
	      TcpMsgLen++;
      	
      	MOD_AnalizeValChanges();
        break;
        
        
    default:
//        MOD_BuildBasicResponse();
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->TransactionID0;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->TransactionID1;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->ProtocolID0;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->ProtocolID1;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = 0x00;	                // LengthHigh
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = 3;                    // LengthLow
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = pModbusInMsg->UI;
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = 0x83;                 // FC = error
	      TcpMsgLen++;
	      net_buf[TCP_APP_OFFSET+TcpMsgLen] = 1;                    // Data[0]
	      TcpMsgLen++;
        break;
  }
  
  // Copio en el buffer del socket la respuesta a transmitir, en el primer byte la len
  *((UINT8*)pModbusInMsg) = TcpMsgLen;
  (void)memcpy (&((UINT8*)pModbusInMsg)[1], (UINT8*)(&net_buf[TCP_APP_OFFSET]),TcpMsgLen);
  
//  (void)tcp_send(TcpSockSrv[CurrentParam].TcpSrvTcpSoch, &net_buf[TCP_APP_OFFSET], NETWORK_TX_BUFFER_SIZE - TCP_APP_OFFSET, TcpMsgLen);
  if (tcp_send(TcpSockSrv[CurrentParam].TcpSrvTcpSoch, &net_buf[TCP_APP_OFFSET], NETWORK_TX_BUFFER_SIZE - TCP_APP_OFFSET, TcpMsgLen) != TcpMsgLen)
  { 
    if (TcpSockSrv[CurrentParam].TxRetries) // si quedan reintentos, reintentar luego de un tiempo
    {      
        TcpSockSrv[CurrentParam].TxRetries--;
  	    TcpSockSrv[CurrentParam].TimerId = TMR_SetTimer ( TCP_TIMEOUT_SENDRETRY , PROC_MODBUS, 0x00, FALSE );	// Reintentar
    }
	}
	else
	{
      TcpSockSrv[CurrentParam].TxRetries = 0;
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
/*
  ModbusOutMsg.TransactionID0 = pModbusInMsg->TransactionID0;  
  ModbusOutMsg.TransactionID1 = pModbusInMsg->TransactionID1; 
   
  ModbusOutMsg.ProtocolID0 = pModbusInMsg->ProtocolID0;  
  ModbusOutMsg.ProtocolID1 = pModbusInMsg->ProtocolID1; 
  
  ModbusOutMsg.LengthHigh = 0x00;  
//  ModbusOutMsg.LengthLow = ;  
  ModbusOutMsg.UI = pModbusInMsg->UI;  
  ModbusOutMsg.FC = pModbusInMsg->FC;  
*/
  return;
}


//-----------------------------------------------------------------------------
// Analizar comandos recibidos y cambios en variables 
//-----------------------------------------------------------------------------
//
void MOD_AnalizeValChanges (void)
{
/*
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
*/

}

#pragma CODE_SEG DEFAULT

