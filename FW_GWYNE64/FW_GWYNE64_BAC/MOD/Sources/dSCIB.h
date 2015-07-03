
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

#define TMR_TX_TIMEOUT    500
#define TMR_RX_TIMEOUT    2000

//#define   SCIB_RS485_MODE

//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
UINT16 SCIB_RxByteCount;		      // Cantidad de bytes pendientes de recepcion
UINT8  SCIB_RxByteIdx;			      // Indice contador de Bytes recibidos
UINT16 SCIB_TxByteCount;					// Cantidad de bytes pendientes de transmision
UINT8  SCIB_TmpVar;

UINT8  m_SCIB_SCISR1;

UINT8 * SCIB_FrameToRx; 
UINT8 * SCIB_FrameToTx; 

#ifdef SCIB_RS485_MODE        // Si se configura al puerto en modo RS485
UINT8 SCIB_RX_TimerID;
UINT8 SCIB_TX_TimerID;
#endif		                    //  SCIB_RS485_MODE

UINT8 SCIB_RX_FrameTimerID;
UINT8 SCIB_CurrentTxAddres;
UINT8 SCIB_LastRxByte;


UINT8 SCIB_TxErrAcc;
UINT8 SCIB_RxErrAcc;

UINT8   SCIB_CurrentTxToken;
UINT8   SCIB_CurrentRxToken;

#pragma CODE_SEG NON_BANKED        

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
// IDLE
void f_SCIBIdleTimeout  (void);

#ifdef SCIB_RS485_MODE        // Si se configura al puerto en modo RS485
void f_SCIBSendRequest  (void);
void f_SCIBReceiveMsg   (void);

// TOTRANSMIT
void f_SCIBToTxTimeout (void);

// TRANSMITTING
void f_SCIBTxTimeout    (void);
void f_SCIBTxFrame      (void);

// RECEIVING
void f_SCIBRxTimeout    (void);
void f_SCIBRxFrame      (void);
#endif		                    //  SCIB_RS485_MODE

// Funciones Auxiliares
void SCIB_Init          (void);
void SCIB_ReInit        (void);
void SCIB_ReceiveFrame  ( UINT16 RxLen, UINT8 RxMsgToken);
RETCODE SCIB_TransmitFrame ( UINT16 TxLen, UINT8 TxMsgToken);
RETCODE SCIB_TransmitBuffer ( UINT16 TxLen, UINT8* TxMsgBuffer); 


//#pragma CODE_SEG NON_BANKED
interrupt void near SCI1_isr (void);

#pragma CODE_SEG DEFAULT
#endif				 // dSCIBH
