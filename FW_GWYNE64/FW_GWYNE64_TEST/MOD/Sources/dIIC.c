#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"

#include "Commdef.h"
#include "dIIC.h"
#include "global.h"


#pragma MESSAGE DISABLE C5919 //WARNING C5919: Conversion of floating to unsigned int

//-----------------------------------------------------------------------------
// Estados del proceso PROC_IIC
//-----------------------------------------------------------------------------
//
const EVENT_FUNCTION	IICProc_IDLE [] =
{
{EVT_TMR_TIMEOUT 	    , f_IICTimeout		},
{EVT_OTHER_MSG     		, f_Consume       }};



//-----------------------------------------------------------------------------
// Transiciones para el proceso PROC_IIC
//-----------------------------------------------------------------------------

//---------- estado : IDLE  ---------------------------


void f_IICTimeout (void)
{
	RxTimerID = (UINT8)(-1);
	TxTimerID = (UINT8)(-1);
	IIC_TransferState = IIC_TFR_ERR;
	return;

}



//-----------------------------------------------------------------------------
// Funciones Auxiliares del Proceso
//-----------------------------------------------------------------------------
//
void IIC_Init (void)
{
 
	IBFD = 0xAD; 										    // frequency divider register
	IBAD = 0xA0;										    // slave address of this module
	IBCR = (IBEN|IBIE); 								// enable Iic module

	TxPacketpositionptr = &TxPacket[0];					// point to first TxPacket element
	TxPacketendptr = &TxPacket[(IIC_TX_PACKETSIZE-1)]; 	// point to last TxPacket element
	TxBufferemptyflag = FALSE;
	RxPacketpositionptr = &RxPacket[0];					// point to first TxPacket element
	RxPacketendptr = &RxPacket[(IIC_RX_PACKETSIZE-1)]; 	// point to last TxPacket element
	RxBufferfullflag = FALSE; 
	
	MasterRxFlag = FALSE;
	RxCompleteflag = FALSE;
	
  (void)IIC_ReceiveMsg (IIC_RTC_ADDRESS, 2);    // Leer el estado del oscilador
	TxPacket[0] = 0x00;                   // dirección 0 bit_7=CH para habilitar oscilador

  if ( (!(RxPacket[0] & 0x80)) &&       // Si estaba habilitado CH=0
       (!(SYS_ConfigParam.RunConfig & CFG_RTC_RUNSTATE_MASK)) ){
    	    TxPacket[1] = 0x80;           // CH = 1 deshabilita oscilador	  
          (void)IIC_SendMsg (IIC_RTC_ADDRESS, 2, 0);
  }
  if ( (RxPacket[0] & 0x80) && 					// si estaba deshabilitado CH=1
       (SYS_ConfigParam.RunConfig & CFG_RTC_RUNSTATE_MASK) ){
      	  TxPacket[1] = 0x00;           // CH = 0 habilita oscilador
          (void)IIC_SendMsg (IIC_RTC_ADDRESS, 2, 0);
  }
  
	TxPacket[0] = 0x00;                   // comando para habilitar DAC_0
	TxPacket[1] = 0x08;                   // POWER DOWN
  (void)IIC_SendMsg (IIC_DAC_ADDRESS, 2, 0);
//	TxPacket[1] = 0x00;                   // registro DAC_0 = 0
//	TxPacket[2] = 0x01;                   // comando para habilitar DAC_1
//	TxPacket[3] = 0x00;                   // registro DAC_1 = 0
//  (void)IIC_SendMsg (IIC_DAC_ADDRESS, 4, 0);
		
}


void IIC_WriteRTCMem (UINT8 Len, UINT8 Addr, UINT8* DataPtr) {
  
 TxPacket[0] = Addr;
 (void) memcpy ( &(TxPacket[1]), DataPtr, Len);
 (void)IIC_SendMsg (IIC_RTC_ADDRESS, (Len+1), 0);
  return; 
}

void IIC_ReadRTCMem (UINT8 Len, UINT8 Addr, UINT8* DataPtr) {
  
 TxPacket[0] = Addr;
 (void)IIC_SendMsg (IIC_RTC_ADDRESS, 1, (Len+1));
 (void) memcpy (DataPtr, &(RxPacket[0]), Len);
  return; 
}

