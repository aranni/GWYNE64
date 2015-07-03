
#ifndef dSCIAH
#define dSCIAH

//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define _BASE 0x0000  /**< Base on register map */
#define ECLK 25000000 /**< this is BUSCLK */

#define reg(x)  *((volatile tU08 *)(_BASE+x))
#define regw(x)  *((volatile tU16 *)(_BASE+x))

//#define SCI0    0x00c8
//#define SCI1    0x00d0

#define SCIA_SCIBD  regw(SCI1+0x00)
#define SCIA_SCICR1 reg(SCI1+0x02)
#define SCIA_SCICR2 reg(SCI1+0x03)
#define SCIA_SCISR1 reg(SCI1+0x04)
#define SCIA_SCIDRL reg(SCI1+0x07)

#define SCIA_BAUD_RATE    9600
#define SCIA_BAUD_DIV     ECLK/16/SCIA_BAUD_RATE

#define RS485CFG_A_DE   DDRG_DDRG6
#define RS485CFG_A_RE_  DDRG_DDRG7
#define RS485_A_DE      PTG_PTG6
#define RS485_A_RE_     PTG_PTG7

#define RS485CFG_A_SEL  DDRG_DDRG4
#define RS485_A_SEL     PTG_PTG4

#define MAX_SERIAL_RXERR  10
#define MAX_SERIAL_TXERR  10

//#define TMR_TX_TIMEOUT    500
//#define TMR_RX_TIMEOUT    2000

#define FLG_TX_COMMAND  0x01      // El mensaje en curso es un comando, esperar respuesta  
#define FLG_RX_RELOAD   0x02      // Modo recarga automatica del buffer de recepción

//#define   SCIA_RS485_MODE

//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
UINT8   SCIA_ConfigByte;					// Byte de configuración de los parametros del puerto  
UINT8   SCIA_TargetProc;          // Proceso destinatario de los mensajes y excepciones
UINT8   SCIA_ProtocolFrame;       // Codigo del protocolo que deberá manejar el puerto
UINT8   SCIA_StateFlags;          // Flags auxiliares del proceso

UINT16 SCIA_RxByteCount;		      // Cantidad de bytes pendientes de recepcion
UINT16 SCIA_RxByteIdx;			      // Indice contador de Bytes recibidos
UINT16 SCIA_TxByteCount;					// Cantidad de bytes pendientes de transmision
UINT8  SCIA_TmpVar;

UINT8  m_SCIA_SCISR1;
UINT8  m_RxMsg_FC;
UINT16  m_RxMsgLen; 

UINT8 * SCIA_FrameToRx; 
UINT8 * SCIA_FrameToTx; 

UINT8 SCIA_RX_TimerID;
UINT8 SCIA_TX_TimerID;

UINT8 SCIA_TxErrAcc;
UINT8 SCIA_RxErrAcc;

UINT8   SCIA_CurrentTxToken;
UINT8   SCIA_CurrentRxToken;

UINT8  SCIA_TxOffset;             // Offset del buffer desde donde comenzar a transmitir
UINT8  SCIA_RxOffset;             // Offset del buffer desde donde comenzar a recibir 

UINT16 SCIA_TMR_TX_TIMEOUT;
UINT16 SCIA_TMR_RX_TIMEOUT;
UINT16 SCIA_TMR_DRIVE_TIMEOUT;

#pragma CODE_SEG NON_BANKED        

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
// IDLE
void f_SCIAIdleTimeout  (void);
void f_SCIASendCommand  (void);
void f_SCIASendRequest  (void);
void f_SCIAReceiveMsg   (void);
void f_SCIAIdleFrameTx  (void);
void f_SCIAIdleFrameRx  (void);

// TOTRANSMIT
void f_SCIAToTxTimeout  (void);
void f_SCIATxNotReady   (void);  // Desde aquí en todos los estados sucesivos...

// TRANSMITTING
void f_SCIATxTimeout    (void);
void f_SCIATxFrame      (void);

// RECEIVING
void f_SCIARxTimeout    (void);
void f_SCIARxFrame      (void);

// Funciones Auxiliares
void SCIA_Init          (BYTE TargetProc, BYTE  ProtocolFrame);
void SCIA_ReInit        (BYTE TargetProc, BYTE  ProtocolFrame);
void SCIA_ReceiveFrame  ( UINT16 RxLen, UINT8 RxMsgToken);
RETCODE SCIA_TransmitFrame ( UINT16 TxLen, UINT8 TxMsgToken);

//#pragma CODE_SEG NON_BANKED
interrupt void near SCI1_isr (void);

#pragma CODE_SEG DEFAULT
#endif				 // dSCIAH