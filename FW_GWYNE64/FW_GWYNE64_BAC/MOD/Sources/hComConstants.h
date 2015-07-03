
/* To Clear or Set single bits in a byte variable */
//#define	b_SetBit(bit_ID, varID)		(varID |= (unsigned char)(1<<bit_ID))
//#define	b_ClearBit(bit_ID, varID)	(varID &= ~(unsigned char)(1<<bit_ID))
//#define	b_XorBit(bit_ID, varID)		(varID ^= (unsigned char)(1<<bit_ID))

//-----------------------------------------------------------------------------
//  DEFINICIONES DE MACROS
//-----------------------------------------------------------------------------
//
#define	FOREVER			  for(;;)
#define	MAXIMO(x,y)		(x>y)?x:y
#define	MINIMO(x,y)		(x<y)?x:y
#define	LOOP(x,y)		  for(x=y; x>0; x--)

#define SETBIT(x,y)   x|=(1L<<y)
#define CLRBIT(x,y)   x&=~(1L<<y)
#define BITMASK(X)    (1L << (X))


//-----------------------------------------------------------------------------
//  DEFINICIONES DE CONSTANTES
//-----------------------------------------------------------------------------
//
#define SCI0    0x00C8
#define SCI1    0x00D0

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

#define NON_STACKTOP        0x3FFF

#define	TMR_MAX_NODES		        20    // Cantidad de timers disponibles
#define MAX_EVENTMSG_QUEUE	    30		// Capacidad de la cola de mensajes de eventos
//#define MAX_SERIAL_IN_BUFLEN    50   // Capacidad del buffer de entrada de puertos serie
//#define MAX_SERIAL_OUT_BUFLEN   50   // Capacidad del buffer de salida de puertos serie
#define MAX_DEBUGBUF_LEN        50   // Capacidad del buffer de debug, (y mensajes a TCP) 
#define MAX_UDP_FRAME_LEN       128  // Buffer udp para printfs

#define DBG_BY_TCP            0x01   // Enviar mensaje de debug por TCP
#define DBG_BY_SERIAL         0x02   // Enviar mensaje de debug por puerto serie.

#define STX  (UINT8)0x02
#define EOT  (UINT8)0x0D

#define ERROR (UINT8)(-1)
#define OK    (UINT8)(0x00)

// Selección de señales para RX del SCI
#define  MUX_SEL_RS485  0             
#define  MUX_SEL_RS232  1


//--FLAGS DE SISTEMA-----------------------------------------------------------
//
#define   SYS_FLAG_SCIADOWN     0x01    // Caida en la comunicacion serie por SCIA


//--TIMERS GLOBALES------------------------------------------------------------
//
#define	TCP_TIMEOUT_BUSYSENDRETRY   200   // Espera para reintentar envío por TCP
#define	TCP_TIMEOUT_SENDRETRY   80   // Espera para reintentar envío por TCP


//--MACROS PARA CONTROL DE LEDS------------------------------------------------
//
#define LED_RED_PORT   PORTA
#define LED_RED_CFG    DDRA
#define LED_RED_MASK   0x80    

#define LED_GREEN_PORT   PTL
#define LED_GREEN_CFG    DDRL				
#define LED_GREEN_MASK   0x04    

#define LED_BICOLOR

#ifdef LED_BICOLOR

#define  LED_RED_ON     {(LED_RED_PORT  |= LED_RED_MASK);\
                         (LED_GREEN_PORT  &= ~(LED_GREEN_MASK));}
                         
#define  LED_GREEN_ON   {(LED_GREEN_PORT  |= LED_GREEN_MASK);\
                         (LED_RED_PORT  &= ~(LED_RED_MASK));}
												
#define  LED_OFF        {(LED_GREEN_PORT  &= ~(LED_GREEN_MASK));\
                         (LED_RED_PORT  &= ~(LED_RED_MASK));}

