/******************************************************************************
 *
 *                      (c) Freescale  Inc. 2004 All rights reserved
 *
 * File Name     : CGI_Func.c
 * Description   : 
 *
 * Version : 1.0
 * Date    : Mar/05/2004
 *
 *
 ******************************************************************************/
#include "CGI_Func.h"
#include <stdlib.h>

#include <MC9S12NE64.h>
#include <string.h>
#include "system.h"
//#include "timedate.h"
#include "commdef.h"
#include "global.h"


#define NumCmds  (sizeof(CmdTable) / sizeof(CmdTblEntry))	/* the number of entries in the command table */

char ProcessCommandLine(char *CmdLineStr);
void get_PartID_func(void);
void get_SwVer_func(void);
void get_HwVer_func(void);
void get_Serial_func(void);

extern int sprintf(LIBDEF_StringPtr s, LIBDEF_ConstStringPtr format, ...);
char temp_buff[33];
char CGI_Temp_Buf[20];


/*************************************************************************************/
/* Command Table entry definition */
/* Command Table */

typedef struct {
                const char *CommandStr;					/* pointer to the command string */
                void (*far ExecuteCmd)(void);	  /* pointer to the function that implements the command */
                } CmdTblEntry, *CmdTblEntryP;


// Commands can not have white space embeded in them !!!
const static CmdTblEntry CmdTable[] = { 
                                       "SwVer_get",   get_SwVer_func,
                                       "PartID_get",  get_PartID_func,
                                       "Serial_get",   get_Serial_func,
                                       "HwVer_get",   get_HwVer_func,
                                      };


/*************************************************************************************/
/*************************************************************************************/
char ProcessCommandLine(char *CmdLineStr)
 {
  int  x;							            /* index for searching the command table */
  for (x = 0; x < NumCmds; x++)		/* search the command table*/
//   if (strcmp(CmdTable[x].CommandStr, &CGI_Temp_Buf[0]) == 0)
   if (strcmp(CmdTable[x].CommandStr, CmdLineStr) == 0)
    {
     CmdTable[x].ExecuteCmd();    /* execute the command & return*/
     return(1);							
    }
  return(-1);  //Error command not found
 }
 
 
 /**************************************************************************
* Name: get part ID_message
*   
*  Updates:
**************************************************************************/
void get_PartID_func(void)
{
UINT16 PartID = ((PARTIDH<<8) + PARTIDL);
	switch(PartID)
	{
		case 0x8200:(void) sprintf(temp_buff,"%s","0L19S  9S12NE64 1.0"); break;
		case 0x8201:(void) sprintf(temp_buff,"%s","1L19S  9S12NE64 1.1"); break;
		case 0x8202:(void) sprintf(temp_buff,"%s","2L19S  9S12NE64 1.2"); break;
		default:(void)  sprintf(temp_buff,"????");
	}
}

//***************************************************************************
//  get_SwVer_func .... 
//***************************************************************************
void get_SwVer_func(void){
   
   (void) sprintf(temp_buff,"%s",FwVersionData.FW_Version);
}

void get_HwVer_func(void){
     	eeprom_read_buffer(EE_ADDR_VER_HWVER,(UINT8 *)(temp_buff), SIZE_HW_VER); 
}

void get_Serial_func(void){
     	eeprom_read_buffer(EE_ADDR_VER_SERIAL,(UINT8 *)(temp_buff), SIZE_SERIAL); 
}
  

//***************************************************************************
//  Print_time_func .... 
//***************************************************************************
/* 
void Print_time_func() { 

        (void) sprintf(temp_buff,"%02lu:%02lu:%02lu",\
        ((run_count/1000/60/60)%24),\
        ((run_count/1000/60)%60),\
        ((run_count/1000)%60));
}
*/ 

