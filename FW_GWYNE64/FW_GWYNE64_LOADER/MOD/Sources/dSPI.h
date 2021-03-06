#ifndef dSPIH 
#define dSPIH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
// Instruction Set for serial EEPROM (M95010) 
#define EEPROM_CMD_WREN             0x06    // Write Enable
#define EEPROM_CMD_WRDI             0x04    // Write Disable
#define EEPROM_CMD_RDSR             0x05    // Read Status Register
#define EEPROM_CMD_WRSR             0x01    // Write Status Register
#define EEPROM_CMD_READ             0x03    // Read Data Bytes
#define EEPROM_CMD_WRITE            0x02    // Write Data Bytes

// Instruction Set for serial FLASH (AT45DB041)
#define FLASH_CMD_READSTATUS        0xD7    // Read Status Byte
#define FLASH_CMD_READPAGE          0xD2    // Read Page Data Bytes
#define FLASH_CMD_WRITEBUF1         0x84    // Buffer 1 Write
#define FLASH_CMD_PROGEBUF1         0x83    // Buffer 1 Program Erase.
#define FLASH_CMD_PAGEERASE         0x81    // Page Erase
#define FLASH_CMD_BLOCKERASE        0x50    // Block Erase

// Organización de los 2048 registros de la memoria FLASH
// 0000 - 0099		:  Reservados los primeros 100 registros 
// 0100 - 1023 		:  Para logs de eventos.
// 1024 - 1535		:  512 Reg = 128Kb 
// 1536 - 1791		:  256 Reg = 64Kb para actualización de la flash
// 1792 - 2047		:  256 Reg = 64Kb para recuperación de FW de fabrica.

// Macro definitions
#define flash_cs_h()                PTS |= 0x80  
#define flash_cs_l()                PTS &= ~0x80                 
#define eeprom_cs_h()               PORTE |= 0x80  
#define eeprom_cs_l()               PORTE &= ~0x80  

#define spi_low_speed()             SPIBR = 0x02    // BaudRate = 3.125 MHz                
#define spi_high_speed()            SPIBR = 0x00    // BaudRate = 12.5 MHz                

// Instruction Set for serial FLASH (M25P80)
#define FLASH_CMD_WREN              0x06    // Write Enable
#define FLASH_CMD_WRDI              0x04    // Write Disable
#define FLASH_CMD_RDSR              0x05    // Read Status Register
#define FLASH_CMD_WRSR              0x01    // Write Status Register
#define FLASH_CMD_READ              0x03    // Read Data Bytes
#define FLASH_CMD_PP                0x02    // Page Program
#define FLASH_CMD_FAST_READ         0x0B    // Read Data Bytes at Higher Speed
#define FLASH_CMD_SE                0xD8    // Sector Erase
#define FLASH_CMD_BE                0xC7    // Bulk Erase
#define FLASH_CMD_DP                0xB9    // Deep Power-down
#define FLASH_CMD_RES               0xAB    // Release from Deep Power-dowm


//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
LOGS_PTR	  FLA_LogsPtr;
UINT8  	    SpiBuf [FLASH_HEADER_SIZE+1];


//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void eeprom_unlock(void);

void eeprom_read_buffer   (unsigned int addr, unsigned char *buf, int len);
void eeprom_write_buffer  (unsigned int addr, unsigned char *buf, int len);

void  init_serial_memories (void);
void  FLA_Init          (void);

void  FLA_SaveReg   (UINT16 RegIdx, UINT8 DataLen, UINT8* DataSourcePtr);
void  FLA_ReadReg   (UINT16 RegIdx, UINT8 DataLen, UINT8* DataTargetPtr);

void  SPI_TransferMsg    (UINT8 HdrLen, UINT8 DataLen, UINT8* MsgPtr);
void  FLA_WaitFree      (void);


#endif    //  dSPIH
