/******************************************************************************
 *
 *                      (c) Freescale  Inc. 2005 All rights reserved
 *
 * File Name     : FileSys.h
 * Description   : 
 *
 * Version : 1.1
 * Date    : Mar/05/2005
 *
 *
 ******************************************************************************/
#ifndef __FILE_SYS_H__
#define __FILE_SYS_H__

#include"datatypes.h"

typedef struct TFileEntry
{
    INT8                    filename[24];
    void                    (* GetFunction)(UINT8 fat_index, UINT16 len, UINT8 ses);
    const unsigned char*    file_start_address;
	  unsigned short          file_length;
} TFileEntry;

extern const TFileEntry FAT [];

#endif