void IIC_RtcGetDateTime (void) {
 
 TxPacket[0] = 0x00;                   // para posicionar puntero de lectura en el origen
 (void)IIC_SendMsg (IIC_RTC_ADDRESS, 1, 7);
 (void) memcpy (&(StatisticData.CurrentDateTime), &(RxPacket[0]), 7);

 return;  
}

void IIC_RtcSetDateTime (void) {
 
 TxPacket[0] = 0x00;                   // para posicionar puntero de lectura en el origen
 (void) memcpy ( &(TxPacket[1]), &(StatisticData.CurrentDateTime), 7);
 (void)IIC_SendMsg (IIC_RTC_ADDRESS, 8, 0);

 return;  
}


void IIC_DacSetOutput (UINT8 Chan, UINT8 OutVal) {
 
 TxPacket[0] = (Chan & 0x01);                   // Canal
 TxPacket[1] = OutVal;                          // Valor del registro
 (void)IIC_SendMsg (IIC_DAC_ADDRESS, 2, 0);
  
 return;  
}


void IIC_DacSetValues (void) {

  float TmpFltVal;
  UINT8 TmpVal;

	TxPacket[0] = 0x00;                   // comando para habilitar DAC_0
  TmpFltVal = (InputOutputData.AAG_Output_A * AAG_4_20_OUT_FACTOR);  
  TmpVal = (UINT8)TmpFltVal;
	TxPacket[1] = TmpVal;                   // registro DAC_0 = 0


	TxPacket[2] = 0x01;                   // comando para habilitar DAC_1
  TmpFltVal = (InputOutputData.AAG_Output_B * AAG_4_20_OUT_FACTOR);  
  TmpVal = (UINT8)TmpFltVal;
	TxPacket[3] = TmpVal;                   // registro DAC_1 = 0

  (void)IIC_SendMsg (IIC_DAC_ADDRESS, 4, 0);
  
}


UINT8 IIC_SendMsg (UINT8 SlaveAddress, UINT8 LenTx, UINT8 LenRx)
{


	MasterRxFlag = FALSE;
	TxPacketpositionptr = &TxPacket[0];			// apuntar al primer elemento de TxPacket
	TxBufferemptyflag = FALSE;
		
	TxPacketendptr = &TxPacket[(LenTx-1)]; 	// apuntar al último elemento a transmitir

	CurrentSlave = SlaveAddress;
	TxMsgLen = LenTx;
	RxMsgLen = LenRx;
	
	IIC_TransferState = IIC_TRANSFERING;    // Estado = Transfiriendo
	TxTimerID =  TMR_SetTimer ( 500 , PROC_IIC, f_IICTimeout, FALSE);

	while(IBSR & IBB){		            // mientras el bus IIC esté ocupado
    if (IIC_TransferState != IIC_TRANSFERING)	
    	return ERROR;   
	}
	
	IBCR = (IBEN|IBIE|MSSL|TXRX);			// grab bus - start condition initiated
	IBDR = SlaveAddress;					    // address the slave
	
	while (IIC_TransferState == IIC_TRANSFERING){ 
    REFRESH_WATCHDOG;
	}

	if ( IIC_TransferState == IIC_TX_OK ){
		TMR_FreeTimer (TxTimerID);				// anular el timer de supervisión de la transmisión
		if (!RxMsgLen)
		  return OK;
		else
  	  return (IIC_ReceiveMsg (CurrentSlave, RxMsgLen));	// ####################			
	}
	
	IIC_Init();
	return ERROR;
	
}


UINT8 IIC_ReceiveMsg (UINT8 SlaveAddress, UINT8 LenRx)
{
	MasterRxFlag = TRUE;
	RxCompleteflag = FALSE;
	RxPacketpositionptr = &RxPacket[0];			// apuntar al primer elemento de RxPacket
	RxBufferfullflag = FALSE; 
	
	RxPacketendptr = &RxPacket[(LenRx-1)]; // apuntar a la ubicación del último elemento a recibir

	IIC_TransferState = IIC_TRANSFERING;		// Estado = Transfiriendo
	RxTimerID =  TMR_SetTimer ( 500 , PROC_IIC, f_IICTimeout, FALSE);
	
	while(IBSR & IBB){		            // mientras el bus IIC esté ocupado
    if (IIC_TransferState != IIC_TRANSFERING)	
    	return ERROR;   
	}
	

	IBCR = (IBEN|IBIE|MSSL|TXRX);			// grab bus - start condition initiated
	IBDR = (SlaveAddress | 0x01);			// address the slave, insruct slave to transmit


	while (IIC_TransferState == IIC_TRANSFERING){
    REFRESH_WATCHDOG;
	}

	if ( IIC_TransferState == IIC_RX_OK ){
		TMR_FreeTimer (RxTimerID);
 		return OK;
	}
	
	IIC_Init();
	return ERROR;

	
}	


