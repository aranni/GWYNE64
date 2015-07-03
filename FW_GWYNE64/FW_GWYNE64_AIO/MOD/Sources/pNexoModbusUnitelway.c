#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"
#include "Commdef.h"

#if  (FW_APP_BUILT == FW_APP_UNITELWAY)   // Generar aplicación para protocolos MODBUSTCP a MODBUSRTU

#include "pNexoModbusUnitelway.h"
#include "Global.h"
#include "system.h"
#include "tcp_ip.h"

///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_NEXO
///////////////////////////////////////////////////////////////////////////////
//
const EVENT_FUNCTION	NexusProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	      , f_NexusIdleTimeout      },
{EVT_MODRTU_MSG 	      , f_NexusModbusRtuAny     },
{EVT_MODTCP_RD_MUL_REG 	, f_NexusModbusTcpAny     },
{EVT_MODTCP_WR_SIN_REG 	, f_NexusModbusTcpAny     },
{EVT_MODTCP_WR_MUL_REG 	, f_NexusModbusTcpAny     },
{EVT_SCI_RXMSG 	        , f_NexusModbusRtuMsg     },
{EVT_SCI_RXERR 	        , f_NexusModbusRtuErr     },
{EVT_SCI_TXERR 	        , f_NexusModbusRtuErr     },
{EVT_SCI_TXBUSY         , f_NexusModbusRtuErr     },
{EVT_OTHER_MSG     		  , f_Consume               }};



///////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_NEXO
///////////////////////////////////////////////////////////////////////////////
//
//-------------- estado : IDLE ----------------------------------------------------------------
//
//-----------------------------------------------------------------------------
// Reintentar envio de mensaje modbus
//-----------------------------------------------------------------------------
//
void f_NexusIdleTimeout (void)
{

	  return;
}

//-----------------------------------------------------------------------------
// Procesar respuesta recibida en ModbusRTU MASTER
//-----------------------------------------------------------------------------
//
void f_NexusModbusRtuMsg (void)
{
    UINT8   CurrentRxToken; 
    UINT8   TmpIdx;
    UINT16  CurrentRxModbusRtuLen; 
    UINT8   RouteMark;
    UINT8   OutTmpByte;

    CurrentRxToken = (UINT8)CurrentMsg.Param;
    
    SETBIT(dwIntegrityMonitor,PROC_NEXO);
    
    if ( MemGetRouteMark(CurrentRxToken, &RouteMark) != OK)   // Marca de origen
    {
      	  // Se liberó el buffer en otro proceso, descartar respuesta y procesar la proxima encuesta encolada
#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
      		  (void)sprintf((char *)udp_buf,"NexoModbus Err:0xC2 \n");
            udp_data_to_send = TRUE;
  }
#endif  	
          MON_ResetIfNotConnected(0XC2);    // Error de SW, Reset (TODO: revisar acciones a tomar)
          //return;          
    }
    else
    {
                
          if (RouteMark == PROC_MODBUS_SLAVE_RTU)   // Si era un mensaje de Modbus RTU Slave
          {
              PutEventMsg (EVT_MODRTU_TXMSG, PROC_MODBUS_SLAVE_RTU, PROC_NEXO, CurrentRxToken);      
          } 
          else
          {   
              // los bytes de CRC no se envían en ModbusTCP
              //TcpSockSrv[TmpIdx].MsgLen =  CurrentRxModbusRtuLen+4; //+6 del overhead TCP; -2 del CRC_RTU
              PutEventMsg (EVT_MODTCP_TXMSG, PROC_MODBUS_SLAVE_TCP, PROC_NEXO, CurrentRxToken);
          }          
    }
    
    // Desencolar mensaje enviado, cuya respuesta se está procesando
    (void)QUE_GetByteAndQuit (&OutTmpByte);
    
    // Enviar siguiente mensaje, si es que hay alguno encolado
    while ( NEX_SendNextQueuedMsg() != OK);
    
	  return;
}

void f_NexusModbusRtuErr (void)
{
    UINT8   CurrentRxToken; 
    UINT8   RouteMark;
    UINT8   OutTmpByte;
//    UINT8*	pModbusInMsg;					                  // Mensaje recibido por ModbusTCP.

    CurrentRxToken = (UINT8)CurrentMsg.Param;

    if ( MemGetRouteMark(CurrentRxToken, &RouteMark) == OK)   // Marca de origen
    {  
        if (RouteMark == PROC_MODBUS_SLAVE_RTU)   // Si era un mensaje de Modbus RTU Slave
        {
            PutEventMsg (EVT_MODRTU_ERR, PROC_MODBUS_SLAVE_RTU, PROC_NEXO, CurrentRxToken);      
        } else 
        {
            PutEventMsg (EVT_MODRTU_ERR, PROC_MODBUS_SLAVE_TCP, PROC_NEXO, CurrentRxToken);               
        }
          
    } 
    //else			 // se perdio el buffer      
      //MON_ResetIfNotConnected(0XC3);    // Error de SW, Reset (TODO: revisar acciones a tomar)
   
    
    // Si hubo error en el envio del mensaje, desencolarlo igualmente.
    (void)QUE_GetByteAndQuit (&OutTmpByte);
    
    // Enviar siguiente mensaje, si es que hay alguno encolado
    while ( NEX_SendNextQueuedMsg() != OK);
    
    
}


