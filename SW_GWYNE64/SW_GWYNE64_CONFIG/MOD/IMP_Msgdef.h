#ifndef IMP_MsgDefH
#define IMP_MsgDefH

#define TCP_TIMERTIC    100
#define MAX_TX_RETRIES  5

#define INVCODE_SIE_MICROMASTER         3
#define INVCODE_SIE_MASTERDRIVE         9
#define INVCODE_HIT_J300                4
#define INVCODE_HIT_SJ300              10

#define CFG_TCP_READ            1
#define CFG_TCP_WRITE           2
#define CFG_TCP_RESET           3       // Resetear el módulo
#define CFG_TCP_READ_STAT       4       // Leer estadísticas
#define CFG_TCP_READ_VER        5       // Leer valores de las versiones
#define CFG_TCP_WRITE_VER       6       // Modificar datos de la version de HW y serial
#define CFG_TCP_READ_CTES       7       // Lestura de valores constantes
#define CFG_TCP_WRITE_CTES      8       // Modificar constantes
#define CFG_TCP_WRITE_DATE      9       // Modificar fecha y hora
#define CFG_TCP_ACK             10      // Confirmación positiva
#define CFG_TCP_NAK             11      // Confirmación negativa
#define CFG_TCP_POL             12      // Polling periódico

#define CFG_TCP_READ_EEPROM     13      // Lectura de EEPROM
#define CFG_TCP_WRITE_EEPROM    14      // Escritura en EEPROM

#define CFG_TCP_READ_FLASH      15      // Lesctura desde una posición de la flash
#define CFG_TCP_WRITE_FLASH     16      // Escritura en una posición de la flash

#define CFG_TCP_UPGRADE_FW      17      // Grabar FW Transferido.
#define CFG_TCP_VALIDATE_FW     18      // Validar FW cargado
#define CFG_TCP_RESTORE_FW      19      // Recupera el FW de fábrica.

#define CFG_TCP_GET_IOVALUES    30        // Lectura de I/O
#define CFG_TCP_SET_IOVALUES    31        // Escritura de I/O


#define CFG_TCP_READ_DNP        40      // Lectura de configuracion DNP
#define CFG_TCP_WRITE_DNP       41      // Escritura de configuracion DNP

#define CFG_TCP_GET_SCICONFIG   42      // Lectura de Configuracion de puertos SCI
#define CFG_TCP_SET_SCICONFIG   43      // Escritura de Config. de puertos SCI


#define CFG_TCP_WRITE_FLASH256  32      // Escritura en la flash un página de 256 bytes

#define CFG_TCP_LOG_READ        50      // Lectura de logs
#define CFG_TCP_LOG_REWIND      51      // Rebobinar puntero de logs
#define CFG_TCP_LOG_RESET       52      // Resetear logs


#define CFG_RTC_RUNSTATE_MASK   0x01
#define CFG_RTU_RUNSTATE_MASK   0x02
    
#define	MAXIMO(x,y)		(x>y)?x:y

#define READ_FLASH_VERIFY       0x01
#define READ_FLASH_HDRUPDATE    0x02
#define READ_FLASH_HDRFACTORY   0x03
#define READ_FLASH_DEBUG        0x04

#define READ_LOG_REWIND         0x01
#define READ_LOG_READING        0x02


#define   FW_FACTORY          0x01
#define   FW_UPDATE           0x02
#define   FW_OWN_RESET        0x04

#define   WEB_FAT_STAGE       0x01
#define   WEB_FILES_STAGE     0x02
          

// Estructura de mensaje de configuracion del la comunicación DNP3
struct TMsgDnpInfo{
        unsigned short       AddrMaster;
        unsigned short       AddrSlave;
};


// Mensaje con datos de configuracion del canal TCP
struct TMsgTcpTimers{
        UINT8           var_TCP_INIT_RETRY_TOUT;
        UINT8           var_TCP_RETRY_TOUT;
        UINT8           var_TCP_DEF_RETRIES;
};

// Mensaje con datos de configuracion
struct TMsgInfo{
        unsigned char   DebugLevel;
        unsigned short  PollingFrec;
        unsigned char   TcpIP[4];
        unsigned char   TcpMask[4];
        unsigned char   TcpGW[4];
        unsigned char   TcpMAC[6];
        unsigned short  SIE_CmdWordA;
        unsigned char   RunConfig;
        unsigned short   var_TCP_INIT_RETRY_TOUT;
        unsigned short   var_TCP_RETRY_TOUT;
        unsigned char   var_TCP_DEF_RETRIES;

};

