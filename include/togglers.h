#ifndef LITE_EDITION

#if defined(COBRA_ONLY) || defined(REX_ONLY)
static void swap_file(const char *path, const char *curfile, const char *rento, const char *newfile)
{
	char file1[64], file2[64], file3[64];

	sprintf(file3, "%s%s", path, newfile);

	if(file_exists(file3))
	{
		sprintf(file1, "%s%s", path, curfile);
		sprintf(file2, "%s%s", path, rento);

		cellFsRename(file1, file2);
		cellFsRename(file3, file1);
	}
}
#endif

#endif

#ifdef REX_ONLY
static bool toggle_rebug_mode(void)
{
	enable_dev_blind("REBUG Mode Switcher activated!");

	if(file_exists(VSH_MODULE_PATH "vsh.self.swp"))
	{
		show_msg((char*)"Normal Mode detected!\n"
						"Switch to REBUG Mode Debug XMB...");
		sys_ppu_thread_sleep(3);

		swap_file(VSH_ETC_PATH, "index.dat", "index.dat.nrm", "index.dat.swp");
		swap_file(VSH_ETC_PATH, "version.txt", "version.txt.nrm", "version.txt.swp");
		swap_file(VSH_MODULE_PATH, "vsh.self", "vsh.self.nrm", "vsh.self.swp");

		return true; // vsh reboot
	}
	else
	if((file_exists(VSH_MODULE_PATH "vsh.self.nrm"))
	&& (file_exists(VSH_MODULE_PATH "vsh.self.cexsp")))
	{
		show_msg((char*)"REBUG Mode Debug XMB detected!\n"
						"Switch to Retail XMB...");
		sys_ppu_thread_sleep(3);

		swap_file(VSH_MODULE_PATH, "vsh.self", "vsh.self.dexsp", "vsh.self.cexsp");

		return true; // vsh reboot
	}
	else
	if(file_exists(VSH_MODULE_PATH "vsh.self.dexsp"))
	{
		show_msg((char*)"REBUG Mode Retail XMB detected!\n"
						"Switch to Debug XMB...");
		sys_ppu_thread_sleep(3);

		swap_file(VSH_MODULE_PATH, "vsh.self", "vsh.self.cexsp", "vsh.self.dexsp");

		return true; // vsh reboot
	}
	return false;
}

static bool toggle_normal_mode(void)
{
	enable_dev_blind("Normal Mode Switcher activated!");

	if((file_exists(VSH_MODULE_PATH "vsh.self.nrm"))
	&& (file_exists(VSH_MODULE_PATH "vsh.self.cexsp")))
	{
		show_msg((char*)"REBUG Mode Debug XMB detected!\n"
						"Switch to Normal Mode...");

		swap_file(VSH_ETC_PATH, "index.dat", "index.dat.swp", "index.dat.nrm");
		swap_file(VSH_ETC_PATH, "version.txt", "version.txt.swp", "version.txt.nrm");
		swap_file(VSH_MODULE_PATH, "vsh.self", "vsh.self.swp", "vsh.self.nrm");

		return true; // vsh reboot
	}
	else
	if(file_exists(VSH_MODULE_PATH "vsh.self.dexsp"))
	{
		show_msg((char*)"REBUG Mode Retail XMB detected!\n"
						"Switch to Normal Mode...");

		swap_file(VSH_ETC_PATH, "index.dat", "index.dat.swp", "index.dat.nrm");
		swap_file(VSH_ETC_PATH, "version.txt", "version.txt.swp", "version.txt.nrm");
		swap_file(VSH_MODULE_PATH, "vsh.self", "vsh.self.cexsp", "vsh.self.nrm");

		cellFsRename(VSH_MODULE_PATH "vsh.self.dexsp", VSH_MODULE_PATH "vsh.self.swp");

		return true; // vsh reboot
	}
	else
	if(file_exists(VSH_MODULE_PATH "vsh.self.swp"))
	{
		show_msg((char*)"Normal Mode detected!\n"
						"No need to switch!");
		sys_ppu_thread_sleep(3);
		disable_dev_blind();
	}
	return false;
}

static void toggle_debug_menu(void)
{
	enable_dev_blind("Debug Menu Switcher activated!");

	if(file_exists(VSH_MODULE_PATH "sysconf_plugin.sprx.dex"))
	{
		show_msg((char*)"CEX QA Menu is active!\n"
						"Switch to DEX Debug Menu...");

		swap_file(VSH_MODULE_PATH, "sysconf_plugin.sprx", "sysconf_plugin.sprx.cex", "sysconf_plugin.sprx.dex");
	}
	else
	if(file_exists(VSH_MODULE_PATH "sysconf_plugin.sprx.cex"))
	{
		show_msg((char*)"DEX Debug Menu is active!\n"
						"Switch to CEX QA Menu...");

		swap_file(VSH_MODULE_PATH, "sysconf_plugin.sprx", "sysconf_plugin.sprx.dex", "sysconf_plugin.sprx.cex");
	}
	sys_ppu_thread_sleep(1);
	disable_dev_blind();
}
#endif //#ifdef REX_ONLY

