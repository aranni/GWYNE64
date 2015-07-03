#include "MC9S12NE64.h"
#include "datatypes.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "stdlib.h"
#include "MATH.h"

#include "Commdef.h"
#include "pMts.h"
#include "global.h"

const float	RADIO_DIESEL	    = (float)18.00;			    // Radio del tanque en decimetros
const float	RADIO_DIESEL_100	= (float)1800.00;			  // Radio del tanque en milimetros

const float	LARGO_DIESEL	= (float)90.00;			        // Largo del tanque en decimetros
const float	PI				    = (float)3.1415926536;		


///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_MTS
///////////////////////////////////////////////////////////////////////////////
//

const EVENT_FUNCTION	MTSProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_MTSIdleTimeout },
{EVT_485_RXMSG 	      , f_MTSIdleRxMsgEnd},
{EVT_485_TXMSG 	      , f_MTSIdleTxMsgEnd},
{EVT_OTHER_MSG     		, f_Consume         }};


////////////////////////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_MTS
////////////////////////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------------------------------
//-------------- estado : IDLE ----------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//--Timeout, se envía periódicamente un comando de consulta
//
void f_MTSIdleTimeout (void) {
  UINT8*  pTxBuffer;
  UINT8   bTmpToken; 
  UINT16  wMsgParam;


  (void)MemGetBuffer ( (void**)(&pTxBuffer), &bTmpToken);
  wMsgParam = bTmpToken; // Token en la parte baja.
  wMsgParam |= 0x0300;    // LEn en la parte alta del parametro

  MTS_CmdIdx ^= 0x01;  
  
  pTxBuffer[0]=0x00;    // extra byte..
  pTxBuffer[1]=0xC0;    // Dirección del sensor
  if ( MTS_CmdIdx ) 
  {
    pTxBuffer[2]=0x12;    // Comando de lectura de niveles
  }
  else 
  {
    pTxBuffer[2]=0x20;    // Comando de lectura de temperaturas
  }
  
  PutEventMsg (EVT_485_TXREQ, PROC_SCIA, PROC_MTS, wMsgParam);     // Enviar requerimiento de TX.

  return; 
}


