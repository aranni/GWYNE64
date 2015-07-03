
#ifndef CommdefH
#define CommdefH

//#include "hComConstants.h"
//#include "hComStructDat.h"
//#include "hComSwitches.h"

/* To Clear or Set single bits in a byte variable */
//#define	b_SetBit(bit_ID, varID)		(varID |= (unsigned char)(1<<bit_ID))
//#define	b_ClearBit(bit_ID, varID)	(varID &= ~(unsigned char)(1<<bit_ID))
//#define	b_XorBit(bit_ID, varID)		(varID ^= (unsigned char)(1<<bit_ID))

//-----------------------------------------------------------------------------
//  DEFINICIONES DE MACROS
//-----------------------------------------------------------------------------
#define	FOREVER			  for(;;)
#define	MAXIMO(x,y)		(x>y)?x:y
#define	MINIMO(x,y)		(x<y)?x:y
#define	LOOP(x,y)		  for(x=y; x>0; x--)

//-----------------------------------------------------------------------------
//  DEFINICIONES DE CONSTANTES
//-----------------------------------------------------------------------------
#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

#define NON_STACKTOP        0x3FFF

#define	TMR_MAX_NODES		        20    // Cantidad de timers disponibles
#define MAX_EVENTMSG_QUEUE	    30		// Capacidad de la cola de mensajes de eventos
#define MAX_SERIAL_IN_BUFLEN    50   // Capacidad del buffer de entrada de puertos serie
#define MAX_SERIAL_OUT_BUFLEN   50   // Capacidad del buffer de salida de puertos serie
#define MAX_DEBUGBUF_LEN        128   // Capacidad del buffer de debug, (y mensajes a TCP) 

#define DBG_BY_TCP            0x01   // Enviar mensaje de debug por TCP
#define DBG_BY_SERIAL         0x02   // Enviar mensaje de debug por puerto serie.

#define STX  (UINT8)0x02
#define EOT  (UINT8)0x0D

#define ERROR (UINT8)(-1)
#define OK    (UINT8)(0x00)

// Selección de señales para RX del SCI
#define  MUX_SEL_RS485  0             
#define  MUX_SEL_RS232  1


//--TIMERS GLOBALES------------------------------------------------------------

#define	TCP_TIMEOUT_SENDRETRY   80   // Espera para reintentar envío por TCP


//--MACROS PARA CONTROL DE LEDS------------------------------------------------

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

//--DIRECCIONES DE CAMPOS EN EEPROM--------------------------------------------

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

// Información sobre la comunicación con el variador Canal_B
#define EE_ADDR_INV_FREC_B      150       // Consigna de frecuencia
#define EE_ADDR_INV_POLL_B      154				// ScanRate al variador
#define EE_ADDR_INV_TYPE_B      156       // Modelo de Variador 
#define EE_ADDR_PORT_CFG_B      158       // Estado de ejecución de procesos

#define EE_ADDR_SIE_CMDWA_B     160				// Palabra de control Variador Siemens

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


// Para TransferInfo desde el LOADER
#define EE_ADDR_LDR_INFO        400



//--DIRECCIONES DE CAMPOS EN NVR DEL RTC---------------------------------------
#define NVR_ADDR_LOGSPTR        0x20      // Punteros de logs en FLASH 3x2bytes
#define NVR_ADDR_FIRMA          0x26      // Firma para control de integridad 4bytes
#define NVR_VAL_FIRMA           0xCAFECAFE  

//-----------------------------------------------------------------------------
//  DEFINICIONES DE ENUMERACIONES
//-----------------------------------------------------------------------------
// PROCESOS
typedef enum {
	PROC_DEBUG	= 0		,
	PROC_MODBUS    		,
	PROC_SERIAL    		,
	PROC_CONFIG       ,
	PROC_RTU          ,
	PROC_MODRTU       ,
	PROC_IIC          ,
	PROC_DNP          ,

	PROCESS_QTTY 		,
	OS_NOT_PROC			,
	
	PROC_NAMES_FILL	= 0xFF
} PROCESS_NAMES_ENUM;

