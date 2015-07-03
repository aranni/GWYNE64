/*********************************************************************
*
* SOURCE FILENAME:	DemoCGI.c
*
* DATE CREATED:		28 June 2002
*
* PROGRAMMER:		Karl Kobel/Tom Parkinson
*
* DESCRIPTION:  	Builds and sends first Configuration screen
*
**********************************************************************/
#include <stdio.h>
#include "datatypes.h"
#include "dynamic_tools.h"
#include "http_server.h"

#include "commdef.h"
#include "global.h"

extern struct http_server_state https [NO_OF_HTTP_SESSIONS];

WORD    TagPolFrec;
BYTE    TagFlags;
WORD    LogParam;
BYTE    sndbuf[450];



/**********************************************************************
*                           DEFINITIONS
**********************************************************************/
const char https_ack_page_OK[] = "HTTP/1.1 200 OK\r\nKeep-Alive: timeout=5, max=100\r\nConnection: Keep-Alive\r\nContent-Type: text/xml\r\n\r\n";
const char https_ack_page_Accepted[] = "HTTP/1.0 202 Accepted\r\n";
//const char https_ack_page[] = "HTTP/1.0 205 Reset Content\r\n";
const char https_ack_page[] = "HTTP/1.0 204 No content\r\n";
const char GoodHTTP204[]   = "HTTP/1.0 204 No Content\r\n\r\n";
const char BadHTTP400[]    = "HTTP/1.0 400 Bad Request\r\n\r\nBad Request";

