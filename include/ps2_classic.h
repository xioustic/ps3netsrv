#ifdef COBRA_ONLY
#define SYSCALL8_OPCODE_ENABLE_PS2NETEMU	0x1ee9	/* Cobra 7.50 */
#define PS2NETEMU_GET_STATUS				2

static int get_cobra_ps2netemu_status(void)
{
	system_call_2(SC_COBRA_SYSCALL8, (uint64_t) SYSCALL8_OPCODE_ENABLE_PS2NETEMU, (uint64_t) PS2NETEMU_GET_STATUS);
	return (int)p1;
}

static void enable_netemu_cobra(int param)
{
	int status = get_cobra_ps2netemu_status();

	if(status < 0 || status == param) return;

	system_call_2(SC_COBRA_SYSCALL8, (uint64_t) SYSCALL8_OPCODE_ENABLE_PS2NETEMU, (uint64_t) param);
}
#endif

#ifndef LITE_EDITION

static void enable_classic_ps2_mode(void)
{
	save_file(PS2_CLASSIC_TOGGLER, NULL, 0);
}

static void disable_classic_ps2_mode(void)
{
	cellFsUnlink(PS2_CLASSIC_TOGGLER);
}

#endif