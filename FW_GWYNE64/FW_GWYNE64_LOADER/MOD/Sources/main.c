#include <hidef.h>          // common defines and macros
#include <MC9S12NE64.h>     // derivative information

#include "Commdef.h"
#include "flash.h"

#pragma LINK_INFO DERIVATIVE "1"

extern void FLA_ReadReg   (UINT16 RegIdx, UINT8 DataLen, UINT8* DataTargetPtr);
extern void init_serial_memories (void);
extern void eeprom_read_buffer   (unsigned int addr, unsigned char *buf, int len);
extern void eeprom_write_buffer  (unsigned int addr, unsigned char *buf, int len);


FLASH_FWREG_FORMAT  FileReg;
UINT16  FwFileReg;
UINT8   FwFileRegLen;
UINT8   FwFileRegCntr;
UINT8   FwStateFlag;

UINT8         CRYP_INI;
//UINT8         CRYP_MUL;
//UINT8         CRYP_SUM;
#define    CRYP_MUL   (UINT8)13
#define    CRYP_SUM   (UINT8)1

UINT8         CryptoByte;
UINT8         CryptoRegLen;
UINT8 far *   CryptoRegPtr;


void main(void) {
 
    volatile signed char  rval; 
    unsigned long         i;
    UINT8                 TmpByte; 
    UINT8                 RestoreSwitchCounter;
    UINT8                 CharCntr;
    UINT16                WaitCntr;
    
    
    
//    CRYP_MUL = 13;
//    CRYP_SUM = 1;


    PORTA = 0x00;                       // Sets Port A data to 0x00
    DDRA  = 0x00;                       // Sets Port A as input
    LED_RED_CFG    |= LED_RED_MASK;		  // Puerto como salida
    LED_GREEN_CFG  |= LED_GREEN_MASK;		// Puerto como salida
    
    init_serial_memories();    
    eeprom_read_buffer(EE_ADDR_FW_UPDATE, &FwStateFlag, 1);

    for (WaitCntr=0 ; WaitCntr<TMR_WAIT_ON ; WaitCntr++)
      LED_GREEN_ON;

    LED_OFF;

    
//----------------------------------------------------------------------
    // Si el flag es LDR_UPDATE_VALID entonces ya corrió alguna vez
    // el FW cargado sin problemas así que : STARTUP con Watchdog
    if (FwStateFlag == LDR_UPDATE_VALID){
         
      // Habilitar WATCHDOG y saltar al startup de FW cargado..
      TmpByte = 0x00;
      TmpByte |= COPCTL_CR0_MASK;
      TmpByte |= COPCTL_CR1_MASK;
      TmpByte |= COPCTL_CR2_MASK;
      COPCTL = TmpByte;
      ARMCOP = 0x55;
      ARMCOP = 0xAA;
      
      __asm{
        ldx     #0
        ldy     $0EFFE,X      // Reset pseudo vector
        jmp     0,Y
      }
    
    }
    

//----------------------------------------------------------------------
    // Si el flag es LDR_UPDATE_TEST hubo una actualización y se forzó 
    // un reset para probar el código cargado, entonces STARTUP con Watchdog 
    if (FwStateFlag == LDR_UPDATE_TEST){
    
      // Antes grabo el flag con LDR_FACTORY_TEST; si el FW cargado
      // no cambia a LDR_UPDATE_VALID se restablecerá el FW de fábrica.
      FwStateFlag =LDR_FACTORY_TEST;		      
      eeprom_write_buffer(EE_ADDR_FW_UPDATE, &FwStateFlag, 1);     
      
      // Habilitar WATCHDOG e intentar startup de FW cargado..
      TmpByte = 0x00;
      TmpByte |= COPCTL_CR0_MASK;
      TmpByte |= COPCTL_CR1_MASK;
      TmpByte |= COPCTL_CR2_MASK; 
      COPCTL = TmpByte;
      ARMCOP = 0x55;
      ARMCOP = 0xAA;
      
      __asm{
        ldx     #0
        ldy     $0EFFE,X      // Reset pseudo vector
        jmp     0,Y
      }    
    
    }
    

//----------------------------------------------------------------------
    // Si el flag es LDR_FACTORY_TEST (nibble alto) hubo una recuperación de
    // FW de fabrica, por n-esima vez. Si es IMPAR  hay que intentar ejecutarlo 
    // Incrementar el contador de intentos y saltar al STARTUP con Watchdog 
    if ((FwStateFlag >= LDR_FACTORY_TEST) && (FwStateFlag & 0x01)){
    
      if (FwStateFlag >= (LDR_FACTORY_TEST + (2*LDR_FACTORY_ATTEMPT)) ){    

        // si hubo más de N intentos PARAR DEFINITIVAMENTE !!
        RestoreSwitchCounter = 0x00;
        FOREVER{
            if((PORTA & 0x01) == 0){      // Resetear configuración.
              RestoreSwitchCounter++;
              if ( RestoreSwitchCounter > MAX_RESTORESWITCH_CNTR ){
                while(!(PORTA & 0x01)){
                }
                // Si se presiona el SWITCH resetear y volver a intentar 
                FwStateFlag = LDR_FACTORY_TEST;		      
                eeprom_write_buffer(EE_ADDR_FW_UPDATE, &FwStateFlag, 1);     
                TmpByte = 0x00;
                TmpByte |= COPCTL_CR0_MASK;
                TmpByte |= COPCTL_CR1_MASK;
                TmpByte |= COPCTL_CR2_MASK; 
                COPCTL = TmpByte;
                ARMCOP = 0x55;
                ARMCOP = 0xAA;
              }
            } 
            else
              RestoreSwitchCounter = 0x00;
          
        }
        
      }
      
      // Incremento el flag,los valores mayores a LDR_FACTORY_TEST y
      // Impares : indican la orden de intentar un START 
      // Pares   : indican que falló la carga porque no cambió a _VALID  
      FwStateFlag++;		      
      eeprom_write_buffer(EE_ADDR_FW_UPDATE, &FwStateFlag, 1);     
      
      // Habilitar WATCHDOG e intentar startup de FW de FABRICA..
      TmpByte = 0x00;
      TmpByte |= COPCTL_CR0_MASK;
      TmpByte |= COPCTL_CR1_MASK;
      TmpByte |= COPCTL_CR2_MASK; 
      COPCTL = TmpByte;
      ARMCOP = 0x55;
      ARMCOP = 0xAA;
      
      __asm{
        ldx     #0
        ldy     $0EFFE,X      // Reset pseudo vector
        jmp     0,Y
      }    
    
    }
    
//----------------------------------------------------------------------
//----------------------------------------------------------------------

    // Si se llega hasta aquí es porque el flag es 
    // LDR_FACTORY_TEST: el FW cargado no pudo cambiar a _VALID
    // o LDR_UPDATE_LOAD: hubo un pedido de UPDATE desde IMP_Config.

    for (WaitCntr=0 ; WaitCntr<TMR_WAIT_ON ; WaitCntr++)
      LED_RED_ON;

 
    if (FwStateFlag == LDR_UPDATE_LOAD){
      
      FwFileReg = FLASH_UPDATE_FWFILE;          
      
      FwStateFlag = LDR_UPDATE_TEST;      
      eeprom_write_buffer(EE_ADDR_FW_UPDATE, &FwStateFlag, 1);     

      LED_RED_ON;

    } 
    else{
    
      FwFileReg = FLASH_FACTORY_FWFILE;    
          
      if ( ( FwStateFlag < LDR_FACTORY_TEST ) ||
           ( FwStateFlag > (LDR_FACTORY_TEST + (2*LDR_FACTORY_ATTEMPT))) ){
        
        // si está fuera de rango por algún error, empiezo del principio
        FwStateFlag = LDR_FACTORY_TEST;          
      }

      // Incremento el flag,los valores mayores a LDR_FACTORY_TEST y
      // Impares : indican la orden de intentar un START 
      // Pares   : indican que falló la carga porque no cambió a _VALID  
      FwStateFlag++;
      eeprom_write_buffer(EE_ADDR_FW_UPDATE, &FwStateFlag, 1);     

      LED_GREEN_ON;

    }



//----------------------------------------------------------------------
 
   
    Flash_Init(25000);	        // Frecuencia del oscilador
  
    for (i=0x3C; i<0x3F; i++)   // BORRAR TODO 3C, 3D Y 3E
    {
        rval=Flash_Erase_Block((unsigned int *far)((i<<16) + 0x8000),
                               (unsigned int *far)((i<<16) + 0xBE00));
    }
    
    // En la última pagina borro hasta donde empieza el loader (0-12K)
    i = 0x3F;
    rval=Flash_Erase_Block((unsigned int *far)((i<<16) + 0x8000),
                           (unsigned int *far)((i<<16) + 0xAE00));
                                    

//----------------------------------------------------------------------
		 

    FLA_ReadReg (FwFileReg, sizeof(FLASH_FWREG_FORMAT), (UINT8*) &(FileReg));
    FwFileReg++;
    
    FwFileRegLen = ((CFG_FW_HEADER*)(&(FileReg.Data[0])))->RegInFlashQtty;

    // Desencriptar la parte encriptada del HEADER..
    CRYP_INI = (UINT8)(((CFG_FW_HEADER*)(&(FileReg.Data[0])))->VersionInfo.TimeOfCompilation[6]); // decena de segundos
    CRYP_INI = (((CRYP_INI * CRYP_MUL) + CRYP_SUM ) & 0xFF);

		// Comienza a encriptarse en el campo: TransferInfo
		CryptoRegPtr = (UINT8*) &(((CFG_FW_HEADER*)(&(FileReg.Data[0])))->TransferInfo);							
    CryptoRegLen = sizeof(CFG_FW_HEADER) - (sizeof(CFG_FW_VERSION) + 2);
                
    for ( CharCntr = 0; CharCntr < CryptoRegLen ; CharCntr++){

        // CRYPTO..
        CryptoByte = *CryptoRegPtr; 
        CryptoByte ^= CRYP_INI;
        *CryptoRegPtr = CryptoByte; 
        CryptoRegPtr++;
        
        CRYP_INI = (((CRYP_INI * CRYP_MUL) + CRYP_SUM ) & 0xFF);
                
    }
    
    // Una vez desencriptada la info del HEADER, grabarla en la EEPROM
    eeprom_write_buffer(EE_ADDR_TRIAL_CODE, 
                        (UINT8*) &(((CFG_FW_HEADER*)(&(FileReg.Data[0])))->RestrictType ),
                        1); 
                            
    // Si hay que transferir la información de Limitaciones, grabarla en EEPROM 
    if ( ((((CFG_FW_HEADER*)(&(FileReg.Data[0])))->TransferInfo) & 0x02) ){

      eeprom_write_buffer(EE_ADDR_LIMIT_FROMSRL, 
                        (UINT8*) &(((CFG_FW_HEADER*)(&(FileReg.Data[0])))->RestrictFromSerial[0] ),
                        10);  
                           
      eeprom_write_buffer(EE_ADDR_LIMIT_TOSRL, 
                        (UINT8*) &(((CFG_FW_HEADER*)(&(FileReg.Data[0])))->RestrictToSerial[0] ),
                        10);     

      eeprom_write_buffer(EE_ADDR_LIMIT_MSGQTTY, 
                        (UINT8*) &(((CFG_FW_HEADER*)(&(FileReg.Data[0])))->TrialMsgLimit ),
                        4); 
      eeprom_write_buffer(EE_ADDR_LIMIT_TIME, 
                        (UINT8*) &(((CFG_FW_HEADER*)(&(FileReg.Data[0])))->TrialTimeLimit ),
                        4); 
      eeprom_write_buffer(EE_ADDR_LIMIT_OTHER, 
                        (UINT8*) &(((CFG_FW_HEADER*)(&(FileReg.Data[0])))->TrialOtherLimit ),
                        4); 
    }

    // Si hay que transferir la información adicional, grabarla en EEPROM 
    if ( ((((CFG_FW_HEADER*)(&(FileReg.Data[0])))->TransferInfo) & 0x01) ){
      
      eeprom_write_buffer(EE_ADDR_LDR_INFO, 
                          (UINT8*) &(((CFG_FW_HEADER*)(&(FileReg.Data[0])))->FW_Autor[0] ),
                          100);     
    }


//----------------------------------------------------------------------

		// A partir de aquí se comienza a leer el codigo de FW, encriptado completamente
    CRYP_INI = (UINT8)(((CFG_FW_HEADER*)(&(FileReg.Data[0])))->VersionInfo.TimeOfCompilation[7]); // unidad de segundos
    CRYP_INI = (((CRYP_INI * CRYP_MUL) + CRYP_SUM ) & 0xFF);

    for (FwFileRegCntr=2; FwFileRegCntr<=FwFileRegLen; FwFileRegCntr++)
    {
    
      FLA_ReadReg (FwFileReg, sizeof(FLASH_FWREG_FORMAT), (UINT8*) &(FileReg));


			CryptoRegPtr = (UINT8*) &(FileReg);
								
      CryptoRegLen = *CryptoRegPtr; 
      CryptoRegLen ^= CRYP_INI;
      *CryptoRegPtr = CryptoRegLen; 
      CryptoRegPtr++;
      CRYP_INI = (((CRYP_INI * CRYP_MUL) + CRYP_SUM ) & 0xFF);
                
      for ( CharCntr = 1; CharCntr <= CryptoRegLen ; CharCntr++){

        // CRYPTO..
        CryptoByte = *CryptoRegPtr; 
        CryptoByte ^= CRYP_INI;
        *CryptoRegPtr = CryptoByte; 
        CryptoRegPtr++;
        
        CRYP_INI = (((CRYP_INI * CRYP_MUL) + CRYP_SUM ) & 0xFF);
                
      }

 
      FileReg.Len -= 4;           // 3 del address y 1 del CheckSum
      if (FileReg.Len & 0x0001){  // Si es una longitud impar
          FileReg.Data[FileReg.Len] = 0xFF;                
          FileReg.Len++;					// Redondear para alinear a words
      }
      rval=Flash_Write_Block ( (unsigned int *far)(&(FileReg.Data[0])), 
                               FileReg.FirstAddress,
                               (UINT16)((FileReg.Len)/2));
      FwFileReg++;                               

    }
  
    LED_OFF;

    TmpByte = 0x00;
    TmpByte |= COPCTL_CR0_MASK;
    TmpByte |= COPCTL_CR1_MASK;
    TmpByte |= COPCTL_CR2_MASK;
    COPCTL = TmpByte;
    ARMCOP = 0x55;
    ARMCOP = 0xAA;
   
//    EnableInterrupts;

    FOREVER;
    
}


