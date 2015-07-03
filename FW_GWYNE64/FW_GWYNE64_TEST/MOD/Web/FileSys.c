/******************************************************************************
 *
 *                      (c) Freescale  Inc. 2004 All rights reserved
 *
 * File Name     : FileSys.c
 * Description   : 
 *
 * Version : 1.0
 * Date    : Mar/05/2004
 *
 *
 ******************************************************************************/
#include "FileSys.h"
#include <stdlib.h>
#include <MC9S12NE64.h>


extern void DemoXML (UINT8 fat_index, UINT16 wLen, UINT8 bSession);
extern void DemoCGI (UINT8 fat_index, UINT16 wLen, UINT8 bSession);


const TFileEntry FAT [] = {
//        filename        function      file           file length    
     {   "demo.cgi"   ,    DemoCGI,  NULL,               0                       }, //0
     {   "data_1.xml" ,    DemoXML,  NULL,               0                       }, //1
     {   "data_2.xml" ,    DemoXML,  NULL,               0                       }, //2
     {   "DataObs.xml",    DemoXML,  NULL,               0                       }, //3
     {   "DNPSOCK_01.xml", DemoXML,  NULL,               0                       }, //4 
     {   "DNPSOCK_02.xml", DemoXML,  NULL,               0                       }, //5 
     {   "DNPSOCK_03.xml", DemoXML,  NULL,               0                       }, //6 
     {   ""           ,    NULL,     NULL,               0                       }
};

//     {   "goodhttp204",    NULL,     GoodHTTP204,        sizeof(GoodHTTP204)     },	 //  51
