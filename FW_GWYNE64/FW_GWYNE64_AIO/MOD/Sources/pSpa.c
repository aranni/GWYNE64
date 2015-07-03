
#include "MC9S12NE64.h"
#include "datatypes.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "stdlib.h"
#include "MATH.h"

#include "Commdef.h"
#include "pSpa.h"
#include "global.h"

#if  (FW_APP_BUILT == FW_APP_WEG)   // Generar aplicación para protololcos SPACOM de ABB

extern unsigned short CalcCrc(unsigned char * pData, unsigned short length);


///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_SPA
///////////////////////////////////////////////////////////////////////////////
//
					
const EVENT_FUNCTION	SPAProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_SPAIdleTimeout },
{EVT_SCI_RXMSG 	      , f_SPAIdleRxMsgEnd},
{EVT_SCI_TXMSG 	      , f_SPAIdleTxMsgEnd},
{EVT_OTHER_MSG     		, f_Consume         }};


////////////////////////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_SPA
////////////////////////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------------------------------
//-------------- estado : IDLE ----------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//--Timeout, se envía periódicamente un comando de consulta
//
void f_SPAIdleTimeout (void) {

  return; 
}


//-----------------------------------------------------------------------------
//--RxAtEnd se analiza el mensaje recibido como respuesta a un comando
//
void f_SPAIdleRxMsgEnd (void) {
  return; 
}

//-----------------------------------------------------------------------------
//--TxAtEnd libera el buffer de transmisión cuando se terminó de enviar un msg
//
void f_SPAIdleTxMsgEnd (void) {

  return; 
  
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Funciones Específicas de capa de red de protocolo DNP3 (SPA)
////////////////////////////////////////////////////////////////////////////////////////////////
//



////////////////////////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
////////////////////////////////////////////////////////////////////////////////////////////////
//

//--------------------------------------------------------------------------------------
//-- Funcion de Inicialización
//
void SPA_Init (void)
{
   
}


//--------------------------------------------------------------------------------------
//-- Funcion de Reinicialización en caso de fallas 
//
void SPA_ReInit (void)
{
	
}


#pragma CODE_SEG DEFAULT

#endif  // (FW_APP_BUILT == FW_APP_SPA) : generar aplicación para protololcos SPACOM de ABB


//--------------------------------------------BUFFER_GARBAGE_INI------------------------------------------
//
//--------------------------------------------BUFFER_GARBAGE_END------------------------------------------


