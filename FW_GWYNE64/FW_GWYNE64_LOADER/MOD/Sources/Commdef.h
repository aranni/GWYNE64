#ifndef CommdefH 
#define CommdefH

#include "MOTTYPES.h"

//-----------------------------------------------------------------------------
//  DEFINICIONES DE MACROS
//-----------------------------------------------------------------------------
#define	FOREVER			  for(;;)
#define	MAXIMO(x,y)		(x>y)?x:y
#define	MINIMO(x,y)		(x<y)?x:y
#define	LOOP(x,y)		  for(x=y; x>0; x--)

//-----------------------------------------------------------------------------
//  DEFINICIONES DE CONSTANTES
//-----------------------------------------------------------------------------
#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif


#define FLASH_UPDATE_FWFILE   1536
#define FLASH_FACTORY_FWFILE  1792

//--MACROS PARA CONTROL DE LEDS------------------------------------------------

#define LED_RED_PORT   PORTA
#define LED_RED_CFG    DDRA
#define LED_RED_MASK   0x80    

#define LED_GREEN_PORT   PTL
#define LED_GREEN_CFG    DDRL				
#define LED_GREEN_MASK   0x04    

#define  LED_RED_ON     {(LED_RED_PORT  |= LED_RED_MASK);\
                         (LED_GREEN_PORT  &= ~(LED_GREEN_MASK));}
                         
#define  LED_GREEN_ON   {(LED_GREEN_PORT  |= LED_GREEN_MASK);\
                         (LED_RED_PORT  &= ~(LED_RED_MASK));}
												
#define  LED_OFF        {(LED_GREEN_PORT  &= ~(LED_GREEN_MASK));\
                         (LED_RED_PORT  &= ~(LED_RED_MASK));}

#define  LED_RED_OFF    (LED_RED_PORT  &= ~(LED_RED_MASK))
#define  LED_RED_TOG    (LED_RED_PORT  ^= LED_RED_MASK)
#define  LED_GREEN_OFF  (LED_GREEN_PORT  &= ~(LED_GREEN_MASK))
#define  LED_GREEN_TOG  (LED_GREEN_PORT  ^= LED_GREEN_MASK)

#define  TMR_WAIT_ON              30000L
#define  TMR_WAIT_OFF             20000L
#define  MAX_RESTORESWITCH_CNTR   200

//--DIRECCIONES DE CAMPOS EN EEPROM--------------------------------------------

//--DIRECCIONES DE CAMPOS EN EEPROM--------------------------------------------

//--Datos de parametrización del canal TCP-IP
#define EE_ADDR_VERS            0
#define EE_ADDR_MAC             1
#define EE_ADDR_IP_ADDR         7
#define EE_ADDR_IP_SUBNET       11
#define EE_ADDR_IP_GATEWAY      15

#define EE_ADDR_DNS_ENABLE      19
#define EE_ADDR_IP_DNS          20

#define EE_ADDR_TCP_INITOUT     24
#define EE_ADDR_TCP_TOUT        26
#define EE_ADDR_TCP_RETRIES     28

//--Información estadística especial.
#define EE_ADDR_RST_FLAG        40
#define EE_ADDR_RST_CNTR        42
#define EE_ADDR_DOG_CNTR        44


//-- Información de configuación específica..
#define EE_ADDR_FW_UPDATE				50				// Flag para control de upgrade de FW
#define EE_ADDR_RUN_CFG         52        // Estado de ejecución de procesos
#define EE_ADDR_TRIAL_CODE      54        // Tipo de versión trial en caso que aplique
#define EE_ADDR_LIMIT_FROMSRL   60        // Desde Serial Nr.
#define EE_ADDR_LIMIT_TOSRL     70        // Hasta Serial Nr.
#define EE_ADDR_LIMIT_MSGQTTY   80        // Limit por cantidad de MSG
#define EE_ADDR_LIMIT_TIME      84        // Limit por tiempo
#define EE_ADDR_LIMIT_OTHER     88        // Limit Otro aun no especificado

// Información sobre la comunicación con el variador Canal_A
#define EE_ADDR_INV_FREC        100       // Consigna de frecuencia
#define EE_ADDR_INV_POLL        104				// ScanRate al variador
#define EE_ADDR_INV_TYPE        106       // Modelo de Variador 
#define EE_ADDR_PORT_CFG        108       // Estado de ejecución de procesos

#define EE_ADDR_SIE_CMDWA       110				// Palabra de control Variador Siemens

// Información sobre la comunicación con el variador Canal_B
#define EE_ADDR_INV_FREC_B      150       // Consigna de frecuencia
#define EE_ADDR_INV_POLL_B      154				// ScanRate al variador
#define EE_ADDR_INV_TYPE_B      156       // Modelo de Variador 
#define EE_ADDR_PORT_CFG_B      158       // Estado de ejecución de procesos

#define EE_ADDR_SIE_CMDWA_B     160				// Palabra de control Variador Siemens

// Datos de la VERSION..
#define EE_ADDR_VER_HWVER       200       // Version de HW
#define EE_ADDR_VER_SERIAL      220       // Numero de serie
#define EE_ADDR_VER_TEXT        240       // Descripcion de usuario

// Constantes almacenadas.
#define EE_ADDR_PAR_RELTX       300        // Relacion de transmision
#define EE_ADDR_PAR_CRESB       304        // Coeficiente de resbalamiento
#define EE_ADDR_PAR_RPMOT       308        // RPM en motor