#define  LED_RED_OFF    (LED_RED_PORT  &= ~(LED_RED_MASK))
#define  LED_RED_TOG    (LED_RED_PORT  ^= LED_RED_MASK)
#define  LED_GREEN_OFF  (LED_GREEN_PORT  &= ~(LED_GREEN_MASK))
#define  LED_GREEN_TOG  (LED_GREEN_PORT  ^= LED_GREEN_MASK)

#else   // LED_BICOLOR

#define  LED_GREEN_ON    {(LED_RED_PORT  |= LED_RED_MASK);\
                         (LED_GREEN_PORT  &= ~(LED_GREEN_MASK));}
                         
#define  LED_RED_ON     {(LED_GREEN_PORT  |= LED_GREEN_MASK);\
                         (LED_RED_PORT  |= LED_RED_MASK);}

#define  LED_OFF        {(LED_GREEN_PORT  |= LED_GREEN_MASK);\
                         (LED_RED_PORT  |= LED_RED_MASK);}
												

#define  LED_RED_OFF    {(LED_GREEN_PORT  |= LED_GREEN_MASK);\
                         (LED_RED_PORT  |= LED_RED_MASK);}

#define  LED_GREEN_OFF  {(LED_GREEN_PORT  |= LED_GREEN_MASK);\
                         (LED_RED_PORT  |= LED_RED_MASK);}


#endif  // LED_BICOLOR


//-----------------------------------------------------------------------------
//  DEFINICIONES DE ENUMERACIONES
//-----------------------------------------------------------------------------

// -------------------------------------------------------------------------
// PROCESOS
//
typedef enum {
	PROC_DEBUG	= 0		,
	PROC_MODBUS    		,   // Modbus TCP
	PROC_SCIA     		,   // Driver puerto de comunicación serie SCIA
	PROC_SCIB         ,   // Driver puerto de comunicación serie SCIB
	PROC_CONFIG       ,		// Configuración: maneja la comunicación con GWY_Config
	PROC_IIC          ,		// Driver puerto de comunicación I2C
	PROC_BAC          ,		// Manejador de protocolo propietario de sensores BAC		

	PROCESS_QTTY 		,
	OS_NOT_PROC			,
	
	PROC_NAMES_FILL	= 0xFF
//	PROC_MODRTU       ,
} PROCESS_NAMES_ENUM;

// -------------------------------------------------------------------------
// EVENTOS
//
typedef enum {
	EVT_NONE		 	= 0x00	,
	EVT_TMR_TIMEOUT				,						// Timeout requerido
	EVT_DBG_TXMSG         ,						// Transmitir mensaje de debug
  EVT_DBG_TXPRN         ,           // Enviar Debug Printf en Message[]
	EVT_MOD_RXMSG         ,           // Mensaje MODBUS recibido
	EVT_485_RXMSG         ,           // Mensaje recibido del variador
	EVT_485_TXMSG         ,           // Mensaje Transmitido al variador
	EVT_SIE_RXFRAME       ,           // Mensaje para variador SIEMENS
	EVT_485_TXREQ         ,           // Transmitir requerimiento al variador
	EVT_485_RXREQ         ,
	EVT_CFG_RXMSG         ,           // Mensaje de configuración recibido via TCP
	EVT_VAR_CMDSTART      ,           // Comando de Start al variador 
	EVT_VAR_CMDSTOP       ,           // Comando de Stop al variador 
	EVT_VAR_CMDSETFREC    ,           // Comando de Seteo de frecuencia
	EVT_VAR_CMDRESETFAULT ,           // Comando de reset de falla
	EVT_VAR_CMDDEBUG      ,           // Comando de debug..
	EVT_232_RXMSG         ,           // Mensaje recibido del puerto serie RS232
	EVT_232_TXMSG         ,           // Mensaje Transmitido por puerto serie RS232
	EVT_DVR_EXCEPTION     ,           // Excepción de driver de comunicaciones
	
	EVT_DNP_RXFRAME       ,           // Mensaje recibido para DNP
	EVT_DNP_POLLREQ       ,           // Polling con mensaje de Request 
	EVT_DNP_SETOUTON      ,           // Comando para setear una salida en ON
	EVT_DNP_SETOUTOFF     ,           // Comando para setear una salida en OFF
	
	EVT_SERIAL_OUTREQ     ,           // Datos encolados para salida por puerto serie
	EVT_SERIAL_INREQ      ,           // Datos encolados para salida por puerto TCP
	EVT_SERIAL_POLREQ     ,           // Polling cíclico para proceso Serial Server
	
	EVT_ETH_RETRY         ,           // Retry de send por tcp client, porque no estaba connectado
	EVT_CPY_TXMSG         ,           // Mensaje de copia retransmitido por puerto serie RS232

	EVT_LOG_SAVE         ,           // Grabar un log de eventos.
	EVT_LOG_ID           ,           // Grabar un log de eventos.
	
	EVT_OTHER_MSG	= 0xFF
} EVENT_NAMES_ENUM;

