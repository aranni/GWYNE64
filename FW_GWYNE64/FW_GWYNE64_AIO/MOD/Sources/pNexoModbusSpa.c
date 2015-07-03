#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"
#include "Commdef.h"

#if  (FW_APP_BUILT == FW_APP_SPA)   // Generar aplicación para protocolos SPACOM de ABB

#include "pNexoModbusSpa.h"
#include "Global.h"
#include "system.h"
#include "tcp_ip.h"

#define   MAX_SPA_MSGLEN  10

///////////////////////////////////////////////////////////////////////////////
// Estados del proceso PROC_NEXO
///////////////////////////////////////////////////////////////////////////////
//

const EVENT_FUNCTION	NexusProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	      , f_NexusIdleTimeout      },
{EVT_MODTCP_RD_MUL_REG 	, f_NexusAnalizeMsg     },
//{EVT_MODTCP_WR_SIN_REG 	, f_NexusAnalizeMsg     },
//{EVT_MODTCP_WR_MUL_REG 	, f_NexusAnalizeMsg     },
{EVT_NEX_INICMD 	        , f_NexusAnswerSpa      },
{EVT_OTHER_MSG     		  , f_Consume             }};


const EVENT_FUNCTION	NexusProc_TRXSPA [] =
{
{EVT_TMR_TIMEOUT 	      , f_NexusSpaTimeout	},
{EVT_NEX_INICMD 	      , f_NexusSpaSendMsg },
{EVT_SCI_RXMSG 	        , f_NexusSpaRxMsg   },
{EVT_SCI_RXERR 	        , f_NexusSpaErr     },
{EVT_SCI_TXERR 	        , f_NexusSpaErr     },
{EVT_OTHER_MSG     		  , f_Consume         }};


///////////////////////////////////////////////////////////////////////////////
// Transiciones para el proceso PROC_NEXO
///////////////////////////////////////////////////////////////////////////////
//
//-------------- estado : IDLE ----------------------------------------------------------------
//
//-----------------------------------------------------------------------------
// Reintentar envio de mensaje modbus
//-----------------------------------------------------------------------------
//
void f_NexusIdleTimeout (void)
{

	return;
}

//-----------------------------------------------------------------------------
// Enviar respuestas SPA recibidas por puerto serie como respuesta ModbusTCP
//-----------------------------------------------------------------------------
//
void f_NexusAnswerSpa (void)
{
    UINT8   TxModBusTcpToken;
    MODBUS_MSG*  pModbusOutMsg;
    UINT8   BytesRequired;

//    if (NEX_ModbusTcpRequestSocketIdx >= MAX_SRV_TCPSOCH){
//      MON_ResetIfNotConnected(0XB1);    // Error de SW, Reset (TODO: revisar acciones a tomar)
//      return;   // ABR_: (SW_ERR) analizar acciones a tomar     
//    }
    
    // ABR: 2013-08-15 actualizacion al nuevo modelo modbus tcp
    TxModBusTcpToken = NEX_ModbusTcpRequesrToken; // TcpSockSrv[NEX_ModbusTcpRequestSocketIdx].BufferToken;
    
    // Si se cerró el socket en el interin, cancelar transaccion
    if (TxModBusTcpToken == 0xFF ) return;
    if (MemGetBufferPtr( TxModBusTcpToken , &((void*)pModbusOutMsg) ) != OK ){    
      MON_ResetIfNotConnected(0XB2);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;   
    }
    
    BytesRequired = SpaDataIndex;// + 1;
    BytesRequired *= 2;    
        
    DummyPtr = memcpy( (UINT8*)(&(pModbusOutMsg->Data[1])), (UINT8*)SpaData, BytesRequired); 
    pModbusOutMsg->LengthLow = 3 + BytesRequired;              // LengthLow
    pModbusOutMsg->Data[0]   = BytesRequired;                  //BytesRequired  
   
    //TcpSockSrv[NEX_ModbusTcpRequestSocketIdx].MsgLen =  9 + BytesRequired ;  // Len de respuesta modbus
        
    if ( MemSetUsedLen(TxModBusTcpToken, 9 + BytesRequired) != OK)        // Bytes recibidos
    {  
#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
    	  (void)sprintf((char *)udp_buf,"SCIA Err:0x02 \n");
        udp_data_to_send = TRUE;
  }
#endif  	
    }
    
    //PutEventMsg (EVT_MODTCP_TXMSG, PROC_MODBUS_SLAVE_TCP, PROC_NEXO, NEX_ModbusTcpRequestSocketIdx);
    PutEventMsg (EVT_MODTCP_TXMSG, PROC_MODBUS_SLAVE_TCP, PROC_NEXO, TxModBusTcpToken);

    
	  return;

}


