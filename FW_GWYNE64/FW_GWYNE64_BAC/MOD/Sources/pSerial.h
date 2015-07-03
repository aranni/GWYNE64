#ifndef SerialH
#define SerialH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define TMR_TXPOLL_DELAY      500     // Delay entre pollings al variador
#define TMR_TXREQ_DELAY       10      // Delay entre req. de parametros
#define DNP_TMR_TXREQ_DELAY   50      // Delay entre req. de parametros

#define MAX_SRLIN_QUEUE       30
#define MAX_SRLOUT_QUEUE      10



//------------------------------------------------------------------------------------
// Estructuras de DATOS
//------------------------------------------------------------------------------------
//


//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
UINT8	            SerialInMsgQueue  [MAX_SRLIN_QUEUE];	  // Cola de bytes, de SCI a TCP
UINT8	            SerialOutMsgQueue [MAX_SRLOUT_QUEUE];	  // Cola de bytes, de TCP a SCI
FIFO_LARGE_CNTRL  SerialInQueueIdx;		 				          	// Variables para control 
FIFO_CNTRL			  SerialOutQueueIdx;		 				          // Variables para control 


//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
#pragma CODE_SEG NON_BANKED        

void f_SerialIdleTimeout      (void);
void f_SerialPollingMsg       (void);
void f_SerialOutMsg           (void);


UINT8 SerialInGetByte             (UINT8* BytePtr);
UINT8 SerialInGetByteAndQuit      (UINT8* BytePtr);
void  SerialInPutByte             (UINT8 DataByte);
UINT8 SerialOutGetByte            (UINT8* BytePtr);
UINT8 SerialOutGetByteAndQuit     (UINT8* BytePtr);
void  SerialOutPutByte            (UINT8 DataByte);

void  SRL_Init              (void);
void  SRL_ReInit            (void);

#pragma CODE_SEG DEFAULT

#endif
