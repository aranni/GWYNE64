
#ifndef ModbusMasterTCPProcH
#define ModbusMasterTCPProcH

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
void f_ModbusMasterTcpIdleQueueMsg (void); 
void f_ModbusMasterTcpIdleTimeout (void);
void f_ModbusMasterTcpIdleAnalizeMsg (void);
void f_ModbusMasterTcpIdleSendMsg(void); 
void f_ModbusMasterTcpIdleErrMsg(void); 
void f_ModbusMasterTcpIdleClose(void); 

void MOD_ModbusMasterTcpInit(void);

RETCODE MOD_SendNextQueuedMsg(void);

#endif
