#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"

#include "Commdef.h"
#include "pConfig.h"
#include "Global.h"

#pragma CONST_SEG  VERSION_ROM

const struct CFG_FW_VERSION FwVersionData = {

  FW_VERSION_DEF,
  __DATE__,
  __TIME__,
  FW_TYPE_CODE,
    'B', 
  FW_VERSION_FULL,
      
};
  
#pragma CONST_SEG DEFAULT
  
///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_CONFIG
///////////////////////////////////////////////////////////////////////////////
//
const EVENT_FUNCTION	ConfigProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_ConfigIdleTimeout	},
{EVT_CFG_RXMSG 	      , f_ConfigRxCfgFrame  },
{EVT_OTHER_MSG     		, f_Consume           }};


///////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_CONFIG
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// Timeout recibido...
//-----------------------------------------------------------------------------
//
void f_ConfigIdleTimeout (void)
{
  if (TCP_SendCfgBuff()){				  // Error en envio de respuesta
    // Error en reintento, acciones correctivas
    DummyVar = 0x00;
  }
	return;
}

//-----------------------------------------------------------------------------
// Analizar mensaje de configuracion recibido por canal TCP.
//-----------------------------------------------------------------------------
//
void f_ConfigRxCfgFrame (void)
{
  UINT8   TmpWriteByte;
  
	BYTE    tmpbuf[100];
  UINT8*  pReadLogBuffer;
  UINT8   bLogToken; 
  UINT16  wLogIdx;
 
  switch (CfgMsgFrame.OpCode){
 
    case CFG_TCP_READ:
      CfgMsgInfo.DebugLevel  = SYS_ConfigParam.DebugLevel;
      CfgMsgInfo.ScanRate     = SYS_ConfigParam.ScanRate;
      CfgMsgInfo.RunConfig    = SYS_ConfigParam.RunConfig;
//      CfgMsgInfo.SIE_CmdWordA = SIE_ConfigParam.CmdWordA;
      //eeprom_read_buffer(EE_ADDR_SIE_CMDWA, (UINT8 *)&(CfgMsgInfo.SIE_CmdWordA), 2); 
      (void)memcpy ((UINT8 *)(&(CfgMsgFrame.Data)),
              (UINT8 *)(&CfgMsgInfo),
              sizeof(CFG_MSG_INFO));        
      CfgMsgFrame.Len = sizeof(CFG_MSG_INFO);      
      break;    

    case CFG_TCP_READ_DNP:
      eeprom_read_buffer ( EE_ADDR_DNP_MASTER, 
                           (UINT8 *)(&((CFG_MSG_DNPINFO *)(CfgMsgFrame.Data))->AddrMaster), 2); 
      eeprom_read_buffer ( EE_ADDR_DNP_SLAVE, 
                           (UINT8 *)(&((CFG_MSG_DNPINFO *)(CfgMsgFrame.Data))->AddrSlave), 2); 
      CfgMsgFrame.Len = sizeof(CFG_MSG_DNPINFO);      
      break;      


    case CFG_TCP_READ_CTES:
      (void)memcpy ((UINT8 *)(&((CFG_MSG_CTES *)(CfgMsgFrame.Data))->ConsignaFrec),
              (UINT8 *)(&(SYS_ConfigParam.ConsignaFrec)),
              4*sizeof(float));  
     	eeprom_read_buffer(EE_ADDR_VER_TEXT,
     	                  (UINT8 *)(((CFG_MSG_CTES *)(CfgMsgFrame.Data))->UsrText), 30); 
      CfgMsgFrame.Len = sizeof(CFG_MSG_CTES);    
      break;    
      
 
    case CFG_TCP_READ_STAT:
      IIC_RtcGetDateTime ();
      (void)memcpy ((UINT8 *)(&(CfgMsgFrame.Data)),
                    (UINT8 *)(&StatisticData),
                     sizeof(STATISTIC_MSG));  
      CfgMsgFrame.Len = sizeof(STATISTIC_MSG);      
      break;    
      

    case CFG_TCP_READ_VER:
      (void)memcpy ((UINT8 *)(&((CFG_MSG_VERSION *)(CfgMsgFrame.Data))->FW_VersionHdr), (UINT8*)(&FwVersionData), sizeof(CFG_FW_VERSION));  
     	eeprom_read_buffer(EE_ADDR_VER_HWVER, 
     	                  (UINT8 *)(&((CFG_MSG_VERSION *)(CfgMsgFrame.Data))->HW_Version),
     	                   SIZE_HW_VER); 
     	eeprom_read_buffer(EE_ADDR_VER_SERIAL,
     	                  (UINT8 *)(&((CFG_MSG_VERSION *)(CfgMsgFrame.Data))->Serial),
     	                   SIZE_SERIAL); 
      CfgMsgFrame.Len = sizeof(CFG_MSG_VERSION);
      break;    
  

    case CFG_TCP_WRITE_VER:
      if ( (((CFG_MSG_VERSION *)(CfgMsgFrame.Data))->KeyCode) != VER_KEY_CODE) break; 
              
     	eeprom_write_buffer(EE_ADDR_VER_HWVER,
     	                   (UINT8 *)(&((CFG_MSG_VERSION *)(CfgMsgFrame.Data))->HW_Version),
     	                    SIZE_HW_VER); 
    	eeprom_write_buffer(EE_ADDR_VER_SERIAL,
     	                   (UINT8 *)(&((CFG_MSG_VERSION *)(CfgMsgFrame.Data))->Serial),
     	                    SIZE_SERIAL); 
      StatisticData.OsStat.MsgRstCntr = 0x00;
      StatisticData.OsStat.MsgDogCntr = 0x00;
      eeprom_write_buffer(EE_ADDR_DOG_CNTR, (UINT8 *)&(StatisticData.OsStat.MsgDogCntr), 2);     
      eeprom_write_buffer(EE_ADDR_RST_CNTR, (UINT8 *)&(StatisticData.OsStat.MsgRstCntr), 2);     
      CfgMsgFrame.Len = 0;
      break; 
      

    case CFG_TCP_WRITE_CTES:
      (void)memcpy ((UINT8 *)(&(SYS_ConfigParam.ConsignaFrec)),
                    (UINT8 *)(&((CFG_MSG_CTES *)(CfgMsgFrame.Data))->ConsignaFrec),
                    (4 * sizeof(float)));  
          
      eeprom_write_buffer(EE_ADDR_INV_FREC, (UINT8 *)&(SYS_ConfigParam.ConsignaFrec), 4); 
      eeprom_write_buffer(EE_ADDR_PAR_RELTX, (UINT8 *)&SYS_ConfigParam.RelacionTx, 4); 
      eeprom_write_buffer(EE_ADDR_PAR_CRESB, (UINT8 *)&SYS_ConfigParam.CoefResbal, 4); 
      eeprom_write_buffer(EE_ADDR_PAR_RPMOT, (UINT8 *)&SYS_ConfigParam.RpmEnMotor, 4); 

     	eeprom_write_buffer(EE_ADDR_VER_TEXT,
     	                  (UINT8 *)(((CFG_MSG_CTES *)(CfgMsgFrame.Data))->UsrText), 30); 
      CfgMsgFrame.Len = 0;
      break;  


    case CFG_TCP_WRITE_DATE:    
      (void)memcpy ((UINT8 *)(&(StatisticData.CurrentDateTime.Second)),
                    (UINT8 *)(&((DATE_TIME *)(CfgMsgFrame.Data))->Second),
                    (sizeof(DATE_TIME)));  
      IIC_RtcSetDateTime ();
     
      CfgMsgFrame.Len = 0;
      break;  
              

    case CFG_TCP_WRITE:
      (void)memcpy ((UINT8 *)(&(CfgMsgInfo)),(UINT8 *)(&(CfgMsgFrame.Data)),
                    (sizeof(CFG_MSG_INFO)));  
      eeprom_write_buffer(EE_ADDR_DBG_LEVEL, &(CfgMsgInfo.DebugLevel), 1); 
      SYS_ConfigParam.DebugLevel = CfgMsgInfo.DebugLevel;
	    eeprom_write_buffer(EE_ADDR_IP_ADDR, CfgMsgInfo.TcpIP, 4);
	    eeprom_write_buffer(EE_ADDR_IP_SUBNET, CfgMsgInfo.TcpMask, 4);
	    eeprom_write_buffer(EE_ADDR_IP_GATEWAY, CfgMsgInfo.TcpGW, 4);
	    eeprom_write_buffer(EE_ADDR_MAC, CfgMsgInfo.TcpMAC, 6);
      eeprom_write_buffer(EE_ADDR_RUN_CFG, &(CfgMsgInfo.RunConfig), 1); 

      //CfgMsgInfo.var_TCP_INIT_RETRY_TOUT /= 100; 
      
      eeprom_write_buffer(EE_ADDR_TCP_INITOUT, (UINT8 *)&(CfgMsgInfo.var_TCP_INIT_RETRY_TOUT), 2); 
      eeprom_write_buffer(EE_ADDR_TCP_TOUT, (UINT8 *)&(CfgMsgInfo.var_TCP_DEF_RETRY_TOUT), 2); 
      eeprom_write_buffer(EE_ADDR_TCP_RETRIES, &(CfgMsgInfo.var_TCP_DEF_RETRIES), 1);    
      if (!CfgMsgInfo.var_TCP_DEF_RETRIES) CfgMsgInfo.var_TCP_DEF_RETRIES = 7;
      if (!CfgMsgInfo.var_TCP_INIT_RETRY_TOUT) CfgMsgInfo.var_TCP_INIT_RETRY_TOUT = 100;
      if (!CfgMsgInfo.var_TCP_DEF_RETRY_TOUT) CfgMsgInfo.var_TCP_DEF_RETRY_TOUT = 400;
  
			SYS_ConfigParam.ScanRate     = CfgMsgInfo.ScanRate;
			SYS_ConfigParam.RunConfig    = CfgMsgInfo.RunConfig;
     
      eeprom_write_buffer(EE_ADDR_INV_POLL, (UINT8 *)&(CfgMsgInfo.ScanRate), 2); 
      //eeprom_write_buffer(EE_ADDR_SIE_CMDWA, (UINT8 *)&(CfgMsgInfo.SIE_CmdWordA), 2); 

      (void)memcpy ((UINT8 *)(&(CfgMsgFrame.Data)),
                    (UINT8 *)(&CfgMsgInfo),
                     sizeof(CFG_MSG_INFO));        
      CfgMsgFrame.Len = sizeof(CFG_MSG_INFO);
      break;    
      
    case CFG_TCP_WRITE_DNP:
      eeprom_write_buffer ( EE_ADDR_DNP_MASTER, 
                           (UINT8 *)(&((CFG_MSG_DNPINFO *)(CfgMsgFrame.Data))->AddrMaster), 2); 
      eeprom_write_buffer ( EE_ADDR_DNP_SLAVE, 
                           (UINT8 *)(&((CFG_MSG_DNPINFO *)(CfgMsgFrame.Data))->AddrSlave), 2); 

      CfgMsgFrame.Len = sizeof(CFG_MSG_DNPINFO);
      //ABR_alerta.. TEST
      //MOD_ModbusRtuAutoQueryResetBanks();
      break;      

       
    case CFG_TCP_GET_IOVALUES:
      (void)memcpy ((UINT8 *)(&(CfgMsgFrame.Data[0])),
              (UINT8 *)(&(InputOutputData)),
              sizeof(GIO_STATE_VAL));  
      CfgMsgFrame.Len = sizeof(GIO_STATE_VAL);        
      break;
      
      
    case CFG_TCP_SET_IOVALUES:
      (void)memcpy ( (UINT8 *)(&(InputOutputData)),
                     (UINT8 *)(&(CfgMsgFrame.Data[0])),              
                      sizeof(GIO_STATE_VAL));  
      GIO_SetOutVal ( ((GIO_STATE_VAL *)(&(CfgMsgFrame.Data[0])))->DigOutVal );
      IIC_DacSetValues();
      CfgMsgFrame.Len = sizeof(GIO_STATE_VAL);       
      break;

  
    case CFG_TCP_RESET:
      MON_ResetBoard(0xF1);    
        __asm SEI;                         // Deshabilitar Interrupciones (RESET)
      FOREVER;    
      break;

      
    case CFG_TCP_POL:
      CfgMsgFrame.OpCode = CFG_TCP_ACK;
      CfgMsgFrame.Len = 0;
      break;
    

    case CFG_TCP_WRITE_FLASH:
      FLA_SaveReg (CfgMsgFrame.Parameter, (UINT16)CfgMsgFrame.Len, (UINT8*) &(CfgMsgFrame.Data[0]));
      CfgMsgFrame.Len = 0;
      break;

    case CFG_TCP_WRITE_FLASH256:
      FLA_SaveReg (CfgMsgFrame.Parameter, (UINT16)256, (UINT8*) &(CfgMsgFrame.Data[0]));
      CfgMsgFrame.Len = 0;
      break;

    case CFG_TCP_READ_FLASH:
      FLA_ReadReg (CfgMsgFrame.Parameter, (UINT16)(CfgMsgFrame.AuxParam), (UINT8*) &(CfgMsgFrame.Data[0]));
      CfgMsgFrame.Len = (UINT8)(CfgMsgFrame.AuxParam);
      break;

    case CFG_TCP_READ_EEPROM:
     	eeprom_read_buffer(CfgMsgFrame.Parameter,
     	                  (UINT8*) &(CfgMsgFrame.Data[0]),
     	                  (UINT8)(CfgMsgFrame.AuxParam) ); 
    
      CfgMsgFrame.Len = (UINT8)(CfgMsgFrame.AuxParam);
      break;


    case CFG_TCP_GET_SCICONFIG:
     	eeprom_read_buffer(EE_ADDR_CFG_SCIA,
     	                  (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIA_ConfigCode),1); 
     	eeprom_read_buffer(EE_ADDR_NODE_SCIA,
     	                  (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIA_ConfigNode),1); 
      eeprom_read_buffer(EE_ADDR_INV_POLL, 
     	                  (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIA_ConfigScanRate),2); 
     	eeprom_read_buffer(EE_ADDR_CFG_SCIB,
     	                  (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIB_ConfigCode),1); 
     	eeprom_read_buffer(EE_ADDR_NODE_SCIB,
     	                  (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIB_ConfigNode),1); 
      eeprom_read_buffer(EE_ADDR_INV_POLL_B, 
     	                  (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIB_ConfigScanRate),2); 
     	                   
      eeprom_read_buffer(EE_ADDR_TMR_TXWAIT_SCIA, 
     	                  (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIA_TmrTxWait),2); 
      eeprom_read_buffer(EE_ADDR_TMR_RXWAIT_SCIA, 
     	                  (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIA_TmrRxWait),2); 
      eeprom_read_buffer(EE_ADDR_TMR_DRIVE_SCIA, 
     	                  (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIA_TmrDrive),2); 

      eeprom_read_buffer(EE_ADDR_TMR_TXWAIT_SCIB, 
     	                  (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIB_TmrTxWait),2); 
      eeprom_read_buffer(EE_ADDR_TMR_RXWAIT_SCIB, 
     	                  (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIB_TmrRxWait),2); 
      eeprom_read_buffer(EE_ADDR_TMR_DRIVE_SCIB, 
     	                  (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIB_TmrDrive),2); 

      CfgMsgFrame.Len = sizeof(CFG_MSG_SERIALCONFIG);
      break;    
  

    case CFG_TCP_SET_SCICONFIG:
     	eeprom_write_buffer(EE_ADDR_CFG_SCIA,
     	                   (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIA_ConfigCode),1); 
     	eeprom_write_buffer(EE_ADDR_NODE_SCIA,
     	                   (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIA_ConfigNode),1); 
      eeprom_write_buffer(EE_ADDR_INV_POLL, 
     	                   (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIA_ConfigScanRate),2); 
     	eeprom_write_buffer(EE_ADDR_CFG_SCIB,
     	                   (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIB_ConfigCode),1); 
     	eeprom_write_buffer(EE_ADDR_NODE_SCIB,
     	                   (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIB_ConfigNode),1); 
      eeprom_write_buffer(EE_ADDR_INV_POLL_B, 
     	                   (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIB_ConfigScanRate),2);    
     	                   
      eeprom_write_buffer(EE_ADDR_TMR_TXWAIT_SCIA, 
     	                  (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIA_TmrTxWait),2); 
      eeprom_write_buffer(EE_ADDR_TMR_RXWAIT_SCIA, 
     	                  (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIA_TmrRxWait),2); 
      eeprom_write_buffer(EE_ADDR_TMR_DRIVE_SCIA, 
     	                  (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIA_TmrDrive),2); 

      eeprom_write_buffer(EE_ADDR_TMR_TXWAIT_SCIB, 
     	                  (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIB_TmrTxWait),2); 
      eeprom_write_buffer(EE_ADDR_TMR_RXWAIT_SCIB, 
     	                  (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIB_TmrRxWait),2); 
      eeprom_write_buffer(EE_ADDR_TMR_DRIVE_SCIB, 
     	                  (UINT8 *)(&((CFG_MSG_SERIALCONFIG *)(CfgMsgFrame.Data))->SCIB_TmrDrive),2); 
     	                       	                   	                   
      // ABR: generalizar con defines UP_DRIVER_SCIA_REINIT ()...   UP_DRIVER_SCIB_REINIT ()...
// ABR_TIP: revisar esto..
//      BAC_ReInit();			 // Para actualizar configuración del puerto serie y polling 
      CfgMsgFrame.Len = 0;
      break; 

 
    case CFG_TCP_UPGRADE_FW:
      TmpWriteByte = LDR_UPDATE_LOAD;
      eeprom_write_buffer(EE_ADDR_FW_UPDATE, &TmpWriteByte, 1);     
      MON_ResetBoard(0xF2);    
      __asm SEI;                         // Deshabilitar Interrupciones (RESET)
      FOREVER;    
      break;

//  FW cargado ok, grabar bandera para informar al loader..
    case CFG_TCP_VALIDATE_FW:
      TmpWriteByte = LDR_UPDATE_VALID;
      eeprom_write_buffer(EE_ADDR_FW_UPDATE, &TmpWriteByte, 1);     
      CfgMsgFrame.Len = 0;
      break;

//  RECUPERAR EL FW DE FABRICA.. 
    case CFG_TCP_RESTORE_FW:
      TmpWriteByte = LDR_FACTORY_TEST;
      eeprom_write_buffer(EE_ADDR_FW_UPDATE, &TmpWriteByte, 1);     
      MON_ResetBoard(0xF3);    
      __asm SEI;                         // Deshabilitar Interrupciones (RESET)
      FOREVER;    
      break;
      
      
    case CFG_TCP_LOG_READ:
    		
      if ( ERROR == MemGetBuffer ( (void**)(&pReadLogBuffer), &bLogToken)) 
      {
          CfgMsgFrame.Len = 0;
      } 
      else
      {
          wLogIdx = FLA_ReadLog(pReadLogBuffer);        
        	if ( ERROR != wLogIdx ) 
         	{
          	  
          	  (void)sprintf((char*)tmpbuf, "%d - \0", 
          	                    ((LOG_FORMAT*)pReadLogBuffer)->ParamVal); 
            	(void)strcpy((char*)CfgMsgFrame.Data, (char*)tmpbuf);	

          	  (void)sprintf((char*)tmpbuf, "[ %02x/%02x/%02x - %02x:%02x:%02x ]:\0 ", 
                          			((LOG_FORMAT*)pReadLogBuffer)->LogDateTime.Date,
                          			((LOG_FORMAT*)pReadLogBuffer)->LogDateTime.Month,
                          			((LOG_FORMAT*)pReadLogBuffer)->LogDateTime.Year,
                          			((LOG_FORMAT*)pReadLogBuffer)->LogDateTime.Hour,
                          			((LOG_FORMAT*)pReadLogBuffer)->LogDateTime.Minute,
                          			((LOG_FORMAT*)pReadLogBuffer)->LogDateTime.Second);
            	(void)strcat((char*)CfgMsgFrame.Data, (char*)tmpbuf);	
             
          	  (void)sprintf((char*)tmpbuf, "%s\0", 
          	                    (char*)&(((LOG_FORMAT*)pReadLogBuffer)->Data[0]));
            	(void)strcat((char*)CfgMsgFrame.Data, (char*)tmpbuf);	
                        
              (void)MemFreeBuffer (bLogToken);
    
            CfgMsgFrame.Len = sizeof(LOG_FORMAT);

         	} 
         	else {           	
            CfgMsgFrame.Len = 0;           	
         	}       	
      }
      break;
 
 
    case CFG_TCP_LOG_REWIND:
      (void)FLA_ResetLogReadCntr(CfgMsgFrame.Parameter);
      CfgMsgFrame.Len = 0;
      break;

    case CFG_TCP_LOG_RESET:
      FLA_ResetLogPtr();
      CfgMsgFrame.Len = 0;
      break;
       
      
    default:
      CfgMsgFrame.OpCode = CFG_TCP_NAK;
      CfgMsgFrame.Len = 0;
      break;  
  }

 
  CfgMsgFrame.RxWindow = CfgMsgFrame.TxWindow;
  
  if (TCP_SendCfgBuff()){				                    // Error en envio de respuesta
	  (void)TMR_SetTimer ( TCP_TIMEOUT_SENDRETRY , PROC_CONFIG, 0x00, FALSE);	            // Reintentar
  }
//  TCP_SendCfgBuff();
  return;
   
}


        

///////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// CFG_Init : inicializacion de variables del proceso
//-----------------------------------------------------------------------------
//
void CFG_Init(void)
{
  
  return;

}


#pragma CODE_SEG DEFAULT


