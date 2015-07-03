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

extern UINT8             SCI1_InBuff [MAX_SERIAL_IN_BUFLEN];
extern UINT8             SCI1_OutBuff[MAX_SERIAL_OUT_BUFLEN];
extern UINT8             SCI2_InBuff [MAX_SERIAL_IN_BUFLEN];
extern UINT8             SCI2_OutBuff[MAX_SERIAL_OUT_BUFLEN];

extern INVERTER_PARAM     InverterParam;      // Parametros del inversor
extern SYS_CONFIG_PARAM   SYS_ConfigParam;    // Parametros de configuracion
extern STATISTIC_MSG      StatisticData;			// Variables de Estadisticas
extern INV_CONFIG_PARAM   INV_ConfigParam;    // Parametros de configuracion del variador.

extern EVENT_MSG	   	CurrentMsg;							              // Mensaje actual analizandose.
extern EVENT_FUNCTION * FirstEventInCurrentState;	          // EVENTO en Estado Actual de Current Process.
extern EVENT_FUNCTION * CurrentProcessState [PROCESS_QTTY];	// Array de Punteros a Estados de los procesos
#endif


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde AUXFUN
//-----------------------------------------------------------------------------
#ifndef sAuxFunH
#define sAuxFunH

// VARIABLES
extern const UINT8 SYS_HexChar[];    	  

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
extern void   FLA_ReadBuff  (UINT32 Addr, UINT16 DataLen, UINT8* DataTargetPtr);
extern void   FLA_ReadBlock  (UINT32 Addr, UINT16 DataLen, UINT8* DataTargetPtr);

extern void   FLA_Init      (void);
extern void   FLA_SaveLog   (UINT8 Param);
extern UINT8  FLA_ReadLog   (void);
extern void   FLA_SaveReg   (UINT16 RegIdx, UINT8 DataLen, UINT8* DataSourcePtr);
extern void   FLA_ReadReg   (UINT16 RegIdx, UINT8 DataLen, UINT8* DataTargetPtr);

#endif


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde driver TCP
//-----------------------------------------------------------------------------
#ifndef dTCPH
#define dTCPH

extern UINT8          TcpConfigState;

extern UINT8  TCP_SendModbusMsg (void);
extern UINT8  TCP_SendCfgBuff   (void);

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
extern const EVENT_FUNCTION	SerialProc_IDLE [];

// FUNCIONES
extern void SRL_Init 		(void);
extern void SRL_ReInit  (void);
#endif


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde driver SCI B
//-----------------------------------------------------------------------------
#ifndef dSCIBH
#define dSCIBH
// VARIABLES
extern const EVENT_FUNCTION	RtuProc_IDLE [];

// FUNCIONES
extern void RTU_TransmitFrame ( UINT8 Len, UINT8 * MsgPtr); 
extern void RTU_ReceiveFrame  ( UINT8 Len, UINT8 * MsgPtr);

extern void RTU_Init 		(void);
extern void RTU_ReInit  (void);
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

extern UINT8  GIO_AdcRead     (UINT8 Chan);
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
extern MODBUS_MSG	   	 	ModbusOutMsg;				// Respuesta al requerimiento recibido

extern const EVENT_FUNCTION	ModbusProc_IDLE [];

// FUNCIONES
extern void MOD_Init(void);
#endif


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde protocolo MODBUS RTU
//-----------------------------------------------------------------------------
#ifndef ModbusRTUProcH
#define ModbusRTUProcH

// VARIABLES
extern MODBUS_RTU_MSG*	   	pModRtuInMsg;					// Mensaje recibido por ModbusRTU.
extern MODBUS_RTU_MSG*  	 	pModRtuOutMsg;				// Respuesta al requerimiento recibido

extern const EVENT_FUNCTION	ModRtuProc_IDLE [];

// FUNCIONES
extern void MRT_Init(void);
#endif


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde protocolo SIEMENS MICROMASTER
//-----------------------------------------------------------------------------
#ifndef SieMicroMstProcH
#define SieMicroMstProcH

// VARIABLES
extern SIEMENS_MSG*	   	 	pSiemensInMsg;				// Mensaje recibido por Siemens.
extern SIEMENS_MSG*	   	 	pSiemensOutMsg;			  // Respuesta al requerimiento recibido
extern INV_CONFIG_PARAM*  pSIE_ConfigParam;

extern const EVENT_FUNCTION	SiemensProc_IDLE [];

// FUNCIONES
extern void SIE_Init(void);
#endif


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde protocolo SIEMENS MASTER DRIVE
//-----------------------------------------------------------------------------
#ifndef SieMasterDrvProcH
#define SieMasterDrvProcH

// VARIABLES
extern SIEMENS_MSG*	   	 	pSieMasterDrvInMsg;				// Mensaje recibido por Siemens.
extern SIEMENS_MSG*	   	 	pSieMasterDrvOutMsg;			// Respuesta al requerimiento recibido
extern INV_CONFIG_PARAM*  pSMD_ConfigParam;

extern const EVENT_FUNCTION	SieMasterDrvProc_IDLE [];

// FUNCIONES
extern void SMD_Init(void);
#endif


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde SIMULADOR de SIEMENS MASTER DRIVE
//-----------------------------------------------------------------------------
#ifndef SimSieMasterDrvProcH
#define SimSieMasterDrvProcH

// VARIABLES
extern SIEMENS_MSG*	   	 	pSimSieMasterDrvInMsg;			// Mensaje recibido por Siemens.
extern SIEMENS_MSG*	   	 	pSimSieMasterDrvOutMsg;			// Respuesta al requerimiento recibido
extern INV_CONFIG_PARAM*  pSimSMD_ConfigParam;

extern const EVENT_FUNCTION	SimSieMasterDrvProc_IDLE [];

// FUNCIONES
extern void SimSMD_Init(void);
#endif


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde protocolo HITACHI J300
//-----------------------------------------------------------------------------
#ifndef HitJ300H
#define HitJ300H

// VARIABLES
extern HITACHI_IN_MSG*	   	pHitachiInMsg;			// Mensaje recibido por Hitachi.
extern HITACHI_OUT_MSG*	    pHitachiOutMsg;			// Respuesta al requerimiento recibido

extern const EVENT_FUNCTION	HitachiProc_IDLE [];

// FUNCIONES
extern float   HIT_StrToFloat         ( UINT8 IntLen, UINT8 DecLen, UINT8 * DataStr );
extern UINT16  HIT_StrToInt           ( UINT8 IntLen, UINT8 * DataStr );
extern UINT16  HIT_StrxToInt          ( UINT8 IntLen, UINT8 * DataStr );
extern void    HIT_IntToStr           ( UINT16 val,   UINT8 ndigit,  UINT8 * DataStr );

extern void HIT_Init(void);

#endif


//-----------------------------------------------------------------------------
// Funciones y Variables exportadas desde protocolo HITACHI SJ300
//-----------------------------------------------------------------------------
#ifndef HitSJ300H
#define HitSJ300H

// VARIABLES
extern HITSJ300_IN_MSG*	   	pHitSJ300InMsg;			// Mensaje recibido por Hitachi.
extern HITSJ300_OUT_MSG*	  pHitSJ300OutMsg;		// Respuesta al requerimiento recibido

extern const EVENT_FUNCTION	HitSJ300Proc_IDLE [];

// FUNCIONES
extern void HSJ_Init(void);
#endif