RETCODE NEX_SendNextQueuedMsg(void)
{
    UINT8             RxModBusToken;
    UINT16            wSenRequestParam;
    UINT8             RouteMark;
    
    if ( QUE_GetByte (&RxModBusToken) ) 
    {
    
        // ABR_TODO: implementar mecanismo (return ERROR), para seguir depachando mensajes encolados
        // si se perdio el buffer a transmitir
        if (RxModBusToken == 0xFF ) 
        {          
#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
      		  (void)sprintf((char *)udp_buf,"NexoModbus Err:0xC7 \n");
            udp_data_to_send = TRUE;
  }
#endif  	
          MON_ResetIfNotConnected(0XC3);    // Error de SW, Reset (TODO: revisar acciones a tomar)
          return ERROR; 
        }
        
        if ( MemGetRouteMark(RxModBusToken, &RouteMark) != OK)   // Marca de origen          
        {          
#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
      		  (void)sprintf((char *)udp_buf,"NexoModbus Err:0xC8 \n");
            udp_data_to_send = TRUE;
  }
#endif  	
          MON_ResetIfNotConnected(0XC4);    // Error de SW, Reset (TODO: revisar acciones a tomar)
          return ERROR;
        }
        
        if (RouteMark == PROC_MODBUS_SLAVE_RTU)   // Si es un mensaje de Modbus RTU Slave
        {
            wSenRequestParam = 0;     // En la parte alta del parámetro se pasa el offset de transmisión
        } 
        else
        {      
            wSenRequestParam = 6;     // En la parte alta del parámetro se pasa el offset de transmisión
        }
        
        wSenRequestParam <<= 8;
        wSenRequestParam |= RxModBusToken;

        PutEventMsg ( EVT_SCI_TXCMD, NEX_TargetSci, PROC_NEXO, wSenRequestParam );          
    
    }
    
    return OK;
}

//-----------------------------------------------------------------------------
// Procesar requerimiento ModbusRTU CUALQUIER FUNCTION CODE
//-----------------------------------------------------------------------------
//
void f_NexusModbusRtuAny(void)
{
    UINT8             RxModBusRtuToken;
    UINT8             QueueCounter;    
    UINT16            wSenRequestParam;
    MODBUS_RTU_MSG*	  pModbusInMsg;	
        
    // en el parámetro está el token del buffer
    RxModBusRtuToken = (UINT8)CurrentMsg.Param;
    if (MemGetBufferPtr( RxModBusRtuToken , &((void*)pModbusInMsg) ) != OK ){    
        MON_ResetIfNotConnected(0XC5);    // Error de SW, Reset (TODO: revisar acciones a tomar)
        return;   
    }
  
    // Encolar token del msg.
     QueueCounter = QUE_PutByte (RxModBusRtuToken);
		// Si no hay lugar en la cola resetear. ( Hay más espacio que buffers ) 
    if (!QueueCounter) {
        MON_ResetIfNotConnected(0XCC);    // Error de SW, Reset (TODO: revisar acciones a tomar)
        return;       
    }   
		// Si es el primero en la cola, enviarlo inmediatamente
     if ( QueueCounter == 0x01 )  
    {
        wSenRequestParam = 0;     // En la parte alta del parámetro se pasa el offset de transmisión
        wSenRequestParam <<= 8;
        wSenRequestParam |= RxModBusRtuToken;

        PutEventMsg (EVT_SCI_TXCMD, NEX_TargetSci, PROC_NEXO, wSenRequestParam );
    }
     
#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
		  (void)sprintf((char *)udp_buf,"ModRTU_Queue: %d\n",
		                (BYTE)QueueCounter);
      udp_data_to_send = TRUE;
  }
#endif  	
    
    
    return;
}

