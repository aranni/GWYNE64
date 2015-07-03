#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"
#include "Commdef.h"

#if  (FW_APP_BUILT == FW_APP_BIO)   // Generar aplicación para lector BIOMETRICO por UDP

#include "pNexoBioUdp.h"
#include "Global.h"
#include "system.h"
#include "tcp_ip.h"

///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_NEXO
///////////////////////////////////////////////////////////////////////////////
//
const EVENT_FUNCTION	NexusProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	      , f_NexusIdleTimeout      },
{EVT_DIO_CHANGEON 	    , f_NexusIdleIoChangeOn   },
{EVT_DIO_CHANGEOFF 	    , f_NexusIdleIoChangeOff  },
{EVT_DIO_STATEON 	      , f_NexusIdleIoStateOn    },
{EVT_DIO_STATEOFF 	    , f_NexusIdleIoStateOff   },
{EVT_DIO_PULSE 	        , f_NexusIdleIoPulse      },
{EVT_OTHER_MSG     		  , f_Consume               }};

const EVENT_FUNCTION	NexusProc_TRXA [] =
{
{EVT_TMR_TIMEOUT 	      , f_NexusTRXErr     },
{EVT_SCI_RXMSG 	        , f_NexusTRXARxMsg      },
{EVT_SCI_RXERR 	        , f_NexusTRXErr         },
{EVT_SCI_TXERR 	        , f_NexusTRXErr         },
{EVT_SCI_TXBUSY         , f_NexusTRXErr         },
{EVT_OTHER_MSG     		  , f_Consume             }};

const EVENT_FUNCTION	NexusProc_TRXB [] =
{
{EVT_TMR_TIMEOUT 	      , f_NexusTRXErr     },
{EVT_SCI_RXMSG 	        , f_NexusTRXBRxMsg      },
{EVT_SCI_RXERR 	        , f_NexusTRXErr         },
{EVT_SCI_TXERR 	        , f_NexusTRXErr         },
{EVT_SCI_TXBUSY         , f_NexusTRXErr         },
{EVT_OTHER_MSG     		  , f_Consume             }};

const EVENT_FUNCTION	NexusProc_TRXC [] =
{
{EVT_TMR_TIMEOUT 	      , f_NexusTRXErr     },
{EVT_SCI_RXMSG 	        , f_NexusTRXCRxMsg      },
{EVT_SCI_RXERR 	        , f_NexusTRXErr         },
{EVT_SCI_TXERR 	        , f_NexusTRXErr         },
{EVT_SCI_TXBUSY         , f_NexusTRXErr         },
{EVT_OTHER_MSG     		  , f_Consume             }};


///////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_NEXO
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
//
void f_NexusTRXErr (void)
{
    
    // Pasar al estado de encuestas IDLE
    FirstEventInCurrentState = (EVENT_FUNCTION*)NexusProc_IDLE;  
    
    //Reinicializar el timer al volver del estado de encuesta serie..
    if (NEX_TimerId == 0xFF)
	    NEX_TimerId = TMR_SetTimer ( NEX_PERIODIC_TIMERVAL , PROC_NEXO, 0x00, FALSE);

}

//-------------- estado : TRXA ----------------------------------------------------------------
//
//-----------------------------------------------------------------------------
// Enviar próxima encuesta BIO por puerto serie
//-----------------------------------------------------------------------------
//
void f_NexusTRXARxMsg(void) 
{
    UINT8*  pTxBioBuffer;
    UINT8   CurrentBioRxToken; 
    UINT16  CurrentBioRxLen; 
    
    CurrentBioRxToken = (UINT8)CurrentMsg.Param;
    if (CurrentBioRxToken != NEX_BioMsgToken) {      
      MON_ResetIfNotConnected(0XB0);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;
    }
  
		// Bytes recibidos  
    if ( MemGetUsedLen( CurrentBioRxToken, &CurrentBioRxLen) != OK){
      MON_ResetIfNotConnected(0XB7);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;
    }
    if (MemGetBufferPtr( CurrentBioRxToken , &((void*)pTxBioBuffer) ) != OK ){    
      MON_ResetIfNotConnected(0XB8);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;   
    }
       
    //------------

#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
		  (void)sprintf((char *)udp_buf,"BIO_RX: %x, %x, %x, %x, %x, %x \n",
		                pTxBioBuffer[0],
		                pTxBioBuffer[1],
		                pTxBioBuffer[2],
		                pTxBioBuffer[3],
		                pTxBioBuffer[4],
		                pTxBioBuffer[5]
		                 );
      udp_data_to_send = TRUE;
  }
