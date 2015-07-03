
#ifndef ModAsciiProcH
#define ModAsciiProcH

//-----------------------------------------------------------------------------
//  DEFINICIONES DE CONSTANTES
//-----------------------------------------------------------------------------
#define MOD_FC_RD_MUL_REG     3       // Read Multiple Register
#define MOD_FC_WR_MUL_REG    16       // Write Multiple Register
#define MOD_FC_WR_SIN_REG     6       // Write Single Register

#define MOD_SYS_OFFSET      100
#define MOD_SYS_OFFSET_BY   (2 * MOD_SYS_OFFSET)

#define MOD_STA_OFFSET      200
#define MOD_STA_OFFSET_BY   (2 * MOD_STA_OFFSET)

#define REVPOLY 0xA001   // Polinomio generador para desplazamiento hacia la derecha

//-----------------------------------------------------------------------------
//  VARIABLES GLOBALES
//-----------------------------------------------------------------------------
MODBUS_ASCII_MSG*	   	pModAsciiInMsg;				// Mensaje recibido por ModbusASCII.
MODBUS_ASCII_MSG*  	 	pModAsciiOutMsg;			// Respuesta al requerimiento recibido

//------------------------------------------------------------------- 
// Prototipos de FUNCIONES
//-------------------------------------------------------------------
void f_ModAsciiIdleTimeout  (void);
void f_ModAsciiAnalizeMsg   (void);

void    MAS_Init                (void);
void    MAS_BuildBasicResponse  (void);
void    MAS_AnalizeValChanges   (void);
UINT16  MAS_BuildLCR            (UINT8 * puchMsg, UINT16 usDataLen);

#endif
