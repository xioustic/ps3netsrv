#ifdef COBRA_ONLY
#define SYSCALL8_OPCODE_ENABLE_PS2NETEMU	0x1ee9	/* Cobra 7.50 */

static void enable_netemu_cobra()
{
	system_call_1(SC_COBRA_SYSCALL8, (uint64_t) SYSCALL8_OPCODE_ENABLE_PS2NETEMU);
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