#endif  		                              

    
    if (pTxBioBuffer[26])   // hay datos
    {
        f_NexusBioSendQuery(BioFrameB, SIZE_FRAME_B );
        // Pasar al estado de encuestas BIO, hasta recibir respuesta a la consulta
        FirstEventInCurrentState = (EVENT_FUNCTION*)NexusProc_TRXB;  
    }
    else  // No hay registros nuevos volver al estado IDLE con un timer iniciado
    {
        // Pasar al estado de encuestas BIO, hasta recibir respuesta a la consulta
        FirstEventInCurrentState = (EVENT_FUNCTION*)NexusProc_IDLE;  
        
        //Reinicializar el timer al volver del estado de encuesta serie..
    	  NEX_TimerId = TMR_SetTimer ( NEX_PERIODIC_TIMERVAL , PROC_NEXO, 0x00, FALSE);
    }

  return; 
}


//-------------- estado : TRXB ----------------------------------------------------------------
//
//-----------------------------------------------------------------------------
// Enviar próxima encuesta BIO por puerto serie
//-----------------------------------------------------------------------------
//
void f_NexusTRXBRxMsg(void) 
{
    UINT8*  pTxBioBuffer;
    UINT8   CurrentBioRxToken; 
    UINT16  CurrentBioRxLen; 

    UINT8*  pTxBuffer;
    UINT8   bTmpToken; 
    
    
    CurrentBioRxToken = (UINT8)CurrentMsg.Param;
    if (CurrentBioRxToken != NEX_BioMsgToken) {      
      MON_ResetIfNotConnected(0XB0);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;
    }
  
		// Bytes recibidos  
    if ( MemGetUsedLen( CurrentBioRxToken, &CurrentBioRxLen) != OK){
      MON_ResetIfNotConnected(0XB7);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;
    }
    if (MemGetBufferPtr( CurrentBioRxToken , &((void*)pTxBioBuffer) ) != OK ){    
      MON_ResetIfNotConnected(0XB8);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;   
    }
       
    //------------

#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
		  (void)sprintf((char *)udp_buf,"BIO_RX: %x, %x, %x, %x, %x, %x \n",
		                pTxBioBuffer[0],
		                pTxBioBuffer[1],
		                pTxBioBuffer[2],
		                pTxBioBuffer[3],
		                pTxBioBuffer[4],
		                pTxBioBuffer[5]
		                 );
      udp_data_to_send = TRUE;
  }
#endif  		                              

    
    if (pTxBioBuffer[14])   // hay datos
    {
        if ( OK == MemGetBuffer ( (void**)(&pTxBuffer), &bTmpToken)) 
        {
            (void)sprintf((char *)&(((LOG_EVENT_FORMAT*)pTxBuffer)->Data[0]),"BIO_REGISTER_LOG %02X\0",
                          (UINT8)pTxBioBuffer[14]);
            ((LOG_EVENT_FORMAT*)pTxBuffer)->EventSource =  PROC_NEXO;
            ((LOG_EVENT_FORMAT*)pTxBuffer)->EventLastState =  EVENT_STATE_PEND;    
            ((LOG_EVENT_FORMAT*)pTxBuffer)->EventCode =  EVENT_CODE_BIO_REGISTER_LOG;    
            ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamByteA =  pTxBioBuffer[14];  // entrada reportada   
            ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamByteB =  0x00;    
            ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamWord  =  0x00;      // no usado 
            ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamDword =  0x00;      // no usado 
                                                                                   
            PutEventMsg (EVT_LOG_EVENT_SAVE, PROC_DEBUG, PROC_NEXO, (UINT16)bTmpToken);    
        } 
    }


    // Enviar ultimo comando para finalizar
    f_NexusBioSendQuery(BioFrameC, SIZE_FRAME_C );
    // Pasar al estado de encuestas BIO, hasta recibir respuesta a la consulta
    FirstEventInCurrentState = (EVENT_FUNCTION*)NexusProc_TRXC;  
        

  return; 
}


