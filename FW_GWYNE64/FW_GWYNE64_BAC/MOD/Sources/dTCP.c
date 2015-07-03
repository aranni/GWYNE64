#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <stdlib.h>
#include "debug.h"
#include "tcp_ip.h"
#include "MC9S12NE64.h"
#include "ne64debug.h"

#include "Commdef.h"
#include "dTCP.h"
#include "Global.h"

#define   TCP_SRV_TOUT	  120
#define   TCP_CLI_TOUT	  120
#define   DNP_LOCAL_PORT  1104

#define   TCP_WAITFORSEND_TOUT  500     // Timer esperando que se pueda enviar mensaje por tcp


//-----------------------------------------------------------------------------------------
// TCP_Init: inicializa todos los sockets
//-----------------------------------------------------------------------------------------
//
void TCP_Init (void)
{

	/* Get socket:
	 * 	TCP_TYPE_SERVER - type of TCP socket is server
	 * 	TCP_TOS_NORMAL  - no other type of service implemented so far
	 *	TCP_DEF_TOUT	- timeout value in seconds. If for this many seconds
	 *		no data is exchanged over the TCP connection the socket will be
	 *		closed.
	 *	tcps_control_panel_eventlistener - pointer to event listener function for
	 * 		this socket.	
	 */
 UINT8   TmpCntr; 	

 #ifdef   DNP_SLAVE_MODE

  UINT8   CliTcpIpAddrTxt[30];
  UINT8   CliTcpIpAddrIdx;
  UINT8   CliTcpIpCharIdx;
  UINT8   CliTcpIpChar[4];

  // el txt tiene address y puerto con formato xxx.xxx.xxx.xxx:ppp
 	eeprom_read_buffer( EE_ADDR_VER_TEXT, CliTcpIpAddrTxt, 30);
 	CliTcpIpAddrIdx=0; 
 	CliTcpIpCharIdx=0; 
 	for ( TmpCntr=0; TmpCntr<30 ; TmpCntr++){
 	  if (CliTcpIpAddrTxt[TmpCntr] == '.'){
 	    CliTcpIpAddrTxt[TmpCntr] = '\0';
 	    CliTcpIpChar[CliTcpIpCharIdx++] = (UINT8)(atoi((INT8*)&(CliTcpIpAddrTxt[CliTcpIpAddrIdx])));
 	    CliTcpIpAddrIdx = TmpCntr+1;
 	  } 
 	  if (CliTcpIpAddrTxt[TmpCntr] == ':'){
 	    CliTcpIpAddrTxt[TmpCntr] = '\0';
 	    CliTcpIpChar[CliTcpIpCharIdx++] = (UINT8)(atoi((INT8*)&(CliTcpIpAddrTxt[CliTcpIpAddrIdx])));
 	    CliTcpIpAddrIdx = TmpCntr+1;
 	    break;
 	  }
// 	  if (CliTcpIpAddrTxt[TmpCntr] == '\0') break;
 	}
  CliTcpIp = *((UINT32 *)CliTcpIpChar); 
  CliTcpSockPort = atoi((INT8*)&(CliTcpIpAddrTxt[CliTcpIpAddrIdx]));
 	
#endif //  DNP_SLAVE_MODE
	
	
	DEBUGOUT("Initializing TCP application. \r\n");
	
	
#ifdef   DNP_MASTER_MODE
// abrir todos los sockets destinados a recibir conexiones de DNP esclavos
for (TmpCntr=0 ; TmpCntr<MAX_SRV_TCPSOCH ; TmpCntr++)
{
	  // Puerto TCP Server recibir mensajes DNP_ETH desde el conversor remoto
	  TcpSockSrv[TmpCntr].TcpSrvTcpSoch = tcp_getsocket(TCP_TYPE_SERVER, TCP_TOS_NORMAL, TCP_SRV_TOUT, TCP_SrvTcpEventlistener);
	  if ( TcpSockSrv[TmpCntr].TcpSrvTcpSoch < 0 ){
		  DEBUGOUT("TCP: Unable to get socket. Resetting!!!\r\n");
		  RESET_SYSTEM();
	  }	 	
	  (void)tcp_listen(TcpSockSrv[TmpCntr].TcpSrvTcpSoch, TCP_DNPSRV_PORT);  // listen on port TCP_DNPSRV_PORT
//	(void)tcp_listen(TcpSrvTcpSoch, (UINT16)(CliTcpSockPort));  // listen on port CliTcpRemotePort
	  TcpSockSrv[TmpCntr].status = STATUS_LISTENING;
		TcpSockSrv[TmpCntr].SlaveIpAddress = 0;
  	TcpSockSrv[TmpCntr].SlaveTcpPort = 0;
  	TcpSockSrv[TmpCntr].SlaveNodeAddress = 0;
    TcpSockSrv[TmpCntr].BufferToken = 0xFF;
  	
}

#endif //  DNP_MASTER_MODE			
					
  // Puerto TCP para Server de mensajes de configuración, recibidos por GWY_Config desde PC
	TcpConfigSoch = tcp_getsocket(TCP_TYPE_SERVER, TCP_TOS_NORMAL, TCP_DEF_TOUT, TCP_ConfigEventlistener);
	if ( TcpConfigSoch < 0 ){
		DEBUGOUT("TCP: Unable to get socket. Resetting!!!\r\n");
		RESET_SYSTEM();
	} 	
	(void)tcp_listen(TcpConfigSoch, TCP_CFG_PORT);  // listen on some port TCP_CFG_PORT


#ifdef   DNP_SLAVE_MODE

  // Puerto TCP  para Cliente DNP, envía los mensajes recibidos por puerto serie.
 	TcpCliTcpSoch = tcp_getsocket(TCP_TYPE_CLIENT, TCP_TOS_NORMAL, TCP_CLI_TOUT, TCP_CliTcpEventlistener);	

	if ( TcpCliTcpSoch < 0 ){
		DEBUGOUT("TCP: Unable to get socket. Resetting!!!\r\n");
		RESET_SYSTEM();
	}
	
//  if (tcp_getstate(TcpCliTcpSoch) != TCP_STATE_CONNECTED){
//      (void) tcp_connect (TcpCliTcpSoch, CliTcpIp, (UINT16)(CliTcpSockPort), 0);//(UINT16)(DNP_LOCAL_PORT) );
//  }

#endif  // DNP_SLAVE_MODE	

	TcpConfigState =  FALSE;
	
}


