#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"
#include "Commdef.h"

#ifdef  MOD_MODBUSTCP_MASTER_ENABLED   // Generar aplicación que utiliza MODBUSTCP_MASTER

#include "pModbusMasterTCP.h"
#include "Global.h"
#include "system.h"
#include "tcp_ip.h"

///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_MODBUS_MASTER_TCP
///////////////////////////////////////////////////////////////////////////////
//
const EVENT_FUNCTION	ModbusMasterTcpProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_ModbusMasterTcpIdleTimeout	    },
{EVT_MODTCP_RXMSG 	  , f_ModbusMasterTcpIdleAnalizeMsg  },
{EVT_MODTCP_TXMSG 	  , f_ModbusMasterTcpIdleQueueMsg    },
{EVT_MODRTU_ERR 	    , f_ModbusMasterTcpIdleErrMsg      },
{EVT_MODTCP_SEND      , f_ModbusMasterTcpIdleSendMsg     },
//{EVT_MODTCP_CLOSE     , f_ModbusMasterTcpIdleClose       },			 // deshabilitar momentanemente el tratamiento infomrado de limpieza de buffers, se hace cuando se detecta el canal caido.
{EVT_OTHER_MSG     		, f_Consume                       }};


///////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_MODBUS_SLAVE_TCP
///////////////////////////////////////////////////////////////////////////////
//
//
//-----------------------------------------------------------------------------
// Limpiar variables, liberar buffers, etc
//-----------------------------------------------------------------------------
//
void f_ModbusMasterTcpIdleClose (void)
{

    UINT8             RxModBusToken;
    
    while ( QUE_GetByteAndQuit (&QTCP_QueueIdx, &RxModBusToken) ) 
    {
         
        (void)MemFreeBuffer (RxModBusToken);   // Liberar el buffer
    
#if DEBUG_AT(DBG_LEVEL_2)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_2)
  {                    
        		  (void)sprintf((char *)udp_buf,"TcpModbus FreeBuffer \n");
              udp_data_to_send = TRUE;
  }
#endif  	
               
    }
    
    //PutEventMsg (EVT_MODTCP_CLOSE, PROC_NEXO, PROC_MODBUS_SLAVE_TCP, 0x00);	 // avisar al proceso

}


//-------------- estado : IDLE ------------------------------------------------
//
//-----------------------------------------------------------------------------
// Analizar mensaje Modbus recibido
//-----------------------------------------------------------------------------
//
void f_ModbusMasterTcpIdleAnalizeMsg (void)
{
  UINT8   BytesRequired;
  UINT8   * pInDataMsg;
  UINT8   * pDataToSend;
  UINT8   * pDataToChange;
  UINT8   ModbusOffsetIdx;
  UINT8   IdxSocket;  
  UINT8   RxModBusTcpToken;
  MODBUS_MSG*	  pModbusInMsg;					            // Mensaje recibido por ModbusTCP.
  UINT8   bErrMsgLen;
  
  RxModBusTcpToken = (UINT8)CurrentMsg.Param;
   
  // Si se cerró el socket en el interin, cancelar transaccion
  //if (RxModBusTcpToken == 0xFF ) return;
  
  if (MemGetBufferPtr( RxModBusTcpToken , &((void*)pModbusInMsg) ) != OK ){    
      MON_ResetIfNotConnected(0XA2);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;   
  }

  if ( MemGetRouteMark(RxModBusTcpToken, &IdxSocket ) != OK)   // Marca de origen
  {
      MON_ResetIfNotConnected(0xA0);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;   // ABR_: (SW_ERR) analizar acciones a tomar         
  }
  
  if (IdxSocket >= MAX_SRV_TCPSOCH){
      MON_ResetIfNotConnected(0xA1);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;   // ABR_: (SW_ERR) analizar acciones a tomar     
  }

  //SETBIT(dwIntegrityMonitor,PROC_MODBUS_SLAVE_TCP);
  
  
#ifdef MODBUSTCP_BUSY_SUPPORT
  if (MemGetFreeBuffQtty() <= 2) 
  {  
  
      bModbusBusyCounter++;
      if (bModbusBusyCounter >= MODBUSTCP_BUSY_MAXQTTY){
      
#if DEBUG_AT(DBG_LEVEL_2)
        if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_2)
        {                    
              	  (void)sprintf((char *)udp_buf,">> MODBUSTCP MAX_BUSY RESET !!! \n");
                  udp_data_to_send = TRUE;
        }
#endif  	
        MON_ResetIfNotConnected(0xA9);    // Error , Reset (TODO: revisar acciones a tomar)
        //return;
      
      } 
      else
      {
#if DEBUG_AT(DBG_LEVEL_2)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_2)
  {                    
        	  (void)sprintf((char *)udp_buf,">> MODBUSTCP SLAVE_BUSY !! \n");
            udp_data_to_send = TRUE;
  }
#endif       
      }

	      pModbusInMsg->LengthLow = 0x03;       // LengthLow
	      pModbusInMsg->FC        = 0x83;       // FC = exception
	      pModbusInMsg->Data[0]   = 0x06;       // Slave Busy retry later
	      
	      bErrMsgLen = 9;
        if ( MemSetUsedLen(RxModBusTcpToken, bErrMsgLen) != OK)        // Bytes recibidos
        {  
#if DEBUG_AT(DBG_LEVEL_2)
            if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_2)
            {                    
                  	  (void)sprintf((char *)udp_buf,"MBTCP Err:0x02 \n");
                      udp_data_to_send = TRUE;
            }
#endif  	
            return;
          
        }
        	      
        PutEventMsg (EVT_MODTCP_TXMSG, PROC_MODBUS_SLAVE_TCP, PROC_MODBUS_SLAVE_TCP, RxModBusTcpToken);
        return;    
  } 
  else
  {
    bModbusBusyCounter = 0;
  }
