//#define SC_PEEK_LV2					(6)
//#define SC_POKE_LV2					(7)
#define SC_PEEK_LV1 					(8)
#define SC_POKE_LV1 					(9)

#define SYSCALL8_OPCODE_PS3MAPI			0x7777
#define PS3MAPI_OPCODE_LV1_POKE			0x1009

static inline uint64_t peek_lv1(uint64_t addr)
{
	system_call_1(SC_PEEK_LV1, (uint64_t) addr);
	return (uint64_t) p1;
}

static void poke_lv1( uint64_t addr, uint64_t value)
{
	if(!syscalls_removed)
		{system_call_2(SC_POKE_LV1, addr, value);}
#ifdef COBRA_ONLY
	else
		{system_call_4(SC_COBRA_SYSCALL8, SYSCALL8_OPCODE_PS3MAPI, PS3MAPI_OPCODE_LV1_POKE, addr, value);} // use ps3mapi extension to support poke lv1
#endif
}

static inline uint64_t peekq(uint64_t addr) //lv2
{
	system_call_1(SC_PEEK_LV1, addr + LV2_OFFSET_ON_LV1); //old: {system_call_1(SC_PEEK_LV2, addr);}
	return (uint64_t) p1;
}

static void pokeq(uint64_t addr, uint64_t value) //lv2
{
	if(!syscalls_removed)
		{system_call_2(SC_POKE_LV1, addr + LV2_OFFSET_ON_LV1, value);} // {system_call_2(SC_POKE_LV2, addr, value);}
#ifdef COBRA_ONLY
	else
		{system_call_4(SC_COBRA_SYSCALL8, SYSCALL8_OPCODE_PS3MAPI, PS3MAPI_OPCODE_LV1_POKE, addr + LV2_OFFSET_ON_LV1, value);} // use ps3mapi extension to support poke lv2
#endif
}

#ifndef COBRA_ONLY
static void remove_lv2_memory_protection(void)
{
	uint64_t HV_START_OFFSET = 0;

	//Remove Lv2 memory protection
	if(c_firmware==3.55f)
	{
		HV_START_OFFSET = HV_START_OFFSET_355;
	}
	else
	if(c_firmware==4.21f)
	{
		HV_START_OFFSET = HV_START_OFFSET_421;
	}
	else
	if(c_firmware>=4.30f && c_firmware<=4.53f)
	{
		HV_START_OFFSET = HV_START_OFFSET_430;
	}
	else
	if(c_firmware>=4.55f /*&& c_firmware<=4.80f*/)
	{
		HV_START_OFFSET = HV_START_OFFSET_460;
	}

	if(!HV_START_OFFSET) return;

	poke_lv1(HV_START_OFFSET + 0x00, 0x0000000000000001ULL);
	poke_lv1(HV_START_OFFSET + 0x08, 0xe0d251b556c59f05ULL);
	poke_lv1(HV_START_OFFSET + 0x10, 0xc232fcad552c80d7ULL);
	poke_lv1(HV_START_OFFSET + 0x18, 0x65140cd200000000ULL);
}

static void install_peek_poke(void)
{
	remove_lv2_memory_protection();

	if(c_firmware>=4.30f /*&& c_firmware<=4.80f*/)
	{	// add lv2 peek/poke + lv1 peek/poke
		pokeq(0x800000000000171CULL + 0x00, 0x7C0802A6F8010010ULL);
		pokeq(0x800000000000171CULL + 0x08, 0x396000B644000022ULL);
		pokeq(0x800000000000171CULL + 0x10, 0x7C832378E8010010ULL);
		pokeq(0x800000000000171CULL + 0x18, 0x7C0803A64E800020ULL);
		pokeq(0x800000000000171CULL + 0x20, 0x7C0802A6F8010010ULL);
		pokeq(0x800000000000171CULL + 0x28, 0x396000B744000022ULL);
		pokeq(0x800000000000171CULL + 0x30, 0x38600000E8010010ULL);
		pokeq(0x800000000000171CULL + 0x38, 0x7C0803A64E800020ULL);
		pokeq(0x800000000000171CULL + 0x40, 0x7C0802A6F8010010ULL);
		pokeq(0x800000000000171CULL + 0x48, 0x7D4B537844000022ULL);
		pokeq(0x800000000000171CULL + 0x50, 0xE80100107C0803A6ULL);
		pokeq(0x800000000000171CULL + 0x58, 0x4E80002080000000ULL);
		pokeq(0x800000000000171CULL + 0x60, 0x0000170C80000000ULL);
		pokeq(0x800000000000171CULL + 0x68, 0x0000171480000000ULL);
		pokeq(0x800000000000171CULL + 0x70, 0x0000171C80000000ULL);
		pokeq(0x800000000000171CULL + 0x78, 0x0000173C80000000ULL);
		pokeq(0x800000000000171CULL + 0x80, 0x0000175C00000000ULL);

		// enable syscalls 6, 7, 8, 9, 10
		for(u8 sc = 6; sc < 11; sc++)
			pokeq(SYSCALL_PTR(sc), 0x8000000000001748ULL + sc * 8ULL); // 0x8000000000001778 (sc6) to 0x8000000000001798 (sc10)
	}
}
#endif

