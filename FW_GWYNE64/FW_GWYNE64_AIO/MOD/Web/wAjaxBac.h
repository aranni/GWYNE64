
static const USR_DYNAMIC_INITIALIZE SPA_ConfigData[] = 					 
{
	{"TagPolFrec",               &SPA_PollingPeriod,        WORD_TYPE},
	{"TagPolEnable",             &SPA_CtrlFlags,            BIT_0_TYPE},
	{"TagResetList",             &SPA_CtrlFlags,            BIT_1_TYPE},
	{"TagMirrorFrame",           &SPA_CtrlFlags,            BIT_2_TYPE},
	{"", NULL, WORD_TYPE}								
};

		
							 
static const USR_DYNAMIC_INITIALIZE BacNode_01[] = 					 
{
	{"SPA_NODE_ADD",      &(SPA_BankMemManager[0].BacNodeAddress),    XWORD_TYPE },	
	{"SPA_OFFSET_INI",    &(SPA_BankMemManager[0].BankOffsetStart),   WORD_TYPE },	
	{"BAC_MODBUS_ADD",    &(SPA_BankMemManager[0].BankModbusAddress), WORD_TYPE },	
	{"SPA_BANK_SIZE",     &(SPA_BankMemManager[0].BankSize),          BYTE_TYPE },	
	{"SPA_BANK_STAT",     &(SPA_BankMemManager[0].BankStatus),        BYTE_TYPE },	
	{"", NULL, WORD_TYPE}								
};