//-----------------------------------------------------------------------------
// Analizar mensaje Modbus recibido
//-----------------------------------------------------------------------------
//
void f_NexusAnalizeMsg (void)
{
    UINT8   BytesRequired;
    UINT8   ModbusOffsetIdx;
    UINT8   RxModBusTcpToken;
    MODBUS_MSG*	  pModbusInMsg;					            // Mensaje recibido por ModbusTCP.
    UINT8  IdxSocket; 

    SETBIT(dwIntegrityMonitor,PROC_NEXO);
    
    
    /////
/*
    // en el parámetro está el index del socket TCP por donde llegó la encuesta ModbusTCP
    NEX_ModbusTcpRequestSocketIdx = (UINT8)CurrentMsg.Param;  // Guardo el Idx en variable global.

    if (NEX_ModbusTcpRequestSocketIdx >= MAX_SRV_TCPSOCH){
      MON_ResetIfNotConnected(0XB3);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;   // ABR_: (SW_ERR) analizar acciones a tomar     
    }    
    
    RxModBusTcpToken = TcpSockSrv[NEX_ModbusTcpRequestSocketIdx].BufferToken;
    // Si se cerró el socket en el interin, cancelar transaccion
    if (RxModBusTcpToken == 0xFF ) return;
    if (MemGetBufferPtr( RxModBusTcpToken , &((void*)pModbusInMsg) ) != OK ){    
      MON_ResetIfNotConnected(0XB4);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;   
    }
  
*/      
  ////
  
    RxModBusTcpToken = (UINT8)CurrentMsg.Param;
    NEX_ModbusTcpRequesrToken = RxModBusTcpToken;
     
    // Si se cerró el socket en el interin, cancelar transaccion
    if (RxModBusTcpToken == 0xFF ) return;
    
    if (MemGetBufferPtr( RxModBusTcpToken , &((void*)pModbusInMsg) ) != OK ){    
#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
  		  (void)sprintf((char *)udp_buf,"NexoModbus Err:0xC2 \n");
        udp_data_to_send = TRUE;
  }
#endif  	
       //MON_ResetIfNotConnected(0XC2);    // Error de SW, Reset (TODO: revisar acciones a tomar)
       return;   
    }

    if ( MemGetRouteMark(RxModBusTcpToken, &IdxSocket ) != OK)   // Marca de origen
    {
#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
  		  (void)sprintf((char *)udp_buf,"NexoModbus Err:0xC0 \n");
        udp_data_to_send = TRUE;
  }
 #endif  	
       //MON_ResetIfNotConnected(0xC0);    // Error de SW, Reset (TODO: revisar acciones a tomar)
        return;   // ABR_: (SW_ERR) analizar acciones a tomar         
    }
    
    if (IdxSocket >= MAX_SRV_TCPSOCH){
        MON_ResetIfNotConnected(0xC1);    // Error de SW, Reset (TODO: revisar acciones a tomar)
        return;   // ABR_: (SW_ERR) analizar acciones a tomar     
    }
      
  /////
  
  
 // case MOD_FC_RD_MUL_REG:										                    // Read Multiple Register
    ValuesRequired = pModbusInMsg->Data[3];
    if ( ValuesRequired > MAX_MODBUS_REG ) return;		        // Data[3]:0-125 Word Count FC=3
    BytesRequired = ( 2 * ValuesRequired );
        