#endif 
   

#if DEBUG_AT(DBG_LEVEL_4)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_4)
  {                    
  		  (void)sprintf((char *)udp_buf,"ModbusTCP_Rx FC: %d, IP:%d \n",
  		                ((MODBUS_MSG*)pModbusInMsg)->FC,
  		                (BYTE)TcpSockSrv[IdxSocket].SlaveIpAddress);
        udp_data_to_send = TRUE;
  }
#endif    
          
  switch(pModbusInMsg->FC)
  {
    case MOD_FC_RD_MUL_REG:										                    // Read Multiple Register
        ValuesRequired = pModbusInMsg->Data[3];
        if ( ValuesRequired > MAX_MODBUS_REG ) return;		        // Data[3]:0-125 Word Count FC=3
        BytesRequired = ( 2 * ValuesRequired );
        
        if (pModbusInMsg->UI == ModbusTcpLocalNode ) 
        {
            GIO_GetAAGInp();
            GIO_GetTemp();
            (void)GIO_GetInpVal();
            MOD_MirrorInputOutput[0] = (UINT16)(InputOutputData.DigInpVal & 0x01)?1:0;
            MOD_MirrorInputOutput[1] = (UINT16)(InputOutputData.DigInpVal & 0x02)?1:0;
            MOD_MirrorInputOutput[2] = (UINT16)(InputOutputData.DigInpVal & 0x04)?1:0;
            MOD_MirrorInputOutput[3] = (UINT16)(InputOutputData.DigInpVal & 0x08)?1:0;
            MOD_MirrorInputOutput[4] = (UINT16)(InputOutputData.DigInpVal & 0x10)?1:0;
            MOD_MirrorInputOutput[5] = (UINT16)(InputOutputData.DigInpVal & 0x20)?1:0;
        
            //pDataToSend = (UINT8 *)(&InputOutputData);  							// Mapeo en 40000
            pDataToSend = (UINT8 *)(&(MOD_MirrorInputOutput[0]));  							// Mapeo en 40000
            ModbusOffset   = *((UINT16*)&(pModbusInMsg->Data[0]));        
            ModbusOffsetIdx = (UINT8)(ModbusOffset / 100);
            
#ifdef MOD_MODBUSRTU_AUTOQUERY_ENABLED		
            if (ModbusOffsetIdx >= 10) 
            {
                ModbusOffsetIdx /=  10;
                if (ModbusOffsetIdx > MAX_AUTOQUERY_BANKS) return;   // Direccion fuera de rango..
                pDataToSend = (UINT8 *)(&(autoQueryModbusData[MAQ_BankMemManager[ModbusOffsetIdx-1].BankOffsetStart]));    
                ModbusOffset -= (WORD)(ModbusOffsetIdx*1000);            
            } 
            else            
#endif
#ifdef MOD_SYMEO_ENABLED	//	SYM_BankMemManager
            if (ModbusOffsetIdx >= 10) 
            {
                ModbusOffsetIdx /=  10;
                if (ModbusOffsetIdx > MAX_AUTOQUERY_BANKS) return;   // Direccion fuera de rango..
                pDataToSend = (UINT8 *)(&(SYM_BankMemManager[ModbusOffsetIdx-1]));    
                ModbusOffset -= (WORD)(ModbusOffsetIdx*1000);            
            } 
            else            
#endif
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
                   
                 default:
                    break;          
                }
                ModbusOffset   *= 2;
                pDataToSend += ModbusOffset; 
                
            }

            DummyPtr = memcpy( (UINT8*)(&(pModbusInMsg->Data[1])), pDataToSend, BytesRequired); 
            pModbusInMsg->LengthLow = 3 + BytesRequired;              // LengthLow
            pModbusInMsg->Data[0]   = BytesRequired;                  //BytesRequired  
           
            TcpSockSrv[IdxSocket].MsgLen = 9 + BytesRequired ;
            
            // Despues MsgSend le quita do (CRC) y le suma 6 (Overhead TCP). -> 6+3+BytesRequired+CRC -> 3+CRC=5
            if ( MemSetUsedLen(RxModBusTcpToken, 9 + BytesRequired) != OK){
            
#if DEBUG_AT(DBG_LEVEL_2)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_2)
  {                    
          		  (void)sprintf((char *)udp_buf,"ModbusMasterTcp Err:0xA1 \n");
                udp_data_to_send = TRUE;
  }
 #endif  	
               //MON_ResetIfNotConnected(0XA1);    // Error de SW, Reset (TODO: revisar acciones a tomar)
                return;   
            } 
            else{
                        
              PutEventMsg (EVT_MODTCP_TXMSG, PROC_MODBUS_SLAVE_TCP, PROC_MODBUS_SLAVE_TCP, RxModBusTcpToken);
            }
          
            
        } 
        else 
        {
            PutEventMsg (EVT_MODTCP_RD_MUL_REG, PROC_NEXO, PROC_MODBUS_SLAVE_TCP, RxModBusTcpToken);          
        }
        break;
        
        
    case MOD_FC_WR_SIN_REG:                                       // Write Single Register
