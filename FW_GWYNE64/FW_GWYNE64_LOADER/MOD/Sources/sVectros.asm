            XREF  _Startup
;*********************************************************************
;* Include standard definitions that are common to all derivatives
;*********************************************************************
;             base    10           ;ensure default number base to decimal
             nolist               ;turn off listing
             
             list                 ;turn listing back on
;*********************************************************************
;* general equates for bootloader/monitor program valid for all
;* derivatives
;*********************************************************************
BootStart:   equ    $F800         ;start of protected boot block
RamLast:     equ    $3fff         ;last RAM location (all devices)
Window:      equ    $8000         ;PPAGE Window start
RomStart:    equ    $4000         ;start of flash

VecTabSize:  equ    $80                   ;size of vector table
VectorTable: equ    $10000-VecTabSize     ;start of vector table
PVecTable:   equ    BootStart-VecTabSize  ;start of pseudo vector table

BootBlkSize: equ	  4096	        ; Erase protected bootblock size.

FProtStart:  equ    $FF00         ;start of FLASH protection/security
FProtBlksz:  equ    $CF           ;protect code for boot block ($C7 2K, $CF 4K)
;FProtBlksz:  equ    $FF          ;protect code for boot block (none)

;FSecure:     iequ    $BE           ;Disable Security and backdoor access
FSecure:     equ    $00          ;Enable Security and backdoor access



