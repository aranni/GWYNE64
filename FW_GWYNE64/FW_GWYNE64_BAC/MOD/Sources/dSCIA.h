
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

#define TMR_TX_TIMEOUT    500
#define TMR_RX_TIMEOUT    2000

//#define   SCIA_RS485_MODE

//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
UINT16 SCIA_RxByteCount;		      // Cantidad de bytes pendientes de recepcion
UINT8  SCIA_RxByteIdx;			      // Indice contador de Bytes recibidos
UINT16 SCIA_TxByteCount;					// Cantidad de bytes pendientes de transmision
UINT8  SCIA_TmpVar;

UINT8  m_SCIA_SCISR1;

UINT8 * SCIA_FrameToRx; 
UINT8 * SCIA_FrameToTx; 

#ifdef SCIA_RS485_MODE        // Si se configura al puerto en modo RS485
UINT8 SCIA_RX_TimerID;
UINT8 SCIA_TX_TimerID;
#endif		                    //  SCIA_RS485_MODE

UINT8 SCIA_RX_FrameTimerID;
UINT8 SCIA_CurrentTxAddres;
UINT8 SCIA_LastRxByte;


UINT8 SCIA_TxErrAcc;
UINT8 SCIA_RxErrAcc;

UINT8   SCIA_CurrentTxToken;
UINT8   SCIA_CurrentRxToken;

#pragma CODE_SEG NON_BANKED        

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
// IDLE
void f_SCIAIdleTimeout  (void);

#ifdef SCIA_RS485_MODE        // Si se configura al puerto en modo RS485
void f_SCIASendRequest  (void);
void f_SCIAReceiveMsg   (void);

// TOTRANSMIT
void f_SCIAToTxTimeout (void);

// TRANSMITTING
void f_SCIATxTimeout    (void);
void f_SCIATxFrame      (void);

// RECEIVING
void f_SCIARxTimeout    (void);
void f_SCIARxFrame      (void);
#endif		                    //  SCIA_RS485_MODE

// Funciones Auxiliares
void SCIA_Init          (void);
void SCIA_ReInit        (void);
void SCIA_ReceiveFrame  ( UINT16 RxLen, UINT8 RxMsgToken);
RETCODE SCIA_TransmitFrame ( UINT16 TxLen, UINT8 TxMsgToken);
RETCODE SCIA_TransmitBuffer ( UINT16 TxLen, UINT8* TxMsgBuffer); 


//#pragma CODE_SEG NON_BANKED
interrupt void near SCI1_isr (void);

#pragma CODE_SEG DEFAULT
#endif				 // dSCIAH