//        if ( pModbusInMsg->Data[1] >= MAX_MODBUS_REG ) return;    // offset en WORDS

        if (pModbusInMsg->UI == ModbusTcpLocalNode ) 
        {
            pInDataMsg = (UINT8 *)(&(pModbusInMsg->Data[2]));
            pDataToChange = (UINT8 *)(&(MOD_MirrorInputOutput[0]));
            pDataToChange += ( 2 * (pModbusInMsg->Data[1]) );        // Offset 

            pModbusInMsg->LengthLow = 8;                              // LengthLow
            // Data 0,1,2 y 3 no cambian mantienen el offset y el valor a escribir
        		*pDataToChange = *pInDataMsg;    		
    	      pDataToChange++;
    	      pInDataMsg++;
         		*pDataToChange = *pInDataMsg;
         		
            InputOutputData.DigOutVal = 0x00;
            if (MOD_MirrorInputOutput[0])  InputOutputData.DigOutVal |= 0x01;
            if (MOD_MirrorInputOutput[1])  InputOutputData.DigOutVal |= 0x02;
            if (MOD_MirrorInputOutput[2])  InputOutputData.DigOutVal |= 0x04;
            if (MOD_MirrorInputOutput[3])  InputOutputData.DigOutVal |= 0x08;
            if (MOD_MirrorInputOutput[4])  InputOutputData.DigOutVal |= 0x10;
            if (MOD_MirrorInputOutput[5])  InputOutputData.DigOutVal |= 0x20;
            GIO_SetOutVal(InputOutputData.DigOutVal);
         		
            TcpSockSrv[IdxSocket].MsgLen = 12 ;
            (void) MemSetUsedLen(RxModBusTcpToken, 8);

            PutEventMsg (EVT_MODTCP_TXMSG, PROC_MODBUS_SLAVE_TCP, PROC_MODBUS_SLAVE_TCP, RxModBusTcpToken);
        } 
        else 
        {
            PutEventMsg (EVT_MODTCP_WR_SIN_REG, PROC_NEXO, PROC_MODBUS_SLAVE_TCP, RxModBusTcpToken);          
        }
        break;
        
  
    case MOD_FC_WR_MUL_REG:										          // Write Multiple Register