//EVENTOS
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
	
	EVT_DNP_RXMSG         ,           // Mensaje recibido para DNP
	EVT_DNP_POL           ,           // Polling si hay mensajes..
	EVT_TCP_CONNECT       ,           // Coneccion TCP establecida.. ( cliente para NEC)

	EVT_OTHER_MSG	= 0xFF
} EVENT_NAMES_ENUM;

// CODIGOS DE COMANDOS A LOS VARIADORES
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

// CODIGOS DE MENSAJES
typedef enum {
	MSG_STARTUP			= 0x00	,
	MSG_TEST					,
	MSG_LAST_COD				,

	MSG_COD_FILL = 0xFF
} MSG_COD_ENUM;

// VARIADORES
typedef enum {
	INV_NONE_NONE			= 0x00	,
	INV_SIEMENS 					,
	INV_HITACHI 					,

	INV_COLE_FILL = 0xFF
} INV_CODES_ENUM;

//-----------------------------------------------------------------------------
//  DEFINICIONES DE TIPOS DE DATOS
//-----------------------------------------------------------------------------
//typedef unsigned char BOOL;
//#define BOOL    unsigned char
//#define Bool    unsigned char


// Pares de EVENTO-FUNCION para definir transiciones en cada estado.
typedef struct {
	EVENT_NAMES_ENUM	EventMsg;
	void				(*TransitionFunc)(void);
} EVENT_FUNCTION;


// Estructura para FIFO de Mensajes de EVENTOS 
typedef struct {
	EVENT_NAMES_ENUM	EventMsg;
	UINT8				Source;
	UINT8				Target;
	UINT16				Param;
} EVENT_MSG;

// Estructura para control de FIFOs 
typedef struct {
	UINT8	IndexIn;
	UINT8	IndexOut;
	UINT8	Counter;
} FIFO_CNTRL;

// Estructura de datos Fecha/Hora
typedef struct {
	UINT8	Second;
	UINT8	Minute;
	UINT8	Hour;
	UINT8	Day;
	UINT8	Date;
	UINT8	Month;
	UINT8	Year;
} DATE_TIME;


// -------------------------------------------------------------------------
// Estructura para manejo de buffer (tanto en TX como en RX queue)
//
typedef struct _RTS_BUFFERS_POOL{
    BYTE        fInUse;
    void*       vpBuffer;
    BYTE        bToken;
} RTS_BUFFERS_POOL, *LPRTS_BUFFERS_POOL;

//----------------------------------------------------------
//     Estructuras de datos transferidos
//----------------------------------------------------------

#define MAX_PARAM_QTTY   31	  // Cantidad de WORDS
typedef struct {
	float	  RealFrec;         // 01 - Frecuencia Real
	float	  OutCurrent;       // 03 - Corriente de salida
	float   OutVoltage;       // 05 - Tension de Salida
	float	  ConsEnergia;      // 07 - Consumo de Energia
	float	  ParReal;          // 09 - Par Real
	float	  CorrienteNom;     // 11 - Corriente Nominal
	float	  Potencia;         // 13 - Potencia Nominal
  float   PotenciaReal;     // 15 - Potencia Real
  float   MinFrec;          // 17 - Frecuencia Minima
  float   MaxFrec;          // 19 - Frecuencia Maxima
  float   CmdParam;         // 21 - Parametro del comando
	UINT16  HorasRun;         // 23 - Horas Running
  UINT16  Polos;            // 24 - Nro. de polos
	UINT16  UltimaFalla;      // 25 - Codigo de ultima falla
	UINT16  HoraFalla;        // 26 - Hora de ultima falla
	UINT16  ValorFalla;       // 27 - Valor de la falla
	UINT16  InpVal;           // 28 - Estado de entradas
	UINT16  OutVal;           // 29 - Estado de salidas
	UINT16  StatusWord ;      // 30 - Palabra de Estado Unificada
  UINT16  CmdPending;       // 31 - Comando al variador

} INVERTER_PARAM;