//  if (pModbusInMsg->UI == ModbusTcpLocalNode ) 
//    pDataToSend = (UINT8 *)(&InputOutputData);  							// Mapeo en 40000

    ModbusOffset   = *((UINT16*)&(pModbusInMsg->Data[0]));    
    ModbusOffset++;    
    ModbusOffsetIdx = (UINT8)(ModbusOffset / 100);

    // la unidad de mil indica el canal: ej 40xxx = Ch0, 41xxx = Ch1,..
    // la centena inidica la entidad: 4x1xx = Input, 4x2xx = Output, 4x3xx = Setting, 4x4xx = Variable
    // la decena y unidad indica el indice dentro de la seleccion
    // Por ej. Output 3 del canal 1 será 41203
    
    SpaSlaveNode = pModbusInMsg->UI + 0x30;  // nodo Modbus es numero de esclavo SPA    
    SpaChannel = (UINT8)(ModbusOffsetIdx / 10);
    SpaEntity  = ModbusOffsetIdx - (SpaChannel*10);
    SpaIndexMin  = ModbusOffset   - (ModbusOffsetIdx*100);
    SpaIndexMax  = SpaIndexMin + ValuesRequired;
    
    // Pasar al estado de encuestas SPA, hasta recibir todas las variables requeridas
    FirstEventInCurrentState = (EVENT_FUNCTION*)NexusProc_TRXSPA;  
    SpaDataIndex = 0x00;
    PutEventMsg (EVT_NEX_INICMD, PROC_NEXO, PROC_NEXO, 0x00);

   	return;
}
		
		

//-------------- estado : TRXSPA -----------------------------------------------------------
//
//-----------------------------------------------------------------------------
// Enviar respuesta según aplicación
//-----------------------------------------------------------------------------
//
void f_NexusSpaTimeout (void) 
{
    BYTE bTmpIdx;
    
    for (bTmpIdx=0;bTmpIdx<MAX_SPA_DATA_REQUEST;bTmpIdx++)
      SpaData[SpaDataIndex] = (UINT16)0xFFFF;
    
    FirstEventInCurrentState = (EVENT_FUNCTION*)NexusProc_IDLE;  
    PutEventMsg (EVT_NEX_INICMD, PROC_NEXO, PROC_NEXO, 0x01);
    return;
}

