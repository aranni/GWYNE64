

						 
/**********************************************************************
*                           FILES TO HANDLE
**********************************************************************/

static const USR_DYNAMIC_INITIALIZE Socket_01[] = 					 
{
	{"SKO1_STAT",            &(TcpSockSrv[0].status),                 BIT_0_TYPE },	
	{"SKO1_IP_A",            &(TcpSockSrv[0].SlaveIpAddress),         DWORD_HH_TYPE },	
	{"SKO1_IP_B",            &(TcpSockSrv[0].SlaveIpAddress),         DWORD_HL_TYPE },	
	{"SKO1_IP_C",            &(TcpSockSrv[0].SlaveIpAddress),         DWORD_LH_TYPE },	
	{"SKO1_IP_D",            &(TcpSockSrv[0].SlaveIpAddress),         DWORD_LL_TYPE },	
	{"SKO1_PORT",            &(TcpSockSrv[0].SlaveTcpPort),           WORD_TYPE },	
	{"SKO1_MSG_RX",          &(TcpSockSrv[0].MsgRxOk),                DWORD_TYPE },	
//	{"SKO1_NODE",            &(TcpSockSrv[0].SlaveNodeAddress),       WORD_TYPE },	
	{"", NULL, WORD_TYPE}								
};

static const USR_DYNAMIC_INITIALIZE Socket_02[] = 					 
{
	{"SKO1_STAT",            &(TcpSockSrv[1].status),                 BIT_0_TYPE },	
	{"SKO1_IP_A",            &(TcpSockSrv[1].SlaveIpAddress),         DWORD_HH_TYPE },	
	{"SKO1_IP_B",            &(TcpSockSrv[1].SlaveIpAddress),         DWORD_HL_TYPE },	
	{"SKO1_IP_C",            &(TcpSockSrv[1].SlaveIpAddress),         DWORD_LH_TYPE },	
	{"SKO1_IP_D",            &(TcpSockSrv[1].SlaveIpAddress),         DWORD_LL_TYPE },	
	{"SKO1_PORT",            &(TcpSockSrv[1].SlaveTcpPort),           WORD_TYPE },	
	{"SKO1_MSG_RX",          &(TcpSockSrv[1].MsgRxOk),                DWORD_TYPE },	
//	{"SKO1_NODE",            &(TcpSockSrv[1].SlaveNodeAddress),       WORD_TYPE },	
	{"", NULL, WORD_TYPE}								
};

#ifdef MOD_MODBUSRTU_AUTOQUERY_ENABLED  	

static const USR_DYNAMIC_INITIALIZE MaqNode_01[] = 					 
{
	{"MAQ_MODBUS_OFF",    &(MAQ_BankMemManager[0].ModbusQueryAddress),  WORD_TYPE },	
	{"MAQ_MODBUS_FC",     &(MAQ_BankMemManager[0].ModbusQueryOpCode),   BYTE_TYPE },	
	{"MAQ_MODBUS_LEN",    &(MAQ_BankMemManager[0].ModbusQueryLen),      BYTE_TYPE },	
	{"MAQ_OFFSET_INI",    &(MAQ_BankMemManager[0].BankOffsetStart),     WORD_TYPE },	
	{"MAQ_MODBUS_ADD",    &(MAQ_BankMemManager[0].BankModbusAddress),   WORD_TYPE },	
	{"MAQ_BANK_STAT",     &(MAQ_BankMemManager[0].BankStatus),          BYTE_TYPE },	
	{"", NULL, WORD_TYPE}								
};


static const USR_DYNAMIC_INITIALIZE MaqNode_02[] = 					 
{
	{"MAQ_MODBUS_OFF",    &(MAQ_BankMemManager[1].ModbusQueryAddress),  WORD_TYPE },	
	{"MAQ_MODBUS_FC",     &(MAQ_BankMemManager[1].ModbusQueryOpCode),   BYTE_TYPE },	
	{"MAQ_MODBUS_LEN",    &(MAQ_BankMemManager[1].ModbusQueryLen),      BYTE_TYPE },	
	{"MAQ_OFFSET_INI",    &(MAQ_BankMemManager[1].BankOffsetStart),     WORD_TYPE },	
	{"MAQ_MODBUS_ADD",    &(MAQ_BankMemManager[1].BankModbusAddress),   WORD_TYPE },	
	{"MAQ_BANK_STAT",     &(MAQ_BankMemManager[1].BankStatus),          BYTE_TYPE },	
	{"", NULL, WORD_TYPE}								
};

static const USR_DYNAMIC_INITIALIZE MaqNode_03[] = 					 
{
	{"MAQ_MODBUS_OFF",    &(MAQ_BankMemManager[2].ModbusQueryAddress),  WORD_TYPE },	
	{"MAQ_MODBUS_FC",     &(MAQ_BankMemManager[2].ModbusQueryOpCode),   BYTE_TYPE },	
	{"MAQ_MODBUS_LEN",    &(MAQ_BankMemManager[2].ModbusQueryLen),      BYTE_TYPE },	
	{"MAQ_OFFSET_INI",    &(MAQ_BankMemManager[2].BankOffsetStart),     WORD_TYPE },	
	{"MAQ_MODBUS_ADD",    &(MAQ_BankMemManager[2].BankModbusAddress),   WORD_TYPE },	
	{"MAQ_BANK_STAT",     &(MAQ_BankMemManager[2].BankStatus),          BYTE_TYPE },	
	{"", NULL, WORD_TYPE}								
};

static const USR_DYNAMIC_INITIALIZE MaqNode_04[] = 					 
{
	{"MAQ_MODBUS_OFF",    &(MAQ_BankMemManager[3].ModbusQueryAddress),  WORD_TYPE },	
	{"MAQ_MODBUS_FC",     &(MAQ_BankMemManager[3].ModbusQueryOpCode),   BYTE_TYPE },	
	{"MAQ_MODBUS_LEN",    &(MAQ_BankMemManager[3].ModbusQueryLen),      BYTE_TYPE },	
	{"MAQ_OFFSET_INI",    &(MAQ_BankMemManager[3].BankOffsetStart),     WORD_TYPE },	
	{"MAQ_MODBUS_ADD",    &(MAQ_BankMemManager[3].BankModbusAddress),   WORD_TYPE },	
	{"MAQ_BANK_STAT",     &(MAQ_BankMemManager[3].BankStatus),          BYTE_TYPE },	
	{"", NULL, WORD_TYPE}								
};

static const USR_DYNAMIC_INITIALIZE MaqNode_05[] = 					 
{
	{"MAQ_MODBUS_OFF",    &(MAQ_BankMemManager[4].ModbusQueryAddress),  WORD_TYPE },	
	{"MAQ_MODBUS_FC",     &(MAQ_BankMemManager[4].ModbusQueryOpCode),   BYTE_TYPE },	
	{"MAQ_MODBUS_LEN",    &(MAQ_BankMemManager[4].ModbusQueryLen),      BYTE_TYPE },	
	{"MAQ_OFFSET_INI",    &(MAQ_BankMemManager[4].BankOffsetStart),     WORD_TYPE },	
	{"MAQ_MODBUS_ADD",    &(MAQ_BankMemManager[4].BankModbusAddress),   WORD_TYPE },	
	{"MAQ_BANK_STAT",     &(MAQ_BankMemManager[4].BankStatus),          BYTE_TYPE },	
	{"", NULL, WORD_TYPE}								
};

#endif

void MobusAjax (UINT8 fat_index, UINT16 wLen, UINT8 bSession);
