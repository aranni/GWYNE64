#ifndef DnpH
#define DnpH


#define DNP_COOPER_OLD_VER           // Habilitar para conectar a versiones mas viejas de F5 
//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define TMR_TXPOLL_DELAY      500     // Delay entre pollings al variador
#define TMR_TXREQ_DELAY       10      // Delay entre req. de parametros
#define DNP_TMR_TXREQ_DELAY   50      // Delay entre req. de parametros

#define MAX_DNPMSG_QUEUE      2
#define MAX_DNP_TXATTEMP     10       // Transmisiones sin respuesta antes de enviar ResetLink

#define DNP_CMDMASK_FLAG   0x0F				// Mascara para recuperar parametro del comando
#define DNP_CMDFLAG_ON     0x01				// Flag para marcar comando de ON pendiente
#define DNP_CMDFLAG_OFF    0x02       // Flag para marcar comando de OFF pendiente

#define DNP_CMDMASK_INS    0xF0				// Mascara para recuperar instancia de comando
#define DNP_CMDFLAG_I0     0x10				// Flag para marcar primera instancia del comando
#define DNP_CMDFLAG_I1     0x20				// Flag para marcar segunda instancia del comando

typedef enum {
	DNP_POLIDX_AAGINP    = 0x00	,
	DNP_POLIDX_BININP           ,
	DNP_POLIDX_BINOUT           ,
	DNP_POLIDX_COUNTR						,

	DNP_POLIDX_MAX_QTTY         ,
		
	DNP_POLIDX_FILL = 0xFF
} DNP_POLIDX_ENUM;

const struct _dnp_poling_talbe
	{
	const UINT8   PolingIndex;
	const UINT8   OG;
	const UINT8   OV;
	const UINT8   QL;
	const UINT8   I1;
	const UINT8   I2;
	const UINT8   DataObjToRx;
	}DNP_PolingTable[] = {
#ifdef DNP_COOPER_OLD_VER		// Version mas vieja de reconectadores
	{DNP_POLIDX_AAGINP,  30,  4,  0,  0,   64,  65 } ,
	{DNP_POLIDX_BININP,   1,  1,  0,  0,   63,   8 } ,	  
	{DNP_POLIDX_BINOUT,  10,  2,  0,  0,  113,  15 } ,
	{DNP_POLIDX_COUNTR,  20,  6,  0,  0,   28,  29 } ,
	{DNP_POLIDX_MAX_QTTY, 0,  0,  0,  0,    0,   0 } };  // Fin de la lista
#else
	{DNP_POLIDX_AAGINP,  30,  4,  0,  0,  106, 107 } ,
	{DNP_POLIDX_BININP,   1,  1,  0,  0,   63,   8 } ,	  
	{DNP_POLIDX_BINOUT,  10,  2,  0,  0,  127,  16 } ,
	{DNP_POLIDX_COUNTR,  20,  6,  0,  0,   28,  29 } ,
	{DNP_POLIDX_MAX_QTTY, 0,  0,  0,  0,    0,   0 } };  // Fin de la lista
#endif


