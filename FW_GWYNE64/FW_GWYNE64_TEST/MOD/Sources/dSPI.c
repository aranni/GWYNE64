#include <hidef.h>
#include <string.h>

#include "debug.h"
#include "MC9S12NE64.h"

#include "Commdef.h"
#include "dSPI.h"
#include "Global.h"

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
  
  UINT32  TmpVarFirma;

  IIC_ReadRTCMem (4 ,NVR_ADDR_FIRMA, (UINT8*)(&TmpVarFirma));
  if ( TmpVarFirma != NVR_VAL_FIRMA ){
    FLA_LogsPtr.NextToWrite = FLASH_ADDR_LOGSINI;
    FLA_LogsPtr.NextToRead  = FLASH_ADDR_LOGSINI;
    FLA_LogsPtr.FirstToRead = FLASH_ADDR_LOGSINI;
    IIC_WriteRTCMem (sizeof(FLA_LogsPtr) ,NVR_ADDR_LOGSPTR, (UINT8*)(&FLA_LogsPtr));
    TmpVarFirma = NVR_VAL_FIRMA;
    IIC_WriteRTCMem (4 ,NVR_ADDR_FIRMA, (UINT8*)(&TmpVarFirma));
  } 
  else
    IIC_ReadRTCMem (sizeof(FLA_LogsPtr) ,NVR_ADDR_LOGSPTR, (UINT8*)(&FLA_LogsPtr));
   
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
//	  UINT8	StatusRead[2];
	  	
    FOREVER {
 	    ((FLASH_REG_FORMAT*)SpiBuf)->Command 	= FLASH_CMD_READSTATUS;	
 	    ((FLASH_REG_FORMAT*)SpiBuf)->AddressPAH = 0x00;
//		  StatusRead[0] = FLASH_CMD_READSTATUS;
		  SPI_TransferMsg ( 2, 0, 0x00 );		  
//		  if (StatusRead[1] & 0x80) break;
		  if (SpiBuf[1] & 0x80) break;
	  }
}



void FLA_ReadBuff (UINT32 Addr, UINT16 DataLen, UINT8* DataTargetPtr)
{
/*
	UINT16  DataLenLeft;
	UINT16   MaxLenPage;

  DataLenLeft = DataLen;
  MaxLenPage = (UINT16)(Addr & 0x00FF);
  MaxLenPage = 0x0100-MaxLenPage;
  
  if (DataLenLeft <= MaxLenPage){
    FLA_ReadBlock (Addr, DataLenLeft, DataTargetPtr);
    return;
  }
  
  while (DataLenLeft){
    FLA_ReadBlock (Addr, MaxLenPage, DataTargetPtr);

    Addr += MaxLenPage;
    DataLenLeft -= MaxLenPage;
    DataTargetPtr += MaxLenPage;
    
    MaxLenPage = (UINT16)(Addr & 0x00FF);
    MaxLenPage = 0x0100-MaxLenPage;

    if (DataLenLeft <= MaxLenPage){
      FLA_ReadBlock (Addr, DataLenLeft, DataTargetPtr);
      return;
    }

  }
*/
	return;
}



