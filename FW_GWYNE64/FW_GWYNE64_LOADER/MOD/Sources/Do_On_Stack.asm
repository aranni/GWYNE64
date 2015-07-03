;*******************************************************************
;*                    freescale semiconductor inc 
;*
;*    DESCRIPTION:   S12 Flash Asm Routines
;*    SOURCE:        Do_On_Stach.asm
;*    COPYRIGHT:     © 09/2004   freescale
;*    AUTHOR:        rat579 
;*    REV. HISTORY:  060304 - fixed CCR return value and optimized
;*                            in SpSub routine  
;*                   090904 - Modifed to use standard MW stationery
;*
;*******************************************************************/
;*******************************************************************
; Local defines
;*******************************************************************
; include derivative specific macros
            INCLUDE 'MC9S12ne64.inc'	 ;can change to current MCU but
                                       ;but all registers used are 
                                       ;common to all HCS12s. Really
                                       ;not required.

; export symbols
              xdef    DoOnStack
            ; we use export 'DoOnStack' as symbol. This allows us to
            ; reference 'DoOnStack' either in the linker .prm file
            ; or from C/C++ later on


;*********************************************************************
;* DoOnStack - copy SpSub onto stack and call it (see also SpSub)
;*  De-allocates the stack space used by SpSub after returning from it.
;*  Allows final steps in a flash prog/erase command to execute out
;*  of RAM (on stack) while flash is out of the memory map
;*  This routine can be used for flash word-program or erase commands
;*
;* Calling Convention:
;*           jsr    DoOnStack
;*
;* Uses 32 bytes on stack + 3 bytes if Call instruction used
;********************************************************************
DoOnStack:  
            pshb                 ;save B - PPAGE

            ldx   #SpSubEnd-2    ;point at last word to move to stack
SpmoveLoop: ldd    2,x-          ;read from flash
            pshd                 ;move onto stack
            cpx   #SpSub-2       ;past end?
            bne    SpmoveLoop    ;loop till whole sub on stack
            tfr    sp,x          ;point to sub on stack
            ldaa  #mFSTAT_CBEIF  ;preload mask to register command
            call   0,x,00        ;execute the sub on the stack
            leas   SpSubEnd-SpSub,sp  ;de-allocate space used by sub
            pulb                 ;restore B
            rtc                  ;to flash where DoOnStack was called
                                 ; assume banked calling function
                                 
;*********************************************************************
;* SpSub - register flash command and wait for Flash CCIF
;*  this subroutine is copied onto the stack before executing
;*  because you can't execute out of flash while a flash command is
;*  in progress (see DoOnStack to see how this is used)
;*
;* Note: must be even # of bytes!
;*
;* Uses 24 bytes on stack + 3 bytes for CALL above
;*********************************************************************
            EVEN                 ;Make code start word aliened
SpSub:       
            ldab   SpSubEnd-SpSub+3,sp  ;get PPAGE back from stack
            stab   PPAGE                ;Store the PPAGE address
            tfr    ccr,b                ;get copy of ccr
            orcc  #$10                  ;disable interrupts
            staa   FSTAT                ;[PwO] register command
            nop                         ;[O] wait min 4~ from w cycle to r
            nop                         ;[O]
            nop                         ;[O]
            brclr  FSTAT,mFSTAT_CCIF,*  ;[rfPPP] wait for queued commands to finish
            tfr    b,ccr         ;restore ccr and int condition
            rtc                  ;back into DoOnStack in flash
SpSubEnd:
