
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

#define SCI0    0x00c8
#define SCI1    0x00d0

#define RTU_SCIBD  regw(SCI0+0x00)
#define RTU_SCICR1 reg(SCI0+0x02)
#define RTU_SCICR2 reg(SCI0+0x03)
#define RTU_SCISR1 reg(SCI0+0x04)
#define RTU_SCIDRL reg(SCI0+0x07)

#define RTU_BAUD_RATE    9600
#define RTU_BAUD_DIV     ECLK/16/RTU_BAUD_RATE

// ENVERSION B
//#define RS485CFG_B_DE   DDRG_DDRG4
//#define RS485CFG_B_RE_  DDRG_DDRG5
//#define RS485_B_DE      PTG_PTG4
//#define RS485_B_RE_     PTG_PTG5

// EN VERSION C
#define RS485CFG_B_DE   DDRG_DDRG2
#define RS485CFG_B_RE_  DDRG_DDRG3
#define RS485_B_DE      PTG_PTG2
#define RS485_B_RE_     PTG_PTG3

#define RS485CFG_B_SEL  DDRG_DDRG5
#define RS485_B_SEL     PTG_PTG5



//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void f_RtuIdleTimeout (void);

void RTU_TransmitFrame ( UINT8 Len, UINT8 * MsgPtr); 
void RTU_ReceiveFrame ( UINT8 Len, UINT8 * MsgPtr);

void RTU_Init   (void);
void RTU_ReInit (void);

#pragma CODE_SEG NON_BANKED
interrupt void near SCI0_isr (void);
#pragma CODE_SEG DEFAULT

#endif				 // dSCIBH