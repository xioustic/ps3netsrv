static char FW[10];
static char payload_type[64];
static char kernel_type[64];
static char netstr[64];
static char cfw_str[64];

extern int32_t netctl_main_9A528B81(int32_t size, const char *ip);  // get ip addr of interface "eth0"

struct platform_info {
	uint32_t firmware_version;
} info;

////////////////////////////////////////////////////////////////////////
//                      GET FIRMWARE VERSION                          //
////////////////////////////////////////////////////////////////////////

#define SYSCALL8_OPCODE_GET_MAMBA				0x7FFFULL
#define SYSCALL8_OPCODE_GET_VERSION				0x7000
#define SYSCALL8_OPCODE_GET_VERSION2			0x7001

static int sys_get_version(uint32_t *version)
{
	system_call_2(8, SYSCALL8_OPCODE_GET_VERSION, (uint64_t)(uint32_t)version);
	return (int)p1;
}

static int sys_get_version2(uint16_t *version)
{
	system_call_2(8, SYSCALL8_OPCODE_GET_VERSION2, (uint64_t)(uint32_t)version);
	return (int)p1;
}

static int is_cobra_based(void)
{
	uint32_t version = 0x99999999;

	if (sys_get_version(&version) < 0)
		return 0;

	if (version != 0x99999999) // If value changed, it is cobra
		return 1;

	return 0;
}

static int lv2_get_platform_info(struct platform_info *info)
{
	system_call_1(387, (uint32_t) info);
	return (int32_t)p1;
}

static void get_kernel_type(void)
{
	uint64_t type;
	memset(kernel_type, 0, 64);
	system_call_1(985, (uint32_t)&type);
	if(type == 1) sprintf(kernel_type, "CEX"); else // Retail
	if(type == 2) sprintf(kernel_type, "DEX"); else // Debug
	if(type == 3) sprintf(kernel_type, "Debugger"); // Debugger
}

static void get_payload_type(void)
{
	if(!is_cobra_based()) return;

	bool is_mamba; {system_call_1(8, SYSCALL8_OPCODE_GET_MAMBA); is_mamba = ((int)p1 ==0x666);}

	uint16_t cobra_version; sys_get_version2(&cobra_version);
	sprintf(payload_type, "%s %X.%X", is_mamba ? "Mamba" : "Cobra", cobra_version>>8, (cobra_version & 0xF) ? (cobra_version & 0xFF) : ((cobra_version>>4) & 0xF));

	sprintf(cfw_str, "Firmware : %c.%c%c %s %s", FW[0], FW[2], FW[3], kernel_type, payload_type);
}

static void get_firmware_version(void)
{
	get_kernel_type();
	lv2_get_platform_info(&info);
	sprintf(FW, "%02X", info.firmware_version);
	get_payload_type();
}

static void get_network_info(void)
{
	char netdevice[32];
	char ipaddr[32];

	net_info info1;
	memset(&info1, 0, sizeof(net_info));
	xsetting_F48C0548()->sub_44A47C(&info1);

	if (info1.device == 0)
	{
		strcpy(netdevice, "LAN");
	}
	else if (info1.device == 1)
	{
		strcpy(netdevice, "WLAN");
	}
	else
		strcpy(netdevice, "[N/A]");

	int32_t size = 0x10;
	char ip[size];
	netctl_main_9A528B81(size, ip);

	if (ip[0] == '\0')
		strcpy(ipaddr, "[N/A]");
	else
		sprintf(ipaddr, "%s", ip);

	sprintf(netstr, "Network connection :  %s\r\nIP address :  %s", netdevice, ipaddr);
}

////////////////////////////////////////////////////////////////////////
//                      GET CPU & RSX TEMPERATURES                  //
////////////////////////////////////////////////////////////////////////
static void get_temperature(uint32_t _dev, uint32_t *_temp)
{
	system_call_2(383, (uint64_t)(uint32_t) _dev, (uint64_t)(uint32_t) _temp);
}