// -------------------------------------------------------------------------
// CODIGOS DE COMANDOS A LOS VARIADORES
//
typedef enum {
	CMD_VAR_NONE			= 0x00	,
	CMD_VAR_START   					,
	CMD_VAR_STOP      				,
	CMD_VAR_SETFREC   				,
	CMD_VAR_SETRELTX          ,     // Setear valor de relacion de transmision
	CMD_VAR_SETCRESB          ,     // Setear valor de coef. de resbalamiento
	CMD_VAR_SETRPMOT          ,			// Setear valor de RPM del motor
  CMD_VAR_RESETFAULT        ,     // Resetear falla en el variador
  CMD_VAR_SETAAG_A          ,     // Modificar el valor de salida aag.1
  CMD_VAR_SETAAG_B          ,     // Modificar el valor de salida aag.2
  
  CMD_VAR_CLEARSTAT = 200   ,     // Limpiar variables de estadisticas
  CMD_VAR_DEBUG             ,     // Debug
	CMD_VAR_FILL = 0xFF
} CMD_VAR_ENUM;

// -------------------------------------------------------------------------
// Excepciones reportadas por Drivers de comunicaciones
//
typedef enum _DRIVER_EXCEPTIONS{
    DRV_NONE        = 0x00  ,
    DRV_CHANNEL_UP          ,
    DRV_CHANNEL_DOWN        ,
    DRV_READ_TIMEOUT        ,
    DRV_WRITE_TIMEOUT       ,
    DRV_READ_ERROR          ,
    DRV_WRITE_ERROR         ,
    DRV_FATAL_DRV_ERROR
}DRIVER_EXCEPTIONS;

// -------------------------------------------------------------------------
// Excepciones reportadas por Drivers de comunicaciones
//
typedef enum _CHANNEL_STATUS{
    STATUS_NONE        = 0x00  ,
    STATUS_DISABLED         ,
    STATUS_LISTENING        ,
    STATUS_ACTIVE           ,
    STATUS_CLOSING          ,
    STATUS_UNKNOWN = 0xFF 
}CHANNEL_STATUS;


//-----------------------------------------------------------------------------
//  CODIGOS DE OPERACION Y DIRECCIONES FIJAS.
//-----------------------------------------------------------------------------
//
//--DIRECCIONES DE CAMPOS EN EEPROM--------------------------------------------
//
//--Datos de parametrización del canal TCP-IP
#define EE_ADDR_VERS            0
#define EE_ADDR_MAC             1
#define EE_ADDR_IP_ADDR         7
#define EE_ADDR_IP_SUBNET       11
#define EE_ADDR_IP_GATEWAY      15

#define EE_ADDR_DNS_ENABLE      19
#define EE_ADDR_IP_DNS          20

#define EE_ADDR_TCP_INITOUT     24
#define EE_ADDR_TCP_TOUT        26
#define EE_ADDR_TCP_RETRIES     28

#define EE_ADDR_IP_DEBUG_ADDR   30        // Direccion IP donde enviar printfs de debug
#define EE_ADDR_IP_DEBUG_PORT   34        // Puerto UDP donde enviar printfs de debug