//-------------- estado : TRXC ----------------------------------------------------------------
//
//-----------------------------------------------------------------------------
// Enviar próxima encuesta BIO por puerto serie
//-----------------------------------------------------------------------------
//
void f_NexusTRXCRxMsg(void) 
{
    UINT8*  pTxBioBuffer;
    UINT8   CurrentBioRxToken; 
    UINT16  CurrentBioRxLen; 
    
    CurrentBioRxToken = (UINT8)CurrentMsg.Param;
    if (CurrentBioRxToken != NEX_BioMsgToken) {      
      MON_ResetIfNotConnected(0XB0);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;
    }
  
		// Bytes recibidos  
    if ( MemGetUsedLen( CurrentBioRxToken, &CurrentBioRxLen) != OK){
      MON_ResetIfNotConnected(0XB7);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;
    }
    if (MemGetBufferPtr( CurrentBioRxToken , &((void*)pTxBioBuffer) ) != OK ){    
      MON_ResetIfNotConnected(0XB8);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;   
    }
       
    //------------

#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
		  (void)sprintf((char *)udp_buf,"BIO_RX: %x, %x, %x, %x, %x, %x \n",
		                pTxBioBuffer[0],
		                pTxBioBuffer[1],
		                pTxBioBuffer[2],
		                pTxBioBuffer[3],
		                pTxBioBuffer[4],
		                pTxBioBuffer[5]
		                 );
      udp_data_to_send = TRUE;
  }
#endif  		                              

    
    // Pasar al estado de encuestas IDLE
    FirstEventInCurrentState = (EVENT_FUNCTION*)NexusProc_IDLE;  
    
    //Reinicializar el timer al volver del estado de encuesta serie..
	  NEX_TimerId = TMR_SetTimer ( NEX_PERIODIC_TIMERVAL , PROC_NEXO, 0x00, FALSE);

  return; 
}



//
//-------------- estado : IDLE ----------------------------------------------------------------
//

//-----------------------------------------------------------------------------
// Enviar próxima encuesta SPA por puerto serie
//-----------------------------------------------------------------------------
//
void f_NexusBioSendQuery(const UINT8* pcBioFrame, UINT8 bBioFrameSize) 
{
    UINT8*  pTxBioBuffer;
    UINT16  wSenRequestParam;
    
    // Obtener el puntero al buffer de transmisión serie para mensaje BIO
    if (MemGetBufferPtr( NEX_BioMsgToken , &((void*)pTxBioBuffer) ) != OK ){    
      MON_ResetIfNotConnected(0XB5);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;   
    }

	  (void)memcpy( (UINT8*)pTxBioBuffer, (UINT8*)pcBioFrame, bBioFrameSize );
				                 
    // longitud de trama SCI a recibir, se escribe en el parametro UsedLen del buffer (esperar por lo menos 8 bytes)
    if ( MemSetUsedLen(NEX_BioMsgToken, bBioFrameSize) != OK){
      MON_ResetIfNotConnected(0XB6);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;   
    }

    wSenRequestParam = 0;    // En la parte alta del parámetro se pasa el offset de transmisión
    wSenRequestParam <<= 8;
    wSenRequestParam |= NEX_BioMsgToken;

    PutEventMsg (EVT_SCI_TXCMD, NEX_TargetSci, PROC_NEXO, wSenRequestParam );


#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
		  (void)sprintf((char *)udp_buf,"BIO_TX: %x, %x, %x, %x, %x, %x \n",
		                pTxBioBuffer[0],
		                pTxBioBuffer[1],
		                pTxBioBuffer[2],
		                pTxBioBuffer[3],
		                pTxBioBuffer[4],
		                pTxBioBuffer[5]
		                 );
      udp_data_to_send = TRUE;
  }
