#include <hidef.h>
#include <string.h>

#include "debug.h"
#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"

#include "Commdef.h"
#include "dSPI.h"
#include "Global.h"

#pragma CODE_SEG NON_BANKED        


//*******************************************************************************************
//
//  Lectura y escritura de LOGs de EVENTOS en Flash
//
//*******************************************************************************************
//


//-------------------------------------------------------------------------------------------
//  FLA_SearchStateEvent: Busca y Lee el primer evento con un estado especificado
//-------------------------------------------------------------------------------------------
//
RETCODE FLA_SearchStateEvent (UINT8 LogEventToken)
{
	UINT16	TmpWord;	
	UINT8   bTmpState;
  BYTE*   pLogMsg;
  
  if (MemGetBufferPtr( LogEventToken , (void**)(&pLogMsg) ) != OK ){    
      // ABR: control de errores
      return ERROR;   
  }
																			        
  bTmpState = ((LOG_EVENT_FORMAT*)pLogMsg)->EventLastState; 

	if (FLA_LogsEventPtr.NextToWrite == FLA_LogsEventPtr.FirstToRead ) return ERROR;
	
	TmpWord = FLA_LogsEventPtr.FirstToRead;
	
	while(TmpWord != FLA_LogsEventPtr.NextToWrite) {
	  
      FLA_ReadReg (TmpWord, sizeof(LOG_EVENT_FORMAT), pLogMsg);
      
      //ABR:2012-10-09: provisorio para corregir error de escritura en RegID Rausch
      ((LOG_EVENT_FORMAT*)pLogMsg)->EventId = TmpWord;      
      
      if (((LOG_EVENT_FORMAT*)pLogMsg)->EventLastState == bTmpState ) 
      {
          return OK;
      }
    	TmpWord++;
    	if (TmpWord >= FLASH_MAX_LOG_EVENT_ADDR ) TmpWord = FLASH_MIN_LOG_EVENT_ADDR;
      
	}
	
	return ERROR;
  
}


//-------------------------------------------------------------------------------------------
//  FLA_ChangeStateEvent: cambia el estado de un evento, segun el codigo wEventId
//-------------------------------------------------------------------------------------------
//
RETCODE FLA_ChangeStateEvent (UINT16 wEventId, UINT8 bEventState)
{

  BYTE  pLogMsg[sizeof(LOG_EVENT_FORMAT)];
  
  
  if ( (wEventId < FLASH_MIN_LOG_EVENT_ADDR) || (wEventId > FLASH_MAX_LOG_EVENT_ADDR) )
        return ERROR;
  
  FLA_ReadReg (wEventId, sizeof(LOG_EVENT_FORMAT), pLogMsg);
  
 	IIC_RtcGetDateTime();	
	(void)memcpy( (UINT8*)&(((LOG_EVENT_FORMAT*)pLogMsg)->StateDateTime.Second),
				        (UINT8*)&(StatisticData.CurrentDateTime.Second), sizeof(DATE_TIME) );
				        
  ((LOG_EVENT_FORMAT*)pLogMsg)->EventLastState =  bEventState; 


  FLA_SaveReg (wEventId, sizeof(LOG_EVENT_FORMAT), (UINT8*)pLogMsg);
       
	return OK;
	
}


//-------------------------------------------------------------------------------------------
//  FLA_SaveLogEvent: graba un EVENTO, segun el token del buffer recibido como argumento
//-------------------------------------------------------------------------------------------
//
void FLA_SaveLogEvent (UINT8 LogToken)
{
  BYTE*         pLogMsg;
  
  if (MemGetBufferPtr( LogToken , (void**)(&pLogMsg) ) != OK ){    
    // ABR: control de errores
    return;   
  }
																	
 	IIC_RtcGetDateTime();	
	(void)memcpy( (UINT8*)&(((LOG_EVENT_FORMAT*)pLogMsg)->LogDateTime.Second),
				        (UINT8*)&(StatisticData.CurrentDateTime.Second), sizeof(DATE_TIME) );
				        
	(void)memcpy( (UINT8*)&(((LOG_EVENT_FORMAT*)pLogMsg)->StateDateTime.Second),
				        (UINT8*)&(StatisticData.CurrentDateTime.Second), sizeof(DATE_TIME) );
				        
  ((LOG_EVENT_FORMAT*)pLogMsg)->EventId = FLA_LogsEventPtr.NextToWrite; 
        
  FLA_SaveReg (FLA_LogsEventPtr.NextToWrite, sizeof(LOG_EVENT_FORMAT), (UINT8*)pLogMsg);
       
	FLA_LogsEventPtr.NextToWrite++;
	if (FLA_LogsEventPtr.NextToWrite >= FLASH_MAX_LOG_EVENT_ADDR ) FLA_LogsEventPtr.NextToWrite = FLASH_MIN_LOG_EVENT_ADDR;

	if (FLA_LogsEventPtr.NextToWrite == FLA_LogsEventPtr.FirstToRead ){
	  FLA_LogsEventPtr.FirstToRead++;
		if (FLA_LogsEventPtr.FirstToRead >= FLASH_MAX_LOG_EVENT_ADDR ) FLA_LogsEventPtr.FirstToRead = FLASH_MIN_LOG_EVENT_ADDR;
	}

	IIC_WriteRTCMem (sizeof(FLA_LogsEventPtr) ,NVR_ADDR_LOGS_ENVENT_PTR, (UINT8*)(&FLA_LogsEventPtr));

  (void)MemFreeBuffer (LogToken);

	return;
}


