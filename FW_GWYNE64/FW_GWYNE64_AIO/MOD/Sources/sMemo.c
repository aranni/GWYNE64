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
        sBuffersPooles[iCounter].fInUse     = FALSE;
        sBuffersPooles[iCounter].vpBuffer   = (void*)(&(saBuffersData[iCounter]));
        sBuffersPooles[iCounter].bToken     = (UINT8)iCounter;
        sBuffersPooles[iCounter].wUsedLen   = 0x00;
        sBuffersPooles[iCounter].bRouteMark = 0xFF;

    }
        
    bMemBuffersInUse = 0x00;    
    return;
}


BYTE MemGetFreeBuffQtty (void)
{
    BYTE bBuffersFree = MAX_POOLES_QTTY - bMemBuffersInUse;
    return bBuffersFree;
}


RETCODE MemGetBuffer ( void** sppBufferPtrAddress, UINT8* bpToken)
{
    BYTE bLoopCounter = 0;

    while(sBuffersPooles[bLoopCounter].fInUse){
        bLoopCounter++;
        if ( bLoopCounter == MAX_POOLES_QTTY ) return ERROR;
    }
    sBuffersPooles[bLoopCounter].fInUse = TRUE;
    *sppBufferPtrAddress = sBuffersPooles[bLoopCounter].vpBuffer;
    *bpToken = bLoopCounter;
    bMemBuffersInUse++;
 
    return OK;
}


RETCODE MemFreeBuffer (UINT8 bToken)
{
    if ( bToken >= MAX_POOLES_QTTY ) return ERROR;
    
    if ( sBuffersPooles[bToken].fInUse ){      
      bMemBuffersInUse--;
      sBuffersPooles[bToken].fInUse = FALSE;
      sBuffersPooles[bToken].bRouteMark = 0xFF;
    }
    return OK;
}

RETCODE MemFreeMarkBuffer (UINT8 bMark)
{
  
    UINT8   iCounter;

    // Creo los pooles de buffers para transmision y recepcion.
    for ( iCounter = 0 ; iCounter < MAX_POOLES_QTTY ; iCounter++ ){
        if (( sBuffersPooles[iCounter].fInUse ) &&
            ( sBuffersPooles[iCounter].bRouteMark == bMark))
            {
                bMemBuffersInUse--;
                sBuffersPooles[iCounter].fInUse = FALSE;
                sBuffersPooles[iCounter].bRouteMark = 0xFF;            
            }

    }
    
    return OK;
}



RETCODE MemGetBufferPtr (UINT8 bToken, void** vppBuffer)
{
    if ( bToken >= MAX_POOLES_QTTY ) return ERROR;

    if ( !sBuffersPooles[bToken].fInUse ) return ERROR;

    *vppBuffer = sBuffersPooles[bToken].vpBuffer;
    return OK;
}


RETCODE MemSetUsedLen (UINT8 bToken, UINT16 wLen)
{

    if ( bToken >= MAX_POOLES_QTTY ) return ERROR;
    
    if ( sBuffersPooles[bToken].fInUse ){      
      sBuffersPooles[bToken].wUsedLen = wLen;
      return OK;
    } 
    else{
      return ERROR;
    }
}


RETCODE MemGetUsedLen (UINT8 bToken,  UINT16* wpLen)
{
    if ( bToken >= MAX_POOLES_QTTY ) return ERROR;
    if ( !sBuffersPooles[bToken].fInUse ) return ERROR;
    
    *wpLen = sBuffersPooles[bToken].wUsedLen;
    return OK;
}

RETCODE MemSetRouteMark (UINT8 bToken, UINT8 bMark)
{

    if ( bToken >= MAX_POOLES_QTTY ) return ERROR;
//    if ( ! bMark) return ERROR;
    
    if ( sBuffersPooles[bToken].fInUse ){      
      sBuffersPooles[bToken].bRouteMark = bMark;
      return OK;
    } 
    else{
      return ERROR;
    }
}


RETCODE MemGetRouteMark (UINT8 bToken,  UINT8* bMark)
{
    if ( bToken >= MAX_POOLES_QTTY ) return ERROR;
    
    if ( sBuffersPooles[bToken].fInUse ){      
      *bMark = sBuffersPooles[bToken].bRouteMark;
      return OK;
    } 
    else{
      return ERROR;
    }
    
}

#pragma CODE_SEG DEFAULT
