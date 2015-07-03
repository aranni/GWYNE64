#include <stddef.h>
#include <string.h>

#include "debug.h"
#include "tcp_ip.h"
#include "MC9S12NE64.h"
#include "ne64debug.h"

#include "Commdef.h"
#include "dTCP.h"
#include "Global.h"


// ---------------------------------------------------------------------------
// Recibir mensaje desde TCP para interpretar con protocolo Modbus
// ---------------------------------------------------------------------------
//
void TCP_AnalizeModbusMsg (UINT32 len)
{
  char i;
  UINT8 * pInMsg;
  pInMsg = (UINT8 *)(&ModbusInMsg);

  for (i=0; i<len; i++){
    if(i > MAX_TCP_DATA)
        break;
    *pInMsg =  GET_DATA();
    pInMsg++;
  }

  PutEventMsg (EVT_MOD_RXMSG, PROC_MODBUS, OS_NOT_PROC, 0x00);
 	
}


// ---------------------------------------------------------------------------
// TCP_SendModbusMsg : Enviar mensaje Modbus por puerto TCP
// ---------------------------------------------------------------------------
//
UINT8 TCP_SendModbusMsg (void)
{
  char len;
  UINT8 * pOutMsg;
  pOutMsg = (UINT8 *)(&ModbusOutMsg);
	
/* first check if data sending is possible (it may be that
 * previously sent data is not yet acknowledged)
 */
  if (tcp_checksend(tcp_soch) < 0) return 0x01;
    ;       /* Not yet */
/* put message in buffer. Message needs to start from TCP_APP_OFFSET
 * because TCP/IP stack will put headers in front of the message to
 * avoid data copying
 */

  for ( len=0 ; len < ((ModbusOutMsg.LengthLow) + 6) ; len++ ){
	  net_buf[TCP_APP_OFFSET+len] = *pOutMsg;
	  pOutMsg++;
	}
	
  if (tcp_send(tcp_soch, &net_buf[TCP_APP_OFFSET], NETWORK_TX_BUFFER_SIZE - TCP_APP_OFFSET, len) == len){
	    DEBUGOUT("TCP: bytes reply sent!\r\n");
	}
  else {
      DEBUGOUT("TCP: error occured while trying to send data!\r\n");
	    return 0x02;
	}
	
	return 0x00;
}



// ---------------------------------------------------------------------------

void TCP_Init (void)
{
	
	DEBUGOUT("Initializing TCP application. \r\n");
	
	/* Get socket:
	 * 	TCP_TYPE_SERVER - type of TCP socket is server
	 * 	TCP_TOS_NORMAL  - no other type of service implemented so far
	 *	TCP_DEF_TOUT	- timeout value in seconds. If for this many seconds
	 *		no data is exchanged over the TCP connection the socket will be
	 *		closed.
	 *	tcps_control_panel_eventlistener - pointer to event listener function for
	 * 		this socket.	
	 */
	
	tcp_soch = tcp_getsocket(TCP_TYPE_SERVER, TCP_TOS_NORMAL, TCP_DEF_TOUT, TCP_ModbusEventlistener);
	
	if ( tcp_soch < 0 ){
		DEBUGOUT("TCP: Unable to get socket. Resetting!!!\r\n");
		RESET_SYSTEM();
	}
	 	
	(void)tcp_listen(tcp_soch, TCP_AK_PORT);  // listen on some port TCP_AK_PORT

	TcpConfigSoch = tcp_getsocket(TCP_TYPE_SERVER, TCP_TOS_NORMAL, TCP_DEF_TOUT, TCP_ConfigEventlistener);
	
	if ( TcpConfigSoch < 0 ){
		DEBUGOUT("TCP: Unable to get socket. Resetting!!!\r\n");
		RESET_SYSTEM();
	}
	 	
	(void)tcp_listen(TcpConfigSoch, TCP_CFG_PORT);  // listen on some port TCP_CFG_PORT
	TcpConfigState =  FALSE;

	
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
// TCP listen handler para mensajes MODBUS
// ---------------------------------------------------------------------------
//
INT32 TCP_ModbusEventlistener(INT8 cbhandle, UINT8 event, UINT32 par1, UINT32 par2)
{
par2++;	// to avoid the compiler warning

/* This function is called by TCP stack to inform about events	*/

	if( cbhandle != tcp_soch)		/* Not our handle	*/
		return(-1);
	
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
			break;
		
		/* Connection established event - three-way handshaking performed
		 * OK and connection is established.
		 * Parameters:
		 *  - par1 - remote hosts IP address
		 *  - par2 - remote hosts port number
		 */
		case TCP_EVENT_CONNECTED:
			DEBUGOUT("TCP: TCP connection established!\r\n");
		
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
		  TCP_AnalizeModbusMsg(par1);
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
        if ( TCP_SendModbusMsg() )
          DEBUGOUT("TCP: Error intentando regenerar paquete perdido\r\n");
			break;
	
		default:
			return(-1);
	}
}


// ---------------------------------------------------------------------------
// Recibir mensaje desde TCP con informacion de configuracion
// ---------------------------------------------------------------------------
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
    
  PutEventMsg (EVT_CFG_RXMSG, PROC_CONFIG, OS_NOT_PROC, 0x00);
    
}


// ---------------------------------------------------------------------------
// Enviar mensaje por puerto TCP con informacion de configuracion
// ---------------------------------------------------------------------------
//
UINT8 TCP_SendCfgBuff(void)
{
  UINT8 i;
  UINT16 len;
  UINT8 * pOutCfgMsg;
  pOutCfgMsg = (UINT8 *)(&CfgMsgFrame);
	 	   
//  while(tcp_checksend(TcpConfigSoch) < 0);
  if (tcp_checksend(TcpConfigSoch) < 0) return 0x01;

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
// ---------------------------------------------------------------------------
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


///////////////////////////////////////////////////////////////////////////////
//    UPD
///////////////////////////////////////////////////////////////////////////////
//
void udp_analize_msg(UINT32 len)
{
char i;
//UINT8 * pInMsg;
//pInMsg = (UINT8 *)(&ModbusInMsg);

for(i=0; i<len; i++)
    {
    if(i > MAX_BUF_LEN) break;
    udp_buf[i] = GET_DATA();
//    *pInMsg = DebugBuffer[i];
//    pInMsg++;
    }
    	
}

// ---------------------------------------------------------------------------

INT16 udp_send_buffer(void)
{
char len;
	
for(len=0;;)
    {
	net_buf[UDP_APP_OFFSET+len] = udp_buf[len];
	if(udp_buf[len++] == '\n')
	    break;
	}
	
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
	    udp_analize_msg(datalen);
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
// ---------------------------------------------------------------------------
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