//        if ( pModbusInMsg->Data[1] >= MAX_MODBUS_REG ) return;    // offset en WORDS

        if (pModbusInMsg->UI == ModbusTcpLocalNode ) 
        {
            ValuesRequired = pModbusInMsg->Data[3];
            if ( ValuesRequired > 100 ) return;		          // Data[3]:0-100 Word Count FC=16
            BytesRequired = pModbusInMsg->Data[4];
            
            pInDataMsg = (UINT8 *)(&(pModbusInMsg->Data[5]));
            
            pDataToChange = (UINT8 *)(&(MOD_MirrorInputOutput[0]));
            pDataToChange += ( 2 * (pModbusInMsg->Data[1]) );        // Offset 

            DummyPtr = memcpy (pDataToChange, pInDataMsg, BytesRequired);  
            
            //InputOutputData.DigOutVal &= ~0x20;
            //if (MOD_MirrorInputOutput[1])  InputOutputData.DigOutVal |= 0x20;

            InputOutputData.DigOutVal = 0x00;
            if (MOD_MirrorInputOutput[0])  InputOutputData.DigOutVal |= 0x01;
            if (MOD_MirrorInputOutput[1])  InputOutputData.DigOutVal |= 0x02;
            if (MOD_MirrorInputOutput[2])  InputOutputData.DigOutVal |= 0x04;
            if (MOD_MirrorInputOutput[3])  InputOutputData.DigOutVal |= 0x08;
            if (MOD_MirrorInputOutput[4])  InputOutputData.DigOutVal |= 0x10;
            if (MOD_MirrorInputOutput[5])  InputOutputData.DigOutVal |= 0x20;
            GIO_SetOutVal(InputOutputData.DigOutVal);
            
            pModbusInMsg->LengthLow = 8;                              // LengthLow
            // Data 0,1,2 y 3 no cambian mantienen el offset y el valor a escribir
            TcpSockSrv[IdxSocket].MsgLen = 12;
            (void) MemSetUsedLen(RxModBusTcpToken, 8);            
            PutEventMsg (EVT_MODTCP_TXMSG, PROC_MODBUS_SLAVE_TCP, PROC_MODBUS_SLAVE_TCP, RxModBusTcpToken);
        } 
        else 
        {
            PutEventMsg (EVT_MODTCP_WR_MUL_REG, PROC_NEXO, PROC_MODBUS_SLAVE_TCP, RxModBusTcpToken);          
        }
        break;
        
    case MOD_FC_READ_COILS:    
    case MOD_FC_WRITE_COIL:
    case MOD_FC_RD_INP_STA:
    case MOD_FC_RD_INP_REG:
    case MOD_FC_USER_DEF:    
        PutEventMsg (EVT_MODTCP_WR_MUL_REG, PROC_NEXO, PROC_MODBUS_SLAVE_TCP, RxModBusTcpToken);          
        break;
       
    default:
    
	      pModbusInMsg->LengthLow = 0x03;       // LengthLow
	      pModbusInMsg->FC        = 0x83;       // FC = exception
	      pModbusInMsg->Data[0]   = 0x01;       // Data[0]
	      
	      bErrMsgLen = 9;
        if ( MemSetUsedLen(RxModBusTcpToken, bErrMsgLen) != OK)        // Bytes recibidos
        {  
#if DEBUG_AT(DBG_LEVEL_2)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_2)
  {                    
        	  (void)sprintf((char *)udp_buf,"MBTCP Err:0x02 \n");
            udp_data_to_send = TRUE;
  }
#endif  	
          return;
          
        }	      
	      //TcpSockSrv[IdxSocket].MsgLen  = 9;
        PutEventMsg (EVT_MODTCP_TXMSG, PROC_MODBUS_SLAVE_TCP, PROC_MODBUS_SLAVE_TCP, RxModBusTcpToken);
        
        break;
  }

 	return;
}
		

//----  -------------------------------------------------------------------------
// Timeout en intento de transmisión de respuesta
//-----------------------------------------------------------------------------
//
void f_ModbusMasterTcpIdleTimeout (void) 
{
//  UINT8 TmpIdx;
//  UINT8 CurrentTimerId;

//  CurrentTimerId = (UINT8)CurrentMsg.Param;

  // Buscar con el id del timer a qué socket modbus tcp pertenece el timeout
//  TmpIdx=0; 
//  while (CurrentTimerId != TcpSockSrv[TmpIdx].TimerId ){   
//    TmpIdx++;
//    if ( TmpIdx >= MAX_SRV_TCPSOCH ){
//        MON_ResetIfNotConnected(0XA3);    // Error de SW, Reset (TODO: revisar acciones a tomar)
//        return;
//    }
//  }

//  TcpSockSrv[TmpIdx].TimerId = 0xFF;
//  PutEventMsg (EVT_MODTCP_TXMSG, PROC_MODBUS_SLAVE_TCP, PROC_MODBUS_SLAVE_TCP, TcpSockSrv[TmpIdx].BufferToken);
  
  
  RetryTimerId = 0xFF;
  
  // Enviar siguiente mensaje, si es que hay alguno encolado
  //while ( MOD_SendNextQueuedMsg() != OK);        
  (void) MOD_SendNextQueuedMsg();        
  
	
	return;
}


