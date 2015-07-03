/*********************************************************************
*
* SOURCE FILENAME:	dynamic_tools.c
*
* DATE CREATED:		9 Feb 2005
*
* AUTHOR:	    	Karl Kobel/Tom Parkinson
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
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
//#include <math.h>
#include "datatypes.h"
#include "dynamic_tools.h"
#include "system.h"
#include <stdlib.h>


/**********************************************************************
*                           FUNCTION PROTOTYPES
**********************************************************************/

extern double pow  (double x, double y);
void Fn2A(char *pcString, float fValue, int nPrecision);
float atof1(const char * p);
char *StripPlus(char *ptr);
float ptrToFloat(char *ptrA);
void SYS_LongToStr (UINT32 val, UINT8 * DataStr );

/**********************************************************************
*                           GLOBAL VARIABLES
**********************************************************************/
unsigned char scsWdDataLine[] = "<%s>%d</%s>\n";			
//unsigned char scsFtDataLine[] = "<%s>%.2f</%s>\n";			
unsigned char scsSzDataLine[] = "<%s>%s</%s>\n";			

int errno;

/**********************************************************************
*                           LOCAL VARIABLES
**********************************************************************/

/**********************************************************************
*                           CODE
**********************************************************************/

/*********************************************************************
*
* FUNCTION:			LoadValue
*
* ARGUMENTS:		tmpbuf
*                   iBufSize
*                   ptrUDI
*
* RETURNS:			None
*
* DESCRIPTION:		
*              
* RESTRICTIONS:		
*
*********************************************************************/
void LoadValue(char *tmpbuf, int iBufSize, USR_DYNAMIC_INITIALIZE *ptrUDI)
{
	int wVal;
	char* szptr;
	UINT32 dVal;
	UINT16 wType;
	float fVal;
	char tmpbuf1[20];
	BYTE  bVal;
	WORD  wTmpEndian;

//    pragma unused(iBufSize);
  iBufSize++;
    
	szptr = ptrUDI->fieldInitialize;
	wType = ptrUDI->fieldType & DATA_TYPE_MASK;
	
	switch (wType)
	{
	  case STRING_TYPE:
		  (void)sprintf((char*)tmpbuf, (char*)scsSzDataLine, szptr, ptrUDI->fieldDataSource, szptr);
		  break;

	  case WORD_TYPE:
//      wVal = *((UINT16*)ptrUDI->fieldDataSource);		
      wVal = *((UINT16*)ptrUDI->fieldDataSource);
      wTmpEndian = (wVal & 0xFF00)>>8;
      wVal  <<= 8 ;		
      wVal |= wTmpEndian;
      		 
		  (void)sprintf((char*)tmpbuf, (char*)scsWdDataLine, szptr, wVal, szptr);
		  break;

	  case DWORD_TYPE:
      dVal = *((UINT32*)ptrUDI->fieldDataSource);
//		  (void)sprintf((char*)tmpbuf, (char*)scsDdDataLine, szptr, dVal, szptr);		
      SYS_LongToStr	(dVal, (UINT8*)tmpbuf1);
      (void)sprintf((char*)tmpbuf,"<%s>%s</%s>", szptr,tmpbuf1, szptr);
      break;

	  case DWORD_HH_TYPE:
      dVal = *((UINT32*)ptrUDI->fieldDataSource) & 0xFF000000;
      bVal = (BYTE)(dVal>>24);
  	  (void)sprintf((char*)tmpbuf, (char*)scsWdDataLine, szptr, bVal, szptr);		
      break;

	  case DWORD_HL_TYPE:
      dVal = *((UINT32*)ptrUDI->fieldDataSource) & 0x00FF0000;
      bVal = (BYTE)(dVal>>16);
  	  (void)sprintf((char*)tmpbuf, (char*)scsWdDataLine, szptr, bVal, szptr);		
      break;
      
	  case DWORD_LH_TYPE:
      dVal = *((UINT32*)ptrUDI->fieldDataSource) & 0x0000FF00;
      bVal = (BYTE)(dVal>>8);
  	  (void)sprintf((char*)tmpbuf, (char*)scsWdDataLine, szptr, bVal, szptr);		
      break;
      
	  case DWORD_LL_TYPE:
      dVal = *((UINT32*)ptrUDI->fieldDataSource) & 0x000000FF;
      bVal = (BYTE)dVal;
  	  (void)sprintf((char*)tmpbuf, (char*)scsWdDataLine, szptr, bVal, szptr);		
      break;
      
	  case FLOAT_TYPE:
      fVal = *((float*)ptrUDI->fieldDataSource);
      (void) Fn2A((char*)tmpbuf1, fVal, 2);
      (void)sprintf((char*)tmpbuf,"<%s>%s</%s>", szptr,tmpbuf1, szptr);
      break;

	  case BIT_0_TYPE:
      wVal = (*((UINT8*)ptrUDI->fieldDataSource) & 0x01)?1:0; ;				 
		  (void)sprintf((char*)tmpbuf, (char*)scsWdDataLine, szptr, wVal, szptr);
		  break;

	  case BIT_1_TYPE:
      wVal = (*((UINT8*)ptrUDI->fieldDataSource) & 0x02)?1:0; ;				 
		  (void)sprintf((char*)tmpbuf, (char*)scsWdDataLine, szptr, wVal, szptr);
	    break;
	    
	  case BIT_2_TYPE:
      wVal = (*((UINT8*)ptrUDI->fieldDataSource) & 0x04)?1:0; ;				 
		  (void)sprintf((char*)tmpbuf, (char*)scsWdDataLine, szptr, wVal, szptr);
		  break;

	  case BIT_3_TYPE:
      wVal = (*((UINT8*)ptrUDI->fieldDataSource) & 0x08)?1:0; ;				 
		  (void)sprintf((char*)tmpbuf, (char*)scsWdDataLine, szptr, wVal, szptr);
		  break;

	  case BIT_4_TYPE:
      wVal = (*((UINT8*)ptrUDI->fieldDataSource) & 0x10)?1:0; ;				 
		  (void)sprintf((char*)tmpbuf, (char*)scsWdDataLine, szptr, wVal, szptr);
		  break;

	  case BIT_5_TYPE:
      wVal = (*((UINT8*)ptrUDI->fieldDataSource) & 0x20)?1:0; ;				 
		  (void)sprintf((char*)tmpbuf, (char*)scsWdDataLine, szptr, wVal, szptr);
		  break;

	  case BIT_6_TYPE:
      wVal = (*((UINT8*)ptrUDI->fieldDataSource) & 0x40)?1:0; ;				 
		  (void)sprintf((char*)tmpbuf, (char*)scsWdDataLine, szptr, wVal, szptr);
		  break;

	  case BIT_7_TYPE:
      wVal = (*((UINT8*)ptrUDI->fieldDataSource) & 0x80)?1:0; ;				 
		  (void)sprintf((char*)tmpbuf, (char*)scsWdDataLine, szptr, wVal, szptr);
		  break;
	
	  default:
	    break;	  
	}
	
}																	


