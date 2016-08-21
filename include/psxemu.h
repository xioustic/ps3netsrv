#ifdef COBRA_ONLY
static void select_ps1emu(void)
{
	CellPadData pad_data = pad_read();

	if(pad_data.len > 0)
	{
		if(pad_data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_R2) {webman_config->ps1emu = 1; save_settings();} else
		if(pad_data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_L2) {webman_config->ps1emu = 0; save_settings();}
	}

	char msg[100];

	if(webman_config->ps1emu)
	{
		sys_map_path("/dev_flash/ps1emu/ps1_netemu.self", "//dev_flash/ps1emu/ps1_emu.self");
		sys_map_path("/dev_flash/ps1emu/ps1_emu.self"   , "//dev_flash/ps1emu/ps1_netemu.self");

		sprintf(msg, "ps1_netemu.self %s", STR_ENABLED);
	}
	else
	{
		sys_map_path("/dev_flash/ps1emu/ps1_netemu.self", "//dev_flash/ps1emu/ps1_netemu.self");
		sys_map_path("/dev_flash/ps1emu/ps1_emu.self"   , "//dev_flash/ps1emu/ps1_emu.self");

		sprintf(msg, "ps1_emu.self %s",    STR_ENABLED);
	}

	show_msg(msg);
}
#endif
