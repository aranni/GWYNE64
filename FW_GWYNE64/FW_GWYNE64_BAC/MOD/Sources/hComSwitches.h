
//------------SWITCHES DE CONFIGURACION DE FW------------------------
//
//********************************************************************
//********************************************************************
// habilita registro de estadísticas del OS
// #define DBG_WATCH_OS                 

// habilita reset por acumulacion de errores
// #define MON_SERIAL_ERR_RESET_ENABLE  

// descomentar para generar un FW original de fábrica..
//#define FW_FACTORY_MODE

// Habilita la generacion de un código con vectores de int. verdaderos..
//#define   OWN_RESET_VECTOR

// Para generar un FW que realice test, y configure inicialmente la EEPROM
//#define   FW_FACTORY_TEST

//#define HW_VERSION_DEF    "GWY-C-001.09.06"
#define HW_SERIAL_DEF     "A-000000.FEB.07"


#define   DNP_MASTER_MODE
//#define   DNP_SLAVE_MODE

//#define   DNP_SLAVE_SIM

#ifdef  DNP_MASTER_MODE
 #define FW_VERSION_DEF    "GWH-C-002.083.D"				// Maestro
#else	    // DNP_MASTER_MODE                        
 #define FW_VERSION_DEF    "GWH-C-E02.076.D"       // Esclavo
#endif    // DNP_MASTER_MODE

//********************************************************************
//********************************************************************

#ifdef  FW_FACTORY_TEST
#define   OWN_RESET_VECTOR
#endif

#ifdef OWN_RESET_VECTOR
#define   FLAG_OWN_RESET    FW_OWN_RESET
#else	  // OWN_RESET_VECTOR	 
#define   FLAG_OWN_RESET    0
#endif  // OWN_RESET_VECTOR

#ifdef FW_FACTORY_MODE
#define FLAG_FACTORY      FW_FACTORY 
#else   // FW_FACTORY_MODE
#define FLAG_FACTORY      FW_UPDATE
#endif  // FW_FACTORY_MODE

#ifdef FW_FACTORY_TEST
#define FLAG_TEST         FW_TEST
#else   // FW_FACTORY_TEST
#define FLAG_TEST         0
#endif  // FW_FACTORY_TEST


#define FW_TYPE_CODE    (FLAG_FACTORY|FLAG_OWN_RESET|FLAG_TEST)

          
// habilita WatchDog
#define WATCHDOG_ENABLE

#ifdef  WATCHDOG_ENABLE
#define REFRESH_WATCHDOG  {(ARMCOP = 0x55);\
                           (ARMCOP = 0xAA);} 
#else                     
#define REFRESH_WATCHDOG         
#endif

#define   DEBUG_MAX_LEVEL   10
#define   DEBUGLEVEL  20
#define   DEBUG_AT(x) (DEBUGLEVEL<=x)

#if       (DEBUGLEVEL <= DEBUG_MAX_LEVEL)
#define   UDP_DEBUG_PRINTFON
#endif

#define FLASH_MAX_LOGADDR	          1500//1000 //4096	// Max versiones viejas = 2048 
#define FLASH_MIN_LOGADDR           1024//500 //2048 

#define FW_WEBDATA_ENABLED