;*************************************************************************************************************
;
;       This is the jump table that is used to access the secondary interrupt vector table. Each one
;       of the actual interrupt vectors, begining at $ff80, points to an entry in this table. Each jmp
;       instruction uses indexed indirect program counter relative (pcr) addressing to access the
;       secondary interrupt vector table that is located just below the bootblock.
;
;*************************************************************************************************************
;
BSRTable:
uvector63:	jmp	   [vector63-BootBlkSize,pcr]    ; /* vector 63 */
uvector62:	jmp    [vector62-BootBlkSize,pcr]    ; /* vector 62 */
uvector61:	jmp    [vector61-BootBlkSize,pcr]    ; /* vector 61 */
uvector60:	jmp    [vector60-BootBlkSize,pcr]    ; /* vector 60 */
uvector59:	jmp    [vector59-BootBlkSize,pcr]    ; /* vector 59 */
uvector58:	jmp    [vector58-BootBlkSize,pcr]    ; /* vector 58 */
uvector57:	jmp    [vector57-BootBlkSize,pcr]    ; /* vector 57 */
uvector56:	jmp    [vector56-BootBlkSize,pcr]    ; /* vector 56 */
uvector55:	jmp    [vector55-BootBlkSize,pcr]    ; /* vector 55 */
uvector54:	jmp    [vector54-BootBlkSize,pcr]    ; /* vector 54 */
uvector53:	jmp    [vector53-BootBlkSize,pcr]    ; /* vector 53 */
uvector52:	jmp    [vector52-BootBlkSize,pcr]    ; /* vector 52 */
uvector51:	jmp    [vector51-BootBlkSize,pcr]    ; /* vector 51 */
uvector50:	jmp    [vector50-BootBlkSize,pcr]    ; /* vector 50 */
uvector49:	jmp    [vector49-BootBlkSize,pcr]    ; /* vector 49 */
uvector48:	jmp    [vector48-BootBlkSize,pcr]    ; /* vector 48 */
uvector47:	jmp    [vector47-BootBlkSize,pcr]    ; /* vector 47 */
uvector46:	jmp    [vector46-BootBlkSize,pcr]    ; /* vector 46 */
uvector45:	jmp    [vector45-BootBlkSize,pcr]    ; /* vector 45 */
uvector44:	jmp    [vector44-BootBlkSize,pcr]    ; /* vector 44 */
uvector43:	jmp    [vector43-BootBlkSize,pcr]    ; /* vector 43 */
uvector42:	jmp    [vector42-BootBlkSize,pcr]    ; /* vector 42 */
uvector41:	jmp    [vector41-BootBlkSize,pcr]    ; /* vector 41 */
uvector40:	jmp    [vector40-BootBlkSize,pcr]    ; /* vector 40 */
uvector39:	jmp    [vector39-BootBlkSize,pcr]    ; /* vector 39 */
uvector38:	jmp    [vector38-BootBlkSize,pcr]    ; /* vector 38 */
uvector37:	jmp    [vector37-BootBlkSize,pcr]    ; /* vector 37 */
uvector36:	jmp    [vector36-BootBlkSize,pcr]    ; /* vector 36 */
uvector35:	jmp    [vector35-BootBlkSize,pcr]    ; /* vector 35 */
uvector34:	jmp    [vector34-BootBlkSize,pcr]    ; /* vector 34 */
uvector33:	jmp    [vector33-BootBlkSize,pcr]    ; /* vector 33 */
uvector32:	jmp    [vector32-BootBlkSize,pcr]    ; /* vector 32 */
uvector31:	jmp    [vector31-BootBlkSize,pcr]    ; /* vector 31 */
uvector30:	jmp    [vector30-BootBlkSize,pcr]    ; /* vector 30 */
uvector29:	jmp    [vector29-BootBlkSize,pcr]    ; /* vector 29 */
uvector28:	jmp    [vector28-BootBlkSize,pcr]    ; /* vector 28 */
uvector27:	jmp    [vector27-BootBlkSize,pcr]    ; /* vector 27 */
uvector26:	jmp    [vector26-BootBlkSize,pcr]    ; /* vector 26 */
uvector25:	jmp    [vector25-BootBlkSize,pcr]    ; /* vector 25 */
uvector24:	jmp    [vector24-BootBlkSize,pcr]    ; /* vector 24 */
uvector23:	jmp    [vector23-BootBlkSize,pcr]    ; /* vector 23 */
uvector22:	jmp    [vector22-BootBlkSize,pcr]    ; /* vector 22 */
uvector21:	jmp    [vector21-BootBlkSize,pcr]    ; /* vector 21 */
uvector20:	jmp    [vector20-BootBlkSize,pcr]    ; /* vector 20 */
uvector19:	jmp    [vector19-BootBlkSize,pcr]    ; /* vector 19 */
uvector18:	jmp    [vector18-BootBlkSize,pcr]    ; /* vector 18 */
uvector17:	jmp    [vector17-BootBlkSize,pcr]    ; /* vector 17 */
uvector16:	jmp    [vector16-BootBlkSize,pcr]    ; /* vector 16 */
uvector15:	jmp    [vector15-BootBlkSize,pcr]    ; /* vector 15 */
uvector14:	jmp    [vector14-BootBlkSize,pcr]    ; /* vector 14 */
uvector13:	jmp    [vector13-BootBlkSize,pcr]    ; /* vector 13 */
uvector12:	jmp    [vector12-BootBlkSize,pcr]    ; /* vector 12 */
uvector11:	jmp    [vector11-BootBlkSize,pcr]    ; /* vector 11 */
uvector10:	jmp    [vector10-BootBlkSize,pcr]    ; /* vector 10 */
uvector09:	jmp    [vector09-BootBlkSize,pcr]    ; /* vector 09 */
uvector08:	jmp    [vector08-BootBlkSize,pcr]    ; /* vector 08 */
uvector07:	jmp    [vector07-BootBlkSize,pcr]    ; /* vector 07 */
uvector06:	jmp    [vector06-BootBlkSize,pcr]    ; /* vector 06 */
uvector05:	jmp    [vector05-BootBlkSize,pcr]    ; /* vector 05 */
uvector04:	jmp    [vector04-BootBlkSize,pcr]    ; SWI, used for breakpoints (place holder only)
uvector03:	jmp    [vector03-BootBlkSize,pcr]    ; /* vector 03 */
uvector02:	jmp    [vector02-BootBlkSize,pcr]    ; /* vector 02 */
uvector01:	jmp    [vector01-BootBlkSize,pcr]    ; /* vector 01 */
uvector00:  jmp    [vector00-BootBlkSize,pcr]    ; /* Reset vector */


;*********************************************************************
;* Define all vectors even if program doesn't use them all
;*********************************************************************

           org    VectorTable
