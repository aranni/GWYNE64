
#ifndef pMtsH
#define pMtsH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//

#define REVPOLY               0xA6BC    // Reverse polynomial for right shifts

#define TMR_MTS_RESETLINKWAIT  5000		// Tiempo de espera entre cada comando
#define MAX_MTS_RESETATTEMP		  60    // Cantidad de resets sin respuesta antes de ResetBoard

//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
UINT8   MTS_ResetNoAnswerAcc;				// Contador de ResetsLink sin respuesta

UINT8   MTS_WaitAnswerTimerId;
UINT8   MTS_CmdIdx;
MTS_STATE_VAL   MTS_SensorValues;

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//

// IDLE
void f_MTSIdleTimeout  (void);
void f_MTSIdleRxMsgEnd (void);
void f_MTSIdleTxMsgEnd (void);
void f_MTSIdleRetry    (void);

// Funciones Auxiliares
void MTS_Init          (void);
void MTS_ReInit        (void);

// Funciones de capa de RED protocolo DNP3
void MTS_ComputeCRC    ( UINT8 dataOctet, UINT16* crcAccum );
void MTS_SendCommand   ( UINT8 bPrimary, UINT8 LinkCommand);
void MTS_SendAppMsg    ( UINT8 DataLen, UINT8 TxMsgToken);
void MTS_ChgWordEndian ( UINT16* DataWord);

#endif				 // MtsH