const char scsHeader[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<setup>\n";
const char scsTrailer[] = "</setup>\n";


/**********************************************************************
*                           EXTERNAL FUNCTION PROTOTYPES
**********************************************************************/
/*
extern signed char Flash_Write_Block(unsigned int *far address_source,\
                        unsigned int *far far_address_destination,\
                        unsigned int count);
extern signed char Flash_Erase_Sector(unsigned int *far far_address);
*/

/**********************************************************************
*                           CODE
**********************************************************************/
static const USR_DYNAMIC_INITIALIZE StringFields[] = 					 
{
//	{"Escenario",           &MuxParam.Escenario,                    WORD_TYPE},
	{"", NULL, WORD_TYPE}								
};

static const USR_DYNAMIC_INITIALIZE AjaxReqFields[] = 					 
{
	{"TagPolFrec",               &TagPolFrec,               WORD_TYPE},
	{"TagPolEnable",             &TagFlags,                 BIT_0_TYPE},
	{"TagResetList",             &TagFlags,                 BIT_1_TYPE},
	{"TagMirrorFrame",           &TagFlags,                 BIT_2_TYPE},
	{"", NULL, WORD_TYPE}								
};

static const USR_DYNAMIC_INITIALIZE LogsReqFields[] = 					 
{
	{"LogReadNext",              &TagFlags,                 BYTE_TYPE},
	{"LogParam",                 &LogParam,                  WORD_TYPE},
	{"", NULL, WORD_TYPE}								
};


static const USR_DYNAMIC_INITIALIZE Socket_01[] = 					 
{
	{"SKO1_STAT",            &(TcpSockSrv[0].status),                 BIT_0_TYPE },	
	{"SKO1_IP_A",            &(TcpSockSrv[0].SlaveIpAddress),         DWORD_HH_TYPE },	
	{"SKO1_IP_B",            &(TcpSockSrv[0].SlaveIpAddress),         DWORD_HL_TYPE },	
	{"SKO1_IP_C",            &(TcpSockSrv[0].SlaveIpAddress),         DWORD_LH_TYPE },	
	{"SKO1_IP_D",            &(TcpSockSrv[0].SlaveIpAddress),         DWORD_LL_TYPE },	
	{"SKO1_PORT",            &(TcpSockSrv[0].SlaveTcpPort),           WORD_TYPE },	
	{"SKO1_MSG_RX",          &(TcpSockSrv[0].MsgRxOk),                DWORD_TYPE },	
//	{"SKO1_NODE",            &(TcpSockSrv[0].SlaveNodeAddress),       WORD_TYPE },	
	{"", NULL, WORD_TYPE}								
};

static const USR_DYNAMIC_INITIALIZE Socket_02[] = 					 
{
	{"SKO1_STAT",            &(TcpSockSrv[1].status),                 BIT_0_TYPE },	
	{"SKO1_IP_A",            &(TcpSockSrv[1].SlaveIpAddress),         DWORD_HH_TYPE },	
	{"SKO1_IP_B",            &(TcpSockSrv[1].SlaveIpAddress),         DWORD_HL_TYPE },	
	{"SKO1_IP_C",            &(TcpSockSrv[1].SlaveIpAddress),         DWORD_LH_TYPE },	
	{"SKO1_IP_D",            &(TcpSockSrv[1].SlaveIpAddress),         DWORD_LL_TYPE },	
	{"SKO1_PORT",            &(TcpSockSrv[1].SlaveTcpPort),           WORD_TYPE },	
	{"SKO1_MSG_RX",          &(TcpSockSrv[1].MsgRxOk),                DWORD_TYPE },	
//	{"SKO1_NODE",            &(TcpSockSrv[1].SlaveNodeAddress),       WORD_TYPE },	
	{"", NULL, WORD_TYPE}								
};

/*
static const USR_DYNAMIC_INITIALIZE Socket_03[] = 					 
{
	{"SKO1_STAT",            &(TcpSockSrv[2].status),                 BIT_0_TYPE },	
	{"SKO1_IP_A",            &(TcpSockSrv[2].SlaveIpAddress),         DWORD_HH_TYPE },	
	{"SKO1_IP_B",            &(TcpSockSrv[2].SlaveIpAddress),         DWORD_HL_TYPE },	
	{"SKO1_IP_C",            &(TcpSockSrv[2].SlaveIpAddress),         DWORD_LH_TYPE },	
	{"SKO1_IP_D",            &(TcpSockSrv[2].SlaveIpAddress),         DWORD_LL_TYPE },	
	{"SKO1_PORT",            &(TcpSockSrv[2].SlaveTcpPort),           WORD_TYPE },	
	{"SKO1_NODE",            &(TcpSockSrv[2].SlaveNodeAddress),       WORD_TYPE },	
	{"", NULL, WORD_TYPE}								
};
*/	

static const USR_DYNAMIC_INITIALIZE BacNode_01[] = 					 
{
	{"BAC_NODE_ADD",      &(BAC_BankMemManager[0].BacNodeAddress),    XWORD_TYPE },	
	{"BAC_OFFSET_INI",    &(BAC_BankMemManager[0].BankOffsetStart),   WORD_TYPE },	
	{"BAC_MODBUS_ADD",    &(BAC_BankMemManager[0].BankModbusAddress), WORD_TYPE },	
	{"BAC_BANK_SIZE",     &(BAC_BankMemManager[0].BankSize),          BYTE_TYPE },	
	{"BAC_BANK_STAT",     &(BAC_BankMemManager[0].BankStatus),        BYTE_TYPE },	
	{"", NULL, WORD_TYPE}								
};

static const USR_DYNAMIC_INITIALIZE BacNode_02[] = 					 
{
	{"BAC_NODE_ADD",      &(BAC_BankMemManager[1].BacNodeAddress),    XWORD_TYPE },	
	{"BAC_OFFSET_INI",    &(BAC_BankMemManager[1].BankOffsetStart),   WORD_TYPE },	
	{"BAC_MODBUS_ADD",    &(BAC_BankMemManager[1].BankModbusAddress), WORD_TYPE },	
	{"BAC_BANK_SIZE",     &(BAC_BankMemManager[1].BankSize),          BYTE_TYPE },	
	{"BAC_BANK_STAT",     &(BAC_BankMemManager[1].BankStatus),        BYTE_TYPE },	
	{"", NULL, WORD_TYPE}								
};

static const USR_DYNAMIC_INITIALIZE BacNode_03[] = 					 
{
	{"BAC_NODE_ADD",      &(BAC_BankMemManager[2].BacNodeAddress),    XWORD_TYPE },	
	{"BAC_OFFSET_INI",    &(BAC_BankMemManager[2].BankOffsetStart),   WORD_TYPE },	
	{"BAC_MODBUS_ADD",    &(BAC_BankMemManager[2].BankModbusAddress), WORD_TYPE },	
	{"BAC_BANK_SIZE",     &(BAC_BankMemManager[2].BankSize),          BYTE_TYPE },	
	{"BAC_BANK_STAT",     &(BAC_BankMemManager[2].BankStatus),        BYTE_TYPE },	
	{"", NULL, WORD_TYPE}								
};

static const USR_DYNAMIC_INITIALIZE BacNode_04[] = 					 
{
	{"BAC_NODE_ADD",      &(BAC_BankMemManager[3].BacNodeAddress),    XWORD_TYPE },	
	{"BAC_OFFSET_INI",    &(BAC_BankMemManager[3].BankOffsetStart),   WORD_TYPE },	
	{"BAC_MODBUS_ADD",    &(BAC_BankMemManager[3].BankModbusAddress), WORD_TYPE },	
	{"BAC_BANK_SIZE",     &(BAC_BankMemManager[3].BankSize),          BYTE_TYPE },	
	{"BAC_BANK_STAT",     &(BAC_BankMemManager[3].BankStatus),        BYTE_TYPE },	
	{"", NULL, WORD_TYPE}								
};

static const USR_DYNAMIC_INITIALIZE BacNode_05[] = 					 
{
	{"BAC_NODE_ADD",      &(BAC_BankMemManager[4].BacNodeAddress),    XWORD_TYPE },	
	{"BAC_OFFSET_INI",    &(BAC_BankMemManager[4].BankOffsetStart),   WORD_TYPE },	
	{"BAC_MODBUS_ADD",    &(BAC_BankMemManager[4].BankModbusAddress), WORD_TYPE },	
	{"BAC_BANK_SIZE",     &(BAC_BankMemManager[4].BankSize),          BYTE_TYPE },	
	{"BAC_BANK_STAT",     &(BAC_BankMemManager[4].BankStatus),        BYTE_TYPE },	
	{"", NULL, WORD_TYPE}								
};

static const USR_DYNAMIC_INITIALIZE BacNode_06[] = 					 
{
	{"BAC_NODE_ADD",      &(BAC_BankMemManager[5].BacNodeAddress),    XWORD_TYPE },	
	{"BAC_OFFSET_INI",    &(BAC_BankMemManager[5].BankOffsetStart),   WORD_TYPE },	
	{"BAC_MODBUS_ADD",    &(BAC_BankMemManager[5].BankModbusAddress), WORD_TYPE },	
	{"BAC_BANK_SIZE",     &(BAC_BankMemManager[5].BankSize),          BYTE_TYPE },	
	{"BAC_BANK_STAT",     &(BAC_BankMemManager[5].BankStatus),        BYTE_TYPE },	
	{"", NULL, WORD_TYPE}								
};

static const USR_DYNAMIC_INITIALIZE BacNode_07[] = 					 
{
	{"BAC_NODE_ADD",      &(BAC_BankMemManager[6].BacNodeAddress),    XWORD_TYPE },	
	{"BAC_OFFSET_INI",    &(BAC_BankMemManager[6].BankOffsetStart),   WORD_TYPE },	
	{"BAC_MODBUS_ADD",    &(BAC_BankMemManager[6].BankModbusAddress), WORD_TYPE },	
	{"BAC_BANK_SIZE",     &(BAC_BankMemManager[6].BankSize),          BYTE_TYPE },	
	{"BAC_BANK_STAT",     &(BAC_BankMemManager[6].BankStatus),        BYTE_TYPE },	
	{"", NULL, WORD_TYPE}											 
};

static const USR_DYNAMIC_INITIALIZE BacNode_08[] = 					 
{
	{"BAC_NODE_ADD",      &(BAC_BankMemManager[7].BacNodeAddress),    XWORD_TYPE },	
	{"BAC_OFFSET_INI",    &(BAC_BankMemManager[7].BankOffsetStart),   WORD_TYPE },	
	{"BAC_MODBUS_ADD",    &(BAC_BankMemManager[7].BankModbusAddress), WORD_TYPE },	
	{"BAC_BANK_SIZE",     &(BAC_BankMemManager[7].BankSize),          BYTE_TYPE },	
	{"BAC_BANK_STAT",     &(BAC_BankMemManager[7].BankStatus),        BYTE_TYPE },	
	{"", NULL, WORD_TYPE}								
};

static const USR_DYNAMIC_INITIALIZE BacNode_09[] = 					 
{
	{"BAC_NODE_ADD",      &(BAC_BankMemManager[8].BacNodeAddress),    XWORD_TYPE },	
	{"BAC_OFFSET_INI",    &(BAC_BankMemManager[8].BankOffsetStart),   WORD_TYPE },	
	{"BAC_MODBUS_ADD",    &(BAC_BankMemManager[8].BankModbusAddress), WORD_TYPE },	
	{"BAC_BANK_SIZE",     &(BAC_BankMemManager[8].BankSize),          BYTE_TYPE },	
	{"BAC_BANK_STAT",     &(BAC_BankMemManager[8].BankStatus),        BYTE_TYPE },	
	{"", NULL, WORD_TYPE}											 
};

static const USR_DYNAMIC_INITIALIZE BacNode_10[] = 					 
{
	{"BAC_NODE_ADD",      &(BAC_BankMemManager[9].BacNodeAddress),    XWORD_TYPE },	
	{"BAC_OFFSET_INI",    &(BAC_BankMemManager[9].BankOffsetStart),   WORD_TYPE },	
	{"BAC_MODBUS_ADD",    &(BAC_BankMemManager[9].BankModbusAddress), WORD_TYPE },	
	{"BAC_BANK_SIZE",     &(BAC_BankMemManager[9].BankSize),          BYTE_TYPE },	
	{"BAC_BANK_STAT",     &(BAC_BankMemManager[9].BankStatus),        BYTE_TYPE },	
	{"", NULL, WORD_TYPE}											 
};


//----------------------------------------------------------------------------------


static const USR_DYNAMIC_INITIALIZE StringStatistic[] = 					 
{
	{"par_tx_err",          &StatisticData.SerialStat.MsgTxErr,     DWORD_TYPE},
	{"par_tx_ok",           &StatisticData.SerialStat.MsgTxOk,      DWORD_TYPE},
	{"par_rx_err",          &StatisticData.SerialStat.MsgRxErr,     DWORD_TYPE},
	{"par_rx_ok",           &StatisticData.SerialStat.MsgRxOk,      DWORD_TYPE},
	{"", NULL, WORD_TYPE}								
};

static const USR_DYNAMIC_INITIALIZE StringVersionInfo[] = 					 
{
	{"par_FW_ver",          &FwVersionData.FW_Version,              STRING_TYPE},
	{"par_FW_date",         &FwVersionData.DateOfCompilation,       STRING_TYPE},
	{"par_FW_time",         &FwVersionData.TimeOfCompilation,       STRING_TYPE},
	{"par_HW_ver",          &(HwVer[0]),                            STRING_TYPE},
	{"par_HW_serial",       &(HwSerial[0]),                         STRING_TYPE},
	{"", NULL, WORD_TYPE}								
};

static const USR_DYNAMIC_INITIALIZE BAC_ConfigData[] = 					 
{
	{"TagPolFrec",               &BAC_PollingPeriod,        WORD_TYPE},
	{"TagPolEnable",             &BAC_CtrlFlags,            BIT_0_TYPE},
	{"TagResetList",             &BAC_CtrlFlags,            BIT_1_TYPE},
	{"TagMirrorFrame",           &BAC_CtrlFlags,            BIT_2_TYPE},
	{"", NULL, WORD_TYPE}								
};

					 
/*********************************************************************
*
* FUNCTION:			BuildMenuIndexes
*
* ARGUMENTS:		fat_index   Index of the file system table entry
*                   wLen        Lenght of the request string from the web page
*                   bSession    Http session
*
* RETURNS:			None
*
* DESCRIPTION:		Decodes the CGI tag/value pairs and updates the variables
*              
* RESTRICTIONS:		
*
*********************************************************************/
void DemoCGI(BYTE fat_index, WORD wLen, BYTE bSession)
{
    USR_DYNAMIC_INITIALIZE* pDynamicTable;
    UINT16 ValuePairEnd;
    char szName[32];
    char szValue[32];

    fat_index++;		//not used - stops compiler warning
    do
    {
        ValuePairEnd = GetNameValuePair(wLen, szName, szValue);
        pDynamicTable = FindTableName(szName, (USR_DYNAMIC_INITIALIZE *)(StringFields));
        if(pDynamicTable != NULL)
        {
            SaveValue(szValue, pDynamicTable);
        }
    }while(ValuePairEnd);
    

	  https[bSession].fstart    = (UINT32)https_ack_page; //(unsigned char*)https_ack_page;
	  https[bSession].fstart   |= (UINT32)0xF0000000;
  	https[bSession].funacked  = 0;
	  https[bSession].flen      = strlen((char *)https_ack_page);
	  https[bSession].fpoint    = 0;

//    PutEventMsg (EVT_VAR_CMDSTART, PROC_DNP, OS_NOT_PROC, 0x00);
	
}


/*********************************************************************
*
* FUNCTION:			DemoAjax
*
* ARGUMENTS:		fat_index   Index of the file system table entry
*                   wLen        Lenght of the request string from the web page
*                   bSession    Http session
*
* RETURNS:			None
*
* DESCRIPTION:		Builds the XML tags and send it to the web page to populate the page
*              
* RESTRICTIONS:		
*
*********************************************************************/
void DemoAjax(BYTE fat_index, WORD wLen, BYTE bSession)
{

	BYTE tmpbuf[100];
	WORD i = 0;
	USR_DYNAMIC_INITIALIZE* StringFieldPtr;
  UINT16 ValuePairEnd;
  char szName[32];
  char szValue[32];
  BYTE  bTmpVal;
	
//  fat_index++;  //not used - stops compiler warning
//  wLen++;				//not used - stops compiler warning

  if (fat_index<=1)      // 1 - Lectura ó escritura de Tags
  {
      TagFlags= 0x00;
      do
      {
          ValuePairEnd = GetNameValuePair(wLen, szName, szValue);
          StringFieldPtr = FindTableName(szName, (USR_DYNAMIC_INITIALIZE *)(AjaxReqFields));
          if(StringFieldPtr != NULL)
          {
              SaveValue(szValue, StringFieldPtr);
          }
          wLen -= ValuePairEnd;
      }while(ValuePairEnd);
  }
  
  if (fat_index==0)    // FatIndex = 0 - Escritura
  {
    BAC_PollingPeriod = TagPolFrec;
    BAC_CtrlFlags &= 0xF0;
    TagFlags  &= 0x0F;
    BAC_CtrlFlags |= TagFlags;

    if (BAC_CtrlFlags & BAC_MASK_RESET_LIST) 
    {
      BAC_CtrlFlags &= ~BAC_MASK_RESET_LIST;
      TagFlags &= ~BAC_MASK_RESET_LIST;
      BAC_ResetBanks();
    }
    
    if (BAC_CtrlFlags & BAC_MASK_POL_ENABLE)
      bTmpVal = 1;
    else
      bTmpVal = 0;
   	eeprom_write_buffer(EE_ADDR_NODE_SCIA, &bTmpVal,1);           

    if (BAC_CtrlFlags & BAC_MASK_MIRR_FRAME)
      bTmpVal = 1;
    else
      bTmpVal = 0;
   	eeprom_write_buffer(EE_ADDR_NODE_SCIB, &bTmpVal,1);           
  
    eeprom_write_buffer(EE_ADDR_INV_POLL, 
 	                      (UINT8 *)&BAC_PollingPeriod,2); 
 	                      
    BAC_ReInit(); 	                      
 	                      
   	(void)strcpy((char*)sndbuf, https_ack_page);	

  	// ABR: agregar verificación de longitud máxima.. (ToDo)
  	https[bSession].fstart   = (UINT32)sndbuf;
  	https[bSession].fstart   |= (UINT32)0xF0000000;
  	https[bSession].flen     = strlen((char*)sndbuf);
  	https[bSession].fpoint   = 0;
  	https[bSession].funacked = 0;
  	
  	return;  

  }

// Si es mayor que 1, entonces es lectura de tabla de memoria

 	(void)strcpy((char*)sndbuf, scsHeader);
 	
 	switch (fat_index){
 	  case 2:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)StringStatistic;
 	    break;
 
 	  case 3:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)StringVersionInfo;
 	    break;

 	  case 4:					// DataObs.xml =  Campo de observaciones
      (void)strcat((char*)sndbuf, (char*)"<par_HW_obs>");
     	eeprom_read_buffer(EE_ADDR_VER_TEXT,(UINT8*)tmpbuf,30);
      (void)strcat((char*)sndbuf, (char*)tmpbuf);
      (void)strcat((char*)sndbuf, (char*)"</par_HW_obs>\n");
      
 	    (void)strcat((char*)sndbuf, scsTrailer);
	    https[bSession].fstart   = (UINT32)sndbuf;
	    https[bSession].fstart   |= (UINT32)0xF0000000;
	    https[bSession].flen     = strlen((char*)sndbuf);
	    https[bSession].fpoint   = 0;
	    https[bSession].funacked = 0;
			return;
 	    break; 	    

 	  case 5:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)Socket_01;
 	    break;

 	  case 6:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)Socket_02;
 	    break;

 	  case 7:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)BacNode_01;
 	    break;

 	  case 8:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)BacNode_02;
 	    break;

 	  case 9:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)BacNode_03;
 	    break;

 	  case 10:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)BacNode_04;
 	    break;

 	  case 11:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)BacNode_05;
 	    break;

 	  case 12:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)BacNode_06;
 	    break;

 	  case 13:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)BacNode_07;
 	    break;

 	  case 14:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)BacNode_08;
 	    break;

 	  case 15:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)BacNode_09;
 	    break;

 	  case 16:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)BacNode_10;
 	    break;

 	  case 17:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)BAC_ConfigData;
 	    break;

 	  default:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)StringFields;
 	    break;
 	    
 	}

	while (StringFieldPtr[i].fieldDataSource != NULL)
	{
		LoadValue((char*)tmpbuf, sizeof(tmpbuf),(USR_DYNAMIC_INITIALIZE *)&StringFieldPtr[i]);
        (void)strcat((char*)sndbuf, (char*)tmpbuf);
		i++;
	}

	(void)strcat((char*)sndbuf, scsTrailer);
	// ABR: agregar verificación de longitud máxima.. (ToDo)
	https[bSession].fstart   = (UINT32)sndbuf;
	https[bSession].fstart   |= (UINT32)0xF0000000;
	https[bSession].flen     = strlen((char*)sndbuf);
	https[bSession].fpoint   = 0;
	https[bSession].funacked = 0;

}
					 