void TCP_StopWhileProc(void) 
{
  TcpWhileFlag = 0x00;
  return;  
}

#ifdef   DNP_MASTER_MODE

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// ····················································································
// Funciones de nexo entre SW Layer TCP_IP y SW Layer SrvTcp_TCP 
// ····················································································
//
// ---------------------------------------------------------------------------
// Recibir mensaje desde TCP para interpretar con protocolo SrvTcp
//
void TCP_AnalizeSrvTcpMsg (UINT32 len, UINT8 SocketIdx)
{
  UINT16 i;
  UINT8*  pTxBuffer;
  UINT8*  pTmpTxBuffer;
  UINT8   bTmpToken; 

  bTmpToken = TcpSockSrv[SocketIdx].BufferToken;
  if (MemGetBufferPtr( bTmpToken ,  (void**)(&pTxBuffer) ) != OK ){      
    (void)MemFreeBuffer (bTmpToken);   // Liberar por las dudas ? ABR: control de errores
    return;   
  }

/*
  // ABR: tratamiento de mensajes modbus con buffers de memoria
  if ( ERROR == MemGetBuffer ( (void**)(&pTxBuffer), &bTmpToken)) 
  {
      return; 
  } 
  TcpSockSrv[SocketIdx].BufferToken = bTmpToken;
*/
  
  pTmpTxBuffer = pTxBuffer;
//  pTmpTxBuffer = (UINT8*)&ModbusInMsg;
  for (i=0; i<len; i++){
    if(i > MAX_DNP_DATA)
        break;
    *pTmpTxBuffer =  GET_DATA();
    pTmpTxBuffer++;
  }

#if DEBUG_AT(1)
		  (void)sprintf((char *)udp_buf,"TCP_Data Len: %d, IP:%d \n",
		                (WORD)len,
		                (BYTE)TcpSockSrv[SocketIdx].SlaveIpAddress);
      udp_data_to_send = TRUE;
#endif  		

  TcpSockSrv[SocketIdx].TxRetries = 2;

  PutEventMsg (EVT_MOD_RXMSG, PROC_MODBUS, OS_NOT_PROC, SocketIdx);
 	
}



