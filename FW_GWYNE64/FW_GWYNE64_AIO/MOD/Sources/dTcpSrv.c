#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <stdlib.h>
#include "debug.h"
#include "tcp_ip.h"
#include "MC9S12NE64.h"
#include "ne64debug.h"

#include "Commdef.h"
#include "dTcpSrv.h"
#include "Global.h"

#ifdef   TCP_MASTER_MODE			              // La aplicación a generar requiere sockets TCP SERVER 502
  #ifdef    MOD_MODBUSTCP_SLAVE_ENABLED     // Se utilizan los SOCKETS para MODBUSTCP_SLAVE
    #define   TCP_SRV_TMR_SOCKET    120
    #define   TCP_SRV_TMR_WAITTOTX  500             // Timer esperando que se pueda enviar msg
    #define   TCP_SOCKSRV_PORT             502
    #define   TCP_EVT_RXMSG   EVT_MODTCP_RXMSG			// evento a generar con mensajes recibidos
    #define   TCP_EVT_CLOSE   EVT_MODTCP_CLOSE      // evento para avisar que se cerrò la conexion
    #define   TCP_PROC_RXMSG  PROC_MODBUS_SLAVE_TCP	// proceso que atenderá los mensajes
  #else                                   // Se utilizan los SOCKETS para aplicacion indefinida!!
    #error TCP Server Sockets definidos sin utilidad asignada (ver dTcpSrv.c)
  #endif
#endif

#ifdef   TCP_UNITELWAY_MODE			              // La aplicación a generar requiere sockets TCP SERVER  23
  #ifdef    MOD_MODBUSTCP_SLAVE_ENABLED     // Se utilizan los SOCKETS para MODBUSTCP_SLAVE
    #define   TCP_SRV_TMR_SOCKET    120
    #define   TCP_SRV_TMR_WAITTOTX  500             // Timer esperando que se pueda enviar msg
    #define   TCP_SOCKSRV_UNITELWAY_PORT    23
    #define   TCP_EVT_UNITELWAY_RXMSG   EVT_UNITELWAYTCP_RXMSG			// evento a generar con mensajes recibidos
    #define   TCP_PROC_UNITELWAY_RXMSG  PROC_UNITELWAY_TCP	// proceso que atenderá los mensajes
  #else                                   // Se utilizan los SOCKETS para aplicacion indefinida!!
    #error TCP Server Sockets definidos sin utilidad asignada (ver dTcpSrv.c)
  #endif
#endif
  
#ifdef  MOD_SYMEO_ENABLED   // Generar aplicación que utiliza PROC_MODBUS_SYMEO
  #ifdef    MOD_MODBUSTCP_SLAVE_ENABLED     // Se utilizan los SOCKETS para MODBUSTCP_SLAVE
    #define   TCP_SRV_TMR_SOCKET        120
    #define   TCP_SRV_TMR_WAITTOTX      500               // Timer esperando que se pueda enviar msg
    #define   TCP_SOCKSRV_SYMEO_PORT    3046
    #define   TCP_EVT_SYMEO_RXMSG   EVT_SYMEOTCP_RXMSG		// evento a generar con mensajes recibidos
    #define   TCP_PROC_SYMEO_RXMSG  PROC_SYMEO	          // proceso que atenderá los mensajes
  #else                                   // Se utilizan los SOCKETS para aplicacion indefinida!!
    #error TCP Server Sockets definidos sin utilidad asignada (ver dTcpSrv.c)
  #endif  
#endif

