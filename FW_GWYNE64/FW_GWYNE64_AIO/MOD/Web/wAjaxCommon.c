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
//#include "dynamic_tools.h"
#include "http_server.h"

#include "commdef.h"
#include "global.h"

#include <stdlib.h>
#include <MC9S12NE64.h>

#ifdef FW_WEBDATA_ENABLED

#include "wAjaxCommon.h"
						 
/**********************************************************************
*                           FILES TO HANDLE
**********************************************************************/
//
extern void LogsAjax    (UINT8 fat_index, UINT16 wLen, UINT8 bSession);
extern void EventsAjax    (UINT8 fat_index, UINT16 wLen, UINT8 bSession);
extern void ModbusAjax  (UINT8 fat_index, UINT16 wLen, UINT8 bSession);

const TFileEntry FAT [] = {
//        filename        function      file           file length    
#if       (FW_APP_BUILT == FW_APP_MODBUS)
     {   "TCPSOCK_01.xml", ModbusAjax,  NULL,               0                        }, //0 
     {   "TCPSOCK_02.xml", ModbusAjax,  NULL,               0                        }, //1 
     {   "MAQTABLE_01.xml", ModbusAjax,  NULL,              0                        }, //2 
     {   "MAQTABLE_02.xml", ModbusAjax,  NULL,              0                        }, //3 
     {   "MAQTABLE_03.xml", ModbusAjax,  NULL,              0                        }, //4 
     {   "MAQTABLE_04.xml", ModbusAjax,  NULL,              0                        }, //5 
     {   "MAQTABLE_05.xml", ModbusAjax,  NULL,              0                        }, //6 
#endif     
//     {   "logs.html",      NULL,        logs_html,          sizeof(logs_html)       }, 
     {   "gwyread.cgi",    WebApiRead,  NULL,               0                       }, 
     {   "gwywrite.cgi",   WebApiWrite, NULL,               0                       }, 
     {   "logs.xml",       LogsAjax,    NULL,               0                       }, 
     {   "events.xml",     EventsAjax,  NULL,               0                       }, 
     {   "",               NULL,        NULL,               0                       }
};

//     {   "gwyrun.cgi",     WebApiRun,   NULL,               0                       }, 

/*********************************************************************
*
* FUNCTION:			WebApiRead
*
* ARGUMENTS:		fat_index   Index of the file system table entry
*               wLen        Lenght of the request string from the web page
*               bSession    Http session
*
* RETURNS:			None
*
* DESCRIPTION:	
*
*********************************************************************/
void WebApiRead (BYTE fat_index, UINT16 wLen, BYTE bSession)
{

  	USR_DYNAMIC_FIELDS* StringFieldPtr;
    UINT16 ValuePairEnd;
  	BYTE tmpbuf [100];
    char szName [32];
    char szValue[32];
  	
  	fat_index++;      // evitar warning
 	
	  (void)strcpy((char*)sndbuf, scsHeader);

    do
    {
        ValuePairEnd = GetNameValuePair(wLen, szName, szValue);
        StringFieldPtr = FindTableName(szName, (USR_DYNAMIC_FIELDS *)(StringFields));
        if(StringFieldPtr != NULL)
        {
            if ( StringFieldPtr->fieldDataSource != NULL ) 
            {
        		    LoadValue((char*)tmpbuf, sizeof(tmpbuf),StringFieldPtr);
	              if ( (sizeof(sndbuf) - strlen((char*)sndbuf)) > strlen((char*)tmpbuf)) 
	              {
                    (void)strcat((char*)sndbuf, (char*)tmpbuf);
	              } 
	              else
	              {
                	  https[bSession].fstart    = (UINT32)https_ack_page_Bad;
                	  https[bSession].flen      = strlen((char *)https_ack_page_Bad);
                  	https[bSession].fstart   |= (UINT32)0xF0000000;
                  	https[bSession].fpoint   = 0;
                  	https[bSession].funacked = 0;
                  	return;
	              }
            } 
            else if ( StringFieldPtr->GetVarFunction != NULL ) 
            {
                (void)memset ((char*)tmpbuf, 0x00, sizeof(tmpbuf) );
                StringFieldPtr->GetVarFunction(StringFieldPtr->FunctionParam, StringFieldPtr->FunctionLen, (void*)tmpbuf);
	              if ( (sizeof(sndbuf) - strlen((char*)sndbuf)) > strlen((char*)tmpbuf)) 
	              {
                    (void)strcat((char*)sndbuf, (char*)tmpbuf);
	              } 
	              else
	              {
                	  https[bSession].fstart    = (UINT32)https_ack_page_Bad;
                	  https[bSession].flen      = strlen((char *)https_ack_page_Bad);
                  	https[bSession].fstart   |= (UINT32)0xF0000000;
                  	https[bSession].fpoint   = 0;
                  	https[bSession].funacked = 0;
                  	return;
	              }
            }
        }
        wLen -= ValuePairEnd;
        
    }while(ValuePairEnd);
    
	  (void)strcpy((char*)tmpbuf, scsTrailer);    // para poder obtener la longitud del string
  	if ( (sizeof(sndbuf) - strlen((char*)sndbuf)) > strlen((char*)tmpbuf)) 
  	{
  	    (void)strcat((char*)sndbuf, scsTrailer);
      	https[bSession].fstart   = (UINT32)sndbuf;
      	https[bSession].flen     = strlen((char*)sndbuf);
  	}
    else
    {
#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
		  (void)sprintf((char *)udp_buf,"WebRead: buf:%d, used:%d, need:%d \n",
		                sizeof(sndbuf) ,
		                strlen((char*)sndbuf),
		                strlen((char*)tmpbuf) );
      udp_data_to_send = TRUE;
  }
#endif  		
    	  https[bSession].fstart    = (UINT32)https_ack_page_Bad;
    	  https[bSession].flen      = strlen((char *)https_ack_page_Bad);
    }
  	
  	https[bSession].fstart   |= (UINT32)0xF0000000;
  	https[bSession].fpoint   = 0;
  	https[bSession].funacked = 0;
   	
   	return;  

}