#endif  		                              

  return; 
}

//-----------------------------------------------------------------------------
// Reintentar envio de mensaje modbus
//-----------------------------------------------------------------------------
//
void f_NexusIdleTimeout (void)
{
    UINT8*  pTxBuffer;
    UINT8   bTmpToken; 
  
    NEX_TimerId = 0xFF;
    
    if ( OK == MemGetBuffer ( (void**)(&pTxBuffer), &bTmpToken)) 
    {
          
        ((LOG_EVENT_FORMAT*)pTxBuffer)->EventLastState = EVENT_STATE_PEND;
        
        if (OK == FLA_SearchStateEvent(bTmpToken) )
        {
        
            // ENVIAR MENSAE UDP
        	  (void)sprintf((char *)udp_cli_buf,">RGP%02x%02x%02x%02x%02x%02x%02x%02x%02x;ID=%02x;%04d;*\n",
        	                ((LOG_EVENT_FORMAT*)pTxBuffer)->LogDateTime.Date,
        	                ((LOG_EVENT_FORMAT*)pTxBuffer)->LogDateTime.Month,
        	                ((LOG_EVENT_FORMAT*)pTxBuffer)->LogDateTime.Year,
        	                ((LOG_EVENT_FORMAT*)pTxBuffer)->LogDateTime.Hour,
        	                ((LOG_EVENT_FORMAT*)pTxBuffer)->LogDateTime.Minute,
        	                ((LOG_EVENT_FORMAT*)pTxBuffer)->LogDateTime.Second,
        	                ((LOG_EVENT_FORMAT*)pTxBuffer)->EventCode,
        	                ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamByteA,
        	                ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamByteB,
        	                0x01, // ID del modulo
        	                ((LOG_EVENT_FORMAT*)pTxBuffer)->EventId);
            xorMsg(udp_cli_buf);        	                
            udp_cli_data_to_send = TRUE;               
                
            (void)FLA_ChangeStateEvent (((LOG_EVENT_FORMAT*)pTxBuffer)->EventId, EVENT_STATE_SENT);    
        
        }         
        else 		// Si no hay pendientes de envio, revisar si hay enviados sin ACK
        {
        
            ((LOG_EVENT_FORMAT*)pTxBuffer)->EventLastState = EVENT_STATE_SENT;
            
            if (OK == FLA_SearchStateEvent(bTmpToken) )
            {
            
                // Verificar la antiguedad del mensaje.
                
                // ENVIAR MENSAE UDP
            	  (void)sprintf((char *)udp_cli_buf,">RGP%02x%02x%02x%02x%02x%02x%02x%02x%02x;ID=%02x;%04d;*\n",
            	                ((LOG_EVENT_FORMAT*)pTxBuffer)->LogDateTime.Date,
            	                ((LOG_EVENT_FORMAT*)pTxBuffer)->LogDateTime.Month,
            	                ((LOG_EVENT_FORMAT*)pTxBuffer)->LogDateTime.Year,
            	                ((LOG_EVENT_FORMAT*)pTxBuffer)->LogDateTime.Hour,
            	                ((LOG_EVENT_FORMAT*)pTxBuffer)->LogDateTime.Minute,
            	                ((LOG_EVENT_FORMAT*)pTxBuffer)->LogDateTime.Second,
            	                ((LOG_EVENT_FORMAT*)pTxBuffer)->EventCode,
            	                ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamByteA,
            	                ((LOG_EVENT_FORMAT*)pTxBuffer)->EventParamByteB,
            	                0x01, // ID del modulo
            	                ((LOG_EVENT_FORMAT*)pTxBuffer)->EventId);
                xorMsg(udp_cli_buf);        	                
                udp_cli_data_to_send = TRUE;                                 
                    
                (void)FLA_ChangeStateEvent (((LOG_EVENT_FORMAT*)pTxBuffer)->EventId, EVENT_STATE_SENT);    
            
            }      
        }
        (void)MemFreeBuffer(bTmpToken);
    }      	      

    // Pasar al estado de encuestas BIO, hasta recibir respuesta a la consulta
    FirstEventInCurrentState = (EVENT_FUNCTION*)NexusProc_TRXA;  
    
    f_NexusBioSendQuery(BioFrameA, SIZE_FRAME_A );
     
    //Reinicializar el timer al volver del estado de encuesta serie..
	  //NEX_TimerId = TMR_SetTimer ( NEX_PERIODIC_TIMERVAL , PROC_NEXO, 0x00, FALSE);

	  return;
}

