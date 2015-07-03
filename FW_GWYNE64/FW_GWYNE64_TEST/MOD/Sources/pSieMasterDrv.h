
#ifndef SieMasterDrvProcH
#define SieMasterDrvProcH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define USS_REPLYID_NOTEXEC     7       // Error: Task cannot be executed
#define SMD_TMR_TXREQ_DELAY    20       // Delay entre req. de parametros
#define SMD_RXERR_CLEARPARAM    3       // Cantidad de errores consecutivos para borrar params.


#define SMD_PAR_RealFrec        2		      // Frecuencia Real Filtrada
#define SMD_PAR_OutCurrent 	    4			    // Corriente de salida 
#define SMD_PAR_OutVoltage      3				  // Tension de salida
//#define SMD_PAR_ConsEnergia     39				// Consumo de energia
#define SMD_PAR_ParReal         7				  // Par real filtrado
#define SMD_PAR_CorrienteNom    102				// Corriente nominal
#define SMD_PAR_Potencia        105				// Potencia nominal
#define SMD_PAR_PotenciaReal    5				  // Potencia real
#define SMD_PAR_Polos           109				// Polos
#define SMD_PAR_UltimaFalla     947				// Ultimo codigo de falla
//#define SMD_PAR_HoraFalla       948				// Hora del fallo
#define SMD_PAR_ValorFalla      949				// Valor de fallo
#define SMD_PAR_HorasRunDias    825 			// Contador de horas funcionando
#define SMD_PAR_HorasRunHoras   825   		// Contador de horas funcionando
#define SMD_PAR_FrecuenciaNom   107 			// Frecuencia nominal
//#define SMD_PAR_MinFrec         1080 			// Frecuencia minima
//#define SMD_PAR_MaxFrec         1082 			// Frecuencia maxima


//------------------------------------------------------------------------------------
// Estructuras de DATOS
//------------------------------------------------------------------------------------
//
typedef enum {
	SMD_IDX_RealFrec	= 0x00	,
	SMD_IDX_OutCurrent 				,
	SMD_IDX_OutVoltage 			  ,
//	SMD_IDX_ConsEnergia       ,
	SMD_IDX_ParReal           ,
	SMD_IDX_CorrienteNom      ,
	SMD_IDX_Potencia          ,
	SMD_IDX_PotenciaReal      ,
	SMD_IDX_Polos             ,
	SMD_IDX_UltimaFalla       ,
//	SMD_IDX_HoraFalla         ,
	SMD_IDX_ValorFalla        ,
	SMD_IDX_HorasRunDias      ,
	SMD_IDX_HorasRunHoras     ,
	SMD_IDX_FrecuenciaNom     ,
//	SMD_IDX_MinFrec           ,
//	SMD_IDX_MaxFrec           ,
	
	SMD_IDX_MAX_PARAM_QTTY		,
	
	SMD_IDX_PAR_FILL = 0xFF
} SMD_IDX_PARAM_ENUM;


const struct _smd_params_codes
	{
	const UINT16  BasicParam;
	const UINT8   ExtendedParam;
	const UINT8   Idx;
	}SMD_ParamData[] = {
	{SMD_PAR_RealFrec     ,  0x00 ,  0x00	  } ,	  
	{SMD_PAR_OutCurrent		,  0x00	,  0x00	  } ,	  
	{SMD_PAR_OutVoltage   ,  0x00	,  0x00	  } ,	  
//	{SMD_PAR_ConsEnergia	,  0x00 ,  0x00   } ,	  
	{SMD_PAR_ParReal		  ,  0x00 ,  0x00   } ,	  
	{SMD_PAR_CorrienteNom	,  0x00 ,  0x01   } ,	  
	{SMD_PAR_Potencia     ,  0x00 ,  0x01	  } ,	  
	{SMD_PAR_PotenciaReal ,  0x00 ,  0x00	  } ,	  
	{SMD_PAR_Polos        ,  0x00 ,  0x01	  } ,	  
	{SMD_PAR_UltimaFalla  ,  0x00 ,  0x01	  } ,	  
//	{SMD_PAR_HoraFalla    ,  0x00 ,  0x00	  } ,	  
	{SMD_PAR_ValorFalla   ,  0x00 ,  0x01	  } ,	  
	{SMD_PAR_HorasRunDias ,  0x00	,  0x01   } ,   
	{SMD_PAR_HorasRunHoras,  0x00	,  0x02   } ,   
	{SMD_PAR_FrecuenciaNom,  0x00 ,  0x01	  } ,	  
//	{SMD_PAR_MinFrec      ,  0x00 ,  0x00	  } ,
//	{SMD_PAR_MaxFrec      ,  0x00 ,  0x00	  } ,
	{0x00                 ,  0x00	,  0x00   } };  // Parametro nulo



//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
UINT8             SMD_PollingIdx;
UINT16            SMD_CmdPending;
UINT8             SMD_RxERRcntr;

SIEMENS_MSG*	   	pSieMasterDrvInMsg;					// Respuesta al requerimiento enviado 
SIEMENS_MSG*	   	pSieMasterDrvOutMsg;				// Mensaje enviado al variador por Siemens

INV_CONFIG_PARAM* pSMD_ConfigParam;
																					 

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void f_SieMasterDrvIdleTimeout  (void);
void f_SieMasterDrvAnalizeMsg   (void);
void f_SieMasterDrvCmdStart     (void);
void f_SieMasterDrvCmdStop      (void);
void f_SieMasterDrvCmdSetFrec   (void);
void f_SieMasterDrvCmdResFault  (void);

void SMD_Init               (void);
void SMD_BuildBasicRequest  (void);
void SMD_BuildBCC           (void);
void SMD_ReinitParam        (void);


#endif
