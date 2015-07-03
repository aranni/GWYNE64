
//--------------------------------------------------------------------------------------------
//     Estructuras para manejo de informacion del NUCLEO DEL SISTEMA
//--------------------------------------------------------------------------------------------
//
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

// Estructura para control de FIFOs de mayor capacidad
typedef struct {
	UINT16	IndexIn;
	UINT16	IndexOut;
	UINT16	Counter;
} FIFO_LARGE_CNTRL;


// -------------------------------------------------------------------------
// Estructura para manejo de buffer (tanto en TX como en RX queue)
//
typedef struct _RTS_BUFFERS_POOL{
    UINT8        fInUse;
    void*        vpBuffer;
    UINT8        bToken;
} RTS_BUFFERS_POOL, *LPRTS_BUFFERS_POOL;


//--------------------------------------------------------------------------------------------
//     Estructuras para manejo de informacion del SISTEMA
//--------------------------------------------------------------------------------------------
//
//--Parametros del sistema 
typedef struct {
	float   ConsignaFrec;     // Consigna de Frecuencia
	float   RelacionTx;       // Relacion de transmision
	float   CoefResbal;       // Coeficiente de Resbalamiento
	float   RpmEnMotor;       // RPMs del motor
	
  UINT8   RunConfig;
	UINT8 	InvCode;          // Tipo de variador conectado
	UINT16 	ScanRate;         // Retardo entre pollings SCI1
		
} SYS_CONFIG_PARAM;

//--Parámetros de configuración para los puertos serie
typedef struct {
  UINT8   BaudRate;         // Tasa de transferencia, tabulada
  UINT8   Frame;		        // Bits de stop, Paridad, Bits de start
                            // Bit 0 : Capa física: RS232 ó RS485
  UINT8   Protocolo;        // Protocolo asociado 
  UINT8   Proceso;          // Proceso asociado  
  
} SCI_CONFIG_PARAM;

//--Parametros propios de variador Siemens
typedef struct {
	UINT16  CmdWordA;         // Palabra de control A
	UINT16  CmdWordB;         // Palabra de control B
	float   FrecuenciaNom;    //
} INV_CONFIG_PARAM;

//--Valores de las variables de entrada salida
typedef struct {
	float   AAG_Input_A;       // Entrada analógica 1
	float   AAG_Input_B;       // Entrada analógica 2
	float   AAG_Output_A;      // Salida analógica 1
	float   AAG_Output_B;      // Salida analógica 2
	float   Temperatura;       // Temperatura interna    
	UINT8   DigInpVal;         // 28 - Estado de entradas
	UINT8   DigOutVal;         // 29 - Estado de salidas
} GIO_STATE_VAL;

//--Estructura de datos Fecha/Hora
typedef struct {
	UINT8	Second;
	UINT8	Minute;
	UINT8	Hour;
	UINT8	Day;
	UINT8	Date;
	UINT8	Month;
	UINT8	Year;
} DATE_TIME;



//--------------------------------------------------------------------------------------------
//     Estructuras para manejo de informacion de NEXO EN LA TRADUCCION DE PROTOCOLOS
//--------------------------------------------------------------------------------------------
//
typedef struct {
	UINT16  InpVal;           // 28 - Estado de entradas
	UINT16  OutVal;           // 29 - Estado de salidas
	UINT16  StatusWord ;      // 30 - Palabra de Estado Unificada
  UINT16  CmdPending;       // 31 - Comando al variador

} INVERTER_PARAM;


//--------------------------------------------------------------------------------------------
//     Estructuras para manejo de informacion de ESTADISTICAS
//--------------------------------------------------------------------------------------------
//
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

//--------------------------------------------------------------------------------------------
//     Estructuras para manejo de informacion para TRANSFERNCIAS AL PROGRAMA DE CONFIG
//--------------------------------------------------------------------------------------------
//
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

// Estructura de mensaje de configuracion del la comunicación DNP3
typedef struct {
        UINT16       AddrMaster;
        UINT16       AddrSlave;
}CFG_MSG_DNPINFO;


//--------------------------------------------------------------------------------------------
//     Estructuras para manejo de informacion alojada en FLASH EXTERNA 
//--------------------------------------------------------------------------------------------
//
//--------LONGITUDES DE CAMPOS CON INFORMACION DE VERSION.------------
#define SIZE_FW_VER     15
#define SIZE_HW_VER     15
#define SIZE_SERIAL     15
#define VER_KEY_CODE    1974