//--Información estadística especial.
#define EE_ADDR_RST_FLAG        40
#define EE_ADDR_RST_CNTR        42
#define EE_ADDR_DOG_CNTR        44


//-- Información de configuación específica..
#define EE_ADDR_FW_UPDATE				50				// Flag para control de upgrade de FW
#define EE_ADDR_RUN_CFG         52        // Estado de ejecución de procesos
#define EE_ADDR_TRIAL_CODE      54        // Tipo de versión trial en caso que aplique
#define EE_ADDR_LIMIT_FROMSRL   60        // Desde Serial Nr.
#define EE_ADDR_LIMIT_TOSRL     70        // Hasta Serial Nr.
#define EE_ADDR_LIMIT_MSGQTTY   80        // Limit por cantidad de MSG
#define EE_ADDR_LIMIT_TIME      84        // Limit por tiempo
#define EE_ADDR_LIMIT_OTHER     88        // Limit Otro aun no especificado

// Información sobre la comunicación con el variador Canal_A
#define EE_ADDR_INV_FREC        100       // Consigna de frecuencia
#define EE_ADDR_INV_POLL        104				// ScanRate al variador
#define EE_ADDR_INV_TYPE        106       // Modelo de Variador 
#define EE_ADDR_PORT_CFG        108       // Estado de ejecución de procesos

#define EE_ADDR_SIE_CMDWA       110				// Palabra de control Variador Siemens
#define EE_ADDR_CFG_SCIA        112				// Configuracion del puerto SCIA (reservar 113)
#define EE_ADDR_NODE_SCIA       114				// Configuracion de direccion de esclavo SCIA 

#define EE_ADDR_DNP_MASTER      120				// Dirección de master para Cx DNP3 por SCIA
#define EE_ADDR_DNP_SLAVE       122				// Dirección de slave  para Cx DNP3 por SCIA


// Información sobre la comunicación con el variador Canal_B
#define EE_ADDR_INV_FREC_B      150       // Consigna de frecuencia
#define EE_ADDR_INV_POLL_B      154				// ScanRate al variador
#define EE_ADDR_INV_TYPE_B      156       // Modelo de Variador 
#define EE_ADDR_PORT_CFG_B      158       // Estado de ejecución de procesos

#define EE_ADDR_SIE_CMDWA_B     160				// Palabra de control Variador Siemens
#define EE_ADDR_CFG_SCIB        162				// Configuracion del puerto SCIB (reservar 163)
#define EE_ADDR_NODE_SCIB       164				// Configuracion de direccion de esclavo SCIB 

// Datos de la VERSION..
#define EE_ADDR_VER_HWVER       200       // Version de HW
#define EE_ADDR_VER_SERIAL      220       // Numero de serie
#define EE_ADDR_VER_TEXT        240       // Descripcion de usuario

// Constantes almacenadas.
#define EE_ADDR_PAR_RELTX       300        // Relacion de transmision
#define EE_ADDR_PAR_CRESB       304        // Coeficiente de resbalamiento
#define EE_ADDR_PAR_RPMOT       308        // RPM en motor

#define EE_ADDR_PAR_ESCEN       350        // Escenario
#define EE_ADDR_PAR_PUESTO      352        // Puesto de trabajo


#define EE_ADDR_BAC_STATE       398        // Estado de los bancos almacenados. 
// Para TransferInfo desde el LOADER
#define EE_ADDR_LDR_INFO        400     
#define EE_ADDR_BAC_BANKS       400			   // Uso el mismo para almacenar el mapeo de bancos,
                                           // Podría generarse una versió de FW que tenga
                                           // Incorporada esta información..  



//--DIRECCIONES DE CAMPOS EN NVR DEL RTC---------------------------------------
#define NVR_ADDR_LOGSPTR        0x20      // Punteros de logs en FLASH 3x2bytes
#define NVR_ADDR_FIRMA          0x26      // Firma para control de integridad 4bytes
#define NVR_VAL_FIRMA           0xCAFECAFE  



