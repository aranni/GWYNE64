//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde MAINPROC
//-----------------------------------------------------------------------------
#ifndef mainH
#define mainH
// FUNCIONES
extern UINT8 GetEventMsg (void);
extern void PutEventMsg (UINT8 PutEventMsg, UINT8 PutTarget, UINT8 PutSource, UINT16 PutParam);
extern void f_Consume (void);

// VARIABLES
extern UINT16             TimerVal;
extern UINT16             PeriodicTimer;
extern UINT16	            DummyVar;
extern void *             DummyPtr;

//extern UINT8             SCI1_InBuff [50];
//extern UINT8             SCI1_OutBuff[50];
//extern UINT8             SCI2_InBuff [MAX_SERIAL_IN_BUFLEN];
//extern UINT8             SCI2_OutBuff[MAX_SERIAL_OUT_BUFLEN];

//extern INVERTER_PARAM     InverterParam;      // Parametros del inversor

extern SYS_CONFIG_PARAM   SYS_ConfigParam;    // Parametros de configuracion
extern STATISTIC_MSG      StatisticData;			// Variables de Estadisticas
extern UINT32              dwIntegrityMonitor;

extern UINT8 bac_data[MAX_BAC_BUF];

extern UINT8  HwVer[SIZE_HW_VER+1];
extern UINT8  HwSerial[SIZE_SERIAL+1];

extern EVENT_MSG	   	CurrentMsg;							              // Mensaje actual analizandose.
extern EVENT_FUNCTION * FirstEventInCurrentState;	          // EVENTO en Estado Actual de Current Process.
extern EVENT_FUNCTION * CurrentProcessState [PROCESS_QTTY];	// Array de Punteros a Estados de los procesos
#endif

//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde MEMO
//-----------------------------------------------------------------------------
#ifndef sMemoH
#define sMemoH

// VARIABLES

// FUNCIONES
extern void   SYS_MemoInit       (void);

extern RETCODE MemGetBuffer    ( void** sppBufferPtrAddress, UINT8* bpToken);
extern RETCODE MemFreeBuffer   (UINT8 bToken);
extern RETCODE MemGetBufferPtr (UINT8 bToken, void** vppBuffer);

#endif


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde TIMER
//-----------------------------------------------------------------------------
#ifndef sTimerH
#define sTimerH
extern void 	TMR_Init 		(void);
//extern UINT8 	TMR_SetTimer 	(UINT16 Interval, UINT8 Owner);
extern UINT8 	TMR_SetTimer 	( UINT16 Interval, UINT8 Owner, void(*FuncPrt)(void), UINT8 Periodic);
extern void 	TMR_FreeTimer 	(UINT8 TimerId);
extern void 	TMR_ReloadTimer (UINT8 TimerId, UINT16 Interval);
extern void 	TMR_Wait 		    (UINT16 DelayMS);

#endif



//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde AUXFUN
//-----------------------------------------------------------------------------
#ifndef sAuxFunH
#define sAuxFunH

// VARIABLES
extern const UINT8 SYS_HexChar[];    	  
extern const UINT16 BaudCode2RegVal[];

// FUNCIONES
extern void   SYS_AuxGlobalInit       (void);

extern float   SYS_StrToFloat         ( UINT8 IntLen, UINT8 DecLen, UINT8 * DataStr );
extern UINT16  SYS_StrToInt           ( UINT8 IntLen, UINT8 * DataStr );
extern UINT16  SYS_StrxToInt          ( UINT8 IntLen, UINT8 * DataStr );
extern void    SYS_IntToStr           ( UINT16 val,   UINT8 ndigit,  UINT8 * DataStr );

#endif


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde TIMER
//-----------------------------------------------------------------------------
#ifndef sTimerH
#define sTimerH
extern void 	TMR_Init 		(void);
//extern UINT8 	TMR_SetTimer 	(UINT16 Interval, UINT8 Owner);
extern UINT8 	TMR_SetTimer 	( UINT16 Interval, UINT8 Owner, void(*FuncPrt)(void), UINT8 Periodic);
extern void 	TMR_FreeTimer 	(UINT8 TimerId);
extern void 	TMR_ReloadTimer (UINT8 TimerId, UINT16 Interval);
extern void 	TMR_Wait 		    (UINT16 DelayMS);