// Para TransferInfo desde el LOADER
#define EE_ADDR_LDR_INFO        400


//--DIRECCIONES DE CAMPOS EN NVR DEL RTC---------------------------------------
#define NVR_ADDR_LOGSPTR        0x20      // Punteros de logs en FLASH 3x2bytes
#define NVR_ADDR_FIRMA          0x26      // Firma para control de integridad 4bytes
#define NVR_VAL_FIRMA           0xCAFECAFE  



//----ESTRUCTURAS PARA ACCESO AL FW GUARDADO EN FLASH EXTERNA------------
//-----------------------------------------------------------------------
//

// Estructura de datos Fecha/Hora
typedef struct {
	UINT8	Second;
	UINT8	Minute;
	UINT8	Hour;
	UINT8	Day;
	UINT8	Date;
	UINT8	Month;
	UINT8	Year;
} DATE_TIME;


// Estructura de formato de datos para lectura/escritura en FLASH
#define  FLASH_REG_SIZE		256
typedef struct {
	  UINT8	  	  Command;
    UINT8     	AddressPAH;
    UINT8     	AddressPAL;
    UINT8     	AddressBA;
    UINT8     	DontCare_1;
    UINT8     	DontCare_2;
    UINT8     	DontCare_3;
    UINT8     	DontCare_4;
    UINT8		    Data[FLASH_REG_SIZE];
}FLASH_REG_FORMAT;

#define FLASH_HEADER_SIZE   (sizeof(FLASH_REG_FORMAT)-FLASH_REG_SIZE)


// Estructura de formato de datos de Firmware guardado en FLASH externa
#define  FLASH_FWREG_SIZE		250
typedef struct {
	  UINT8	  	            Len;
    unsigned int *far     FirstAddress;
    UINT8		              Data[FLASH_FWREG_SIZE];
    UINT8                 CheckSum;
}FLASH_FWREG_FORMAT;

#define FLASH_FWREG_HDRSIZE   (sizeof(FLASH_FWREG_FORMAT)-FLASH_FWREG_SIZE)


// Estructura de mensaje con informacion de VERSION
#define SIZE_FW_VER     15
#define SIZE_HW_VER     15
#define SIZE_SERIAL     15


// Nuevo Formato para almacenar datos de versión de FW.
typedef struct CFG_FW_VERSION {
        UINT8       FW_Version[SIZE_FW_VER+1];
        UINT8       DateOfCompilation[20];
        UINT8       TimeOfCompilation[10];
        UINT8       FW_Type;
        UINT8       FW_BackWardCompatibility;        
        UINT8       Reservado[52];
}CFG_FW_VERSION;

// Formato del HEADER completo con información del FW en FLASH EXTERNA
typedef struct CFG_FW_HEADER {
        CFG_FW_VERSION  VersionInfo;     // 100 bytes en FLASH del micro
        UINT8           ReservedQttyWord;  
        UINT8           RegInFlashQtty;  
        
        UINT8           TransferInfo;                   // 103
        UINT8           RestrictType;                   // 104
        
        UINT8           RestrictFromSerial[10];
        UINT8           RestrictToSerial[10];
        UINT32          TrialMsgLimit;
        UINT32          TrialTimeLimit;
        UINT32          TrialOtherLimit;                // 136
        
        UINT8           FW_Autor[20];
        UINT8           FW_Info[50];
        UINT8           FW_ReservedSpace[30];           // 236

}CFG_FW_HEADER;


#define   LDR_UPDATE_VALID    0x56
#define   LDR_UPDATE_TEST     0x5A
#define   LDR_UPDATE_LOAD     0x5C
#define   LDR_FACTORY_TEST    0xA0
#define   LDR_FACTORY_ATTEMPT 0x03


#define   FW_RESTRICT_FREE            0xAB              // Sin restricciones
#define   FW_RESTRICT_MSG             0xAC              //
#define   FW_RESTRICT_TIME            0xAD              //
#define   FW_RESTRICT_OTHER           0xAE              //


//-----------------------------------------------------------------------
//

// Estructura de datos de los punteros a los logs grabados en FLASH
typedef struct {
	UINT16	NextToWrite;
	UINT16	NextToRead;
	UINT16	FirstToRead;
} LOGS_PTR;

// Codigos de comandos de operacion recibidos por TCP
#define CFG_TCP_READ          1
#define CFG_TCP_WRITE         2
#define CFG_TCP_RESET         3
#define CFG_TCP_READ_STAT     4
#define CFG_TCP_READ_VER      5
#define CFG_TCP_WRITE_VER     6
#define CFG_TCP_READ_CTES     7
#define CFG_TCP_WRITE_CTES    8
#define CFG_TCP_WRITE_DATE    9

#define CFG_TCP_ACK           10
#define CFG_TCP_NAK           11
#define CFG_TCP_POL           12

#define CFG_TCP_READ_FLASH    15
#define CFG_TCP_WRITE_FLASH   16


//#define WATCHDOG_ENABLE            // habilita WatchDog

#ifdef  WATCHDOG_ENABLE
#define REFRESH_WATCHDOG  {(ARMCOP = 0x55);\
                           (ARMCOP = 0xAA);} 
#else                     
#define REFRESH_WATCHDOG         
#endif

#endif    // CommdefH