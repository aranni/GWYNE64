#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <stdlib.h>
#include "debug.h"
#include "tcp_ip.h"
#include "MC9S12NE64.h"
#include "ne64debug.h"

#include "Commdef.h"
#include "dUdpCli.h"
#include "Global.h"


#ifdef UDP_CLIENT_MODE

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// ····················································································
// Funciones de nexo con SW Layer UDP_IP 
// ····················································································
//

void udp_cli_analize_msg (UINT32 len)
{
    char i;  
  
#if  (FW_SUBAPP_FLAG == FW_SUBAPP_MODBUS_SYMEO) // SYMEO

      UINT8*  pTxBuffer;
      UINT8   bTmpToken; 

      if(len > MEM_MAX_BUF_LEN)
        return;
 
      if ( ERROR == MemGetBuffer ( (void**)(&pTxBuffer), &bTmpToken)) 
      {
#if DEBUG_AT(DBG_LEVEL_1)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_1)
  {                    
    		  (void)sprintf((char *)udp_buf,"ERROR MEMBUF para UdpCli IP:%d \n",
    		                (UINT8)udp_cli_ipaddr);
          udp_data_to_send = TRUE;
  }
#endif  		          
          PutEventMsg (EVT_LOG_ID, PROC_DEBUG, OS_NOT_PROC, (UINT16)LOG_ID_TCP_NOBUFF);    
          MON_ResetIfNotConnected(0x31);    // Error de SW, Reset (TODO: revisar acciones a tomar)
          
          return;
      } 
             
      // Marcar el buffer con el ID del PROCESO DESTINO
      (void)MemSetRouteMark(bTmpToken, PROC_SYMEO);
   
      // Copiar los datos al buffer
      for(i=0; i<len; i++){
        if(i > MAX_BUF_LEN) break;
        pTxBuffer[i] = GET_DATA();
      }
  

        if ( MemSetUsedLen(bTmpToken, (UINT16)len) != OK){
    #if DEBUG_AT(DBG_LEVEL_3)
      if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
      {                    
      		  (void)sprintf((char *)udp_buf,"UdpSymeoErr:0xC7 \n");
            udp_data_to_send = TRUE;
      }
    #endif  	
           MON_ResetIfNotConnected(0XC7);    // Error de SW, Reset (TODO: revisar acciones a tomar)
           return;   
        } 
         
        PutEventMsg (EVT_SYMEOUDP_RXMSG, PROC_SYMEO, OS_NOT_PROC, bTmpToken);
        
    
#elif  (FW_APP_BUILT == FW_APP_BIO)			      // BIOMETRIC_ANVIZ + IO <-> UDP_MESSAGE + LOG_REPORT

    UINT16 sequence;  

    for(i=0; i<len; i++){
      if(i > MAX_BUF_LEN) break;
      udp_cli_buf[i] = GET_DATA();
      if (udp_cli_buf[i] == '\n') break;
    }
    
  //  if (strcmp ((char *)udp_cli_buf, ">ACK\0") == 0)
    {
      sequence = SYS_StrToInt ( 4, (UINT8*)&(udp_cli_buf[11]) );
      if (sequence)
        (void) FLA_ChangeStateEvent (sequence, EVENT_STATE_ACK);
    }

#endif  
  
  
}


// ---------------------------------------------------------------------------

INT16 udp_cli_send_buffer(void)
{
    UINT8 len;
  	
    for(len=0 ; len<MAX_UDP_FRAME_LEN ; len++){
    	net_buf[UDP_APP_OFFSET+len] = udp_cli_buf[len];
  	  if(udp_cli_buf[len] == '\n')
  	    break;
  	}
    len++;
    return(udp_send(udp_cli_soch, udp_cli_ipaddr, udp_cli_port, &net_buf[UDP_APP_OFFSET], NETWORK_TX_BUFFER_SIZE -UDP_APP_OFFSET, len));

}

// ---------------------------------------------------------------------------

