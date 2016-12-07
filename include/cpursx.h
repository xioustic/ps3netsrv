static void get_cobra_version(char *cfw_info)
{
	// returns cfw_info[20]

#ifdef COBRA_ONLY
	if(!is_mamba && !syscalls_removed) {system_call_1(SC_COBRA_SYSCALL8, SYSCALL8_OPCODE_GET_MAMBA); is_mamba = ((int)p1 ==0x666);}

	if(!cobra_version) sys_get_version2(&cobra_version);

	char cobra_ver[8];
	if((cobra_version & 0x0F) == 0)
		sprintf(cobra_ver, "%X.%X", cobra_version>>8, (cobra_version & 0xFF) >> 4);
	else
		sprintf(cobra_ver, "%X.%02X", cobra_version>>8, (cobra_version & 0xFF));

	#if defined(DECR_SUPPORT)
		sprintf(cfw_info, "%s %s: %s", (dex_mode == 1) ? "DECR" : dex_mode ? "DEX" : "CEX", is_mamba ? "Mamba" : "Cobra", cobra_ver);
	#elif defined(DEX_SUPPORT)
		sprintf(cfw_info, "%s %s: %s", dex_mode ? "DEX" : "CEX", is_mamba ? "Mamba" : "Cobra", cobra_ver);
	#else
		sprintf(cfw_info, "%s %s: %s", "CEX", is_mamba ? "Mamba" : "Cobra", cobra_ver);
	#endif
#elif DEX_SUPPORT
	#if defined(DECR_SUPPORT)
		sprintf(cfw_info, "%s", (dex_mode == 1) ? "DECR" : dex_mode ? "DEX" : "CEX");
	#else
		sprintf(cfw_info, "%s", dex_mode ? "DEX" : "CEX");
	#endif
#else
		sprintf(cfw_info, "CEX");
#endif
}

static void get_net_info(char *net_type, char *ip)
{
	// returns net_type[8], ip[ip_size]

	int32_t status = 0; xsetting_F48C0548()->GetSettingNet_enable(&status);

	if(status == 0) {strcpy(net_type, "OFFLINE"); *ip = NULL; return;}

	net_info info;
	memset(&info, 0, sizeof(net_info));
	xsetting_F48C0548()->sub_44A47C(&info); //info.ipAddress

	if (info.device == 0) strcpy(net_type, "LAN"); else
	if (info.device == 1) strcpy(net_type, "WLAN");

	netctl_main_9A528B81(ip_size, ip);
}

static void add_game_info(char *buffer, char *templn, bool is_cpursx)
{
	if(IS_INGAME)
	{
		get_game_info();

		if(strlen(_game_TitleID) == 9)
		{
			if(is_cpursx && sys_admin)
			{
#ifdef GET_KLICENSEE
				buffer += concat(buffer, " [<a href=\"/klic.ps3\">KLIC</a>]");
#endif
#ifdef SYS_BGM
				buffer += concat(buffer, " [<a href=\"/sysbgm.ps3\">BGM</a>]");
#endif
#ifdef VIDEO_REC
				buffer += concat(buffer, " [<a href=\"/videorec.ps3\">REC</a>]");
#endif
			}

			char path[MAX_PATH_LEN], version[8] = "01.00", *app_ver = version;

			sprintf(templn, "<hr><span style=\"position:relative;top:-20px;\"><H2><a href=\"%s/%s/%s-ver.xml\" target=\"_blank\">%s</a>", "https://a0.ww.np.dl.playstation.net/tpl/np", _game_TitleID, _game_TitleID, _game_TitleID); buffer += concat(buffer, templn);

			sprintf(path, "%s%s/PARAM.SFO", HDD0_GAME_DIR, _game_TitleID);
			if(file_exists(path) == false) sprintf(path, "/dev_bdvd/PS3_GAME/PARAM.SFO");

			getTitleID(path, app_ver, GET_VERSION); if(*app_ver == '0') *app_ver='v'; if(strstr(_game_Title, app_ver)) *app_ver = NULL;

			sprintf(templn, " <a href=\"%s%s\">%s %s</a> &nbsp; ", search_url, _game_Title, _game_Title, app_ver); buffer += concat(buffer, templn);

			sprintf(path, "%s%s", HDD0_GAME_DIR, _game_TitleID);
			if(file_exists(path) == false) sprintf(path, "/dev_bdvd/PS3_GAME");

			sprintf(templn, "<a href=\"%s\"><img src=\"%s/ICON0.PNG\" height=\"60\" border=0%s></a>", path, path, " style=\"position:relative;top:20px;\""); buffer += concat(buffer, templn);

			buffer += concat(buffer, "</H2></span>");
		}
	}
}


