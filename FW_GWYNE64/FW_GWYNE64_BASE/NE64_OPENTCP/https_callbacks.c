#include"datatypes.h"
#include"debug.h"
#include"globalvariables.h"
#include"system.h"
#include"http_server.h"
#include <string.h>

#include "commdef.h"
#include "global.h"

extern struct   http_server_state https [NO_OF_HTTP_SESSIONS];

INT16 https_findfile        (INT16 len, UINT8 ses);
INT16 https_loadbuffer      (UINT8, UINT8*, UINT16);

#define NULL                   ((void *) 0)
const char https_not_found_page[] = "<HEAD><TITLE> Conversor de protocolos -  WEB Server</TITLE></HEAD><BODY><H2> GWY - Conversor de Protocolos.</H2><HR>Servicio de gestion web deshabilitado en esta version<HR>></BODY>";


/** \brief Brief function description here
 * 	\author 
 *		\li Jari Lahti (jari.lahti@violasystems.com)
 *	\date 09.10.2002
 *	\param hash Calculated file-name hash value. Used so that the whole
 *		file name doesn't need to be stored in RAM
 *	\param ses HTTP session identifier
 *	\return
 *		\li -1 - This function should return -1 if no file has been found
 *		\li 1 - This function should return 1 if a file with appropriate
 *			hash value has been found.
 *	\warning
 *		\li This function <b>MUST</b> be implemented by user application
 *		to work with local configuration
 *
 *	This function is invoked by the HTTP server once a hash value of a 
 *	requested file name has been calculated. User application uses this
 *	hash value to check if appropriate file is available to web server.
 *	Appropriate https session entry is then filled accordingly.	
 *
 */
 
INT16 https_findfile (INT16 len, UINT8 ses)
{
  UINT32  i;
  unsigned char  j = 0;
  char    buf[24];
  UINT16  addr;
  WEB_FAT FatEntry;

    for (i=0; i<len; i++){
        if(i == 24)
            break;
        buf[i] = RECEIVE_NETWORK_B();
		    if ( buf[i] ==' ' ||  buf[i] =='=' || buf[i] =='\n' || buf[i] =='?')
          break;
    }
    
    buf[i] = '\0';
    if(buf[0] == '\0')
      strcpy(buf, "index.html"); 
        
#ifdef FW_WEBDATA_ENABLED        
  // Buscar primero el archivo en la lista de xml y cgi alojada en memoria de programa
	for (j=0; FAT[j].filename[0]; j++)
	{       
     if (strcmp(buf, FAT[j].filename) == 0){
          	if(FAT[j].GetFunction != NULL){          	  
                FAT[j].GetFunction(j, len, ses);
                return(1);
	          }
	          else{
              // para agregar la posibilidad de alojar una pagina en memoria de programa.
              if ( FAT[j].file_length ){
      	  		    https[ses].fstart = (UINT32)FAT[j].file_start_address;
      	  		    https[ses].fstart |= (UINT32)0xF0000000;
                  https[ses].funacked  = 0;
                  https[ses].flen = FAT[j].file_length;
                  https[ses].fpoint = 0;	
                  return(1);
              }
	            break;  	            
	          }
     }
	}
#endif
                
    // Buscar ahora si esta el archivo en la lista alojada en memoria flash externa           
    addr = 0;
    for(;;){
    
        FLA_ReadReg ((UINT16)addr, (UINT8)(sizeof(WEB_FAT)), (UINT8*) &FatEntry);
        
        if (addr > 99) break;
        if(FatEntry.filename[0] == 0) break;
        
        if (strcmp(buf, FatEntry.filename) == 0){
        		https[ses].fstart = FatEntry.file_addr;
        		https[ses].fstart += 100;
        		https[ses].fstart <<= 8;        		
        		        		
        		https[ses].funacked  = 0;
        		https[ses].flen = FatEntry.file_lenght;
        		https[ses].fpoint = 0;
            return(1);
        }
        addr++;        
    }
        
  // Si no se encontro en ningun lugar, enviar pagina de aviso
  https[ses].fstart    = (UINT32)https_not_found_page;
  https[ses].fstart   |= (UINT32)0xF0000000;
  https[ses].funacked  = 0;
  https[ses].flen = strlen(&https_not_found_page[0]);
  https[ses].fpoint = 0;	
  return(-1);
  
}


//=======================================================
/** \brief Fill network transmit buffer with HTTP headers&data
 * 	\author
 *		\li Jari Lahti (jari.lahti@violasystems.com)
 *	\date 09.10.2002
 *	\param ses HTTP session identifier
 *	\param buf Pointer to buffer where data is to be stored
 *	\param buflen Length of the buffer in bytes
 *	\return
 *		\li >=0 - Number of bytes written to buffer
 *	\warning 
 *		\li This function <b>MUST</b> be implemented by user application
 *		to work with local configuration
 *
 *	This handlers' job is to fill the buffer with the data that web server
 *	should return back through the TCP connection. This is accomplished
 *	based session identifer and values of variables in appropriate
 *	https entry.
 */
INT16 https_loadbuffer (UINT8 ses, UINT8* buf, UINT16 buflen) 
{
	INT16 fPointCntr;
	INT16  BytesInsertCntr;
	UINT8 * TrueStartPtr;
	
  // Si es una pagina de control o generada internamente por el FW.
	if( https[ses].fstart & 0xF0000000 )
	{
    TrueStartPtr = (UINT8 *)(https[ses].fstart & ((UINT32)(~0xF0000000)));
		kick_WD();
		
		for(fPointCntr=0; fPointCntr < (https[ses].flen - https[ses].fpoint); fPointCntr++)
		{
			if(fPointCntr >= buflen)
				break;
		
			*buf++ = TrueStartPtr[https[ses].fpoint + fPointCntr];
		}
    https[ses].funacked = fPointCntr;
		return(fPointCntr);
	
	}
	
	
  // Cargar el fragmento de pagina desde la flash externa 
	BytesInsertCntr = (INT16)(https[ses].flen - https[ses].fpoint);
  if(BytesInsertCntr >= buflen) BytesInsertCntr = buflen;		
  FLA_ReadBuff ( (https[ses].fstart + https[ses].fpoint), 
                   (UINT16)(BytesInsertCntr), 
                   (UINT8*) buf );    

  https[ses].funacked = BytesInsertCntr;
  
	return (BytesInsertCntr);
	
}