//-----------------------------------------------------------------------------
// Encolar mensaje Modbus recibido y transmitirlo si es pertinente
//-----------------------------------------------------------------------------
//
void f_ModbusMasterTcpIdleQueueMsg (void) 
{
    UINT8   IdxSocket;  
    UINT8   RxModBusTcpToken;
    UINT8*	pModbusInMsg;					                  // Mensaje recibido por ModbusTCP.
    BYTE    QueueCounter;
    
    //SETBIT(dwIntegrityMonitor,PROC_MODBUS_SLAVE_TCP);  
    ProcDebugFlag[PROC_MODBUS_SLAVE_TCP] &= ~DBGFLAG_ACTIVITY_SIGN_MASK;
    ProcDebugFlag[PROC_MODBUS_SLAVE_TCP] |= DBGFLAG_ACTIVITY_SIGN;
    
    
    RxModBusTcpToken = (UINT8)CurrentMsg.Param;
     
    // Si se cerró el socket en el interin, cancelar transaccion
    //if (RxModBusTcpToken == 0xFF ) return;
    
    if (MemGetBufferPtr( RxModBusTcpToken , &((void*)pModbusInMsg) ) != OK ){    
        MON_ResetIfNotConnected(0XA4);    // Error de SW, Reset (TODO: revisar acciones a tomar)
        return;   
    }

    if ( MemGetRouteMark(RxModBusTcpToken, &IdxSocket ) != OK)   // Marca de origen
    {
        MON_ResetIfNotConnected(0xA5);    // Error de SW, Reset (TODO: revisar acciones a tomar)
        return;   // ABR_: (SW_ERR) analizar acciones a tomar         
    }
    
    if (IdxSocket >= MAX_SRV_TCPSOCH){
        MON_ResetIfNotConnected(0xA6);    // Error de SW, Reset (TODO: revisar acciones a tomar)
        return;   // ABR_: (SW_ERR) analizar acciones a tomar     
    }


  //--------------------------------------------

      // Encolar token del msg.
      QueueCounter = QUE_PutByte (&QTCP_QueueIdx, RxModBusTcpToken);
  		// Si no hay lugar en la cola resetear. ( Hay más espacio que buffers ) 
      if (!QueueCounter) {
          MON_ResetIfNotConnected(0XCC);    // Error de SW, Reset (TODO: revisar acciones a tomar)
          return;       
      }   
  		// Si es el primero en la cola, enviar ahora
      if ( QueueCounter == 0x01 )
      {    
          RetryAttemps = TCP_MAX_TXRETRY;      // para el proximo envio          
          PutEventMsg (EVT_MODTCP_SEND, PROC_MODBUS_SLAVE_TCP, PROC_MODBUS_SLAVE_TCP, RxModBusTcpToken);
     }
        
      return;


}


