#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <stdlib.h>
#include "debug.h"
#include "tcp_ip.h"
#include "MC9S12NE64.h"
#include "ne64debug.h"

#include "Commdef.h"
#include "dUdpDebug.h"
#include "Global.h"


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



  eeprom_read_buffer(EE_ADDR_UDPTRACE_IP, (UINT8 *)&udp_ipaddr, 4);
	//udp_ipaddr = localmachine.defgw;
  udp_port= 5001;
  //udp_ipaddr = 0xC0A80105; //192.168.1.5
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
      eeprom_write_buffer(EE_ADDR_UDPTRACE_IP, (UINT8 *)&udp_ipaddr, 4);
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