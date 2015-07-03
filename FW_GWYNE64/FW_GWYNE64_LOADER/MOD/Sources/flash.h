/*******************************************************************
 *                   freescale semiconductor inc 
 *
 *    DESCRIPTION:   Header file for S12 single array Flash routines
 *    SOURCE:        flash.h
 *    COPYRIGHT:     © 04/2004  freescale
 *    AUTHOR:        rat579 
 *    REV. HISTORY:  (none)  
 *
 *******************************************************************/
#ifndef FLASH_H        /* Prevent duplicated includes */
#define FLASH_H

/* Functions from flash.c */
void Flash_Init(unsigned long oscclk);
signed char Flash_Write_Word(unsigned int *far address, unsigned int data);
signed char Flash_Erase_Sector(unsigned int *far far_address);
signed char Flash_Write_Block(unsigned int *far address_source,\
                        unsigned int *far far_address_destination,\
                        unsigned int count);
signed char Flash_Erase_Block(unsigned int *far start_address,\
                              unsigned int *far end_address);

/* Error codes */
#define Flash_Odd_Access         -1
#define Flash_Not_Erased         -2
#define Access_Error             -3
#define Protection_Error         -4
#define Not_StartofSector_Error  -5

#endif   /*FLASH_H*/

