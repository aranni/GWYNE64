
#ifndef ModbusSlaveTCPProcH
#define ModbusSlaveTCPProcH

//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//

#define MAX_MODBUSTCP_SENDRETRIES   3

//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//

UINT8   ModbusTcpLocalNode; 
UINT16  MOD_MirrorInputOutput[6];

UINT16  ModbusOffset;									  // offset de la operación WR o RD
UINT8   ValuesRequired;                 // cantidad de WORDS solicitadas

UINT8   RetryTimerId;
UINT8   RetryAttemps;  
UINT8   bModbusBusyCounter;

UINT8	            QTCP_MsgQueue  [MAX_QUEUE];
FIFO_CNTRL			  QTCP_QueueIdx;


//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void f_ModbusSlaveTcpIdleQueueMsg (void); 
void f_ModbusSlaveTcpIdleTimeout (void);
void f_ModbusSlaveTcpIdleAnalizeMsg (void);
void f_ModbusSlaveTcpIdleSendMsg(void); 
void f_ModbusSlaveTcpIdleErrMsg(void); 
void f_ModbusSlaveTcpIdleClose(void); 

void MOD_ModbusSlaveTcpInit(void);

RETCODE MOD_SendNextQueuedMsg(void);

#endif