//-----------------------------------------------------------------------------
// Entrada digital por nivel reporte periodico , estado ON
//-----------------------------------------------------------------------------
//
void f_NexusIdleIoStateOn (void)
{

	  //(void)sprintf((char *)udp_cli_buf,"GPIO_REPORT_ON: %02X \n",(UINT8)CurrentMsg.Param);
  	IIC_RtcGetDateTime();	
 	  (void)sprintf((char *)udp_cli_buf,">RGP%02x%02x%02x%02x%02x%02x%02x%02x%02x;ID=%02x;%04d;*\n",
  	                StatisticData.CurrentDateTime.Date,
  	                StatisticData.CurrentDateTime.Month,
  	                StatisticData.CurrentDateTime.Year,
  	                StatisticData.CurrentDateTime.Hour,
  	                StatisticData.CurrentDateTime.Minute,
  	                StatisticData.CurrentDateTime.Second,
  	                EVENT_CODE_DIO_LEVEL_STATE, // EventCode del evento = Reporte periodico de estado
  	                (UINT8)CurrentMsg.Param,    // ParamA de Evento = Numero de Entrada
  	                0x01,											  // ParamB de Evento = Valor de la entrada
  	                0x01,                       // ID del modulo
  	                0x0000);										// Reportes periodicos no llevan secuencia, no hace falta ACK
      xorMsg(udp_cli_buf);        	                
      udp_cli_data_to_send = TRUE;                         
          
  	  return;
}

//-----------------------------------------------------------------------------
// Entrada digital por nivel reporte periodico , estado OFF
//-----------------------------------------------------------------------------
//
void f_NexusIdleIoStateOff (void)
{

	  //(void)sprintf((char *)udp_cli_buf,"GPIO_REPORT_OFF: %02X \n",(UINT8)CurrentMsg.Param);
  	IIC_RtcGetDateTime();	
 	  (void)sprintf((char *)udp_cli_buf,">RGP%02x%02x%02x%02x%02x%02x%02x%02x%02x;ID=%02x;%04d;*\n",
  	                StatisticData.CurrentDateTime.Date,
  	                StatisticData.CurrentDateTime.Month,
  	                StatisticData.CurrentDateTime.Year,
  	                StatisticData.CurrentDateTime.Hour,
  	                StatisticData.CurrentDateTime.Minute,
  	                StatisticData.CurrentDateTime.Second,
  	                EVENT_CODE_DIO_LEVEL_STATE, // EventCode del evento = Reporte periodico de estado
  	                (UINT8)CurrentMsg.Param,    // ParamA de Evento = Numero de Entrada
  	                0x00,											  // ParamB de Evento = Valor de la entrada
  	                0x01,                       // ID del modulo
  	                0x0000);										// Reportes periodicos no llevan secuencia, no hace falta ACK
      xorMsg(udp_cli_buf);        	                
      udp_cli_data_to_send = TRUE;               

  	  return;
}


//-----------------------------------------------------------------------------
// Entrada digital por nivel camia a ON
//-----------------------------------------------------------------------------
//
void f_NexusIdleIoChangeOn (void)
{

	  (void)sprintf((char *)udp_cli_buf,"GPIO_LEVEL_ON: %02X \n",
	                (UINT8)CurrentMsg.Param);
    udp_cli_data_to_send = TRUE;
          
          
	  return;
}