//-----------------------------------------------------------------------------------------
// TCP_Init: inicializa todos los sockets
//-----------------------------------------------------------------------------------------
//
void TCP_InitSrvSockets (void)
{

#ifdef   TCP_MASTER_MODE
  UINT8 TmpCntr;
#endif  // TCP_MASTER_MODE			
  
	/* Get socket:
	 * 	TCP_TYPE_SERVER - type of TCP socket is server
	 * 	TCP_TOS_NORMAL  - no other type of service implemented so far
	 *	TCP_DEF_TOUT	- timeout value in seconds. If for this many seconds
	 *		no data is exchanged over the TCP connection the socket will be
	 *		closed.
	 *	tcps_control_panel_eventlistener - pointer to event listener function for
	 * 		this socket.	
	 */
	DEBUGOUT("Initializing TCP application. \r\n");

  // Puerto TCP para Server de mensajes de configuración, recibidos por GWY_Config desde PC
	TcpConfigSoch = tcp_getsocket(TCP_TYPE_SERVER, TCP_DEF_TOUT, TCP_ConfigEventlistener, (UINT16)TCP_DEF_MTU);
	if ( TcpConfigSoch < 0 ){
		DEBUGOUT("TCP: Unable to get socket. Resetting!!!\r\n");
		RESET_SYSTEM();
	} 	
	(void)tcp_listen(TcpConfigSoch, TCP_CFG_PORT);  // listen on some port TCP_CFG_PORT
	TcpConfigState =  FALSE;
	

#ifdef   MOD_SYMEO_ENABLED
  // abrir socket id=0 para symeo
	  TcpSockSrv[0].TcpSrvTcpSoch = tcp_getsocket(TCP_TYPE_SERVER, TCP_SRV_TMR_SOCKET, TCP_SrvTcpEventlistener, (UINT16)TCP_DEF_MTU);
	  if ( TcpSockSrv[0].TcpSrvTcpSoch < 0 ){
		  DEBUGOUT("TCP: Unable to get socket. Resetting!!!\r\n");
		  RESET_SYSTEM();
	  }	 	
	  (void)tcp_listen(TcpSockSrv[0].TcpSrvTcpSoch, TCP_SOCKSRV_SYMEO_PORT);  // listen on port TCP_SOCKSRV_SYMEO_PORT
	  TcpSockSrv[0].status = STATUS_LISTENING;
		TcpSockSrv[0].SlaveIpAddress = 0;
  	TcpSockSrv[0].SlaveTcpPort = 0;
  	TcpSockSrv[0].SlaveNodeAddress = 0;
    TcpSockSrv[0].BuffersInFlight = 0;
    	
#endif  // MOD_SYMEO_ENABLED		

#ifdef   TCP_UNITELWAY_MODE
  // abrir socket id=0 para unitelway
	  TcpSockSrv[0].TcpSrvTcpSoch = tcp_getsocket(TCP_TYPE_SERVER, TCP_SRV_TMR_SOCKET, TCP_SrvTcpEventlistener, (UINT16)TCP_DEF_MTU);
	  if ( TcpSockSrv[0].TcpSrvTcpSoch < 0 ){
		  DEBUGOUT("TCP: Unable to get socket. Resetting!!!\r\n");
		  RESET_SYSTEM();
	  }	 	
	  (void)tcp_listen(TcpSockSrv[0].TcpSrvTcpSoch, TCP_SOCKSRV_UNITELWAY_PORT);  // listen on port TCP_SOCKSRV_PORT
	  TcpSockSrv[0].status = STATUS_LISTENING;
		TcpSockSrv[0].SlaveIpAddress = 0;
  	TcpSockSrv[0].SlaveTcpPort = 0;
  	TcpSockSrv[0].SlaveNodeAddress = 0;
    TcpSockSrv[0].BuffersInFlight = 0;
    	
#endif  // TCP_UNITELWAY_MODE	
	

#ifdef   TCP_MASTER_MODE
  // abrir todos los sockets destinados a SERVER TCP

 #if  ((FW_SUBAPP_FLAG == FW_SUBAPP_MODBUS_SYMEO) || (FW_SUBAPP_FLAG == FW_SUBAPP_MODBUS_RTUWAY))		 // si es symeo o unitellway reservar el socket 0
  for (TmpCntr=1 ; TmpCntr<MAX_SRV_TCPSOCH ; TmpCntr++)
 #else
  for (TmpCntr=0 ; TmpCntr<MAX_SRV_TCPSOCH ; TmpCntr++)
 #endif  
  {
  	  TcpSockSrv[TmpCntr].TcpSrvTcpSoch = tcp_getsocket(TCP_TYPE_SERVER, TCP_SRV_TMR_SOCKET, TCP_SrvTcpEventlistener, (UINT16)TCP_DEF_MTU);//TCP_MIN_MTU);
  	  if ( TcpSockSrv[TmpCntr].TcpSrvTcpSoch < 0 ){
  		  DEBUGOUT("TCP: Unable to get socket. Resetting!!!\r\n");
  		  RESET_SYSTEM();
  	  }	 	
  	  (void)tcp_listen(TcpSockSrv[TmpCntr].TcpSrvTcpSoch, TCP_SOCKSRV_PORT);  // listen on port TCP_SOCKSRV_PORT
  	  TcpSockSrv[TmpCntr].status = STATUS_LISTENING;
  		TcpSockSrv[TmpCntr].SlaveIpAddress = 0;
    	TcpSockSrv[TmpCntr].SlaveTcpPort = 0;
    	TcpSockSrv[TmpCntr].SlaveNodeAddress = 0;
      TcpSockSrv[TmpCntr].BuffersInFlight = 0;
    	
  }
#endif  // TCP_MASTER_MODE
	
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// ····················································································
// Funciones de nexo entre SW Layer TCP_IP y SW Layer Config_TCP 
// ····················································································
//
// ---------------------------------------------------------------------------
// Recibir mensaje desde TCP con informacion de configuracion
//
void TCP_AnalizeConfigMsg(UINT32 len)
{
UINT8 i;
UINT8 * pInCfgMsg;
len++;                                     // para evitar warning del compilador
pInCfgMsg = (UINT8 *)(&CfgMsgFrame);

  for (i=0; i<MIN_CFGFRAME_LEN; i++){
    *pInCfgMsg = GET_DATA();
    pInCfgMsg++;
  }
    
  for ( i=0 ; i < CfgMsgFrame.Len ; i++ ){
    *pInCfgMsg = GET_DATA();
    pInCfgMsg++;
	}
	
	if (CfgMsgFrame.OpCode == CFG_TCP_WRITE_FLASH256)  *pInCfgMsg = GET_DATA(); // leer el nro:256
    
  PutEventMsg (EVT_CFG_RXMSG, PROC_CONFIG, OS_NOT_PROC, 0x00);
    
}

// ---------------------------------------------------------------------------
// Enviar mensaje por puerto TCP con informacion de configuracion
//
UINT8 TCP_SendCfgBuff(void)
{
  UINT8 i;
  UINT16 len;
  UINT8 * pOutCfgMsg;
  pOutCfgMsg = (UINT8 *)(&CfgMsgFrame);
	 	   
//  while(tcp_checksend(TcpConfigSoch) < 0);
  if (tcp_checksend(TcpConfigSoch) < 0){
    return 0x01;
  }
  
  len = 0;
  for( i=0 ; i < MIN_CFGFRAME_LEN ; i++ ){
	  net_buf[TCP_APP_OFFSET+len] = *pOutCfgMsg;
	  pOutCfgMsg++;
	  len++;
	}

  for( i=0 ; i < CfgMsgFrame.Len ; i++ ){
	  net_buf[TCP_APP_OFFSET+len] = *pOutCfgMsg;
	  pOutCfgMsg++;
	  len++;
	}
    	
  if(tcp_send(TcpConfigSoch, &net_buf[TCP_APP_OFFSET], NETWORK_TX_BUFFER_SIZE - TCP_APP_OFFSET, len) == len) {
	    DEBUGOUT("TCP: bytes reply sent!\r\n");
	}
  else {
      DEBUGOUT("TCP: error occured while trying to send data!\r\n");
	}

	return 0x00;
}


// ---------------------------------------------------------------------------
// TCP listen handler para mensajes de configuracion
//
INT32 TCP_ConfigEventlistener(INT8 cbhandle, UINT8 event, UINT32 par1, UINT32 par2)
{
par2++;	// to avoid the compiler warning

/* This function is called by TCP stack to inform about events	*/

	if( cbhandle != TcpConfigSoch)		/* Not our handle	*/
		return(-1);
	
	switch( event )
	{
	
		case TCP_EVENT_CONNECTED:
		  TcpConfigState =  TRUE;		
			break;
			
		case TCP_EVENT_CLOSE:
		  TcpConfigState =  FALSE;
			break;
	
		case TCP_EVENT_CONREQ:
			DEBUGOUT("TCP: Connection request arrived!\r\n");
			return(1);
			break;
				
		case TCP_EVENT_DATA:
			DEBUGOUT("TCP: Data arrived!\r\n");
		    if(par1 <= 0)
		        break;
		    TCP_AnalizeConfigMsg(par1);
			break;
			
		case TCP_EVENT_REGENERATE:
		    TCP_SendCfgBuff();
			break;
	
		default:
			return(-1);
	}
}



#ifdef   TCP_MASTER_MODE			  // La aplicación a generar requiere sockets TCP SERVER
//
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// ····················································································
// Funciones de implementación de SOCKETS TCP MASTER
// ····················································································
//
// ---------------------------------------------------------------------------
//  Desactivación de flag para timer en transmisiones TCP
//
void TCP_StopSrvWhileProc(void) 
{
  TcpSrvWhileFlag = 0x00;
  return;  
}

// ---------------------------------------------------------------------------
// Recibir mensaje desde TCP para interpretar con protocolo SrvTcp
//
void TCP_AnalizeSrvTcpMsg (UINT32 len, UINT8 SocketIdx)
{
  UINT8*  pTxBuffer;
  UINT8*  pTmpTxBuffer;
  UINT8   bTmpToken; 

    if(len > MEM_MAX_BUF_LEN)
      return;
 
//----------------------------------------------------------
// ABR...  
//  if (*pTxBuffer != 0xFF) return;

      if ( ERROR == MemGetBuffer ( (void**)(&pTxBuffer), &bTmpToken)) 
      {
#if DEBUG_AT(DBG_LEVEL_1)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_1)
  {                    
    		  (void)sprintf((char *)udp_buf,"ERROR MEMBUF para Idx: %d, IP:%d \n",
    		                SocketIdx,
    		                (BYTE)TcpSockSrv[SocketIdx].SlaveIpAddress);
          udp_data_to_send = TRUE;
  }
#endif  		          
          PutEventMsg (EVT_LOG_ID, PROC_DEBUG, OS_NOT_PROC, (UINT16)LOG_ID_TCP_NOBUFF);    
          MON_ResetIfNotConnected(0x31);    // Error de SW, Reset (TODO: revisar acciones a tomar)
          
          //ABR_TODO: Avisar al driver modbus que llego un mensaje y no pudo ser almacenado	 ??
          return;
      } 
       
      //TcpSockSrv[SocketIdx].BufferToken = bTmpToken;
      
      // Marcar el buffer con el ID del socket
      (void)MemSetRouteMark(bTmpToken, SocketIdx);

//------------------------------------------------------------
   
  pTmpTxBuffer = pTxBuffer;
  
  RECEIVE_NETWORK_BUF(pTxBuffer, (UINT16)len);
 
  TcpSockSrv[SocketIdx].MsgLen = (UINT16)len;
  TcpSockSrv[SocketIdx].TxRetries = 3;
  TcpSockSrv[SocketIdx].BuffersInFlight++;			 
  
#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
      if ( TcpSockSrv[SocketIdx].BuffersInFlight > 1 )
      {        
      		  (void)sprintf((char *)udp_buf,"TcpServerSocket BuffersInFlight:%d, Idx:%d \n",
      		                TcpSockSrv[SocketIdx].BuffersInFlight,
      		                SocketIdx);
            udp_data_to_send = TRUE;
      }
  }
#endif  		          
  
  

#ifdef AUTO_MTU
  if ( TcpSockSrv[SocketIdx].BuffersInFlight > MODTCP_QUERIES_ALLOWED )
  {
    (void)tcp_changemtu (TcpSockSrv[SocketIdx].TcpSrvTcpSoch, TCP_MIN_MTU);    
  } 
#endif  
  
  
#if  (FW_SUBAPP_FLAG == FW_SUBAPP_MODBUS_SYMEO) // SYMEO

        if ( MemSetUsedLen(bTmpToken, (UINT16)len) != OK){
    #if DEBUG_AT(DBG_LEVEL_3)
      if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
      {                    
      		  (void)sprintf((char *)udp_buf,"TcpSymeoErr:0xC7 \n");
            udp_data_to_send = TRUE;
      }
    #endif  	
           MON_ResetIfNotConnected(0XC7);    // Error de SW, Reset (TODO: revisar acciones a tomar)
           return;   
        } 
         
        if (SocketIdx==0)
          PutEventMsg (TCP_EVT_SYMEO_RXMSG, TCP_PROC_SYMEO_RXMSG, OS_NOT_PROC, bTmpToken);
        else
          PutEventMsg (TCP_EVT_RXMSG, TCP_PROC_RXMSG, OS_NOT_PROC, bTmpToken);
        
    
#elif  (FW_SUBAPP_FLAG == FW_SUBAPP_MODBUS_RTUWAY)  //   UNITELWAY

        if ( MemSetUsedLen(bTmpToken, (UINT16)len) != OK){
    #if DEBUG_AT(DBG_LEVEL_3)
      if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
      {                    
      		  (void)sprintf((char *)udp_buf,"TcpUnitelwayErr:0xC7 \n");
            udp_data_to_send = TRUE;
      }
    #endif  	
           MON_ResetIfNotConnected(0XC7);    // Error de SW, Reset (TODO: revisar acciones a tomar)
           return;   
        } 
         
        if (SocketIdx==0)
          PutEventMsg (TCP_EVT_UNITELWAY_RXMSG, TCP_PROC_UNITELWAY_RXMSG, OS_NOT_PROC, bTmpToken);
        else
          PutEventMsg (TCP_EVT_RXMSG, TCP_PROC_RXMSG, OS_NOT_PROC, bTmpToken);
        
    
#else

     PutEventMsg (TCP_EVT_RXMSG, TCP_PROC_RXMSG, OS_NOT_PROC, bTmpToken);
  
#endif

#if DEBUG_AT(DBG_LEVEL_5)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_5)
  {                    
		  (void)sprintf((char *)udp_buf,"TCP_Data Len: %d, IP:%d \n",
		                (WORD)len,
		                (BYTE)TcpSockSrv[SocketIdx].SlaveIpAddress);
      udp_data_to_send = TRUE;
  }
#endif  		

 	
}