//-----------------------------------------------------------------------------
// Transmitir o Retransmitir mensaje Modbus recibido
//-----------------------------------------------------------------------------
//
void f_ModbusMasterTcpIdleSendMsg (void)
{

  UINT8   IdxSocket;  
  UINT16  TcpMsgLen; 
  UINT8   RxModBusTcpToken;
  UINT8*	pModbusInMsg;					                  // Mensaje recibido por ModbusTCP.
  UINT8   OutTmpByte;
  
 
  
  RxModBusTcpToken = (UINT8)CurrentMsg.Param;
   
  // Si se cerró el socket en el interin, cancelar transaccion
  //if (RxModBusTcpToken == 0xFF ) return;
  
  if (MemGetBufferPtr( RxModBusTcpToken , &((void*)pModbusInMsg) ) != OK ){    
      MON_ResetIfNotConnected(0XA4);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      // Desencolar mensaje con error
      (void)QUE_GetByteAndQuit (&QTCP_QueueIdx, &OutTmpByte);
      return;   
  }

  if ( MemGetRouteMark(RxModBusTcpToken, &IdxSocket ) != OK)   // Marca de origen
  {
      MON_ResetIfNotConnected(0xA5);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      // Desencolar mensaje con error
      (void)QUE_GetByteAndQuit (&QTCP_QueueIdx, &OutTmpByte);
      return;   // ABR_: (SW_ERR) analizar acciones a tomar         
  }
  
  if (IdxSocket >= MAX_SRV_TCPSOCH){
      MON_ResetIfNotConnected(0xA6);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      // Desencolar mensaje con error
      (void)QUE_GetByteAndQuit (&QTCP_QueueIdx, &OutTmpByte);
      return;   // ABR_: (SW_ERR) analizar acciones a tomar     
  }


  // Si se cerró la conexion TCP descartar el buffer en curso 
  if ( TcpSockSrv[IdxSocket].status != STATUS_ACTIVE)   
  {
  	  // Descartar respuesta y procesar la proxima encuesta encolada   	 
#if DEBUG_AT(DBG_LEVEL_2)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_2)
  {                    
  		  (void)sprintf((char *)udp_buf,"ModbusTcpSend TCP_CLOSED:0xAA \n");
        udp_data_to_send = TRUE;
  }
#endif  	
      // Desencolar mensaje enviado, cuya respuesta se está procesando
      (void)QUE_GetByteAndQuit (&QTCP_QueueIdx, &OutTmpByte);        
      (void)MemFreeBuffer (RxModBusTcpToken);   // Liberar el buffer
      // Enviar siguiente mensaje, si es que hay alguno encolado
      //while ( MOD_SendNextQueuedMsg() != OK);        
      (void) MOD_SendNextQueuedMsg();        

                          
      return;            
  }


   // Verificar si es posible enviar datos (puede haber un msg. previo aun sin ACK)
    if (tcp_checksend(TcpSockSrv[IdxSocket].TcpSrvTcpSoch) < 0){
      
      // Intentar Luego.
      if (RetryAttemps > 0) // si quedan reintentos, reintentar luego de un tiempo
      {      
          RetryAttemps--;
    	    RetryTimerId = TMR_SetTimer ( TCP_TIMEOUT_SENDRETRY , PROC_MODBUS_SLAVE_TCP, 0x00, FALSE );	// Reintentar
      }
      else  // descartar el envío de respuesta
      {
      
#if DEBUG_AT(DBG_LEVEL_2)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_2)
  {                    
          		  (void)sprintf((char *)udp_buf,"TCP_Trash TrID: %d, IP:%d \n",
          		                ((MODBUS_MSG*)pModbusInMsg)->TransactionID1,//len,
          		                (BYTE)TcpSockSrv[IdxSocket].SlaveIpAddress);
                udp_data_to_send = TRUE;
  }
#endif    
          // Desencolar mensaje enviado, cuya respuesta se está procesando
          (void)QUE_GetByteAndQuit (&QTCP_QueueIdx, &OutTmpByte);
          
          (void)MemFreeBuffer (RxModBusTcpToken);   // Liberar el buffer
          TcpSockSrv[IdxSocket].BuffersInFlight--;
  #ifdef AUTO_MTU
          if ( TcpSockSrv[IdxSocket].BuffersInFlight < MODTCP_QUERIES_ALLOWED )
            (void)tcp_changemtu (TcpSockSrv[IdxSocket].TcpSrvTcpSoch, (UINT16)TCP_MIN_MTU);      
  #endif          
          // Enviar siguiente mensaje, si es que hay alguno encolado
          //while ( MOD_SendNextQueuedMsg() != OK);        
          (void) MOD_SendNextQueuedMsg();        

          RetryAttemps = TCP_MAX_TXRETRY;      // para el proximo envio                         
      }
      return;
      
    }

  // recupero el mensaje, del buffer asociado al socket
  // TcpMsgLen = (UINT8)TcpSockSrv[IdxSocket].MsgLen;


    if ( MemGetUsedLen(RxModBusTcpToken, &TcpMsgLen ) != OK)   // Bytes recibidos
    {
    	  // Se liberó el buffer en otro proceso, descartar respuesta y procesar la proxima encuesta encolada   	 
#if DEBUG_AT(DBG_LEVEL_2)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_2)
  {                    
    		  (void)sprintf((char *)udp_buf,"NexoModbus Err:0xA8 \n");
          udp_data_to_send = TRUE;
  }
#endif
        
        (void)QUE_GetByteAndQuit (&QTCP_QueueIdx, &OutTmpByte);        
        (void)MemFreeBuffer (RxModBusTcpToken);   // Liberar el buffer
        TcpSockSrv[IdxSocket].BuffersInFlight--;
        // Enviar siguiente mensaje, si es que hay alguno encolado
        //while ( MOD_SendNextQueuedMsg() != OK);        
        (void) MOD_SendNextQueuedMsg();        
        

#ifdef AUTO_MTU
        if ( TcpSockSrv[IdxSocket].BuffersInFlight < MODTCP_QUERIES_ALLOWED )
          (void)tcp_changemtu (TcpSockSrv[IdxSocket].TcpSrvTcpSoch, (UINT16)TCP_MIN_MTU);      
#endif        
        return;            
    }
   
    
    //TcpMsgLen -=2; // -2 del CRC_RTU sin CRC

    //((MODBUS_MSG*)pModbusInMsg)->LengthHigh = (UINT8)((TcpMsgLen & 0xFF00 )>>8);
    //((MODBUS_MSG*)pModbusInMsg)->LengthLow =  (UINT8)( TcpMsgLen & 0x00FF );
  
    // los bytes de CRC no se envían en ModbusTCP
    //TcpMsgLen +=  6; //+6 del overhead TCP;
 
    // ABR: 2012-02-22: analizar luego por que la aplicacion no reconoce respuestas con len < 12
    //if (TcpMsgLen < 12)
    //  TcpMsgLen=12;
  
  
  (void)memcpy ((UINT8*)(&net_buf[TCP_APP_OFFSET]), &((UINT8*)pModbusInMsg)[0], TcpMsgLen);
  
  //for (bIdxCntr=0 ; bIdxCntr<TcpMsgLen ; bIdxCntr++)
  //  net_buf[TCP_APP_OFFSET+bIdxCntr] = ((UINT8*)pModbusInMsg)[bIdxCntr];
    
  
  if (tcp_send(TcpSockSrv[IdxSocket].TcpSrvTcpSoch, &net_buf[TCP_APP_OFFSET], NETWORK_TX_BUFFER_SIZE - TCP_APP_OFFSET, TcpMsgLen) != TcpMsgLen)
  { 
    if (RetryAttemps > 0) // si quedan reintentos, reintentar luego de un tiempo
    {      
        RetryAttemps--;
  	    RetryTimerId = TMR_SetTimer ( TCP_TIMEOUT_SENDRETRY , PROC_MODBUS_SLAVE_TCP, 0x00, FALSE );	// Reintentar
#if DEBUG_AT(DBG_LEVEL_2)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_2)
  {                    
        		  (void)sprintf((char *)udp_buf,"TCP_Retry TrID: %d, IP:%d \n",
        		                ((MODBUS_MSG*)pModbusInMsg)->TransactionID1,//len,
        		                (BYTE)TcpSockSrv[IdxSocket].SlaveIpAddress);
              udp_data_to_send = TRUE;
  }
#endif    
  	    
    } 
    else  // descartar el envío de respuesta
    {
    
#if DEBUG_AT(DBG_LEVEL_2)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_2)
  {                    
        		  (void)sprintf((char *)udp_buf,"TCP_Trash TrID: %d, IP:%d \n",
        		                ((MODBUS_MSG*)pModbusInMsg)->TransactionID1,//len,
        		                (BYTE)TcpSockSrv[IdxSocket].SlaveIpAddress);
              udp_data_to_send = TRUE;
  }
#endif    
        // Desencolar mensaje enviado, cuya respuesta se está procesando
        (void)QUE_GetByteAndQuit (&QTCP_QueueIdx, &OutTmpByte);
        
        (void)MemFreeBuffer (RxModBusTcpToken);   // Liberar el buffer
        TcpSockSrv[IdxSocket].BuffersInFlight--;
#ifdef AUTO_MTU
        if ( TcpSockSrv[IdxSocket].BuffersInFlight < MODTCP_QUERIES_ALLOWED )
          (void)tcp_changemtu (TcpSockSrv[IdxSocket].TcpSrvTcpSoch, (UINT16)TCP_MIN_MTU);      
#endif  
        
        // Enviar siguiente mensaje, si es que hay alguno encolado
        //while ( MOD_SendNextQueuedMsg() != OK);        
        (void) MOD_SendNextQueuedMsg();        
        RetryAttemps = TCP_MAX_TXRETRY;      // para el proximo envio                         
    }
	}
	else    // respuesta enviada OK
	{
#if DEBUG_AT(DBG_LEVEL_4)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_4)
  {                    
        		  (void)sprintf((char *)udp_buf,"TCP_Sent TrID: %d, IP:%d \n",
        		                ((MODBUS_MSG*)pModbusInMsg)->TransactionID1,//len,
        		                (BYTE)TcpSockSrv[IdxSocket].SlaveIpAddress);
              udp_data_to_send = TRUE;
  }
#endif    
                // Desencolar mensaje enviado, cuya respuesta se está procesando
        (void)QUE_GetByteAndQuit (&QTCP_QueueIdx, &OutTmpByte);

        (void)MemFreeBuffer (RxModBusTcpToken);   // Liberar el buffer
        TcpSockSrv[IdxSocket].BuffersInFlight--;
#ifdef AUTO_MTU
        if ( TcpSockSrv[IdxSocket].BuffersInFlight < MODTCP_QUERIES_ALLOWED )
          (void)tcp_changemtu (TcpSockSrv[IdxSocket].TcpSrvTcpSoch, (UINT16)TCP_MIN_MTU);      
#endif   
        // Enviar siguiente mensaje, si es que hay alguno encolado
        //while ( MOD_SendNextQueuedMsg() != OK);
        (void) MOD_SendNextQueuedMsg();        
        RetryAttemps = TCP_MAX_TXRETRY;      // para el proximo envio                         

	}
 	return;
}

	
RETCODE MOD_SendNextQueuedMsg(void)
{
    UINT8             RxModBusToken;
    
    if ( QUE_GetByte (&QTCP_QueueIdx, &RxModBusToken) ) 
    {
    
        // ABR_TODO: implementar mecanismo (return ERROR), para seguir depachando mensajes encolados
        // si se perdio el buffer a transmitir
        if (RxModBusToken == 0xFF ) 
        {          
#if DEBUG_AT(DBG_LEVEL_2)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_2)
  {                    
        		  (void)sprintf((char *)udp_buf,"TcpModbus Err:0xC7 \n");
              udp_data_to_send = TRUE;
  }
#endif  	
            MON_ResetIfNotConnected(0XC3);    // Error de SW, Reset (TODO: revisar acciones a tomar)
            (void)QUE_GetByteAndQuit (&QTCP_QueueIdx, &RxModBusToken);
            return ERROR; 
        }

        PutEventMsg (EVT_MODTCP_SEND, PROC_MODBUS_SLAVE_TCP, PROC_MODBUS_SLAVE_TCP, RxModBusToken);   
               
    }
    
    return OK;      
}
		