//-----------------------------------------------------------------------------
// Entrada digital por nivel camia a OFF
//-----------------------------------------------------------------------------
//
void f_NexusIdleIoChangeOff (void)
{

	  (void)sprintf((char *)udp_cli_buf,"GPIO_LEVEL_OFF: %02X \n",
	                (UINT8)CurrentMsg.Param);
    udp_cli_data_to_send = TRUE;
    
   
	  return;
}

//-----------------------------------------------------------------------------
// Entrada digital por flanco detecta pulso
//-----------------------------------------------------------------------------
//
void f_NexusIdleIoPulse (void)
{

	  (void)sprintf((char *)udp_cli_buf,"GPIO_PULSE_ON: %02X \n",
	                (UINT8)CurrentMsg.Param);
    udp_cli_data_to_send = TRUE;


	  return;
}




///////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// NEX_Init : inicializacion de variables del proceso
//-----------------------------------------------------------------------------
//
void NEX_Init(void)
{

    UINT8*  pTxBuffer;
      
    if ( OK != MemGetBuffer((void**)(&pTxBuffer), &NEX_BioMsgToken)) 
    {
        PutEventMsg (EVT_LOG_ID, PROC_DEBUG, OS_NOT_PROC, (UINT16)LOG_ID_OS_NOBUFFER);    
        MON_ResetIfNotConnected(0XB9);    // Error de SW, Reset (TODO: revisar acciones a tomar)
    } 


		NEX_TargetSci = PROC_SCIA;
    SCIA_Init (PROC_NEXO, SCI_FRAME_BIOMETRIC_ANVIZ);

          
	  NEX_TimerId = TMR_SetTimer ( NEX_PERIODIC_TIMERVAL , PROC_NEXO, 0x00, FALSE);
    
	  return;
}


void xorMsg(UINT8* pReadLogBuffer) 
{ 

  	BYTE xorValue = 0x00; 
  	BYTE bTmpCounter;
  	

  	for (bTmpCounter = 0; bTmpCounter < MAX_UDP_FRAME_LEN; bTmpCounter++)
  	{ 
  		if (pReadLogBuffer[bTmpCounter] == '*') break;
  		xorValue ^= pReadLogBuffer[bTmpCounter]; 
  	}
  	bTmpCounter++;
    (void)sprintf((char *)(&(pReadLogBuffer[bTmpCounter])),"%02X<\r\n", xorValue);
        	
} 


//-----------------------------------------------------------------------------
// Calcular CRC
//-----------------------------------------------------------------------------
//
//  The function returns the CRC as a unsigned short type 
//  puchMsg   =  message to calculate CRC upon
//  usDataLen =  quantity of bytes in message

void NEX_BioBuildCRC16 ( UINT8 * puchMsg, UINT16 usDataLen )
{
  UINT16 CRC_Outcome;
  UINT8  TmpByte;
  UINT8  TmpIdx;
  UINT16 TmpWord;
  
  CRC_Outcome = 0xFFFF;
  
  while (usDataLen--){                          // pass through message buffer
    TmpByte = (*puchMsg);
    puchMsg++;
    for (TmpIdx = 0; TmpIdx < 8; TmpIdx++){
      TmpWord = (CRC_Outcome ^ TmpByte) & 1;
      CRC_Outcome >>= 1;
      TmpByte >>= 1;
      if (TmpWord)
        CRC_Outcome ^= BIO_REVPOLY;
    }
  }
  
  TmpByte = (CRC_Outcome & 0xFF00)>>8;
  CRC_Outcome <<= 8;
  CRC_Outcome |= TmpByte;
  
  *((UINT16*)puchMsg) =  CRC_Outcome;
//  return CRC_Outcome;  

}



#pragma CODE_SEG DEFAULT

#endif 	// (FW_APP_BUILT == FW_APP_BIO)   // Generar aplicación para lector BIOMETRICO por UDP
