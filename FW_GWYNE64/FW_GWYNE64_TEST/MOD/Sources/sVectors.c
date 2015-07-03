/******************************************************************************
 * File Name     : sVectors.c
 ******************************************************************************/
#include "MC9S12NE64.h"
#include "datatypes.h"
#include "Commdef.h"

extern void near _Startup(void);       
extern void near RealTimeInterrupt(void);    
extern void near emac_ec_isr(void);
extern void near emac_lc_isr(void);
extern void near emac_b_rx_error_isr(void);
extern void near emac_rx_b_b_o_isr(void);
extern void near emac_rx_b_a_o_isr(void);
extern void near emac_rx_error_isr(void);
extern void near emac_mii_mtc_isr(void);
extern void near emac_rx_fc_isr(void);
extern void near emac_f_tx_c_isr(void);
extern void near emac_rx_b_b_c_isr(void);
extern void near emac_rx_b_a_c_isr(void);
extern void near ephy_isr(void);

extern void near tim0_ch7_isr(void);
extern void near SCI1_isr (void);	  
extern void near SCI0_isr (void);	  
extern void near IIC_isr  (void);	  


//************************************************************************
// SOFTWARE TRAP FUNCTION
// DESCRIPTION:
// Function that traps  all unexpected interrupts.  Used for debugging 
// software to find runaway code.
//************************************************************************

#pragma CODE_SEG __NEAR_SEG NON_BANKED        
__interrupt void software_trap(void){ for(;;); }
#pragma CODE_SEG DEFAULT 


//************************************************************************