/*********************************************************************
*
* FUNCTION:			SaveValue
*
* ARGUMENTS:		None
*
* RETURNS:			None
*
* DESCRIPTION:		
*              
* RESTRICTIONS:		
*
*********************************************************************/
void SaveValue(char *ptr, USR_DYNAMIC_INITIALIZE *ptrUDI)
{
	if (ptr)
	{	//parameter was found in response...
		if (ptrUDI->fieldType == WORD_TYPE)
		{
            *(UINT16*)ptrUDI->fieldDataSource = atoi(ptr);

		}else if (ptrUDI->fieldType == DWORD_TYPE)
		{
            *(UINT32*)ptrUDI->fieldDataSource = atol(ptr);
		}
		else if(ptrUDI->fieldType == STRING_TYPE)
		{
            (void)strcpy((char*)ptrUDI->fieldDataSource, StripPlus(ptr));
		}
    else      //float
    {
            *(float*)ptrUDI->fieldDataSource = ptrToFloat(ptr);
     }
	}
}

/*********************************************************************
*
* FUNCTION:			StripPlus
*
* ARGUMENTS:		None
*
* RETURNS:			None
*
* DESCRIPTION:		
*              
* RESTRICTIONS:		
*
*********************************************************************/
char *StripPlus(char *ptr)
{
	char *ptrLocal = ptr;

	while (*ptrLocal)
	{
		if (*ptrLocal == '+') *ptrLocal = ' ';
		ptrLocal++;
	}
	return ptr;
}



/*********************************************************************
*
* FUNCTION:			GetNameValuePair
*
* ARGUMENTS:		None
*
* RETURNS:			None
*
* DESCRIPTION:		
*              
* RESTRICTIONS:		
*
*********************************************************************/
UINT16 GetNameValuePair(UINT16 wLen, char* pszName, char* pszValue)
{
    UINT8 ch;
	UINT8 i;
	char* pStr;
	char cBuild;
	
	pStr = pszName;
	
    for(i = 0; i < wLen; i++)
	{
		ch = RECEIVE_NETWORK_B();
		
		switch(ch)
		{
		    case ' ':
		        *pStr = 0;
    		    return 0;
		    break;
		    
		    case '&':
		        *pStr = 0;
		        return i;
		    break;
		    
		    case '=':
		        *pStr = 0;
	            pStr = pszValue;
		    break;
		    
		    case '%':
        		ch = RECEIVE_NETWORK_B();
    			cBuild = ch - 0x30;
    			ch = RECEIVE_NETWORK_B();
    			cBuild = (0x10 * cBuild) + (ch - 0x30);
		    break;
		    
		    default:
		        *pStr++ = ch;
		    break;
		}
	}
    return 0;
}


/*********************************************************************
*
* FUNCTION:			FindTableName
*
* ARGUMENTS:		None
*
* RETURNS:			None
*
* DESCRIPTION:		
*              
* RESTRICTIONS:		
*
*********************************************************************/
USR_DYNAMIC_INITIALIZE* FindTableName(char* pszName, USR_DYNAMIC_INITIALIZE *ptrUDI)
{
        
    while(ptrUDI->fieldInitialize[0])
    {
        if(strcmp(ptrUDI->fieldInitialize, pszName) == 0)
        {
            return ptrUDI;
        }
        ptrUDI++;
    }
}