//-------------------------------------------------------------------------------------------
//  FLA_SaveLogEventFixed: graba un EVENTO predefinido, segun el codigo dLogId
//-------------------------------------------------------------------------------------------
//
void FLA_SaveLogEventFixed (UINT8 bEventCode, UINT8 bSourceProc)
{

  BYTE         pLogMsg[sizeof(LOG_EVENT_FORMAT)];
  
 	IIC_RtcGetDateTime();	
	(void)memcpy( (UINT8*)&(((LOG_EVENT_FORMAT*)pLogMsg)->LogDateTime.Second),
				        (UINT8*)&(StatisticData.CurrentDateTime.Second), sizeof(DATE_TIME) );
  ((LOG_EVENT_FORMAT*)pLogMsg)->EventSource =  bSourceProc; 
  ((LOG_EVENT_FORMAT*)pLogMsg)->EventId = FLA_LogsEventPtr.NextToWrite; 
  ((LOG_EVENT_FORMAT*)pLogMsg)->EventCode =  bEventCode; 

  if  (bEventCode < EVENT_ID_MAX)
	  (void)strcpy((INT8*)&(((LOG_EVENT_FORMAT*)pLogMsg)->Data[0]), (INT8*)EVENT_TextId[bEventCode]);	

  FLA_SaveReg (FLA_LogsEventPtr.NextToWrite, sizeof(LOG_EVENT_FORMAT), (UINT8*)pLogMsg);
       
	FLA_LogsEventPtr.NextToWrite++;
	if (FLA_LogsEventPtr.NextToWrite >= FLASH_MAX_LOG_EVENT_ADDR ) FLA_LogsEventPtr.NextToWrite = FLASH_MIN_LOG_EVENT_ADDR;

	if (FLA_LogsEventPtr.NextToWrite == FLA_LogsEventPtr.FirstToRead ){
	  FLA_LogsEventPtr.FirstToRead++;
		if (FLA_LogsEventPtr.FirstToRead >= FLASH_MAX_LOG_EVENT_ADDR ) FLA_LogsEventPtr.FirstToRead = FLASH_MIN_LOG_EVENT_ADDR;
	}

	IIC_WriteRTCMem (sizeof(FLA_LogsEventPtr) ,NVR_ADDR_LOGS_ENVENT_PTR, (UINT8*)(&FLA_LogsEventPtr));

	return;
}

//-------------------------------------------------------------------------------------------
//  FLA_ReadLogEvent: Lee el proximo evento segun el indice NextToRead
//-------------------------------------------------------------------------------------------
//
UINT16 FLA_ReadLogEvent (UINT8* pReadLogBuffer)
{
	UINT16	TmpWord;

	if (FLA_LogsEventPtr.NextToWrite == FLA_LogsEventPtr.NextToRead ) return ERROR;
	
  FLA_ReadReg (FLA_LogsEventPtr.NextToRead, sizeof(LOG_EVENT_FORMAT), pReadLogBuffer);
  
	((LOG_EVENT_FORMAT*)pReadLogBuffer)->Data[LOG_DATA_SIZE-1] = '\0';
		 
  TmpWord = FLA_LogsEventPtr.NextToRead;
	FLA_LogsEventPtr.NextToRead++;
	if (FLA_LogsEventPtr.NextToRead >= FLASH_MAX_LOG_EVENT_ADDR ) FLA_LogsEventPtr.NextToRead = FLASH_MIN_LOG_EVENT_ADDR;

	return TmpWord;
}


