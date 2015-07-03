
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


// La version nueva de flash tiene 1Mb de capacidad
#define HW_NEW_FLASH

#ifdef  HW_NEW_FLASH

#define FLASH_MIN_LOGADDR           1024
#define FLASH_MAX_LOGADDR	          1524

#define FLASH_MIN_LOG_EVENT_ADDR    2048
#define FLASH_MAX_LOG_EVENT_ADDR	  4095

#else  
                   
#define FLASH_MIN_LOGADDR           613
#define FLASH_MAX_LOGADDR	          1023

#define FLASH_MIN_LOG_EVENT_ADDR    1024
#define FLASH_MAX_LOG_EVENT_ADDR	  1524

#endif

#define FLASH_NULL_PTR							(UINT16)(-1)

#define HW_FULL_VERSION

//------------------LISTA DE APLICACIONES DEFINIDAS PARA GENERAR FIRMWARE------------------//
#define   FW_APP_MODBUS			0X01
#define   FW_APP_SIEMENS    0X02
#define   FW_APP_HITACHI    0X03
#define   FW_APP_HOSTLINK   0X04
#define   FW_APP_USTDII			0X05
#define   FW_APP_SILCON			0X06
#define   FW_APP_DNP3				0X07
#define   FW_APP_BAC				0X08
#define   FW_APP_SPA				0X09
#define   FW_APP_WEG				0X0A
#define   FW_APP_DNPMASTER	0X0B
#define   FW_APP_DNPSLAVE		0X0C
#define   FW_APP_BIO		    0X0D

//------------------LISTA DE SUBAPLICACIONES DEFINIDAS PARA GENERAR FIRMWARE------------------//
#define   FW_SUBAPP_MODBUS_SYMEO			0X01									// SYMEO
#define   FW_SUBAPP_MODBUS_AUTOQUERY	0X02								 	// MODBUS AUTOQUERY
#define   FW_SUBAPP_MODBUS_DUALRTU	  0X03									// DUAL RTU
#define   FW_SUBAPP_MODBUS_RTUWAY			0X04									// UNITELLWAY

#define   FW_SUBAPP_MODBUS_RTUTCP			0X05									// MODBUS MASTER TCP


//---MODO DE RELEASE:------------------------------
//
#define   DBG_LEVEL_1   1
#define   DBG_LEVEL_2   2
#define   DBG_LEVEL_3   3
#define   DBG_LEVEL_4   4
#define   DBG_LEVEL_5   5

#define   DEBUGLEVEL  10   // Nivel de DEBUG habilitado					          
#define   DEBUG_MAX_LEVEL   10
#define   DEBUG_AT(x) (DEBUGLEVEL>=x)

#if       (DEBUGLEVEL)// <= DEBUG_MAX_LEVEL)
  #define   UDP_DEBUG_PRINTFON
  #define   FW_RELEASE_CODE ".D"
#else
  #define   FW_RELEASE_CODE ".F"
#endif

//---NUMERO DE VERSION:------------------------------
// 
#define    FW_VERSION_NOW_NUMBER  "A.04.0006"
//
//---------------------------------------------------

#define    FW_VERSION_DEF         "GWY-C-004.001.D"   // FW DE BASE

#define    FW_VERSION_NOW         FW_VERSION_NOW_NUMBER FW_RELEASE_CODE 


//----------------------------APLICACION DE FIRMWARE ACTUAL -------------------------------//
//******************************************************************************************
//****                                                                                  ****
      #define   FW_APP_BUILT			FW_APP_MODBUS

      #define   FW_SUBAPP_FLAG		FW_SUBAPP_MODBUS_DUALRTU      // DUAL RTU
      //#define   FW_SUBAPP_FLAG		FW_SUBAPP_MODBUS_SYMEO				// SYMEO
      //#define   FW_SUBAPP_FLAG		FW_SUBAPP_MODBUS_AUTOQUERY		// MODBUS AUTOQUERY
      //#define   FW_SUBAPP_FLAG		FW_SUBAPP_MODBUS_RTUWAY		    // UNITELLWAY
      //#define   FW_SUBAPP_FLAG		FW_SUBAPP_MODBUS_RTUTCP      // MODBUS RTU -> MODBUS-TCP
      