//--Nuevo Formato para almacenar datos de versión de FW.
typedef struct CFG_FW_VERSION {
        UINT8       FW_Version[SIZE_FW_VER+1];
        UINT8       DateOfCompilation[20];
        UINT8       TimeOfCompilation[10];
        UINT8       FW_Type;
        UINT8       FW_BackWardCompatibility;        
        UINT8       Reservado[52];
}CFG_FW_VERSION;


//--Formato del HEADER completo con información del FW en FLASH EXTERNA
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


//--Estructura de mensaje con informacion de VERSION
typedef struct {
        CFG_FW_VERSION FW_VersionHdr;
//        UINT8   FW_Version[SIZE_FW_VER+1];
        UINT8   HW_Version[SIZE_HW_VER+1];
        UINT8   Serial[SIZE_SERIAL+1];
        UINT16  KeyCode;
}CFG_MSG_VERSION;

//--Estructura de mensaje con informacion de CONSTANTES
typedef struct {
        float   ConsignaFrec;
        float   RelacionTx;
        float   CoefResbal;
        float   RpmEnMotor;
        UINT8   UsrText[30];
}CFG_MSG_CTES;

// Estructura de mensaje con informacion de Configuracion de puertos Serie
typedef struct {
        UINT8   SCIA_ConfigCode;
        UINT8   SCIA_ConfigNode;
        UINT16  SCIA_ConfigScanRate;
        UINT8   SCIB_ConfigCode;
        UINT8   SCIB_ConfigNode;
        UINT16  SCIB_ConfigScanRate;
}CFG_MSG_SERIALCONFIG;

//--Estructura de formato de datos para lectura/escritura en FLASH
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

#define LOG_ID_SYS  65
#define LOG_ID_TCP  66
#define LOG_DATA_SIZE 100  

//--Estructura de formato de LOGs de eventos
typedef struct {
    DATE_TIME 	LogDateTime;   
    UINT8     	LogId;
    UINT16     	ParamVal;
    UINT8       Data[LOG_DATA_SIZE];    
}LOG_FORMAT;

//--Esctructura de la FAT de los archivos ofrecidos por el servidor web
typedef struct {
  INT8   filename[24];
  UINT32 file_lenght;
  UINT32 file_addr;
}WEB_FAT;


//--Estructura de datos de los punteros a los logs grabados en FLASH
typedef struct {
	UINT16	NextToWrite;
	UINT16	NextToRead;
	UINT16	FirstToRead;
} LOGS_PTR;



//--------------------------------------------------------------------------------------------
//     Estructuras para manipulacion de datos codificados en MODBUS
//--------------------------------------------------------------------------------------------
//
//--Estructura para Mensajes MODBUS_TCP 
#define MAX_MODBUS_REG   126	  // Cantidad de WORDS
#define MAX_MODBUS_DATA   (2 * (MAX_MODBUS_REG+1))  // BYTES
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

//--Estructura para Mensajes MODBUS_RTU
typedef struct {
	UINT8	  UI;                       // Slave address..
	UINT8	  FC;			                  // Function Code..
	UINT8   Data[MAX_MODBUS_DATA];    // Datos segun el FC
	UINT16  CRC;
} MODBUS_RTU_MSG;

//--Estructura para Mensajes MODBUS_ASCII
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



//--------------------------------------------------------------------------------------------
//     Estructuras para manipulacion de datos de VARIADORES SIEMENS
//--------------------------------------------------------------------------------------------
//
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


//--------------------------------------------------------------------------------------------
//     Estructuras para manipulacion de datos de VARIADORES HITACHI
//--------------------------------------------------------------------------------------------
//
//--Estructura para Mensajes HITACHI J300
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


//--estructura para Mensajes HITACHI SJ300 
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