// ABR: cambiar por estructuras DNP
typedef struct {
	UINT8	  START;                    // Flag
	UINT8   Data[20];                 // Datos 
	UINT16  CRC;
} DNP_DATA_MSG;

// Estructura para FIFO de Mensajes de EVENTOS 
typedef struct {
	UINT8				Data_0;
	UINT8				Data_1;
} DNP_MSG;
// ABR: cambiar por estructuras DNP


// Estructura para Mensajes MODBUS_TCP 
#define MAX_MODBUS_DATA   (2 * (MAX_PARAM_QTTY+1))  // BYTES
typedef struct {
	UINT8	  TransactionID0;
	UINT8	  TransactionID1;			
	UINT8	  ProtocolID0;				  // = 0
	UINT8	  ProtocolID1;				  // = 0
	UINT8	  LengthHigh;			      // = 0 todos los mensajes < 256
	UINT8	  LengthLow;					  // Longitud a partir de aqui
	UINT8	  UI;						        // Unit Identifier (slave address)
	UINT8	  FC;                   // Modbus Function Code
	UINT8   Data[MAX_MODBUS_DATA];// Datos segun el FC
} MODBUS_MSG;

#define MAX_TCP_DATA (MAX_MODBUS_DATA  + 10)

// Estructura para Mensajes MODBUS_RTU
typedef struct {
	UINT8	  UI;                       // Slave address..
	UINT8	  FC;			                  // Function Code..
	UINT8   Data[MAX_MODBUS_DATA];    // Datos segun el FC
	UINT16  CRC;
} MODBUS_RTU_MSG;

// Estructura para Mensajes MODBUS_ASCII
#define MAS_START_FLAG      0x3A
#define MAS_END_CR_FLAG     0x0D
#define MAS_END_LF_FLAG     0x0A
typedef struct {
  UINT8   START;                      // Flag de START = ':' = 0x3A
	UINT8	  UIH;                        // Slave address..
	UINT8	  UIL;                        // Slave address..
	UINT8	  FCH;			                  // Function Code..
	UINT8	  FCL;			                  // Function Code..
	UINT8   Data[2*MAX_MODBUS_DATA];    // Datos segun el FC
	UINT8   LRCH;
	UINT8   LRCL;
	UINT8   END_CR;                     // Flag END = CR = 0x0D
	UINT8   END_LF;                     // Flag END = LF = 0x0A
} MODBUS_ASCII_MSG;


// Estructura para Mensajes SIEMENS 
//#define MAX_SIEMENS_DATA   10
#define SIE_START_FLAG       STX
#define SIE_COMMON_LGE       14
#define SIE_DEFAULT_SLAVE     1
typedef struct {
	UINT8	  IniFlag;                // STX
	UINT8	  LGE;                   // Len
	UINT8	  Addr;                  // Slave Address
	UINT16  PKE;                   // 1st word: Parameter ID
	UINT16  IND;                   // 2nd word: Parameter Index
	UINT16  PWE1;                  // 3rd word: Parameter Value
	UINT16  PWE2;                  // 4th word: Parameter Value
	UINT16  PZD1;
	UINT16  PZD2;
	UINT8   BCC;                   // Block Check Character
} SIEMENS_MSG;

// Estructura para Mensajes HITACHI J300
#define HIT_MAX_DATAOUT      6
#define HIT_CMD_LEN          4
#define HIT_MAX_DATAIN       80
typedef struct {
  UINT8   TxLen;
  UINT8   RxLen;
	UINT8	  IniFlag;                // STX
	UINT8   Addr[2];                // Slave Address
	UINT8   Command[HIT_CMD_LEN];
	UINT8   Data[HIT_MAX_DATAOUT];
	UINT8   Sum[2];
	UINT8   EndFlag;
} HITACHI_OUT_MSG;

