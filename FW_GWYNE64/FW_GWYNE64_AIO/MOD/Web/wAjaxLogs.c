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
#include "http_server.h"

#include "commdef.h"
#include "global.h"

#include <stdlib.h>
#include <MC9S12NE64.h>

#include "wAjaxLogs.h"

extern struct http_server_state https [NO_OF_HTTP_SESSIONS];

void LogsAjax(BYTE fat_index, UINT16 wLen, BYTE bSession) 
{
  
	BYTE tmpbuf[100];
	WORD idx = 0;
	USR_DYNAMIC_FIELDS* StringFieldPtr;
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
        StringFieldPtr = FindTableName(szName, (USR_DYNAMIC_FIELDS *)(LogsReqFields));
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
  		 
