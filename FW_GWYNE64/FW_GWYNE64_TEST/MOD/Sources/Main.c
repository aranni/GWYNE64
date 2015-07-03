#include "debug.h"
#include "datatypes.h"
#include "timers.h"
#include "system.h"
#include "ethernet.h"
#include "arp.h"
#include "ip.h"
#include "tcp_ip.h"

#include "http_server.h"
#include "dns.h"
#include "pop3_client.h"
#include "smtp_client.h"
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

UINT32 IP_POP3_Server = 0;
UINT8  POP3_Connected = 0;
//==============================================================
void dns_pop3_listener(UINT8 event, UINT32 data)
{
//>>>>>  
if(event == DNS_EVENT_SUCCESS)
    {
    IP_POP3_Server = data;
    }
}
//==============================================================

/* main stuff */
void main(void)
{
  INT16 len;
	UINT8	TmpByte;

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
	CurrentProcessState [PROC_MODBUS]  	      = (EVENT_FUNCTION*)ModbusProc_IDLE;
	CurrentProcessState [PROC_SERIAL] 	      = (EVENT_FUNCTION*)SerialProc_IDLE;	
	CurrentProcessState [PROC_SIEMENS] 	      = (EVENT_FUNCTION*)SiemensProc_IDLE;	
//	CurrentProcessState [PROC_HITACHI] 	      = (EVENT_FUNCTION*)HitachiProc_IDLE;	
	CurrentProcessState [PROC_CONFIG] 	      = (EVENT_FUNCTION*)ConfigProc_IDLE;	
	CurrentProcessState [PROC_RTU] 	          = (EVENT_FUNCTION*)RtuProc_IDLE;	
	CurrentProcessState [PROC_MODRTU] 	      = (EVENT_FUNCTION*)ModRtuProc_IDLE;	
	CurrentProcessState [PROC_IIC] 	          = (EVENT_FUNCTION*)IICProc_IDLE;	
	CurrentProcessState [PROC_SIE_MASTERDRV]  = (EVENT_FUNCTION*)SieMasterDrvProc_IDLE;	
// 	CurrentProcessState [PROC_HIT_SJ300]      = (EVENT_FUNCTION*)HitSJ300Proc_IDLE;	
 	CurrentProcessState [PROC_SIM_SMD]        = (EVENT_FUNCTION*)SimSieMasterDrvProc_IDLE;	

  init_serial_memories();
  eeprom_read_buffer(EE_ADDR_TCP_INITOUT, (UINT8 *)&(CfgMsgInfo.var_TCP_INIT_RETRY_TOUT), 2); 
  eeprom_read_buffer(EE_ADDR_TCP_TOUT, (UINT8 *)&(CfgMsgInfo.var_TCP_DEF_RETRY_TOUT), 2); 
  eeprom_read_buffer(EE_ADDR_TCP_RETRIES, &(CfgMsgInfo.var_TCP_DEF_RETRIES), 1);    

  if (!CfgMsgInfo.var_TCP_DEF_RETRIES) CfgMsgInfo.var_TCP_DEF_RETRIES = 7;
  if (!CfgMsgInfo.var_TCP_INIT_RETRY_TOUT) CfgMsgInfo.var_TCP_INIT_RETRY_TOUT = 100;
  if (!CfgMsgInfo.var_TCP_DEF_RETRY_TOUT) CfgMsgInfo.var_TCP_DEF_RETRY_TOUT = 400;
 
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
    TmpByte = 0x00;
    eeprom_write_buffer(EE_ADDR_RST_FLAG, &TmpByte, 1);     
  } 
  else{											 // Reset por WatchDog..
      StatisticData.OsStat.MsgDogCntr++;
      eeprom_write_buffer(EE_ADDR_DOG_CNTR, (UINT8 *)&(StatisticData.OsStat.MsgDogCntr), 2);     
  }

  // Parametros de los INVERTERS
  eeprom_read_buffer(EE_ADDR_INV_TYPE, (UINT8 *)&SYS_ConfigParam.InvCode, 1); 
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

	TMR_Init();			        // Controlador de timers
	SRL_Init();             // Driver de comunicación serie (SCI1) 
  IIC_Init();             // Init del proceso IIC
  FLA_Init();             // Organización de la memoria FLASH
	DBG_Init();             // Para test y debug de tareas
	MOD_Init();             // Protocol handler  para Modbus TCP
	GIO_Init();             // Inicialización de proceso controlador de I/O
	CFG_Init();             // Inicialización de proceso de configuracion
	
  if (SYS_ConfigParam.RunConfig & CFG_RTU_RUNSTATE_MASK){
    RTU_Init();             // Init. del driver del puerto serie RS232
	  MRT_Init();             // Init del proceso Modbus RTU
  }
