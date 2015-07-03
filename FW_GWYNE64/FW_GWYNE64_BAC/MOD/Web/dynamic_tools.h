#ifndef DToolsH
#define DToolsH

/*********************************************************************
*
* SOURCE FILENAME:	dynamic_tools.h
*
* DATE CREATED:		28 June 2002
*
* PROGRAMMER:		Tom Parkinson/Karl Kobel
*
* DESCRIPTION:  	CGI and XML support functions
*
* Copyright (c) 2005 Tecnova.
* All rights reserved. 
* 
* Redistribution and use in source and binary forms, with or without modification, 
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
* 3. The name of the author may not be used to endorse or promote products
*    derived from this software without specific prior written permission. 
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
* SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
* OF SUCH DAMAGE.
*
* Tecnova
* 1486 Saint Paul Avenue
* Gurnee, IL  60031-2129
* U.S.A.
 
* Telephone: +1.847.662.6260
* Facsimile: +1.847.336.7288
* Internet: www.Tecnova.com
*
**********************************************************************/

extern const char https_ack_page[];

//#define DATA_TYPE_MASK  0xfff0
#define DATA_TYPE_MASK  0x00ff  // ABR: Revisar luego todo el mecanismo...
#define PRECISION_MASK  0x000f

#define WORD_TYPE   0x0010
#define FLOAT_TYPE  0x0020
#define DWORD_TYPE  0x0040
#define STRING_TYPE 0x0080

#define BIT_0_TYPE  0x0001
#define BIT_1_TYPE  0x0002
#define BIT_2_TYPE  0x0003
#define BIT_3_TYPE  0x0004
#define BIT_4_TYPE  0x0005
#define BIT_5_TYPE  0x0006
#define BIT_6_TYPE  0x0007
#define BIT_7_TYPE  0x0008

#define DWORD_HH_TYPE  0x0009
#define DWORD_HL_TYPE  0x000A
#define DWORD_LH_TYPE  0x000B
#define DWORD_LL_TYPE  0x000C

#define BYTE_TYPE     0x000D
#define XWORD_TYPE    0x000E


typedef struct 
{
  char* fieldInitialize;
	void*	fieldDataSource;
	unsigned char fieldType;
} USR_DYNAMIC_INITIALIZE;


extern void LoadValue(char *tmpbuf, int iBufSize, USR_DYNAMIC_INITIALIZE *ptrUDI);
extern void SaveValue(char *ptr, USR_DYNAMIC_INITIALIZE *ptrUDI);

extern UINT16 GetNameValuePair(UINT16 wLen, char* pszName, char* pszValue);
extern USR_DYNAMIC_INITIALIZE* FindTableName(char* pszName, USR_DYNAMIC_INITIALIZE *ptrUDI);

extern char* XMLEntityOut(char* myData, char* sOut);

#endif // DToolsH