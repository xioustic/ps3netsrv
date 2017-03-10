#define VSH_MODULE_PATH     "/dev_blind/vsh/module/"
#define VSH_ETC_PATH        "/dev_blind/vsh/etc/"

////////////////////////////////////////////////////////////////////////
//              DELETE TURNOFF FILE TO AVOID BAD SHUTDOWN            //
////////////////////////////////////////////////////////////////////////
static void soft_reboot(void)
{
	cellFsUnlink("/dev_hdd0/tmp/turnoff");
	{system_call_3(379, 0x8201, NULL, 0);}
	sys_ppu_thread_exit(0);
}

static void hard_reboot(void)
{
	cellFsUnlink("/dev_hdd0/tmp/turnoff");
	{system_call_3(379, 0x1200, NULL, 0);}
	sys_ppu_thread_exit(0);
}

static void shutdown_system(void)
{
	cellFsUnlink("/dev_hdd0/tmp/turnoff");
	{system_call_4(379, 0x1100, 0, 0, 0);}
	sys_ppu_thread_exit(0);
}

////////////////////////////////////////////////////////////////////////
//                       MOUNT DEV_BLIND                            //
////////////////////////////////////////////////////////////////////////

static void mount_dev_blind(void)
{
	system_call_8(837, (uint64_t)(char*)"CELL_FS_IOS:BUILTIN_FLSH1", (uint64_t)(char*)"CELL_FS_FAT", (uint64_t)(char*)"/dev_blind", 0, 0, 0, 0, 0);
}

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

//////////////////////////////////////////////////////////////////////
//                      TOGGLE NORMAL/REBUG MODE                    //
//////////////////////////////////////////////////////////////////////

static void toggle_normal_rebug_mode(void)
{
	mount_dev_blind();

	if(file_exists(VSH_MODULE_PATH "vsh.self.swp"))
	{
		stop_VSH_Menu();
		vshtask_notify( "Normal Mode detected!\r\n"
						"Switch to REBUG Mode...");
		play_rco_sound("system_plugin", "snd_system_ok");
		sys_timer_sleep(1);

		swap_file(VSH_ETC_PATH, "index.dat", "index.dat.nrm", "index.dat.swp");
		swap_file(VSH_ETC_PATH, "version.txt", "version.txt.nrm", "version.txt.swp");
		swap_file(VSH_MODULE_PATH, "vsh.self", "vsh.self.nrm", "vsh.self.swp");

		soft_reboot();
	}
	else
	if(file_exists(VSH_MODULE_PATH "vsh.self.nrm"))
	{
		stop_VSH_Menu();
		vshtask_notify( "Rebug Mode detected!\r\n"
						"Switch to Normal Mode...");
		play_rco_sound("system_plugin", "snd_system_ok");
		sys_timer_sleep(1);

		swap_file(VSH_ETC_PATH, "index.dat", "index.dat.swp", "index.dat.nrm");
		swap_file(VSH_ETC_PATH, "version.txt", "version.txt.swp", "version.txt.nrm");
		swap_file(VSH_MODULE_PATH, "vsh.self", "vsh.self.swp", "vsh.self.nrm");

		soft_reboot();
	}
}

////////////////////////////////////////////////////////////////////////
//                       TOGGLE XMB MODE                              //
////////////////////////////////////////////////////////////////////////

static void toggle_xmb_mode(void)
{
	mount_dev_blind();

	if(file_exists(VSH_MODULE_PATH "vsh.self.cexsp"))
	{
		stop_VSH_Menu();
		vshtask_notify( "Debug XMB detected!\r\n"
						"Switch to Retail XMB...");
		play_rco_sound("system_plugin", "snd_system_ok");
		sys_timer_sleep(1);

		swap_file(VSH_MODULE_PATH, "vsh.self", "vsh.self.dexsp", "vsh.self.cexsp");

		soft_reboot();
	}
	else
	if(file_exists(VSH_MODULE_PATH "vsh.self.dexsp"))
	{
		stop_VSH_Menu();
		vshtask_notify( "Retail XMB detected!\r\n"
						"Switch to Debug XMB...");
		play_rco_sound("system_plugin", "snd_system_ok");
		sys_timer_sleep(1);

		swap_file(VSH_MODULE_PATH, "vsh.self", "vsh.self.cexsp", "vsh.self.dexsp");

		soft_reboot();
	}
}

////////////////////////////////////////////////////////////////////////
//                      TOGGLE DEBUG MENU                             //
////////////////////////////////////////////////////////////////////////