#ifdef FW_FACTORY_TEST
  else{
   RTU_Init();             // Init. del driver del puerto serie RS232    
   SimSMD_Init(); 
  }
#endif
  	  
  REFRESH_WATCHDOG;

  switch (SYS_ConfigParam.InvCode){
    
    case (PROC_SIEMENS ):
      PeriodicTimer = 2000;
	    SIE_Init();         // Inicializacion de Proceso de Cx. con variador Siemens MicroMaster
      eeprom_read_buffer(EE_ADDR_SIE_CMDWA, (UINT8 *)(&(pSIE_ConfigParam->CmdWordA)), 2); 
      break;
/*
    case (PROC_HITACHI ):
      PeriodicTimer = 2000;
	    HIT_Init();         // Inicializacion de Proceso de Cx. con variador Hitachi J300
      break;
*/ 
     case (PROC_SIE_MASTERDRV ):
      PeriodicTimer = 2000;
	    SMD_Init();         // Inicializacion de Proceso de Cx. con variador Siemens MasterDrive
      pSMD_ConfigParam->CmdWordA = 0x9C7E;
      break;
/*      
     case (PROC_HIT_SJ300 ):
      PeriodicTimer = 2000;
	    HSJ_Init();         // Inicializacion de Proceso de Cx. con variador Siemens MasterDrive
      break;
*/     
    default:
      PeriodicTimer = 0x00;
      SYS_ConfigParam.InvCode = 0x00;
      break;
  }

  timer_pool_init();			// Inicializar servicios del sistema
	mBufInit ();            // Inicializar buffers descriptores
  EtherInit();            // Inicializar todas las capas de red
  
  // DESPUES de la inicializacion de timers, etc. pueden habilitarse interrupciones   	
    __asm CLI;            // Habilitar Interrupciones

  LED_RED_ON;
  TMR_Wait ( 50);
  LED_OFF;
  TMR_Wait ( 10);
  
  for (TmpByte = 0 ; TmpByte < 4 ; TmpByte++){
  
    LED_GREEN_ON;
    TMR_Wait ( 30);
    LED_OFF;
    TMR_Wait ( 10);
  }							  
  LED_RED_ON;
  TMR_Wait ( 50);
  LED_OFF;

#if USE_EXTBUS
    ExternalBusCfg();
#endif   	
   	arp_init();
   	(void)udp_init();
  	(void)tcp_init();

	  (void)https_init ();
    (void)tftps_init(); 

    /* Note: If enabled, this call initializes the SMTPC protocol */
    /*  smtpc_init ();  */
    /* Note: If enabled, this call initializes the DNS */
    /*  dns_init();   */
    /* Note: If enabled, this call initializes the POP3 protocol */
    /*    pop3c_init(); */

    TCP_Init();     
    UDP_Init(); 
			
	  RTI_Enable ();

    /* Note: If enabled, this call starts an E-mail sending procedure           */
    /* Remember to set the Outgoing mail IP Server and the Server Port number   */
    /* Also, please verify the SMTPC_TOUT parameter (Server Timeout)            */ 
    /* (void)smtpc_connect (0xC0A80101, 25);    */