//-------------------------------------------------------------------------------------------
//  FLA_ResetLogEventReadCntr: retrocede el indice NextToRead wBackIdx posiciones.desde el 
//														 ultimo registro de evento ingresado
//-------------------------------------------------------------------------------------------
//
UINT16 FLA_ResetLogEventReadCntr (UINT16 wBackIdx)
{
  WORD wTmpCntr;
  // FLA_LogsEventPtr.NextToRead = FLA_LogsEventPtr.FirstToRead; 

  wTmpCntr=0;
  if (FLA_LogsEventPtr.FirstToRead == FLA_LogsEventPtr.NextToWrite ) return 0;
  
  FLA_LogsEventPtr.NextToRead = FLA_LogsEventPtr.NextToWrite; 
  FLA_LogsEventPtr.NextToRead--; 
  if (FLA_LogsEventPtr.NextToRead < FLASH_MIN_LOG_EVENT_ADDR ) FLA_LogsEventPtr.NextToRead = FLASH_MAX_LOG_EVENT_ADDR;
  if (FLA_LogsEventPtr.NextToRead == FLA_LogsEventPtr.FirstToRead ) return 1;
  
  for (wTmpCntr=1; wTmpCntr<wBackIdx ; wTmpCntr++) 
  {
  	  if (FLA_LogsEventPtr.NextToRead == FLA_LogsEventPtr.FirstToRead ) break;
      FLA_LogsEventPtr.NextToRead--; 
  	  if (FLA_LogsEventPtr.NextToRead < FLASH_MIN_LOG_EVENT_ADDR ) FLA_LogsEventPtr.NextToRead = FLASH_MAX_LOG_EVENT_ADDR;
  }
  
  return wTmpCntr;
  
}


//-------------------------------------------------------------------------------------------
//  FLA_ResetLogEventPtr: Inicializa todos los indices 
//
//  -> YA NO SE PODRAN CONSULTAR LOS REGISTROS QUE ESTABAN GRABADOS !!!!
//
//-------------------------------------------------------------------------------------------
//
void FLA_ResetLogEventPtr (void)
{
    UINT32  TmpVarFirma;

    FLA_LogsEventPtr.NextToWrite = FLASH_MIN_LOG_EVENT_ADDR;
    FLA_LogsEventPtr.NextToRead  = FLASH_MIN_LOG_EVENT_ADDR;
    FLA_LogsEventPtr.FirstToRead = FLASH_MIN_LOG_EVENT_ADDR;
    IIC_WriteRTCMem (sizeof(FLA_LogsEventPtr) ,NVR_ADDR_LOGS_ENVENT_PTR, (UINT8*)(&FLA_LogsEventPtr));
    TmpVarFirma = NVR_VAL_FIRMA;
    IIC_WriteRTCMem (4 ,NVR_ADDR_FIRMA, (UINT8*)(&TmpVarFirma));
}


//#################################################################################################################
//#################################################################################################################
//#################################################################################################################


//*******************************************************************************************
//
//  Lectura y escritura de LOGs de debug en Flash
//
//*******************************************************************************************
//

//-------------------------------------------------------------------------------------------
//  FLA_SaveLog: graba un LOG de debug, segun el token del buffer recibido como argumento
//-------------------------------------------------------------------------------------------
//
void FLA_SaveLog (UINT8 LogToken)
{
//	UINT16	      TmpWord;
  BYTE*         pLogMsg;
  
  if (MemGetBufferPtr( LogToken , (void**)(&pLogMsg) ) != OK ){    
    // ABR: control de errores
    return;   
  }

 	IIC_RtcGetDateTime();	
	(void)memcpy( (UINT8*)&(((LOG_FORMAT*)pLogMsg)->LogDateTime.Second),
				        (UINT8*)&(StatisticData.CurrentDateTime.Second), sizeof(DATE_TIME) );
  ((LOG_FORMAT*)pLogMsg)->LogId =  LOG_ID_TCP;
  ((LOG_FORMAT*)pLogMsg)->ParamVal = FLA_LogsPtr.NextToWrite; 
       
       
  FLA_SaveReg (FLA_LogsPtr.NextToWrite, sizeof(LOG_FORMAT), (UINT8*)pLogMsg);
       
	FLA_LogsPtr.NextToWrite++;
	if (FLA_LogsPtr.NextToWrite >= FLASH_MAX_LOGADDR ) FLA_LogsPtr.NextToWrite = FLASH_MIN_LOGADDR;

	if (FLA_LogsPtr.NextToWrite == FLA_LogsPtr.FirstToRead ){
	  FLA_LogsPtr.FirstToRead++;
		if (FLA_LogsPtr.FirstToRead >= FLASH_MAX_LOGADDR ) FLA_LogsPtr.FirstToRead = FLASH_MIN_LOGADDR;
	}

	IIC_WriteRTCMem (sizeof(FLA_LogsPtr) ,NVR_ADDR_LOGSPTR, (UINT8*)(&FLA_LogsPtr));

  (void)MemFreeBuffer (LogToken);

	return;
}


