
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


// Convierte el código de baudrate en valor del registro de configuración.
#define BAUD_RATE       9600      // ej.
#define ECLK            25000000  // this is BUSCLK
#define BAUD_DIV_NUM    ECLK/16
#define BAUD_DIV        ECLK/16/BAUD_RATE

const UINT16 BaudCode2RegVal[] = {    
  0, 	                            // 0
  (BAUD_DIV_NUM/300),						  // 1
  (BAUD_DIV_NUM/600),						  // 2
  (BAUD_DIV_NUM/1200),						// 3
  (BAUD_DIV_NUM/2400),						// 4
  (BAUD_DIV_NUM/4800),						// 5 
  (BAUD_DIV_NUM/9600),						// 6
  (BAUD_DIV_NUM/19200),						// 7
  (BAUD_DIV_NUM/38400),						// 8
  (BAUD_DIV_NUM/115200),					// 9
  0,                              // 10
  0,                              // 11
  0,                              // 12
  0,                              // 13
  0,                              // 14
  0,                              // 15
  };  


//------------------------------------------------------------------------------------
// Estructuras de DATOS
//------------------------------------------------------------------------------------
//

//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//

#pragma CODE_SEG NON_BANKED

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void    SYS_AuxGlobalInit      (void);
float   SYS_StrToFloat         ( UINT8 IntLen, UINT8 DecLen, UINT8 * DataStr );
UINT16  SYS_StrToInt           ( UINT8 IntLen, UINT8 * DataStr );
UINT16  SYS_StrxToInt          ( UINT8 IntLen, UINT8 * DataStr );
void    SYS_IntToStr           (UINT16 val, UINT8 ndigit,  UINT8 * DataStr );

#pragma CODE_SEG DEFAULT


#endif				 // sAuxFunH