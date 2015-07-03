#include "MC9S12NE64.h"
#include "datatypes.h"

#include "Commdef.h"
#include "sAuxFun.h"
#include "global.h"


// ---------------------------------------------------------------------------
// Inicializacion de puertos y registros GLOBALES
// ---------------------------------------------------------------------------
//
void SYS_AuxGlobalInit(void)
{

    PORTA = 0x00;                       // Sets Port A data to 0x00
    DDRA = 0x00;                        // Sets Port A as input

    LED_RED_CFG  |= LED_RED_MASK;				// Puerto como salida
    LED_GREEN_CFG  |= LED_GREEN_MASK;		// Puerto como salida
    
// A/D Converters (Potentiometer and NTC input)
    ATDCTL3 = 0x08;                     // One conversion per sequence
    ATDCTL4 |= 0x83;                    // Sets A/D conversion to 8 bit resolution
    ATDCTL2 = 0x80;                     // Normal ATD functionality
    
// Timer configurated as output compare
    TIOS = 0x30 | 0x80;
    TSCR2 = 0x09;   

    TC7 = 12500;     // 1Kz
    TC4 = 0xFFFF;
    TC5 = 0xFFFF;
    TCNT = 0;
    TSCR1 = 0x80;	  
    TIE |= 0x80;
        
    return;
}



//---------------------------------------------------------------------------
// Convierte valor en string a flotante
//---------------------------------------------------------------------------
//
float SYS_StrToFloat ( UINT8 IntLen, UINT8 DecLen, UINT8 * DataStr ) {
  UINT8   CharCntr;
  UINT16  TmpDigitVal;
  float   TmpFloatVal;
  float   TmpFloatDec;
  UINT8 * pStrVal;
  
  if ( IntLen > MAX_FLT_DIGLEN ) return 0;
  if ( DecLen > MAX_FLT_DECLEN ) return 0;

  pStrVal = DataStr;
  TmpFloatVal = 0x00;
  
  for (CharCntr = IntLen ; CharCntr > 0 ; CharCntr--){
    TmpDigitVal = (*pStrVal) - '0';
    TmpDigitVal *= DigWeight[CharCntr]; 
    TmpFloatVal += TmpDigitVal;
    pStrVal++;    
  }
  
//  pStrVal++;              // si tiene '.' decimal
  CharCntr = 0;
  while (CharCntr < DecLen){
    TmpDigitVal = (*pStrVal) - '0';
    TmpFloatDec = TmpDigitVal * SubDecWeight[CharCntr];
    TmpFloatVal += TmpFloatDec;
    pStrVal++;   
    CharCntr++;     
  }

  return TmpFloatVal;

}


//---------------------------------------------------------------------------
// Convierte valor en string a entero
//---------------------------------------------------------------------------
//
UINT16 SYS_StrToInt ( UINT8 IntLen, UINT8 * DataStr ) {
  UINT8   CharCntr;
  UINT16  TmpDigitVal;
  UINT16  TmpIntVal;
  UINT8 * pStrVal;
  
  if ( IntLen > MAX_INT_DIGLEN ) return 0;

  pStrVal = DataStr;
  TmpIntVal = 0x00;
  
  for (CharCntr = IntLen ; CharCntr > 0 ; CharCntr--){
    TmpDigitVal = (*pStrVal) - '0';
    TmpDigitVal *= DigWeight[CharCntr]; 
    TmpIntVal += TmpDigitVal;
    pStrVal++;    
  }
  
  return TmpIntVal;
}


//---------------------------------------------------------------------------
// Convierte valor en string (con caracteres hexa) a entero
//---------------------------------------------------------------------------
//
UINT16 SYS_StrxToInt ( UINT8 IntLen, UINT8 * DataStr ) {
  UINT8   CharCntr;
  UINT16  TmpDigitVal;
  UINT16  TmpIntVal;
  UINT8 * pStrVal;
  
  if ( IntLen > MAX_INT_DIGLEN ) return 0;

  pStrVal = DataStr;
  TmpIntVal = 0x00;
  
  for (CharCntr = IntLen ; CharCntr > 0 ; CharCntr--){
    TmpDigitVal = (*pStrVal);
    if (TmpDigitVal <= '9') 
      TmpDigitVal -= '0';
    else{
      TmpDigitVal &= 0xFFDF;    // forzar mayusculas
      TmpDigitVal -= ('A' - 10);
    }
    TmpDigitVal *= HexWeight[CharCntr]; 
    TmpIntVal += TmpDigitVal;
    pStrVal++;    
  }
  
  return TmpIntVal;
}


//---------------------------------------------------------------------------
// Convierte valor entero a string
//---------------------------------------------------------------------------
//
void SYS_IntToStr (UINT16 val, UINT8 ndigit,  UINT8 * DataStr )
{
  UINT8 tmp[14];
  INT8 c, i,len;

  UINT8 * pStrVal;
  
  if ( ndigit > MAX_FLT_DIGLEN ) return;

  pStrVal = DataStr;

  for(i=0,len=sizeof(DecWeigh)/sizeof(UINT16)-1; len>=0; len--,i++)
	{
	tmp[i] = '0';
	if(val >= DecWeigh[len])
		{
		tmp[i] = (char)(val / DecWeigh[len]);
		val   -= tmp[i] * DecWeigh[len];
		tmp[i] += '0';
		}
	}
  tmp[i] = '\0';
  for(c=0, i-=ndigit; tmp[i]; i++,c++)
	  pStrVal[c] = tmp[i];
	
}