//--------------------------------------------------------------------------------------------
//     Estructuras para manipulacion de datos codificados en PROTOCOLO DNP3
//--------------------------------------------------------------------------------------------
//
// -------------------------------------------------------------------------
// Estructura de mensajes de Link Protocol  Data Unit (LPDU)
//
typedef struct _RTS_MSG_FORMAT {
    WORD    wStart;
    BYTE    bLenght;
    BYTE    bControl;
    WORD    wTarget;
    WORD    wSource;
    WORD    wCrcHdr;
    BYTE    bDataBlock1[MSG_DATABLOCK_LEN];
    WORD    wCrcBlock1;
    BYTE    bDataBlock2[MSG_DATABLOCK_LEN];
    WORD    wCrcBlock2;
    BYTE    bDataBlock3[MSG_DATABLOCK_LEN];
    WORD    wCrcBlock3;
    BYTE    bDataBlock4[MSG_DATABLOCK_LEN];
    WORD    wCrcBlock4;
    BYTE    bDataBlock5[MSG_DATABLOCK_LEN];
    WORD    wCrcBlock5;
    BYTE    bDataBlock6[MSG_DATABLOCK_LEN];
    WORD    wCrcBlock6;
    BYTE    bDataBlock7[MSG_DATABLOCK_LEN];
    WORD    wCrcBlock7;
    BYTE    bDataBlock8[MSG_DATABLOCK_LEN];
    WORD    wCrcBlock8;
    BYTE    bDataBlock9[MSG_DATABLOCK_LEN];
    WORD    wCrcBlock9;
    BYTE    bDataBlock10[MSG_DATABLOCK_LEN];
    WORD    wCrcBlock10;
    BYTE    bDataBlock11[MSG_DATABLOCK_LEN];
    WORD    wCrcBlock11;
    BYTE    bDataBlock12[MSG_DATABLOCK_LEN];
    WORD    wCrcBlock12;
    BYTE    bDataBlock13[MSG_DATABLOCK_LEN];
    WORD    wCrcBlock13;
    BYTE    bDataBlock14[MSG_DATABLOCK_LEN];
    WORD    wCrcBlock14;
    BYTE    bDataBlock15[MSG_DATABLOCK_LEN];
    WORD    wCrcBlock15;
    BYTE    bDataBlock16[MSG_LASTBLOCK_LEN];
    WORD    wCrcBlock16;
} RTS_MSG_FORMAT,*LPRTS_MSG_FORMAT;


// -------------------------------------------------------------------------
// Estructura para REQUEST de Application Protocol Data Unit (APDU)
//
typedef struct _APU_MSG_REQUEST {
    BYTE    TransportHeader;
    BYTE    ApplicationControl;
    BYTE    FunctionCode;
    BYTE    ObjectGroup;
    BYTE    ObjectVariation;
    BYTE    Qualifier;
    BYTE    bData[250];
} APU_MSG_REQUEST,*LPAPU_MSG_REQUEST;


// -------------------------------------------------------------------------
// Estructura para RESPONSE de Application Protocol Data Unit (APDU)
//
typedef struct _APU_MSG_RESPONSE {
    BYTE    TransportHeader;
    BYTE    ApplicationControl;
    BYTE    FunctionCode;
    BYTE    IIN_1;
    BYTE    IIN_2;
    BYTE    ObjectGroup;
    BYTE    ObjectVariation;
    BYTE    Qualifier;
    BYTE    bData[250];
} APU_MSG_RESPONSE,*LPAPU_MSG_RESPONSE;


#define   MAX_DNP_DATA            292       // Longitud máxima de cualquier trama DNP3.0

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

// -------------------------------------------------------------------------
// Estructura para SOCKETS SRV DNP-TCP para conversor DNP3 <-> DNP-TCP y demás sockets genericos
//
#define MAX_SRV_TCPSOCH             2       // Cantidad de sockets abiertos para DNP_SRV
typedef struct {
  INT8      TcpSrvTcpSoch;	    // Socket handle SRV para conexion DNP entrante
	UINT8	    status;             // Estado de la conexion
	UINT16    SlaveNodeAddress;   // Direccion del esclavo que se conectó por este canal
	UINT32    SlaveIpAddress;
	UINT16    SlaveTcpPort;
	UINT32    MsgTxOk;
	UINT32    MsgRxOk;
	UINT8     TxRetries;
	UINT8     TimerId;
	UINT8     BufferToken;
	
} SRV_TCP_SOCKET;



//--------------------------------------------------------------------------------------------
//     Estructuras para manipulacion de datos codificados en PROTOCOLO de sensores MTS
//--------------------------------------------------------------------------------------------
//
// -------------------------------------------------------------------------
// Estructura de mensajes para comando y respuesta
//
#define MSG_MAX_MTS_LEN   80
//
typedef struct _MTS_MSG_FORMAT {
    BYTE    bAddress;
    BYTE    bCommand;
    BYTE    bStartFlag;
    BYTE    bData[MSG_MAX_MTS_LEN];
    BYTE    bEndFlag;
    BYTE    bCheckSum[5];
} MTS_MSG_FORMAT,*LPMTS_MSG_FORMAT;

