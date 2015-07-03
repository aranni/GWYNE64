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


extern void DemoAjax (UINT8 fat_index, UINT16 wLen, UINT8 bSession);
extern void LogsAjax (UINT8 fat_index, UINT16 wLen, UINT8 bSession);
extern void DemoCGI (UINT8 fat_index, UINT16 wLen, UINT8 bSession);


const TFileEntry FAT [] = {
//        filename        function      file           file length    
     {   "web_write.php",  DemoAjax,  NULL,               0                       }, //0
     {   "web_read.php",   DemoAjax,  NULL,               0                       }, //1
     {   "Estadis.xml" ,   DemoAjax,  NULL,               0                       }, //2
     {   "Version.xml",    DemoAjax,  NULL,               0                       }, //3
     {   "DataObs.xml",    DemoAjax,  NULL,               0                       }, //4
     {   "TCPSOCK_01.xml", DemoAjax,  NULL,               0                       }, //5 
     {   "TCPSOCK_02.xml", DemoAjax,  NULL,               0                       }, //6 
     {   "BACNODE_01.xml", DemoAjax,  NULL,               0                       }, //7 
     {   "BACNODE_02.xml", DemoAjax,  NULL,               0                       }, //8 
     {   "BACNODE_03.xml", DemoAjax,  NULL,               0                       }, //9 
     {   "BACNODE_04.xml", DemoAjax,  NULL,               0                       }, //10
     {   "BACNODE_05.xml", DemoAjax,  NULL,               0                       }, //11
     {   "BACNODE_06.xml", DemoAjax,  NULL,               0                       }, //12
     {   "BACNODE_07.xml", DemoAjax,  NULL,               0                       }, //13
     {   "BACNODE_08.xml", DemoAjax,  NULL,               0                       }, //14
     {   "BACNODE_09.xml", DemoAjax,  NULL,               0                       }, //15
     {   "BACNODE_10.xml", DemoAjax,  NULL,               0                       }, //16
     {   "BAC_CONFIG.xml", DemoAjax,  NULL,               0                       }, //17
     {   "logs.xml",       LogsAjax,  NULL,               0                       }, //18
     {   ""           ,    NULL,     NULL,                0                       }
};
//     {   "goodhttp204",    NULL,     GoodHTTP204,        sizeof(GoodHTTP204)     },	 //  51
