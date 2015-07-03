
#ifndef ModbusTCPProcH
#define ModbusTCPProcH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define MOD_FC_RD_MUL_REG      3        // Read Multiple Register
#define MOD_FC_WR_MUL_REG     16        // Write Multiple Register
#define MOD_FC_WR_SIN_REG      6        // Write Single Register

#define MOD_SYS_OFFSET_IDX    1         //  100
#define MOD_STA_OFFSET_IDX    2         //  200
#define MOD_GIO_OFFSET_IDX    3			    //  300   

#define MOD_SYS_OFFSET      (100 * MOD_SYS_OFFSET_IDX)
#define MOD_SYS_OFFSET_BY   (2 * MOD_SYS_OFFSET)

#define MOD_STA_OFFSET      (100 * MOD_STA_OFFSET_IDX)
#define MOD_STA_OFFSET_BY   (2 * MOD_STA_OFFSET)

#define MOD_GIO_OFFSET      (100 * MOD_GIO_OFFSET_IDX)
#define MOD_GIO_OFFSET_BY   (2 * MOD_GIO_OFFSET)


//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
MODBUS_MSG	   	 	ModbusInMsg;					// Mensaje recibido por ModbusTCP.
MODBUS_MSG	   	 	ModbusOutMsg;					// Respuesta al requerimiento recibido


//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void f_ModbusIdleTimeout (void);
void f_ModbusSendDbgMsg (void);
void f_ModbusAnalizeMsg (void);

void MOD_Init(void);
void MOD_BuildBasicResponse (void);
void MOD_AnalizeValChanges (void);

#endif
