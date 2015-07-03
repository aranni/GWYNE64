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

/**********************************************************************
*                           DEFINITIONS
**********************************************************************/

//const char https_ack_page[] = "HTTP/1.0 202 Accepted\r\n";
//const char https_ack_page[] = "HTTP/1.0 200 Ok\r\n";
//const char https_ack_page[] = "HTTP/1.0 205 Reset Content\r\n";
const char https_ack_page[] = "HTTP/1.0 204 No content\r\n";
const UINT8 GoodHTTP204[]   = "HTTP/1.0 204 No Content\r\n\r\n";
const UINT8 BadHTTP400[]    = "HTTP/1.0 400 Bad Request\r\n\r\nBad Request";

const char scsHeader[] = "<?xml version=\"1.0\"?>\n<preferences>\n<setup>\n";
const char scsTrailer[] = "</setup>\n</preferences>";

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

#ifdef   DNP_MASTER_MODE

static const USR_DYNAMIC_INITIALIZE Socket_01[] = 					 
{
	{"SKO1_STAT",            &(TcpSockSrv[0].status),                 BIT_0_TYPE },	
	{"SKO1_IP_A",            &(TcpSockSrv[0].SlaveIpAddress),         DWORD_HH_TYPE },	
	{"SKO1_IP_B",            &(TcpSockSrv[0].SlaveIpAddress),         DWORD_HL_TYPE },	
	{"SKO1_IP_C",            &(TcpSockSrv[0].SlaveIpAddress),         DWORD_LH_TYPE },	
	{"SKO1_IP_D",            &(TcpSockSrv[0].SlaveIpAddress),         DWORD_LL_TYPE },	
	{"SKO1_PORT",            &(TcpSockSrv[0].SlaveTcpPort),           WORD_TYPE },	
	{"SKO1_NODE",            &(TcpSockSrv[0].SlaveNodeAddress),       WORD_TYPE },	
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
	{"SKO1_NODE",            &(TcpSockSrv[1].SlaveNodeAddress),       WORD_TYPE },	
	{"", NULL, WORD_TYPE}								
};

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

#endif

/*
//-----------------------------------------------------------------------------------

static const USR_DYNAMIC_INITIALIZE StringAAG_0_9[] = 					 
{
	{"O30_V2_I0",           &(DNP_AnalogInputs[0]),                 FLOAT_TYPE },	
	{"O30_V2_I1",           &(DNP_AnalogInputs[1]),                 FLOAT_TYPE },	
	{"O30_V2_I2",           &(DNP_AnalogInputs[2]),                 FLOAT_TYPE },	
	{"O30_V2_I3",           &(DNP_AnalogInputs[3]),                 FLOAT_TYPE },	
	{"O30_V2_I4",           &(DNP_AnalogInputs[4]),                 FLOAT_TYPE },	
	{"O30_V2_I5",           &(DNP_AnalogInputs[5]),                 FLOAT_TYPE },	
	{"O30_V2_I6",           &(DNP_AnalogInputs[6]),                 FLOAT_TYPE },	
	{"O30_V2_I7",           &(DNP_AnalogInputs[7]),                 FLOAT_TYPE },	
	{"O30_V2_I8",           &(DNP_AnalogInputs[8]),                 FLOAT_TYPE },	
	{"O30_V2_I9",           &(DNP_AnalogInputs[9]),                 FLOAT_TYPE },	
	{"", NULL, WORD_TYPE}								
};
*/
//----------------------------------------------------------------------------------


static const USR_DYNAMIC_INITIALIZE StringStatistic[] = 					 
{
	{"par_tx_err",          &StatisticData.SerialStat.MsgTxErr,     DWORD_TYPE},
	{"par_tx_ok",           &StatisticData.SerialStat.MsgTxOk,      DWORD_TYPE},
	{"par_rx_err",          &StatisticData.SerialStat.MsgRxErr,     DWORD_TYPE},
	{"par_rx_ok",           &StatisticData.SerialStat.MsgRxOk,      DWORD_TYPE},
	{"", NULL, WORD_TYPE}								
};


/*********************************************************************
*
* FUNCTION:			DemoXML
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
void DemoXML(BYTE fat_index, WORD wLen, BYTE bSession)
{
//  static BYTE sndbuf[512];
  static BYTE sndbuf[450];
	BYTE tmpbuf[100];
	WORD i = 0;
	USR_DYNAMIC_INITIALIZE *  StringFieldPtr;
	
//  fat_index++;  //not used - stops compiler warning
  wLen++;				//not used - stops compiler warning
  
  
 	(void)strcpy((char*)sndbuf, scsHeader);
 	
 	switch (fat_index){
 	  case 1:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)StringStatistic;
 	    break;
 
 	  case 3:					// DataObs.xml =  Campo de observaciones
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
 	    
#ifdef   DNP_MASTER_MODE
 	  case 4:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)Socket_01;
 	    break;

 	  case 5:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)Socket_02;
 	    break;

 	  case 6:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)Socket_03;
 	    break;
#endif

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

	/* Modify structure	*/
	https[bSession].fstart   = (UINT32)sndbuf;
	https[bSession].fstart   |= (UINT32)0xF0000000;
	https[bSession].flen     = strlen((char*)sndbuf);
	https[bSession].fpoint   = 0;
	https[bSession].funacked = 0;

}
					 
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