//--Codigos de comandos de operacion recibidos por TCP----------------------------
#define CFG_TCP_READ          1
#define CFG_TCP_WRITE         2
#define CFG_TCP_RESET         3
#define CFG_TCP_READ_STAT     4
#define CFG_TCP_READ_VER      5
#define CFG_TCP_WRITE_VER     6
#define CFG_TCP_READ_CTES     7
#define CFG_TCP_WRITE_CTES    8
#define CFG_TCP_WRITE_DATE    9

#define CFG_TCP_ACK           10
#define CFG_TCP_NAK           11
#define CFG_TCP_POL           12

#define CFG_TCP_READ_EEPROM   13
#define CFG_TCP_WRITE_EEPROM  14

#define CFG_TCP_READ_FLASH    15
#define CFG_TCP_WRITE_FLASH   16

#define CFG_TCP_UPGRADE_FW    17      // Cargar FW transferido
#define CFG_TCP_VALIDATE_FW   18      // Validar FW cargado 
#define CFG_TCP_RESTORE_FW    19      // Recupera el FW de fábrica.

#define CFG_TCP_GET_IOVALUES  30      // Lectura de I/O
#define CFG_TCP_SET_IOVALUES  31      // Escritura de I/O

#define CFG_TCP_WRITE_FLASH256  32    // Escribir una página de 256 bytes en la flash     

#define CFG_TCP_READ_DNP        40    // Lectura de configuracion DNP
#define CFG_TCP_WRITE_DNP       41    // Escritura de configuracion DNP

#define CFG_TCP_GET_SCICONFIG   42      // Lectura de Configuracion de puertos SCI
#define CFG_TCP_SET_SCICONFIG   43      // Escritura de Config. de puertos SCI
																
#define STATUS_READY_BIT    0x01
#define STATUS_RUN_BIT      0x02
#define STATUS_FAULT_BIT    0x04

#define STATUS_SCIA_FAIL_BIT  0x80

#define CFG_RTC_RUNSTATE_MASK   0x01    
#define CFG_RTU_RUNSTATE_MASK   0x02    

#define CFG_AUX_RUNSTATE_MASK   0x80    // Solo lectura para SW_Config. define si la placa 
                                        // está equipada con perifericos o no.

//---------DEFINICIONES DE MASCARAS PARA PALABRA DE CONFIGURACION DE LOS PUERTOS SCI----
//
#define CFG_SCI_BAUDRATE_MASK   0x0F      // 0000-0111: 300-38400    

#define CFG_SCI_PARITY_MASK     0x30			// 01:impar, 10:par, 11:sin paridad
#define CFG_SCI_PARITY_EVEN     0X20
#define CFG_SCI_PARITY_ODD      0X10
#define CFG_SCI_PARITY_NONE     0X30

#define CFG_SCI_STOPBITS_MASK   0x40      // 0: 1 bit, 1: 2 bits
    


//---------DEFINICIONES DE CODIGOS INTERPRETADOS POR EL LOADER-----------------
//
#define   FW_RESTRICT_FREE            0xAB              // Sin restricciones
#define   FW_RESTRICT_MSG             0xAC              //
#define   FW_RESTRICT_TIME            0xAD              //
#define   FW_RESTRICT_OTHER           0xAE              //

#define   LDR_UPDATE_VALID    0x56
#define   LDR_UPDATE_TEST     0x5A
#define   LDR_UPDATE_LOAD     0x5C
#define   LDR_FACTORY_TEST    0xA0
#define   LDR_FACTORY_ATTEMPT 0x03


#define   FW_FACTORY          0x01
#define   FW_UPDATE           0x02
#define   FW_OWN_RESET        0x04
#define   FW_TEST             0x08


//--------------------------------------------------------------------------------------------
//     Codificacion en campos de mensajes de PROTOCOLO DNP3
//--------------------------------------------------------------------------------------------
//

#define MSG_HDR_LEN    		      10
#define MSG_DATABLOCK_LEN       16
#define MSG_LASTBLOCK_LEN       10

