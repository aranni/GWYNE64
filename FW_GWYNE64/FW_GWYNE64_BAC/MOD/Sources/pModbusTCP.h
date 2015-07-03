
#ifndef ModbusTCPProcH
#define ModbusTCPProcH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define MOD_FC_READ_COILS      1        // Read Coils
#define MOD_FC_WRITE_COIL      5        // Write Coil
#define MOD_FC_RD_MUL_REG      3        // Read Multiple Register
#define MOD_FC_WR_MUL_REG     16        // Write Multiple Register
#define MOD_FC_WR_SIN_REG      6        // Write Single Register
  

// Offsets para particion de tabla de lectura de registros Read Multiple Registers
#define MOD_REG_SYS_OFFSET_IDX    6         //  600: Variables del sistema GWY
#define MOD_REG_STA_OFFSET_IDX    7         //  700: Estadisticas  
#define MOD_REG_GIO_OFFSET_IDX    8			    //  800: Entradas salidas GWY 

#define MOD_REG_VAL_OFFSET_IDX    10		    //  41000 : banco 0

#define MOD_REG_SYS_OFFSET      (100 * MOD_REG_SYS_OFFSET_IDX)
#define MOD_REG_SYS_OFFSET_BY   (2 * MOD_REG_SYS_OFFSET)

#define MOD_REG_STA_OFFSET      (100 * MOD_REG_STA_OFFSET_IDX)
#define MOD_REG_STA_OFFSET_BY   (2 * MOD_REG_STA_OFFSET)

#define MOD_REG_GIO_OFFSET      (100 * MOD_REG_GIO_OFFSET_IDX)
#define MOD_REG_GIO_OFFSET_BY   (2 * MOD_REG_GIO_OFFSET)

#define MOD_REG_VAL_OFFSET      (100 * MOD_REG_VAL_OFFSET_IDX)
#define MOD_REG_VAL_OFFSET_BY   (2 * MOD_REG_VAL_OFFSET)

#define MAX_MODBUSTCP_SENDRETRIES   3

//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
//MODBUS_MSG*	   	 	pModbusInMsg;					// Mensaje recibido por ModbusTCP.
//MODBUS_MSG	   	 	ModbusOutMsg;					// Respuesta al requerimiento recibido

//UINT16        wLastRxMsgParam;
//UINT8         bSendsRetryCntr;
//UINT8         RxModbusTcpSocketIdx;
//UINT8         m_ProtocolID0, m_ProtocolID1; 
//UINT8         m_TransactionID0, m_TransactionID1;


//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void f_ModbusIdleTimeout (void);
void f_ModbusSendDbgMsg (void);
void f_ModbusAnalizeMsg (void);
void f_ModbusRetryTxMsg (void);

void MOD_Init(void);
void MOD_BuildBasicResponse (void);
void MOD_AnalizeValChanges (void);


#endif
