
#ifndef dSCIBH
#define dSCIBH

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

#define SCIB_SCIBD  regw(SCI0+0x00)
#define SCIB_SCICR1 reg(SCI0+0x02)
#define SCIB_SCICR2 reg(SCI0+0x03)
#define SCIB_SCISR1 reg(SCI0+0x04)
#define SCIB_SCIDRL reg(SCI0+0x07)

#define SCIB_BAUD_RATE    9600
#define SCIB_BAUD_DIV     ECLK/16/SCIB_BAUD_RATE

#define RS485CFG_B_DE   DDRG_DDRG2
#define RS485CFG_B_RE_  DDRG_DDRG3
#define RS485_B_DE      PTG_PTG2
#define RS485_B_RE_     PTG_PTG3

#define RS485CFG_B_SEL  DDRG_DDRG5
#define RS485_B_SEL     PTG_PTG5

#define MAX_SERIAL_RXERR  10
#define MAX_SERIAL_TXERR  10

//#define TMR_TX_TIMEOUT    500
//#define TMR_RX_TIMEOUT    2000

#define FLG_TX_COMMAND  0x01      // El mensaje en curso es un comando, esperar respuesta  
#define FLG_RX_RELOAD   0x02      // Modo recarga automatica del buffer de recepción


//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
UINT8   SCIB_ConfigByte;					// Byte de configuración de los parametros del puerto  
UINT8   SCIB_TargetProc;          // Proceso destinatario de los mensajes y excepciones
UINT8   SCIB_ProtocolFrame;       // Codigo del protocolo que deberá manejar el puerto
UINT8   SCIB_StateFlags;          // Flags auxiliares del proceso

UINT8   SCIB_EVT_SCI_RXMSG;
UINT8   SCIB_EVT_SCI_RXERR;
UINT8   SCIB_EVT_SCI_TXMSG;
UINT8   SCIB_EVT_SCI_TXERR;
UINT8   SCIB_EVT_SCI_TXBUSY;
  
UINT16 SCIB_RxByteCount;		      // Cantidad de bytes pendientes de recepcion
UINT16 SCIB_RxByteIdx;			      // Indice contador de Bytes recibidos
UINT16 SCIB_TxByteCount;					// Cantidad de bytes pendientes de transmision
UINT8  SCIB_TmpVar;

UINT8  m_SCIB_SCISR1;
UINT8  m_SCIB_RxMsg_FC;
UINT8  m_SCIB_RxMsgLen; 

UINT8 * SCIB_FrameToRx; 
UINT8 * SCIB_FrameToTx; 

UINT8 SCIB_RX_TimerID;
UINT8 SCIB_TX_TimerID;

UINT8 SCIB_TxErrAcc;
UINT8 SCIB_RxErrAcc;

UINT8   SCIB_CurrentTxToken;
UINT8   SCIB_CurrentRxToken;

UINT8  SCIB_TxOffset;             // Offset del buffer desde donde comenzar a transmitir
UINT8  SCIB_RxOffset;             // Offset del buffer desde donde comenzar a recibir 

UINT16 SCIB_TMR_TX_TIMEOUT;
UINT16 SCIB_TMR_RX_TIMEOUT;
UINT16 SCIB_TMR_DRIVE_TIMEOUT;

#pragma CODE_SEG NON_BANKED        

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
// IDLE
void f_SCIBIdleTimeout  (void);
void f_SCIBSendCommand  (void);
void f_SCIBSendRequest  (void);
void f_SCIBReceiveMsg   (void);
void f_SCIBIdleFrameTx  (void);
void f_SCIBIdleFrameRx  (void);
void f_SCIBReceiveAll   (void);

// TOTRANSMIT
void f_SCIBToTxTimeout  (void);
void f_SCIBTxNotReady   (void);  // Desde aquí en todos los estados sucesivos...

// TRANSMITTING
void f_SCIBTxTimeout    (void);
void f_SCIBTxFrame      (void);

// RECEIVING
void f_SCIBRxTimeout    (void);
void f_SCIBRxFrame      (void);

// Funciones Auxiliares
void SCIB_Init          (BYTE TargetProc, BYTE  ProtocolFrame, BYTE _SCI_RX_MSG, BYTE _SCI_RX_ERR, BYTE _SCI_TX_MSG, BYTE _SCI_TX_ERR, BYTE _SCI_TX_BUSY);
void SCIB_ReInit        (BYTE TargetProc, BYTE  ProtocolFrame, BYTE _SCI_RX_MSG, BYTE _SCI_RX_ERR, BYTE _SCI_TX_MSG, BYTE _SCI_TX_ERR, BYTE _SCI_TX_BUSY);
void SCIB_ReceiveFrame  ( UINT16 RxLen, UINT8 RxMsgToken);
RETCODE SCIB_TransmitFrame ( UINT16 TxLen, UINT8 TxMsgToken);

//#pragma CODE_SEG NON_BANKED
interrupt void near SCI0_isr (void);
#pragma CODE_SEG DEFAULT

#endif				 // dSCIBH