#define MSG_MAX_LEN   		     292


#define MAX_DNP_BININP          12   // 8 bytes = 64 entradas digitales	+ 2 byte para IIN
                                     // + 1 byte para Status de respuesta a comandos.
                                     // + 1 byte para Indicadores de Estado (Cx.,etc).
#define MAX_DNP_BINOUT          16   // 16 bytes = 128 salidas digitales
#define MAX_DNP_AAGINP         107   // Cantidad de entradas analogicas a consultar
#define MAX_DNP_COUNTR          29   // Cantidad de contadores a consultar
#define MAX_DNP_MODBUS         (2 * MAX_DNP_AAGINP)	  // Maxima cantidad de words.
 		
// -------------------------------------------------------------------------
// Definiciones de Codigos y constantes en capa LPDU   (LINK)
//
#define LPDU_DEF_START   		    (WORD)0X0564
#define LPDU_BITMASK_CTRL_DIR   		    0X80
#define LPDU_BITMASK_CTRL_PRM   		    0X40
#define LPDU_BITMASK_CTRL_FCB   		    0X20
#define LPDU_BITMASK_CTRL_FCV   		    0X10
#define LPDU_BITMASK_CTRL_FC   		      0X0F

#define LPDU_FC_PRM_RESETREM   		        0   // Reset Remote Link
#define LPDU_FC_PRM_RESETUSR   		        1   // Reset Usr Process
#define LPDU_FC_PRM_TESTLINK   		        2   // Test function for link
#define LPDU_FC_PRM_USERDATA  		        3   // User Data
#define LPDU_FC_PRM_USRDATNOCONF          4   // Unconfirmed User Data
#define LPDU_FC_PRM_RQSTATUS   		        9   // Request Link Status

#define LPDU_FC_SEC_ACK        		        0   // Positive Acknowledgement
#define LPDU_FC_SEC_NACK        		      1   // Msg. not accepted, line busy
#define LPDU_FC_SEC_STATUS    		       11   // Status of link (DCF=0 or 1)
#define LPDU_FC_SEC_NOTFUNC    		       14   // Link service not functioning
#define LPDU_FC_SEC_NOTUSED    		       15   // Link service not used or imp.


// -------------------------------------------------------------------------
// Definiciones de Codigos y constantes en capa TPDU (TRANSPORT)
//
#define TPDU_BITMASK_TH_FIN        		    0X80 // Final Frame
#define TPDU_BITMASK_TH_FIR        		    0X40 // First frame
#define TPDU_BITMASK_TH_SEQ        		    0X3F // Sequence number


// -------------------------------------------------------------------------
// Definiciones de Codigos y constantes en capa APDU (APPLICATION)
//
// Bitmasks para el campo APPLICATION CONTROL
#define APDU_BITMASK_AC_FIR        		    0X80 // First fragment of multi-frg.msg.
#define APDU_BITMASK_AC_FIN        		    0X40 // Final fragment of multi-frg.msg.
#define APDU_BITMASK_AC_CON        		    0X20 // Confirmation required
#define APDU_BITMASK_AC_SEQ        		    0X1F // Fragment Sequence number

// Valores del campo FUNCTION CODE
#define APDU_FC_REQ_CONFIRM   		           0 // Transfer function: confirm
#define APDU_FC_REQ_READ     		             1 // Transfer function: read
#define APDU_FC_REQ_WRITE     		           2 // Transfer function: write

#define APDU_FC_REQ_SELECT    		           3 // Control function: select
#define APDU_FC_REQ_OPERATE    		           4 // Control function: operate
#define APDU_FC_REQ_DIROPERATE 		           5 // Control function: direct operate
#define APDU_FC_REQ_DIROPNOACK		           6 // Control function: direct op no ack