#endif


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde DEBUG
//-----------------------------------------------------------------------------
#ifndef sDebugH
#define sDebugH
// VARIABLES
extern char DebugBuffer [MAX_DEBUGBUF_LEN + 1];

extern const EVENT_FUNCTION	DebugProc_IDLE [];

// FUNCIONES
extern void DBG_Init(void);
extern void MON_ResetBoard (void);
extern void MON_ResetIfNotConnected (void);
#endif


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde driver SPI
//-----------------------------------------------------------------------------
#ifndef dSPIH
#define dSPIH
extern void init_serial_memories (void);
extern void eeprom_read_buffer(unsigned int addr, unsigned char *buf, int len);
extern void eeprom_write_buffer(unsigned int addr, unsigned char *buf, int len);
extern void flash_program     (unsigned long addr, int len, unsigned char (*flash_callback_write)(unsigned long));
extern void flash_read        (unsigned long addr, int len, void (*flash_callback_read)(unsigned long, unsigned char));
extern void flash_read_buffer (unsigned long addr, unsigned char *buf, int len);
extern void flash_bulk_erase  (void);

extern void   FLA_Init      (void);
extern void   FLA_SaveLog   (UINT8 Param);
extern void   FLA_SaveLogFixed (UINT8 dLogId);
extern UINT16  FLA_ReadLog   (UINT8* pReadLogBuffer);
extern UINT16  FLA_ResetLogReadCntr (UINT16 wBackIdx);
extern void   FLA_ResetLogPtr (void);

extern void   FLA_SaveReg   (UINT16 RegIdx, UINT16 DataLen, UINT8* DataSourcePtr);
extern void   FLA_ReadReg   (UINT16 RegIdx, UINT16 DataLen, UINT8* DataTargetPtr);
extern void   FLA_ReadBuff  (UINT32 Addr, UINT16 DataLen, UINT8* DataTargetPtr);
extern void   FLA_ReadBlock  (UINT32 Addr, UINT16 DataLen, UINT8* DataTargetPtr);

#endif


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde driver TCP
//-----------------------------------------------------------------------------
#ifndef dTCPH
#define dTCPH
// VARIABLES
extern UINT8          TcpConfigState;
extern INT8           TcpSerialSoch;						// Serial Server Socket handle	
extern SRV_TCP_SOCKET TcpSockSrv[MAX_SRV_TCPSOCH];

#ifdef UDP_DEBUG_PRINTFON
extern UINT8          udp_buf[MAX_UDP_FRAME_LEN];
extern UINT8          udp_data_to_send;
extern UINT32         udp_ipaddr;
extern UINT16         udp_port;
#endif

// FUNCIONES
extern UINT8  TCP_SendCfgBuff   (void);
extern UINT8  TCP_SendCliTcpBuff(UINT8 * Buff, UINT16 BufLen);
extern UINT8  TCP_SendSrvTcpBuff(UINT8 * Buff, UINT16 BufLen, UINT8 SocketIdx);

extern UINT8 TCP_VerifyCliTcpConnection(void);

extern void TCP_Init           (void);     
extern void UDP_Init           (void); 
extern void UDP_Loop           (void);


#endif


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde driver IIC
//-----------------------------------------------------------------------------
#ifndef dIIH
#define dIIH
// VARIABLES
extern const EVENT_FUNCTION	IICProc_IDLE [];

// FUNCIONES
extern UINT8 IIC_SendMsg 		  (UINT8 SlaveAddress, UINT8 LenTx, UINT8 LenRx);
extern UINT8 IIC_ReceiveMsg 	(UINT8 SlaveAddress, UINT8 LenRx);

extern void IIC_WriteRTCMem  (UINT8 Len, UINT8 Addr, UINT8* DataPtr);
extern void IIC_ReadRTCMem   (UINT8 Len, UINT8 Addr, UINT8* DataPtr);

extern void IIC_RtcGetDateTime  (void);
extern void IIC_RtcSetDateTime  (void);

extern void IIC_DacSetOutput    (UINT8 Chan, UINT8 OutVal);
extern void IIC_DacSetValues    (void);

extern void IIC_Init 		(void);
#endif


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde driver SCI A
//-----------------------------------------------------------------------------
#ifndef dSCIAH
#define dSCIAH
// VARIABLES
extern const EVENT_FUNCTION	SCIAProc_IDLE [];