void FLA_ReadBlock (UINT32 Addr, UINT16 DataLen, UINT8* DataTargetPtr) 
{
	UINT32	TmpDWord;
  

	FLA_WaitFree();

	TmpDWord = Addr;
	TmpDWord	>>= 8;
	TmpDWord	>>= 7;
	TmpDWord	&= 0x00ff;
  ((FLASH_REG_FORMAT*)SpiBuf)->AddressPAH = (UINT8)TmpDWord;

	TmpDWord = Addr;
	TmpDWord	>>= 8;
	TmpDWord	<<= 1;
	TmpDWord	&= 0x00ff;
  ((FLASH_REG_FORMAT*)SpiBuf)->AddressPAL = (UINT8)TmpDWord;

	TmpDWord = Addr;
	TmpDWord	&= 0x00ff;
  ((FLASH_REG_FORMAT*)SpiBuf)->AddressBA  = (UINT8)TmpDWord;

  ((FLASH_REG_FORMAT*)SpiBuf)->DontCare_1 = 0x00;
  ((FLASH_REG_FORMAT*)SpiBuf)->DontCare_2 = 0x00;
  ((FLASH_REG_FORMAT*)SpiBuf)->DontCare_3 = 0x00;
  ((FLASH_REG_FORMAT*)SpiBuf)->DontCare_4 = 0x00;
  ((FLASH_REG_FORMAT*)SpiBuf)->Command 	= FLASH_CMD_READPAGE;	

	SPI_TransferMsg ( 8, DataLen, (UINT8*)DataTargetPtr );

  return;
  
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



// ---------------------------------------------------------------------------
//  Lectura y escritura de LOGs de eventos en Flash
// ---------------------------------------------------------------------------
//
void FLA_SaveLog (UINT8 Param)
{

	UINT16	TmpWord;

  	IIC_RtcGetDateTime();	
	(void)memcpy ( (UINT8*)&(((LOG_FORMAT*)DebugBuffer)->LogDateTime),
				   (UINT8*)&(StatisticData.CurrentDateTime), sizeof(DATE_TIME) );


 	((FLASH_REG_FORMAT*)SpiBuf)->Command 	= FLASH_CMD_WRITEBUF1;	
 	((FLASH_REG_FORMAT*)SpiBuf)->AddressPAH = 0x00;
 	((FLASH_REG_FORMAT*)SpiBuf)->AddressPAL = 0x00;
 	((FLASH_REG_FORMAT*)SpiBuf)->AddressBA  = 0x00;

 	((LOG_FORMAT*)DebugBuffer)->EstadoEje1	= 0;
 	((LOG_FORMAT*)DebugBuffer)->EstadoEje2	= 0;
 	
	FLA_WaitFree();
		
	SPI_TransferMsg ( 4, sizeof(LOG_FORMAT), (UINT8*)(DebugBuffer) );


	if ((FLA_LogsPtr.NextToWrite < FLASH_MIN_LOGPTR) ||
		(FLA_LogsPtr.NextToWrite > FLASH_MAX_LOGPTR) ){
		
//		MON_FatalError(ERR_RANGE_LOG);    				
      return;
	}
	
	TmpWord = FLA_LogsPtr.NextToWrite;
	TmpWord	>>= 7;
	TmpWord	&= 0x00ff;
 	((FLASH_REG_FORMAT*)SpiBuf)->AddressPAH = (UINT8)TmpWord;

	TmpWord = FLA_LogsPtr.NextToWrite;
	TmpWord	<<= 1;
	TmpWord	&= 0x00ff;
 	((FLASH_REG_FORMAT*)SpiBuf)->AddressPAL = (UINT8)TmpWord;
 	((FLASH_REG_FORMAT*)SpiBuf)->AddressBA  = 0x00;
 	((FLASH_REG_FORMAT*)SpiBuf)->Command 	= FLASH_CMD_PROGEBUF1;
  	
	SPI_TransferMsg ( 4, 0, 0x00 );

	FLA_LogsPtr.NextToWrite++;
	if (FLA_LogsPtr.NextToWrite >= FLASH_MAX_PAGEADDR ) FLA_LogsPtr.NextToWrite = FLASH_ADDR_LOGSINI;

	if (FLA_LogsPtr.NextToWrite == FLA_LogsPtr.FirstToRead ){
	  FLA_LogsPtr.FirstToRead++;
		if (FLA_LogsPtr.FirstToRead >= FLASH_MAX_PAGEADDR ) FLA_LogsPtr.FirstToRead = FLASH_ADDR_LOGSINI;
	}

	IIC_WriteRTCMem (sizeof(FLA_LogsPtr) ,NVR_ADDR_LOGSPTR, (UINT8*)(&FLA_LogsPtr));

	return;
}


UINT8 FLA_ReadLog (void)
{
	UINT16	TmpWord;

	if (FLA_LogsPtr.NextToWrite == FLA_LogsPtr.NextToRead ) return ERROR;

	FLA_WaitFree();

	TmpWord = FLA_LogsPtr.NextToRead;
	TmpWord	>>= 7;
	TmpWord	&= 0x00ff;
 	((FLASH_REG_FORMAT*)SpiBuf)->AddressPAH = (UINT8)TmpWord;

	TmpWord = FLA_LogsPtr.NextToRead;
	TmpWord	<<= 1;
	TmpWord	&= 0x00ff;
 	((FLASH_REG_FORMAT*)SpiBuf)->AddressPAL = (UINT8)TmpWord;
 	((FLASH_REG_FORMAT*)SpiBuf)->AddressBA  = 0x00;

 	((FLASH_REG_FORMAT*)SpiBuf)->DontCare_1 = 0x00;
 	((FLASH_REG_FORMAT*)SpiBuf)->DontCare_2 = 0x00;
 	((FLASH_REG_FORMAT*)SpiBuf)->DontCare_3 = 0x00;
 	((FLASH_REG_FORMAT*)SpiBuf)->DontCare_4 = 0x00;
 	((FLASH_REG_FORMAT*)SpiBuf)->Command 	= FLASH_CMD_READPAGE;	

	SPI_TransferMsg ( 8, sizeof(LOG_FORMAT), (UINT8*)DebugBuffer );

	FLA_LogsPtr.NextToRead++;
	if (FLA_LogsPtr.NextToRead >= FLASH_MAX_PAGEADDR ) FLA_LogsPtr.NextToRead = FLASH_ADDR_LOGSINI;

	return OK;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


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

/*
 	((FLASH_REG_FORMAT*)SpiBuf)->DontCare_1 = FLASH_CMD_WRITEBUF1;
 	((FLASH_REG_FORMAT*)SpiBuf)->DontCare_2 = 0x00;
 	((FLASH_REG_FORMAT*)SpiBuf)->DontCare_3 = 0x00;
 	((FLASH_REG_FORMAT*)SpiBuf)->DontCare_4 = 0x00;

 	(void) memcpy (((UINT8*)(&(((FLASH_REG_FORMAT*)SpiBuf)->Data[0]))),
  			 	   ((UINT8*)DataSourcePtr), DataLen);
*/

	SPI_TransferMsg ( 4, DataLen, (UINT8*)DataSourcePtr );  
//					 ((UINT8*)(&(((FLASH_REG_FORMAT*)SpiBuf)->DontCare_1))));
	
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

//  	(void) memcpy ( (DataTargetPtr),
//  					((UINT8*)(&(((FLASH_REG_FORMAT*)SpiBuf)->Data[0]))),
//  				   	DataLen);

	return;
}



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void flash_wre(void)
{
  flash_cs_l();
  (void)flash_spi(FLASH_CMD_WREN);
  flash_cs_h();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void flash_wait_WIP(void)
{
  flash_cs_l();
  (void)flash_spi(FLASH_CMD_RDSR);
  while(flash_spi(0x00) & 0x01)
      kick_WD();
  flash_cs_h();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void flash_bulk_erase(void)
{
  flash_wre();
  flash_cs_l();
  (void)flash_spi(FLASH_CMD_BE);
  flash_cs_h();
  flash_wait_WIP();
}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void flash_program(unsigned long addr, int len, unsigned char (*flash_callback_write)(unsigned long))
{
int nblock;

while(len > 0)
    {
    nblock = (int)(256 - (addr & 0xFF));
    if(len < nblock)
        nblock = len;
    flash_wre();
    flash_cs_l();
    (void)flash_spi(FLASH_CMD_PP);
    (void)flash_spi((unsigned char)(addr>>16));
    (void)flash_spi((unsigned char)(addr>>8));
    (void)flash_spi((unsigned char)(addr>>0));
    len  -= nblock;
    while(nblock-- > 0)
        {
        kick_WD();
        (void)flash_spi(flash_callback_write(addr++));
        }
    flash_cs_h();
    flash_wait_WIP();
    }
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void flash_read(unsigned long addr, int len, void (*flash_callback_read)(unsigned long, unsigned char))
{
  flash_cs_l();
  (void)flash_spi(FLASH_CMD_FAST_READ);
  (void)flash_spi((unsigned char)(addr>>16));
  (void)flash_spi((unsigned char)(addr>>8));
  (void)flash_spi((unsigned char)(addr>>0));
  (void)flash_spi(0x00);        // Dummy Byte
  while(len-- > 0)
      {
      kick_WD();
      flash_callback_read(addr++, flash_spi(0x00));
      }
  flash_cs_h();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void flash_read_buffer(unsigned long addr, unsigned char *buf, int len)
{
  flash_cs_l();
  (void)flash_spi(FLASH_CMD_FAST_READ);
  (void)flash_spi((unsigned char)(addr>>16));
  (void)flash_spi((unsigned char)(addr>>8));
  (void)flash_spi((unsigned char)(addr>>0));
  (void)flash_spi(0x00);        // Dummy Byte
  while(len-- > 0)
      {
      kick_WD();
      *buf++ = flash_spi(0x00);
      }
  flash_cs_h();
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
      kick_WD();
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
      kick_WD();
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
           kick_WD();
          (void)eeprom_spi(*buf++);
          addr++;
          }
      eeprom_cs_h();
      eeprom_wait_WIP();
      }
  spi_high_speed();       // switch to high speed (for serial FLASH)
 }