// ---------------------------------------------------------------------------
// TCP_RetransmitSrvTcpMsg : ReEnviar mensaje SrvTcp por puerto TCP
//
/*
UINT8 TCP_RetransmitSrvTcpMsg(UINT8 SocketIdx)
{
  UINT16  i;
  UINT16  len;
  UINT8   TimerGetStateId;
  UINT8*  pTxBuffer;
  UINT8   bTmpToken; 
  UINT16  BufLen;

  if (tcp_getstate(TcpSockSrv[SocketIdx].TcpSrvTcpSoch) != TCP_STATE_CONNECTED){
        return (-1);
  }

  if (tcp_checksend(TcpSockSrv[SocketIdx].TcpSrvTcpSoch) < 0) 
  {
    TcpSrvWhileFlag = 0x01;
    TimerGetStateId = TMR_SetTimer ( TCP_SRV_TMR_WAITTOTX , OS_NOT_PROC, TCP_StopSrvWhileProc, FALSE);
    while (tcp_checksend(TcpSockSrv[SocketIdx].TcpSrvTcpSoch) < 0)
    {
       if (!TcpSrvWhileFlag) return (-1);    // Cuando vence el timer se resetea TcpSrvWhileFlag
    }
    TMR_FreeTimer ( TimerGetStateId );
  }

  bTmpToken = TcpSockSrv[SocketIdx].BufferToken;
  if (MemGetBufferPtr( bTmpToken ,  (void**)(&pTxBuffer) ) != OK ){      
    (void)MemFreeBuffer (bTmpToken);   // Liberar por las dudas ? ABR: control de errores
    return(-1);   
  }

  BufLen = TcpSockSrv[SocketIdx].MsgLen;

  len = 0;
  for( i=0 ; i < BufLen ; i++ ){
	  net_buf[TCP_APP_OFFSET+len] = *pTxBuffer;
	  pTxBuffer++;
	  len++;
	}

  if(tcp_send(TcpSockSrv[SocketIdx].TcpSrvTcpSoch, &net_buf[TCP_APP_OFFSET], NETWORK_TX_BUFFER_SIZE - TCP_APP_OFFSET, len) == len) {
	    DEBUGOUT("TCP: bytes reply sent!\r\n");
#if DEBUG_AT(DBG_LEVEL_4)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_4)
  {                    
		  (void)sprintf((char *)udp_buf,"TCP_Sent Len: %d, IP:%d \n",
		                len,
		                (BYTE)TcpSockSrv[SocketIdx].SlaveIpAddress);
      udp_data_to_send = TRUE;
   }
#endif
	}
  else {
      DEBUGOUT("TCP: error occured while trying to send data!\r\n");
	}

	return (1);
	
}
*/

