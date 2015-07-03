
#ifndef HitSJ300H
#define HitSJ300H


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define TMR_TXPOLL_DELAY      500     // Delay entre pollings al variador
#define HSJ_TMR_TXREQ_DELAY   50      // Delay entre req. de parametros


#define HSJ_START_FLAG        STX
#define HSJ_END_FLAG          0x0D
#define HSJ_DEFAULT_SLAVE     "01"

#define HSJ_CMD_RUNSTOP         "00"	      // Arranque - Parada
#define HSJ_CMD_SETFREC         "01"	      // Seteo de frecuencia
#define HSJ_CMD_READPARAMS      "03"	      // Lectura de conjunto de parametros principales
#define HSJ_CMD_READSTATUS      "04"	      // Lectura de estado del variador
#define HSJ_CMD_READTRIP        "05"	      // Lectura de historico de fallas
#define HSJ_CMD_READONEPAR      "06"	      // Lectura de un solo parametro
#define HSJ_CMD_SETONEPAR       "07"	      // Escritura de un parametro

#define HSJ_PAR_MAXFREC         "A061"	    // Parametro Frecuencia Maxima
#define HSJ_PAR_MINFREC         "A062"	    // Parametro Frecuencia Minima
#define HSJ_PAR_POLOS           "H004"	    // Parametro Nro. de pares de polos
#define HSJ_PAR_POTNOM          "H003"	    // Parametro potencia del motor
#define HSJ_PAR_NONE            "    "	    // Lectura de estado no tiene parametro

#define HSJ_PAR_FORWARD         "1   "
#define HSJ_PAR_REVERSE         "2   "
#define HSJ_PAR_STOP            "0   "

#define HSJ_MAX_POTCODES        22
const float MotorPotCodes[] = {     	  
  0.2,
	0.37,
	0.4,
	0.55,
	0.75,
	1.1,
	1.5,
	2.2,
	3.0,
	3.7,
	4.0,
	5.5,
	7.5,
	11,
	15,
	18.5,
	22,
	30,
	37,
	45,
	55,
	75,
  };


//------------------------------------------------------------------------------------
// Estructuras de DATOS
//------------------------------------------------------------------------------------
//
typedef enum {
	IDX_HSJ_PAR_STATUS	   = 0x00   ,
	IDX_HSJ_PAR_MAXFREC	        	  ,
	IDX_HSJ_PAR_MINFREC	        	  ,
	IDX_HSJ_PAR_READPARAMS	        ,
	IDX_HSJ_PAR_POLOS               ,
	IDX_HSJ_PAR_POTNOM              ,
	IDX_HSJ_PAR_READTRIP            ,   // encuestar siempre al final..
	
	IDX_HSJ_MAX_PARAM		      ,
	
	IDX_HSJ_PAR_FORWARD       ,
	IDX_HSJ_PAR_REVERSE       ,
	IDX_HSJ_PAR_STOP          ,
	IDX_HSJ_PAR_SETFREC       ,
  
	IDX_HSJ_PAR_FILL = 0xFF
} IDX_HSJ_PARAM_ENUM;

//	IDX_HSJ_PAR_SETF001       ,
//	IDX_HSJ_READTIME			  ,
//	IDX_HSJ_READTRIP			  ,

#define IDX_HSJ_FirstCommand  IDX_HSJ_PAR_STATUS
#define HSJ_CMD_PARLEN     4

const struct _hsj_params_codes
	{
	const UINT8   Command[HSJ_CMD_LEN+1];
	const UINT8   CmdPar[HSJ_CMD_PARLEN+1];
	const UINT8   TransmitLen;
	const UINT8   ResponseLen;
	const UINT8   CmdId;
	}HSJ_ParamData[] = {
	{HSJ_CMD_READSTATUS     ,HSJ_PAR_NONE       ,8     ,  14	, IDX_HSJ_PAR_STATUS    } ,	  
	{HSJ_CMD_READONEPAR     ,HSJ_PAR_MAXFREC    ,12    ,  15	, IDX_HSJ_PAR_MAXFREC   } ,	  
	{HSJ_CMD_READONEPAR     ,HSJ_PAR_MINFREC    ,12    ,  15	, IDX_HSJ_PAR_MINFREC   } ,	  
	{HSJ_CMD_READPARAMS     ,HSJ_PAR_NONE       ,8     ,  110	, IDX_HSJ_PAR_READPARAMS} ,	  
	{HSJ_CMD_READONEPAR     ,HSJ_PAR_POLOS      ,12    ,  15	, IDX_HSJ_PAR_POLOS     } ,	  
	{HSJ_CMD_READONEPAR     ,HSJ_PAR_POTNOM     ,12    ,  15	, IDX_HSJ_PAR_POTNOM    } ,	  
	{HSJ_CMD_READTRIP       ,HSJ_PAR_NONE       ,8     ,  110	, IDX_HSJ_PAR_READTRIP  } ,	  
	{0x00                    ,0x00              ,0x00 ,  0x00 , IDX_HSJ_MAX_PARAM     } ,	  
	{HSJ_CMD_RUNSTOP         ,HSJ_PAR_FORWARD   ,9    ,  7	  , IDX_HSJ_PAR_FORWARD   } ,	  
	{HSJ_CMD_RUNSTOP         ,HSJ_PAR_REVERSE   ,9    ,  7	  , IDX_HSJ_PAR_REVERSE   } ,	  
	{HSJ_CMD_RUNSTOP         ,HSJ_PAR_STOP      ,9    ,  7	  , IDX_HSJ_PAR_STOP      } ,	  
	{HSJ_CMD_SETFREC         ,HSJ_PAR_NONE      ,14   ,  7	  , IDX_HSJ_PAR_SETFREC   } ,	  
	{0x00                    ,0x00             ,0x00  ,  0x00	, IDX_HSJ_PAR_FILL    } };  // Parametro nulo

//	{HSJ_CMD_SETONEPAR       ,HSJ_PAR_REALFREC  ,20   ,  7	  , IDX_HSJ_PAR_SETF001   } ,	  
//	{HSJ_CMD_READTIME        ,10     ,  30	  , IDX_HSJ_READTIME    } ,	  
//	{HSJ_CMD_READTRIP        ,10     ,  83	  , IDX_HSJ_READTRIP    } ,	  
//	{HSJ_CMD_DEBUG           ,10     ,  30	  , IDX_HSJ_CMD_DEBUG   } ,	  

													
//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
UINT8             HSJ_PollingIdx;
HITSJ300_IN_MSG*	pHitSJ300InMsg;				// Respuesta al requerimiento enviado 
HITSJ300_OUT_MSG* pHitSJ300OutMsg;			// Mensaje enviado al variador por Hitachi
float             HSJ_FloatVal;
UINT8             HSJ_CmpPending;


//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void f_HitSJ300IdleTimeout (void);
void f_HitSJ300AnalizeMsg  (void);
void f_HitSJ300CmdStart    (void);
void f_HitSJ300CmdStop     (void);
void f_HitSJ300CmdSetFrec  (void);
void f_HitSJ300CmdDebug    (void);

void    HSJ_Init               (void);
void    HSJ_BuildBasicRequest  (UINT8 ListIdx);
void    HSJ_BuildBCC           (void);

#endif