// FUNCIONES
extern void SCIA_ReceiveFrame     ( UINT16 RxLen, UINT8 RxMsgToken);
extern RETCODE SCIA_TransmitFrame ( UINT16 TxLen, UINT8 TxMsgToken);
extern RETCODE SCIA_TransmitBuffer ( UINT16 TxLen, UINT8* TxMsgBuffer); 

extern void SCIA_Init 		(void);
extern void SCIA_ReInit  (void);
#endif


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde driver SCI B
//-----------------------------------------------------------------------------
#ifndef dSCIBH
#define dSCIBH
// VARIABLES
extern const EVENT_FUNCTION	SCIBProc_IDLE [];

// FUNCIONES
extern void SCIB_ReceiveFrame     ( UINT16 RxLen, UINT8 RxMsgToken);
extern RETCODE SCIB_TransmitFrame ( UINT16 TxLen, UINT8 TxMsgToken);
extern RETCODE SCIB_TransmitBuffer ( UINT16 TxLen, UINT8* TxMsgBuffer); 

extern void SCIB_Init 		(void);
extern void SCIB_ReInit   (void);
#endif

//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde driver GPIO
//-----------------------------------------------------------------------------
#ifndef dGPIOH
#define dGPIOH

// VARIABLES
extern GIO_STATE_VAL     InputOutputData;
extern const float AAG_4_20_OUT_FACTOR;

// FUNCIONES
extern void   GIO_GetInpVal   (void);
extern void   GIO_SetOutVal   (UINT8 OutputVal); 

extern UINT16  GIO_AdcRead     (UINT8 Chan);
extern void   GIO_GetTemp     (void);
extern void   GIO_GetAAGInp   (void);
extern void   GIO_SetAAGOut   (UINT8 Chan);

extern void   GIO_Init  (void);

#endif


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde protocolo CONFIG
//-----------------------------------------------------------------------------
#ifndef pConfigH
#define pConfigH

// VARIABLES
extern CFG_MSG_FRAME    CfgMsgFrame;
extern CFG_MSG_INFO     CfgMsgInfo;

extern const struct CFG_FW_VERSION FwVersionData;

extern const EVENT_FUNCTION	ConfigProc_IDLE [];

// FUNCIONES
extern void CFG_Init 		(void);
#endif


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde protocolo MODBUS TCP
//-----------------------------------------------------------------------------
#ifndef ModbusTCPProcH
#define ModbusTCPProcH

// VARIABLES
extern MODBUS_MSG	   	 	ModbusInMsg;				// Mensaje recibido por ModbusTCP.
//extern MODBUS_MSG	   	 	ModbusOutMsg;				// Respuesta al requerimiento recibido

extern const EVENT_FUNCTION	ModbusProc_IDLE [];

// FUNCIONES
extern void MOD_Init(void);
#endif


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde protocolo BAC 
//-----------------------------------------------------------------------------
#ifndef MtsH
#define MtsH

// VARIABLES

extern const EVENT_FUNCTION	BACProc_IDLE [];

extern BAC_BANK_ALLOC  BAC_BankMemManager[MAX_BAC_BANKS];
extern BAC_PROC_DIAG   BAC_Diag;

extern UINT16  BAC_PollingPeriod;				  // Periodo de polling en milisegundos
extern UINT8   BAC_CtrlFlags;				      // Flags

// FUNCIONES
extern void BAC_SendAppMsg    ( UINT8 DataLen, UINT8 TxMsgToken);
extern void BAC_ChgWordEndian ( UINT16* DataWord);
extern void BAC_Init          (void);
extern void BAC_ReInit        (void);
extern void BAC_ResetBanks    (void); 

#endif

/*
//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde protocolo SERIAL
//-----------------------------------------------------------------------------
#ifndef SerialH
#define SerialH

// VARIABLES

extern const EVENT_FUNCTION	SerialProc_IDLE [];

// FUNCIONES
extern void  SerialInPutByte          (UINT8 DataByte);
extern UINT8 SerialOutGetByteAndQuit  (UINT8* BytePtr);
extern void  SerialOutPutByte         (UINT8 DataByte);

extern void SRL_Init      (void);
extern void SRL_ReInit    (void);

#endif
*/

//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde protocolo DYNAMIC_TOOLS
//-----------------------------------------------------------------------------
#ifndef DToolsH
#define DToolsH

// VARIABLES


// FUNCIONES
extern float ptrToFloat(char *ptrA);

#endif		//DToolsH