INT32 UPD_CliPanelEventlistener (INT8 cbhandle, UINT8 event, UINT32 ipaddr, UINT16 port, UINT16 buffindex, UINT16 datalen)
{
  buffindex++;	// to avoid the compiler warning


  if(cbhandle != udp_cli_soch)
  {
  	DEBUGOUT("UDP: not my handle!!!!");
  	return (-1);
	}
	
  switch(event)
  {
    	case UDP_EVENT_DATA:

    	    if(datalen <= 0)
                break;
    	    udp_cli_ipaddr = ipaddr;        /**< Remote IP address this application will send data to*/ 
    	    udp_cli_port = port;            /**< Port number on remote server we'll send data to */
    	    udp_cli_analize_msg(datalen);
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
void UDP_CliLoop(void)
{
if(udp_cli_data_to_send) 
    {
	switch(udp_cli_send_buffer())
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
        	udp_cli_data_to_send = FALSE;
			break;
	    }	  
    }
}


//-----------------------------------------------------------------------------------------
// UDP_CliInit: inicializa socket
//-----------------------------------------------------------------------------------------
//
void UDP_CliInit (void)
{
  
  //UINT8       CliTcpIpHostAddr[20];
/*  
  UINT8   CliUdpIpAddrTxt[30];
  UINT8   CliUdpIpAddrIdx;
  UINT8   CliUdpIpCharIdx;
  UINT8   CliUdpIpChar[4];
  UINT8   TmpCntr;

  // el txt tiene address y puerto con formato xxx.xxx.xxx.xxx:ppp
 	eeprom_read_buffer( EE_ADDR_VER_TEXT, CliUdpIpAddrTxt, 30);

 	CliUdpIpAddrIdx=0; 
 	CliUdpIpCharIdx=0; 
// 	CliUdpIpHostAddr[sizeof(CliUdpIpHostAddr)-1] = '\0';
 	
 	for ( TmpCntr=0; TmpCntr<30 ; TmpCntr++){
 	
// 	  if (TmpCntr<sizeof(CliUdpIpHostAddr)){
// 	    CliUdpIpHostAddr[TmpCntr] = CliUdpIpAddrTxt[TmpCntr];
// 	    if (CliUdpIpHostAddr[TmpCntr] == ':') CliUdpIpHostAddr[TmpCntr] = '\0'; 	    
// 	  } 
 	  if (CliUdpIpAddrTxt[TmpCntr] == '.'){
 	    CliUdpIpAddrTxt[TmpCntr] = '\0';
 	    CliUdpIpChar[CliUdpIpCharIdx++] = (UINT8)(atoi((INT8*)&(CliUdpIpAddrTxt[CliUdpIpAddrIdx])));
 	    CliUdpIpAddrIdx = TmpCntr+1;
 	  } 
 	  if (CliUdpIpAddrTxt[TmpCntr] == ':'){
 	    CliUdpIpAddrTxt[TmpCntr] = '\0';
 	    CliUdpIpChar[CliUdpIpCharIdx++] = (UINT8)(atoi((INT8*)&(CliUdpIpAddrTxt[CliUdpIpAddrIdx])));
 	    CliUdpIpAddrIdx = TmpCntr+1;
 	    break;
 	  }
// 	  if (CliUdpIpAddrTxt[TmpCntr] == '\0') break;
 	}
  udp_cli_ipaddr = *((UINT32 *)CliUdpIpChar); 
  udp_cli_port = atoi((INT8*)&(CliUdpIpAddrTxt[CliUdpIpAddrIdx]));
  
	DEBUGOUT("Initializing UDP Echo client\r\n");
	
*/ 	

  udp_cli_ipaddr = 0xFFFFFF; 
  udp_cli_port = 9001;

	udp_cli_soch = udp_getsocket(0 , UPD_CliPanelEventlistener , UDP_OPT_SEND_CS | UDP_OPT_CHECK_CS);
	
	if(udp_cli_soch == -1)
	{
  		DEBUGOUT("No free UDP sockets!! \r\n");
  		RESET_SYSTEM();
	}
	
	/* open socket */
	(void)udp_open(udp_cli_soch, UDP_CLI_PORT);
	
}



#endif  // UDP_CLIENT_MODE