//-----------------------------------------------------------------------------
// Error en mensaje convertido a RTU descartar buffer TCP
//-----------------------------------------------------------------------------
//
void f_ModbusMasterTcpIdleErrMsg (void)
{

  UINT8   IdxSocket;  
  UINT8   RxModBusTcpToken;
  UINT8   OutTmpByte;
  
 
  RxModBusTcpToken = (UINT8)CurrentMsg.Param;
   
  // Si se cerró el socket en el interin, cancelar transaccion
  //if (RxModBusTcpToken == 0xFF ) return;
  

  if ( MemGetRouteMark(RxModBusTcpToken, &IdxSocket ) != OK)   // Marca de origen
  {
      MON_ResetIfNotConnected(0xA5);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;   // ABR_: (SW_ERR) analizar acciones a tomar         
  }
  
  if (IdxSocket >= MAX_SRV_TCPSOCH){
      MON_ResetIfNotConnected(0xA6);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;   // ABR_: (SW_ERR) analizar acciones a tomar     
  }
  

         // Desencolar mensaje enviado, cuya respuesta se está procesando
        (void)QUE_GetByteAndQuit (&QTCP_QueueIdx, &OutTmpByte);

        (void)MemFreeBuffer (RxModBusTcpToken);   // Liberar el buffer
        TcpSockSrv[IdxSocket].BuffersInFlight--;
#ifdef AUTO_MTU
        if ( TcpSockSrv[IdxSocket].BuffersInFlight < MODTCP_QUERIES_ALLOWED )
          (void)tcp_changemtu (TcpSockSrv[IdxSocket].TcpSrvTcpSoch, (UINT16)TCP_MIN_MTU);      
#endif   
        // Enviar siguiente mensaje, si es que hay alguno encolado
        //while ( MOD_SendNextQueuedMsg() != OK);
        (void) MOD_SendNextQueuedMsg();        
        RetryAttemps = TCP_MAX_TXRETRY;      // para el proximo envio                         
  

#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
      		  (void)sprintf((char *)udp_buf,"pMbTCP_SlaveCleanBuff InfFlight: %d, Idx: %d\n", 
      		      TcpSockSrv[IdxSocket].BuffersInFlight,
      		      IdxSocket);      		      
            udp_data_to_send = TRUE;
  }
#endif  	
     
  
  //ABR_TODO: enviar excepcion ??

        
}


///////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// MOD_ModbusMasterTcpInit : inicializacion de variables del proceso
//-----------------------------------------------------------------------------
//
void MOD_ModbusMasterTcpInit(void)
{
  	eeprom_read_buffer(EE_ADDR_NODE_SCIA, &ModbusTcpLocalNode ,1); 
  	
    QTCP_QueueIdx.qData = QTCP_MsgQueue;
    QUE_Init(&QTCP_QueueIdx);

    ProcDebugFlag[PROC_MODBUS_SLAVE_TCP] = 0x00;
    ProcDebugFlag[PROC_MODBUS_SLAVE_TCP] |= DBGFLAG_ACTIVITY_CHECK;

    RetryTimerId = 0;
    RetryAttemps = 0;  

    bModbusBusyCounter = 0;

	  return;
}

#pragma CODE_SEG DEFAULT

#endif		//  MOD_MODBUSTCP_MASTER_ENABLED   Generar aplicación que utiliza MODBUSTCP_MASTER