void LogsAjax(BYTE fat_index, WORD wLen, BYTE bSession) 
{
  
//  static BYTE    sndbuf[450];
	BYTE tmpbuf[100];
	WORD idx = 0;
	USR_DYNAMIC_INITIALIZE* StringFieldPtr;
  UINT16 ValuePairEnd;
  char szName[32];
  char szValue[32];
  
  UINT8*  pReadLogBuffer;
  UINT8   bLogToken; 
  UINT16  wLogIdx;
  
	
  fat_index++;  //not used - stops compiler warning
//  wLen++;				//not used - stops compiler warning

    LogParam = 0;
    TagFlags = 0;
    do
    {
        ValuePairEnd = GetNameValuePair(wLen, szName, szValue);
        StringFieldPtr = FindTableName(szName, (USR_DYNAMIC_INITIALIZE *)(LogsReqFields));
        if(StringFieldPtr != NULL)
        {
            SaveValue(szValue, StringFieldPtr);
        }
        wLen -= ValuePairEnd;
    }while(ValuePairEnd);

    if (TagFlags == 1) 
    {
  
        if ( ERROR == MemGetBuffer ( (void**)(&pReadLogBuffer), &bLogToken)) 
        {
          	(void)strcpy((char*)sndbuf, https_ack_page_OK);	
           	(void)strcat((char*)sndbuf, scsHeader);
            (void)sprintf((char*)tmpbuf, "<%s>%s</%s>\n", 
        	                    (char*)"LogText", 
        	                    (char*)"ERR: GetBuffer ERROR", 
        	                    (char*)"LogText");
            (void)strcat((char*)sndbuf, (char*)tmpbuf);

        	  (void)sprintf((char*)tmpbuf, "<%s>%d</%s>\n", 
        	                    (char*)"OpCode", 
        	                    0xFF, 
        	                    (char*)"OpCode");
            (void)strcat((char*)sndbuf, (char*)tmpbuf);	 

          	(void)strcat((char*)sndbuf, scsTrailer);
          	https[bSession].fstart   = (UINT32)sndbuf;
          	https[bSession].fstart   |= (UINT32)0xF0000000;
          	https[bSession].flen     = strlen((char*)sndbuf);
          	https[bSession].fpoint   = 0;
          	https[bSession].funacked = 0;
          	return;  
        }
    
        wLogIdx = FLA_ReadLog(pReadLogBuffer);        
      	if ( ERROR != wLogIdx ) 
       	{
        	  
          	(void)strcpy((char*)sndbuf, https_ack_page_OK);	
           	(void)strcat((char*)sndbuf, scsHeader);

        	  (void)sprintf((char*)tmpbuf, "<%s>%02x/%02x/%02x - %02x:%02x:%02x</%s>\n", 
        	                    (char*)"LogDate", 
                        			((LOG_FORMAT*)pReadLogBuffer)->LogDateTime.Date,
                        			((LOG_FORMAT*)pReadLogBuffer)->LogDateTime.Month,
                        			((LOG_FORMAT*)pReadLogBuffer)->LogDateTime.Year,
                        			((LOG_FORMAT*)pReadLogBuffer)->LogDateTime.Hour,
                        			((LOG_FORMAT*)pReadLogBuffer)->LogDateTime.Minute,
                        			((LOG_FORMAT*)pReadLogBuffer)->LogDateTime.Second,
        	                    (char*)"LogDate");
            (void)strcat((char*)sndbuf, (char*)tmpbuf);
            
        	  (void)sprintf((char*)tmpbuf, "<%s>%d</%s>\n", 
        	                    (char*)"LogId", 
        	                    ((LOG_FORMAT*)pReadLogBuffer)->LogId, 
        	                    (char*)"LogId");
            (void)strcat((char*)sndbuf, (char*)tmpbuf);

        	  (void)sprintf((char*)tmpbuf, "<%s>%d</%s>\n", 
        	                    (char*)"LogParam", 
        	                    ((LOG_FORMAT*)pReadLogBuffer)->ParamVal, 
        	                    (char*)"LogParam");
            (void)strcat((char*)sndbuf, (char*)tmpbuf);	 
/*
        	  (void)sprintf((char*)tmpbuf, "<%s>%d</%s>\n", 
        	                    (char*)"LogIndex", 
        	                    wLogIdx, 
        	                    (char*)"LogIndex");
            (void)strcat((char*)sndbuf, (char*)tmpbuf);            
*/         
        	  (void)sprintf((char*)tmpbuf, "<%s>%d</%s>\n", 
        	                    (char*)"OpCode", 
        	                    0x01, 
        	                    (char*)"OpCode");
            (void)strcat((char*)sndbuf, (char*)tmpbuf);	 
            
//            pLogMsg->Data[50] = '\0';
         
        	  (void)sprintf((char*)tmpbuf, "<%s>%s</%s>\n", 
        	                    (char*)"LogText", 
        	                    (char*)&(((LOG_FORMAT*)pReadLogBuffer)->Data[0]), 
//        	                    (char*)pReadLogBuffer,
        	                    (char*)"LogText");
            (void)strcat((char*)sndbuf, (char*)tmpbuf);
                      
            (void)MemFreeBuffer (bLogToken);

       	}
       	else 
       	{										    
//          	(void)strcpy((char*)sndbuf, BadHTTP400);
          	(void)strcpy((char*)sndbuf, https_ack_page_OK);	
           	(void)strcat((char*)sndbuf, scsHeader);
            (void)sprintf((char*)tmpbuf, "<%s>%s</%s>\n", 
        	                    (char*)"LogText", 
        	                    (char*)"ERR: ReadLog ERROR", 
        	                    (char*)"LogText");
            (void)strcat((char*)sndbuf, (char*)tmpbuf);

        	  (void)sprintf((char*)tmpbuf, "<%s>%d</%s>\n", 
        	                    (char*)"OpCode", 
        	                    0xFF, 
        	                    (char*)"OpCode");
            (void)strcat((char*)sndbuf, (char*)tmpbuf);	 
            
        }				
        
       	(void)strcat((char*)sndbuf, scsTrailer);
      	// ABR: agregar verificación de longitud máxima.. (ToDo)
      	https[bSession].fstart   = (UINT32)sndbuf;
      	https[bSession].fstart   |= (UINT32)0xF0000000;
      	https[bSession].flen     = strlen((char*)sndbuf);
      	https[bSession].fpoint   = 0;
      	https[bSession].funacked = 0;
      	return;  
    }


  if (TagFlags == 10)
  {
  
    if (LogParam)
      idx = FLA_ResetLogReadCntr(LogParam);
    else
      idx = FLA_ResetLogReadCntr(10);

  	(void)strcpy((char*)sndbuf, https_ack_page_OK);	
   	(void)strcat((char*)sndbuf, scsHeader);
    (void)sprintf((char*)tmpbuf, "<%s>%s</%s>\n", 
	                    (char*)"LogText", 
	                    (char*)"LOGS REWIND OK", 
	                    (char*)"LogText");
    (void)strcat((char*)sndbuf, (char*)tmpbuf);            

	  (void)sprintf((char*)tmpbuf, "<%s>%d</%s>\n", 
	                    (char*)"OpCode", 
	                    idx, 
	                    (char*)"OpCode");
    (void)strcat((char*)sndbuf, (char*)tmpbuf);	 
              
  	(void)strcat((char*)sndbuf, scsTrailer);
    	https[bSession].fstart   = (UINT32)sndbuf;
    	https[bSession].fstart   |= (UINT32)0xF0000000;
    	https[bSession].flen     = strlen((char*)sndbuf);
    	https[bSession].fpoint   = 0;
    	https[bSession].funacked = 0;
    	return;  
  }

  if (TagFlags == 11)
  {
  
     FLA_ResetLogPtr();

  	(void)strcpy((char*)sndbuf, https_ack_page_OK);	
   	(void)strcat((char*)sndbuf, scsHeader);
    (void)sprintf((char*)tmpbuf, "<%s>%s</%s>\n", 
	                    (char*)"LogText", 
	                    (char*)"LOGS RESET OK", 
	                    (char*)"LogText");
    (void)strcat((char*)sndbuf, (char*)tmpbuf);            

	  (void)sprintf((char*)tmpbuf, "<%s>%d</%s>\n", 
	                    (char*)"OpCode", 
	                    0x02, 
	                    (char*)"OpCode");
    (void)strcat((char*)sndbuf, (char*)tmpbuf);	 
              
  	(void)strcat((char*)sndbuf, scsTrailer);
    	https[bSession].fstart   = (UINT32)sndbuf;
    	https[bSession].fstart   |= (UINT32)0xF0000000;
    	https[bSession].flen     = strlen((char*)sndbuf);
    	https[bSession].fpoint   = 0;
    	https[bSession].funacked = 0;
    	return;  
  }

  	(void)strcpy((char*)sndbuf, https_ack_page_OK);	
   	(void)strcat((char*)sndbuf, scsHeader);
    (void)sprintf((char*)tmpbuf, "<%s>%s</%s>\n", 
	                    (char*)"LogText", 
	                    (char*)"ERR:FALTA CtrlFlag", 
	                    (char*)"LogText");
    (void)strcat((char*)sndbuf, (char*)tmpbuf);
              
  	(void)strcat((char*)sndbuf, scsTrailer);
   	https[bSession].fstart   = (UINT32)sndbuf;
   	https[bSession].fstart   |= (UINT32)0xF0000000;
   	https[bSession].flen     = strlen((char*)sndbuf);
   	https[bSession].fpoint   = 0;
   	https[bSession].funacked = 0;
   	return;  
   
}
  		 
