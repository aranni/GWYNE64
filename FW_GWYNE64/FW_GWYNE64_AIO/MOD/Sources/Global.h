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
extern UINT16	            DummyVar;
extern void *             DummyPtr;

extern SYS_CONFIG_PARAM   SYS_ConfigParam;    // Parametros de configuracion
extern STATISTIC_MSG      StatisticData;			// Variables de Estadisticas
//extern UINT32             dwIntegrityMonitor;
extern UINT8              ProcDebugFlag[PROCESS_QTTY];

#ifdef MOD_MODBUSRTU_AUTOQUERY_ENABLED		
  extern UINT8             autoQueryModbusData[MAX_AUTOQUERY_BUF];
#endif														

#ifdef MOD_SYMEO_ENABLED		
//  extern UINT8             symeoData[MAX_SYMEO_BUF];
#endif														

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

extern BYTE    MemGetFreeBuffQtty (void);
extern RETCODE MemGetBuffer    ( void** sppBufferPtrAddress, UINT8* bpToken);
extern RETCODE MemFreeBuffer   (UINT8 bToken);
extern RETCODE MemFreeMarkBuffer (UINT8 bMark);
extern RETCODE MemGetBufferPtr (UINT8 bToken, void** vppBuffer);
extern RETCODE MemSetUsedLen   (UINT8 bToken, UINT16 wLen);
extern RETCODE MemGetUsedLen (UINT8 bToken,  UINT16* wpLen);
extern RETCODE MemSetRouteMark (UINT8 bToken, UINT8 bMark);
extern RETCODE MemGetRouteMark (UINT8 bToken,  UINT8* bMark);

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
// Funciones y Variables exportadas desde QUEUE
//-----------------------------------------------------------------------------
#ifndef sQueueH
#define sQueueH

// VARIABLES
//

// FUNCIONES
extern UINT8 QUE_GetByte             (FIFO_CNTRL* qIdx, UINT8* BytePtr);
extern UINT8 QUE_GetByteAndQuit      (FIFO_CNTRL* qIdx, UINT8* BytePtr);
extern UINT8 QUE_PutByte             (FIFO_CNTRL* qIdx, UINT8 DataByte);

extern void  QUE_Init                (FIFO_CNTRL* qIdx);

#endif  // sQueueH


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
extern UINT32  dwUpTimeHalfSeg;

extern const EVENT_FUNCTION	DebugProc_IDLE [];


// FUNCIONES
extern void DBG_Init(void);
extern void MON_ResetBoard (BYTE bResetReason);
extern void MON_ResetIfNotConnected (BYTE bResetReason);
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
extern void   FLA_SaveLogFixed (UINT8 dLogId, UINT8 bSourceProc);
extern UINT16 FLA_ReadLog   (UINT8* pReadLogBuffer);
extern UINT16 FLA_ResetLogReadCntr (UINT16 wSpakIdx);
extern void   FLA_ResetLogPtr (void);

extern void   FLA_SaveLogEvent      (UINT8 Param);
extern void   FLA_SaveLogEventFixed (UINT8 bEventCode, UINT8 bSourceProc);
extern UINT16 FLA_ReadLogEvent      (UINT8* pReadLogBuffer);
extern UINT16 FLA_ResetLogEventReadCntr (UINT16 wSpakIdx);
extern void   FLA_ResetLogEventPtr  (void);

extern void   FLA_SaveReg   (UINT16 RegIdx, UINT16 DataLen, UINT8* DataSourcePtr);
extern void   FLA_ReadReg   (UINT16 RegIdx, UINT16 DataLen, UINT8* DataTargetPtr);
extern void   FLA_ReadBuff  (UINT32 Addr, UINT16 DataLen, UINT8* DataTargetPtr);
extern void   FLA_ReadBlock  (UINT32 Addr, UINT16 DataLen, UINT8* DataTargetPtr);

extern RETCODE FLA_SearchStateEvent  (UINT8 LogEventToken);
extern RETCODE FLA_ChangeStateEvent  (UINT16 wEventId, UINT8 bEventState);

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

extern void SCIA_Init 		(BYTE TargetProc, BYTE  ProtocolFrame);
extern void SCIA_ReInit   (BYTE TargetProc, BYTE  ProtocolFrame);
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

//extern void SCIB_Init 		(BYTE TargetProc, BYTE  ProtocolFrame);
//extern void SCIB_ReInit   (BYTE TargetProc, BYTE  ProtocolFrame);
extern void SCIB_Init          (BYTE TargetProc, BYTE  ProtocolFrame, BYTE _SCI_RX_MSG, BYTE _SCI_RX_ERR, BYTE _SCI_TX_MSG, BYTE _SCI_TX_ERR, BYTE _SCI_TX_BUSY);
extern void SCIB_ReInit        (BYTE TargetProc, BYTE  ProtocolFrame, BYTE _SCI_RX_MSG, BYTE _SCI_RX_ERR, BYTE _SCI_TX_MSG, BYTE _SCI_TX_ERR, BYTE _SCI_TX_BUSY);
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
extern UINT8  GIO_GetInpVal   (void);
extern void   GIO_SetOutVal   (UINT8 OutputVal); 

