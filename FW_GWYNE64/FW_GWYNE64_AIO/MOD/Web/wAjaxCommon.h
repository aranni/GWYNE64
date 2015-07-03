#ifndef wAjaxCommonH
#define wAjaxCommonH

//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
const unsigned char logs_html[] = "<HEAD><TITLE> Conversor de protocolos -  WEB Server</TITLE></HEAD><BODY><H2> GWY - Conversor de Protocolos.</H2><HR>Consulta web de Logs deshabilitada<HR>></BODY>";

//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
extern struct http_server_state https [NO_OF_HTTP_SESSIONS];

//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void WebApiRead           (UINT8 fat_index, UINT16 wLen, UINT8 bSession);
void WebApiWrite          (UINT8 fat_index, UINT16 wLen, UINT8 bSession);
void WebPrepareMsgAnswer  (char * MsgText, UINT16 MsgCode, BYTE bSession);
void WebReadEEpromVar     (UINT16 getParam, UINT16 getLen, void* getData);
void WebWriteEEpromVar    (UINT16 setParam, UINT16 setLen, void* setData);

//------------------------------------------------------------------------------------
//  LISTA DE VARIABLES HABILITADAS PARA CONSUTAS XML
//------------------------------------------------------------------------------------
//
static const USR_DYNAMIC_FIELDS StringFields[] = 					 
{
	{"TagPolFrec",          &TagPolFrec,                        WORD_TYPE,            NULL,   NULL,   0,    0   },
	{"SciATxErr",           &StatisticData.SerialStat.MsgTxErr, DWORD_TYPE,           NULL,   NULL,   0,    0   },
	{"SciATxOk",            &StatisticData.SerialStat.MsgTxOk,  DWORD_TYPE,           NULL,   NULL,   0,    0   },
	{"SciARxErr",           &StatisticData.SerialStat.MsgRxErr, DWORD_TYPE,           NULL,   NULL,   0,    0   },
	{"SciARxOk",            &StatisticData.SerialStat.MsgRxOk,  DWORD_TYPE,           NULL,   NULL,   0,    0   },
	{"SysFwVer",            &FwVersionData.FW_Version,          STRING_TYPE,          NULL,   NULL,   0,    0   },
	{"SysFwDate",           &FwVersionData.DateOfCompilation,   STRING_TYPE,          NULL,   NULL,   0,    0   },
	{"SysFwTime",           &FwVersionData.TimeOfCompilation,   STRING_TYPE,          NULL,   NULL,   0,    0   },
	{"SysHwVer",            NULL,                               STRING_TYPE,          WebReadEEpromVar,   WebWriteEEpromVar, EE_ADDR_VER_HWVER,   SIZE_HW_VER   },
	{"SysHwSerial",         NULL,                               STRING_TYPE,          WebReadEEpromVar,   WebWriteEEpromVar, EE_ADDR_VER_SERIAL,  SIZE_SERIAL   },
	{"SysInfoText",         NULL,                               SET_WR(STRING_TYPE),  WebReadEEpromVar,   WebWriteEEpromVar, EE_ADDR_VER_TEXT,    30   },

	{"GwyPolFrec",          &TagPolFrec,                        SET_WR(WORD_TYPE),    NULL,   NULL,   0,    0   },
	{"GwySaveBanks",        &TagFlags,                          SET_WR(BIT_0_TYPE),   NULL,   NULL,   0,    0   },

#ifdef MOD_MODBUSRTU_AUTOQUERY_ENABLED  	

	{"MAQ_MODBUS_OFF_1",   &(MAQ_BankMemManager[0].ModbusQueryAddress),            WORD_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },
 	{"MAQ_MODBUS_FC_1",    &(MAQ_BankMemManager[0].ModbusQueryOpCode),             BYTE_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },	
 	{"MAQ_MODBUS_LEN_1",   &(MAQ_BankMemManager[0].ModbusQueryLen),                BYTE_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },	
	{"MAQ_BANK_STAT_1",    &(MAQ_BankMemManager[0].BankStatus),                    BYTE_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },
	{"MAQ_OFFSET_INI_1",   &(MAQ_BankMemManager[0].BankOffsetStart),               WORD_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },
		
	{"MAQ_MODBUS_OFF_2",   &(MAQ_BankMemManager[1].ModbusQueryAddress),            WORD_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },
 	{"MAQ_MODBUS_FC_2",    &(MAQ_BankMemManager[1].ModbusQueryOpCode),             BYTE_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },	
 	{"MAQ_MODBUS_LEN_2",   &(MAQ_BankMemManager[1].ModbusQueryLen),                BYTE_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },	
	{"MAQ_BANK_STAT_2",    &(MAQ_BankMemManager[1].BankStatus),                    BYTE_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },	
	{"MAQ_OFFSET_INI_2",   &(MAQ_BankMemManager[1].BankOffsetStart),               WORD_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },
	
	{"MAQ_MODBUS_OFF_3",   &(MAQ_BankMemManager[2].ModbusQueryAddress),            WORD_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },
 	{"MAQ_MODBUS_FC_3",    &(MAQ_BankMemManager[2].ModbusQueryOpCode),             BYTE_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },	
 	{"MAQ_MODBUS_LEN_3",   &(MAQ_BankMemManager[2].ModbusQueryLen),                BYTE_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },	
	{"MAQ_BANK_STAT_3",    &(MAQ_BankMemManager[2].BankStatus),                    BYTE_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },	
	{"MAQ_OFFSET_INI_3",   &(MAQ_BankMemManager[2].BankOffsetStart),               WORD_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },
	
	{"MAQ_MODBUS_OFF_4",   &(MAQ_BankMemManager[3].ModbusQueryAddress),            WORD_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },
 	{"MAQ_MODBUS_FC_4",    &(MAQ_BankMemManager[3].ModbusQueryOpCode),             BYTE_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },	
 	{"MAQ_MODBUS_LEN_4",   &(MAQ_BankMemManager[3].ModbusQueryLen),                BYTE_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },	
	{"MAQ_BANK_STAT_4",    &(MAQ_BankMemManager[3].BankStatus),                    BYTE_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },	
	{"MAQ_OFFSET_INI_4",   &(MAQ_BankMemManager[3].BankOffsetStart),               WORD_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },
	
	{"MAQ_MODBUS_OFF_5",   &(MAQ_BankMemManager[4].ModbusQueryAddress),            WORD_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },
 	{"MAQ_MODBUS_FC_5",    &(MAQ_BankMemManager[4].ModbusQueryOpCode),             BYTE_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },	
 	{"MAQ_MODBUS_LEN_5",   &(MAQ_BankMemManager[4].ModbusQueryLen),                BYTE_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },	
	{"MAQ_BANK_STAT_5",    &(MAQ_BankMemManager[4].BankStatus),                    BYTE_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },	
	{"MAQ_OFFSET_INI_5",   &(MAQ_BankMemManager[4].BankOffsetStart),               WORD_TYPE|WRITE_ACCESS_MASK,       NULL,   NULL,   0,    0    },

#endif

 	//{"MAQ_EEPROM_BANK",    NULL,                               STRING_TYPE,        WebReadEEpromVar,   WebWriteEEpromVar, EE_ADDR_MAQ_BANKS,  (sizeof(AUTOQUERY_BANK_ALLOC)*MAX_AUTOQUERY_BANKS)   },

	{"", NULL, WORD_TYPE, NULL,  NULL, 0, 0}								
};

#endif    // wAjaxCommonH