//>>>>> DNS	
//    (void)get_host_by_name((unsigned char *)"mail.softecmicro.com", dns_pop3_listener);


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//               LAZO INFINITO del PRGRAMA PRINCIPAL...
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
	DEBUGOUT(">>>>>>>>>Entering to MAIN LOOP>>>>>>>>>\n\r");	
	
	FOREVER {

    REFRESH_WATCHDOG;
  
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
      RestoreSwitchCounter = 0x00;
  
	
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
        
    	/* manage arp cache tables */
		  arp_manage();   	
    	/* TCP/IP stack Periodic tasks here... */
  		/* manage opened TCP connections (retransmissions, timeouts,...)*/
		  tcp_poll();
		  https_run ();
		  tftps_run();
      UDP_Loop ();
		
    /* Note: Enable this function if you are using the SMTP protocol */
    /*    smtpc_run();  */

/*      dns_run();
        if(IP_POP3_Server && !POP3_Connected)
            {
            (void)pop3c_connect(IP_POP3_Server, 110);
            POP3_Connected = 1;
            }
        if(IP_POP3_Server && POP3_Connected)
            pop3c_run();  	*/  
		
	  } 
	  else {
	 	   //NO LINK
	  }	 // if (gotlink)
	  	  
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
		EventMsgQueue [EventMsgCtrl.IndexIn].EventMsg = PutEvent;
		EventMsgQueue [EventMsgCtrl.IndexIn].Source   = PutSource;
		EventMsgQueue [EventMsgCtrl.IndexIn].Target   = PutTarget;
		EventMsgQueue [EventMsgCtrl.IndexIn].Param    = PutParam;
		EventMsgCtrl.IndexIn++;
		if (EventMsgCtrl.IndexIn == MAX_EVENTMSG_QUEUE) EventMsgCtrl.IndexIn = 0x00;
		EventMsgCtrl.Counter++;
    if (StatisticData.OsStat.MaxMsgCntr < EventMsgCtrl.Counter)
        StatisticData.OsStat.MaxMsgCntr = EventMsgCtrl.Counter;  
	}
	else
        MON_ResetBoard();    
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

	UINT8	TmpByte;

  CfgMsgInfo.InvCode = PROC_HITACHI;
  eeprom_write_buffer(EE_ADDR_INV_TYPE, &(CfgMsgInfo.InvCode), 1); 
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
  eeprom_write_buffer(EE_ADDR_VER_TEXT,(UINT8*)"Conversor de protocolos", 24); 

	eeprom_write_buffer(EE_ADDR_MAC, (unsigned char *)hard_addr, 6);
	
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
  
  CfgMsgInfo.InvCode    = PROC_SIE_MASTERDRV;
	CfgMsgInfo.ScanRate   = 1000;
  SYS_ConfigParam.InvCode   = CfgMsgInfo.InvCode;
  SYS_ConfigParam.ScanRate  = CfgMsgInfo.ScanRate;
  CfgMsgInfo.SIE_CmdWordA   =  1150;
  TmpByte = 0x00;
  
  eeprom_write_buffer(EE_ADDR_INV_TYPE, &(CfgMsgInfo.InvCode), 1); 
  eeprom_write_buffer(EE_ADDR_INV_POLL, (UINT8 *)&(CfgMsgInfo.ScanRate), 2); 
  eeprom_write_buffer(EE_ADDR_SIE_CMDWA, (UINT8 *)&(CfgMsgInfo.SIE_CmdWordA), 2); 
  eeprom_write_buffer(EE_ADDR_PORT_CFG, &TmpByte, 1); 

  eeprom_write_buffer(EE_ADDR_INV_TYPE_B, &(CfgMsgInfo.InvCode), 1); 
  eeprom_write_buffer(EE_ADDR_INV_POLL_B, (UINT8 *)&(CfgMsgInfo.ScanRate), 2); 
  eeprom_write_buffer(EE_ADDR_SIE_CMDWA_B, (UINT8 *)&(CfgMsgInfo.SIE_CmdWordA), 2); 
  eeprom_write_buffer(EE_ADDR_PORT_CFG_B, &TmpByte, 1); 

// Con este flag se va a indicar si hay algun tipo de restricción de uso
// para el FW. Codigo liberado es con 0xAB (ningun trial)
  TmpByte = FW_RESTRICT_FREE;                 
  eeprom_write_buffer(EE_ADDR_TRIAL_CODE, &TmpByte, 1); 

  // Dejar el FLAG preparado para que al grabar el LOADER cargue en 
  // la FLASH del micro el FW de Fábrica previamente grabado..
  TmpByte = LDR_FACTORY_TEST;
  eeprom_write_buffer(EE_ADDR_FW_UPDATE, &TmpByte, 1);     

  StatisticData.OsStat.MsgRstCntr = 0x00;
  StatisticData.OsStat.MsgDogCntr = 0x00;
  eeprom_write_buffer(EE_ADDR_DOG_CNTR, (UINT8 *)&(StatisticData.OsStat.MsgDogCntr), 2);     
  eeprom_write_buffer(EE_ADDR_RST_CNTR, (UINT8 *)&(StatisticData.OsStat.MsgRstCntr), 2);     


#endif      

  LED_RED_ON;
  TMR_Wait ( 200);
  MON_ResetBoard();      
  
}

