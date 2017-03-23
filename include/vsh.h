//int (*_cellGcmIoOffsetToAddress)(u32, void**) = NULL;
int (*vshtask_notify)(int, const char *) = NULL;
int (*View_Find)(const char *) = NULL;
int (*plugin_GetInterface)(int,int) = NULL;

#ifdef SYS_BGM
u32 (*BgmPlaybackDisable)(int, void *) = NULL;
u32 (*BgmPlaybackEnable)(int, void *) = NULL;
#endif

int (*vshmain_is_ss_enabled)(void) = NULL;
int (*set_SSHT_)(int) = NULL;

int opd[2] = {0, 0};

#define EXPLORE_CLOSE_ALL   3

static void * getNIDfunc(const char * vsh_module, u32 fnid, s32 offset)
{
	// 0x10000 = ELF
	// 0x10080 = segment 2 start
	// 0x10200 = code start

	u32 table = (*(u32*)0x1008C) + 0x984; // vsh table address

	while(((u32)*(u32*)table) != 0)
	{
		u32* export_stru_ptr = (u32*)*(u32*)table; // ptr to export stub, size 2C, "sys_io" usually... Exports:0000000000635BC0 stru_635BC0:    ExportStub_s <0x1C00, 1, 9, 0x39, 0, 0x2000000, aSys_io, ExportFNIDTable_sys_io, ExportStubTable_sys_io>

		const char* lib_name_ptr =  (const char*)*(u32*)((char*)export_stru_ptr + 0x10);

		if(strncmp(vsh_module, lib_name_ptr, strlen(lib_name_ptr)) == 0)
		{
			// we got the proper export struct
			u32 lib_fnid_ptr = *(u32*)((char*)export_stru_ptr + 0x14);
			u32 lib_func_ptr = *(u32*)((char*)export_stru_ptr + 0x18);
			u16 count = *(u16*)((char*)export_stru_ptr + 6); // number of exports
			for(int i = 0; i < count; i++)
			{
				if(fnid == *(u32*)((char*)lib_fnid_ptr + i*4))
				{
					// take address from OPD
					return (void**)*((u32*)(lib_func_ptr) + i) + offset;
				}
			}
		}
		table += 4;
	}
	return 0;
}

static sys_memory_container_t get_app_memory_container(void)
{
	if(IS_INGAME || webman_config->mc_app) return 0;
	return vsh_memory_container_by_id(1);
}

static void show_msg(char* msg)
{
	if(!vshtask_notify)
		vshtask_notify = getNIDfunc("vshtask", 0xA02D46E7, 0);
	if(!vshtask_notify) return;

	if(strlen(msg) > 200) msg[200] = NULL; // truncate on-screen message

	vshtask_notify(0, msg);
}

static int get_game_info(void)
{
	int h = View_Find("game_plugin");

	if(h)
	{
		char _game_info[0x120];
		game_interface = (game_plugin_interface *)plugin_GetInterface(h, 1);
		game_interface->gameInfo(_game_info);

		snprintf(_game_TitleID, 10, "%s", _game_info+0x04);
		snprintf(_game_Title,   63, "%s", _game_info+0x14);
	}

	return h;
}

#ifndef LITE_EDITION
static void enable_ingame_screenshot(void)
{
	vshmain_is_ss_enabled = getNIDfunc("vshmain", 0x981D7E9F, 0); //is screenshot enabled?

	if(vshmain_is_ss_enabled() == 0)
	{
		set_SSHT_ = (u32*)&opd;
		memcpy(set_SSHT_, vshmain_is_ss_enabled, 8);
		opd[0] -= 0x2C; // Sub before vshmain_981D7E9F sets Screenshot Flag
		set_SSHT_(1);	// enable screenshot

		show_msg((char*)"Screenshot enabled");
		sys_ppu_thread_sleep(2);
	}
}
#endif

static bool abort_autoplay(void)
{
	if(webman_config->autoplay)
	{
		CellPadData pad_data = pad_read();
		if(pad_data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_L2) {BEEP2; return true;} // abort auto-play holding L2
	}
	return false;
}

