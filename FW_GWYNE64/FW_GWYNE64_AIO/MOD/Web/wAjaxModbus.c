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

#if       (FW_APP_BUILT == FW_APP_MODBUS)

#include "wAjaxModbus.h"

extern struct http_server_state https [NO_OF_HTTP_SESSIONS];





void ModbusAjax(BYTE fat_index, WORD wLen, BYTE bSession)
{
  static BYTE sndbuf[450];
	BYTE tmpbuf[100];
	WORD i = 0;
	USR_DYNAMIC_INITIALIZE *  StringFieldPtr;
	
//  fat_index++;  //not used - stops compiler warning
  wLen++;				//not used - stops compiler warning
  
  
 	(void)strcpy((char*)sndbuf, scsHeader);
 	
 	switch (fat_index){

 	  case 0:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)Socket_01;
 	    break;

 	  case 1:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)Socket_02;
 	    break;

#ifdef MOD_MODBUSRTU_AUTOQUERY_ENABLED  	

 	  case 2:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)MaqNode_01;
 	    break;

 	  case 3:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)MaqNode_02;
 	    break;

 	  case 4:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)MaqNode_03;
 	    break;

 	  case 5:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)MaqNode_04;
 	    break;

 	  case 6:
 	    StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)MaqNode_05;
 	    break;

#endif 

 	  default:
 	    //StringFieldPtr = (USR_DYNAMIC_INITIALIZE *)StringFields;
 	    break;
 	    
 	}


	while (StringFieldPtr[i].fieldDataSource != NULL)
	{
		LoadValue((char*)tmpbuf, sizeof(tmpbuf),(USR_DYNAMIC_FIELDS *)&StringFieldPtr[i]);
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

  		 
#endif  	//  (FW_APP_BUILT == FW_APP_SPA)