static void toggle_debug_menu(void)
{
	mount_dev_blind();

	if(file_exists(VSH_MODULE_PATH "sysconf_plugin.sprx.dex"))
	{

		stop_VSH_Menu();
		vshtask_notify( "CEX QA Menu is active!\r\n"
						"Switch to DEX Debug Menu...");
		play_rco_sound("system_plugin", "snd_system_ok");
		sys_timer_sleep(1);

		swap_file(VSH_MODULE_PATH, "sysconf_plugin.sprx", "sysconf_plugin.sprx.cex", "sysconf_plugin.sprx.dex");
	}
	else
	if(file_exists(VSH_MODULE_PATH "sysconf_plugin.sprx.cex"))
	{
		stop_VSH_Menu();
		vshtask_notify( "DEX Debug Menu is active!\r\n"
						"Switch to CEX QA Menu...");
		play_rco_sound("system_plugin", "snd_system_ok");
		sys_timer_sleep(1);

		swap_file(VSH_MODULE_PATH, "sysconf_plugin.sprx", "sysconf_plugin.sprx.dex", "sysconf_plugin.sprx.cex");
	}
	sys_timer_sleep(1);
	{system_call_3(838, (uint64_t)(char*)"/dev_blind", 0, 1);}
}

////////////////////////////////////////////////////////////////////////
//                      DISABLE COBRA STAGE2                          //
////////////////////////////////////////////////////////////////////////

static void disable_cobra_stage2(void)
{
	stop_VSH_Menu();

	if(is_cobra_based())
	{
		mount_dev_blind();

		vshtask_notify("Cobra Mode detected!\r\nDisabling Cobra stage2...");
		play_rco_sound("system_plugin", "snd_system_ok");
		sys_timer_sleep(1);

		cellFsRename("/dev_blind/rebug/cobra/stage2.cex", "/dev_blind/rebug/cobra/stage2.cex.bak");
		cellFsRename("/dev_blind/rebug/cobra/stage2.dex", "/dev_blind/rebug/cobra/stage2.dex.bak");
		cellFsRename("/dev_blind/sys/stage2.bin", "/dev_blind/sys/stage2_disabled.bin");

		soft_reboot();
	}
	else
	{
		vshtask_notify("Cobra Mode was NOT detected!");
		play_rco_sound("system_plugin", "snd_system_ok");
		sys_timer_sleep(1);
	}
}

////////////////////////////////////////////////////////////////////////
//                      DISABLE Webman                                //
////////////////////////////////////////////////////////////////////////

static void disable_webman(void)
{
	stop_VSH_Menu();

	if(file_exists("/dev_flash/vsh/module/webftp_server.sprx"))
	{
		mount_dev_blind();
		vshtask_notify( "webMAN MOD is Enabled!\r\n"
						"Now will be Disabled...");
		play_rco_sound("system_plugin", "snd_system_ok");
		sys_timer_sleep(1);

		cellFsRename("/dev_blind/vsh/module/webftp_server.sprx", "/dev_blind/vsh/module/webftp_server.sprx.vsh");
		soft_reboot();
	}
	else if(file_exists("/dev_blind/vsh/module/webftp_server.sprx.vsh"))
	{
		mount_dev_blind();
		vshtask_notify( "webMAN MOD Disabled!\r\n"
						"Now will be Enabled...");
		play_rco_sound("system_plugin", "snd_system_ok");
		sys_timer_sleep(1);

		cellFsRename("/dev_blind/vsh/module/webftp_server.sprx.vsh", "/dev_blind/vsh/module/webftp_server.sprx");
		soft_reboot();
	}
	else
	{
		vshtask_notify("webMAN MOD was not found on /dev_flash");
		play_rco_sound("system_plugin", "snd_system_ok");
		sys_timer_sleep(1);
	}
}

static void recovery_mode(void)
{
	#define SC_UPDATE_MANAGER_IF				863
	#define UPDATE_MGR_PACKET_ID_READ_EPROM		0x600B
	#define UPDATE_MGR_PACKET_ID_WRITE_EPROM	0x600C
	#define RECOVER_MODE_FLAG_OFFSET			0x48C61

	stop_VSH_Menu();
	vshtask_notify("Now PS3 will be restarted in Recovery Mode");
	play_rco_sound("system_plugin", "snd_system_ok");
	sys_timer_sleep(1);

   {system_call_7(SC_UPDATE_MANAGER_IF, UPDATE_MGR_PACKET_ID_WRITE_EPROM, RECOVER_MODE_FLAG_OFFSET, 0x00, 0, 0, 0, 0);} // set recovery mode
	hard_reboot();
}

