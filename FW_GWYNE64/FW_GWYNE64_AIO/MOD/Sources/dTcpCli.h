
#ifndef dTcpCliH
#define dTcpCliH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define GET_DATA()                  RECEIVE_NETWORK_B()
#define MAX_BUF_LEN                 MAX_DEBUGBUF_LEN 
																		 
#define TCP_MODBUS_FLAG             0x01    // Bit indicador de conexion modbus establecida
//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//										 
INT8        TcpCliTcpSoch;						// Cliente TCP Socket handle	
UINT8       TcpCliWhileFlag;

UINT16      CliTcpSockPort;
UINT32      CliTcpIp;

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
// Locales..
INT32 TCP_CliTcpEventlistener    (INT8 , UINT8 , UINT32 , UINT32 );

// Globales..
void TCP_InitCliSockets           (void);     

UINT8 TCP_SendCliTcpBuff   (UINT8 * Buff, UINT16 BufLen);
UINT8 TCP_VerifyCliTcpConnection(void);

#endif          // dTcpCliH
