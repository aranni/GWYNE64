
#ifndef pNexusProcH
#define pNexusProcH

//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define MOD_FC_READ_COILS      1        // Read Coils
#define MOD_FC_WRITE_COIL      5        // Write Coil
#define MOD_FC_RD_MUL_REG      3        // Read Multiple Register
#define MOD_FC_WR_MUL_REG     16        // Write Multiple Register
#define MOD_FC_WR_SIN_REG      6        // Write Single Register

#define MODBUS_RTU_REVPOLY  0xA001   // Polinomio generador para desplazamiento hacia la derecha

//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
BYTE  NEX_TargetSci; 
BYTE  NEX_UnitelwaySci;

UINT8	            QNEXO_MsgQueue  [MAX_QUEUE];
FIFO_CNTRL			  QNEXO_QueueIdx;

UINT16    TMR_BetweenRequest;
//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void f_NexusIdleTimeout (void);

void f_NexusModbusRtuMsg        (void);
void f_NexusModbusTcpAny        (void);
void f_NexusModbusTcpReadMulReg (void);
void f_NexusModbusTcpWriteSinReg(void);
void f_NexusModbusTcpWriteMulReg(void);
void f_NexusModbusRtuAny        (void);
void f_NexusModbusRtuErr        (void);

void f_NexusUnitelwayTcpAny     (void);
void f_NexusUnitelwayRtuMsg     (void);
void f_NexusUnitelwayRtuErr     (void);


RETCODE  NEX_SendNextQueuedMsg          (void);

void  NEX_Init(void);
void  NEX_ModbusRtuBuildCRC16 ( UINT8 * puchMsg, UINT16 usDataLen );

#endif
