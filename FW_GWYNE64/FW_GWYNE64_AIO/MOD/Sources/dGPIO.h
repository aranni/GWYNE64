
#ifndef dGPIOH
#define dGPIOH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define INP_DIG_PORT    PORTK
#define INP_DIG_CFG     DDRK

#define INP_DIG_MASK_0  0x08
#define INP_DIG_MASK_1  0x04
#define INP_DIG_MASK_2  0x02
#define INP_DIG_MASK_3  0x01
#define INP_DIG_MASK_4  0x10
#define INP_DIG_MASK_5  0x20

#define OUT_DIG_CFG_1   DDRT
#define OUT_DIG_CFG_0   DDRT
#define OUT_DIG_CFG_4   DDRT
#define OUT_DIG_CFG_5   DDRT

#define OUT_DIG_PORT_1  PTT
#define OUT_DIG_PORT_0  PTT
#define OUT_DIG_PORT_4  PTT
#define OUT_DIG_PORT_5  PTT

#define OUT_DIG_PORT_3  PORTK
#define OUT_DIG_PORT_2  PORTK

#define OUT_DIG_CFG_3   DDRK
#define OUT_DIG_CFG_2   DDRK

#define OUT_DIG_MASK_0  0x40
#define OUT_DIG_MASK_1  0x80

#define OUT_DIG_MASK_2  0x80
#define OUT_DIG_MASK_3  0x40
#define OUT_DIG_MASK_4  0x20
#define OUT_DIG_MASK_5  0x10

//#define AAG_INP_BITRES    (UINT16)256 
#define AAG_INP_BITRES    (UINT16)1024 
#define RES_INP_4_20      (UINT16)330

#define AAG_OUT_BITRES    (UINT16)256 
#define AAG_OUT_VREF      (UINT8)5
#define RES_OUT_4_20      (UINT8)200


const float AAG_INP_VREF      = (float)3.3;
const float AAG_3V3_FACTOR_mV = (float)(1000*AAG_INP_VREF) / (float)AAG_INP_BITRES;
const float AAG_Tmul_FACTOR   = (float)(AAG_3V3_FACTOR_mV) / (float)(6.25);
//const float AAG_Tmul_FACTOR   = (float)(0.515625);

//const float AAG_Tsus_FACTOR   = 
const float AAG_Tsus_FACTOR   = (float)(67.84);     //  (float) / (6.25);

const float AAG_10V_FACTOR    = (float)(3*AAG_INP_VREF) / (float)AAG_INP_BITRES;
const float AAG_4_20_FACTOR   = (float)(1000*AAG_10V_FACTOR) / (float)RES_INP_4_20;

const UINT16 AAG_0_1000_FACTOR_div   = 682;
const UINT16 AAG_0_1000_FACTOR_mul   = 1000;
const float AAG_0_1000_FACTOR   = (float)(1000)/(float)(682);
//const float AAG_0_1000_FACTOR   = (float)(1.466276);

const float AAG_4_20_OUT_FACTOR   = (float)((float)(RES_OUT_4_20*AAG_OUT_BITRES) / (float)(1000*AAG_OUT_VREF));
//const float AAG_4_20_OUT_FACTOR   = (float)(10.24);


//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//

GIO_STATE_VAL     InputOutputData;



#define ADC_QUEUE_SIZE      10    // Cantidad de conversiones promediadas en la colas
#define ADC_SAMPLES_AVG     2    // Muestras que se promedian en cada conversion.


//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
UINT16 AAG_INP_A;
UINT16 AAG_INP_B;

UINT16 AAG_QA[ADC_QUEUE_SIZE];
UINT16 AAG_QB[ADC_QUEUE_SIZE];

UINT16 AAG_INP_A_Suma;
UINT16 AAG_INP_B_Suma;


//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
UINT16 GIO_AdcRead   (UINT8 Chan);
void  GIO_GetTemp   (void);
void  GIO_GetAAGInp (void);
void  GIO_SetAAGOut (UINT8 Chan);

UINT8 GIO_GetInpVal  (void);
void GIO_SetOutVal  (UINT8 OutputVal); 
 
void GIO_Init       (void);

#endif			  //  dGPIOH