typedef void (*near tIsrFunc)(void);
const tIsrFunc _SeudoVect[] @0xEF80 = {/* PSEUDO !! Interrupt table */
        software_trap,                 /* 63-00 Default (unused) interrupt */
        software_trap,                 /* 62-01 Default (unused) interrupt */
        software_trap,                 /* 61-02 Default (unused) interrupt */
        software_trap,                 /* 60-03 Default (unused) interrupt */
        software_trap,                 /* 59-04 Default (unused) interrupt */
        software_trap,                 /* 58-05 Default (unused) interrupt */
        software_trap,                 /* 57-06 Default (unused) interrupt */
        software_trap,                 /* 56-07 Default (unused) interrupt */
        software_trap,                 /* 55-08 Default (unused) interrupt */
        software_trap,                 /* 54-09 Default (unused) interrupt */
        software_trap,                 /* 53-10 Default (unused) interrupt */
        software_trap,                 /* 52-11 Default (unused) interrupt */
        software_trap,                 /* 51-12 Default (unused) interrupt */
        software_trap,                 /* 50-13 Default (unused) interrupt */
        software_trap,                 /* 49-14 Default (unused) interrupt */
        software_trap,                 /* 48-15 Default (unused) interrupt */
        emac_ec_isr,									 /* 47-16 <<<<< EMAC Excs Col  >>>>> */
        emac_lc_isr,									 /* 46-17 <<<<< EMAC Late Col  >>>>> */
        emac_b_rx_error_isr,					 /* 45-18 <<<<< EMAC Bab Err   >>>>> */
        emac_rx_b_b_o_isr,						 /* 44-19 <<<<< EMAC Rx_B PV   >>>>> */
        emac_rx_b_a_o_isr,						 /* 43-20 <<<<< EMAC Rx_A OV   >>>>> */
        emac_rx_error_isr,						 /* 42-21 <<<<< EMAC Rx ERR    >>>>> */
        emac_mii_mtc_isr,							 /* 41-22 <<<<< EMAC MII Mng   >>>>> */
        emac_rx_fc_isr,								 /* 40-23 <<<<< EMAC Rx Flow   >>>>> */
        emac_f_tx_c_isr,							 /* 39-24 <<<<< EMAC Frame Tx  >>>>> */
        emac_rx_b_b_c_isr,						 /* 38-25 <<<<< EMAC Rx_B CP   >>>>> */
        emac_rx_b_a_c_isr,						 /* 37-26 <<<<< EMAC Rx_A CP   >>>>> */
        ephy_isr,											 /* 36-27 <<<<<    EPHY        >>>>> */
        software_trap,                 /* 35-28 Default (unused) interrupt */
        software_trap,                 /* 34-29 Default (unused) interrupt */
        software_trap,                 /* 33-30 Default (unused) interrupt */
        software_trap,                 /* 32-31 Default (unused) interrupt */
        IIC_isr,                       /* 31-32 <<<<<     IIC        >>>>> */
        software_trap,                 /* 30-33 Default (unused) interrupt */
        software_trap,                 /* 29-34 Default (unused) interrupt */
        software_trap,                 /* 28-35 Default (unused) interrupt */
        software_trap,                 /* 27-36 Default (unused) interrupt */
        software_trap,                 /* 26-37 Default (unused) interrupt */
        software_trap,                 /* 25-38 Default (unused) interrupt */
        software_trap,                 /* 24-39 Default (unused) interrupt */
        software_trap,                 /* 23-40 Default (unused) interrupt */
        software_trap,                 /* 22-41 Default (unused) interrupt */
        SCI1_isr,                      /* 21-42 <<<<<    SCI1        >>>>> */
        SCI0_isr,                      /* 20-43 <<<<<    SCI0        >>>>> */
        software_trap,                 /* 19-44 Default (unused) interrupt */
        software_trap,                 /* 18-45 Default (unused) interrupt */
        software_trap,                 /* 17-46 Default (unused) interrupt */
        software_trap,                 /* 16-47 Default (unused) interrupt */
        tim0_ch7_isr,                  /* 15-48 <<<<<    TIMER 7     >>>>> */
        software_trap,                 /* 14-49 Default (unused) interrupt */
        software_trap,                 /* 13-50 Default (unused) interrupt */
        software_trap,                 /* 12-51 Default (unused) interrupt */
        software_trap,                 /* 11-52 Default (unused) interrupt */
        software_trap,                 /* 10-53 Default (unused) interrupt */
        software_trap,                 /* 09-54 Default (unused) interrupt */
        software_trap,                 /* 08-55 Default (unused) interrupt */
        RealTimeInterrupt,             /* 07-56 Default (unused) interrupt */
        software_trap,                 /* 06-57 <<<<<      IRQ       >>>>> */
        software_trap,                 /* 05-58 <<<<<      XIRQ      >>>>> */
        software_trap,                 /* 04-59 <<<<<      SWI       >>>>> */     
        software_trap,                 /* 03-60 <<<<< UNIMP. OPCODE  >>>>> */        
        _Startup,                      /* 02-61 <<<<<    COP(Wdog)   >>>>> */
        software_trap,                 /* 01-62 <<<<<  CLOCK MONITOR >>>>> */
        _Startup,                      /* 00-63 <<<<<  EXT. RESET    >>>>> */
   };

//************************************************************************

#ifdef  OWN_RESET_VECTOR