//-----------------------------------------------------------------------------
//--RxAtEnd se analiza el mensaje recibido como respuesta a un comando
//
void f_MTSIdleRxMsgEnd (void) {
  UINT8   RxEndToken, bTmpIdx, TemperaturaIdx;
  void*   vpAuxBuffer;
  char*   MsgValTxt;
  float   fNivel;

//  if ( CurrentMsg.Param && 0xFF00 )     // si hubo error de recepcion...
  

// Recuperar el puntero al buffer utilizado para recibir el mensaje
// El token del buffer está en la parte baja del parámetro del mensaje
  RxEndToken = (UINT8)CurrentMsg.Param;
  if (MemGetBufferPtr( RxEndToken , &vpAuxBuffer ) != OK ){    
  // ABR: control de errores
    return;   
  }

#ifdef  UDP_DEBUG_PRINTFON
		  (void)sprintf((char *)udp_buf,"MTS_MsgRx:%X %s\n",
		                ((BYTE*)vpAuxBuffer)[1],  // Comando
		                ((char*)&((BYTE*)vpAuxBuffer)[3]));  // Dato_0-0		  
      udp_data_to_send = TRUE;
#endif    

if ( MTS_CmdIdx ) 
{

  bTmpIdx = 0x00;
  while ( ((BYTE*)vpAuxBuffer)[bTmpIdx] != 0x02)   // Buscar bandera de start
  {					
    bTmpIdx++;
    if ( bTmpIdx >= MSG_MAX_MTS_LEN )
    {
      bTmpIdx = 0x00;
      break;
    }
  }
  bTmpIdx++;
	MsgValTxt = (char*)&(((BYTE*)vpAuxBuffer)[bTmpIdx]); 

  while ( ((BYTE*)vpAuxBuffer)[bTmpIdx] != ':')   // Buscar primer separador de valores
  {					
    bTmpIdx++;
    if ( bTmpIdx >= MSG_MAX_MTS_LEN )
    {
      bTmpIdx = 0x00;
      break;
    }
  }
	((BYTE*)vpAuxBuffer)[bTmpIdx] = '\0';
//	MsgValTxt = (char*)&(((BYTE*)vpAuxBuffer)[3]); 
	fNivel		= ptrToFloat(MsgValTxt);
	if (fNivel)
	{
		fNivel *= 25.4;			// de pulgadas a milimetros 
		MTS_SensorValues.fResultadoNivelP = fNivel;
	}

  bTmpIdx++;
	MsgValTxt = (char*)&(((BYTE*)vpAuxBuffer)[bTmpIdx]); 
  while ( ((BYTE*)vpAuxBuffer)[bTmpIdx] != 0x03) 	 // Buscar bandera de fin
  {					
    bTmpIdx++;
    if ( bTmpIdx >= MSG_MAX_MTS_LEN ) 
    {
      bTmpIdx = 0x00;
      break;
    }
  }
//	((BYTE*)vpAuxBuffer)[16] = '\0'; //15
	((BYTE*)vpAuxBuffer)[bTmpIdx] = '\0';
	fNivel		= ptrToFloat(MsgValTxt);
	if (fNivel)
	{
		fNivel *= 25.4;			// de pulgadas a milimetros 
		MTS_SensorValues.fResultadoNivelI = fNivel;
	}
  InputOutputData.AAG_Input_A = MTS_SensorValues.fResultadoNivelP;
  InputOutputData.AAG_Input_B = MTS_SensorValues.fResultadoNivelI;
  
//  fNivel es h
//		fNivel = fResultadoNivelP / (float)100.00;
//		fNivel /=  RADIO_DIESEL;
		fNivel = MTS_SensorValues.fResultadoNivelP / RADIO_DIESEL_100;
		fNivel -= 1.00;
		MTS_SensorValues.fLitrosTanqueDiesel = ( fNivel * sqrtf(1-powf(fNivel,(float)2)) );
   	MTS_SensorValues.fLitrosTanqueDiesel += asinf(fNivel);
		MTS_SensorValues.fLitrosTanqueDiesel += ((float)2 * _M_PI);
		MTS_SensorValues.fLitrosTanqueDiesel -= ((float)1.5 * _M_PI);
		MTS_SensorValues.fLitrosTanqueDiesel *= powf(RADIO_DIESEL,(float)2);
		MTS_SensorValues.fLitrosTanqueDiesel *= LARGO_DIESEL;
  InputOutputData.AAG_Output_A = MTS_SensorValues.fLitrosTanqueDiesel;

		fNivel = MTS_SensorValues.fResultadoNivelI / RADIO_DIESEL_100;
		fNivel -= 1.00;
		MTS_SensorValues.fLitrosTanqueAgua = ( fNivel * sqrtf(1-powf(fNivel,(float)2)) );
		MTS_SensorValues.fLitrosTanqueAgua += asinf(fNivel);
		MTS_SensorValues.fLitrosTanqueAgua += ((float)2.0 * PI);
		MTS_SensorValues.fLitrosTanqueAgua -= ((float)1.5 * PI);
		MTS_SensorValues.fLitrosTanqueAgua *= powf(RADIO_DIESEL,(float)2);
		MTS_SensorValues.fLitrosTanqueAgua *= LARGO_DIESEL;
  InputOutputData.AAG_Output_B = MTS_SensorValues.fLitrosTanqueAgua;

}
else 			// Es comando de lectura de temperaturas..
{
#if 1
  bTmpIdx = 0x00;
  while ( ((BYTE*)vpAuxBuffer)[bTmpIdx] != 0x02)   // Buscar bandera de start
  {					
    bTmpIdx++;
    if ( bTmpIdx >= MSG_MAX_MTS_LEN )
    {
      bTmpIdx = 0x00;
      break;
    }
  }
  bTmpIdx++;

  for (TemperaturaIdx=0 ; TemperaturaIdx<5 ; TemperaturaIdx++) 
  {
    	MsgValTxt = (char*)&(((BYTE*)vpAuxBuffer)[bTmpIdx]); 
      while ( ((BYTE*)vpAuxBuffer)[bTmpIdx] != ':')   // Buscar separador de valores
      {					
        bTmpIdx++;
        if ( bTmpIdx >= MSG_MAX_MTS_LEN )
        {
          bTmpIdx = 0x00;
          break;
        }
      }
    	((BYTE*)vpAuxBuffer)[bTmpIdx] = '\0';
    	MTS_SensorValues.fTemperatura[TemperaturaIdx]	= ptrToFloat(MsgValTxt);
      MTS_SensorValues.fTemperatura[TemperaturaIdx] -= 32;			// de Fareneit a Celcius
    	MTS_SensorValues.fTemperatura[TemperaturaIdx] *= 5;
    	MTS_SensorValues.fTemperatura[TemperaturaIdx] /= 9;
      bTmpIdx++;   	
  }
  
	MsgValTxt = (char*)&(((BYTE*)vpAuxBuffer)[bTmpIdx]); 
  while ( ((BYTE*)vpAuxBuffer)[bTmpIdx] != 0x03) 	 // Buscar bandera de fin
  {					
    bTmpIdx++;
    if ( bTmpIdx >= MSG_MAX_MTS_LEN ) 
    {
      bTmpIdx = 0x00;
      break;
    }
  }
	((BYTE*)vpAuxBuffer)[bTmpIdx] = '\0';
 	MTS_SensorValues.fTemperatura[TemperaturaIdx]	= ptrToFloat(MsgValTxt);
  MTS_SensorValues.fTemperatura[TemperaturaIdx] -= 32;			// de Fareneit a Celcius
	MTS_SensorValues.fTemperatura[TemperaturaIdx] *= 5;
	MTS_SensorValues.fTemperatura[TemperaturaIdx] /= 9;  
	
#if 0	
//#ifdef  UDP_DEBUG_PRINTFON
		  (void)sprintf((char *)udp_buf,"MTS_MsgRx: %#3.1Lf | %#3.1Lf | %#3.1Lf | %#3.1Lf | %#3.1Lf | %#3.1Lf\n",
		                (double)MTS_SensorValues.fTemperatura[0],
		                (double)MTS_SensorValues.fTemperatura[1],
		                (double)MTS_SensorValues.fTemperatura[2],
		                (double)MTS_SensorValues.fTemperatura[3],
		                (double)MTS_SensorValues.fTemperatura[4],
		                (double)MTS_SensorValues.fTemperatura[5]);
      udp_data_to_send = TRUE;
#endif    
	
#endif 
}

 (void)MemFreeBuffer ( RxEndToken);                          // Liberar el buffer de recepcion

// Iniciar un timer para enviar comando..
	MTS_WaitAnswerTimerId =  TMR_SetTimer ( TMR_MTS_RESETLINKWAIT , PROC_MTS, 0x00, FALSE);  
 
  return; 
}