typedef struct {
	UINT8	  IniFlag;                // STX
	UINT8   Addr[2];                // Slave Address
	UINT8   RC[2];                  // Response Code
	UINT8   Command[HIT_CMD_LEN];
	UINT8   Data[HIT_MAX_DATAIN];
	UINT8   Sum[2];
	UINT8   EndFlag;
} HITACHI_IN_MSG;


// Estructura para Mensajes HITACHI SJ300 
#define HSJ_MAX_DATAOUT      12
#define HSJ_CMD_LEN          2
#define HSJ_MAX_DATAIN       104
typedef struct {
  UINT8   TxLen;
  UINT8   RxLen;
	UINT8	  IniFlag;                // STX
	UINT8   Addr[2];                // Slave Address
	UINT8   Command[HSJ_CMD_LEN];
	UINT8   Data[HSJ_MAX_DATAOUT];
	UINT8   Sum[2];
	UINT8   EndFlag;
} HITSJ300_OUT_MSG;

typedef struct {
	UINT8	  IniFlag;                // STX
	UINT8   Addr[2];                // Slave Address
//	UINT8   RC[2];                  // Response Code
//	UINT8   Command[HSJ_CMD_LEN];
	UINT8   Data[HSJ_MAX_DATAIN];
	UINT8   Sum[2];
	UINT8   EndFlag;
} HITSJ300_IN_MSG;


// Parametros del sistema 
typedef struct {
	float   ConsignaFrec;     // Consigna de Frecuencia
	float   RelacionTx;       // Relacion de transmision
	float   CoefResbal;       // Coeficiente de Resbalamiento
	float   RpmEnMotor;       // RPMs del motor
	
  UINT8   RunConfig;
	UINT8 	InvCode;          // Tipo de variador conectado
	UINT16 	ScanRate;         // Retardo entre pollings SCI1
		
} SYS_CONFIG_PARAM;


// Parámetros de configuración para los puertos serie
typedef struct {
  UINT8   BaudRate;         // Tasa de transferencia, tabulada
  UINT8   Frame;		        // Bits de stop, Paridad, Bits de start
                            // Bit 0 : Capa física: RS232 ó RS485
  UINT8   Protocolo;        // Protocolo asociado 
  UINT8   Proceso;          // Proceso asociado  
  
} SCI_CONFIG_PARAM;


// Parametros propios de variador Siemens
typedef struct {
	UINT16  CmdWordA;         // Palabra de control A
	UINT16  CmdWordB;         // Palabra de control B
	float   FrecuenciaNom;    //
} INV_CONFIG_PARAM;

// Valores de las variables de entrada salida
typedef struct {
	float   AAG_Input_A;       // Entrada analógica 1
	float   AAG_Input_B;       // Entrada analógica 2
	float   AAG_Output_A;      // Salida analógica 1
	float   AAG_Output_B;      // Salida analógica 2
	float   Temperatura;       // Temperatura interna    
	UINT8   DigInpVal;         // 28 - Estado de entradas
	UINT8   DigOutVal;         // 29 - Estado de salidas
} GIO_STATE_VAL;

// Valores registrados para estadisticas del OS
typedef struct {
  UINT8   MaxMsgCntr;				// Mayor cantidad de mensajes acumulados
	UINT8   TmrInUseCntr;     // Timers ocupados
	UINT16  WatchRstTmr;      // Timer periodico de supervision
	UINT16  MsgRstCntr;       // Contador de resets forzados
	UINT16  MsgDogCntr;       // Contador de resets por watchdog
} OS_STAT;

// Valores registrados para estadisticas de cx. Serie
typedef struct {
	UINT32 	MsgTxOk;        // Mensajes transmitidos correctamente
	UINT32  MsgTxErr;       // Timeouts en transmision
	UINT32 	MsgRxOk;        // Mensajes recibidos correctamente
	UINT32  MsgRxErr;       // Timeouts en recepcion
} SERIAL_STAT;

