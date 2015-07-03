#ifndef sQueueH
#define sQueueH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define MAX_SRLIN_QUEUE       MAX_QUEUE


//------------------------------------------------------------------------------------
// Estructuras de DATOS
//------------------------------------------------------------------------------------
//


//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
#pragma CODE_SEG NON_BANKED        

UINT8 QUE_GetByte             (FIFO_CNTRL* qIdx, UINT8* BytePtr);
UINT8 QUE_GetByteAndQuit      (FIFO_CNTRL* qIdx, UINT8* BytePtr);
UINT8 QUE_PutByte             (FIFO_CNTRL* qIdx, UINT8 DataByte);

void  QUE_Init                (FIFO_CNTRL* qIdx);

#pragma CODE_SEG DEFAULT

#endif    // sQueueH