#define APDU_FC_REQ_INMFREEZE    		         7 // Freeze function: Inmediate freeze
#define APDU_FC_REQ_INMFREEZENOACK		       8 // Freeze function: Inmediate freeze no ack
#define APDU_FC_REQ_FREEZEANDCLEAR		       9 // Freeze function: Freeze and clear
#define APDU_FC_REQ_FREEZECLRNOACK		      10 // Freeze function: Freeze and clr no ack
#define APDU_FC_REQ_FREEZEWITHTIME	        11 // Freeze function: Freeze with time
#define APDU_FC_REQ_FREEZETIMENOACK    		  12 // Freeze function: Freeze with time no ack

#define APDU_FC_REQ_COLDRESTART   		      13 // App Ctrl function: cold restart
#define APDU_FC_REQ_WARMRESTART   		      14 // App Ctrl function: warm restart
#define APDU_FC_REQ_INITDATA     		        15 // App Ctrl function: init data to default
#define APDU_FC_REQ_INITAPP        		      16 // App Ctrl function: init application
#define APDU_FC_REQ_STARTAPP       		      17 // App Ctrl function: start application
#define APDU_FC_REQ_STOPAPP        		      18 // App Ctrl function: stop application

// faltan config functions 19-22
// faltan time sync functions 23-24
// faltan file functions 25-30

#define APDU_FC_RES_CONFIRM        		     0 // Response function: confirm
#define APDU_FC_RES_RESPONSE        		 129 // Response function: respond to request
#define APDU_FC_RES_UNSOLICITED  		     130 // Response function: unsolicited message


// Bitmasks para los campos II1, II2
#define APDU_BITMASK_II1_DEVRESTART    		0X80 // App. restart
#define APDU_BITMASK_II1_DEVTROUBLE    		0X40 // Device Trouble
#define APDU_BITMASK_II1_POINTSLOCAL    	0x20 // Points in local
#define APDU_BITMASK_II1_TIMESYNCREQ    	0X10 // Time sync. required
#define APDU_BITMASK_II1_CLASS3AVAIL    	0X08 // Class 3 data available
#define APDU_BITMASK_II1_CLASS2AVAIL    	0X04 // Class 2 data available
#define APDU_BITMASK_II1_CLASS1AVAIL    	0X02 // Class 1 data available
#define APDU_BITMASK_II1_ALLSTATIONS    	0X01 // All station msg received

#define APDU_BITMASK_II2_RESERVED_7    		0X80 // Reserved (0)
#define APDU_BITMASK_II2_RESERVED_6    		0X40 // Reserved (0)
#define APDU_BITMASK_II2_CORRUPTCONF    	0x20 // Corrupt configuration
#define APDU_BITMASK_II2_OPINPROGRESS    	0X10 // Operation already in progress
#define APDU_BITMASK_II2_BUFFEROVF      	0X08 // Buffer overflow
#define APDU_BITMASK_II2_PARAMINVALID   	0X04 // Parameters invalid
#define APDU_BITMASK_II2_OBJUNKNOWN     	0X02 // Request objects unknown
#define APDU_BITMASK_II2_FCNOTIMP       	0X01 // FC not implemented


//--------------------------------------------------------------------------
// -------- DEFINICIONES PARA IMPLEMENTACION DE LOGS
//--------------------------------------------------------------------------

typedef enum {
	LOG_ID_GENERAL				= 0 ,
	LOG_ID_TCP_CONNECT		    ,
	LOG_ID_TCP_CLOSE			    ,
	LOG_ID_TCP_ABORT			    ,
	LOG_ID_TCP_REGENERATE     ,
	LOG_ID_TCP_NOBUFF			    ,
	LOG_ID_TCP_BADTOKEN 	    ,
	

  LOG_ID_MAX
	
} LOG_ID_ENUM;



#ifndef __FILE_SYS_H__
#define __FILE_SYS_H__

//#include"datatypes.h"

typedef struct TFileEntry
{
    INT8                    filename[24];
    void                    (* GetFunction)(UINT8 fat_index, UINT16 len, UINT8 ses);
    const unsigned char*    file_start_address;
	  unsigned short          file_length;
} TFileEntry;

extern const TFileEntry FAT [];

#endif