static void lv2poke32(u64 addr, u32 value)
{
	pokeq(addr, (((u64) value) <<32) | (peekq(addr) & 0xffffffffULL));
}

/*
static u32 lv2peek32(u64 addr)
{
	u32 ret = (u32) (peekq(addr) >> 32ULL);
	return ret;
}
*/

#ifndef COBRA_ONLY

typedef struct
{
	char src[384];
	char dst[384];
} redir_files_struct;

static redir_files_struct file_to_map[10];

static void add_to_map(const char *path1, const char *path2)
{
	if(max_mapped == 0) pokeq(MAP_BASE + 0x00, 0x0000000000000000ULL);

	if(max_mapped < 10)
	{
		for(u8 n = 0; n < max_mapped; n++)
		{
			if(IS(file_to_map[n].src, path1)) return;
		}

		sprintf(file_to_map[max_mapped].src, "%s", path1);
		sprintf(file_to_map[max_mapped].dst, "%s", path2);
		max_mapped++;
	}
}

static u16 string_to_lv2(char* path, uint64_t addr)
{
	u16 len  = (strlen(path) + 8) & 0x7f8;
	len = RANGE(len, 8, 384);
	u16 len2 = strlen(path); if(len2 > len) len2 = len;

	u8 data2[384];
	u8* data = data2;
	memset(data, 0, 384);
	memcpy(data, path, len2);

	uint64_t val = 0x0000000000000000ULL;
	for(uint64_t n = 0; n < len; n += 8)
	{
		memcpy(&val, &data[n], 8);
		pokeq(addr + n, val);
	}
	return len2;
}
#endif

#if defined(PS3MAPI) || defined(DEBUG_MEM) || defined(SPOOF_CONSOLEID)

static uint64_t convertH(char *val)
{
	uint64_t ret = 0; char c;

	for(uint8_t buff, i = 0, n = 0; i < 16 + n; i++)
	{
		if(val[i]==' ') {n++; continue;}

		c = (val[i] | 0x20);
		if(c >= '0' && c <= '9') buff = (c - '0');      else
		if(c >= 'a' && c <= 'f') buff = (c - 'a' + 10); else
		return ret;

		ret = (ret << 4) | buff;
	}

	return ret;
}

#endif
#ifdef PS3MAPI

static void Hex2Bin(const char* src, char* target)
{
	char value[3]; value[2] = NULL;
	while(*src && src[1])
	{
		value[0] = src[0], value[1] = src[1];
		*(target++) = (u8)convertH(value);
		src += 2;
	}
}

#endif

/*
s32 lv2_get_platform_info(struct platform_info *info)
{
	system_call_1(SC_GET_PLATFORM_INFO, (uint64_t) info);
	return_to_user_prog(s32);
}

s32 lv2_get_target_type(uint64_t *type)
{
	lv2syscall1(985, (uint64_t) type);
	return_to_user_prog(s32);
}

uint64_t find_syscall_table()
{
	uint64_t targettype;
	lv2_get_target_type(&targettype);

	for(uint64_t i = 0x8000000000340000ULL; i<0x8000000000400000ULL; i+=4)
	{
		if(peekq(i) == 0x3235352E3235352EULL) return (i + (targettype == 2) ? 0x1228 : 0x1220);
	}
	return 0;
}
*/
