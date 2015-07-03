
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

#define SCI0    0x00c8
#define SCI1    0x00d0

#define SCIBD  regw(SCI1+0x00)
#define SCICR1 reg(SCI1+0x02)
#define SCICR2 reg(SCI1+0x03)
#define SCISR1 reg(SCI1+0x04)
#define SCIDRL reg(SCI1+0x07)

#define BAUD_RATE    9600
#define BAUD_DIV     ECLK/16/BAUD_RATE

#define RS485CFG_A_DE   DDRG_DDRG6
#define RS485CFG_A_RE_  DDRG_DDRG7
#define RS485_A_DE      PTG_PTG6
#define RS485_A_RE_     PTG_PTG7

#define RS485CFG_A_SEL  DDRG_DDRG4
#define RS485_A_SEL     PTG_PTG4


#define MAX_SERIAL_RXERR  10
#define MAX_SERIAL_TXERR  10

#define TMR_TX_TIMEOUT    500
#define TMR_RX_TIMEOUT    500


//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
UINT8 SCI_rx_bytecount;
UINT8 SCI_tx_bytecount;
UINT8 SCI_tmp_var;
UINT8 * frame_to_rx; 
UINT8 * frame_to_tx; 
UINT8 SCI_RX_TimerID;
UINT8 SCI_TX_TimerID;
UINT8 SCI_TxErrAcc;
UINT8 SCI_RxErrAcc;
//UINT8 SCI_RxWaitFirst;


//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
// IDLE
void f_SerialSendRequest(void);

// TRANSMITTING
void f_SerialTxTimeout  (void);
void f_SerialTxFrame    (void);

// RECEIVING
void f_SerialRxTimeout  (void);
void f_SerialRxFrame    (void);

// Funciones Auxiliares
void SRL_Init         (void);
void SRL_ReInit       (void);

#pragma CODE_SEG NON_BANKED
interrupt void near SCI1_isr (void);
#pragma CODE_SEG DEFAULT

#endif				 // dSCIAH