/*********************************************************************
*
* FUNCTION:			WebApiWrite
*
* ARGUMENTS:		fat_index   Index of the file system table entry
*               wLen        Lenght of the request string from the web page
*               bSession    Http session
*
* RETURNS:			None
*
* DESCRIPTION:	
*
*********************************************************************/
void WebApiWrite (BYTE fat_index, UINT16 wLen, BYTE bSession)
{
  	USR_DYNAMIC_FIELDS* StringFieldPtr;
    UINT16 ValuePairEnd;
    char szName [32];
    char szValue[32];
  	
  	fat_index++;        // evitar warning  
  	
    TagFlags= 0x00;
    do
    {
        ValuePairEnd = GetNameValuePair(wLen, szName, szValue);
        StringFieldPtr = FindTableName(szName, (USR_DYNAMIC_FIELDS *)(StringFields));
        if(StringFieldPtr != NULL)
        {
            if ( StringFieldPtr->fieldType & WRITE_ACCESS_MASK) 
            {
                if ( StringFieldPtr->fieldDataSource != NULL )
                {
                    SaveValue(szValue, StringFieldPtr);
                } 
                else if ( StringFieldPtr->SetVarFunction != NULL ) 
                {
                    StringFieldPtr->SetVarFunction(StringFieldPtr->FunctionParam, StringFieldPtr->FunctionLen, (void*)szValue);
                }
            }
            else
            {
                WebPrepareMsgAnswer ("Error: No tiene permisos de escritura", 0x01, bSession);
                return;
            }
        }
        wLen -= ValuePairEnd;
        
    }while(ValuePairEnd);
  
	  https[bSession].fstart    = (UINT32)https_ack_page_Accepted;
	  https[bSession].flen      = strlen((char *)https_ack_page_Accepted);
  	https[bSession].fstart   |= (UINT32)0xF0000000;
  	https[bSession].fpoint   = 0;
  	https[bSession].funacked = 0;
  	
#ifdef MOD_MODBUSRTU_AUTOQUERY_ENABLED  	
  	if (TagFlags & 0x01)
  	  MOD_ModbusRtuAutoQuerySaveBanks();
#endif
  	
   	return;  

}


void WebPrepareMsgAnswer (char * MsgText, UINT16 MsgCode, BYTE bSession) 
{
  	BYTE tmpbuf [100];

  	(void)strcpy((char*)sndbuf, https_ack_page_OK);	
   	(void)strcat((char*)sndbuf, scsHeader);
    (void)sprintf((char*)tmpbuf, "<%s>%s</%s>\n", 
	                    (char*)"MsgInfo", 
	                    MsgText, 
	                    (char*)"MsgInfo");
	                    
  	if ( (sizeof(sndbuf) - strlen((char*)sndbuf)) > strlen((char*)tmpbuf)) 
  	{
        (void)strcat((char*)sndbuf, (char*)tmpbuf); 
  	}
  	else
    {
    	  https[bSession].fstart    = (UINT32)https_ack_page_Bad;
    	  https[bSession].flen      = strlen((char *)https_ack_page_Bad);
      	https[bSession].fstart   |= (UINT32)0xF0000000;
      	https[bSession].fpoint   = 0;
      	https[bSession].funacked = 0;
      	return;
    }
  	
	  (void)sprintf((char*)tmpbuf, "<%s>%d</%s>\n", 
	                    (char*)"MsgCode", 
	                    MsgCode, 
	                    (char*)"MsgCode");
	                    
  	if ( (sizeof(sndbuf) - strlen((char*)sndbuf)) > strlen((char*)tmpbuf)) 
  	{
        (void)strcat((char*)sndbuf, (char*)tmpbuf);	 
  	}
  	else
    {
    	  https[bSession].fstart    = (UINT32)https_ack_page_Bad;
    	  https[bSession].flen      = strlen((char *)https_ack_page_Bad);
      	https[bSession].fstart   |= (UINT32)0xF0000000;
      	https[bSession].fpoint   = 0;
      	https[bSession].funacked = 0;
      	return;
    }
              
	  (void)strcpy((char*)tmpbuf, scsTrailer);    // para poder obtener la longitud del string
  	if ( (sizeof(sndbuf) - strlen((char*)sndbuf)) > strlen((char*)tmpbuf)) 
  	{
    	  (void)strcat((char*)sndbuf, scsTrailer);
      	https[bSession].fstart   = (UINT32)sndbuf;
      	https[bSession].fstart   |= (UINT32)0xF0000000;
      	https[bSession].flen     = strlen((char*)sndbuf);
      	https[bSession].fpoint   = 0;
      	https[bSession].funacked = 0;
  	}
  	else
    {
    	  https[bSession].fstart    = (UINT32)https_ack_page_Bad;
    	  https[bSession].flen      = strlen((char *)https_ack_page_Bad);
      	https[bSession].fstart   |= (UINT32)0xF0000000;
      	https[bSession].fpoint   = 0;
      	https[bSession].funacked = 0;
    }
    
   	return;
}


void WebReadEEpromVar (UINT16 getParam, UINT16 getLen, void* getData) 
{
    eeprom_read_buffer(getParam, (UINT8*)getData, getLen);  
}

void WebWriteEEpromVar (UINT16 setParam, UINT16 setLen, void* setData) 
{
    eeprom_write_buffer(setParam, (UINT8*)setData, setLen);  
}


#endif  	//  FW_WEBDATA_ENABLED
