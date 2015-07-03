
#ifndef pInputOutputH
#define pInputOutputH

//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define PIO_PERIODIC_TIMERVAL      100 			// periodo de actualizacion de valores (mseg)
#define PIO_PERIODIC_REPORT        3000	    // periodo de reportes de estaado (en unidades de PIO_PERIODIC_TIMERVAL ) 
#define MAX_DIO_QTTY               6        // Cantidad de IO digitales   
       

#define CFG_DIO_TRIGGER_MASK            0x0F     //    
#define CFG_DIO_TRIGGER_DISABLED        0x00     // configuracion entrada deshabilitada
#define CFG_DIO_TRIGGER_LEVEL           0x01     // configuracion entrada por nivel 
#define CFG_DIO_TRIGGER_PSLOPE          0x02     // configuracion entrada por flanco positivo 
#define CFG_DIO_TRIGGER_NSLOPE          0x04     // configuracion entrada por flanco negativo

#define CFG_DIO_REPORT_MASK             0x30     //    
#define CFG_DIO_REPORT_EVENT            0x10     // reporta cambios como eventos para guardar en FLASH
#define CFG_DIO_REPORT_MESSAGE          0x20     // reporta cambios como mensaje a otro proceso

#define CFG_DIO_REPORT_PERIODIC_MASK    0xC0     // reporta cambios periodicamente
#define CFG_DIO_REPORT_PERIODIC_EVENT   0x40     // reporta estados como eventos para guardar en FLASH
#define CFG_DIO_REPORT_PERIODIC_MESSAGE 0x80     // reporta estados como mensaje a otro proceso


//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//

typedef union uGPIO_STATE_VAL
{
  	UINT8	  bIoByteVal;
    struct {
    		UINT8 	dio_1   :1;				// digital io 1
    		UINT8 	dio_2   :1;				// digital io 2
    		UINT8 	dio_3   :1;				// digital io 3
    		UINT8 	dio_4   :1;				// digital io 4
    		UINT8 	dio_5   :1;				// digital io 5
    		UINT8 	dio_6   :1;				// digital io 6
    		UINT8	  notUsed :2;
    }IoBitVal;
}GPIO_STATE_VAL;


typedef struct _GPIO_CTRL_VAL 
{
    UINT8            CfgIo[MAX_DIO_QTTY];
     
    UINT16  PeriodSetPoint;
    UINT16  PeriodCounter;
    
    UINT8   CurrentVal;
    UINT8   PreviousVal;
    UINT8   ChangeReport;
    
} GPIO_CTRL_VAL,*LPGPIO_CTRL_VAL;


UINT8           GpioTimerId;
GPIO_CTRL_VAL   GpioControl;

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void f_PioIdleTimeout (void);

void  PIO_Init(void);

#endif		//pInputOutputH