// ---------------------------------------------------------------------------
// TCP_SendSrvTcpMsg : Enviar mensaje SrvTcp por puerto TCP
//
/*
UINT8 TCP_SendSrvTcpBuff(UINT8 * Buff, UINT16 BufLen, UINT8 SocketIdx)
{
  UINT16  i;
  UINT16  len;
  UINT8   TimerGetStateId;

  if (tcp_getstate(TcpSockSrv[SocketIdx].TcpSrvTcpSoch) != TCP_STATE_CONNECTED){
        return 0x01;
  }

  if (tcp_checksend(TcpSockSrv[SocketIdx].TcpSrvTcpSoch) < 0) 
  {
    TcpSrvWhileFlag = 0x01;
    TimerGetStateId = TMR_SetTimer ( TCP_SRV_TMR_WAITTOTX , OS_NOT_PROC, TCP_StopSrvWhileProc, FALSE);
    while (tcp_checksend(TcpSockSrv[SocketIdx].TcpSrvTcpSoch) < 0)
    {
       if (!TcpSrvWhileFlag) return 0x01;    // Cuando vence el timer se resetea TcpSrvWhileFlag
    }
    TMR_FreeTimer ( TimerGetStateId );
  }

  len = 0;
  for( i=0 ; i < BufLen ; i++ ){
	  net_buf[TCP_APP_OFFSET+len] = *Buff;
	  Buff++;
	  len++;
	}
	

  if(tcp_send(TcpSockSrv[SocketIdx].TcpSrvTcpSoch, &net_buf[TCP_APP_OFFSET], NETWORK_TX_BUFFER_SIZE - TCP_APP_OFFSET, len) == len) {
	    DEBUGOUT("TCP: bytes reply sent!\r\n");
#if DEBUG_AT(DBG_LEVEL_4)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_)
  {                    
		  (void)sprintf((char *)udp_buf,"TCP_Sent Len: %d, IP:%d \n",
		                len,
		                (BYTE)TcpSockSrv[SocketIdx].SlaveIpAddress);
      udp_data_to_send = TRUE;
   }
#endif
	}
  else {
      DEBUGOUT("TCP: error occured while trying to send data!\r\n");
	}

	return 0x00;
	
}
*/