// ---------------------------------------------------------------------------
// TCP_RetransmitSrvTcpMsg : ReEnviar mensaje SrvTcp por puerto TCP
//
UINT8 TCP_RetransmitSrvTcpMsg(UINT8 SocketIdx)
{
  UINT16  i;
  UINT16  len;
  UINT8   TimerGetStateId;
  UINT8*  pTxBuffer;
  UINT8   bTmpToken; 
  UINT8   BufLen;

  if (tcp_getstate(TcpSockSrv[SocketIdx].TcpSrvTcpSoch) != TCP_STATE_CONNECTED){
        return (-1);
  }

  if (tcp_checksend(TcpSockSrv[SocketIdx].TcpSrvTcpSoch) < 0) 
  {
    TcpWhileFlag = 0x01;
    TimerGetStateId = TMR_SetTimer ( TCP_WAITFORSEND_TOUT , OS_NOT_PROC, TCP_StopWhileProc, FALSE);
    while (tcp_checksend(TcpSockSrv[SocketIdx].TcpSrvTcpSoch) < 0)
    {
       if (!TcpWhileFlag) return (-1);    // Cuando vence el timer se resetea TcpWhileFlag
    }
    TMR_FreeTimer ( TimerGetStateId );
  }

  bTmpToken = TcpSockSrv[SocketIdx].BufferToken;
  if (MemGetBufferPtr( bTmpToken ,  (void**)(&pTxBuffer) ) != OK ){      
    (void)MemFreeBuffer (bTmpToken);   // Liberar por las dudas ? ABR: control de errores
    return(-1);   
  }

  BufLen = *pTxBuffer;
  pTxBuffer++;

  len = 0;
  for( i=0 ; i < BufLen ; i++ ){
	  net_buf[TCP_APP_OFFSET+len] = *pTxBuffer;
	  pTxBuffer++;
	  len++;
	}

  if(tcp_send(TcpSockSrv[SocketIdx].TcpSrvTcpSoch, &net_buf[TCP_APP_OFFSET], NETWORK_TX_BUFFER_SIZE - TCP_APP_OFFSET, len) == len) {
	    DEBUGOUT("TCP: bytes reply sent!\r\n");
#if DEBUG_AT(1)
		  (void)sprintf((char *)udp_buf,"TCP_Sent Len: %d, IP:%d \n",
		                len,
		                (BYTE)TcpSockSrv[SocketIdx].SlaveIpAddress);
      udp_data_to_send = TRUE;
#endif
	}
  else {
      DEBUGOUT("TCP: error occured while trying to send data!\r\n");
	}

	return (1);
	
}