//-------------------------------------------------------------------------------------------
//  FLA_SaveLogFixed: graba un LOG de debug predefinido, segun el codigo dLogId
//-------------------------------------------------------------------------------------------
//
void FLA_SaveLogFixed (UINT8 dLogId, UINT8 bSourceProc)
{

  BYTE         pLogMsg[sizeof(LOG_FORMAT)];
  
 	IIC_RtcGetDateTime();	
	(void)memcpy( (UINT8*)&(((LOG_FORMAT*)pLogMsg)->LogDateTime.Second),
				        (UINT8*)&(StatisticData.CurrentDateTime.Second), sizeof(DATE_TIME) );
  ((LOG_FORMAT*)pLogMsg)->LogId =  bSourceProc; // dLogId;
  ((LOG_FORMAT*)pLogMsg)->ParamVal = FLA_LogsPtr.NextToWrite; 

  if  (dLogId < LOG_ID_MAX)
	  (void)strcpy((INT8*)&(((LOG_FORMAT*)pLogMsg)->Data[0]), (INT8*)LOG_TextId[dLogId]);	

  FLA_SaveReg (FLA_LogsPtr.NextToWrite, sizeof(LOG_FORMAT), (UINT8*)pLogMsg);
       
	FLA_LogsPtr.NextToWrite++;
	if (FLA_LogsPtr.NextToWrite >= FLASH_MAX_LOGADDR ) FLA_LogsPtr.NextToWrite = FLASH_MIN_LOGADDR;

	if (FLA_LogsPtr.NextToWrite == FLA_LogsPtr.FirstToRead ){
	  FLA_LogsPtr.FirstToRead++;
		if (FLA_LogsPtr.FirstToRead >= FLASH_MAX_LOGADDR ) FLA_LogsPtr.FirstToRead = FLASH_MIN_LOGADDR;
	}

	IIC_WriteRTCMem (sizeof(FLA_LogsPtr) ,NVR_ADDR_LOGSPTR, (UINT8*)(&FLA_LogsPtr));

	return;
}


//-------------------------------------------------------------------------------------------
//  FLA_ReadLog: Lee el proximo LOG segun el indice NextToRead
//-------------------------------------------------------------------------------------------
//
UINT16 FLA_ReadLog (UINT8* pReadLogBuffer)
{
	UINT16	TmpWord;

	if (FLA_LogsPtr.NextToWrite == FLA_LogsPtr.NextToRead ) return ERROR;
	
  FLA_ReadReg (FLA_LogsPtr.NextToRead, sizeof(LOG_FORMAT), pReadLogBuffer);
  
	((LOG_FORMAT*)pReadLogBuffer)->Data[LOG_DATA_SIZE-1] = '\0';
		 
  TmpWord = FLA_LogsPtr.NextToRead;
	FLA_LogsPtr.NextToRead++;
	if (FLA_LogsPtr.NextToRead >= FLASH_MAX_LOGADDR ) FLA_LogsPtr.NextToRead = FLASH_MIN_LOGADDR;

	return TmpWord;
}


//-------------------------------------------------------------------------------------------
//  FLA_ResetLogReadCntr: retrocede el indice NextToRead wBackIdx posiciones.desde el 
//												ultimo registro de evento ingresado
//-------------------------------------------------------------------------------------------
//
UINT16 FLA_ResetLogReadCntr (UINT16 wBackIdx)
{
  WORD wTmpCntr;
  // FLA_LogsPtr.NextToRead = FLA_LogsPtr.FirstToRead; 

  wTmpCntr=0;
  if (FLA_LogsPtr.FirstToRead == FLA_LogsPtr.NextToWrite ) return 0;
  
  FLA_LogsPtr.NextToRead = FLA_LogsPtr.NextToWrite; 
  FLA_LogsPtr.NextToRead--; 
  if (FLA_LogsPtr.NextToRead < FLASH_MIN_LOGADDR ) FLA_LogsPtr.NextToRead = FLASH_MAX_LOGADDR;
  if (FLA_LogsPtr.NextToRead == FLA_LogsPtr.FirstToRead ) return 1;
  
  for (wTmpCntr=1; wTmpCntr<wBackIdx ; wTmpCntr++) 
  {
  	  if (FLA_LogsPtr.NextToRead == FLA_LogsPtr.FirstToRead ) break;
      FLA_LogsPtr.NextToRead--; 
  	  if (FLA_LogsPtr.NextToRead < FLASH_MIN_LOGADDR ) FLA_LogsPtr.NextToRead = FLASH_MAX_LOGADDR;
  }
  
  return wTmpCntr;
  
}


