
#ifndef dTCPH
#define dTCPH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define UDP_AK_PORT                 5000
#define TCP_DNPSRV_PORT             502
#define TCP_AK_PORT                 502
#define TCP_CFG_PORT                5000
#define TCP_SRL_PORT                5001
#define GET_DATA()                  RECEIVE_NETWORK_B()
#define MAX_BUF_LEN                 MAX_DEBUGBUF_LEN 
																		 
#define TCP_MODBUS_FLAG             0x01    // Bit indicador de conexion modbus establecida
//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//										 
SRV_TCP_SOCKET   TcpSockSrv[MAX_SRV_TCPSOCH];
INT8        TcpConfigSoch;						// Config Socket handle	
INT8        TcpSerialSoch;						// Serial Server Socket handle	
INT8        TcpCliTcpSoch;						// Cliente DNP Socket handle	
#ifdef UDP_DEBUG_PRINTFON
INT8        udp_soch;
UINT32      udp_ipaddr;
UINT16      udp_port;
UINT8       udp_buf[MAX_UDP_FRAME_LEN];
UINT8       udp_data_to_send;
#endif
UINT8       TcpConfigState;
UINT8       TcpWhileFlag;

UINT16      CliTcpSockPort;
UINT32      CliTcpIp;

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
// Locales..
INT32 TCP_SrvTcpEventlistener (INT8 , UINT8 , UINT32 , UINT32 );
INT32 TCP_ConfigEventlistener (INT8 , UINT8 , UINT32 , UINT32 );
INT32 TCP_SerialEventlistener (INT8 , UINT8 , UINT32 , UINT32 );
INT32 TCP_CliTcpEventlistener    (INT8 , UINT8 , UINT32 , UINT32 );

INT32 UPD_PanelEventlistener  (INT8 cbhandle, UINT8 event, UINT32 ipaddr, UINT16 port, UINT16 buffindex, UINT16 datalen);

// Globales..
void TCP_Init           (void);     
void UDP_Init           (void); 
void UDP_Loop           (void);

UINT8 TCP_SendCfgBuff   (void);

UINT8 TCP_SendSrvTcpBuff   (UINT8 * Buff, UINT16 BufLen, UINT8 SocketIdx);
UINT8 TCP_SendCliTcpBuff   (UINT8 * Buff, UINT16 BufLen);
UINT8 TCP_RetransmitSrvTcpMsg (UINT8 SocketIdx);

UINT8 TCP_VerifyCliTcpConnection(void);

#endif          // dTCPH
