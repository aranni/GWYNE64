
#ifndef dIIH
#define dIIH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define IBSWAI	0x01	// bit masks
#define RSTA	0x04
#define TXAK	0x08
#define TXRX	0x10
#define MSSL	0x20
#define IBIE	0x40
#define IBEN	0x80

#define RXAK	0x01	// bit masks
#define IBIF 	0x02
#define SRW 	0x04
#define IBAL 	0x10
#define IBB 	0x20
#define IAAS 	0x40
#define TCF 	0x80

#define IIC_TX_PACKETSIZE 	 10		// IIC TX buffers size
#define IIC_RX_PACKETSIZE 	 10		// IIC RX buffers size

#define IIC_RTC_ADDRESS      0xD0
#define IIC_DAC_ADDRESS      0x58


//------------------------------------------------------------------------------------
// Estructuras de DATOS
//------------------------------------------------------------------------------------
//
typedef enum {
	IIC_TRANSFERING			= 0	,
	IIC_TX_OK					,
	IIC_TX_ERR				,
	IIC_RX_OK					,
	IIC_RX_ERR				,
	IIC_TFR_ERR				,
	IIC_IDLE					,

	IIC_MAX_TFR		
	
} IIC_TRANSFER_ENUM;

#pragma CODE_SEG NON_BANKED        

//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
UINT8 IIC_TransferState;

UINT8 TxPacket[IIC_TX_PACKETSIZE];	// global transmit packet
UINT8 *TxPacketpositionptr;					// pointer to current location in transmit packet
UINT8 *TxPacketendptr;							// (fixed) pointer to end of transmit packet
UINT8 TxBufferemptyflag;						// global flag for transmit functionality

UINT8 RxPacket[IIC_RX_PACKETSIZE];	// global receive packet
UINT8 *RxPacketpositionptr;					// pointer to current location in receive packet
UINT8 *RxPacketendptr;							// (fixed) pointer to end of receive packet

UINT8 RxBufferfullflag, MasterRxFlag;	// global flags for receive functionality
UINT8 TxCompleteflag, RxCompleteflag;	// global flags for ISR functionality
UINT8 TxTimerID, RxTimerID;
UINT8 CurrentSlave, TxMsgLen, RxMsgLen;


//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void f_IICTimeout 		(void);

void IIC_Init 			  (void);
UINT8 IIC_SendMsg 		(UINT8 SlaveAddress, UINT8 LenTx, UINT8 LenRx);
UINT8 IIC_ReceiveMsg 	(UINT8 SlaveAddress, UINT8 LenRx);

void IIC_WriteRTCMem  (UINT8 Len, UINT8 Addr, UINT8* DataPtr);
void IIC_ReadRTCMem   (UINT8 Len, UINT8 Addr, UINT8* DataPtr);

void IIC_RtcGetDateTime (void);
void IIC_RtcSetDateTime (void);

void IIC_DacSetOutput   (UINT8 Chan, UINT8 OutVal);
void IIC_DacSetValues   (void);

//#pragma CODE_SEG NON_BANKED
interrupt void near IIC_isr (void);
#pragma CODE_SEG DEFAULT

#endif				 // dIIH

