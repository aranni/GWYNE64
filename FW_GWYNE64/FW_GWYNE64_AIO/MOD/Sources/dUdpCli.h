
#ifndef dUDPCLIH
#define dUDPCLIH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define UDP_CLI_PORT                3046
#define GET_DATA()                  RECEIVE_NETWORK_B()
#define MAX_BUF_LEN                 MAX_DEBUGBUF_LEN 
																		 
//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//										 

INT8        udp_cli_soch;
UINT32      udp_cli_ipaddr;
UINT16      udp_cli_port;
UINT8       udp_cli_buf[MAX_UDP_FRAME_LEN];
UINT8       udp_cli_data_to_send;

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
INT32 UPD_CliPanelEventlistener  (INT8 cbhandle, UINT8 event, UINT32 ipaddr, UINT16 port, UINT16 buffindex, UINT16 datalen);

// Globales..
void UDP_CliInit           (void); 
void UDP_CliLoop           (void);

#endif          // dUDPCLIH
