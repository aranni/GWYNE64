#include "debug.h"
#include "datatypes.h"
#include "timers.h"
#include "system.h"
#include "ethernet.h"
#include "arp.h"
#include "ip.h"
#include "tcp_ip.h"
#include <stdio.h>

//#include "http_server.h"
#include "dns.h"
#include "tftps.h"

#include "ne64driver.h"
#include "ne64api.h"
#include "mBuf.h"
#include "ne64config.h"

#include "address.h"
#include "MC9S12NE64.h"

#include "Commdef.h"
#include "Main.h"
#include "Global.h"
#include "ne64debug.h"

#define   OS_MSG_PRIORITY  2

/* Network Interface definition. Must be somewhere so why not here? :-)*/
struct netif localmachine;

extern void RTI_Enable (void);
extern void RTI_Init (void);
//extern void tcp_send_buffer(void);
extern INT8  https_init            (void);
extern void  https_run             (void);


extern tU16 gotxflowc;	// Global Variable For Determination of Flow Control Packets 
                        // are sent in Full Duplex 
extern tU08 gotlink;		// Global Variable For Determination if link is active

#if ZERO_COPY
#else
  tU08 ourbuffer [1518]; // *< Space for packet temporary storage if zero copy not used
#endif

#if USE_SWLED
tU16 LEDcounter=0;
#endif


#pragma CODE_SEG NON_BANKED        

