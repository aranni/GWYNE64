
#ifndef SieMicroMstProcH
#define SieMicroMstProcH


//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
#define TMR_TXPOLL_DELAY        500     // Delay entre pollings al variador
#define TMR_TXREQ_DELAY         10      // Delay entre req. de parametros


#define SIE_PAR_RealFrec        21			  // Frecuencia Real Filtrada
#define SIE_PAR_OutCurrent 	    27			  // Tension de salida
#define SIE_PAR_OutVoltage      25				// Corriente de salida
#define SIE_PAR_ConsEnergia     39				// Consumo de energia
#define SIE_PAR_ParReal         31				// Par real filtrado
#define SIE_PAR_CorrienteNom    305				// Corriente nominal
#define SIE_PAR_Potencia        307				// Potencia nominal
#define SIE_PAR_PotenciaReal    32				// Potencia real
#define SIE_PAR_Polos           313				// Polos
#define SIE_PAR_UltimaFalla     947				// Ultimo codigo de falla
#define SIE_PAR_HoraFalla       948				// Hora del fallo
#define SIE_PAR_ValorFalla      949				// Valor de fallo
#define SIE_PAR_HorasRun        2114			// Contador de horas funcionando
#define SIE_PAR_FrecuenciaNom   310 			// Frecuencia nominal
#define SIE_PAR_MinFrec         1080 			// Frecuencia minima
#define SIE_PAR_MaxFrec         1082 			// Frecuencia maxima


//------------------------------------------------------------------------------------
// Estructuras de DATOS
//------------------------------------------------------------------------------------
//
typedef enum {
	IDX_RealFrec	= 0x00	,
	IDX_OutCurrent 				,
	IDX_OutVoltage 			  ,
	IDX_ConsEnergia       ,
	IDX_ParReal           ,
	IDX_CorrienteNom      ,
	IDX_Potencia          ,
	IDX_PotenciaReal      ,
	IDX_Polos             ,
	IDX_UltimaFalla       ,
	IDX_HoraFalla         ,
	IDX_ValorFalla        ,
	IDX_HorasRun          ,
	IDX_FrecuenciaNom     ,
	IDX_MinFrec           ,
	IDX_MaxFrec           ,
	
	IDX_MAX_PARAM_QTTY		,
	
	IDX_PAR_FILL = 0xFF
} IDX_PARAM_ENUM;


const struct _params_codes
	{
	const UINT16  BasicParam;
	const UINT8   ExtendedParam;
	const UINT8   Idx;
	}ParamData[] = {
	{SIE_PAR_RealFrec     ,  0x00 ,  0x00	  } ,	  
	{SIE_PAR_OutCurrent		,  0x00	,  0x00	  } ,	  
	{SIE_PAR_OutVoltage   ,  0x00	,  0x00	  } ,	  
	{SIE_PAR_ConsEnergia	,  0x00 ,  0x00   } ,	  
	{SIE_PAR_ParReal		  ,  0x00 ,  0x00   } ,	  
	{SIE_PAR_CorrienteNom	,  0x00 ,  0x00   } ,	  
	{SIE_PAR_Potencia     ,  0x00 ,  0x00	  } ,	  
	{SIE_PAR_PotenciaReal ,  0x00 ,  0x00	  } ,	  
	{SIE_PAR_Polos        ,  0x00 ,  0x00	  } ,	  
	{SIE_PAR_UltimaFalla  ,  0x00 ,  0x00	  } ,	  
	{SIE_PAR_HoraFalla    ,  0x00 ,  0x00	  } ,	  
	{SIE_PAR_ValorFalla   ,  0x00 ,  0x00	  } ,	  
	{SIE_PAR_HorasRun     ,  0x01	,  0x00   } ,   
	{SIE_PAR_FrecuenciaNom,  0x00 ,  0x00	  } ,	  
	{SIE_PAR_MinFrec      ,  0x00 ,  0x00	  } ,
	{SIE_PAR_MaxFrec      ,  0x00 ,  0x00	  } ,
	{0x00                 ,  0x00	,  0x00   } };  // Parametro nulo


//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
UINT8             SIE_PollingIdx;
UINT16            SIE_CmdPending;

SIEMENS_MSG*	   	pSiemensInMsg;				// Respuesta al requerimiento enviado 
SIEMENS_MSG*	   	pSiemensOutMsg;				// Mensaje enviado al variador por Siemens

INV_CONFIG_PARAM* pSIE_ConfigParam;
																					 

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void f_SiemensIdleTimeout (void);
void f_SiemensAnalizeMsg (void);
void f_SiemensCmdStart (void);
void f_SiemensCmdStop (void);
void f_SiemensCmdSetFrec (void);
void f_SiemensCmdResFault (void);

void SIE_Init(void);
void SIE_BuildBasicRequest (void);
void SIE_BuildBCC (void);
void SIE_ReinitParam(void);


#endif