// Estadisticas agrupadas en una sola estructura para transferencia. 
typedef struct {
  OS_STAT       OsStat;
  SERIAL_STAT   SerialStat;
  DATE_TIME     CurrentDateTime;
} STATISTIC_MSG;

/*
// Encabezado de mensajes de configuracion
typedef struct {
        UINT8           Len;
        UINT8           OpCode;
}CFG_MSG_FRAME;
*/

#define MAX_MSG_DATA    256  
//Estructura de todos los mensajes de config : Encabezado + Data
typedef struct {
        UINT8     Len;
        UINT8     OpCode;
        UINT8     TxWindow;
        UINT8     RxWindow;
        UINT16    Parameter;
        UINT16    AuxParam;
        UINT8     Data[MAX_MSG_DATA];
}CFG_MSG_FRAME;

#define MIN_CFGFRAME_LEN  (sizeof(CFG_MSG_FRAME)-MAX_MSG_DATA) 

// Estructura de mensaje de configuracion del modulo
typedef struct {
        UINT8       InvCode;
        UINT16      ScanRate;
        UINT8       TcpIP[4];
        UINT8       TcpMask[4];
        UINT8       TcpGW[4];
        UINT8       TcpMAC[6];
        UINT16      SIE_CmdWordA;
        UINT8       RunConfig;
        UINT16      var_TCP_INIT_RETRY_TOUT;
        UINT16      var_TCP_DEF_RETRY_TOUT;
        UINT8       var_TCP_DEF_RETRIES;			 
}CFG_MSG_INFO;



//--------LONGITUDES DE CAMPOS CON INFORMACION DE VERSION.------------
#define SIZE_FW_VER     15
#define SIZE_HW_VER     15
#define SIZE_SERIAL     15
#define VER_KEY_CODE    1974

// Nuevo Formato para almacenar datos de versión de FW.
typedef struct CFG_FW_VERSION {
        UINT8       FW_Version[SIZE_FW_VER+1];
        UINT8       DateOfCompilation[20];
        UINT8       TimeOfCompilation[10];
        UINT8       FW_Type;
        UINT8       FW_BackWardCompatibility;        
        UINT8       Reservado[52];
}CFG_FW_VERSION;


// Formato del HEADER completo con información del FW en FLASH EXTERNA
typedef struct CFG_FW_HEADER {
        CFG_FW_VERSION  VersionInfo;     // 100 bytes en FLASH del micro
        UINT8           ReservedQttyWord;  
        UINT8           RegInFlashQtty;  
        
        UINT8           TransferInfo;                   // 103
        UINT8           RestrictType;                   // 104
        
        UINT8           RestrictFromSerial[10];
        UINT8           RestrictToSerial[10];
        UINT32          TrialMsgLimit;
        UINT32          TrialTimeLimit;
        UINT32          TrialOtherLimit;                // 136
        
        UINT8           FW_Autor[20];
        UINT8           FW_Info[50];
        UINT8           FW_ReservedSpace[30];           // 236

}CFG_FW_HEADER;


// Estructura de mensaje con informacion de VERSION
typedef struct {
        CFG_FW_VERSION FW_VersionHdr;
//        UINT8   FW_Version[SIZE_FW_VER+1];
        UINT8   HW_Version[SIZE_HW_VER+1];
        UINT8   Serial[SIZE_SERIAL+1];
        UINT16  KeyCode;
}CFG_MSG_VERSION;

// Estructura de mensaje con informacion de CONSTANTES
typedef struct {
        float   ConsignaFrec;
        float   RelacionTx;
        float   CoefResbal;
        float   RpmEnMotor;
        UINT8   UsrText[30];
}CFG_MSG_CTES;