static void cpu_rsx_stats(char *buffer, char *templn, char *param, u8 is_ps3_http)
{
	{ PS3MAPI_ENABLE_ACCESS_SYSCALL8 }

	u32 t1 = 0, t2 = 0, t1f, t2f;
	get_temperature(0, &t1); // CPU // 3E030000 -> 3E.03°C -> 62.(03/256)°C
	get_temperature(1, &t2); // RSX

	t1f = (1.8f*(float)t1+32.f);
	t2f = (1.8f*(float)t2+32.f);

	_meminfo meminfo;
	{system_call_1(SC_GET_FREE_MEM, (uint64_t)(u32) &meminfo);}

	if((webman_config->fanc == 0) && (get_fan_policy_offset > 0))
	{
		u8 st, mode, unknown;
		backup[2]=peekq(get_fan_policy_offset);
		lv2poke32(get_fan_policy_offset, 0x38600001); // sys 409 get_fan_policy
		sys_sm_get_fan_policy(0, &st, &mode, &fan_speed, &unknown);
		pokeq(get_fan_policy_offset, backup[2]);

		if(strstr(param, "?u")) enable_fan_control(3, templn);
	}

#ifdef SPOOF_CONSOLEID
	get_eid0_idps();
	get_idps_psid();
#endif

	if(sys_admin) {sprintf(templn, " [<a href=\"/shutdown.ps3\">%s</a>] [<a href=\"/restart.ps3\">%s</a>]", STR_SHUTDOWN, STR_RESTART ); buffer += concat(buffer, templn);}

	add_game_info(buffer, templn, true);

#ifdef COPY_PS3
	if(copy_in_progress)
	{
		sprintf(templn, "<hr><font size=2><a href=\"%s$abort\">&#9746 %s</a> %s (%i %s)</font>", "/copy.ps3", STR_COPYING, current_file, copied_count, STR_FILES); buffer += concat(buffer, templn);
	}
	else if(fix_in_progress)
	{
		sprintf(templn, "<hr><font size=2><a href=\"%s$abort\">&#9746 %s</a> %s (%i %s)</font>", "/fixgame.ps3", STR_FIXING, current_file, fixed_count, STR_FILES); buffer += concat(buffer, templn);
	}
#endif

	if(strstr(param, "?"))
	{
		char *pos = strstr(param, "fan=");
		if(pos) {u32 read = get_valuen(param, "fan=", 0, 99); max_temp = 0; if(!read) enable_fan_control(0, templn); else {webman_config->manu = read; if(webman_config->fanc == 0) enable_fan_control(1, templn);}}
		else {pos = strstr(param, "max=");
		if(pos) {max_temp = get_valuen(param, "max=", 40, MAX_TEMPERATURE);}
		else
		if(strstr(param, "?m")) {if((max_temp && !strstr(param, "dyn")) || strstr(param, "man")) max_temp=0; else {max_temp=webman_config->temp1;} if(webman_config->fanc == 0) enable_fan_control(1, templn);}}

		if(max_temp) //auto mode
		{
			if(strstr(param, "?u")) max_temp++;
			if(strstr(param, "?d")) max_temp--;
			webman_config->temp1=RANGE(max_temp, 40, MAX_TEMPERATURE); // dynamic fan max temperature in °C
			webman_config->temp0=FAN_AUTO;

			fan_ps2_mode=false;
		}
		else
		{
			if(strstr(param, "?u")) webman_config->manu++;
			if(strstr(param, "?d")) webman_config->manu--;
			webman_config->manu=RANGE(webman_config->manu, 20, 95); //%

			reset_fan_mode();
		}
		save_settings();
	}

	{ PS3MAPI_ENABLE_ACCESS_SYSCALL8 }

	char max_temp1[128], max_temp2[64]; *max_temp2 = NULL;

	if(fan_ps2_mode)
	{
		sprintf(max_temp1, " (PS2 Mode: %i%%)", webman_config->ps2temp);
	}
	else if((webman_config->fanc == 0) || (!webman_config->temp0 && !max_temp))
		sprintf(max_temp1, " <small>[%s %s]</small>", STR_FANCTRL3, STR_DISABLED);
	else if(max_temp)
	{
		sprintf(max_temp1, " (MAX: %i°C)", max_temp);
		sprintf(max_temp2, " (MAX: %i°F)", (int)(1.8f*(float)max_temp+32.f));
	}
	else
		sprintf(max_temp1, " <small>[FAN: %i%% %s]</small>", webman_config->manu, STR_MANUAL);

	*templn = NULL;

	int hdd_free;

#ifndef LITE_EDITION
	for(u8 d = 1; d < 7; d++)
	{
		if(isDir(drives[d]))
		{
			hdd_free = (int)(get_free_space(drives[d])>>20);
			sprintf(param, "<br><a href=\"%s\">USB%c%c%c: %'d %s</a>", drives[d], drives[d][8], drives[d][9], drives[d][10], hdd_free, STR_MBFREE); strcat(templn, param);
		}
	}
#endif

	hdd_free = (int)(get_free_space("/dev_hdd0")>>20);

	sprintf(param, "<hr><font size=\"42px\"><b><a class=\"s\" href=\"/cpursx.ps3?up\">"
											"CPU: %i°C%s<br>"
											"RSX: %i°C</a><hr>"
											"<a class=\"s\" href=\"/cpursx.ps3?dn\">"
											"CPU: %i°F%s<br>"
											"RSX: %i°F</a><hr>"
											"<a class=\"s\" href=\"/games.ps3\">"
											"MEM: %'d KB</a><br>"
											"<a href=\"%s\">HDD: %'d %s</a>%s<hr>"
											"<a class=\"s\" href=\"/cpursx.ps3?mode\">"
											"%s %i%% (0x%X)</a><br>",
					t1, max_temp1, t2,
					t1f, max_temp2, t2f,
					(meminfo.avail>>10), drives[0], hdd_free, STR_MBFREE, templn,
					STR_FANCH2, (int)((int)fan_speed*100)/255, fan_speed); buffer += concat(buffer, param);

	if( !max_temp && !is_ps3_http)
		sprintf( templn, "<input type=\"range\" value=\"%i\" min=\"%i\" max=\"95\" style=\"width:600px\" onchange=\"self.location='/cpursx.ps3?fan='+this.value\"><hr>", webman_config->manu, DEFAULT_MIN_FANSPEED);
	else
		sprintf( templn, "<hr>");

	buffer += concat(buffer, templn);

	CellRtcTick pTick; cellRtcGetCurrentTick(&pTick); u32 dd, hh, mm, ss;

	// detect aprox. time when a game is launched
	if(IS_ON_XMB) gTick=rTick; else if(gTick.tick==rTick.tick) cellRtcGetCurrentTick(&gTick);

	////// play time //////
	if(gTick.tick>rTick.tick)
	{
		ss = (u32)((pTick.tick - gTick.tick)/1000000);
		dd = (u32)(ss / 86400); ss = ss % 86400; hh = (u32)(ss / 3600); ss = ss % 3600; mm = (u32)(ss / 60); ss = ss % 60;
		if(dd<100) {sprintf( templn, "<label title=\"Play\">&#9737;</label> %id %02d:%02d:%02d<br>", dd, hh, mm, ss); buffer += concat(buffer, templn);}
	}
	///////////////////////

	//// startup time /////
	ss = (u32)((pTick.tick - rTick.tick)/1000000);
	dd = (u32)(ss / 86400); ss = ss % 86400; hh = (u32)(ss / 3600); ss = ss % 3600; mm = (u32)(ss / 60); ss = ss % 60;
	sprintf( templn, "<a href=\"/dev_hdd0/home/%08i\"><label title=\"Startup\">&#8986;</label> %id %02d:%02d:%02d</a>", xsetting_CC56EB2D()->GetCurrentUserNumber(), dd, hh, mm, ss); buffer += concat(buffer, templn);
	///////////////////////

	if(isDir("/dev_bdvd"))
	{
		get_last_game(param);

		if(*param == '/') {sprintf( templn, "<hr><font size=\"3\">" HTML_URL " -> ", IS_ON_XMB ? "/play.ps3" : "/dev_bdvd", "/dev_bdvd"); buffer += concat(buffer, templn); add_breadcrumb_trail(buffer, param); buffer += concat(buffer, "</font>");}
	}

	// Get mac address [0xD-0x12]
	if(sys_admin)
	{
		u8 mac_address[0x13];
		{system_call_3(SYS_NET_EURUS_POST_COMMAND, CMD_GET_MAC_ADDRESS, (u64)(u32)mac_address, 0x13);}

		char *cfw_info = param;
		get_cobra_version(cfw_info);

		char net_type[8] = "", ip[ip_size] = "-";
		get_net_info(net_type, ip);

		sprintf( templn, "<hr></font><h2><a class=\"s\" href=\"/setup.ps3\">"
							"Firmware : %s %s<br>"
							"%s<br>"
#ifdef SPOOF_CONSOLEID
						"PSID LV2 : %016llX%016llX<hr>"
						"IDPS EID0: %016llX%016llX<br>"
						"IDPS LV2 : %016llX%016llX<br>"
#endif
						"MAC Addr : %02X:%02X:%02X:%02X:%02X:%02X - %s %s</h2></a></b>",
						fw_version, cfw_info,
						(syscalls_removed) ? STR_CFWSYSALRD : "",
#ifdef SPOOF_CONSOLEID
						PSID[0], PSID[1],
						eid0_idps[0], eid0_idps[1],
						IDPS[0], IDPS[1],
#endif
						mac_address[13], mac_address[14], mac_address[15], mac_address[16], mac_address[17], mac_address[18], ip, net_type); buffer += concat(buffer, templn);
	}

	/////////////////////////////
#ifdef COPY_PS3
	if(copy_in_progress)
	{
		sprintf( templn, "<hr>%s %s (%i %s)", STR_COPYING, current_file, copied_count, STR_FILES); buffer += concat(buffer, templn);
	}
	else
	if(fix_in_progress)
	{
		sprintf( templn, "<hr>%s %s (%i %s)", STR_FIXING, current_file, fixed_count, STR_FILES); buffer += concat(buffer, templn);
	}
#endif
	/////////////////////////////

	strcat(buffer,  HTML_BLU_SEPARATOR
					"webMAN - Simple Web Server" EDITION "<p>");

	{ PS3MAPI_DISABLE_ACCESS_SYSCALL8 }
}

static void get_cpursx(char *cpursx)
{
	u32 t1 = 0, t2 = 0;
	get_temperature(0, &t1); // CPU // 3E030000 -> 3E.03°C -> 62.(03/256)°C
	get_temperature(1, &t2); // RSX

	sprintf(cpursx, "CPU: %i°C | RSX: %i°C", t1, t2);
}