//-----------------------------------------------------------------------------
// Enviar próxima encuesta SPA por puerto serie
//-----------------------------------------------------------------------------
//
void f_NexusSpaSendMsg(void) 
{
    UINT8   SpaEntityChar;
    UINT8*  pTxSpaBuffer;
    UINT16  wSenRequestParam;
    UINT8   TmpCntr;
    UINT8   TmpByte;
    UINT8   TmpIdx;
    
  	SPA_TimerIdx =  TMR_SetTimer ( 3000, PROC_NEXO, 0x00, FALSE);

    if ( SpaIndexMin > SpaIndexMax ) return;

    // Obtener el puntero al buffer de transmisión serie para mensaje SPA
    if (MemGetBufferPtr( NEX_SpaMsgToken , &((void*)pTxSpaBuffer) ) != OK ){    
      MON_ResetIfNotConnected(0XB5);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;   
    }
      
    switch ( SpaEntity ) 
    {            
        case 1:
          SpaEntityChar = 'I';        // Input 
          break;
          
        case 2:
          SpaEntityChar = 'O';        // Output
          break;
          
        case 3:
          SpaEntityChar = 'S';        // Setting
          break;

        case 4:
          SpaEntityChar = 'V';        // Variable
          break;
          
        default:
          return;
          break;
        
    }

    pTxSpaBuffer[0] = '>';
    pTxSpaBuffer[1] = SpaSlaveNode;
    pTxSpaBuffer[2] = 'R';
    pTxSpaBuffer[3] = SpaChannel + 0x30;        // pasar unico digito del canal a ASCII
    pTxSpaBuffer[4] = SpaEntityChar;    
    
    if ( SpaIndexMin < 10) 
    {
      pTxSpaBuffer[5] = SpaIndexMin + 0x30;
      TmpIdx = 6;
    }
    else 
    {
      SYS_IntToStr ((UINT16)SpaIndexMin , 0x02,  &(pTxSpaBuffer[5]) );
      TmpIdx = 7;
    }
    
    pTxSpaBuffer[TmpIdx] = ':';
    TmpIdx++;

    TmpByte = 0x00;
    for (TmpCntr=0 ; TmpCntr<TmpIdx ; TmpCntr++) 
    {
        TmpByte ^= pTxSpaBuffer[TmpCntr];
    }
    //pTxSpaBuffer[6] = ':';		// checksum high
    //pTxSpaBuffer[7] = ':';    // checksum low
//    SYS_IntToStr ((UINT16)TmpByte, 0x02,  &(pTxSpaBuffer[TmpIdx]) );
//    TmpIdx++;
//    TmpIdx++;
    
    pTxSpaBuffer[TmpIdx]=SYS_HexChar[TmpByte>>4];
    TmpIdx++;
    pTxSpaBuffer[TmpIdx]=SYS_HexChar[TmpByte & 0x0F];
    TmpIdx++;
    
    
    pTxSpaBuffer[TmpIdx] = 0x0D;   // cr
    TmpIdx++;
//    pTxSpaBuffer[TmpIdx] = 0x0A;   // lf  
//    TmpIdx++;
   
    // longitud de trama SCI, se escribe en el parametro UsedLen del buffer
    if ( MemSetUsedLen(NEX_SpaMsgToken, TmpIdx) != OK){
      MON_ResetIfNotConnected(0XB6);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;   
    }

    wSenRequestParam = 0;    // En la parte alta del parámetro se pasa el offset de transmisión
    wSenRequestParam <<= 8;
    wSenRequestParam |= NEX_SpaMsgToken;

    PutEventMsg (EVT_SCI_TXCMD, NEX_TargetSci, PROC_NEXO, wSenRequestParam );

#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
		  (void)sprintf((char *)udp_buf,"SPA_TX: %s \n",
		                &(pTxSpaBuffer[0]) );
      udp_data_to_send = TRUE;
  }
#endif  		                              

  return; 
}

