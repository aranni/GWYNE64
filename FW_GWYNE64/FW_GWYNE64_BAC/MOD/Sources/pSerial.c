#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"
#include "float.h"

#include "Commdef.h"
#include "pSerial.h"
#include "Global.h"
#include "system.h"
#include "tcp_ip.h"


#pragma MESSAGE DISABLE C5919 //WARNING C5919: Conversion of floating to unsigned int

///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_SERIAL
///////////////////////////////////////////////////////////////////////////////
//
const EVENT_FUNCTION	SerialProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_SerialIdleTimeout        },
{EVT_SERIAL_INREQ     , f_SerialPollingMsg         },				 
{EVT_SERIAL_OUTREQ    , f_SerialOutMsg             },				 
{EVT_OTHER_MSG     		, f_Consume                  }};

													
#pragma CODE_SEG NON_BANKED        
			
///////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_SERIAL
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// Encola periodicamente un mensaje de consulta al switch
//
void f_SerialIdleTimeout (void)
{
  
	return;
}

//-----------------------------------------------------------------------------
// Busca permanentemente mensajes encolados para enviar..
//
void f_SerialPollingMsg (void)
{

    UINT8   TcpMsgLen, TmpByte;

    
    if (SerialInQueueIdx.Counter == 0x00){
      return;
    }
    
    // Verificar si es posible enviar datos (puede haber un msg. previo aun sin ACK)
    if (tcp_checksend(TcpSerialSoch) < 0){
      PutEventMsg (EVT_SERIAL_INREQ, PROC_SERIAL, PROC_SERIAL, 0x00);
      return;
    }
  
    TcpMsgLen = 0x00;

    while ( SerialInGetByteAndQuit(&TmpByte) ){
      net_buf[TCP_APP_OFFSET+TcpMsgLen] = TmpByte;                    // Data
	    TcpMsgLen++;
    }
    if ( TcpMsgLen == 0x00 ) return;

//    if ( SerialInGetByte(&TmpByte) ){      
//      net_buf[TCP_APP_OFFSET+TcpMsgLen] = TmpByte;                    // Data
//    }
//    else{
//      return;
//    }    
//    TcpMsgLen++;
    
    if (tcp_send(TcpSerialSoch, &net_buf[TCP_APP_OFFSET], NETWORK_TX_BUFFER_SIZE - TCP_APP_OFFSET, TcpMsgLen) != TcpMsgLen){
      // si hubo error en la transmisión, reintentar luego.
      return;
	  } 
//	  else{
	   // si se transmitió bien, quitar de la cola
//        (void)SerialInGetByteAndQuit(&TmpByte);
	   
//	  }

//    //-- Iniciar timer de supervision.
//	  APDU_WaitResponseTmrId = TMR_SetTimer ( 5000 , PROC_DNP, 0x00, FALSE);  //Por ahora..
    return; 
  
}



//-----------------------------------------------------------------------------
// Envía mensaje por puerto serie encolados desde TCP
//
void f_SerialOutMsg (void)
{
  UINT8   OutTmpByte;


  while ( SerialOutGetByteAndQuit (&OutTmpByte) == TRUE){    // Si hay algún byte encolado

     SCIB_TransmitByte (OutTmpByte);  

  } 
//  else{    
//    PutEventMsg (EVT_SERIAL_OUTREQ , PROC_SERIAL, PROC_SERIAL, 0x00);    // revisar periódicamente
//  }
	return;
	
}


///////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// SRL_Init : inicializacion de variables del proceso
//-----------------------------------------------------------------------------
//
void SRL_Init(void)
{
//  UINT8   bTmpCntr;

  SCIB_Init ();

	SerialInQueueIdx.Counter 	= 0x00;
	SerialInQueueIdx.IndexOut	= 0x00;
	SerialInQueueIdx.IndexIn	= 0x00;  

	SerialOutQueueIdx.Counter 	= 0x00;
	SerialOutQueueIdx.IndexOut	= 0x00;
	SerialOutQueueIdx.IndexIn	  = 0x00;  

//  PutEventMsg (EVT_SERIAL_INREQ , PROC_SERIAL, PROC_SERIAL, 0x00);    // revisar periódicamente
//  PutEventMsg (EVT_SERIAL_OUTREQ , PROC_SERIAL, PROC_SERIAL, 0x00);    // revisar periódicamente

}

