
#ifndef sAuxFunH
#define sAuxFunH

//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
const UINT8 SYS_HexChar[] = {     	  
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    'A',
    'B',
    'C',
    'D',
    'E',
    'F'
};


const UINT16 DigWeight[] = {    
  0, 	  
  1,
	10,
	100,
	1000,
	10000,
  };  
   
const UINT16 HexWeight[] = {    
  0x00,
  0x01,
	0x10,
	0x100,
  };

const float SubDecWeight[] = {    
	0.1,
	0.01,
	0.001,
	0.0001,
  };
  
const UINT16 DecWeigh[] = {     	  
  1,
	10,
	100,
	1000,
	10000,
  };
  
  
#define MAX_FLT_DIGLEN   		  5
#define MAX_FLT_DECLEN   		  4
#define MAX_INT_DIGLEN   		  5

//------------------------------------------------------------------------------------
// Estructuras de DATOS
//------------------------------------------------------------------------------------
//

//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//


//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void    SYS_AuxGlobalInit      (void);
float   SYS_StrToFloat         ( UINT8 IntLen, UINT8 DecLen, UINT8 * DataStr );
UINT16  SYS_StrToInt           ( UINT8 IntLen, UINT8 * DataStr );
UINT16  SYS_StrxToInt          ( UINT8 IntLen, UINT8 * DataStr );
void    SYS_IntToStr           (UINT16 val, UINT8 ndigit,  UINT8 * DataStr );


#endif				 // sAuxFunH