// Estructura de formato de datos para lectura/escritura en FLASH
#define  FLASH_REG_SIZE		256
typedef struct {
	  UINT8	  	  Command;
    UINT8     	AddressPAH;
    UINT8     	AddressPAL;
    UINT8     	AddressBA;
    UINT8     	DontCare_1;
    UINT8     	DontCare_2;
    UINT8     	DontCare_3;
    UINT8     	DontCare_4;
    UINT8		    Data[FLASH_REG_SIZE];
}FLASH_REG_FORMAT;

#define FLASH_HEADER_SIZE   (sizeof(FLASH_REG_FORMAT)-FLASH_REG_SIZE)

// Estructura de formato de LOGs de eventos
typedef struct {

    DATE_TIME 	LogDateTime;   
    UINT8       Data[10];    
    UINT8     	Cliente;
    UINT16     	AlarmIdx;
    UINT8		    EstadoEje1;
    UINT8		    EstadoEje2;
    UINT8		    EstadoEje3;
    UINT8		    EstadoEje4;    
    float		    Velocidad;
    
}LOG_FORMAT;

typedef struct {
  INT8   filename[24];
  UINT32 file_lenght;
  UINT32 file_addr;
}WEB_FAT;


// Estructura de datos de los punteros a los logs grabados en FLASH
typedef struct {
	UINT16	NextToWrite;
	UINT16	NextToRead;
	UINT16	FirstToRead;
} LOGS_PTR;

// Codigos de comandos de operacion recibidos por TCP
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

#define STATUS_READY_BIT    0x01
#define STATUS_RUN_BIT      0x02
#define STATUS_FAULT_BIT    0x04

#define STATUS_CX_FAIL_BIT  0x80

#define CFG_RTC_RUNSTATE_MASK   0x01    
#define CFG_RTU_RUNSTATE_MASK   0x02    


//---------DEFINICIONES DE CODIGOS INTERPRETADOS POR EL LOADER--------
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


//------------SWITCHES DE CONFIGURACION DE FW------------------------
//
//********************************************************************
//********************************************************************
// habilita registro de estadísticas del OS
// #define DBG_WATCH_OS                 

// habilita reset por acumulacion de errores
// #define MON_SERIAL_ERR_RESET_ENABLE  

// descomentar para generar un FW original de fábrica..
//#define FW_FACTORY_MODE

// Habilita la generacion de un código con vectores de int. verdaderos..
//#define   OWN_RESET_VECTOR

// Para generar un FW que realice test, y configure inicialmente la EEPROM
//#define   FW_FACTORY_TEST

#define FW_VERSION_DEF    "GWM-C-002.006.D"
#define HW_VERSION_DEF    "GWY-C-001.09.06"
#define HW_SERIAL_DEF     "A-000000.FEB.07"

//********************************************************************
//********************************************************************

#ifdef  FW_FACTORY_TEST
#define   OWN_RESET_VECTOR
#endif

#ifdef OWN_RESET_VECTOR
#define   FLAG_OWN_RESET    FW_OWN_RESET
#else	  // OWN_RESET_VECTOR	 
#define   FLAG_OWN_RESET    0
#endif  // OWN_RESET_VECTOR

#ifdef FW_FACTORY_MODE
#define FLAG_FACTORY      FW_FACTORY 
#else   // FW_FACTORY_MODE
#define FLAG_FACTORY      FW_UPDATE
#endif  // FW_FACTORY_MODE

#ifdef FW_FACTORY_TEST
#define FLAG_TEST         FW_TEST
#else   // FW_FACTORY_TEST
#define FLAG_TEST         0
#endif  // FW_FACTORY_TEST


#define FW_TYPE_CODE    (FLAG_FACTORY|FLAG_OWN_RESET|FLAG_TEST)

          
// habilita WatchDog
#define WATCHDOG_ENABLE

#ifdef  WATCHDOG_ENABLE
#define REFRESH_WATCHDOG  {(ARMCOP = 0x55);\
                           (ARMCOP = 0xAA);} 
#else                     
#define REFRESH_WATCHDOG         
#endif

#endif      // CommdefH