// ---------------------------------------------------------------------------
 /*
 * Event listener invoked when TCP/IP stack receives TCP data for
 * a given socket. Parameters:
 * - cbhandle - handle of the socket this packet is intended for. Check it
 *	just to be sure, but in general case not needed
 * - event - event that is notified. For TCP there are quite a few possible
 *	events, check switch structure below for more information
 * - par1, par2 - parameters who's use depends on the event that is notified
 */
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// TCP listen handler para mensajes ServerTCP
//
INT32 TCP_SrvTcpEventlistener(INT8 cbhandle, UINT8 event, UINT32 par1, UINT32 par2)
{
  UINT8 TmpIdx;
//  UINT8*  pTxBuffer;
//  UINT8   bTmpToken; 
//  UINT8*	pModbusInMsg;					                  // Mensaje recibido por ModbusTCP.

//  par2++;	// to avoid the compiler warning

/* This function is called by TCP stack to inform about events	*/

  TmpIdx=0; 
  while (cbhandle != TcpSockSrv[TmpIdx].TcpSrvTcpSoch )
  {   
    TmpIdx++;
    if ( TmpIdx >= MAX_SRV_TCPSOCH ) {
#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
		  (void)sprintf((char *)udp_buf,"DNP_TCP BadHandle: %d, H1:%d, H2:%d \n",
		                cbhandle ,
		                TcpSockSrv[0].TcpSrvTcpSoch ,
		                TcpSockSrv[1].TcpSrvTcpSoch );
      udp_data_to_send = TRUE;
  }
#endif  		
     return(-1);
    }
  }
	
  
	switch( event )
	{
	
		/* Connection request event. Used by TCP/IP stack to inform
		 * the application someone is trying to establish a connection.
		 * Server can decide, based on provided IP address and port number,
		 * whether to allow or not connection establishment.
		 * Parameters:
		 *  - par1 - remote hosts IP address
		 *  - par2 - remote hosts port number
		 * 
		 * Return values from event listener:
		 * -1 - do not allow connection to be established (send reset)
		 * -2 - do not send any response for now to the SYN packet (let's
		 *		think a little before answering)
		 * 1  - allow connection to be established
		 */
		case TCP_EVENT_CONREQ:
			DEBUGOUT("TCP: Connection request arrived!\r\n");
			
			/* Enable all connections	*/
			return(1);
		
			break;
	
		/* Connection abort event. Connection on a given socket is beeing 
		 * aborted for somereason (usually retransmissions are used up or 
		 * some abnormal situation in communication happened).
 		 * Parameters:
		 *  - par1 - remote hosts IP address
		 *  - par2 - remote hosts port number
		 */
		case TCP_EVENT_ABORT:
			DEBUGOUT("Connection aborting!\r\n");	
			TcpSockSrv[TmpIdx].status = STATUS_LISTENING;
			TcpSockSrv[TmpIdx].SlaveIpAddress = 0;
  		TcpSockSrv[TmpIdx].SlaveTcpPort = 0;
  	  TcpSockSrv[TmpIdx].SlaveNodeAddress = 0;
      //(void)MemFreeBuffer (TcpSockSrv[TmpIdx].BufferToken);   // Liberar el buffer
      TcpSockSrv[TmpIdx].BuffersInFlight = 0;
      
      //2014-03-25 - ABR: TODO: liberar todos los buffers encolados pendientes de transmision
      PutEventMsg (TCP_EVT_CLOSE, TCP_PROC_RXMSG, OS_NOT_PROC, 0x00);  // avisar al proceso

#if DEBUG_AT(DBG_LEVEL_2)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_2)
  {                    
		  (void)sprintf((char *)udp_buf,"TCP_ABORT Idx: %d, IP:%d \n",
		                TmpIdx,
		                (BYTE)par1);
      udp_data_to_send = TRUE;      
  }
#endif  		
/*
      if ( OK == MemGetBuffer ((void**)(&pTxBuffer), &bTmpToken)) 
      {
          (void)sprintf((char *)&(((LOG_FORMAT*)pTxBuffer)->Data[0]),"TCP_ABORT Idx: %d, IP:%d \0",
    		                TmpIdx,
    		                (BYTE)TcpSockSrv[TmpIdx].SlaveIpAddress);        
          ((LOG_FORMAT*)pTxBuffer)->LogId = LOG_ID_TCP;
          PutEventMsg (EVT_LOG_SAVE, PROC_DEBUG, OS_NOT_PROC, (UINT16)bTmpToken);    
      } 
      else
          PutEventMsg (EVT_LOG_ID, PROC_DEBUG, OS_NOT_PROC, (UINT16)LOG_ID_TCP_ABORT);    
*/ 

			break;
		
		/* Connection established event - three-way handshaking performed
		 * OK and connection is established.
		 * Parameters:
		 *  - par1 - remote hosts IP address
		 *  - par2 - remote hosts port number
		 */
		case TCP_EVENT_CONNECTED:
			DEBUGOUT("TCP: TCP connection established!\r\n");
			TcpSockSrv[TmpIdx].status = STATUS_ACTIVE;
			TcpSockSrv[TmpIdx].SlaveIpAddress = par1;
  		TcpSockSrv[TmpIdx].SlaveTcpPort = (UINT16)par2;
  	  TcpSockSrv[TmpIdx].SlaveNodeAddress = 0;
  	  
/*  	  
  	  // Pedir buffer para el nuevo socket.  	  
  	  if (TcpSockSrv[TmpIdx].BufferToken != 0xFF) {
          (void)MemFreeBuffer (TcpSockSrv[TmpIdx].BufferToken);   // Liberar el buffer
  	  }
      // ABR: tratamiento de mensajes modbus con buffers de memoria
      if ( ERROR == MemGetBuffer ( (void**)(&pTxBuffer), &bTmpToken)) 
      {
#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
    		  (void)sprintf((char *)udp_buf,"ERROR MEMBUF para Idx: %d, IP:%d \n",
    		                TmpIdx,
    		                (BYTE)TcpSockSrv[TmpIdx].SlaveIpAddress);
          udp_data_to_send = TRUE;
   }       
#endif  		          
          PutEventMsg (EVT_LOG_ID, PROC_DEBUG, OS_NOT_PROC, (UINT16)LOG_ID_TCP_NOBUFF);    
          return(-1); 
      }  
      TcpSockSrv[TmpIdx].BufferToken = bTmpToken;
      
//      if (MemGetBufferPtr( bTmpToken , &((void*)pModbusInMsg) ) == OK ){       
//          ((UINT8*)pModbusInMsg)[0] = 0xFF;  // liberar          
//      }
*/  	  
  	  
#if DEBUG_AT(DBG_LEVEL_2)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_2)
  {                    
		  (void)sprintf((char *)udp_buf,"TCP_CONN Idx: %d, IP:%d \n",
		                TmpIdx,
		                (BYTE)TcpSockSrv[TmpIdx].SlaveIpAddress);
      udp_data_to_send = TRUE;
  }
#endif  		
/*
      if ( OK == MemGetBuffer((void**)(&pTxBuffer), &bTmpToken)) 
      {
          (void)sprintf((char *)&(((LOG_FORMAT*)pTxBuffer)->Data[0]),"TCP_CONN Idx: %d, IP:%d \0",
    		                TmpIdx,
    		                (BYTE)TcpSockSrv[TmpIdx].SlaveIpAddress);        
          ((LOG_FORMAT*)pTxBuffer)->LogId = LOG_ID_TCP;
//          FLA_SaveLog(bTmpToken);
          PutEventMsg (EVT_LOG_SAVE, PROC_DEBUG, OS_NOT_PROC, (UINT16)bTmpToken);    
          
      } 
      else
          PutEventMsg (EVT_LOG_ID, PROC_DEBUG, OS_NOT_PROC, (UINT16)LOG_ID_TCP_CONNECT);    
*/      

			break;
			
		/* Connection closing event. Happens when TCP connection is
		 * intentionally close by some side calling close function and
		 * initializing proper TCP connection close procedure.
		 * Parameters:
		 *  - par1 - remote hosts IP address
		 *  - par2 - remote hosts port number
		 */
		case TCP_EVENT_CLOSE:
			DEBUGOUT("TCP Connection closing...!\r\n");
			TcpSockSrv[TmpIdx].status = STATUS_LISTENING;
			TcpSockSrv[TmpIdx].SlaveIpAddress = 0;
  		TcpSockSrv[TmpIdx].SlaveTcpPort = 0;
  	  TcpSockSrv[TmpIdx].SlaveNodeAddress = 0;
      //(void)MemFreeMarkBuffer (TmpIdx);   // Liberar los buffers
      TcpSockSrv[TmpIdx].BuffersInFlight = 0;

      PutEventMsg (TCP_EVT_CLOSE, TCP_PROC_RXMSG, OS_NOT_PROC, 0x00);  // avisar al proceso

#if DEBUG_AT(DBG_LEVEL_2)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_2)
  {                    
		  (void)sprintf((char *)udp_buf,"TCP_CLOSE Idx: %d, IP:%d \n",
		                TmpIdx,
		                (BYTE)par1);
      udp_data_to_send = TRUE;
  }
#endif  
/*		
      if ( OK == MemGetBuffer ( (void**)(&pTxBuffer), &bTmpToken)) 
      {
          (void)sprintf((char *)&(((LOG_FORMAT*)pTxBuffer)->Data[0]),"TCP_CLOSE Idx: %d, IP:%d \0",
    		                TmpIdx,
    		                (BYTE)TcpSockSrv[TmpIdx].SlaveIpAddress);        
          ((LOG_FORMAT*)pTxBuffer)->LogId = LOG_ID_TCP;
          PutEventMsg (EVT_LOG_SAVE, PROC_DEBUG, OS_NOT_PROC, (UINT16)bTmpToken);    
      }
      else 
          PutEventMsg (EVT_LOG_ID, PROC_DEBUG, OS_NOT_PROC, (UINT16)LOG_ID_TCP_CLOSE);    
*/      
      
			break;
			
		/* Data acknowledgment event. Happens when data that was
		 * previously sent gets acknowledged. This means we can now
		 * send some more data! :-)
		 * Parameters:
		 *  - par1 - remote hosts IP address
		 *  - par2 - remote hosts port number
		 */
		case TCP_EVENT_ACK:
			DEBUGOUT("TCP: Data acknowledged!\r\n");
			/* if more data should be sent, adjust variables and
				set tcps_demo_senddata variable */
				
			break;
			
		/* Data received event. Happens when we receive some data over the
		 * TCP connection.
		 * Parameters:
		 *  - par1 - number of data bytes received
		 *  - par2 = 0
		 */
		case TCP_EVENT_DATA:
			DEBUGOUT("TCP: Data arrived!\r\n");
			/* read data that was received (and 
			 * probably do something with it :-)
			 */
		  if(par1 <= 0)
		     break;
		  TCP_AnalizeSrvTcpMsg(par1, TmpIdx);
			break;
			
		/* Regenerate data event. Happens when data needs to be
		 * retransmitted because of possible loss on the network.
		 * Note that THE SAME DATA must be sent over and over again
		 * until TCP_EVENT_ACK is generated (for that data)! 
		 * Parameters:
		 *  - par1 - amount of data to regenerate (usually all)
		 *	- par2 = 0
		 */
		case TCP_EVENT_REGENERATE:
      DEBUGOUT("TCP: Error intentando regenerar paquete perdido\r\n");
#if DEBUG_AT(DBG_LEVEL_2)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_2)
  {                    
		  (void)sprintf((char *)udp_buf,"TCP_REGENERATE Idx: %d, IP:%d \n",
		                TmpIdx,
		                (BYTE)TcpSockSrv[TmpIdx].SlaveIpAddress);
      udp_data_to_send = TRUE;
  }      
#endif
/*
      if ( OK == MemGetBuffer ( (void**)(&pTxBuffer), &bTmpToken)) 
      {
          (void)sprintf((char *)&(((LOG_FORMAT*)pTxBuffer)->Data[0]),"TCP_REGENERATE Idx: %d, IP:%d \0",
    		                TmpIdx,
    		                (BYTE)TcpSockSrv[TmpIdx].SlaveIpAddress);        
          ((LOG_FORMAT*)pTxBuffer)->LogId = LOG_ID_TCP;
          PutEventMsg (EVT_LOG_SAVE, PROC_DEBUG, OS_NOT_PROC, (UINT16)bTmpToken);    
      } 
      else
          PutEventMsg (EVT_LOG_ID, PROC_DEBUG, OS_NOT_PROC, (UINT16)LOG_ID_TCP_REGENERATE);    
*/
      //return (TCP_RetransmitSrvTcpMsg(TmpIdx));
      return (-1);
			break;
	
		default:
			return(-1);
	}
	
	return (1);
}

#endif // TCP_MASTER_MODE			 La aplicación a generar requiere sockets TCP SERVER

