#include <hidef.h>
#include <string.h>

#include "MC9S12NE64.h"

#include "Commdef.h"
#include "dSPI.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void init_serial_memories(void)
{
  
  SPICR1 = 0x50;    // SPI enabled, No interrupt, 
                    // Master Mode, CPOL=0, CPHA=0, MSB first
  SPICR2 = 0x00;		// MODFEN=0, Normal pin (MISO & MOSI)
  SPIBR = 0x00;		  // BaudRate = 12.5 MHz 


  PTS |= 0x80;      // CS for serial FLASH
  DDRS |= 0x80;

  PORTE |= 0x80;    // CS for serial EEPROM
  DDRE |= 0x80;

  eeprom_unlock();

  
}

// ---------------------------------------------------------------------------
// Serial FLASH functions (512 KByte)
// ---------------------------------------------------------------------------
void FLA_Init(void)
{
  
  return; 
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

unsigned char flash_spi(unsigned char data)
{
  while(!(SPISR & 0x20))
      ;
  SPIDR = data;
  while(!(SPISR & 0x80))
      ;
  return(SPIDR);
}


// ---------------------------------------------------------------------------
// Transfiere un buffer a la memoria FLASH
// ---------------------------------------------------------------------------
//
void SPI_TransferMsg ( UINT8 HdrLen, UINT8 DataLen, UINT8* MsgPtr ) 
{
    UINT8 TmpData;
    UINT8 * HdrPtr;
    
    HdrPtr = SpiBuf;
      
    flash_cs_l();
    while(HdrLen){
      REFRESH_WATCHDOG;
      TmpData = *HdrPtr;
      *HdrPtr = flash_spi(TmpData);
      HdrPtr++;
      HdrLen--;
    }
    while(DataLen){
      REFRESH_WATCHDOG;
      TmpData = *MsgPtr;
      *MsgPtr = flash_spi(TmpData);
      MsgPtr++;
      DataLen--;
    }
    flash_cs_h();
  
}

// ---------------------------------------------------------------------------
// Espera hasta que la FLASH deje de estar Busy
// ---------------------------------------------------------------------------
//
void FLA_WaitFree (void)
{
	  	
    FOREVER {
 	    ((FLASH_REG_FORMAT*)SpiBuf)->Command 	= FLASH_CMD_READSTATUS;	
 	    ((FLASH_REG_FORMAT*)SpiBuf)->AddressPAH = 0x00;
		  SPI_TransferMsg ( 2, 0, 0x00 );		  
		  if (SpiBuf[1] & 0x80) break;
	  }
}



// ---------------------------------------------------------------------------
// Lectura y escritura de datos en FLASH (genérica, para datos de config)
// ---------------------------------------------------------------------------
//
void FLA_SaveReg (UINT16 RegIdx, UINT8 DataLen, UINT8* DataSourcePtr)
{
	UINT16	TmpWord;

	FLA_WaitFree();
	
 	((FLASH_REG_FORMAT*)SpiBuf)->Command 	= FLASH_CMD_WRITEBUF1;	
 	((FLASH_REG_FORMAT*)SpiBuf)->AddressPAH = 0x00;
 	((FLASH_REG_FORMAT*)SpiBuf)->AddressPAL = 0x00;
 	((FLASH_REG_FORMAT*)SpiBuf)->AddressBA  = 0x00;


	SPI_TransferMsg ( 4, DataLen, (UINT8*)DataSourcePtr );  
	
	TmpWord = RegIdx;
	TmpWord	>>= 7;
  TmpWord	&= 0x00ff;
  ((FLASH_REG_FORMAT*)SpiBuf)->AddressPAH = (UINT8)TmpWord;

	TmpWord = RegIdx;
	TmpWord	<<= 1;
	TmpWord	&= 0x00ff;
 	((FLASH_REG_FORMAT*)SpiBuf)->AddressPAL = (UINT8)TmpWord;
 	((FLASH_REG_FORMAT*)SpiBuf)->AddressBA  = 0x00;
 	((FLASH_REG_FORMAT*)SpiBuf)->Command 	= FLASH_CMD_PROGEBUF1;

	SPI_TransferMsg ( 4, 0, 0x00 );

	return;
}


void FLA_ReadReg (UINT16 RegIdx, UINT8 DataLen, UINT8* DataTargetPtr)
{
	UINT16	TmpWord;

	FLA_WaitFree();

	TmpWord = RegIdx;
	TmpWord	>>= 7;
	TmpWord	&= 0x00ff;
  ((FLASH_REG_FORMAT*)SpiBuf)->AddressPAH = (UINT8)TmpWord;

	TmpWord = RegIdx;
	TmpWord	<<= 1;
	TmpWord	&= 0x00ff;
  ((FLASH_REG_FORMAT*)SpiBuf)->AddressPAL = (UINT8)TmpWord;
  ((FLASH_REG_FORMAT*)SpiBuf)->AddressBA  = 0x00;

  ((FLASH_REG_FORMAT*)SpiBuf)->DontCare_1 = 0x00;
  ((FLASH_REG_FORMAT*)SpiBuf)->DontCare_2 = 0x00;
  ((FLASH_REG_FORMAT*)SpiBuf)->DontCare_3 = 0x00;
  ((FLASH_REG_FORMAT*)SpiBuf)->DontCare_4 = 0x00;
  ((FLASH_REG_FORMAT*)SpiBuf)->Command 	= FLASH_CMD_READPAGE;	

	SPI_TransferMsg ( 8, DataLen, (UINT8*)DataTargetPtr );

	return;
}



// ---------------------------------------------------------------------------
// Serial EEPROM functions (128 bytes)
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
unsigned char eeprom_spi(unsigned char data)
{
  while(!(SPISR & 0x20))
      ;
  SPIDR = data;
  while(!(SPISR & 0x80))
      ;
  return(SPIDR);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void eeprom_wre(void)
{
  eeprom_cs_l();
  (void)eeprom_spi(EEPROM_CMD_WREN);
  eeprom_cs_h();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void eeprom_wait_WIP(void)
{
  eeprom_cs_l();
  (void)eeprom_spi(EEPROM_CMD_RDSR);
  while(eeprom_spi(0x00) & 0x01)
      REFRESH_WATCHDOG;
  eeprom_cs_h();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void eeprom_unlock(void)
{
  spi_low_speed(); 
  eeprom_wre();
  eeprom_cs_l();
  (void)eeprom_spi(EEPROM_CMD_WRSR);
  (void)eeprom_spi(0xF3);
  eeprom_cs_h();
  
  eeprom_wait_WIP();
  spi_high_speed();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void eeprom_read_buffer(unsigned int addr, unsigned char *buf, int len)
{
  spi_low_speed();     
  eeprom_cs_l();
  (void)eeprom_spi(EEPROM_CMD_READ | (unsigned char)((addr >> 5) & 0x08));
  (void)eeprom_spi((unsigned char)addr);
  while(len-- > 0)
      {
      REFRESH_WATCHDOG;
      *buf++ = eeprom_spi(0x00);
      }
  eeprom_cs_h();
  spi_high_speed();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void eeprom_write_buffer(unsigned int addr, unsigned char *buf, int len)
{
  int nblock;

  spi_low_speed();        // switch to low speed 
  while(len > 0)
      {
      nblock = 16 - (addr & 0x0F);
      if(len < nblock)
          nblock = len;
      
      eeprom_wre();
      eeprom_cs_l();
      (void)eeprom_spi(EEPROM_CMD_WRITE | (unsigned char)((addr >> 5) & 0x08));
      (void)eeprom_spi((unsigned char)addr);
      len  -= nblock;
      while(nblock-- > 0)
          {
           REFRESH_WATCHDOG;
          (void)eeprom_spi(*buf++);
          addr++;
          }
      eeprom_cs_h();
      eeprom_wait_WIP();
      }
  spi_high_speed();       // switch to high speed (for serial FLASH)
 }

