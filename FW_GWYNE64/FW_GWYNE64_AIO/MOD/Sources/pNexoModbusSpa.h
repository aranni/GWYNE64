
#ifndef pNexusProcH
#define pNexusProcH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//

#define MODBUS_RTU_REVPOLY  0xA001   // Polinomio generador para desplazamiento hacia la derecha
#define MAX_SPA_DATA_REQUEST    20

//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
BYTE  NEX_TargetSci; 
BYTE  NEX_ModbusTcpRequestSocketIdx;    // Index del socket por donde llegó el requerimiento
BYTE  NEX_ModbusTcpRequesrToken;        // 
BYTE  NEX_SpaMsgToken;
BYTE  SPA_TimerIdx; 

UINT8   SpaSlaveNode;
UINT8   SpaChannel;
UINT8   SpaEntity;
UINT8   SpaIndexMin;
UINT8   SpaIndexMax;
UINT8   SpaDataIndex;
UINT16  SpaData[MAX_SPA_DATA_REQUEST];

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void f_NexusIdleTimeout (void);
void f_NexusAnalizeMsg (void);
void f_NexusAnswerSpa (void);


void f_NexusSpaSendMsg(void); 
void f_NexusSpaTimeout (void); 
void f_NexusSpaRxMsg (void); 
void f_NexusSpaErr (void);

void NextRequestSpa(void);

void NEX_Init(void);


#endif