#ifdef COBRA_ONLY
 #ifndef LITE_EDITION

static bool toggle_cobra(void)
{
	enable_dev_blind("COBRA Toggle activated!");
 #ifdef REX_ONLY
	if( (file_exists(REBUG_COBRA_PATH "stage2.cex")) /* &&
		(file_exists(REBUG_COBRA_PATH "stage2.dex")) */)
	{
		show_msg((char*)"REBUG COBRA is active!\n"
						"Deactivating COBRA...");

		cellFsRename(REBUG_COBRA_PATH "stage2.cex", REBUG_COBRA_PATH "stage2.cex.bak");
		cellFsRename(REBUG_COBRA_PATH "stage2.dex", REBUG_COBRA_PATH "stage2.dex.bak");
		return true; // vsh reboot
	}
	else if((file_exists(REBUG_COBRA_PATH "stage2.cex.bak")) /* &&
			(file_exists(REBUG_COBRA_PATH "stage2.dex.bak")) */)
	{
		show_msg((char*)"REBUG COBRA is inactive!\n"
						"Activating COBRA...");

		cellFsRename(REBUG_COBRA_PATH "stage2.cex.bak", REBUG_COBRA_PATH "stage2.cex");
		cellFsRename(REBUG_COBRA_PATH "stage2.dex.bak", REBUG_COBRA_PATH "stage2.dex");
		return true; // vsh reboot
	}
 #endif //#ifdef REX_ONLY
	if(file_exists(HABIB_COBRA_PATH "stage2.cex"))
	{
		show_msg((char*)"COBRA is active!\n"
						"Deactivating COBRA...");

		cellFsRename(HABIB_COBRA_PATH "stage2.cex", HABIB_COBRA_PATH "stage2_disabled.cex");

		return true; // vsh reboot
	}
	else if(file_exists(HABIB_COBRA_PATH "stage2_disabled.cex"))
	{
		show_msg((char*)"COBRA is inactive!\n"
						"Activating COBRA...");

		cellFsRename(HABIB_COBRA_PATH "stage2_disabled.cex", HABIB_COBRA_PATH "stage2.cex");

		return true; // vsh reboot
	}

	if(file_exists(SYS_COBRA_PATH "stage2.bin"))
	{
		show_msg((char*)"COBRA is active!\n"
						"Deactivating COBRA...");

		cellFsRename(SYS_COBRA_PATH "stage2.bin", SYS_COBRA_PATH "stage2_disabled.bin");

		swap_file(COLDBOOT_PATH, "", ".cobra", ".normal");

		return true; // vsh reboot
	}
	else if(file_exists(SYS_COBRA_PATH "stage2_disabled.bin"))
	{
		show_msg((char*)"COBRA is inactive!\n"
						"Activating COBRA...");

		cellFsRename(SYS_COBRA_PATH "stage2_disabled.bin", SYS_COBRA_PATH "stage2.bin");

		swap_file(COLDBOOT_PATH, "", ".normal", ".cobra");

		return true; // vsh reboot
	}
	return false;
}