// ---------------------------------------------------------------------------
// TCP_SendSrvTcpMsg : Enviar mensaje SrvTcp por puerto TCP
//
//UINT8 TCP_SendSrvTcpMsg (void)
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
    TcpWhileFlag = 0x01;
    TimerGetStateId = TMR_SetTimer ( TCP_WAITFORSEND_TOUT , OS_NOT_PROC, TCP_StopWhileProc, FALSE);
    while (tcp_checksend(TcpSockSrv[SocketIdx].TcpSrvTcpSoch) < 0)
    {
       if (!TcpWhileFlag) return 0x01;    // Cuando vence el timer se resetea TcpWhileFlag
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
#if DEBUG_AT(1)
		  (void)sprintf((char *)udp_buf,"TCP_Sent Len: %d, IP:%d \n",
		                len,
		                (BYTE)TcpSockSrv[SocketIdx].SlaveIpAddress);
      udp_data_to_send = TRUE;
#endif
	}
  else {
      DEBUGOUT("TCP: error occured while trying to send data!\r\n");
	}

	return 0x00;
	
}


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
// TCP listen handler para mensajes ServerDNP
//
INT32 TCP_SrvTcpEventlistener(INT8 cbhandle, UINT8 event, UINT32 par1, UINT32 par2)
{
  UINT8 TmpIdx;
  UINT8*  pTxBuffer;
  UINT8   bTmpToken; 

//  par2++;	// to avoid the compiler warning

/* This function is called by TCP stack to inform about events	*/

  TmpIdx=0; 
  while (cbhandle != TcpSockSrv[TmpIdx].TcpSrvTcpSoch )
  {   
    TmpIdx++;
    if ( TmpIdx >= MAX_SRV_TCPSOCH ) {
#if DEBUG_AT(2)
		  (void)sprintf((char *)udp_buf,"DNP_TCP BadHandle: %d, H1:%d, H2:%d \n",
		                cbhandle ,
		                TcpSockSrv[0].TcpSrvTcpSoch ,
		                TcpSockSrv[1].TcpSrvTcpSoch );
      udp_data_to_send = TRUE;
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
      (void)MemFreeBuffer (TcpSockSrv[TmpIdx].BufferToken);   // Liberar el buffer
      TcpSockSrv[TmpIdx].BufferToken = 0xFF;

#if DEBUG_AT(1)
		  (void)sprintf((char *)udp_buf,"TCP_ABORT Idx: %d, IP:%d \n",
		                TmpIdx,
		                (BYTE)par1);
      udp_data_to_send = TRUE;      
#endif  		

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
  	  // Pedir buffer para el nuevo socket.
  	  if (TcpSockSrv[TmpIdx].BufferToken != 0xFF) {
          (void)MemFreeBuffer (TcpSockSrv[TmpIdx].BufferToken);   // Liberar el buffer
  	  }
      // ABR: tratamiento de mensajes modbus con buffers de memoria
      if ( ERROR == MemGetBuffer ( (void**)(&pTxBuffer), &bTmpToken)) 
      {
#if DEBUG_AT(1)
    		  (void)sprintf((char *)udp_buf,"ERROR MEMBUF para Idx: %d, IP:%d \n",
    		                TmpIdx,
    		                (BYTE)TcpSockSrv[TmpIdx].SlaveIpAddress);
          udp_data_to_send = TRUE;
#endif  		          
          PutEventMsg (EVT_LOG_ID, PROC_DEBUG, OS_NOT_PROC, (UINT16)LOG_ID_TCP_NOBUFF);    
          return(-1); 
      }  
      TcpSockSrv[TmpIdx].BufferToken = bTmpToken;
  	  
#if DEBUG_AT(1)
		  (void)sprintf((char *)udp_buf,"TCP_CONN Idx: %d, IP:%d \n",
		                TmpIdx,
		                (BYTE)TcpSockSrv[TmpIdx].SlaveIpAddress);
      udp_data_to_send = TRUE;
#endif  		

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
      (void)MemFreeBuffer (TcpSockSrv[TmpIdx].BufferToken);   // Liberar el buffer
      TcpSockSrv[TmpIdx].BufferToken = 0xFF;

#if DEBUG_AT(1)
		  (void)sprintf((char *)udp_buf,"TCP_CLOSE Idx: %d, IP:%d \n",
		                TmpIdx,
		                (BYTE)par1);
      udp_data_to_send = TRUE;
#endif  		
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
#if DEBUG_AT(1)
		  (void)sprintf((char *)udp_buf,"TCP_REGENERATE Idx: %d, IP:%d \n",
		                TmpIdx,
		                (BYTE)TcpSockSrv[TmpIdx].SlaveIpAddress);
      udp_data_to_send = TRUE;
#endif

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

      return (TCP_RetransmitSrvTcpMsg(TmpIdx));
			break;
	
		default:
			return(-1);
	}
	
	return (1);
}


#endif // DNP_MASTER_MODE


#ifdef   DNP_SLAVE_MODE

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// ····················································································
// Funciones de nexo entre SW Layer TCP_IP y SW Layer DNP_CLI_TCP 
// ····················································································
//

// ---------------------------------------------------------------------------
// Asegurar conexion
//
UINT8 TCP_VerifyCliTcpConnection(void)
{

  if (tcp_getstate(TcpCliTcpSoch) != TCP_STATE_CONNECTED){
  
      if ( tcp_connect (TcpCliTcpSoch, CliTcpIp, (UINT16)(CliTcpSockPort), 0 ) == (-1)){   //(UINT16)(DNP_LOCAL_PORT) 
        return 0x01;
      }
     
  } 
//  else{
//    tcp_sendcontrol(TcpCliTcpSoch);
//  }
  return 0x00;
}

// ---------------------------------------------------------------------------
// Enviar mensaje por puerto TCP con informacion de CliTcp
//
UINT8 TCP_SendCliTcpBuff(UINT8 * Buff, UINT16 BufLen)
{
  UINT16  i;
  UINT16  len;
  UINT8   TimerGetStateId;
  
//  CliTcpIpChar[0] = 192;
//  CliTcpIpChar[1] = 168;
//  CliTcpIpChar[2] = 0;
//  CliTcpIpChar[3] = 32;
 	   
  if (tcp_getstate(TcpCliTcpSoch) != TCP_STATE_CONNECTED){
  
      if ( tcp_connect (TcpCliTcpSoch, CliTcpIp, (UINT16)(CliTcpSockPort), 0 ) == (-1)){ // (UINT16)(DNP_LOCAL_PORT) 
        return 0x01;
      }
  }

  if (tcp_checksend(TcpCliTcpSoch) < 0) 
  {
    TcpWhileFlag = 0x01;
    TimerGetStateId = TMR_SetTimer ( TCP_WAITFORSEND_TOUT , OS_NOT_PROC, TCP_StopWhileProc, FALSE);
    while (tcp_checksend(TcpCliTcpSoch) < 0)
    {
       if (!TcpWhileFlag) return 0x01;    // Cuando vence el timer se resetea TcpWhileFlag
    }
    TMR_FreeTimer ( TimerGetStateId );
  }
  
  len = 0;
  for( i=0 ; i < BufLen ; i++ ){
	  net_buf[TCP_APP_OFFSET+len] = *Buff;
	  Buff++;
	  len++;
	}

  if(tcp_send(TcpCliTcpSoch, &net_buf[TCP_APP_OFFSET], NETWORK_TX_BUFFER_SIZE - TCP_APP_OFFSET, len) == len) {
	    DEBUGOUT("TCP: bytes reply sent!\r\n");
	}
  else {
      DEBUGOUT("TCP: error occured while trying to send data!\r\n");
	}

	return 0x00;
}

// ---------------------------------------------------------------------------
// Recibir mensaje desde TCP para interpretar con protocolo CliTcp
//
void TCP_AnalizeCliTcpMsg (UINT32 len)
{
  UINT16 i;
  UINT8*  pTmpTxBuffer;
  UINT8   bTmpToken; 

  (void)MemGetBuffer ( (void**)(&pTmpTxBuffer), &bTmpToken);

  for (i=0; i<len; i++){
    if(i > MAX_DNP_DATA)
        break;
    *pTmpTxBuffer =  GET_DATA();
    pTmpTxBuffer++;
  }

  if ( (SCIA_TransmitFrame ((UINT16)len, bTmpToken) ) == ERROR ){
    (void)MemFreeBuffer (bTmpToken);  
  }

}


// ---------------------------------------------------------------------------
// TCP listen handler para mensajes de CliTcp
//
INT32 TCP_CliTcpEventlistener(INT8 cbhandle, UINT8 event, UINT32 par1, UINT32 par2)
{
  	
// ABR: 23-MAR-2009.
//  par1++;	// to avoid the compiler warning
  par2++;	// to avoid the compiler warning

/* This function is called by TCP stack to inform about events	*/

	if( cbhandle != TcpCliTcpSoch)		/* Not our handle	*/
		return(-1);
	
	
	switch( event )
	{

		case TCP_EVENT_CONNECTED:
			break;
	
		case TCP_EVENT_CONREQ:
			DEBUGOUT("TCP: Connection request arrived!\r\n");
			break;
				
		case TCP_EVENT_DATA:
			DEBUGOUT("TCP: Data arrived!\r\n");
		  if(par1 <= 0)
		     break;
		  TCP_AnalizeCliTcpMsg(par1);
			break;
			
		case TCP_EVENT_REGENERATE:
			break;
	
		case TCP_EVENT_CLOSE:
      (void)TCP_VerifyCliTcpConnection();
			break;
					
		default:
			return(-1);
	}
	
	return (1);
}

#endif  //  DNP_SLAVE_MODE


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

#ifdef UDP_DEBUG_PRINTFON

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// ····················································································
// Funciones de nexo con SW Layer UDP_IP 
// ····················································································
//
void udp_analize_msg (UINT32 len)
{
  char i;

  for(i=0; i<len; i++){
    if(i > MAX_BUF_LEN) break;
    udp_buf[i] = GET_DATA();
    if (udp_buf[i] == '\n') break;
  }
/*  
  if (strcmp ((char *)udp_buf, "save_ip\n") == 0){
    eeprom_write_buffer(EE_ADDR_IP_DEBUG_ADDR, (UINT8 *)&udp_ipaddr, 4); 
    eeprom_write_buffer(EE_ADDR_IP_DEBUG_PORT, (UINT8 *)&udp_port, 2);  
  }
*/  
}


// ---------------------------------------------------------------------------

INT16 udp_send_buffer(void)
{
  UINT8 len;
	
  for(len=0 ; len<MAX_UDP_FRAME_LEN ; len++){
  	net_buf[UDP_APP_OFFSET+len] = udp_buf[len];
	  if(udp_buf[len] == '\n')
	    break;
	}
  len++;
  return(udp_send(udp_soch, udp_ipaddr, udp_port, &net_buf[UDP_APP_OFFSET], NETWORK_TX_BUFFER_SIZE -UDP_APP_OFFSET, len));

}

// ---------------------------------------------------------------------------

void UDP_Init (void)
{
	DEBUGOUT("Initializing UDP Echo client\r\n");

	udp_soch = udp_getsocket(0 , UPD_PanelEventlistener , UDP_OPT_SEND_CS | UDP_OPT_CHECK_CS);
	
	if(udp_soch == -1)
	    {
		DEBUGOUT("No free UDP sockets!! \r\n");
		RESET_SYSTEM();
	    }
	
	/* open socket */
	(void)udp_open(udp_soch, UDP_AK_PORT);
}

// ---------------------------------------------------------------------------

INT32 UPD_PanelEventlistener (INT8 cbhandle, UINT8 event, UINT32 ipaddr, UINT16 port, UINT16 buffindex, UINT16 datalen)
{
UINT8 tmpbyte1, tmpbyte2;
buffindex++;	// to avoid the compiler warning


if(cbhandle != udp_soch)
    {
	DEBUGOUT("UDP: not my handle!!!!");
	return (-1);
	}
	
switch(event)
    {
	case UDP_EVENT_DATA:

	    if(datalen <= 0)
            break;
	    udp_ipaddr = ipaddr;        /**< Remote IP address this application will send data to*/ 
	    udp_port = port;            /**< Port number on remote server we'll send data to */
	    tmpbyte1 = (BYTE)(udp_ipaddr>>8);
	    tmpbyte2 = (BYTE)(udp_ipaddr>>0);
	    udp_analize_msg(datalen);
 			(void)sprintf((char *)udp_buf,"UDP_Debug IP:%d.%d, Port:%d\n",
 			      tmpbyte1,
 			      tmpbyte2,
 			      udp_port);	
	    udp_data_to_send = TRUE;
	    break;
		
	default:
		DEBUGOUT("UDP: unknown UDP event :-(");
		break;
    }
return 0;
}

// ---------------------------------------------------------------------------
// UDP Demo app main loop that is periodically invoked from the main loop
//
void UDP_Loop(void)
{
if(udp_data_to_send) 
    {
	switch(udp_send_buffer())
	    {
		case -1:
			DEBUGOUT("Error (General error, e.g. parameters)\r\n");
			break; 
		case -2:
			DEBUGOUT("ARP or lower layer not ready, try again\r\n");
			break;
		case -3:
			DEBUGOUT("Socket closed or socket handle not valid!\r\n");
			break;
		default:
			// data sent (could check how many bytes too) if no more data to send, reset flag
        	udp_data_to_send = FALSE;
			break;
	    }	  
    }
}

#endif  // UDP_DEBUG_PRINTFON