//****                                                                                  ****
//******************************************************************************************

#define   DBG_CHECK_APPLICATION_INTEGRITY

#define   DBGFLAG_ACTIVITY_CHECK      0x80
#define   DBGFLAG_ACTIVITY_SIGN_MASK  0x0F
#define   DBGFLAG_ACTIVITY_SIGN       0x0A
#define   MODBUSTCP_BUSY_SUPPORT                // envia excepcion de BUSY, si se està quedando sin buffers.
#define   MODBUSTCP_BUSY_MAXQTTY      10        // cantidad de busy consecutivos antes de resetar

//------------------------DEFINICIONES ADICIONALES SEGUN APLICACION------------------------//
//
#if     (FW_APP_BUILT == FW_APP_MODBUS)		    // MODBUS_TCP_SLAVE <-> MODBUS_RTU_MASTER

 #define    FW_WEBDATA_ENABLED  

 #if    (FW_SUBAPP_FLAG == FW_SUBAPP_MODBUS_DUALRTU)    
    #define    TCP_MASTER_MODE										// TCP_SERVER_SOCKET_502
    #define    MOD_MODBUSTCP_SLAVE_ENABLED        // MODBUS_TCP_SLAVE <-> MODBUS_RTU_MASTER
    #define    MOD_MODBUSRTU_SLAVE_ENABLED        // opcional MODBUS_RTU_SLAVE <-> MODBUS_RTU_MASTER
    #define    FW_VERSION_APP   "GWY-MTCPRT-MBR-"
    
 #elif  (FW_SUBAPP_FLAG == FW_SUBAPP_MODBUS_AUTOQUERY) 
    #define    TCP_MASTER_MODE										// TCP_SERVER_SOCKET_502
    #define    MOD_MODBUSTCP_SLAVE_ENABLED        // MODBUS_TCP_SLAVE <-> MODBUS_RTU_MASTER
    #define    MOD_MODBUSRTU_AUTOQUERY_ENABLED		// habilitar proceso para listado de encuestas periodicas.
    #define    FW_VERSION_APP   "GWY-MTCPAQ-MBR-" 
    
 #elif  (FW_SUBAPP_FLAG == FW_SUBAPP_MODBUS_SYMEO) 
    #define    TCP_MASTER_MODE										// TCP_SERVER_SOCKET_502
    #define    MOD_MODBUSTCP_SLAVE_ENABLED        // MODBUS_TCP_SLAVE <-> MODBUS_RTU_MASTER
    #define    MOD_SYMEO_ENABLED                  // habilitar proceso para interpretar protocolo de Symeo positioning
    #define    UDP_CLIENT_MODE										// UDP_CLIENT_SOCKET_
    #define    FW_VERSION_APP   "GWY-MODTCP-SYM-"

 #elif  (FW_SUBAPP_FLAG == FW_SUBAPP_MODBUS_RTUWAY) 
    #define    TCP_MASTER_MODE										// TCP_SERVER_SOCKET_502
    #define    MOD_MODBUSTCP_SLAVE_ENABLED        // MODBUS_TCP_SLAVE <-> MODBUS_RTU_MASTER
    #define    TCP_UNITELWAY_MODE							    // TCP_SERVER_SOCKET_23
    #define    MOD_UNITELWAY_TCP_ENABLED          // UNITELWAY_TCP <-> UNITELWAY_485 
    #define    FW_VERSION_APP   "GWY-MTCPRT-UNI-"

 #elif  (FW_SUBAPP_FLAG == FW_SUBAPP_MODBUS_RTUTCP) 
    #define    TCP_SLAVE_MODE										  // TCP_CLIENT_SOCKET
    #define    MOD_MODBUSTCP_MASTER_ENABLED       // MODBUS_TCP_MASTER <-> MODBUS_RTU_SLAVE
    #define    MOD_MODBUSRTU_SLAVE_ENABLED        // opcional MODBUS_RTU_SLAVE <-> MODBUS_RTU_MASTER
    #define    FW_VERSION_APP   "GWY-MODRTU-TCP-"

 #else
    #define    FW_VERSION_APP   "GWY-MODTCP-MBR-"
 #endif 
  