// Mensaje con datos de fecha y hora
struct TMsgDateTime{
        unsigned char   Second;
        unsigned char   Minute;
        unsigned char   Hour;
        unsigned char   Day;
        unsigned char   Date;
        unsigned char   Month;
        unsigned char   Year;
};

// Mensajes con informacion de estadisticas
struct TMsgStat{
        unsigned char   MaxMsgCntr;	 // Mayor cantidad de mensajes acumulados
	unsigned char   MaxTmrInUseCntr; // Timers ocupados
        unsigned short  WatchRstTmr;     //---
        unsigned short  MsgRstCntr;      //---
        unsigned short  MsgDogCntr;      //---
        unsigned int 	MsgTxOk;        // Mensajes transmitidos correctamente
	unsigned int    MsgTxErr;       // Timeouts en transmision
	unsigned int 	MsgRxOk;        // Mensajes recibidos correctamente
	unsigned int    MsgRxErr;       // Timeouts en recepcion
        TMsgDateTime    CurrentDateTime;        // Fecha y hora
};

#define MSG_INFO_SIZE   sizeof(TMsgInfo)        // Tamaño de un mensaje de información
#define MSG_STAT_SIZE   sizeof(TMsgStat)        // Tamaño de un mensaje de estadísticas
#define MAX_MSG_DATA    256                     // Controlar que haya lugar para cualquier estructura

#define FLASH_UPDATE_FWFILE   1536              // Posición de la flash donde se graba el FW de update
#define FLASH_FACTORY_FWFILE  1792              // Posición de la flash donde se graba el FW de fábrica

//Estructura de todos los mensajes : Encabezado + Data
struct TConfigFrame{
        unsigned char   Len;
        unsigned char   OpCode;
        unsigned char   TxWindow;
        unsigned char   RxWindow;
        unsigned short  Parameter;
        unsigned short  AuxParam;
        unsigned char   Data[MAX_MSG_DATA];
};

#define OverHeadCfgSize    (sizeof(TConfigFrame)- MAX_MSG_DATA)         // Tamaño del header de los mensajes
#define MAX_MSG_SIZE       (sizeof(TConfigFrame)+1)                     // Maximo tamaño de un mensaje
#define MAX_FW_REG          256                 // Cantidad máxima de registros reservados para un archivo de FW

// Mensaje con informacion sobre la version del modulo
#define SIZE_FW_VER     15
#define SIZE_HW_VER     15
#define SIZE_SERIAL     15
#define SIZE_FW_FULL    30


// Nuevo Formato para almacenar datos de versión de FW.
//
struct TCfgFwVersion {
        char   FW_Version[SIZE_FW_VER+1];
        char   DateOfCompilation[20];
        char   TimeOfCompilation[10];
        UINT8  FW_Type;
        char   FW_BackWardCompatibility;
        char  FW_VersionFullName[SIZE_FW_FULL+1];
        UINT8  Reservado[21];
};

struct TCfgFwHeader {
        TCfgFwVersion   VersionInfo;
        UINT8           ReservedQttyWord;  
        UINT8           RegInFlashQtty;  
        UINT8           BackwardCompatibility;
        UINT8           KeyGenCode[8];  
        char            RestrictFromSerial[10];
        char            RestrictToSerial[10];
        UINT32          TrialMsgLimit;
        UINT32          TrialTimeLimit;
        char            FW_Autor[30];
        char            FW_Info[50];
};

// Mensaje con datos de las versiones instaladas
struct TMsgVersion{
        TCfgFwVersion   FW_VersionHdr;
//        UINT8   FW_Version[SIZE_FW_VER+1];
        UINT8   HW_Version[SIZE_HW_VER+1];
        UINT8   Serial[SIZE_SERIAL+1];
        unsigned short  KeyCode;

};

// Mensaje con datos de constantes almacenadas en EEPROM
#define SIZE_USR_TXT     30
struct TMsgConst{
        float           ConsignaFrec;
        float           RelacionTx;
        float           CoefResbal;
        float           RpmEnMotor;
        UINT8           UsrText[SIZE_USR_TXT];
};