extern UINT16 GIO_AdcRead     (UINT8 Chan);
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
// Funciones y Variables exportadas desde AJAX TOOLS PARA SERVIDOR WEB
//-----------------------------------------------------------------------------
#ifndef wAjaxToolsH
#define wAjaxToolsH

// VARIABLES
extern BYTE    sndbuf[450];
extern WORD    LogParam;
extern BYTE    TagFlags;
extern WORD    TagPolFrec;

// CONSTANTES
extern const char https_ack_page_OK        [];
extern const char https_ack_page_Accepted  [];
extern const char https_ack_page_Void      [];
extern const char https_ack_page_Reset     [];
extern const char https_ack_page_Bad       [];

extern const char scsHeader     [];
extern const char scsTrailer    [];

extern const TFileEntry FAT     [];

// FUNCIONES
extern float ptrToFloat(char *ptrA);

extern void LoadValue(char *tmpbuf, int iBufSize, USR_DYNAMIC_FIELDS *ptrUDI);
extern void SaveValue(char *ptr, USR_DYNAMIC_FIELDS *ptrUDI);

extern UINT16 GetNameValuePair(UINT16 wLen, char* pszName, char* pszValue);
extern USR_DYNAMIC_FIELDS* FindTableName(char* pszName, USR_DYNAMIC_FIELDS *ptrUDI);
extern USR_DYNAMIC_FIELDS* FindTableNameFull(char* pszName, USR_DYNAMIC_FIELDS *ptrUDI);

extern char* XMLEntityOut(char* myData, char* sOut);


#endif		//wAjaxToolsH


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde driver TCP - UDP
//-----------------------------------------------------------------------------
//
//------------------------------ TCP CLIENT SOCKETS ---------------------------
#ifdef  TCP_SLAVE_MODE
#ifndef dTcpCliH	
#define dTcpCliH
// VARIABLES

// FUNCIONES
extern UINT8  TCP_SendCliTcpBuff(UINT8 * Buff, UINT16 BufLen);
extern UINT8 TCP_VerifyCliTcpConnection(void);

extern void TCP_InitCliSockets           (void);     

#endif			// dTcpCliH
#endif			// TCP_SLAVE_MODE
//------------------------------ TCP SERVER SOCKETS ---------------------------
#ifndef dTcpSrvH
#define dTcpSrvH
// VARIABLES
extern UINT8          TcpConfigState;
#ifdef  TCP_MASTER_MODE
extern SRV_TCP_SOCKET TcpSockSrv[MAX_SRV_TCPSOCH];
#endif			// TCP_MASTER_MODE

// FUNCIONES
extern UINT8  TCP_SendCfgBuff   (void);
#ifdef  TCP_MASTER_MODE
extern UINT8  TCP_SendSrvTcpBuff(UINT8 * Buff, UINT16 BufLen, UINT8 SocketIdx);
#endif			// TCP_MASTER_MODE

extern void TCP_InitSrvSockets       	(void);     

#endif			// dTcpSrvH

//------------------------------ UDP DEBUG SOCKET -----------------------------------
#ifdef  UDP_DEBUG_PRINTFON
#ifndef dUdpDebugH
#define dUdpDebugH
// VARIABLES

extern UINT8          udp_buf[MAX_UDP_FRAME_LEN];
extern UINT8          udp_data_to_send;
extern UINT32         udp_ipaddr;
extern UINT16         udp_port;

// FUNCIONES
extern void UDP_Init           (void); 
extern void UDP_Loop           (void);

#endif			// dUdpDebugH
#endif      // UDP_DEBUG_PRINTFON

//------------------------------ UDP CLIENT SOCKET -----------------------------------
#ifdef  UDP_CLIENT_MODE
#ifndef dUdpCliH
#define dUdpCliH
// VARIABLES

extern UINT8          udp_cli_buf[MAX_UDP_FRAME_LEN];
extern UINT8          udp_cli_data_to_send;
extern UINT32         udp_cli_ipaddr;
extern UINT16         udp_cli_port;

// FUNCIONES
extern void UDP_CliInit           (void); 
extern void UDP_CliLoop           (void);

#endif			// dUdpCliH
#endif      // UDP_CLIENT_MODE


//-----------------------------------------------------------------------------
//----------------------------PROCESOS-----------------------------------------
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde proceso NEXO
//-----------------------------------------------------------------------------
#ifndef pNexusProcH
#define pNexusProcH

// VARIABLES

extern const EVENT_FUNCTION	NexusProc_IDLE [];

// FUNCIONES

#if  (FW_APP_BUILT == FW_APP_MODBUS)
  extern void  NEX_ModbusRtuBuildCRC16 ( UINT8 * puchMsg, UINT16 usDataLen );
#endif

extern void NEX_Init(void);
#endif


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde protocolo MODBUS TCP SLAVE	  
//-----------------------------------------------------------------------------
#ifdef  MOD_MODBUSTCP_SLAVE_ENABLED
#ifndef ModbusSlaveTCPProcH
#define ModbusSlaveTCPProcH