vector63:  fdb    uvector63  ; /* vector 63 */
vector62:  fdb    uvector62  ; /* vector 62 */
vector61:  fdb    uvector61  ; /* vector 61 */
vector60:  fdb    uvector60  ; /* vector 60 */
vector59:  fdb    uvector59  ; /* vector 59 */
vector58:  fdb    uvector58  ; /* vector 58 */
vector57:  fdb    uvector57  ; /* vector 57 */
vector56:  fdb    uvector56  ; /* vector 56 */
vector55:  fdb    uvector55  ; /* vector 55 */
vector54:  fdb    uvector54  ; /* vector 54 */
vector53:  fdb    uvector53  ; /* vector 53 */
vector52:  fdb    uvector52  ; /* vector 52 */
vector51:  fdb    uvector51  ; /* vector 51 */
vector50:  fdb    uvector50  ; /* vector 50 */
vector49:  fdb    uvector49  ; /* vector 49 */
vector48:  fdb    uvector48  ; /* vector 48 */
vector47:  fdb    uvector47  ; /* vector 47 */
vector46:  fdb    uvector46  ; /* vector 46 */
vector45:  fdb    uvector45  ; /* vector 45 */
vector44:  fdb    uvector44  ; /* vector 44 */
vector43:  fdb    uvector43  ; /* vector 43 */
vector42:  fdb    uvector42  ; /* vector 42 */
vector41:  fdb    uvector41  ; /* vector 41 */
vector40:  fdb    uvector40  ; /* vector 40 */
vector39:  fdb    uvector39  ; /* vector 39 */
vector38:  fdb    uvector38  ; /* vector 38 */
vector37:  fdb    uvector37  ; /* vector 37 */
vector36:  fdb    uvector36  ; /* vector 36 */
vector35:  fdb    uvector35  ; /* vector 35 */
vector34:  fdb    uvector34  ; /* vector 34 */
vector33:  fdb    uvector33  ; /* vector 33 */
vector32:  fdb    uvector32  ; /* vector 32 */
vector31:  fdb    uvector31  ; /* vector 31 */
vector30:  fdb    uvector30  ; /* vector 30 */
vector29:  fdb    uvector29  ; /* vector 29 */
vector28:  fdb    uvector28  ; /* vector 28 */
vector27:  fdb    uvector27  ; /* vector 27 */
vector26:  fdb    uvector26  ; /* vector 26 */
vector25:  fdb    uvector25  ; /* vector 25 */
vector24:  fdb    uvector24  ; /* vector 24 */
vector23:  fdb    uvector23  ; /* vector 23 */
vector22:  fdb    uvector22  ; /* vector 22 */
vector21:  fdb    uvector21  ; /* vector 21 */
vector20:  fdb    uvector20  ; /* vector 20 */
vector19:  fdb    uvector19  ; /* vector 19 */
vector18:  fdb    uvector18  ; /* vector 18 */
vector17:  fdb    uvector17  ; /* vector 17 */
vector16:  fdb    uvector16  ; /* vector 16 */
vector15:  fdb    uvector15  ; /* vector 15 */
vector14:  fdb    uvector14  ; /* vector 14 */
vector13:  fdb    uvector13  ; /* vector 13 */
vector12:  fdb    uvector12  ; /* vector 12 */
vector11:  fdb    uvector11  ; /* vector 11 */
vector10:  fdb    uvector10  ; /* vector 10 */
vector09:  fdb    uvector09  ; /* vector 09 */
vector08:  fdb    uvector08  ; /* vector 08 */
vector07:  fdb    uvector07  ; /* vector 07 */
vector06:  fdb    uvector06  ; /* vector 06 */
vector05:  fdb    uvector05  ; /* vector 05 */
vector04:  fdb    uvector04  ; /* vector 04 : Breakpoint */
vector03:  fdb    _Startup   ; /* vector 03 */
vector02:  fdb    _Startup   ; /* vector 02 */
vector01:  fdb    _Startup   ; /* vector 01 */
vector00:  fdb    _Startup   ; /* Reset vector */


;*********************************************************************
;* FLASH configuration: protection, security
;*********************************************************************
             org    FProtStart   ; enable protection
             fdb    $FFFF        ; Skip Backdoor Key
             fdb    $FFFF        ; Skip Backdoor Key
             fdb    $FFFF        ; Skip Backdoor Key
             fdb    $FFFF        ; Skip Backdoor Key

             fdb    $FFFF        ; Skip Reserved

             fcb    $FF          ; protection block 3
             fcb    $FF          ; protection block 2
             fcb    $FF          ; protection block 1
             fcb    FProtBlksz   ; protection block 0
             fcb    $FF          ; Skip
             fcb    FSecure      ; set security and backdoor access


.nolist                      ;skip the symbol table

;*****************************************************************