#elif  (FW_APP_BUILT == FW_APP_SPA)			      // SPACOM_ABB <-> MODBUS_TCP_SLAVE + MODBUS_RTU_SLAVE
 #define    TCP_MASTER_MODE										// TCP_SERVER_SOCKET_502
 #define    MOD_MODBUSTCP_SLAVE_ENABLED       // SPACOM_ABB <-> MODBUS_TCP_SLAVE 
 #define    MOD_MODBUSRTU_SLAVE_ENABLED       // SPACOM_ABB <-> MODBUS_RTU_SLAVE
 #define    FW_WEBDATA_ENABLED
 #define    FW_VERSION_APP   "GWY-MODTCP-MBR-SPA-" 
 
#elif  (FW_APP_BUILT == FW_APP_BIO)			      // BIOMETRIC_ANVIZ + IO <-> UDP_MESSAGE + LOG_REPORT
 #define    UDP_CLIENT_MODE										// UDP_CLIENT_SOCKET_
 #define    MOD_INPUT_OUTPUT_ENABLED					// INPUT OUTPUT MANAGER
 #define    FW_WEBDATA_ENABLED
 #define    FW_VERSION_APP   "GWY-LOGUDP-BIO-" 
 
#elif  (FW_APP_BUILT == FW_APP_BAC)			      // BAC_ALSTON <-> MODBUS_TCP_SLAVE 
 #define    TCP_MASTER_MODE                   // TCP_SERVER_SOCKET_502
 #define    MOD_MODBUSTCP_SLAVE_ENABLED				// BAC_ALSTON <-> MODBUS_TCP_SLAVE 
 #define    FW_WEBDATA_ENABLED
 #define    FW_VERSION_APP   "GWY-MODTCP-BAC-" 
 
#elif  (FW_APP_BUILT == FW_APP_USTDII)			  // USTDII (SENSOR DE NIVEL) <-> API_DB_MYSQL
 #define    TCP_SLAVE_MODE
 #define    FW_WEBDATA_ENABLED
 #define    FW_VERSION_APP   "GWY-WEBAPI-MTS-" 
 
#elif  (FW_APP_BUILT == FW_APP_HOSTLINK)      // HOSTLINK (OMRON) <-> API_DB_MYSQL
 #define    TCP_SLAVE_MODE
 #define    FW_WEBDATA_ENABLED
 #define    FW_VERSION_APP   "GWY-WEBAPI-HST-" 

#elif  (FW_APP_BUILT == FW_APP_DNPMASTER)     // DNP3 serial slave <-> DNP TCP MASTER
 #define    TCP_MASTER_MODE
 #define    FW_VERSION_APP   "GWY-DNPTCP-DNP-" 

#elif  (FW_APP_BUILT == FW_APP_DNPSLAVE)     // DNP3 serial master <-> DNP TCP SLAVE
 #define    TCP_SLAVE_MODE
 #define    FW_VERSION_APP   "GWY-DNPTCP-DNP-" // SLAVE ?
 
#else
 #error Falta definir una aplicacion para generar (ver hComSwitches.h)
#endif  

#define FW_VERSION_FULL   FW_VERSION_APP FW_VERSION_NOW 
//-----------------------------------------------------------------------------------------//