typedef struct {
	float   fResultadoNivelP;  // Nivel de producto en mm
	float   fResultadoNivelI;  // Nivel de interfaz en mm
	float   fLitrosTanqueDiesel;  // Nivel de gasoil en litros
	float   fLitrosTanqueAgua;    // Nivel de agua en litros
	float   fTemperatura[6];   // Temperaturas
	UINT8   dStatus;           // Estado..
} MTS_STATE_VAL;


//--------------------------------------------------------------------------------------------
//     Estructuras para definicion de buffers de memoria para manejo de datos MODBUS
//--------------------------------------------------------------------------------------------
//
// -------------------------------------------------------------------------
// Estructura de mensajes para comando y respuesta
//
#define MSG_MAX_MOD_LEN   252
//
typedef struct _MOD_MSG_FORMAT {
    UINT8   SocketIdx;
  	UINT8	  TransactionID0;
	  UINT8	  TransactionID1;			
	  UINT8	  ProtocolID0;				  // = 0
	  UINT8	  ProtocolID1;				  // = 0
	  UINT8	  LengthHigh;			      // = 0 todos los mensajes < 256
	  UINT8	  LengthLow;					  // Longitud a partir de aqui
	  UINT8	  UI;						        // Unit Identifier (slave address)
	  UINT8	  FC;                   // Modbus Function Code
    UINT8   bData[MSG_MAX_MOD_LEN];
    UINT16  wCheckSum;
} MOD_MSG_FORMAT,*LPMOD_MSG_FORMAT;



//--------------------------------------------------------------------------------------------
//     Estructuras para definicion de buffers de memoria para MemManager
//--------------------------------------------------------------------------------------------
//
#define MEM_MAX_BUF_LEN   256
//
typedef struct _MEM_BUF_FORMAT {
    UINT8   bData[MEM_MAX_BUF_LEN];
    UINT16  bCheckSum;
} MEM_BUF_FORMAT,*LMEM_BUF_FORMAT;


//--------------------------------------------------------------------------------------------
//     Estructuras para manipulacion de datos codificados en PROTOCOLO BAC
//--------------------------------------------------------------------------------------------
//
// -------------------------------------------------------------------------
// Estructura de mensajes para comando y respuesta
//
#define MSG_MAX_BAC_LEN   250

#define MAX_BAC_BUF     1536    // Cantidad de bytes reservados para alojar mensajes.
#define MAX_BAC_BANKS   10			// Cantidad maxima de bancos a alojar
#define BAC_BANKS_INI   0x05    // codigo para estado de bancos inicializados en EEPROM

#define BAC_EXTRA_LEN   0x01    // Bytes adicionales reservados en cada banco..

// Definiciones bytes y bits de control de procesos
//
#define   BAC_MASK_POL_ENABLE       0x01      // Habilita polling
#define   BAC_MASK_RESET_LIST       0x02      // Resetear lista de nodos
#define   BAC_MASK_MIRR_FRAME       0x04      // habilita retransmisión por Comm2

#define   BAC_MASK_INIT_SEQUENCE    0x10      // comienza una secuencia de polling

#define   BAC_DOMAIN_ADDRESS        0xF040    // dirección de mensaje de dominio

// Definiciones bytes de protocolo
//
#define DLE_CHAR		0x10
#define STX_CHAR		0x02
#define CTRLL_CHAR	0x00
#define CTRLN_CHAR	0x00
#define PRIOR_CHAR  0xC8
#define BC_CHAR			0xC0
#define ETX_CHAR    0x03
#define ACK_CHAR		0x06


typedef struct {
	UINT16    BacNodeAddress;     // Direccion del nodo bac
	UINT16    BankOffsetStart;    // Offset hasta el comienzo del banco en el array
	UINT16    BankModbusAddress;  // Dirección de mapeo del banco en modbus
	UINT8     BankStatus;         // Estado del banco..?	
	UINT8     BankSize;           // Tamaño del banco en bytes
} BAC_BANK_ALLOC;

typedef union uBAC_PROC_DIAG
{
  	UINT16	  WordDiag;
    struct {
    		UINT16	alive   :1;				// '1', Comunicación establecida ok
    		UINT16  noMem   :1;				// '1', no hay más memoria para alojar bancos
    		UINT16	noBank  :1;				// '1', no hay más bancos
    		UINT16	notUsed :13;
    }BitDiag;
}BAC_PROC_DIAG;

