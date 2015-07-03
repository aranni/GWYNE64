
#ifndef sMemoH
#define sMemoH

//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
  
#define MAX_POOLES_QTTY  		  (3+MAX_SRV_TCPSOCH)   // antes 5

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

RETCODE MemGetBuffer    ( void** sppBufferPtrAddress, UINT8* bpToken);
RETCODE MemFreeBuffer   (UINT8 bToken);
RETCODE MemGetBufferPtr (UINT8 bToken, void** vppBuffer);

#pragma CODE_SEG DEFAULT


#endif				 // sMemoH