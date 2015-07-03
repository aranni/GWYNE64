/******************************************************************************
 * File Name     : sVectors.c
 ******************************************************************************/

extern void near _Startup(void);       

#define   OWN_RESET_VECTOR

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
        software_trap,								 /* 47-16 <<<<< EMAC Excs Col  >>>>> */
        software_trap,								 /* 46-17 <<<<< EMAC Late Col  >>>>> */
        software_trap,					       /* 45-18 <<<<< EMAC Bab Err   >>>>> */
        software_trap,						     /* 44-19 <<<<< EMAC Rx_B PV   >>>>> */
        software_trap,						     /* 43-20 <<<<< EMAC Rx_A OV   >>>>> */
        software_trap,						     /* 42-21 <<<<< EMAC Rx ERR    >>>>> */
        software_trap,							   /* 41-22 <<<<< EMAC MII Mng   >>>>> */
        software_trap,								 /* 40-23 <<<<< EMAC Rx Flow   >>>>> */
        software_trap,							   /* 39-24 <<<<< EMAC Frame Tx  >>>>> */
        software_trap,						     /* 38-25 <<<<< EMAC Rx_B CP   >>>>> */
        software_trap,						     /* 37-26 <<<<< EMAC Rx_A CP   >>>>> */
        software_trap,								 /* 36-27 <<<<<    EPHY        >>>>> */
        software_trap,                 /* 35-28 Default (unused) interrupt */
        software_trap,                 /* 34-29 Default (unused) interrupt */
        software_trap,                 /* 33-30 Default (unused) interrupt */
        software_trap,                 /* 32-31 Default (unused) interrupt */
        software_trap,                 /* 31-32 <<<<<     IIC        >>>>> */
        software_trap,                 /* 30-33 Default (unused) interrupt */
        software_trap,                 /* 29-34 Default (unused) interrupt */
        software_trap,                 /* 28-35 Default (unused) interrupt */
        software_trap,                 /* 27-36 Default (unused) interrupt */
        software_trap,                 /* 26-37 Default (unused) interrupt */
        software_trap,                 /* 25-38 Default (unused) interrupt */
        software_trap,                 /* 24-39 Default (unused) interrupt */
        software_trap,                 /* 23-40 Default (unused) interrupt */
        software_trap,                 /* 22-41 Default (unused) interrupt */
        software_trap,                 /* 21-42 <<<<<    SCI1        >>>>> */
        software_trap,                 /* 20-43 <<<<<    SCI0        >>>>> */
        software_trap,                 /* 19-44 Default (unused) interrupt */
        software_trap,                 /* 18-45 Default (unused) interrupt */
        software_trap,                 /* 17-46 Default (unused) interrupt */
        software_trap,                 /* 16-47 Default (unused) interrupt */
        software_trap,                 /* 15-48 <<<<<    TIMER 7     >>>>> */
        software_trap,                 /* 14-49 Default (unused) interrupt */
        software_trap,                 /* 13-50 Default (unused) interrupt */
        software_trap,                 /* 12-51 Default (unused) interrupt */
        software_trap,                 /* 11-52 Default (unused) interrupt */
        software_trap,                 /* 10-53 Default (unused) interrupt */
        software_trap,                 /* 09-54 Default (unused) interrupt */
        software_trap,                 /* 08-55 Default (unused) interrupt */
        software_trap,                 /* 07-56 Default (unused) interrupt */
        software_trap,                 /* 06-57 <<<<<      IRQ       >>>>> */
        software_trap,                 /* 05-58 <<<<<      XIRQ      >>>>> */
        software_trap,                 /* 04-59 <<<<<      SWI       >>>>> */
        
#ifdef  OWN_RESET_VECTOR
        _Startup,                      /* 03-60 <<<<< UNIMP. OPCODE  >>>>> */        
        _Startup,                      /* 02-61 <<<<<    COP(Wdog)   >>>>> */
        _Startup,                      /* 01-62 <<<<<  CLOCK MONITOR >>>>> */
        _Startup,                      /* 00-63 <<<<<  EXT. RESET    >>>>> */
#endif        
   };
