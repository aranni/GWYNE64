/*******************************************************************
 *                   freescale semiconductor inc 
 *
 *    DESCRIPTION:   Main routine for S12 single array Flash routines
 *    SOURCE:        main.c
 *    COPYRIGHT:     © 09/2004  freescale
 *    AUTHOR:        rat579 
 *    REV. HISTORY:  (none)  
 *
 *******************************************************************/
#include <hidef.h>      /* common defines and macros */
#include <mc9s12dp256.h>     /* derivative information */
#include "flash.h"
#define PAGE 0x3E0000
#pragma LINK_INFO DERIVATIVE "mc9s12dp256b"
const volatile unsigned char caliz1 @ 0xD002 = 'A';
const volatile unsigned char far caliz2 @(PAGE+0xB400) = 'B';
const volatile unsigned char caliz3 @ 0x4000 = 'A';

 void EXIT(void) {
  for(;;) {} /* wait forever */
 }
 void main(void) {
  /* put your own code here */
  volatile unsigned char a,rval;
  unsigned long i;
  a = caliz1;
  a = caliz3;
  
    Flash_Init(16000);	 //osc freq
    rval=Flash_Erase_Sector((unsigned int *far)0xD000);
    rval=Flash_Write_Word((unsigned int *far)0xD000,0xABCD);
    rval=Flash_Erase_Sector((unsigned int *far)0x4000);
    rval=Flash_Write_Word((unsigned int *far)0x4000,0xABCD);
    for (i=0x3C; i<0x40; i++)
    {
        rval=Flash_Erase_Sector((unsigned int *far)((i<<16) + 0xB400));
        rval=Flash_Write_Word((unsigned int *far)((i<<16) + 0xB400),0xABCD);
        rval=Flash_Write_Word((unsigned int *far)((i<<16) + 0xB800),0xABCD);
        rval=Flash_Write_Word((unsigned int *far)((i<<16) + 0xBC00),0xABCD);
    }

    a = caliz2;
  
  EnableInterrupts;
  for(;;) {} /* wait forever */
}