/* main stuff */
void main(void)
{
  INT16 len;
	UINT8	TmpByte;

  UINT8*  pTxBuffer;
  UINT8   bTmpToken; 

  /* System clock initialization */
  CLKSEL=0;
  CLKSEL_PLLSEL = 0;                   /* Select clock source from XTAL */
  PLLCTL_PLLON = 0;                    /* Disable the PLL */
  SYNR = 0;                            /* Set the multiplier register */
  REFDV = 0;                           /* Set the divider register */
  PLLCTL = 192;
  PLLCTL_PLLON = 1;                    /* Enable the PLL */
  while(!CRGFLG_LOCK);                 /* Wait */
  CLKSEL_PLLSEL = 1;                   /* Select clock source from PLL */

  INTCR_IRQEN = 0;                     /* Disable the IRQ interrupt. IRQ interrupt is enabled after CPU reset by default. */

#ifdef  WATCHDOG_ENABLE
  TmpByte = 0x00;
  TmpByte |= COPCTL_CR0_MASK;
  TmpByte |= COPCTL_CR1_MASK;
  TmpByte |= COPCTL_CR2_MASK;
//  CR [0-1-2] = 000 ->   0.6 ms  OSCCLK * 2^14
//  CR [0-1-2] = 010 ->   2.6 ms  OSCCLK * 2^16
//  CR [0-1-2] = 011 ->  10.5 ms  OSCCLK * 2^18
//  CR [0-1-2] = 100 ->  41.9 ms  OSCCLK * 2^20
//  CR [0-1-2] = 101 -> 167.8 ms  OSCCLK * 2^22
//  CR [0-1-2] = 110 -> 335.5 ms  OSCCLK * 2^23
//  CR [0-1-2] = 111 -> 671.0 ms  OSCCLK * 2^24   (pag. 169)
  
  COPCTL = TmpByte;
  ARMCOP = 0x55;
  ARMCOP = 0xAA;
#endif
	
//	  CfgMsgInfo.var_TCP_INIT_RETRY_TOUT = 100;
//    CfgMsgInfo.var_TCP_DEF_RETRY_TOUT = 400;
//    CfgMsgInfo.var_TCP_DEF_RETRIES = 7;    	  
	  
    REFRESH_WATCHDOG;	

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//        Configuracion e Inicializacion de la Maquina de Estados
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  RestoreSwitchCounter = 0x00;        // contador de veces consecutivas que se encuentra
                                      // el switch de configuración, presionado
 
	CurrentMsg.EventMsg = EVT_NONE;
	CurrentMsg.Target 	= 0x00;
	CurrentMsg.Source 	= 0x00;
	CurrentMsg.Param 	= 0x00;

	EventMsgCtrl.Counter 	= 0x00;
	EventMsgCtrl.IndexOut	= 0x00;
	EventMsgCtrl.IndexIn	= 0x00;
  StatisticData.OsStat.MaxMsgCntr = 0x00;

	CurrentProcessState [PROC_DEBUG]   	      = (EVENT_FUNCTION*)DebugProc_IDLE;
	CurrentProcessState [PROC_SCIA] 	        = (EVENT_FUNCTION*)SCIAProc_IDLE;	
	CurrentProcessState [PROC_SCIB] 	        = (EVENT_FUNCTION*)SCIBProc_IDLE;	
	CurrentProcessState [PROC_CONFIG] 	      = (EVENT_FUNCTION*)ConfigProc_IDLE;	
	CurrentProcessState [PROC_IIC] 	          = (EVENT_FUNCTION*)IICProc_IDLE;	 
	CurrentProcessState [PROC_NEXO] 	        = (EVENT_FUNCTION*)NexusProc_IDLE;	 
	
//#if       (FW_APP_BUILT == FW_APP_MODBUS)
#ifdef  MOD_MODBUSTCP_SLAVE_ENABLED
	CurrentProcessState [PROC_MODBUS_SLAVE_TCP]  	      = (EVENT_FUNCTION*)ModbusSlaveTcpProc_IDLE;
#endif

#ifdef  MOD_MODBUSTCP_MASTER_ENABLED
	CurrentProcessState [PROC_MODBUS_MASTER_TCP]  	      = (EVENT_FUNCTION*)ModbusMasterTcpProc_IDLE;
#endif

#ifdef  MOD_UNITELWAY_TCP_ENABLED
	CurrentProcessState [PROC_UNITELWAY_TCP]  	        = (EVENT_FUNCTION*)UnitelwayTcpProc_IDLE;
#endif																

#ifdef  MOD_MODBUSRTU_SLAVE_ENABLED
	CurrentProcessState [PROC_MODBUS_SLAVE_RTU]  	      = (EVENT_FUNCTION*)ModbusSlaveRtuProc_IDLE;
#endif

#ifdef  MOD_MODBUSRTU_AUTOQUERY_ENABLED
	CurrentProcessState [PROC_MODBUS_AUTOQUERY]  	      = (EVENT_FUNCTION*)MAQProc_IDLE;
#endif

#ifdef  MOD_SYMEO_ENABLED
	CurrentProcessState [PROC_SYMEO]  	      = (EVENT_FUNCTION*)SYMProc_IDLE;
#endif

#ifdef  MOD_INPUT_OUTPUT_ENABLED
	CurrentProcessState [PROC_PIO]  	      = (EVENT_FUNCTION*)PioProc_IDLE;
#endif

#if   (FW_APP_BUILT == FW_APP_WEG)
  CurrentProcessState [PROC_SPA]            = (EVENT_FUNCTION*)SPAProc_IDLE;	
 
#elif (FW_APP_BUILT == FW_APP_BAC)
  CurrentProcessState [PROC_BAC]            = (EVENT_FUNCTION*)BACProc_IDLE;	 
#endif  

  for ( TmpByte=1; TmpByte<PROCESS_QTTY ; TmpByte++ ) 
  {
      ProcDebugFlag[TmpByte] = 0x00;
  }

  init_serial_memories();
  eeprom_read_buffer(EE_ADDR_TCP_INITOUT, (UINT8 *)&(CfgMsgInfo.var_TCP_INIT_RETRY_TOUT), 2); 
  eeprom_read_buffer(EE_ADDR_TCP_TOUT, (UINT8 *)&(CfgMsgInfo.var_TCP_DEF_RETRY_TOUT), 2); 
  eeprom_read_buffer(EE_ADDR_TCP_RETRIES, &(CfgMsgInfo.var_TCP_DEF_RETRIES), 1);    

  if (!CfgMsgInfo.var_TCP_DEF_RETRIES) CfgMsgInfo.var_TCP_DEF_RETRIES = 7;
  if (!CfgMsgInfo.var_TCP_INIT_RETRY_TOUT) CfgMsgInfo.var_TCP_INIT_RETRY_TOUT = 100;
  if (!CfgMsgInfo.var_TCP_DEF_RETRY_TOUT) CfgMsgInfo.var_TCP_DEF_RETRY_TOUT = 400;
 
 
// 	eeprom_read_buffer(EE_ADDR_VER_HWVER, HwVer, SIZE_HW_VER); 
//	eeprom_read_buffer(EE_ADDR_VER_SERIAL, HwSerial, SIZE_SERIAL); 
     	                  

	  /* Mostimportant things to do in this function as far as the TCP/IP stack
	  * is concerned is to:
	  *  - initialize some timer so it executes decrement_timers
	  * 	on every 10ms (TODO: Throw out this dependency from several files
	  *	so that frequency can be adjusted more freely!!!)
	  *  - not mess too much with ports allocated for Ethernet controller
	  */
  init();
  RTI_Init();
	SYS_AuxGlobalInit();
         
      
 	// IP address
 	eeprom_read_buffer(EE_ADDR_IP_ADDR, (unsigned char *)&localmachine.localip, 4); 
 	eeprom_read_buffer(EE_ADDR_IP_ADDR, CfgMsgInfo.TcpIP, 4); 
   	
 	// Default gateway
 	eeprom_read_buffer(EE_ADDR_IP_GATEWAY, (unsigned char *)&localmachine.defgw, 4); 
 	eeprom_read_buffer(EE_ADDR_IP_GATEWAY, CfgMsgInfo.TcpGW, 4); 
   	
  // Subnet mask
 	eeprom_read_buffer(EE_ADDR_IP_SUBNET, (unsigned char *)&localmachine.netmask, 4); 
 	eeprom_read_buffer(EE_ADDR_IP_SUBNET, CfgMsgInfo.TcpMask, 4); 

 	// Ethernet (MAC) address 	
 	eeprom_read_buffer(EE_ADDR_MAC, localmachine.localHW, 6);  
  eeprom_read_buffer(EE_ADDR_MAC, CfgMsgInfo.TcpMAC, 6);  
  
    REFRESH_WATCHDOG;
  
  // Contadores de RESET
  eeprom_read_buffer(EE_ADDR_DOG_CNTR,(UINT8 *)&(StatisticData.OsStat.MsgDogCntr), 2); 
  eeprom_read_buffer(EE_ADDR_RST_CNTR,(UINT8 *)&(StatisticData.OsStat.MsgRstCntr), 2); 
  eeprom_read_buffer(EE_ADDR_RST_FLAG, &TmpByte, 1);
  if (TmpByte){								// Si hubo un RESET forzado por acumulacion de errores
    StatisticData.OsStat.WatchRstTmr = TmpByte;
    TmpByte = 0x00;
    eeprom_write_buffer(EE_ADDR_RST_FLAG, &TmpByte, 1);     
  } 
  else{											 // Reset por WatchDog..
      StatisticData.OsStat.MsgDogCntr++;
      eeprom_write_buffer(EE_ADDR_DOG_CNTR, (UINT8 *)&(StatisticData.OsStat.MsgDogCntr), 2);     
  }
  
  REFRESH_WATCHDOG;

  // Parametros de los INVERTERS
  eeprom_read_buffer(EE_ADDR_DBG_LEVEL, (UINT8 *)&SYS_ConfigParam.DebugLevel, 1); 
  eeprom_read_buffer(EE_ADDR_INV_FREC, (UINT8 *)&SYS_ConfigParam.ConsignaFrec, 4); 
  eeprom_read_buffer(EE_ADDR_INV_POLL, (UINT8 *)&SYS_ConfigParam.ScanRate, 2); 
  eeprom_read_buffer(EE_ADDR_PAR_RELTX, (UINT8 *)&SYS_ConfigParam.RelacionTx, 4); 
  eeprom_read_buffer(EE_ADDR_PAR_CRESB, (UINT8 *)&SYS_ConfigParam.CoefResbal, 4); 
  eeprom_read_buffer(EE_ADDR_PAR_RPMOT, (UINT8 *)&SYS_ConfigParam.RpmEnMotor, 4); 

  eeprom_read_buffer(EE_ADDR_RUN_CFG, (UINT8 *)&SYS_ConfigParam.RunConfig, 1); 


#ifdef  FW_FACTORY_MODE
  // el FW de fábrica se auto-valida !!
  eeprom_read_buffer(EE_ADDR_FW_UPDATE, &TmpByte, 1);
  if (TmpByte != LDR_UPDATE_VALID){								
      TmpByte = LDR_UPDATE_VALID;
      eeprom_write_buffer(EE_ADDR_FW_UPDATE, &TmpByte, 1);     
  } 
#endif  
	  
  REFRESH_WATCHDOG;
 
  //SYS_ConfigParam.InvCode = OS_NOT_PROC;

  timer_pool_init();			// Inicializar servicios del sistema
	mBufInit ();            // Inicializar buffers descriptores
  EtherInit();            // Inicializar todas las capas de red
  SYS_MemoInit();         // Inicializa pooles de buffers para comunicaciones.
	TMR_Init();			        // Controlador de timers
  IIC_Init();             // Init del proceso IIC
  FLA_Init();             // Organización de la memoria FLASH
	GIO_Init();             // Inicialización de proceso controlador de I/O
	CFG_Init();             // Inicialización de proceso de configuracion
	//QUE_Init();             // Inicializacion de cola de tokens

//#if       (FW_APP_BUILT == FW_APP_MODBUS)
#ifdef  MOD_MODBUSTCP_SLAVE_ENABLED
  MOD_ModbusSlaveTcpInit();             // Inicialización de proceso MODBUS_TCP_SLAVE
#endif

#ifdef  MOD_MODBUSTCP_MASTER_ENABLED
  MOD_ModbusMasterTcpInit();             // Inicialización de proceso MODBUS_TCP_MASTER
#endif

#ifdef  MOD_UNITELWAY_TCP_ENABLED
  MOD_UnitelwayTcpInit();
#endif

#ifdef  MOD_MODBUSRTU_SLAVE_ENABLED
  MOD_ModbusSlaveRtuInit();             // Inicialización de proceso MODBUS_RTU_SLAVE
#endif

#ifdef  MOD_MODBUSRTU_AUTOQUERY_ENABLED
  MOD_ModbusRtuAutoQueryInit();             // Inicialización de proceso  PROC_MODBUS_AUTOQUERY
#endif

#ifdef  MOD_INPUT_OUTPUT_ENABLED
  PIO_Init();
#endif

#if   (FW_APP_BUILT == FW_APP_WEG)
  SPA_Init();
 
#elif (FW_APP_BUILT == FW_APP_BAC)
  BAC_Init();
#endif  

  NEX_Init();
    
  // DESPUES de la inicializacion de timers, etc. pueden habilitarse interrupciones   	
    __asm CLI;            // Habilitar Interrupciones

  REFRESH_WATCHDOG;

#if USE_EXTBUS
    ExternalBusCfg();
#endif   	
   	arp_init();
   	(void)udp_init();
  	(void)tcp_init();

	  (void)https_init ();
    (void)tftps_init(); 																						

    TCP_InitSrvSockets();     // Inicializar siempre, al menos está el socket para config 
#ifdef   TCP_SLAVE_MODE
    TCP_InitCliSockets(); 
#endif
#ifdef UDP_DEBUG_PRINTFON
    UDP_Init(); 
#endif			
#ifdef UDP_CLIENT_MODE
    UDP_CliInit(); 
#endif			


	DBG_Init();             // Para test y debug de tareas
	
  RTI_Enable ();          //  Habilitar interrupciones..

  LED_RED_ON; TMR_Wait ( 100);  
  LED_OFF;  TMR_Wait ( 300);
  for (TmpByte = 0 ; TmpByte < FW_APP_BUILT ; TmpByte++)
  {
    LED_GREEN_ON; TMR_Wait ( 150);
    LED_OFF; TMR_Wait ( 200);
  }
  LED_RED_ON; TMR_Wait ( 100);
  LED_OFF; TMR_Wait ( 400);


  if ( OK == MemGetBuffer ( (void**)(&pTxBuffer), &bTmpToken)) 
  {
      // LC: LAST RESET CODE, WC:WATCHDOG RESET COUNTER, RC: OVERALL RESET COUNTER
      (void)sprintf((char *)&(((LOG_FORMAT*)pTxBuffer)->Data[0]),"SYS_START LC: %02X, WC: %d, RC: %d, FW:%s - %s - %s \0",
                    StatisticData.OsStat.WatchRstTmr,
                    StatisticData.OsStat.MsgDogCntr,
                    StatisticData.OsStat.MsgRstCntr,
                    FwVersionData.FW_VersionFullName,
                    FwVersionData.DateOfCompilation,
                    FwVersionData.TimeOfCompilation);
      ((LOG_FORMAT*)pTxBuffer)->LogId = LOG_ID_SYS;
      PutEventMsg (EVT_LOG_SAVE, PROC_DEBUG, OS_NOT_PROC, (UINT16)bTmpToken);    
  }


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//               LAZO INFINITO del PRGRAMA PRINCIPAL...
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
	DEBUGOUT(">>>>>>>>>Entering to MAIN LOOP>>>>>>>>>\n\r");	
	
	TmpByte = 0;
	
	FOREVER {
	  
    REFRESH_WATCHDOG;
 	  
    #if USE_SWLED
          UseSWLedRun();
    #endif
    if (gotlink) 
    {	  
   		/* Try to receive Ethernet Frame	*/    	
   		if( NETWORK_CHECK_IF_RECEIVED() == TRUE )	
	    {
            switch( received_frame.protocol)
            {
       				  case PROTOCOL_ARP:
    				      process_arp (&received_frame);
    	    		  	break;
    	    		  	
       			  	case PROTOCOL_IP:
       				  	len = process_ip_in(&received_frame);
      				  	if(len < 0)
         				    break;
         	  		  switch (received_ip_packet.protocol)
  				        {
         					  case IP_ICMP:
    			            process_icmp_in (&received_ip_packet, len);
      						    break;
         				  	case IP_UDP:
                      process_udp_in (&received_ip_packet,len);   							
                      break;
                    case IP_TCP:
          						process_tcp_in (&received_ip_packet, len);				
         						  break;
         					  default:
        						  break;
      				    }
         			    break;
           			    
                default:
       					    break;
            }
            /* discard received frame */    		
   			NETWORK_RECEIVE_END();   			
      }
    }
		  
    TmpByte++;   			
	  if ( TmpByte > OS_MSG_PRIORITY )
	  {	  
  	    TmpByte = 0;	      	    
  	    
        //-------------------------------------------------------------------------
        //- ANALIZAR MENSAJERIA ENTRE PROCESOS
        //-------------------------------------------------------------------------
        //    		
        if ( GetEventMsg () ) {
    			EventInCurrentState = CurrentProcessState [CurrentMsg.Target];
    			while (  ( EventInCurrentState->EventMsg != CurrentMsg.EventMsg ) && 
    		 			 ( EventInCurrentState->EventMsg != EVT_OTHER_MSG ) ){
    				EventInCurrentState ++;
    			}
    			EventFunction = EventInCurrentState->TransitionFunc;
    			FirstEventInCurrentState = CurrentProcessState [CurrentMsg.Target];
    			EventFunction ();
    			CurrentProcessState [CurrentMsg.Target] = FirstEventInCurrentState;
    		}	
    		

        //-------------------------------------------------------------------------
        //- CHEQUEAR PULSADOR DE RESTABLECIMIENTO DE CONFIGURACION
        //-------------------------------------------------------------------------
        //    		
        if((PORTA & 0x01) == 0){      // Resetear configuración.
    	
          RestoreSwitchCounter++;
          if ( RestoreSwitchCounter > MAX_RESTORESWITCH_CNTR ){
            while(!(PORTA & 0x01)){
             REFRESH_WATCHDOG;			  // Recargar contador del watchdog
            }
            RestoreConfig();
          }
        } 
        else
        {      
            RestoreSwitchCounter = 0x00;
        }
    		
        if (gotlink) 
        {	  
            //-------------------------------------------------------------------------
            //- PROCESAR ESTADOS ASOCIADOS A SERVICIOS IP
            //-------------------------------------------------------------------------
            //    		
          	/* manage arp cache tables */
      		  arp_manage();  
      		   	
          	/* TCP/IP stack Periodic tasks here... */
        		/* manage opened TCP connections (retransmissions, timeouts,...)*/
      		  tcp_poll();
        		
            #ifdef FW_WEBDATA_ENABLED		  
            		  https_run ();
            		  tftps_run();
            #endif      
            		  
            #ifdef UDP_DEBUG_PRINTFON
                  UDP_Loop ();
            #endif      

            #ifdef UDP_CLIENT_MODE
                  UDP_CliLoop ();
            #endif      
            		
    	  }        
    		    		
  	  }   // OS_MSG_PRIORITY
    	  	  
	  }		 // FOREVER for (;;)
	
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                      FUNCIONES AUXILIARES del OS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
//-----------------------------------------------------------------------------
// Sacar un mensaje de la cola EventMsgQueue
//-----------------------------------------------------------------------------
//
UINT8 GetEventMsg (void)
{
	if (EventMsgCtrl.Counter) {
		CurrentMsg.EventMsg = EventMsgQueue [EventMsgCtrl.IndexOut].EventMsg;
		CurrentMsg.Source 	= EventMsgQueue [EventMsgCtrl.IndexOut].Source;
		CurrentMsg.Target   = EventMsgQueue [EventMsgCtrl.IndexOut].Target;
		CurrentMsg.Param  	= EventMsgQueue [EventMsgCtrl.IndexOut].Param;
		EventMsgCtrl.IndexOut++;										
		if (EventMsgCtrl.IndexOut == MAX_EVENTMSG_QUEUE) EventMsgCtrl.IndexOut = 0x00;				
     __asm SEI;                         // Disable Interrupts
		EventMsgCtrl.Counter--;							// Un msg menos.
     __asm CLI;                         // Enable Interrupts
		return TRUE;
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
// Guardar un mensaje en la cola EventMsgQueue
//-----------------------------------------------------------------------------
//
void PutEventMsg (UINT8 PutEvent, UINT8 PutTarget, UINT8 PutSource, UINT16 PutParam)
{
	if (EventMsgCtrl.Counter < MAX_EVENTMSG_QUEUE) {
	  // ABR: 15-AGO-2007: por posible pérdida de mensajes, muy esporádicamente..
     __asm SEI;                         // Disable Interrupts
		EventMsgQueue [EventMsgCtrl.IndexIn].EventMsg = PutEvent;
		EventMsgQueue [EventMsgCtrl.IndexIn].Source   = PutSource;
		EventMsgQueue [EventMsgCtrl.IndexIn].Target   = PutTarget;
		EventMsgQueue [EventMsgCtrl.IndexIn].Param    = PutParam;
		EventMsgCtrl.IndexIn++;
		if (EventMsgCtrl.IndexIn == MAX_EVENTMSG_QUEUE) EventMsgCtrl.IndexIn = 0x00;
		EventMsgCtrl.Counter++;
     __asm CLI;                         // Enable Interrupts
    if (StatisticData.OsStat.MaxMsgCntr < EventMsgCtrl.Counter)
        StatisticData.OsStat.MaxMsgCntr = EventMsgCtrl.Counter;  
	}
	else
        MON_ResetBoard(0xFF);    
	return;
}	

void f_Consume (void)
{
	return;
}


//-----------------------------------------------------------------------------
// Reconfiguracion del sistema (al presionar el boton de reset)
//-----------------------------------------------------------------------------
//
void RestoreConfig (void)
{

#ifdef FW_FACTORY_TEST
	UINT8	TmpByte;
#endif 

//  CfgMsgInfo.InvCode = PROC_HITACHI;
  //CfgMsgInfo.InvCode = 0x00;
  //eeprom_write_buffer(EE_ADDR_INV_TYPE, &(CfgMsgInfo.InvCode), 1); 
  SYS_ConfigParam.RunConfig = 0x00;
  eeprom_write_buffer(EE_ADDR_RUN_CFG, &(SYS_ConfigParam.RunConfig), 1); 

//	eeprom_write_buffer(EE_ADDR_MAC, (unsigned char *)hard_addr, 6);
	eeprom_write_buffer(EE_ADDR_IP_ADDR, (unsigned char *)prot_addr, 4);
	eeprom_write_buffer(EE_ADDR_IP_SUBNET, (unsigned char *)netw_mask, 4);
	eeprom_write_buffer(EE_ADDR_IP_GATEWAY, (unsigned char *)dfgw_addr, 4);	    	    
  RestoreSwitchCounter = 0x00;


#ifdef FW_FACTORY_TEST

 	eeprom_write_buffer(EE_ADDR_VER_HWVER, (UINT8*)HW_VERSION_DEF, SIZE_HW_VER); 
  eeprom_write_buffer(EE_ADDR_VER_SERIAL,(UINT8*)HW_SERIAL_DEF, SIZE_SERIAL); 

	eeprom_write_buffer(EE_ADDR_MAC, (unsigned char *)hard_addr, 6);
//	eeprom_write_buffer(EE_ADDR_IP_ADDR, (unsigned char *)prot_addr, 4);
//	eeprom_write_buffer(EE_ADDR_IP_SUBNET, (unsigned char *)netw_mask, 4);
//	eeprom_write_buffer(EE_ADDR_IP_GATEWAY, (unsigned char *)dfgw_addr, 4);	    	    
	
	SYS_ConfigParam.ConsignaFrec  = 0;
	SYS_ConfigParam.RelacionTx    = 0;
	SYS_ConfigParam.CoefResbal    = 0;
	SYS_ConfigParam.RpmEnMotor    = 0;

  eeprom_write_buffer(EE_ADDR_INV_FREC, (UINT8 *)&(SYS_ConfigParam.ConsignaFrec), 4); 
  eeprom_write_buffer(EE_ADDR_PAR_RELTX, (UINT8 *)&SYS_ConfigParam.RelacionTx, 4); 
  eeprom_write_buffer(EE_ADDR_PAR_CRESB, (UINT8 *)&SYS_ConfigParam.CoefResbal, 4); 
  eeprom_write_buffer(EE_ADDR_PAR_RPMOT, (UINT8 *)&SYS_ConfigParam.RpmEnMotor, 4); 


  CfgMsgInfo.var_TCP_DEF_RETRIES = 7;
  CfgMsgInfo.var_TCP_INIT_RETRY_TOUT = 100;
  CfgMsgInfo.var_TCP_DEF_RETRY_TOUT = 400;
  eeprom_write_buffer(EE_ADDR_TCP_INITOUT, (UINT8 *)&(CfgMsgInfo.var_TCP_INIT_RETRY_TOUT), 2); 
  eeprom_write_buffer(EE_ADDR_TCP_TOUT, (UINT8 *)&(CfgMsgInfo.var_TCP_DEF_RETRY_TOUT), 2); 
  eeprom_write_buffer(EE_ADDR_TCP_RETRIES, &(CfgMsgInfo.var_TCP_DEF_RETRIES), 1);    
  
//  CfgMsgInfo.InvCode    = PROC_HITACHI;
//	CfgMsgInfo.RunConfig  = 0x00;
	CfgMsgInfo.ScanRate   = 1000;

//  SYS_ConfigParam.InvCode   = CfgMsgInfo.InvCode;
//  SYS_ConfigParam.RunConfig = CfgMsgInfo.RunConfig;
  SYS_ConfigParam.ScanRate  = CfgMsgInfo.ScanRate;
  CfgMsgInfo.SIE_CmdWordA   =  1150;
  
//  eeprom_write_buffer(EE_ADDR_INV_TYPE, &(CfgMsgInfo.InvCode), 1); 
//  eeprom_write_buffer(EE_ADDR_RUN_CFG, &(SYS_ConfigParam.RunConfig), 1);    
  eeprom_write_buffer(EE_ADDR_INV_POLL, (UINT8 *)&(CfgMsgInfo.ScanRate), 2); 
  eeprom_write_buffer(EE_ADDR_SIE_CMDWA, (UINT8 *)&(CfgMsgInfo.SIE_CmdWordA), 2); 

  // Dejar el FLAG preparado para que al grabar el LOADER cargue en 
  // la FLASH del micro el FW de Fábrica previamente grabado..
  TmpByte = LDR_FACTORY_TEST;
  eeprom_write_buffer(EE_ADDR_FW_UPDATE, &TmpByte, 1);     

#endif      

  LED_RED_ON;
  TMR_Wait ( 200);
  MON_ResetBoard(0xFE);      
  
}

#pragma CODE_SEG DEFAULT