const float AagInpScale[] = {    
	  0.1,      // 00-Instantaneoues current through bushing 1-2
	  0.1,      // 01-Instantaneoues current through bushing 3-4
	  0.1,      // 02-Instantaneoues current through bushing 5-6
	  0.1,      // 03-Instantaneoues ground current

	    2,      // 04-
	    2,      // 05-
	    2,      // 06-

 0.0001,      // 07-
	    1,      // 08-
	    1,      // 09-
	    1,      // 10-

 0.0001,      // 11-
	    1,      // 12-
	    1,      // 13-
	    1,      // 14-

 0.0001,      // 15-
	    1,      // 16-
	    1,      // 17-
	    1,      // 18-

 0.0001,      // 19-
	    1,      // 20-
	    1,      // 21-
	    1,      // 22-

   0.01,      // 23-
   0.01,      // 24-
   0.01,      // 25-
   0.01,      // 26-

   0.01,      // 27-
   0.01,      // 28-
   0.01,      // 29-

   0.01,      // 30-
 0.0001,      // 31-

    0.1,      // 32-
    0.1,      // 33-
    0.1,      // 34-
    0.1,      // 35-

	    2,      // 36-
	    2,      // 37-
	    2,      // 38-

 0.0001,      // 39-
	    1,      // 40-
	    1,      // 41-
	    1,      // 42-

 0.0001,      // 43-
	    1,      // 44-
	    1,      // 45-
	    1,      // 46-

 0.0001,      // 47-
	    1,      // 48-
	    1,      // 49-
	    1,      // 50-

 0.0001,      // 51-
	    1,      // 52-
	    1,      // 53-
	    1,      // 54-

   0.01,      // 55-
   0.01,      // 56-
   0.01,      // 57-
   0.01,      // 58-

   0.01,      // 59-
   0.01,      // 60-
   0.01,      // 61-

   0.01,      // 62-
   0.01,      // 63-
   0.01,      // 64-

    500,      // 65-
    500,      // 66-
    500,      // 67-
    500,      // 68-

	    2,      // 69-
	    2,      // 70-
	    2,      // 71-

	    2,      // 72-
	    2,      // 73-
	    2,      // 74-

	    2,      // 75-
	    2,      // 76-
	    2,      // 77-

	    2,      // 78-
	    2,      // 79-
	    2,      // 80-

	    2,      // 81-
	    2,      // 82-
	    2,      // 83-

	    2,      // 84-
	    2,      // 85-
	    2,      // 86-

  0.005,      // 87-

	    2,      // 88-
	    2,      // 89-
	    2,      // 90-

	    2,      // 91-
	    2,      // 92-
	    2,      // 93-


	    1,      // 94-
	    1,      // 95-
	    1,      // 96-
	    1,      // 97-
	    1,      // 98-
	    1,      // 99-
	    1,      // 100-
	    1,      // 101-
	    1,      // 102-
	    1,      // 101-
	    1,      // 104-

	    1,      // 105-

	 0.01,      // 106-

};

//------------------------------------------------------------------------------------
// Estructuras de DATOS
//------------------------------------------------------------------------------------
//
typedef struct _APU_FOMRAT_CRTLRELAY {
    BYTE    ControlCode;
    BYTE    Count;
    UINT32  OnTime;
    UINT32  OffTime;
    BYTE    Status;
    WORD    CRC_Place;  // ABR.. prueba transitoria.. CORREGIR.  
} APU_FOMRAT_CRTLRELAY,*LPAPU_FOMRAT_CRTLRELAY;



//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
DNP_DATA_MSG*	    pDnpInMsg;				// Respuesta al requerimiento enviado 
DNP_DATA_MSG*     pDnpOutMsg;				// Mensaje enviado al equipo de medicion
UINT8             DNP_CmdPendingFlag;
UINT8             DNP_CmdPendingIdx;
UINT8             DNP_PolingTableIdx;

UINT8             APDU_WaitResponseTmrId;
UINT8             TPDU_SeqNumber;
UINT8             APDU_SeqNumber;
UINT8             DNP_TxNoAnswerAcc;									// Acumulador de mensajes sin respuesta

//DNP_MSG	        DnpMsgQueue [MAX_DNPMSG_QUEUE];		  // Cola de Mensajes
//FIFO_CNTRL			DnpQueueIdx;		 				          	// Variables para control 


//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//

void f_DnpInitTimeout       (void);
void f_DnpInitDrvException  (void);

void f_DnpIdleTimeout       (void);
void f_DnpPollingMsg        (void);
void f_DnpAnalizeMsg        (void);
void f_DnpSetOutputOn       (void);
void f_DnpSetOutputOff      (void);

void f_DnpReinitPoll        (void);

//UINT8 DnpGetMsg             (void);
//UINT8 DnpGetMsgAndQuit      (void);
//void  DnpPutMsg             (UINT8 Data_0, UINT8 Data_1);

void  DNP_Init              (void);
void  DNP_ReInit            (void);

#endif