//***************************************************************************
//  get_MAC_func .... 
//***************************************************************************
/* 
void get_MAC_func(void){
   
   (void) sprintf(temp_buff,"%02X:%02X:%02X:%02X:%02X:%02X",localmachine.localHW[0],localmachine.localHW[1],localmachine.localHW[2],localmachine.localHW[3],localmachine.localHW[4],localmachine.localHW[5]);
}
*/  
//***************************************************************************
//  get_SMTP_func .... 
//***************************************************************************
/* 
void get_smtp_func(void){
   
   (void) sprintf(temp_buff,"%d.%d.%d.%d",(UINT8)(IP_SMTP_Server>>24)\
                                                 ,(UINT8)(IP_SMTP_Server>>16)\
                                                 ,(UINT8)(IP_SMTP_Server>>8)\
                                                 ,(UINT8)(IP_SMTP_Server));
}
*/
//***************************************************************************
//  get_Time_func .... 
//***************************************************************************
/* 
void get_Time_func(void){
   
   (void) sprintf(temp_buff,"%d.%d.%d.%d",(UINT8)(IP_Time_Server>>24)\
                                         ,(UINT8)(IP_Time_Server>>16)\
                                         ,(UINT8)(IP_Time_Server>>8)\
                                         ,(UINT8)(IP_Time_Server));
}
*/
//***************************************************************************
//  get_Time1900_func .... 
//***************************************************************************
/* 
void get_Time1900_func(void){
   (void) sprintf(temp_buff,"%s",asctime(&systime_tm));
}
*/
//***************************************************************************
//  get_Submask_func .... 
//***************************************************************************
/* 
void get_SubMsk_func(void) {
   
   (void) sprintf(temp_buff,"%d.%d.%d.%d",(UINT8)(localmachine.netmask>>24)\
                                                 ,(UINT8)(localmachine.netmask>>16)\
                                                 ,(UINT8)(localmachine.netmask>>8)\
                                                 ,(UINT8)(localmachine.netmask));

}
*/
//***************************************************************************
//  get_Sip_func .... 
//***************************************************************************
/* 
void get_Sip_func(void) {
   
   (void) sprintf(temp_buff,"%d.%d.%d.%d",(UINT8)(localmachine.localip>>24)\
                                                 ,(UINT8)(localmachine.localip>>16)\
                                                 ,(UINT8)(localmachine.localip>>8)\
                                                 ,(UINT8)(localmachine.localip));

}
*/
//***************************************************************************
//  get_Dip_func .... 
//***************************************************************************
/* 
void get_Dip_func(void) {
   (void) sprintf(temp_buff,"%d.%d.%d.%d",(UINT8)(received_ip_packet.sip>>24)\
                                                 ,(UINT8)(received_ip_packet.sip>>16)\
                                                 ,(UINT8)(received_ip_packet.sip>>8)\
                                                 ,(UINT8)(received_ip_packet.sip));
}
*/   
//***************************************************************************
//  get_GW_func .... 
//***************************************************************************
/* 
void get_GW_func(void) {
   
   (void) sprintf(temp_buff,"%d.%d.%d.%d",(UINT8)(localmachine.defgw>>24)\
                                                 ,(UINT8)(localmachine.defgw>>16)\
                                                 ,(UINT8)(localmachine.defgw>>8)\
                                                 ,(UINT8)(localmachine.defgw));
}
*/
//***************************************************************************
//  get_DNS_func .... 
//***************************************************************************
/* 
void get_DNS_func(void) {
   
   (void) sprintf(temp_buff,"%d.%d.%d.%d",(UINT8)(DNS_SERVER_IP>>24)\
                                                 ,(UINT8)(DNS_SERVER_IP>>16)\
                                                 ,(UINT8)(DNS_SERVER_IP>>8)\
                                                 ,(UINT8)(DNS_SERVER_IP));
}
*/
//***************************************************************************
//  post_clock_func .... 
//***************************************************************************
/* 
void post_clock_func(void){
tU32 clock_temp;

    clock_temp=atol(&temp_buff[6])*60*60;
    clock_temp=clock_temp+(atol(&temp_buff[11])*60);
    clock_temp=clock_temp+atol(&temp_buff[16]);
    run_count=clock_temp*1000;
}
*/