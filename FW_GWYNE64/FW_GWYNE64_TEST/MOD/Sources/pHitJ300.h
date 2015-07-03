
#ifndef HitJ300H
#define HitJ300H


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define TMR_TXPOLL_DELAY      500     // Delay entre pollings al variador
#define TMR_TXREQ_DELAY       10      // Delay entre req. de parametros
#define HIT_TMR_TXREQ_DELAY   50      // Delay entre req. de parametros


#define HIT_START_FLAG        STX
#define HIT_END_FLAG          0x0D
#define HIT_DEFAULT_SLAVE     "01"

#define HIT_CMD_StatMonitor     "MR  "	  // Monitor de estado
#define HIT_CMD_MinMaxFrec      "FR26"	  // Frecuencias minima y maxima

#define HIT_CMD_RUN             "CW  "	  // Arranque - Parada
#define HIT_CMD_SETFREC         "PWFS"	  // Seteo de frecuencia
#define HIT_CMD_READTIME        "PRDB"	  // Lectura de horas run
#define HIT_CMD_READTRIP        "PRER"	  // Lectura de causas de falla
//#define HIT_CMD_SETMODE         "PWCS"	  // Seteo de modo de control
#define HIT_CMD_DEBUG           "FR46"	  //

#define CMD_RUN_FORWARD         'F'
#define CMD_RUN_REVERSE         'R'
#define CMD_RUN_STOP            'S'


//------------------------------------------------------------------------------------
// Estructuras de DATOS
//------------------------------------------------------------------------------------
//
typedef enum {
	IDX_HIT_Monitor	= 0x00	,
	IDX_HIT_MinMaxFrec      ,
	IDX_HIT_READTIME			  ,
	IDX_HIT_READTRIP			  ,
//	IDX_HIT_CMD_SETMODE     ,
	
	IDX_HIT_MAX_PARAM		    ,
	IDX_HIT_CMD_RUN         ,
	IDX_HIT_CMD_SETFREC     ,
  IDX_HIT_CMD_DEBUG  	    ,
  
	IDX_HIT_PAR_FILL = 0xFF
} IDX_HIT_PARAM_ENUM;


const struct _hit_params_codes
	{
	const UINT8   Command[5];
	const UINT8   TransmitLen;
	const UINT8   ResponseLen;
	const UINT8   CmdId;
	}HIT_ParamData[] = {
	{HIT_CMD_StatMonitor     ,10    ,  54	  , IDX_HIT_Monitor     } ,	  
	{HIT_CMD_MinMaxFrec      ,10    ,  21	  , IDX_HIT_MinMaxFrec  } ,	  
	{HIT_CMD_READTIME        ,10    ,  30	  , IDX_HIT_READTIME    } ,	  
	{HIT_CMD_READTRIP        ,10    ,  83	  , IDX_HIT_READTRIP    } ,	  
	{0x00                    ,0x00  ,  0x00 , IDX_HIT_MAX_PARAM   } ,	  
	{HIT_CMD_RUN             ,11    ,  12	  , IDX_HIT_CMD_RUN     } ,	  
	{HIT_CMD_SETFREC         ,16    ,  12	  , IDX_HIT_CMD_SETFREC } ,	  
	{HIT_CMD_DEBUG           ,10    ,  30	  , IDX_HIT_CMD_DEBUG   } ,	  
	{0x00                    ,0x00  ,  0x00	, IDX_HIT_PAR_FILL    } };  // Parametro nulo

//	{HIT_CMD_SETMODE         ,16    ,  12	  , IDX_HIT_CMD_SETMODE } ,	  


//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
UINT8             HIT_PollingIdx;
HITACHI_IN_MSG*	  pHitachiInMsg;				// Respuesta al requerimiento enviado 
HITACHI_OUT_MSG*  pHitachiOutMsg;				// Mensaje enviado al variador por Hitachi
float             HIT_FloatVal;
UINT8             HIT_CmpPending;


//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void f_HitachiIdleTimeout (void);
void f_HitachiAnalizeMsg  (void);
void f_HitachiCmdStart    (void);
void f_HitachiCmdStop     (void);
void f_HitachiCmdSetFrec  (void);
void f_HitachiCmdDebug    (void);

void    HIT_Init               (void);
void    HIT_BuildBasicRequest  (UINT8 ListIdx);
void    HIT_BuildBCC           (void);

UINT8   LookForCmdId           (void);

#endif
