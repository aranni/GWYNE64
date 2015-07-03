#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <stdlib.h>
#include "debug.h"
#include "tcp_ip.h"
#include "MC9S12NE64.h"
#include "ne64debug.h"

#include "Commdef.h"
#include "dTcpCli.h"
#include "Global.h"

#ifdef  TCP_SLAVE_MODE          // La aplicación necesita un cliente TCP

#define   TCP_SRV_TOUT	  120
#define   TCP_CLI_TOUT	  120

#define   TCP_WAITFORSEND_TOUT  500     // Timer esperando que se pueda enviar mensaje por tcp


//-----------------------------------------------------------------------------------------
// TCP_Init: inicializa todos los sockets
//-----------------------------------------------------------------------------------------
//
void TCP_InitCliSockets (void)
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
 	
	DEBUGOUT("Initializing TCP application. \r\n");
	
  // Puerto TCP  para Cliente DNP, envía los mensajes recibidos por puerto serie.
 	TcpCliTcpSoch = tcp_getsocket(TCP_TYPE_CLIENT, TCP_TOS_NORMAL, TCP_CLI_TOUT, TCP_CliTcpEventlistener);	

	if ( TcpCliTcpSoch < 0 ){
		DEBUGOUT("TCP: Unable to get socket. Resetting!!!\r\n");
		RESET_SYSTEM();
	}
	
//  if (tcp_getstate(TcpCliTcpSoch) != TCP_STATE_CONNECTED){
//      (void) tcp_connect (TcpCliTcpSoch, CliTcpIp, (UINT16)(CliTcpSockPort), 0);//(UINT16)(DNP_LOCAL_PORT) );
//  }
	
}


void TCP_StopCliWhileProc(void) 
{
  TcpCliWhileFlag = 0x00;
  return;  
}


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
    TcpCliWhileFlag = 0x01;
    TimerGetStateId = TMR_SetTimer ( TCP_WAITFORSEND_TOUT , OS_NOT_PROC, TCP_StopCliWhileProc, FALSE);
    while (tcp_checksend(TcpCliTcpSoch) < 0)
    {
       if (!TcpCliWhileFlag) return 0x01;    // Cuando vence el timer se resetea TcpCliWhileFlag
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


#endif  //  TCP_SLAVE_MODE    La aplicación necesita un cliente TCP