//-------------------------------------------------------------------------------------------
//  FLA_ResetLogPtr: Inicializa todos los indices 
//
//  -> YA NO SE PODRAN CONSULTAR LOS REGISTROS DE LOGS QUE ESTABAN GRABADOS !!!!
//
//-------------------------------------------------------------------------------------------
//
void FLA_ResetLogPtr (void)
{
    UINT32  TmpVarFirma;

    FLA_LogsPtr.NextToWrite = FLASH_MIN_LOGADDR;
    FLA_LogsPtr.NextToRead  = FLASH_MIN_LOGADDR;
    FLA_LogsPtr.FirstToRead = FLASH_MIN_LOGADDR;
    IIC_WriteRTCMem (sizeof(FLA_LogsPtr) ,NVR_ADDR_LOGSPTR, (UINT8*)(&FLA_LogsPtr));
    TmpVarFirma = NVR_VAL_FIRMA;
    IIC_WriteRTCMem (4 ,NVR_ADDR_FIRMA, (UINT8*)(&TmpVarFirma));
}



//#################################################################################################################
//#################################################################################################################
//#################################################################################################################



//*******************************************************************************************
//
// Lectura y escritura de datos en FLASH (genérica, para datos de config)
//
//*******************************************************************************************
//

// ---------------------------------------------------------------------------
//  FLA_SaveReg:
// ---------------------------------------------------------------------------
//
void FLA_SaveReg (UINT16 RegIdx, UINT16 DataLen, UINT8* DataSourcePtr)
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


//-------------------------------------------------------------------------------------------
//  FLA_ReadReg: 
//-------------------------------------------------------------------------------------------
//
void FLA_ReadReg (UINT16 RegIdx, UINT16 DataLen, UINT8* DataTargetPtr)
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


//-------------------------------------------------------------------------------------------
//  FLA_ReadBuff: 
//-------------------------------------------------------------------------------------------
//
void FLA_ReadBuff (UINT32 Addr, UINT16 DataLen, UINT8* DataTargetPtr)
{
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
  
	return;
}



//-------------------------------------------------------------------------------------------
//  FLA_ReadBlock:
//-------------------------------------------------------------------------------------------
//
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



//#################################################################################################################
//#################################################################################################################
//#################################################################################################################




//*******************************************************************************************
//
// Funciones Auxiliares
//
//*******************************************************************************************
//
//-----------------------------------------------------------------------------
// flash_spi : transfiere un BYTE por SPI
//-----------------------------------------------------------------------------
//
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
void SPI_TransferMsg ( UINT8 HdrLen, UINT16 DataLen, UINT8* MsgPtr ) 
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





//#################################################################################################################
//#################################################################################################################
//#################################################################################################################



