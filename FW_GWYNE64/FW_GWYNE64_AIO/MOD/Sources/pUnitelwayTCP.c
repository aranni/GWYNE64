#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"
#include "Commdef.h"

#ifdef  MOD_UNITELWAY_TCP_ENABLED   // Generar aplicación que utiliza MOD_UNITELWAY_TCP_ENABLED

#include "pUnitelwayTCP.h"
#include "Global.h"
#include "system.h"
#include "tcp_ip.h"

///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_UNITELWAY_TCP
///////////////////////////////////////////////////////////////////////////////
//
const EVENT_FUNCTION	UnitelwayTcpProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	        , f_UnitelwayTcpIdleTimeout	    },
{EVT_UNITELWAYTCP_RXMSG 	, f_UnitelwayTcpIdleAnalizeMsg  },
{EVT_UNITELWAY_TXMSG 	    , f_UnitelwayTcpIdleSendMsg     },
{EVT_MODRTU_ERR 	        , f_UnitelwayTcpIdleErrMsg      },
{EVT_OTHER_MSG     		    , f_Consume               }};

 
///////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_UNITELWAY_TCP
///////////////////////////////////////////////////////////////////////////////
//
//-------------- estado : IDLE ----------------------------------------------------------------
//
//-----------------------------------------------------------------------------
// Analizar mensaje Modbus recibido
//-----------------------------------------------------------------------------
//
void f_UnitelwayTcpIdleAnalizeMsg (void)
{

  UINT8   IdxSocket;  
  UINT8   RxModBusTcpToken;
  UNITELWAY_MSG*	  pUnitelwayInMsg;					            // Mensaje recibido por ModbusTCP.
  
  RxModBusTcpToken = (UINT8)CurrentMsg.Param;
   
  // Si se cerró el socket en el interin, cancelar transaccion
  if (RxModBusTcpToken == 0xFF ) return;
  
  if (MemGetBufferPtr( RxModBusTcpToken , &((void*)pUnitelwayInMsg) ) != OK ){    
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

  
//  if(pUnitelwayInMsg->LEN_BYTE == DLE_CHAR)
//  {
    // ANALIZAR EL CASO
//  }    
  
  PutEventMsg (EVT_UNITELWAY_REQUEST, PROC_NEXO, PROC_UNITELWAY_TCP, RxModBusTcpToken);          

 	return;
}
		

//-----------------------------------------------------------------------------
// Timeout en intento de transmisión de respuesta
//-----------------------------------------------------------------------------
//
void f_UnitelwayTcpIdleTimeout (void) 
{
  UINT8 TmpIdx;
  UINT8 CurrentTimerId;

  CurrentTimerId = (UINT8)CurrentMsg.Param;

  // Buscar con el id del timer a qué socket modbus tcp pertenece el timeout
  TmpIdx=0; 
  while (CurrentTimerId != TcpSockSrv[TmpIdx].TimerId ){   
    TmpIdx++;
    if ( TmpIdx >= MAX_SRV_TCPSOCH ){
        MON_ResetIfNotConnected(0XA3);    // Error de SW, Reset (TODO: revisar acciones a tomar)
        return;
    }
  }

  TcpSockSrv[TmpIdx].TimerId = 0xFF;
  PutEventMsg (EVT_MODTCP_TXMSG, PROC_MODBUS_SLAVE_TCP, PROC_MODBUS_SLAVE_TCP, TcpSockSrv[TmpIdx].BufferToken);
	
	return;
}


//-----------------------------------------------------------------------------
// Transmitir o Retransmitir mensaje Unitelway recibido
//-----------------------------------------------------------------------------
//
void f_UnitelwayTcpIdleSendMsg (void)
{

  UINT8   IdxSocket;  
  UINT16  TcpMsgLen; 
  UINT8   RxModBusTcpToken;
  UINT8*	pUnitelwayInMsg;					 
  
  
  RxModBusTcpToken = (UINT8)CurrentMsg.Param;
   
  // Si se cerró el socket en el interin, cancelar transaccion
  if (RxModBusTcpToken == 0xFF ) return;
  
  if (MemGetBufferPtr( RxModBusTcpToken , &((void*)pUnitelwayInMsg) ) != OK ){    
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


   // Verificar si es posible enviar datos (puede haber un msg. previo aun sin ACK)
  if (tcp_checksend(TcpSockSrv[IdxSocket].TcpSrvTcpSoch) < 0){
    if (TcpSockSrv[IdxSocket].TxRetries) // si quedan reintentos, reintentar luego de un tiempo
    {      
      TcpSockSrv[IdxSocket].TxRetries--;
	    TcpSockSrv[IdxSocket].TimerId = TMR_SetTimer ( TCP_TIMEOUT_BUSYSENDRETRY , PROC_MODBUS_SLAVE_TCP, 0x00, FALSE );	// Reintentar
    } 
    else{
        (void)MemFreeBuffer (RxModBusTcpToken);   // Liberar el buffer
        TcpSockSrv[IdxSocket].BufferToken = 0xFF;    
    }
    return;
  }


    if ( MemGetUsedLen(RxModBusTcpToken, &TcpMsgLen ) != OK)   // Bytes recibidos
    {
    	  // Se liberó el buffer en otro proceso, descartar respuesta y procesar la proxima encuesta encolada   	 
#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
    		  (void)sprintf((char *)udp_buf,"NexoModbus Err:0xA8 \n");
          udp_data_to_send = TRUE;
  }
#endif  	
        (void)MemFreeBuffer (RxModBusTcpToken);   // Liberar el buffer
        TcpSockSrv[IdxSocket].BufferToken = 0xFF;        
        return;            
    }
   
  // ABR: 2012-02-22: analizar luego por que la aplicacion no reconoce respuestas con len < 12
  if (TcpMsgLen < 12)
    TcpMsgLen=12;
  
  
  (void)memcpy ((UINT8*)(&net_buf[TCP_APP_OFFSET]), &((UINT8*)pUnitelwayInMsg)[0], TcpMsgLen);
      
  
  if (tcp_send(TcpSockSrv[IdxSocket].TcpSrvTcpSoch, &net_buf[TCP_APP_OFFSET], NETWORK_TX_BUFFER_SIZE - TCP_APP_OFFSET, TcpMsgLen) != TcpMsgLen)
  { 
    if (TcpSockSrv[IdxSocket].TxRetries) // si quedan reintentos, reintentar luego de un tiempo
    {      
        TcpSockSrv[IdxSocket].TxRetries--;
        TcpSockSrv[IdxSocket].BufferToken = RxModBusTcpToken;   // guardar token de buffer en curso      
  	    TcpSockSrv[IdxSocket].TimerId = TMR_SetTimer ( TCP_TIMEOUT_SENDRETRY , PROC_MODBUS_SLAVE_TCP, 0x00, FALSE );	// Reintentar
    } 
    else  // descartar el envío de respuesta
    {
        (void)MemFreeBuffer (RxModBusTcpToken);   // Liberar el buffer
        TcpSockSrv[IdxSocket].BufferToken = 0xFF;
//      ((UINT8*)pUnitelwayInMsg)[0] = 0xFF;  // liberar 
    }
	}
	else    // respuesta enviada OK
	{
        TcpSockSrv[IdxSocket].TxRetries = 0;
        (void)MemFreeBuffer (RxModBusTcpToken);   // Liberar el buffer
        TcpSockSrv[IdxSocket].BufferToken = 0xFF;
//      ((UINT8*)pUnitelwayInMsg)[0] = 0xFF;  // liberar 
	}
 	return;
}
		

//-----------------------------------------------------------------------------
// Error en mensaje convertido a RTU descartar buffer TCP
//-----------------------------------------------------------------------------
//
void f_UnitelwayTcpIdleErrMsg (void)
{

  UINT8   IdxSocket;  
  UINT8   RxModBusTcpToken;
   
  
  RxModBusTcpToken = (UINT8)CurrentMsg.Param;
   
  // Si se cerró el socket en el interin, cancelar transaccion
  if (RxModBusTcpToken == 0xFF ) return;
  

  if ( MemGetRouteMark(RxModBusTcpToken, &IdxSocket ) != OK)   // Marca de origen
  {
      MON_ResetIfNotConnected(0xA5);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;   // ABR_: (SW_ERR) analizar acciones a tomar         
  }
  
  if (IdxSocket >= MAX_SRV_TCPSOCH){
      MON_ResetIfNotConnected(0xA6);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;   // ABR_: (SW_ERR) analizar acciones a tomar     
  }
  

  (void)MemFreeBuffer (RxModBusTcpToken);   // Liberar el buffer
  TcpSockSrv[IdxSocket].BufferToken = 0xFF;
  
  //ABR_TODO: enviar excepcion ??

        
}


///////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// MOD_UnitelwayTcpInit : inicializacion de variables del proceso
//-----------------------------------------------------------------------------
//
void MOD_UnitelwayTcpInit(void)
{
  	//eeprom_read_buffer(EE_ADDR_NODE_SCIA, &ModbusTcpLocalNode ,1); 
	  return;
}

#pragma CODE_SEG DEFAULT

#endif		//  (MOD_MODBUSTCP_SLAVE_ENABLED)   Generar aplicación que utiliza MODBUSTCP
