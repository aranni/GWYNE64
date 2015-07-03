
#ifndef ModbusRTUProcH
#define ModbusRTUProcH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define MOD_FC_RD_MUL_REG     3       // Read Multiple Register
#define MOD_FC_WR_MUL_REG    16       // Write Multiple Register
#define MOD_FC_WR_SIN_REG     6       // Write Single Register

#define MOD_SYS_OFFSET      100
#define MOD_SYS_OFFSET_BY   (2 * MOD_SYS_OFFSET)

#define MOD_STA_OFFSET      200
#define MOD_STA_OFFSET_BY   (2 * MOD_STA_OFFSET)

#define REVPOLY 0xA001   // Polinomio generador para desplazamiento hacia la derecha


//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
MODBUS_RTU_MSG*	   	pModRtuInMsg;					// Mensaje recibido por ModbusRTU.
MODBUS_RTU_MSG*  	 	pModRtuOutMsg;				// Respuesta al requerimiento recibido


//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void f_ModRtuIdleTimeout  (void);
void f_ModRtuSendDbgMsg   (void);
void f_ModRtuAnalizeMsg   (void);

void    MRT_Init                (void);
void    MRT_BuildBasicResponse  (void);
void    MRT_AnalizeValChanges   (void);
UINT16  MRT_BuildCRC16          (UINT8 * puchMsg, UINT16 usDataLen);

#endif