void SRL_ReInit(void)
{
   
  SCIB_ReInit ();
  return;      
}



//-----------------------------------------------------------------------------
// Sacar un mensaje de la cola IN
//-----------------------------------------------------------------------------
//
UINT8 SerialInGetByteAndQuit (UINT8* BytePtr)
{
	if (SerialInQueueIdx.Counter) {
		*BytePtr     = SerialInMsgQueue [SerialInQueueIdx.IndexOut];
		
     __asm SEI;                         // Disable Interrupts
		SerialInQueueIdx.IndexOut++;										
		if (SerialInQueueIdx.IndexOut == MAX_SRLIN_QUEUE) SerialInQueueIdx.IndexOut = 0x00;				
		SerialInQueueIdx.Counter--;							    
     __asm CLI;                         // Enable Interrupts
		return TRUE;
	}
	return FALSE;
}

UINT8 SerialInGetByte (UINT8* BytePtr)
{
	if (SerialInQueueIdx.Counter) {
		*BytePtr     = SerialInMsgQueue [SerialInQueueIdx.IndexOut];
	
		return TRUE;
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
// Guardar un mensaje en la cola IN
//-----------------------------------------------------------------------------
//
void SerialInPutByte (UINT8 DataByte)
{
	if (SerialInQueueIdx.Counter < MAX_SRLIN_QUEUE) {
		SerialInMsgQueue [SerialInQueueIdx.IndexIn]  = DataByte;
		
		SerialInQueueIdx.IndexIn++;
		if (SerialInQueueIdx.IndexIn == MAX_SRLIN_QUEUE) SerialInQueueIdx.IndexIn = 0x00;

//		if ( SerialInQueueIdx.Counter == 0x00) // si es el primero 
//      PutEventMsg (EVT_SERIAL_INREQ , PROC_SERIAL, PROC_SERIAL, 0x00);    // avisar al serial server

		SerialInQueueIdx.Counter++;
	}
	return;
}	


//-------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Sacar un mensaje de la cola OUT
//-----------------------------------------------------------------------------
//
UINT8 SerialOutGetByteAndQuit (UINT8* BytePtr)
{
	if (SerialOutQueueIdx.Counter) {
		*BytePtr     = SerialOutMsgQueue [SerialOutQueueIdx.IndexOut];
		
     __asm SEI;                         // Disable Interrupts
		SerialOutQueueIdx.IndexOut++;										
		if (SerialOutQueueIdx.IndexOut == MAX_SRLOUT_QUEUE) SerialOutQueueIdx.IndexOut = 0x00;				
		SerialOutQueueIdx.Counter--;							    
     __asm CLI;                         // Enable Interrupts
		return TRUE;
	}
	return FALSE;
}

UINT8 SerialOutGetByte (UINT8* BytePtr)
{
	if (SerialOutQueueIdx.Counter) {
		*BytePtr     = SerialOutMsgQueue [SerialOutQueueIdx.IndexOut];
	
		return TRUE;
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
// Guardar un mensaje en la cola OUT
//-----------------------------------------------------------------------------
//
void SerialOutPutByte (UINT8 DataByte)
{
	if (SerialOutQueueIdx.Counter < MAX_SRLOUT_QUEUE) {
		SerialOutMsgQueue [SerialOutQueueIdx.IndexIn]  = DataByte;
		
		SerialOutQueueIdx.IndexIn++;
		if (SerialOutQueueIdx.IndexIn == MAX_SRLOUT_QUEUE) SerialOutQueueIdx.IndexIn = 0x00;
		
//		if ( SerialOutQueueIdx.Counter == 0x00) // si es el primero 
//      PutEventMsg (EVT_SERIAL_OUTREQ , PROC_SERIAL, PROC_SERIAL, 0x00);    // avisar al serial server
		
		SerialOutQueueIdx.Counter++;
	}
	return;
}	


#pragma CODE_SEG DEFAULT

