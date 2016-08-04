//#define SC_PEEK_LV2					(6)
//#define SC_POKE_LV2					(7)
#define SC_PEEK_LV1 					(8)
#define SC_POKE_LV1 					(9)

#define SYSCALL8_OPCODE_PS3MAPI			0x7777
#define PS3MAPI_OPCODE_LV1_POKE			0x1009

static uint64_t peek_lv1(uint64_t addr)
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

void add_log2(const char *fmt, uint64_t addr, uint64_t value);

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
	if(c_firmware>=4.55f /*&& c_firmware<=4.78f*/)
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

	if(c_firmware>=4.30f /*&& c_firmware<=4.78f*/)
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

#if defined(PS3MAPI) || defined(DEBUG_MEM) || defined(SPOOF_CONSOLEID)
static uint64_t convertH(char *val)
{
	uint64_t ret = 0;

	for(uint8_t buff, i = 0, n=0; i < 16+n; i++)
	{
		if(val[i]==' ') {n++; continue;}

		if(val[i]>='0' && val[i]<='9') buff = (     val[i]-'0'); else
		if(val[i]>='A' && val[i]<='F') buff = (10 + val[i]-'A'); else
		if(val[i]>='a' && val[i]<='f') buff = (10 + val[i]-'a'); else
		return ret;

		ret = (ret << 4) | buff;
	}

	return ret;
}
#endif

#ifdef GET_KLICENSEE
static char *hex_dump(char *buffer, int offset, int size)
{
	for (int k = 0; k < size ; k++)
	{
		sprintf(&buffer[2 * k],"%02X", (unsigned int)(((unsigned char*)offset)[k]));
	}
	return buffer;
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
