
#ifndef pNexusProcH
#define pNexusProcH

//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define NEX_PERIODIC_TIMERVAL      5000 			// periodo de consulta de elementos sin enviar y sin ack

#define BIO_REVPOLY  0x1021   // Polinomio generador para desplazamiento hacia la derecha

#define SIZE_FRAME_A  10
const UINT8 BioFrameA[] = {    
  0xA5,0x00,0x00,0x00,0x00,0x3C,0x00,0x00,0xF2,0xB5, 	  
};  

#define SIZE_FRAME_B  12
const UINT8 BioFrameB[] = {    
  0xA5,0x00,0x00,0x00,0x00,0x40,0x00,0x02,0x02,0x01,0xDC,0x58,
};  

#define SIZE_FRAME_C  14
const UINT8 BioFrameC[] = {    
  0xA5,0x00,0x00,0x00,0x00,0x4E,0x00,0x04,0x02,0x00,0x00,0x01,0xBA,0xDE,
};  
 

//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
BYTE  NEX_BioMsgToken; 
BYTE  NEX_TargetSci; 
BYTE  NEX_TimerId; 

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void f_NexusIdleTimeout (void);

void f_NexusIdleIoChangeOn  (void);
void f_NexusIdleIoChangeOff (void);
void f_NexusIdleIoStateOn  (void);
void f_NexusIdleIoStateOff (void);
void f_NexusIdleIoPulse     (void);

void f_NexusTRXErr (void);
void f_NexusTRXARxMsg(void);
void f_NexusTRXBRxMsg(void); 
void f_NexusTRXCRxMsg(void); 
void f_NexusBioSendQuery(const UINT8* pcBioFrame, UINT8 bBioFrameSize);

void  NEX_Init(void);
void  NEX_BioBuildCRC16 ( UINT8 * puchMsg, UINT16 usDataLen );
void xorMsg(UINT8* pReadLogBuffer); 

#endif
