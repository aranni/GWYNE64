#include "MC9S12NE64.h"
#include "datatypes.h"

#include "Commdef.h"
#include "sMemo.h"
#include "Global.h"

#pragma CODE_SEG NON_BANKED

// ---------------------------------------------------------------------------
// Inicializacion de Manager de MEMORIA
// ---------------------------------------------------------------------------
//
void SYS_MemoInit(void)
{
  UINT8   iCounter;

    // Creo los pooles de buffers para transmision y recepcion.
    for ( iCounter = 0 ; iCounter < MAX_POOLES_QTTY ; iCounter++ ){
        sBuffersPooles[iCounter].fInUse    = FALSE;
        sBuffersPooles[iCounter].vpBuffer  = (void*)(&(saBuffersData[iCounter]));
        sBuffersPooles[iCounter].bToken   = (UINT8)iCounter;
    }
        
    bMemBuffersInUse = 0x00;    
    return;
}


RETCODE MemGetBuffer ( void** sppBufferPtrAddress, UINT8* bpToken){
    BYTE bLoopCounter = 0;

    while(sBuffersPooles[bLoopCounter].fInUse){
        bLoopCounter++;
        if ( bLoopCounter == MAX_POOLES_QTTY ) return ERROR;
    }
    sBuffersPooles[bLoopCounter].fInUse = TRUE;
    *sppBufferPtrAddress = sBuffersPooles[bLoopCounter].vpBuffer;
    *bpToken = bLoopCounter;
    bMemBuffersInUse++;
    if (StatisticData.OsStat.WatchRstTmr < bMemBuffersInUse)
      StatisticData.OsStat.WatchRstTmr = bMemBuffersInUse;

    return OK;
}


RETCODE MemFreeBuffer (UINT8 bToken){

    if ( sBuffersPooles[bToken].fInUse ){      
      bMemBuffersInUse--;
      sBuffersPooles[bToken].fInUse = FALSE;
    }
    return OK;
}


RETCODE MemGetBufferPtr (UINT8 bToken, void** vppBuffer){

    if ( !sBuffersPooles[bToken].fInUse )
            return ERROR;

    *vppBuffer = sBuffersPooles[bToken].vpBuffer;
    return OK;
}


#pragma CODE_SEG DEFAULT


