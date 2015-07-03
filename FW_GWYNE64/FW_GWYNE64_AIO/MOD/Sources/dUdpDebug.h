
#ifndef dUDPH
#define dUDPH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define UDP_AK_PORT                 5000
#define GET_DATA()                  RECEIVE_NETWORK_B()
#define MAX_BUF_LEN                 MAX_DEBUGBUF_LEN 
																		 
//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//										 
#ifdef UDP_DEBUG_PRINTFON
INT8        udp_soch;
UINT32      udp_ipaddr;
UINT16      udp_port;
UINT8       udp_buf[MAX_UDP_FRAME_LEN];
UINT8       udp_data_to_send;
#endif

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
INT32 UPD_PanelEventlistener  (INT8 cbhandle, UINT8 event, UINT32 ipaddr, UINT16 port, UINT16 buffindex, UINT16 datalen);

// Globales..
void UDP_Init           (void); 
void UDP_Loop           (void);

#endif          // dUDPH