//*******************************************************************************************
//
//  INICIALIZACIONES SPI
//
//*******************************************************************************************
//
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
// Serial FLASH Init
// ---------------------------------------------------------------------------
//
void FLA_Init(void)
{
  
  UINT32  TmpVarFirma;

  IIC_ReadRTCMem (4 ,NVR_ADDR_FIRMA, (UINT8*)(&TmpVarFirma));
  
  if ( TmpVarFirma != NVR_VAL_FIRMA ){
  
    FLA_LogsPtr.NextToWrite = FLASH_MIN_LOGADDR;
    FLA_LogsPtr.NextToRead  = FLASH_MIN_LOGADDR;
    FLA_LogsPtr.FirstToRead = FLASH_MIN_LOGADDR;
    IIC_WriteRTCMem (sizeof(FLA_LogsPtr) ,NVR_ADDR_LOGSPTR, (UINT8*)(&FLA_LogsPtr));
    
    FLA_LogsEventPtr.NextToWrite = FLASH_MIN_LOG_EVENT_ADDR;
    FLA_LogsEventPtr.NextToRead  = FLASH_MIN_LOG_EVENT_ADDR;
    FLA_LogsEventPtr.FirstToRead = FLASH_MIN_LOG_EVENT_ADDR;
    IIC_WriteRTCMem (sizeof(FLA_LogsEventPtr) ,NVR_ADDR_LOGS_ENVENT_PTR, (UINT8*)(&FLA_LogsEventPtr));
    
    TmpVarFirma = NVR_VAL_FIRMA;
    IIC_WriteRTCMem (4 ,NVR_ADDR_FIRMA, (UINT8*)(&TmpVarFirma));
  } 
  else{
    IIC_ReadRTCMem (sizeof(FLA_LogsPtr) ,NVR_ADDR_LOGSPTR, (UINT8*)(&FLA_LogsPtr));    
    IIC_ReadRTCMem (sizeof(FLA_LogsEventPtr) ,NVR_ADDR_LOGS_ENVENT_PTR, (UINT8*)(&FLA_LogsEventPtr));
    
/*
	  (void)sprintf((char *)udp_cli_buf,"logs: %d, %d, %d - %d, %d, %d \n",
	                        (UINT16)FLA_LogsEventPtr.FirstToRead,
	                				(UINT16)FLA_LogsEventPtr.NextToRead,
	                				(UINT16)FLA_LogsEventPtr.NextToWrite,
	                				(UINT16)FLA_LogsPtr.FirstToRead,
	                			  (UINT16)FLA_LogsPtr.NextToRead,
                          (UINT16)FLA_LogsPtr.NextToWrite );
	                
    udp_cli_data_to_send = TRUE;
*/    
    
    if ((FLA_LogsEventPtr.NextToWrite < FLASH_MIN_LOG_EVENT_ADDR) || 
        (FLA_LogsEventPtr.NextToWrite > FLASH_MAX_LOG_EVENT_ADDR) ||
        (FLA_LogsEventPtr.NextToRead  < FLASH_MIN_LOG_EVENT_ADDR) || 
        (FLA_LogsEventPtr.NextToRead  > FLASH_MAX_LOG_EVENT_ADDR) ||
        (FLA_LogsEventPtr.FirstToRead < FLASH_MIN_LOG_EVENT_ADDR) || 
        (FLA_LogsEventPtr.FirstToRead > FLASH_MAX_LOG_EVENT_ADDR ))
    {
      FLA_LogsEventPtr.NextToWrite = FLASH_MIN_LOG_EVENT_ADDR;
      FLA_LogsEventPtr.NextToRead  = FLASH_MIN_LOG_EVENT_ADDR;
      FLA_LogsEventPtr.FirstToRead = FLASH_MIN_LOG_EVENT_ADDR;
      IIC_WriteRTCMem (sizeof(FLA_LogsEventPtr) ,NVR_ADDR_LOGS_ENVENT_PTR, (UINT8*)(&FLA_LogsEventPtr));
    }
  
    if ((FLA_LogsPtr.NextToWrite < FLASH_MIN_LOGADDR) || 
        (FLA_LogsPtr.NextToWrite > FLASH_MAX_LOGADDR) ||
        (FLA_LogsPtr.NextToRead  < FLASH_MIN_LOGADDR) || 
        (FLA_LogsPtr.NextToRead  > FLASH_MAX_LOGADDR) ||
        (FLA_LogsPtr.FirstToRead < FLASH_MIN_LOGADDR) || 
        (FLA_LogsPtr.FirstToRead > FLASH_MAX_LOGADDR ))
    {
      FLA_LogsPtr.NextToWrite = FLASH_MIN_LOGADDR;
      FLA_LogsPtr.NextToRead  = FLASH_MIN_LOGADDR;
      FLA_LogsPtr.FirstToRead = FLASH_MIN_LOGADDR;
      IIC_WriteRTCMem (sizeof(FLA_LogsPtr) ,NVR_ADDR_LOGSPTR, (UINT8*)(&FLA_LogsPtr));
    }
  
  }
  return; 
}




//#################################################################################################################
//#################################################################################################################
//#################################################################################################################




//*******************************************************************************************
//
//  OTRAS FUNCIONES SPI
//
//*******************************************************************************************
//
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




//#################################################################################################################
//#################################################################################################################
//#################################################################################################################




//*******************************************************************************************
//
//  Serial EEPROM functions (128 bytes)
//
//*******************************************************************************************
//
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


#pragma CODE_SEG DEFAULT
