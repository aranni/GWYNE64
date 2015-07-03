
#ifndef dTcpSrvH
#define dTcpSrvH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define TCP_CFG_PORT                5000
#define GET_DATA()                  RECEIVE_NETWORK_B()
#define GET_DATA_BYTES()            RECEIVE_NETWORK_B()
#define MAX_BUF_LEN                 MAX_DEBUGBUF_LEN 
																		 
//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//										 
SRV_TCP_SOCKET   TcpSockSrv[MAX_SRV_TCPSOCH];
INT8        TcpConfigSoch;						// Config Socket handle	
UINT8       TcpConfigState;
UINT8       TcpSrvWhileFlag;

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
// Locales..
INT32 TCP_SrvTcpEventlistener (INT8 , UINT8 , UINT32 , UINT32 );
INT32 TCP_ConfigEventlistener (INT8 , UINT8 , UINT32 , UINT32 );

// Globales..
void TCP_InitSrvSockets           (void);     

UINT8 TCP_SendCfgBuff   (void);

UINT8 TCP_SendSrvTcpBuff   (UINT8 * Buff, UINT16 BufLen, UINT8 SocketIdx);
UINT8 TCP_RetransmitSrvTcpMsg (UINT8 SocketIdx);

#endif          // dTcpSrvH
