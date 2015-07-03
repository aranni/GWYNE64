
#ifndef pMtsH
#define pMtsH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//

//#define REVPOLY               0xA6BC    // Reverse polynomial for right shifts

#define TMR_BAC_RESETLINKWAIT 10000		// Tiempo de espera entre cada comando
#define MAX_BAC_RESETATTEMP		   60   // Cantidad de resets sin respuesta antes de ResetBoard
#define TIMEOUT_WAIT_ANSWER    5000   // Tiempo de espera de respuesta al comando

#define OFFSET_DIAG  2 * 4//  mi offset para escribir el diagnostico

/* Cada banco se direccionara en Modbus de la siguiente manera:
Banco 0: 40000
Banco 1: 40500
Banco 2: 41000
 .....
Banco Diagnostico:49500

Los datos en memoria seran los del array "bac_data"
Puede ser un  ACK (secuencia 0x10 - 0x06) o un mensaje completo crc incluido

Para los datos fuera de rango el conversor devolvera el valor 0xFF
*/

/* Esta funcion se llama desde la capa inferior cada vez que llega una trama
   y procesara los bac_len datos almacenados en bac_buff
*/
//void ProcessBacRead(void);
/* Pedido de encuesta */
//void SendBacMsg(void);
/* Timeout handler. Se llama desde la capa inferior cuando se detecta un timeout */
//void BacTimeout(void);

//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
UINT8 bac_len;
BAC_BANK_ALLOC  BAC_BankMemManager[MAX_BAC_BANKS];
BAC_PROC_DIAG   BAC_Diag;
UINT8           BAC_RxMsgToken;    

UINT8   BAC_TxBuff[3];

UINT8   BAC_ResetNoAnswerAcc;				// Contador de ResetsLink sin respuesta
UINT8   BAC_EnablePolling;				  // Habilitar polling
UINT16  BAC_PollingPeriod;				  // Periodo de polling en milisegundos
UINT8   BAC_CtrlFlags;				      // Habilitar polling

UINT8   BAC_WaitAnswerTimerId;
UINT16  BAC_PollingTimeout;				  // Timeout en TX en milisegundos

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//

// IDLE
void f_BACIdleTimeout  (void);
void f_BACIdleRxMsgEnd (void);
void f_BACIdleTxMsgEnd (void);
void f_BACIdleCpyTxMsgEnd (void);

// Funciones Auxiliares
void BAC_Init          (void);
void BAC_ReInit        (void);
void BAC_ResetBanks    (void); 

// Funciones de capa de RED protocolo DNP3
void BAC_ComputeCRC    ( UINT8 dataOctet, UINT16* crcAccum );
void BAC_SendCommand   ( UINT8 bPrimary, UINT8 LinkCommand);
void BAC_SendAppMsg    ( UINT8 DataLen, UINT8 TxMsgToken);

void SeguimosVivos(char vive);
void NoHayMasBancos(void);
void NoHayMasMemoria(void);
void TramaMal(void);
void ClearDLEsFromMsg(BYTE * buff, BYTE * bytes);
void StoreData(BYTE * data, BYTE len);

//UINT16 BAC_BuildCRC16 ( UINT8 * puchMsg, UINT16 usDataLen );

#endif				 // MtsH