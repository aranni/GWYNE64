
#ifndef sMemoH
#define sMemoH

//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
  
//#define MAX_POOLES_QTTY  		  (3+MAX_SRV_TCPSOCH)

#define POOLES_MEM_USED         ( MAX_POOLES_QTTY * (MEM_MAX_BUF_LEN+2) )
#if (POOLES_MEM_USED > 2150)
    #error MEMMORY MANAGER ERR: demasiada memoria requerida por buffers (ver sMemo.h)
#endif

//------------------------------------------------------------------------------------
// Estructuras de DATOS
//------------------------------------------------------------------------------------
//

//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
RTS_BUFFERS_POOL    sBuffersPooles[MAX_POOLES_QTTY];
MEM_BUF_FORMAT      saBuffersData[MAX_POOLES_QTTY];

UINT8               bMemBuffersInUse;

#pragma CODE_SEG NON_BANKED

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void    SYS_MemoInit      (void);

BYTE    MemGetFreeBuffQtty  (void);
RETCODE MemGetBuffer        (void** sppBufferPtrAddress, UINT8* bpToken);
RETCODE MemFreeBuffer       (UINT8 bToken);
RETCODE MemFreeMarkBuffer   (UINT8 bMark);
RETCODE MemGetBufferPtr     (UINT8 bToken, void** vppBuffer);
RETCODE MemSetUsedLen       (UINT8 bToken, UINT16 wLen);
RETCODE MemGetUsedLen       (UINT8 bToken,  UINT16* wpLen);
RETCODE MemSetRouteMark     (UINT8 bToken, UINT8 bMark);
RETCODE MemGetRouteMark     (UINT8 bToken,  UINT8* bMark);

#pragma CODE_SEG DEFAULT


#endif				 // sMemoH