//---------------------------------------------------------------------
//---------------------------------------------------------------------

//  Valores de refresco para el WatchDog según registro CR;
//
//  CR [0-1-2] = 000 ->   0.6 ms  OSCCLK * 2^14
//  CR [0-1-2] = 010 ->   2.6 ms  OSCCLK * 2^16
//  CR [0-1-2] = 011 ->  10.5 ms  OSCCLK * 2^18
//  CR [0-1-2] = 100 ->  41.9 ms  OSCCLK * 2^20
//  CR [0-1-2] = 101 -> 167.8 ms  OSCCLK * 2^22
//  CR [0-1-2] = 110 -> 335.5 ms  OSCCLK * 2^23
//  CR [0-1-2] = 111 -> 671.0 ms  OSCCLK * 2^24   (pag. 169)
  



//    PAra hacer la llamada al startup en C:
//
//    UINT16  * EntryPointValPtr;
//    void (*EntryPoint)  (void);  
//
//    EntryPointValPtr = (UINT16*)0xFFFA;
//    FwFileReg	 = *EntryPointValPtr;
//    EntryPoint = (void(*)())FwFileReg; 
//    EntryPoint();   


//  Para leer primero la len en la flash..
//
//  FLA_ReadReg (FwFileReg, 0x01, (UINT8*) &FwFileRegLen);
//  FLA_ReadReg (FwFileReg, FwFileRegLen, (UINT8*) &(FileReg));
 

//  Calculo de checksum
//
//    UINT8                 CharCntr, CheckSum;
//    UINT8 *               RegBytePtr;
//
//			CheckSum = 0x00;
//			RegBytePtr = (UINT8*)(&FileReg);
//			
//			for (CharCntr = 0; CharCntr < FileReg.Len ;CharCntr++){
//			  CheckSum += *RegBytePtr;
//			  RegBytePtr++;  			  
//			}
//			if (CheckSum != 0xFF){
//        
        // QUÉ HAGO ACÁ-- !!!			  
//			  
//			}