/*********************************************************************
*
* FUNCTION:			XMLEntityOut
*
* ARGUMENTS:		None
*
* RETURNS:			None
*
* DESCRIPTION:		
*              
* RESTRICTIONS:		
*
*********************************************************************/
// To send the internal settings out to XML...
char* XMLEntityOut(char* myData, char* sOut)
{
	char *pcIn = myData;
	char *pcNew = sOut;

	while (*pcIn)
	{
		if (*pcIn == '&')
		{
			*pcNew++ = '&';
			*pcNew++ = 'a';
			*pcNew++ = 'm';
			*pcNew++ = 'p';
			*pcNew++ = ';';
		}
		else
		{
			*pcNew++ = *pcIn;
		}
		pcIn++;
	}
	*pcNew = '\0';
	return sOut;
}
/*********************************************************************
*
* FUNCTION:			MultTable
*
* ARGUMENTS:		None
*
* RETURNS:			None
*
* DESCRIPTION:		
*              
* RESTRICTIONS:		
*
*********************************************************************/
const long MultTable[] =

{
            1,
            10,
            100,
            1000,
            10000,
            100000,
            1000000,
            10000000
};

/*********************************************************************
*
* FUNCTION:			Fn2A - Float to ASCII
*
* ARGUMENTS:		None
*
* RETURNS:			None
*
* DESCRIPTION:		
*              
* RESTRICTIONS:		
*
*********************************************************************/

void Fn2A(char *pcString, float fValue, int nPrecision)
{
    long nWhole, nFrac;
    long nMult;
    unsigned char bSign = FALSE;
    char *pcFormat;


    if (fValue < 0.0)
    {
                bSign = TRUE;
                fValue = -fValue;            
    }
    nMult = MultTable[nPrecision];
    if (fValue < 0.00000000001)
    {
                (void)sprintf(pcString, "0.00");
    }
    else
    {
        fValue += 0.5 / (float)nMult;
        nWhole = (long)fValue;
        nFrac = (long)(fValue * nMult) % nMult;
        if (bSign)
        {
                    pcFormat = "-%ld.%0*ld";
        }
        else
        {
                    pcFormat = "%ld.%0*ld";
        }
        (void)sprintf(pcString, pcFormat, nWhole, nPrecision, nFrac);
    }
}
/*********************************************************************
*
* FUNCTION:			ptrToFloat
*
* ARGUMENTS:		None
*
* RETURNS:			None
*
* DESCRIPTION:		
*              
* RESTRICTIONS:		
*
*********************************************************************/
float ptrToFloat(char *ptrA)
{
    float fTemp = 0.0;
    float fFrac = 1.0;
    float ft1, ft2;
    float fSign = 1.0;
    int iExp;
    BYTE blnDone = FALSE;

    if (*ptrA == '-')
    {
                fSign = -1.0;
                ptrA++;
    }
    while (!blnDone)
    {
      if (*ptrA == '.')
      {
            //hit the decimal
            fFrac = 0.1;
            ptrA++;
      }
      else if (isdigit(*ptrA))
      {
          if (fFrac > 0.9)
          {
                      ft1 = (float)(*ptrA - 0x30);
                      ft2 = (fTemp * 10.0);
                      fTemp = ft1 + ft2;
          }
          else
          {
                      ft1 = (float)(*ptrA - 0x30);
                      ft2 = fFrac * ft1;
                      fTemp += ft2;
                      fFrac /= 10.0;
          }
          ptrA++;
      }
      else if ((*ptrA == 'e') || (*ptrA == 'E'))
      {
        // we have found the exponent
        ptrA++;
        iExp = atoi(ptrA);
        fTemp *= pow(iExp,10);
        blnDone = TRUE;
      }
      else
      {
          //end conversion
          blnDone = TRUE;
      }
    }
    return (fTemp * fSign);
}



//---------------------------------------------------------------------------
// Convierte valor entero a string
//---------------------------------------------------------------------------
//
void SYS_LongToStr (UINT32 val, UINT8 * DataStr )
{
  UINT8 tmp[14];
  INT8 c, i,len;
  UINT8 * pStrVal;
  UINT8   TableSize = sizeof(MultTable)/sizeof(long)-1;

  pStrVal = DataStr;

  for(i=0,len=TableSize; len>=0; len--,i++)
	{
	  tmp[i] = '0';
//	  tmp[i] = ' ';
	  if(val >= MultTable[len])
		{
		  tmp[i] = (char)(val / MultTable[len]);
		  val   -= tmp[i] * MultTable[len];
		  tmp[i] += '0';
		}
	}
  tmp[i] = '\0';
  for(c=0, i=0; tmp[i]; i++,c++){
      pStrVal[c] = tmp[i];
  }
  pStrVal[c] = '\0'; 
	
}
