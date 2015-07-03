
#ifndef ModbusSlaveRtuProcH
#define ModbusSlaveRtuProcH

//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define REVPOLY 0xA001   // Polinomio generador para desplazamiento hacia la derecha

//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
BYTE                MODRTU_SLV_TargetSci; // Puerto serie usado para SLAVE MODBUS RTU  
BYTE                ModbusRtuSlaveMsgToken;
BYTE                ModbusRtuSlaveTimerId;

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void f_ModbusSlaveRtuIdleTimeout  (void);
void f_ModbusSlaveRtuAnalizeMsg   (void);

void    MOD_ModbusSlaveRtuInit           (void);
void    f_ModbusSlaveRtuIdleAnalizeMsg   (void);
void    f_ModbusSlaveRtuIdleSendMsg      (void);
void    f_ModbusSlaveRtuIdleTxedMsg      (void);

#endif