#ifdef PKG_HANDLER
static void unload_web_plugins(void);
#endif
static void explore_close_all(const char *path)
{
	if(IS_INGAME) return;

#ifdef PKG_HANDLER
	unload_web_plugins();
#endif

	int view = View_Find("explore_plugin"); if(!view) return;

	explore_interface = (explore_plugin_interface *)plugin_GetInterface(view, 1);
	if(explore_interface)
	{
		explore_interface->ExecXMBcommand((char*)"close_all_list", 0, 0);
		if(strstr(path, "BDISO") || strstr(path, "DVDISO"))
			explore_interface->ExecXMBcommand((char*)"focus_category video", 0, 0);
		else
			explore_interface->ExecXMBcommand((char*)"focus_category game", 0, 0);
	}
}

static void focus_first_item(void)
{
	if(IS_ON_XMB)
	{
		explore_interface->ExecXMBcommand("focus_index 0", 0, 0);
	}
}

static void explore_exec_push(u32 usecs, u8 focus_first)
{
	if(IS_INGAME) return;

	if(explore_interface)
	{
		sys_ppu_thread_usleep(usecs);

		if(focus_first)
		{
			focus_first_item();
		}

		if(abort_autoplay() || IS_INGAME) return;

		explore_interface->ExecXMBcommand("exec_push", 0, 0);

		if(focus_first)
		{
			sys_ppu_thread_usleep(2000000);
			focus_first_item();
		}
	}
}

static void launch_disc(char *category, char *seg_name, bool execute)
{
	u8 n; int view;

#ifdef COBRA_ONLY
	unload_vsh_gui();
#endif

	for(n = 0; n < 15; n++) {if(abort_autoplay()) return; view = View_Find("explore_plugin"); if(!view) sys_ppu_thread_sleep(2); else break;}

	if(IS(seg_name, "seg_device")) wait_for("/dev_bdvd", 10); if(n) sys_ppu_thread_sleep(3);

	if(view)
	{
		// default category
		if(!*category) sprintf(category, "game");

		// default segment
		if(!*seg_name) sprintf(seg_name, "seg_device");

		if(!IS(seg_name, "seg_device") || isDir("/dev_bdvd"))
		{
			u8 retry = 0, timeout = 10, icon_found = 0;

			while(View_Find("webrender_plugin") || View_Find("webbrowser_plugin"))
			{
				sys_ppu_thread_usleep(50000); retry++; if(retry > 40) break; if(abort_autoplay()) return;
			}

			// use segment for media type
			if(IS(category, "game") && IS(seg_name, "seg_device"))
			{
				if(isDir("/dev_bdvd/PS3_GAME")) {timeout = 40, icon_found = timeout - 5;} else
				if(file_exists("/dev_bdvd/SYSTEM.CNF")) ; else
				if(isDir("/dev_bdvd/BDMV") )    {sprintf(category, "video"); sprintf(seg_name, "seg_bdmav_device");} else
				if(isDir("/dev_bdvd/VIDEO_TS")) {sprintf(category, "video"); sprintf(seg_name, "seg_dvdv_device" );} else
				if(isDir("/dev_bdvd/AVCHD"))    {sprintf(category, "video"); sprintf(seg_name, "seg_avchd_device");} else
				return;
			}

			explore_interface = (explore_plugin_interface *)plugin_GetInterface(view, 1);

			if(mount_unk == EMU_ROMS) {timeout = 2, icon_found = 1;}

			char explore_command[128]; // info: http://www.psdevwiki.com/ps3/explore_plugin

			for(n = 0; n < timeout; n++)
			{
				if(abort_autoplay() || IS_INGAME) return;

				if((n < icon_found) && file_exists("/dev_hdd0/tmp/game/ICON0.PNG")) {n = icon_found;}

				sys_ppu_thread_usleep(50000);
				explore_interface->ExecXMBcommand("close_all_list", 0, 0);
				sys_ppu_thread_usleep(150000);
				sprintf(explore_command, "focus_category %s", category);
				explore_interface->ExecXMBcommand(explore_command, 0, 0);
				sys_ppu_thread_usleep(100000);
				sprintf(explore_command, "focus_segment_index %s", seg_name);
				explore_interface->ExecXMBcommand(explore_command, 0, 0);
				sys_ppu_thread_usleep(100000);
			}

			if(execute) explore_exec_push(0, false);
		}
		else {BEEP3}
	}
}

/*
static void show_msg2(char* msg) // usage: show_msg2(L"text");
{
	if(View_Find("xmb_plugin") != 0)
	{
		xmb2_interface = (xmb_plugin_xmb2 *)plugin_GetInterface(View_Find("xmb_plugin"),'XMB2');
		xmb2_interface->showMsg(msg);
	}
}
*/
