
#ifndef dTCPH
#define dTCPH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define MAX_UDP_FRAME_LEN           20
#define UDP_AK_PORT                 5000
#define TCP_AK_PORT                 502
#define TCP_CFG_PORT                5000
#define GET_DATA()                  RECEIVE_NETWORK_B()
#define MAX_BUF_LEN                 MAX_DEBUGBUF_LEN 


//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
INT8        tcp_soch;								  // Socket handle
INT8        TcpConfigSoch;						// Config Socket handle	
INT8        udp_soch;
UINT32      udp_ipaddr;
UINT16      udp_port;
UINT8       udp_buf[MAX_UDP_FRAME_LEN];
UINT8       udp_data_to_send;
UINT8       TcpConfigState;

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
// Locales..
INT32 TCP_ModbusEventlistener (INT8 , UINT8 , UINT32 , UINT32 );
INT32 TCP_ConfigEventlistener (INT8 , UINT8 , UINT32 , UINT32 );
INT32 UPD_PanelEventlistener  (INT8 cbhandle, UINT8 event, UINT32 ipaddr, UINT16 port, UINT16 buffindex, UINT16 datalen);

// Globales..
void TCP_Init           (void);     
void UDP_Init           (void); 
void UDP_Loop           (void);

UINT8 TCP_SendModbusMsg (void);
UINT8 TCP_SendCfgBuff   (void);

#endif          // dTCPH
