
#ifndef pSpaH
#define pSpaH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//

//#define REVPOLY               0xA6BC    // Reverse polynomial for right shifts

#define TMR_SPA_RESETLINKWAIT 10000		// Tiempo de espera entre cada comando
#define MAX_SPA_RESETATTEMP		   60   // Cantidad de resets sin respuesta antes de ResetBoard
#define TIMEOUT_WAIT_ANSWER    5000   // Tiempo de espera de respuesta al comando


//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
UINT8 bac_len;
BAC_BANK_ALLOC  SPA_BankMemManager[MAX_BAC_BANKS];
UINT8           SPA_RxMsgToken;    

UINT8   SPA_TxBuff[3];

UINT8   SPA_ResetNoAnswerAcc;				// Contador de ResetsLink sin respuesta
UINT8   SPA_EnablePolling;				  // Habilitar polling
UINT16  SPA_PollingPeriod;				  // Periodo de polling en milisegundos
UINT8   SPA_CtrlFlags;				      // Habilitar polling

UINT8   SPA_WaitAnswerTimerId;

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//

// IDLE
void f_SPAIdleTimeout  (void);
void f_SPAIdleRxMsgEnd (void);
void f_SPAIdleTxMsgEnd (void);
void f_SPAIdleCpyTxMsgEnd (void);

// Funciones Auxiliares
void SPA_Init          (void);
void SPA_ReInit        (void);
void SPA_ResetBanks    (void); 

// Funciones de capa de RED protocolo DNP3
void SPA_ComputeCRC    ( UINT8 dataOctet, UINT16* crcAccum );
void SPA_SendCommand   ( UINT8 bPrimary, UINT8 LinkCommand);
void SPA_SendAppMsg    ( UINT8 DataLen, UINT8 TxMsgToken);

void SeguimosVivos(char vive);
void NoHayMasBancos(void);
void NoHayMasMemoria(void);
void TramaMal(void);
void ClearDLEsFromMsg(BYTE * buff, BYTE * bytes);
void StoreData(BYTE * data, BYTE len);

//UINT16 SPA_BuildCRC16 ( UINT8 * puchMsg, UINT16 usDataLen );

#endif				 // SpaH