// Valores de las variables de entrada salida
struct TMsgGioState{
	float   AAG_Input_A;       // Entrada analógica 1
	float   AAG_Input_B;       // Entrada analógica 2
	float   AAG_Output_A;      // Salida analógica 1
	float   AAG_Output_B;      // Salida analógica 2
	float   Temperatura;       // Temperatura interna
	UINT8   DigInpVal;         // 28 - Estado de entradas
	UINT8   DigOutVal;         // 29 - Estado de salidas
};

// Estructura de mensaje con informacion de Configuracion de puertos Serie
struct TMsgSerialCfg{
        UINT8   SCIA_ConfigCode;
        UINT8   SCIA_ConfigNode;
        UINT16  SCIA_ConfigScanRate;
        UINT8   SCIB_ConfigCode;
        UINT8   SCIB_ConfigNode;
        UINT16  SCIB_ConfigScanRate;
        UINT16  SCIA_TmrTxWait;
        UINT16  SCIA_TmrRxWait;
        UINT16  SCIA_TmrDrive;
        UINT16  SCIB_TmrTxWait;
        UINT16  SCIB_TmrRxWait;
        UINT16  SCIB_TmrDrive;             
};


//---------DEFINICIONES DE MASCARAS PARA PALABRA DE CONFIGURACION DE LOS PUERTOS SCI----
//
#define CFG_SCI_BAUDRATE_MASK   0x0F      // 0000-0111: 300-38400    

#define CFG_SCI_PARITY_MASK     0x30			// 01:impar, 10:par, 11:sin paridad
#define CFG_SCI_PARITY_EVEN     0X20
#define CFG_SCI_PARITY_ODD      0X10
#define CFG_SCI_PARITY_NONE     0X30

#define CFG_SCI_DATABITS_MASK   0x40      // 0: 8 bits, 1: 7 bits
#define CFG_SCI_PHYMODE_MASK    0x80      // 0: RS232, 1: RS485

// Definiciones de estructuras para almacenamiento de paginas web en flash
#define SIZE_WEB_FILENAME       24              // Longitud máxima del nombre de archivo
#define SIZE_WEB_FATENTRIES     100             // Cantidad de entradas en la FAT
#define MAX_WEB_REG             512             // Cantidad de registros reservadas para web (512x256=128KB)
#define SIZE_WEB_REG            256             // Tamaño de los regisitros en flash


struct TCfgWebFAT {
        char            Filename[SIZE_WEB_FILENAME];
        unsigned int    FileLenght;
        unsigned int    FileAddr;
};

struct TCfgWebFiles {
        TCfgWebFAT      WebFat[SIZE_WEB_FATENTRIES];
        unsigned char   WebReg[MAX_WEB_REG][SIZE_WEB_REG];
};

#define CHG_CFG_ENDIAN(var)     { unsigned short TmpEndian;\
                                TmpEndian = (var & 0xFF00)>>8;\
                                var = (var & 0x00FF)<<8;\
                                var |= TmpEndian;}

#define CHG_DWORD_ENDIAN(var)   { unsigned char TmpBytes[4];\
                                TmpBytes[0]=(var & 0x000000FF);\
                                TmpBytes[1]=(var & 0x0000FF00)>>8;\
                                TmpBytes[2]=(var & 0x00FF0000)>>16;\
                                TmpBytes[3]=(var & 0xFF000000)>>24;\
                                var = TmpBytes[3];\
                                var |= (TmpBytes[2]<<8);\
                                var |= (TmpBytes[1]<<16);\
                                var |= (TmpBytes[0]<<24);}

#define CHG_FLOAT_ENDIAN(var)   { unsigned char TmpBytes[4];\
                                unsigned int * TmpFloat;\
                                TmpFloat = (unsigned int *)&var;\
                                TmpBytes[0]=(*TmpFloat & 0x000000FF);\
                                TmpBytes[1]=(*TmpFloat & 0x0000FF00)>>8;\
                                TmpBytes[2]=(*TmpFloat & 0x00FF0000)>>16;\
                                TmpBytes[3]=(*TmpFloat & 0xFF000000)>>24;\
                                *TmpFloat = TmpBytes[3];\
                                *TmpFloat |= (TmpBytes[2]<<8);\
                                *TmpFloat |= (TmpBytes[1]<<16);\
                                *TmpFloat |= (TmpBytes[0]<<24);}


#endif