static void toggle_ps2emu(void)
{
	enable_dev_blind("Swapping ps2emu activated!");
 #ifdef REX_ONLY
	if(file_exists(REBUG_TOOLBOX "ps2_netemu.self"))
	{
		struct CellFsStat s;
		uint64_t size1, size2;

		// ---- Backup PS2Emus to Rebug Toolbox folder ----
		if( file_exists(REBUG_TOOLBOX "ps2_netemu.self.cobra") == false )
			  file_copy((char*)PS2_EMU_PATH  "ps2_netemu.self",
						(char*)REBUG_TOOLBOX "ps2_netemu.self.cobra", COPY_WHOLE_FILE);

		if( file_exists(REBUG_TOOLBOX "ps2_gxemu.self.cobra") == false )
			  file_copy((char*)PS2_EMU_PATH  "ps2_gxemu.self",
						(char*)REBUG_TOOLBOX "ps2_gxemu.self.cobra", COPY_WHOLE_FILE);

		if( file_exists(REBUG_TOOLBOX "ps2_emu.self.cobra") == false )
			  file_copy((char*)PS2_EMU_PATH  "ps2_emu.self",
						(char*)REBUG_TOOLBOX "ps2_emu.self.cobra", COPY_WHOLE_FILE);

		// ---- Swap ps2_netemu.self ----
		size1 = size2 = 0;
		if( cellFsStat(PS2_EMU_PATH  "ps2_netemu.self", &s) == CELL_FS_SUCCEEDED ) size1 = s.st_size;
		if( cellFsStat(REBUG_TOOLBOX "ps2_netemu.self", &s) == CELL_FS_SUCCEEDED ) size2 = s.st_size;

		show_msg((size1==size2) ?   (char*)"Restoring original Cobra ps2emu..." :
									(char*)"Switching to custom ps2emu...");

		if(size1>0 && size2>0)
			file_copy((size1==size2) ?  (char*)REBUG_TOOLBOX "ps2_netemu.self.cobra" :
										(char*)REBUG_TOOLBOX "ps2_netemu.self",
										(char*)PS2_EMU_PATH  "ps2_netemu.self", COPY_WHOLE_FILE);

		// ---- Swap ps2_gxemu.self ----
		size1 = size2 = 0;
		if( cellFsStat(PS2_EMU_PATH  "ps2_gxemu.self", &s) == CELL_FS_SUCCEEDED ) size1 = s.st_size;
		if( cellFsStat(REBUG_TOOLBOX "ps2_gxemu.self", &s) == CELL_FS_SUCCEEDED ) size2 = s.st_size;

		if(size1>0 && size2>0)
			file_copy((size1==size2) ?  (char*)REBUG_TOOLBOX "ps2_gxemu.self.cobra" :
										(char*)REBUG_TOOLBOX "ps2_gxemu.self",
										(char*)PS2_EMU_PATH  "ps2_gxemu.self", COPY_WHOLE_FILE);

		// ---- Swap ps2_emu.self ----
		size1 = size2 = 0;
		if( cellFsStat(PS2_EMU_PATH  "ps2_emu.self", &s) == CELL_FS_SUCCEEDED ) size1 = s.st_size;
		if( cellFsStat(REBUG_TOOLBOX "ps2_emu.self", &s) == CELL_FS_SUCCEEDED ) size2 = s.st_size;

		if(size1>0 && size2>0)
			file_copy((size1==size2) ?  (char*)REBUG_TOOLBOX "ps2_emu.self.cobra" :
										(char*)REBUG_TOOLBOX "ps2_emu.self",
										(char*)PS2_EMU_PATH  "ps2_emu.self", COPY_WHOLE_FILE);
	}
	else
 #endif //#ifdef REX_ONLY
	if(file_exists(PS2_EMU_PATH "ps2_netemu.self.swap"))
	{
		show_msg((char*)"Switch to custom ps2emu...");

		swap_file(PS2_EMU_PATH, "ps2_netemu.self", "ps2_netemu.tmp", "ps2_netemu.self.swap");
		swap_file(PS2_EMU_PATH, "ps2_gxemu.self", "ps2_gxemu.tmp", "ps2_gxemu.self.swap");
		swap_file(PS2_EMU_PATH, "ps2_emu.self", "ps2_emu.tmp", "ps2_emu.self.swap");
	}
	else if(file_exists(PS2_EMU_PATH "ps2_netemu.self.sp"))
	{
		show_msg((char*)"Switching to custom ps2emu...");

		swap_file(PS2_EMU_PATH, "ps2_netemu.self", "ps2_netemu.tmp", "ps2_netemu.self.sp");
		swap_file(PS2_EMU_PATH, "ps2_gxemu.self", "ps2_gxemu.tmp", "ps2_gxemu.self.sp");
		swap_file(PS2_EMU_PATH, "ps2_emu.self", "ps2_emu.tmp", "ps2_emu.self.sp");
	}
	else if(file_exists(PS2_EMU_PATH "ps2_netemu.tmp"))
	{
		show_msg((char*)"Restoring original ps2emu...");

		if(c_firmware >= 4.65f)
		{
			swap_file(PS2_EMU_PATH, "ps2_netemu.self", "ps2_netemu.self.swap", "ps2_netemu.tmp");
			swap_file(PS2_EMU_PATH, "ps2_gxemu.self", "ps2_gxemu.self.swap", "ps2_gxemu.tmp");
			swap_file(PS2_EMU_PATH, "ps2_emu.self", "ps2_emu.self.swap", "ps2_emu.tmp");
		}
		else
		{
			swap_file(PS2_EMU_PATH, "ps2_netemu.self", "ps2_netemu.self.sp", "ps2_netemu.tmp");
			swap_file(PS2_EMU_PATH, "ps2_gxemu.self", "ps2_gxemu.self.sp", "ps2_gxemu.tmp");
			swap_file(PS2_EMU_PATH, "ps2_emu.self", "ps2_emu.self.sp", "ps2_emu.tmp");
		}
	}
}

 #endif //#ifndef LITE_EDITION
#endif //#ifdef COBRA_ONLY