//-----------------------------------------------------------------------------
// Analizar respuesta SPA recibida por puerto serie
//-----------------------------------------------------------------------------
//
void f_NexusSpaRxMsg (void) 
{

    UINT8   IdxCntr, ItemIdx;
    UINT8   CurrentSpaRxToken; 
    UINT16  CurrentRxSpaLen; 
    UINT8*  pSpaRxMsg;
    float   spafloatval;
    UINT8   dataIntegerLen;
    UINT8   dataDecimalLen;    
    
    CurrentSpaRxToken = (UINT8)CurrentMsg.Param;

		// Bytes recibidos  
    if ( MemGetUsedLen( CurrentSpaRxToken, &CurrentRxSpaLen) != OK){
      MON_ResetIfNotConnected(0XB7);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;
    }
    if (MemGetBufferPtr( CurrentSpaRxToken , &((void*)pSpaRxMsg) ) != OK ){    
      MON_ResetIfNotConnected(0XB8);    // Error de SW, Reset (TODO: revisar acciones a tomar)
      return;   
    }
       
    //------------

#if DEBUG_AT(DBG_LEVEL_3)
  if (SYS_ConfigParam.DebugLevel>= DBG_LEVEL_3)
  {                    
		  (void)sprintf((char *)udp_buf,"SPA_RX: %s \n",
		                &(pSpaRxMsg[1]) );
      udp_data_to_send = TRUE;
  }
#endif  		                              
    
    IdxCntr = 0;
    while ( pSpaRxMsg[IdxCntr] != '<')     // Buscar caracter de inicio
    {
      IdxCntr++;
       if (IdxCntr>MAX_SPA_MSGLEN) return;
    }
    IdxCntr++;   
    while ( pSpaRxMsg[IdxCntr] <= '9')     // Buscar la primera letra
    {
      IdxCntr++;
       if (IdxCntr>MAX_SPA_MSGLEN) return;
    }
    	
    switch (pSpaRxMsg[IdxCntr]) 
    {
        case 'D':
          // ejemplo <XD:10.4:XX[0xD][0xA]
          IdxCntr++;      // primeros ":"
          IdxCntr++;      // comienzo del string correspondiente al valor
          ItemIdx = IdxCntr;  
          while ( pSpaRxMsg[IdxCntr] != ':')     // Buscar el segundo delimitador ":"
          {
               if (IdxCntr>MAX_SPA_MSGLEN) return;
               if (pSpaRxMsg[IdxCntr] == '.')     // punto decimal
               {
                  dataIntegerLen = IdxCntr - ItemIdx;   // Longitud de la parte entera
                  dataDecimalLen = IdxCntr+1; 
                
                }
                IdxCntr++;
             
          }
          IdxCntr -= dataDecimalLen ;    // logitud de decimales del dato en caracteres        
          dataDecimalLen = IdxCntr; 

          spafloatval = SYS_StrToFloat ( dataIntegerLen, dataDecimalLen, &(pSpaRxMsg[ItemIdx]) );   // 1 decimal

          for ( IdxCntr = 0 ; IdxCntr<dataDecimalLen ; IdxCntr++) 
            spafloatval *= 10;  
          
          SpaData[SpaDataIndex] = (UINT16)spafloatval;
      
          break;
      
        default:  
          SpaData[SpaDataIndex] = (UINT16)0xFFFF;
          break;
    }
    
    //------------
    
    SpaDataIndex++;
    if ( SpaDataIndex >= MAX_SPA_DATA_REQUEST)
    {
        // TODO: Procesar error y reinicializar ciclos
        return;        
    }
    SpaIndexMin++;
    if ( SpaIndexMin >= SpaIndexMax ) 
    {
        FirstEventInCurrentState = (EVENT_FUNCTION*)NexusProc_IDLE;  
        PutEventMsg (EVT_NEX_INICMD, PROC_NEXO, PROC_NEXO, 0x00);      
    } 
    else
    {    
//        PutEventMsg (EVT_NEX_INICMD, PROC_NEXO, PROC_NEXO, 0x00);   
  	    (void) TMR_SetTimer ( 30, PROC_NEXO, NextRequestSpa, FALSE);
           
    }    
  
    TMR_FreeTimer (SPA_TimerIdx);
    SPA_TimerIdx = 0xFF;
  
  return; 
}


void NextRequestSpa(void) 
{
      PutEventMsg (EVT_NEX_INICMD, PROC_NEXO, PROC_NEXO, 0x00);   
      return;
}


void f_NexusSpaErr (void)
{
      TMR_FreeTimer (SPA_TimerIdx);
      SPA_TimerIdx = 0xFF;
      PutEventMsg (EVT_NEX_INICMD, PROC_NEXO, PROC_NEXO, 0x00);   
      return;    
}



///////////////////////////////////////////////////////////////////////////////
// Funciones Auxiliares del Proceso
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// MOD_Init : inicializacion de variables del proceso
//-----------------------------------------------------------------------------
//
void NEX_Init(void)
{
    UINT8*  pTxBuffer;
      
    if ( OK != MemGetBuffer((void**)(&pTxBuffer), &NEX_SpaMsgToken)) 
    {
        PutEventMsg (EVT_LOG_ID, PROC_DEBUG, OS_NOT_PROC, (UINT16)LOG_ID_OS_NOBUFFER);    
        MON_ResetIfNotConnected(0XB9);    // Error de SW, Reset (TODO: revisar acciones a tomar)
    } 

		NEX_TargetSci = PROC_SCIA;
    SCIA_Init (PROC_NEXO, SCI_FRAME_SPACOM);
	  return;
}



#pragma CODE_SEG DEFAULT

#endif			// (FW_APP_BUILT == FW_APP_SPA)    Generar aplicación para protocolos SPACOM de ABB