//-----------------------------------------------------------------------------
// Procesar requerimiento ModbusTCP CUALQUIER FUNCTION CODE
//-----------------------------------------------------------------------------
//
void f_NexusModbusTcpAny(void)
{
    UINT8  IdxSocket; 
    UINT8  QueueCounter;
    UINT8  RxModBusTcpToken;
    UINT16        wSenRequestParam;
    MODBUS_MSG*	  pModbusInMsg;					            // Mensaje recibido por ModbusTCP.



    RxModBusTcpToken = (UINT8)CurrentMsg.Param;
     
    // Si se cerró el socket en el interin, cancelar transaccion
    if (RxModBusTcpToken == 0xFF ) return;
    
    if (MemGetBufferPtr( RxModBusTcpToken , &((void*)pModbusInMsg) ) != OK ){    
#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
  		  (void)sprintf((char *)udp_buf,"NexoModbus Err:0xC2 \n");
        udp_data_to_send = TRUE;
  }
 #endif  	
       MON_ResetIfNotConnected(0XC2);    // Error de SW, Reset (TODO: revisar acciones a tomar)
       return;   
    }

    if ( MemGetRouteMark(RxModBusTcpToken, &IdxSocket ) != OK)   // Marca de origen
    {
#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
  		  (void)sprintf((char *)udp_buf,"NexoModbus Err:0xC0 \n");
        udp_data_to_send = TRUE;
  }
#endif  	
       MON_ResetIfNotConnected(0xC0);    // Error de SW, Reset (TODO: revisar acciones a tomar)
        return;   // ABR_: (SW_ERR) analizar acciones a tomar         
    }
    
    if (IdxSocket >= MAX_SRV_TCPSOCH){
        MON_ResetIfNotConnected(0xC1);    // Error de SW, Reset (TODO: revisar acciones a tomar)
        return;   // ABR_: (SW_ERR) analizar acciones a tomar     
    }


    NEX_ModbusRtuBuildCRC16((UINT8 *)(&(pModbusInMsg->UI)), pModbusInMsg->LengthLow);
  
    // longitud de trama RTU ( 2 del CRC), se escribe en el parametro UsedLen del buffer
    if ( MemSetUsedLen(RxModBusTcpToken, ((pModbusInMsg->LengthLow)+2)) != OK){
#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
  		  (void)sprintf((char *)udp_buf,"NexoModbus Err:0xC7 \n");
        udp_data_to_send = TRUE;
  }
#endif  	
       MON_ResetIfNotConnected(0XC7);    // Error de SW, Reset (TODO: revisar acciones a tomar)
        return;   
    }
  
    // Encolar token del msg.
    QueueCounter = QUE_PutByte (RxModBusTcpToken);
		// Si no hay lugar en la cola resetear. ( Hay más espacio que buffers ) 
    if (!QueueCounter) {
        MON_ResetIfNotConnected(0XCC);    // Error de SW, Reset (TODO: revisar acciones a tomar)
        return;       
    }   
		// Si es el primero en la cola, enviarlo inmediatamente
    if ( QueueCounter == 0x01 )
    {
        wSenRequestParam = 6;     // En la parte alta del parámetro se pasa el offset de transmisión
        wSenRequestParam <<= 8;
        wSenRequestParam |= RxModBusTcpToken;

        PutEventMsg (EVT_SCI_TXCMD, NEX_TargetSci, PROC_NEXO, wSenRequestParam );   
    }
      
    return;
}

//-----------------------------------------------------------------------------
// Procesar requerimiento ModbusTCP READ MULTIPLE REGISTER
//-----------------------------------------------------------------------------
//
void f_NexusModbusTcpReadMulReg (void)
{

    return;
}
		
//-----------------------------------------------------------------------------
// Procesar requerimiento ModbusTCP WRITE SINGLE REGISTER
//-----------------------------------------------------------------------------
//
void f_NexusModbusTcpWriteSinReg (void)
{

    return;
}
		
//-----------------------------------------------------------------------------
// Procesar requerimiento ModbusTCP WRITE MULTIPLE REGISTER
//-----------------------------------------------------------------------------
//
void f_NexusModbusTcpWriteMulReg (void)
{

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
void NEX_Init(void)
{

		NEX_TargetSci = PROC_SCIA;
    SCIA_Init (PROC_NEXO, SCI_FRAME_MODBUSRTU);
    
	  return;
}

//-----------------------------------------------------------------------------
// Calcular CRC
//-----------------------------------------------------------------------------
//
//  The function returns the CRC as a unsigned short type 
//  puchMsg   =  message to calculate CRC upon
//  usDataLen =  quantity of bytes in message

void NEX_ModbusRtuBuildCRC16 ( UINT8 * puchMsg, UINT16 usDataLen )
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
        CRC_Outcome ^= MODBUS_RTU_REVPOLY;
    }
  }
  
  TmpByte = (CRC_Outcome & 0xFF00)>>8;
  CRC_Outcome <<= 8;
  CRC_Outcome |= TmpByte;
  
  *((UINT16*)puchMsg) =  CRC_Outcome;
//  return CRC_Outcome;  

}


#pragma CODE_SEG DEFAULT

#endif 	// (FW_APP_BUILT == FW_APP_MODBUS)   Generar aplicación para protocolos MODBUSTCP a MODBUSRTU