// ---------------------------------------------------------------------------
// IIC ISR
// ---------------------------------------------------------------------------
#pragma CODE_SEG NON_BANKED        

interrupt void near IIC_isr (void)
{
  
	volatile UINT8 DummyRead;							// para lecturas dummy IBDR
	
	IBSR |= IBIF;										      // resetear la interrupcion
//	if (IBCR & MSSL)									  // if module is in master mode
//	{
		if (IBCR & TXRX)	// si es irq de transmision	 <<<<<<<<<<<<<<<<<<<<<<<<<<<<
		{
			if (TxCompleteflag) 						  // si el ultimo byte fue transmitido
			{
				TxCompleteflag = FALSE;					// borrar el ISR transmit complete flag
				IBCR = (IBEN|TXRX);						  // enviar mensaje de stop (borrar MSSL)
				TxBufferemptyflag = TRUE;				// setear el non-ISR transmit buffer empty flag

//			PutEventMsg (EVT_IIC_TXEND, PROC_IIC, PROC_IIC, 0x01);		// Enviar End TX event.
  			IIC_TransferState = IIC_TX_OK;
			}	
			else										          // el ultimo byte no fue transmitido aun
			{	
				if (!(IBSR & RXAK))						  // si el esclavo reconocio la ultima transferencia (ACK)
				{
					if (MasterRxFlag)					    // si la ultima transf. no fue una direccion y 
					{                             // el master está listo para Rx
						IBCR &= ~(TXRX); 				    // switch a modo Rx
						DummyRead = IBDR; 				  // lectura dummy de IBDR
					}
					else								          // la ultima transferencia fue para instar otro esclavo a recibir
					{
						IBDR = *TxPacketpositionptr;				// escribir el próximo byte en IBDR
						if (TxPacketpositionptr == TxPacketendptr)	// si el ultimo byte fue transmitido
						{
							TxCompleteflag = TRUE;	  // setear el ISR transmit complete flag
						}
						else										    // aun hay datos para ser transmitidos
						{
							TxPacketpositionptr++;		// apuntar al próximo byte a transmitir
						}
					}
				}
				else										        // el esclavo no reconoció la última transferencia
				{
					IBCR = (IBEN|TXRX);						// enviar mensaje de stop (borrar MSSL)
					
//					TxBufferemptyflag = TRUE;			// ABR: Prueba cuando no hay ack.. 
//					PutEventMsg (EVT_IIC_TXEND, PROC_IIC, PROC_IIC, 0x02);		// Send End TX event.
//        	IIC_TransferState = IIC_TFR_ERR;    // ABR ???
					
				}
			}
		}
		else		        // si es irq de recepción <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		{				
			if(RxPacketpositionptr == RxPacketendptr)		// si el último byte fue recibido
			{
				IBCR = (IBEN);								  // enviar mensaje de stop (borrar MSSL)
				RxCompleteflag = TRUE;					// setear el ISR receive complete flag
			}
			else											        // el último byte no fue recibido aun
			{
				if(RxPacketpositionptr == (RxPacketendptr - 1))	// si el segundo byte fue recibido
				{
					IBCR |= TXAK;						      // deshabilitar el bit "active low acknowledge" (indica al esclavo parar la Tx)
				}
				else									          // segundo último byte debe ser recibido aun
				{										            // no hacer nada
				}
			}
			
			*RxPacketpositionptr = IBDR;			// leer dato proveniente del esclavo
			
			if (RxCompleteflag)							  // si el ultimo byte fue recibido
			{
				RxCompleteflag = FALSE;					// borrar el ISR receive complete flag
				RxBufferfullflag = TRUE;				// setear el flag de receive buffer full

//			PutEventMsg (EVT_IIC_RXEND, PROC_IIC, PROC_IIC, 0x00);		// Send End TX event.
  			IIC_TransferState = IIC_RX_OK;
			}
			else										          // aun hay datos para recibir
			{
				RxPacketpositionptr++;					// incrementar el punter a la posicion del Rx packet 
			}	
		}
//	}						
}


#pragma CODE_SEG DEFAULT
