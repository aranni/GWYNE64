#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"

#include "Commdef.h"
#include "dGPIO.h"
#include "Global.h"

#pragma MESSAGE DISABLE C5919 //WARNING C5919: Conversion of floating to unsigned int

// ---------------------------------------------------------------------------
// Adc Read routine (10-bit)
// ---------------------------------------------------------------------------
UINT16 GIO_AdcRead (UINT8 Chan)
{
  UINT8 n, CTL5_Val;
  UINT16 NewVal = 0;
  UINT16 average = 0;

  CTL5_Val = (Chan & 0x07);
  CTL5_Val |= (0x80);

  for(n=0; n<8; n++){

//    ATDCTL5 = (Chan & 0x07);        // Seleccionar canal del ADC
    ATDCTL5 = CTL5_Val;               // Seleccionar canal del ADC

    while (!(ATDSTAT1 & 0x01))        // esperar el fin de conversion
        ;
//    NewVal =  ATDDR0H;
//    NewVal <<= 8;
//    NewVal |= ATDDR0L;
    NewVal =  ATDDR0;
    average +=  NewVal;       
    
//    average +=  ATDDR0H;       
    
  }
  return((UINT16)(average/8));
  
}


void GIO_GetTemp (void)
{
  UINT16 TmpVal;

  TmpVal = (UINT16)GIO_AdcRead (0x00);
  InputOutputData.Temperatura = (float)TmpVal; 
  InputOutputData.Temperatura *= AAG_Tmul_FACTOR;
  InputOutputData.Temperatura -= AAG_Tsus_FACTOR;
  return;
  
}


void GIO_SetAAGOut (UINT8 Chan)
{

  float TmpFltVal;
  UINT8 TmpVal;
  
  if (Chan)  
    TmpFltVal = (InputOutputData.AAG_Output_A * AAG_4_20_OUT_FACTOR);  
  else
    TmpFltVal = (InputOutputData.AAG_Output_B * AAG_4_20_OUT_FACTOR);  
  
  TmpVal = (UINT8)TmpFltVal;
   
#ifdef HW_FULL_VERSION
  IIC_DacSetOutput (Chan, TmpVal);
  IIC_DacSetValues ();
#endif
  
  return;
}


UINT8 GIO_GetInpVal() 
{
  UINT8 InputVal;
  
  InputVal = 0x00;
  if (INP_DIG_PORT & INP_DIG_MASK_0)  InputVal |= 0x01;
  if (INP_DIG_PORT & INP_DIG_MASK_1)  InputVal |= 0x02;
  if (INP_DIG_PORT & INP_DIG_MASK_2)  InputVal |= 0x04;
  if (INP_DIG_PORT & INP_DIG_MASK_3)  InputVal |= 0x08;
  if (INP_DIG_PORT & INP_DIG_MASK_4)  InputVal |= 0x10;
  if (INP_DIG_PORT & INP_DIG_MASK_5)  InputVal |= 0x20;
  
  InputVal ^= 0xFF;
//  InverterParam.InpVal = (UINT16)InputVal;
  InputOutputData.DigInpVal = InputVal;
    
  return InputVal;
}

void GIO_SetOutVal (UINT8 OutputVal) 
{

  if (OutputVal & 0x01 )  OUT_DIG_PORT_0 &= ~(OUT_DIG_MASK_0);   
  else                    OUT_DIG_PORT_0 |= OUT_DIG_MASK_0;
  
  if (OutputVal & 0x02 )  OUT_DIG_PORT_1 &= ~(OUT_DIG_MASK_1);
  else                    OUT_DIG_PORT_1 |= OUT_DIG_MASK_1;
  
  if (OutputVal & 0x04 )  OUT_DIG_PORT_2 &= ~(OUT_DIG_MASK_2);   
  else                    OUT_DIG_PORT_2 |= OUT_DIG_MASK_2;
  
  if (OutputVal & 0x08 )  OUT_DIG_PORT_3 &= ~(OUT_DIG_MASK_3);   
  else                    OUT_DIG_PORT_3 |= OUT_DIG_MASK_3;

  if (OutputVal & 0x10 )  OUT_DIG_PORT_4 &= ~(OUT_DIG_MASK_4);   
  else                    OUT_DIG_PORT_4 |= OUT_DIG_MASK_4;
  
  if (OutputVal & 0x20 )  OUT_DIG_PORT_5 &= ~(OUT_DIG_MASK_5);  
  else                    OUT_DIG_PORT_5 |= OUT_DIG_MASK_5;

//  InverterParam.OutVal = (UINT16)OutputVal;
  InputOutputData.DigOutVal = OutputVal;
  return;
  
}