// VARIABLES
extern UINT16  ModbusOffset;									  // offset de la operación WR o RD
extern UINT8   ValuesRequired;                 // cantidad de WORDS solicitadas

extern const EVENT_FUNCTION	ModbusSlaveTcpProc_IDLE [];

// FUNCIONES
extern void MOD_ModbusSlaveTcpInit(void);
#endif
#endif    // MOD_MODBUSTCP_SLAVE_ENABLED

//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde protocolo MODBUS TCP MASTER
//-----------------------------------------------------------------------------
#ifdef  MOD_MODBUSTCP_MASTER_ENABLED
#ifndef ModbusMasterTCPProcH
#define ModbusMasterTCPProcH

// VARIABLES
extern UINT16  ModbusMasterOffset;									  // offset de la operación WR o RD

extern const EVENT_FUNCTION	ModbusMasterTcpProc_IDLE [];

// FUNCIONES
extern void MOD_ModbusMasterTcpInit(void);
#endif
#endif    // MOD_MODBUSTCP_MASTER_ENABLED


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde protocolo UNITELWAY TCP
//-----------------------------------------------------------------------------
#ifdef  MOD_UNITELWAY_TCP_ENABLED
#ifndef UnitelwayTCPProcH
#define UnitelwayTCPProcH

// VARIABLES

extern const EVENT_FUNCTION	UnitelwayTcpProc_IDLE [];

// FUNCIONES
extern void MOD_UnitelwayTcpInit(void);
#endif
#endif    // MOD_UNITELWAY_ENABLED

//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde protocolo MODBUS RTU
//-----------------------------------------------------------------------------
#ifdef  MOD_MODBUSRTU_SLAVE_ENABLED
#ifndef ModbusSlaveRtuProcH
#define ModbusSlaveRtuProcH

// VARIABLES
extern MODBUS_MSG	   	 	ModbusInMsg;				// Mensaje recibido por ModbusTCP.

extern const EVENT_FUNCTION	ModbusSlaveRtuProc_IDLE [];

// FUNCIONES
extern void MOD_ModbusSlaveRtuInit(void);
#endif
#endif    // MOD_MODBUSRTU_SLAVE_ENABLED


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde protocolo MODBUS AUTO QUERY
//-----------------------------------------------------------------------------
#ifdef  MOD_MODBUSRTU_AUTOQUERY_ENABLED
#ifndef pModbusAutoQueryH
#define pModbusAutoQueryH

// VARIABLES
extern AUTOQUERY_BANK_ALLOC  MAQ_BankMemManager[MAX_AUTOQUERY_BANKS];

extern const EVENT_FUNCTION	MAQProc_IDLE [];

// FUNCIONES
extern void MOD_ModbusRtuAutoQueryInit        (void);
extern void MOD_ModbusRtuAutoQueryResetBanks  (void);
extern void MOD_ModbusRtuAutoQuerySaveBanks   (void);

#endif
#endif    // MOD_MODBUSRTU_AUTOQUERY_ENABLED


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde protocolo SYMEO
//-----------------------------------------------------------------------------
#ifdef  MOD_SYMEO_ENABLED
#ifndef pSymeoH
#define pSymeoH

// VARIABLES
extern SYMEO_BANK_ALLOC  SYM_BankMemManager[MAX_SYMEO_BANKS];

extern const EVENT_FUNCTION	SYMProc_IDLE [];

// FUNCIONES
void MOD_SymeoInit          (void);
//extern void MOD_ModbusRtuAutoQueryResetBanks  (void);
//extern void MOD_ModbusRtuAutoQuerySaveBanks   (void);

#endif
#endif    // MOD_MODBUSRTU_AUTOQUERY_ENABLED


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde proceso PIO
//-----------------------------------------------------------------------------
#ifdef  MOD_INPUT_OUTPUT_ENABLED
#ifndef pInputOutputH
#define pInputOutputH

// VARIABLES

extern const EVENT_FUNCTION	PioProc_IDLE [];

// FUNCIONES
extern void PIO_Init(void);

#endif		// pInputOutputH
#endif    // MOD_INPUT_OUTPUT_ENABLED


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde protocolo SPA 
//-----------------------------------------------------------------------------
#if  (FW_APP_BUILT == FW_APP_SPA)   // Aplicación para protololcos SPACOM de ABB
#ifndef SpaH
#define SpaH

// VARIABLES

extern const EVENT_FUNCTION	SPAProc_IDLE [];

extern UINT16  SPA_PollingPeriod;				  // Periodo de polling en milisegundos
extern UINT8   SPA_CtrlFlags;				      // Flags

// FUNCIONES
extern void SPA_SendAppMsg    ( UINT8 DataLen, UINT8 TxMsgToken);
extern void SPA_ChgWordEndian ( UINT16* DataWord);
extern void SPA_Init          (void);
extern void SPA_ReInit        (void);
extern void SPA_ResetBanks    (void); 

#endif
#endif  // (FW_APP_BUILT == FW_APP_SPA)  Aplicación para protololcos SPACOM de ABB






