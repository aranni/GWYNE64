
static const USR_DYNAMIC_FIELDS LogsReqFields[] = 					 
{
	{"LogReadNext",              &TagFlags,                   BYTE_TYPE,        NULL,   NULL,     0,    0   },
	{"LogParam",                 &LogParam,                   WORD_TYPE,        NULL,   NULL,     0,    0   },
	{"", NULL, WORD_TYPE, NULL, NULL, 0, 0}								
};

void LogsAjax (UINT8 fat_index, UINT16 wLen, UINT8 bSession);

