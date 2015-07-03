#include "MC9S12NE64.h"
#include "datatypes.h"
#include "stdio.h"

#include "Commdef.h"
#include "sQueue.h"
#include "Global.h"

													
#pragma CODE_SEG NON_BANKED        


///////////////////////////////////////////////////////////////////////////////
// Funciones de sistema
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// QUE_Init : inicializacion de variables del proceso
//-----------------------------------------------------------------------------
//
void QUE_Init(FIFO_CNTRL* qIdx)
{


	qIdx->Counter 	= 0x00;
	qIdx->IndexOut	= 0x00;
	qIdx->IndexIn	  = 0x00;  

//	QUE_QueueIdx.pQData   = QUE_MsgQueue;
//	QUE_QueueIdx.Counter 	= 0x00;
//	QUE_QueueIdx.IndexOut	= 0x00;
//	QUE_QueueIdx.IndexIn	= 0x00;  

}


//-----------------------------------------------------------------------------
// Sacar un mensaje de la cola IN
//-----------------------------------------------------------------------------
//
UINT8 QUE_GetByteAndQuit (FIFO_CNTRL* qIdx, UINT8* BytePtr)
{
	if (qIdx->Counter) {
		*BytePtr     = qIdx->qData[qIdx->IndexOut];
		
     __asm SEI;                         // Disable Interrupts
		qIdx->IndexOut++;										
		if (qIdx->IndexOut == MAX_SRLIN_QUEUE) qIdx->IndexOut = 0x00;				
		qIdx->Counter--;							    
     __asm CLI;                         // Enable Interrupts
		return TRUE;
	}
	return FALSE;
}

UINT8 QUE_GetByte (FIFO_CNTRL* qIdx, UINT8* BytePtr)
{
	if (qIdx->Counter) {
		*BytePtr     = qIdx->qData[qIdx->IndexOut];
	
		return TRUE;
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
// Guardar un mensaje en la cola IN
//-----------------------------------------------------------------------------
//
UINT8 QUE_PutByte (FIFO_CNTRL* qIdx, UINT8 DataByte)
{
	if (qIdx->Counter < MAX_SRLIN_QUEUE) {
		qIdx->qData[qIdx->IndexIn]  = DataByte;
		
		qIdx->IndexIn++;
		if (qIdx->IndexIn == MAX_SRLIN_QUEUE) qIdx->IndexIn = 0x00;

		qIdx->Counter++;
		
		return qIdx->Counter;
	}
	
	return FALSE;
}	




#pragma CODE_SEG DEFAULT

