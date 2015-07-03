
#ifndef UnitelwayTCPProcH
#define UnitelwayTCPProcH

//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//

#define MAX_MODBUSTCP_SENDRETRIES   3

//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void f_UnitelwayTcpIdleTimeout (void);
void f_UnitelwayTcpIdleAnalizeMsg (void);
void f_UnitelwayTcpIdleSendMsg(void); 
void f_UnitelwayTcpIdleErrMsg(void); 
void MOD_UnitelwayTcpInit(void);

#endif