typedef void (*near tIsrFunc)(void);
const tIsrFunc _vect[] @0xFF80 = {     /* Interrupt table */
        software_trap,                 /* 63-00 Default (unused) interrupt */
        software_trap,                 /* 62-01 Default (unused) interrupt */
        software_trap,                 /* 61-02 Default (unused) interrupt */
        software_trap,                 /* 60-03 Default (unused) interrupt */
        software_trap,                 /* 59-04 Default (unused) interrupt */
        software_trap,                 /* 58-05 Default (unused) interrupt */
        software_trap,                 /* 57-06 Default (unused) interrupt */
        software_trap,                 /* 56-07 Default (unused) interrupt */
        software_trap,                 /* 55-08 Default (unused) interrupt */
        software_trap,                 /* 54-09 Default (unused) interrupt */
        software_trap,                 /* 53-10 Default (unused) interrupt */
        software_trap,                 /* 52-11 Default (unused) interrupt */
        software_trap,                 /* 51-12 Default (unused) interrupt */
        software_trap,                 /* 50-13 Default (unused) interrupt */
        software_trap,                 /* 49-14 Default (unused) interrupt */
        software_trap,                 /* 48-15 Default (unused) interrupt */
        emac_ec_isr,									 /* 47-16 <<<<< EMAC Excs Col  >>>>> */
        emac_lc_isr,									 /* 46-17 <<<<< EMAC Late Col  >>>>> */
        emac_b_rx_error_isr,					 /* 45-18 <<<<< EMAC Bab Err   >>>>> */
        emac_rx_b_b_o_isr,						 /* 44-19 <<<<< EMAC Rx_B PV   >>>>> */
        emac_rx_b_a_o_isr,						 /* 43-20 <<<<< EMAC Rx_A OV   >>>>> */
        emac_rx_error_isr,						 /* 42-21 <<<<< EMAC Rx ERR    >>>>> */
        emac_mii_mtc_isr,							 /* 41-22 <<<<< EMAC MII Mng   >>>>> */
        emac_rx_fc_isr,								 /* 40-23 <<<<< EMAC Rx Flow   >>>>> */
        emac_f_tx_c_isr,							 /* 39-24 <<<<< EMAC Frame Tx  >>>>> */
        emac_rx_b_b_c_isr,						 /* 38-25 <<<<< EMAC Rx_B CP   >>>>> */
        emac_rx_b_a_c_isr,						 /* 37-26 <<<<< EMAC Rx_A CP   >>>>> */
        ephy_isr,											 /* 36-27 <<<<<    EPHY        >>>>> */
        software_trap,                 /* 35-28 Default (unused) interrupt */
        software_trap,                 /* 34-29 Default (unused) interrupt */
        software_trap,                 /* 33-30 Default (unused) interrupt */
        software_trap,                 /* 32-31 Default (unused) interrupt */
        IIC_isr,                       /* 31-32 <<<<<     IIC        >>>>> */
        software_trap,                 /* 30-33 Default (unused) interrupt */
        software_trap,                 /* 29-34 Default (unused) interrupt */
        software_trap,                 /* 28-35 Default (unused) interrupt */
        software_trap,                 /* 27-36 Default (unused) interrupt */
        software_trap,                 /* 26-37 Default (unused) interrupt */
        software_trap,                 /* 25-38 Default (unused) interrupt */
        software_trap,                 /* 24-39 Default (unused) interrupt */
        software_trap,                 /* 23-40 Default (unused) interrupt */
        software_trap,                 /* 22-41 Default (unused) interrupt */
        SCI1_isr,                      /* 21-42 <<<<<    SCI1        >>>>> */
        SCI0_isr,                      /* 20-43 <<<<<    SCI0        >>>>> */
        software_trap,                 /* 19-44 Default (unused) interrupt */
        software_trap,                 /* 18-45 Default (unused) interrupt */
        software_trap,                 /* 17-46 Default (unused) interrupt */
        software_trap,                 /* 16-47 Default (unused) interrupt */
        tim0_ch7_isr,                  /* 15-48 <<<<<    TIMER 7     >>>>> */
        software_trap,                 /* 14-49 Default (unused) interrupt */
        software_trap,                 /* 13-50 Default (unused) interrupt */
        software_trap,                 /* 12-51 Default (unused) interrupt */
        software_trap,                 /* 11-52 Default (unused) interrupt */
        software_trap,                 /* 10-53 Default (unused) interrupt */
        software_trap,                 /* 09-54 Default (unused) interrupt */
        software_trap,                 /* 08-55 Default (unused) interrupt */
        RealTimeInterrupt,             /* 07-56 Default (unused) interrupt */
        software_trap,                 /* 06-57 <<<<<      IRQ       >>>>> */
        software_trap,                 /* 05-58 <<<<<      XIRQ      >>>>> */
        software_trap,                 /* 04-59 <<<<<      SWI       >>>>> */     
        software_trap,                 /* 03-60 <<<<< UNIMP. OPCODE  >>>>> */        
        _Startup,                      /* 02-61 <<<<<    COP(Wdog)   >>>>> */
        software_trap,                 /* 01-62 <<<<<  CLOCK MONITOR >>>>> */
        _Startup,                      /* 00-63 <<<<<  EXT. RESET    >>>>> */
   };

#endif        
