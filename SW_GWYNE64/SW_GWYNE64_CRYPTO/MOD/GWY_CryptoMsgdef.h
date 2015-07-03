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
#define CFG_TCP_RESET           3
#define CFG_TCP_READ_STAT       4
#define CFG_TCP_READ_VER        5
#define CFG_TCP_WRITE_VER       6
#define CFG_TCP_READ_CTES       7
#define CFG_TCP_WRITE_CTES      8
#define CFG_TCP_WRITE_DATE      9
#define CFG_TCP_ACK             10
#define CFG_TCP_NAK             11
#define CFG_TCP_POL             12

#define CFG_TCP_READ_FLASH      15
#define CFG_TCP_WRITE_FLASH     16


#define CFG_RTC_RUNSTATE_MASK   0x01
#define CFG_RTU_RUNSTATE_MASK   0x02

#define   FW_FACTORY          0x01
#define   FW_UPDATE           0x02
#define   FW_OWN_RESET        0x04
#define   FW_TEST             0x08


#define	MAXIMO(x,y)		(x>y)?x:y

// Mensaje con datos de configuracion del canal TCP
struct TMsgTcpTimers{
        UINT8           var_TCP_INIT_RETRY_TOUT;
        UINT8           var_TCP_RETRY_TOUT;
        UINT8           var_TCP_DEF_RETRIES;
};

// Mensaje con datos de configuracion
struct TMsgInfo{
        unsigned char   InvCode;
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

#define MSG_INFO_SIZE   sizeof(TMsgInfo)
#define MSG_STAT_SIZE   sizeof(TMsgStat)
#define MAX_MSG_DATA    256                   // Controlar que haya lugar para cualquier estructura
#define FW_ADDR_INI     1536

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

#define OverHeadCfgSize    (sizeof(TConfigFrame)- MAX_MSG_DATA)
#define MAX_MSG_SIZE       (sizeof(TConfigFrame)+1)
#define MAX_FW_REG          256

// Mensaje con informacion sobre la version del modulo
#define SIZE_FW_VER     15
#define SIZE_HW_VER     15
#define SIZE_SERIAL     15

struct TMsgVersion{
        UINT8   FW_Version[SIZE_FW_VER+1];
        UINT8   HW_Version[SIZE_HW_VER+1];
        UINT8   Serial[SIZE_SERIAL+1];
        unsigned short  KeyCode;

};

// Nuevo Formato para almacenar datos de versión de FW.
struct TCfgFwVersion {
        char   FW_Version[SIZE_FW_VER+1];
        char   DateOfCompilation[20];
        char   TimeOfCompilation[10];
        UINT8  FW_Type;
        char   FW_BackWardCompatibility;
        UINT8  Reservado[52];
};

struct TCfgFwHeader {
        TCfgFwVersion   VersionInfo;                    // 100
        UINT8           ReservedQttyWord;               // 101
        UINT8           RegInFlashQtty;                 // 102

        UINT8           TransferInfo;                   // 103
        UINT8           RestrictType;                   // 104
        char            RestrictFromSerial[10];         // 114
        char            RestrictToSerial[10];           // 124
        UINT32          TrialMsgLimit;
        UINT32          TrialTimeLimit;
        UINT32          TrialOtherLimit;                // 136

        char            FW_Autor[20];                   // 156
        char            FW_Info[50];                    // 206
        UINT8           FW_ReservedSpace[30];           // 236
};

#define   FW_RESTRICT_FREE            0xAB              // Sin restricciones
#define   FW_RESTRICT_MSG             0xAC              //
#define   FW_RESTRICT_TIME            0xAD              //
#define   FW_RESTRICT_OTHER           0xAE              //
#define   FW_RESTRICT_SERIAL          0xAF              //


// Mensaje con datos de constantes almacenadas en EEPROM
#define SIZE_USR_TXT     30
struct TMsgConst{
        float           ConsignaFrec;
        float           RelacionTx;
        float           CoefResbal;
        float           RpmEnMotor;
        UINT8           UsrText[SIZE_USR_TXT];
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