//-----------------------------------------------------------------------------
//--TxAtEnd libera el buffer de transmisión cuando se terminó de enviar un msg
//
void f_MTSIdleTxMsgEnd (void) {
  UINT8 TxEndToken;
  void*   vpAuxBuffer;

//  if ( CurrentMsg.Param && 0xFF00 )     // si hubo error de transmisión...

// Recuperar el puntero al buffer utilizado para transmitir el mensaje
// El token del buffer está en la parte baja del parámetro del mensaje
  TxEndToken = (UINT8)CurrentMsg.Param;
  if (MemGetBufferPtr( TxEndToken , &vpAuxBuffer ) != OK ){    
  // ABR: control de errores
    return;   
  }
  
#if 0  
//#ifdef  UDP_DEBUG_PRINTFON
		  (void)sprintf((char *)udp_buf,"MTS_MsgTx: %X,%X,%X\n",
		                ((BYTE*)vpAuxBuffer)[0],
		                ((BYTE*)vpAuxBuffer)[1],
		                ((BYTE*)vpAuxBuffer)[2]);	
      udp_data_to_send = TRUE;
#endif    
  
  (void)MemFreeBuffer ( TxEndToken);  
  return; 
  
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Funciones Específicas de capa de red de protocolo DNP3 (MTS)
////////////////////////////////////////////////////////////////////////////////////////////////
//


//---------------------------------------------------------------------------
//--Actualiza el valor de CRC en *crcAccum con el agregado del byte dataOctet
//
void MTS_ComputeCRC (
  UINT8 dataOctet, // Data octet
  UINT16* crcAccum // Pointer to 16-bit crc accumulator
  ){
  
    UINT8 idx;    // Index
    UINT16 temp;  // Temporary variable

// Perform right shifts and update crc accumulator
    for (idx = 0; idx < 8; idx++){
        temp = (*crcAccum ^ dataOctet) & 1;
        *crcAccum >>= 1;
        dataOctet >>= 1;
        if (temp)
            *crcAccum ^= REVPOLY;
    }
     
}


//---------------------------------------------------------------------------
//--Cambia el orden de los bytes de una WORD (entre little endian y big endian)
//
void MTS_ChgWordEndian ( UINT16* DataWord)
{  
  UINT16 TmpEndian;
  
  TmpEndian = (*DataWord & 0xFF00)>>8;
  *DataWord = (*DataWord & 0x00FF)<<8;
  *DataWord |= TmpEndian;
  
}



////////////////////////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
////////////////////////////////////////////////////////////////////////////////////////////////
//

#define DNP_REMOTE_ADDR     (WORD)1       //11
#define DNP_LOCAL_ADDR      (WORD)100
//--------------------------------------------------------------------------------------
//-- Funcion de Inicialización
//
void MTS_Init (void)
{
// Las direcciones son WORD, que ya están grabada en EEPROM con formato LSB:MSB
//  eeprom_read_buffer(EE_ADDR_DNP_SLAVE, (UINT8 *)&MTS_wRemoteAddr, 2); 
//  eeprom_read_buffer(EE_ADDR_DNP_MASTER, (UINT8 *)&MTS_wLocalAddr, 2);		 
  
	SCIA_Init();            // Driver de comunicación serie (SCI1) 
	
	MTS_CmdIdx = 0x00;
	
// Iniciar un timer para enviar comando..
	MTS_WaitAnswerTimerId =  TMR_SetTimer ( TMR_MTS_RESETLINKWAIT , PROC_MTS, 0x00, FALSE);  
 
}


//--------------------------------------------------------------------------------------
//-- Funcion de Reinicialización en caso de fallas 
//
void MTS_ReInit (void)
{

	CurrentProcessState [PROC_MTS] = (EVENT_FUNCTION*)MTSProc_IDLE;	

// Iniciar un timer para enviar comando..
	MTS_WaitAnswerTimerId =  TMR_SetTimer ( TMR_MTS_RESETLINKWAIT , PROC_MTS, 0x00, FALSE);  
  
}

#pragma CODE_SEG DEFAULT



//--------------------------------------------BUFFER_GARBAGE_INI------------------------------------------
/*
		szBuffer[8+2] = '\0';
		fTemperatura		= (float)atof(&(szBuffer[3+2]));
		if (fTemperatura)
		{
			fTemperatura -= 32;			// de Fareneit a Celcius
			fTemperatura *= 5;
			fTemperatura /= 9;
			*fResultadoTemp = fTemperatura;
		}
*/
//--------------------------------------------BUFFER_GARBAGE_END------------------------------------------