//-----------------------------------------------------------------------------
// Funciones Auxiliares del Proceso
//-----------------------------------------------------------------------------
//
void GIO_Init(void)
{

  DDRL_DDRL2 = 1;

  LED_GREEN_ON;
  LED_RED_OFF;
  
//  *****INP_DIG_PORT = PORTK
//  INP_DIG_PORT = 0X00;
//  INP_DIG_CFG = 0x00;             // INP_PORT como entrada

  PORTK = 0xC0;                     // Las salidas en 1 para apagar los leds
  DDRK  = 0xC0;                     // Entrada PK[0..5]
  PUCR_PUPKE = 1;                   // Habilitar pull ups de PK
    
  PTT   = 0xF0;                     // OUT_PORT en 1
  DDRT  = 0xF0;                     // PT[4..7] como salidas    
  
  GIO_SetOutVal (0x00);
  (void)GIO_GetInpVal ();
    
}

#pragma CODE_SEG DEFAULT


/*

  InverterParam.UltimaFalla = GIO_AdcRead (0x01);
  InverterParam.Potencia = (float)InverterParam.UltimaFalla;
  InverterParam.Potencia *= AAG_10V_FACTOR; 

  InputVal = GIO_AdcRead (0x02);
  InverterParam.PotenciaReal = (float) InputVal;
  InverterParam.PotenciaReal *= AAG_20V_FACTOR; 
*/  




void GIO_GetAAGInp (void)
{
    UINT8   Cntr;
    UINT16  AAG_INP_A_Acc;
    UINT16  AAG_INP_B_Acc;

    AAG_INP_A_Acc  = 0x00;
    AAG_INP_B_Acc  = 0x00;
   
    for (Cntr = 1 ; Cntr < (ADC_QUEUE_SIZE-1) ; Cntr++ ){
   
      AAG_QA[Cntr] = AAG_QA[Cntr-1];
      AAG_QB[Cntr] = AAG_QB[Cntr-1];
    
    }    

//    for (Cntr = 0 ; Cntr < ADC_SAMPLES_AVG ; Cntr++ ){    
//      AAG_INP_A_Acc += GIO_AdcRead (0x01);
//      AAG_INP_B_Acc += GIO_AdcRead (0x02);     
//    }
//    AAG_QA[ADC_QUEUE_SIZE-1]      = (UINT16)(AAG_INP_A_Acc/ADC_SAMPLES_AVG);
//    AAG_QB[ADC_QUEUE_SIZE-1]      = (UINT16)(AAG_INP_B_Acc/ADC_SAMPLES_AVG);
    
    AAG_QA[0] = GIO_AdcRead (0x01);
    AAG_QB[0] = GIO_AdcRead (0x02);

    AAG_INP_A_Suma      = AAG_QA[0];
    AAG_INP_B_Suma      = AAG_QB[0];
   
    for (Cntr = 1 ; Cntr < ADC_QUEUE_SIZE-1 ; Cntr++ )
    {  
      AAG_INP_A_Suma     += AAG_QA[Cntr];
      AAG_INP_B_Suma     += AAG_QB[Cntr];
    
    }
        
    AAG_INP_A      =  (UINT16)(AAG_INP_A_Suma / ADC_QUEUE_SIZE );
    AAG_INP_B      =  (UINT16)(AAG_INP_B_Suma / ADC_QUEUE_SIZE );

    InputOutputData.AAG_Input_A = (float)AAG_INP_A;  
    InputOutputData.AAG_Input_A *= AAG_4_20_FACTOR; 
  
    InputOutputData.AAG_Input_B = (float) AAG_INP_B;
    InputOutputData.AAG_Input_B *= AAG_4_20_FACTOR; 


    InputOutputData.AAG_InpWord_A = (WORD)(InputOutputData.AAG_Input_A *100);
    InputOutputData.AAG_InpWord_B = (WORD)(InputOutputData.AAG_Input_B *100);

    //InputOutputData.AAG_Output_A = InputOutputData.AAG_Input_A;
    
    return;

}

/*
void GIO_GetAAGInp (void)
{
  UINT16 TmpVal;

  TmpVal = GIO_AdcRead (0x01);
  InputOutputData.AAG_Input_A = (float)TmpVal;  
  //InputOutputData.AAG_Input_A *= AAG_0_1000_FACTOR; 
  InputOutputData.AAG_Input_A *= AAG_4_20_FACTOR; 
  
//  InputOutputData.AAG_Input_A *= AAG_10V_FACTOR; 

  TmpVal = GIO_AdcRead (0x02);
  InputOutputData.AAG_Input_B = (float) TmpVal;
  //InputOutputData.AAG_Input_B *= AAG_0_1000_FACTOR; 
  InputOutputData.AAG_Input_B *= AAG_4_20_FACTOR; 
//  InputOutputData.AAG_Input_B *= AAG_10V_FACTOR; 


  InputOutputData.AAG_InpWord_A = (WORD)(InputOutputData.AAG_Input_A *100);
  InputOutputData.AAG_InpWord_B = (WORD)(InputOutputData.AAG_Input_B *100);
  
  return;
  
}
*/