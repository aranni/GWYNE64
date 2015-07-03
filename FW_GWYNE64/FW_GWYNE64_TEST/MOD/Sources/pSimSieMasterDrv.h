
#ifndef SimSieMasterDrvProcH
#define SimSieMasterDrvProcH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define USS_REPLYID_NOTEXEC     7       // Error: Task cannot be executed
#define SimSMD_TMR_TXREQ_DELAY    20       // Delay entre req. de parametros
#define SimSMD_RXERR_CLEARPARAM    3       // Cantidad de errores consecutivos para borrar params.


#define SimSMD_PAR_RealFrec        2		      // Frecuencia Real Filtrada
#define SimSMD_PAR_OutCurrent 	    4			    // Corriente de salida 
#define SimSMD_PAR_OutVoltage      3				  // Tension de salida
//#define SimSMD_PAR_ConsEnergia     39				// Consumo de energia
#define SimSMD_PAR_ParReal         7				  // Par real filtrado
#define SimSMD_PAR_CorrienteNom    102				// Corriente nominal
#define SimSMD_PAR_Potencia        105				// Potencia nominal
#define SimSMD_PAR_PotenciaReal    5				  // Potencia real
#define SimSMD_PAR_Polos           109				// Polos
#define SimSMD_PAR_UltimaFalla     947				// Ultimo codigo de falla
//#define SimSMD_PAR_HoraFalla       948				// Hora del fallo
#define SimSMD_PAR_ValorFalla      949				// Valor de fallo
#define SimSMD_PAR_HorasRunDias    825 			// Contador de horas funcionando
#define SimSMD_PAR_HorasRunHoras   825   		// Contador de horas funcionando
#define SimSMD_PAR_FrecuenciaNom   107 			// Frecuencia nominal
//#define SimSMD_PAR_MinFrec         1080 			// Frecuencia minima
//#define SimSMD_PAR_MaxFrec         1082 			// Frecuencia maxima


//------------------------------------------------------------------------------------
// Estructuras de DATOS
//------------------------------------------------------------------------------------
//
typedef enum {
	SimSMD_IDX_RealFrec	= 0x00	,
	SimSMD_IDX_OutCurrent 				,
	SimSMD_IDX_OutVoltage 			  ,
//	SimSMD_IDX_ConsEnergia       ,
	SimSMD_IDX_ParReal           ,
	SimSMD_IDX_CorrienteNom      ,
	SimSMD_IDX_Potencia          ,
	SimSMD_IDX_PotenciaReal      ,
	SimSMD_IDX_Polos             ,
	SimSMD_IDX_UltimaFalla       ,
//	SimSMD_IDX_HoraFalla         ,
	SimSMD_IDX_ValorFalla        ,
	SimSMD_IDX_HorasRunDias      ,
	SimSMD_IDX_HorasRunHoras     ,
	SimSMD_IDX_FrecuenciaNom     ,
//	SimSMD_IDX_MinFrec           ,
//	SimSMD_IDX_MaxFrec           ,
	
	SimSMD_IDX_MAX_PARAM_QTTY		,
	
	SimSMD_IDX_PAR_FILL = 0xFF
} SimSMD_IDX_PARAM_ENUM;


const struct _SimSMD_params_codes
	{
	const UINT16  BasicParam;
	const UINT8   ExtendedParam;
	const UINT8   Idx;
	}SimSMD_ParamData[] = {
	{SimSMD_PAR_RealFrec     ,  0x00 ,  0x00	  } ,	  
	{SimSMD_PAR_OutCurrent		,  0x00	,  0x00	  } ,	  
	{SimSMD_PAR_OutVoltage   ,  0x00	,  0x00	  } ,	  
//	{SimSMD_PAR_ConsEnergia	,  0x00 ,  0x00   } ,	  
	{SimSMD_PAR_ParReal		  ,  0x00 ,  0x00   } ,	  
	{SimSMD_PAR_CorrienteNom	,  0x00 ,  0x01   } ,	  
	{SimSMD_PAR_Potencia     ,  0x00 ,  0x01	  } ,	  
	{SimSMD_PAR_PotenciaReal ,  0x00 ,  0x00	  } ,	  
	{SimSMD_PAR_Polos        ,  0x00 ,  0x01	  } ,	  
	{SimSMD_PAR_UltimaFalla  ,  0x00 ,  0x01	  } ,	  
//	{SimSMD_PAR_HoraFalla    ,  0x00 ,  0x00	  } ,	  
	{SimSMD_PAR_ValorFalla   ,  0x00 ,  0x01	  } ,	  
	{SimSMD_PAR_HorasRunDias ,  0x00	,  0x01   } ,   
	{SimSMD_PAR_HorasRunHoras,  0x00	,  0x02   } ,   
	{SimSMD_PAR_FrecuenciaNom,  0x00 ,  0x01	  } ,	  
//	{SimSMD_PAR_MinFrec      ,  0x00 ,  0x00	  } ,
//	{SimSMD_PAR_MaxFrec      ,  0x00 ,  0x00	  } ,
	{0x00                 ,  0x00	,  0x00   } };  // Parametro nulo



//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
UINT8             SimSMD_PollingIdx;
UINT16            SimSMD_CmdPending;
UINT8             SimSMD_RxERRcntr;

SIEMENS_MSG*	   	pSimSieMasterDrvInMsg;					// Respuesta al requerimiento enviado 
SIEMENS_MSG*	   	pSimSieMasterDrvOutMsg;				// Mensaje enviado al variador por Siemens

INV_CONFIG_PARAM* pSimSMD_ConfigParam;
INVERTER_PARAM    SimInverterParam;        // Parametros del inversor
																					 

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
//void f_SimSieMasterDrvIdleTimeout  (void);
void f_SimSieMasterDrvAnalizeMsg   (void);

void SimSMD_Init               (void);
void SimSMD_BuildBasicRequest  (void);
void SimSMD_BuildBCC           (void);
void SimSMD_ReinitParam        (void);


#endif
