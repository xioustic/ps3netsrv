#define MAX_ISO_PARTS			(16)
#define MAX_LAST_GAMES			(5)
#define LAST_GAMES_UPPER_BOUND	(4)

// File name TAGS:
// [auto]   Auto-play
// [online] Auto-disable syscalls
// [gd]     Auto-enable external gameDATA
// [raw]    Use raw_iso.sprx to mount the ISO (ntfs)
// [PS2]    PS2 extracted folders in /PS2DISC (needs PS2_DISC compilation flag)

typedef struct
{
	uint8_t last;
	char game[MAX_LAST_GAMES][MAX_PATH_LEN];
} __attribute__((packed)) _lastgames;

#ifndef COBRA_ONLY
 static uint64_t base_addr=0;
 static uint64_t open_hook=0;

 typedef struct
 {
	char src[384];
	char dst[384];
 } redir_files_struct;

 static redir_files_struct file_to_map[10];
#endif

#define IS_COPY		9

#define TEMP_NET_PSXISO  WMTMP "/~netpsx.iso"
#define PLAYSTATION      "PLAYSTATION "

static void detect_firmware(void)
{
	if((c_firmware > 3.40f) || SYSCALL_TABLE || syscalls_removed) return;

	const uint64_t CEX = 0x4345580000000000ULL;

#ifdef DEX_SUPPORT
	const uint64_t DEX = 0x4445580000000000ULL;
#endif
#ifdef DECR_SUPPORT
	const uint64_t DEH = 0x4445480000000000ULL;
#endif

	dex_mode = 0;

	for(uint8_t lv2_offset = 1; lv2_offset < 0x10; lv2_offset++)
	{
		if(SYSCALL_TABLE) break;

		LV2_OFFSET_ON_LV1 = (uint64_t)lv2_offset * 0x1000000ULL;

		if(peekq(0x80000000002ED808ULL) == CEX) {SYSCALL_TABLE = SYSCALL_TABLE_480;  c_firmware = 4.80f;}				else
		if(peekq(0x80000000002ED818ULL) == CEX) {SYSCALL_TABLE = SYSCALL_TABLE_475;  c_firmware = (peekq(0x80000000002FCB68ULL) == 0x323031352F31322FULL)?4.78f:(peekq(0x80000000002FCB68ULL) == 0x323031352F30382FULL)?4.76f:4.75f;} else
		if(peekq(0x80000000002ED778ULL) == CEX) {SYSCALL_TABLE = SYSCALL_TABLE_470;  c_firmware = 4.70f;}				else
		if(peekq(0x80000000002ED860ULL) == CEX) {SYSCALL_TABLE = SYSCALL_TABLE_465;  c_firmware = (peekq(0x80000000002FC938ULL) == 0x323031342F31312FULL)?4.66f:4.65f;} else
		if(peekq(0x80000000002ED850ULL) == CEX) {SYSCALL_TABLE = SYSCALL_TABLE_460;  c_firmware = 4.60f;}				else
		if(peekq(0x80000000002EC5E0ULL) == CEX) {SYSCALL_TABLE = SYSCALL_TABLE_455;  c_firmware = 4.55f;}				else
		if(peekq(0x80000000002E9D70ULL) == CEX) {SYSCALL_TABLE = SYSCALL_TABLE_453;  c_firmware = 4.53f;}				else
		if(peekq(0x80000000002E9BE0ULL) == CEX) {SYSCALL_TABLE = SYSCALL_TABLE_450;  c_firmware = 4.50f;}				else
		if(peekq(0x80000000002EA9B8ULL) == CEX) {SYSCALL_TABLE = SYSCALL_TABLE_446;  c_firmware = 4.46f;}				else
		if(peekq(0x80000000002E8610ULL) == CEX) {SYSCALL_TABLE = SYSCALL_TABLE_421;  c_firmware = 4.21f;}				else
		if(peekq(0x80000000002D83D0ULL) == CEX) {SYSCALL_TABLE = SYSCALL_TABLE_355;  c_firmware = 3.55f;}				else

 //No cobra cfw but as mamba compatibility
		if(peekq(0x80000000002EA498ULL) == CEX) {SYSCALL_TABLE = SYSCALL_TABLE_441;  c_firmware = 4.41f;}				else
		if(peekq(0x80000000002EA488ULL) == CEX) {SYSCALL_TABLE = SYSCALL_TABLE_440;  c_firmware = 4.40f;}				else
		if(peekq(0x80000000002E9F18ULL) == CEX) {SYSCALL_TABLE = SYSCALL_TABLE_431;  c_firmware = 4.31f;}				else
		if(peekq(0x80000000002E9F08ULL) == CEX) {SYSCALL_TABLE = SYSCALL_TABLE_430;  c_firmware = 4.30f;}				else

#ifdef DEX_SUPPORT
		if(peekq(0x800000000030F3A0ULL) == DEX) {SYSCALL_TABLE = SYSCALL_TABLE_480D; c_firmware = 4.80f; dex_mode = 2;}	else
		if(peekq(0x800000000030F2D0ULL) == DEX) {SYSCALL_TABLE = SYSCALL_TABLE_475D; c_firmware = (peekq(0x800000000031EF48ULL) == 0x323031352F31322FULL)?4.78f:(peekq(0x800000000031EF48ULL) == 0x323031352F30382FULL)?4.76f:4.75f; dex_mode = 2;}	else
		if(peekq(0x800000000030F240ULL) == DEX) {SYSCALL_TABLE = SYSCALL_TABLE_470D; c_firmware = 4.70f; dex_mode = 2;}	else
		if(peekq(0x800000000030F1A8ULL) == DEX) {SYSCALL_TABLE = SYSCALL_TABLE_465D; c_firmware = (peekq(0x800000000031EBA8ULL) == 0x323031342F31312FULL)?4.66f:4.65f; dex_mode = 2;} else
		if(peekq(0x800000000030F198ULL) == DEX) {SYSCALL_TABLE = SYSCALL_TABLE_460D; c_firmware = 4.60f; dex_mode = 2;}	else
		if(peekq(0x800000000030D6A8ULL) == DEX) {SYSCALL_TABLE = SYSCALL_TABLE_455D; c_firmware = 4.55f; dex_mode = 2;}	else
		if(peekq(0x800000000030AEA8ULL) == DEX) {SYSCALL_TABLE = SYSCALL_TABLE_453D; c_firmware = 4.53f; dex_mode = 2;}	else
		if(peekq(0x8000000000309698ULL) == DEX) {SYSCALL_TABLE = SYSCALL_TABLE_450D; c_firmware = 4.50f; dex_mode = 2;}	else
		if(peekq(0x8000000000305410ULL) == DEX) {SYSCALL_TABLE = SYSCALL_TABLE_446D; c_firmware = 4.46f; dex_mode = 2;}	else
		if(peekq(0x8000000000304EF0ULL) == DEX) {SYSCALL_TABLE = SYSCALL_TABLE_441D; c_firmware = 4.41f; dex_mode = 2;}	else
		if(peekq(0x8000000000304EE0ULL) == DEX) {SYSCALL_TABLE = SYSCALL_TABLE_440D; c_firmware = 4.40f; dex_mode = 2;}	else
		if(peekq(0x8000000000304640ULL) == DEX) {SYSCALL_TABLE = SYSCALL_TABLE_431D; c_firmware = 4.31f; dex_mode = 2;}	else
		if(peekq(0x8000000000304630ULL) == DEX) {SYSCALL_TABLE = SYSCALL_TABLE_430D; c_firmware = 4.30f; dex_mode = 2;}	else
		if(peekq(0x8000000000302D88ULL) == DEX) {SYSCALL_TABLE = SYSCALL_TABLE_421D; c_firmware = 4.21f; dex_mode = 2;}	else
		if(peekq(0x80000000002EFE20ULL) == DEX) {SYSCALL_TABLE = SYSCALL_TABLE_355D; c_firmware = 3.55f; dex_mode = 2;}	else
#endif

#ifdef DECR_SUPPORT
		if(peekq(0x8000000000319F78ULL) == DEH) {SYSCALL_TABLE = SYSCALL_TABLE_355H; c_firmware = 3.55f; dex_mode = 1;}	else
	  //if(peekq(0x800000000032B270ULL) == DEH) {SYSCALL_TABLE = SYSCALL_TABLE_450H; c_firmware = 4.50f; dex_mode = 1;}	else
		if(peekq(0x800000000032EDC8ULL) == DEH) {SYSCALL_TABLE = SYSCALL_TABLE_460H; c_firmware = 4.60f; dex_mode = 1;}	else
		if(peekq(0x800000000032EB60ULL) == DEH) {SYSCALL_TABLE = SYSCALL_TABLE_475H; c_firmware = (peekq(0x8000000000344B70ULL) == 0x323031352F31322FULL)?4.78f:(peekq(0x8000000000344B70ULL) == 0x323031352F30382FULL)?4.76f:4.75f; dex_mode = 1;}	else
		if(peekq(0x800000000032EB60ULL) == DEH) {SYSCALL_TABLE = SYSCALL_TABLE_480H; c_firmware = 4.80f; dex_mode = 1;}	else
#endif

#ifndef COBRA_ONLY
		if(peekq(0x80000000002CFF98ULL) == CEX) {SYSCALL_TABLE = SYSCALL_TABLE_341;  c_firmware = 3.41f;} else
		//if(peekq(0x80000000002E79C8ULL) == DEX) {c_firmware = 3.41f; dex_mode = 2;}	else
#endif
		if(SYSCALL_TABLE) break;
	}

	if(!SYSCALL_TABLE) {c_firmware = 0.00f; return;}

	// get payload type & version
	{system_call_1(SC_COBRA_SYSCALL8, SYSCALL8_OPCODE_GET_MAMBA); is_mamba = ((int)p1 == 0x666);}
	sys_get_version2(&cobra_version);

#ifndef COBRA_ONLY
	if(!dex_mode)
	{   // CEX
		if(c_firmware == 3.41f) {base_addr = 0x2B5D30; open_hook = 0x2AAFC8;} else
		if(c_firmware == 3.55f) {base_addr = 0x2BE0D0; open_hook = 0x2B3274;} else
		if(c_firmware == 4.21f) {base_addr = 0x2D0C98; open_hook = 0x2C2558;} else
		if(c_firmware == 4.30f) {base_addr = 0x2D2418; open_hook = 0x2C3CD4;} else
		if(c_firmware == 4.31f) {base_addr = 0x2D2428; open_hook = 0x2C3CE0;} else
		if(c_firmware == 4.40f) {base_addr = 0x2D29A8; open_hook = 0x2C4284;} else
		if(c_firmware == 4.41f) {base_addr = 0x2D29B8; open_hook = 0x2C4290;} else
		if(c_firmware == 4.46f) {base_addr = 0x2D2ED8; open_hook = 0x2C47B0;} else
		if(c_firmware == 4.50f) {base_addr = 0x2D4CB8; open_hook = 0x29DD20;} else
		if(c_firmware == 4.53f) {base_addr = 0x2D4E48; open_hook = 0x29DEF8;} else
		if(c_firmware == 4.55f) {base_addr = 0x2D7660; open_hook = 0x29F748;} else
		if(c_firmware == 4.60f) {base_addr = 0x2D88D0; open_hook = 0x2A02BC;} else
		if(c_firmware == 4.65f) {base_addr = 0x2D88E0; open_hook = 0x2A02C8;} else
		if(c_firmware == 4.66f) {base_addr = 0x2D88E0; open_hook = 0x2A02C8;} else
		if(c_firmware == 4.70f) {base_addr = 0x2D8A70; open_hook = 0x2975C0;} else
		if(c_firmware >= 4.75f && c_firmware <= 4.78f) {base_addr = 0x2D8AF0; open_hook = 0x297638;} else
		if(c_firmware == 4.80f) {base_addr = 0x2D8AE0; open_hook = 0x29762C;}
	}
 #ifdef DECR_SUPPORT
	else if(dex_mode == 1)
	{   // DECR
		if(c_firmware == 3.55f) {base_addr = 0x2F5320; open_hook = 0x2E31F4;} else
	  //if(c_firmware == 4.50f) {base_addr = 0x30D2C0; open_hook = 0x2CEF08;} else
		if(c_firmware == 4.60f) {base_addr = 0x310EE0; open_hook = 0x2D1464;} else
		if(c_firmware == 4.78f) {base_addr = 0x3110F0; open_hook = 0x2C87E0;}
		if(c_firmware == 4.80f) {base_addr = 0x3110F0; open_hook = 0x2C87D4;}
	}
 #endif
 #ifdef DEX_SUPPORT
	else if(dex_mode == 2)
	{   // DEX
		if(c_firmware == 3.55f) {base_addr = 0x2D5B20; open_hook = 0x2C8A94;} else
		if(c_firmware == 4.21f) {base_addr = 0x2EB418; open_hook = 0x2D9718;} else
		if(c_firmware == 4.30f) {base_addr = 0x2ECB48; open_hook = 0x2DAE4C;} else
		if(c_firmware == 4.31f) {base_addr = 0x2ECB58; open_hook = 0x2DAE58;} else
		if(c_firmware == 4.40f) {base_addr = 0x2ED408; open_hook = 0x2DB730;} else
		if(c_firmware == 4.41f) {base_addr = 0x2ED418; open_hook = 0x2DB73C;} else
		if(c_firmware == 4.46f) {base_addr = 0x2ED938; open_hook = 0x2DBC5C;} else
		if(c_firmware == 4.50f) {base_addr = 0x2F4778; open_hook = 0x2B81E8;} else
		if(c_firmware == 4.53f) {base_addr = 0x2F5F88; open_hook = 0x2B83C0;} else
		if(c_firmware == 4.55f) {base_addr = 0x2F8730; open_hook = 0x2B9C14;} else
		if(c_firmware == 4.60f) {base_addr = 0x2FA220; open_hook = 0x2BB004;} else
		if(c_firmware == 4.65f) {base_addr = 0x2FA230; open_hook = 0x2BB010;} else
		if(c_firmware == 4.66f) {base_addr = 0x2FA230; open_hook = 0x2BB010;} else
		if(c_firmware == 4.70f) {base_addr = 0x2FA540; open_hook = 0x2B2480;} else
		if(c_firmware >= 4.75f && c_firmware <= 4.78f) {base_addr = 0x2FA5B0; open_hook = 0x2B24F8;}
		if(c_firmware == 4.80f) {base_addr = 0x2FA680; open_hook = 0x2B25C4;}
	}
 #endif

	base_addr |= 0x8000000000000000ULL;
	open_hook |= 0x8000000000000000ULL;
#endif

	if(!dex_mode)
	{ // CEX
		if(c_firmware >= 4.55f && c_firmware <= 4.80f)
		{
			get_fan_policy_offset = 0x8000000000009E38ULL; // sys 409 get_fan_policy  4.55/4.60/4.65/4.70/4.75/4.76/4.78/4.80
			set_fan_policy_offset = 0x800000000000A334ULL; // sys 389 set_fan_policy

			// idps / psid cex
			if(c_firmware == 4.55f)
			{
				idps_offset1 = 0x80000000003E17B0ULL;
				idps_offset2 = 0x8000000000474F1CULL;
			}
			else if(c_firmware >= 4.60f && c_firmware <= 4.66f)
			{
				idps_offset1 = 0x80000000003E2BB0ULL;
				idps_offset2 = 0x8000000000474F1CULL;
			}
			else if(c_firmware == 4.70f)
			{
				idps_offset1 = 0x80000000003E2DB0ULL;
				idps_offset2 = 0x8000000000474AF4ULL;
			}
			else if(c_firmware >= 4.75f && c_firmware <= 4.80f)
			{
				idps_offset1 = 0x80000000003E2E30ULL;
				idps_offset2 = 0x8000000000474AF4ULL;
			}

			if(idps_offset2) psid_offset = idps_offset2 + 0x18ULL;
		}
		else if(c_firmware >= 4.21f && c_firmware <= 4.53f)
		{
			get_fan_policy_offset = 0x8000000000009E28ULL; // sys 409 get_fan_policy  4.21/4.30/4.31/4.40/4.41/4.46/4.50/4.53
			set_fan_policy_offset = 0x800000000000A324ULL; // sys 389 set_fan_policy
		}
		else if(c_firmware == 3.55f)
		{
			get_fan_policy_offset = 0x8000000000008CBCULL; // sys 409 get_fan_policy
			set_fan_policy_offset = 0x80000000000091B8ULL; // sys 389 set_fan_policy
		}
#ifndef COBRA_ONLY
		else if(c_firmware == 3.41f)
		{
			get_fan_policy_offset = 0x8000000000008644ULL; // sys 409 get_fan_policy
			set_fan_policy_offset = 0x8000000000008B40ULL; // sys 389 set_fan_policy
		}
#endif
	}
#if defined(DEX_SUPPORT) || defined(DECR_SUPPORT)
	else if(dex_mode)
	{ // DEX & DEH
		if(c_firmware >= 4.55f && c_firmware <= 4.80f)
		{
				get_fan_policy_offset = 0x8000000000009EB8ULL; // sys 409 get_fan_policy  4.55/4.60/4.65/4.70/4.75/4.76/4.78/4.80
				set_fan_policy_offset = 0x800000000000A3B4ULL; // sys 389 set_fan_policy

			// idps / psid dex
#ifdef DECR_SUPPORT
			if(dex_mode == 1)
			{ // DEH
				if(c_firmware >= 4.60f && c_firmware <= 4.66f)
				{
					idps_offset1 = 0x8000000000432430ULL;
					idps_offset2 = 0x80000000004C4F1CULL;
				}
				else if(c_firmware >= 4.75f && c_firmware <= 4.80f)
				{
					idps_offset1 = 0x80000000004326B0ULL;
					idps_offset2 = 0x80000000004C4AF4ULL;
				}
			}
			else
#endif
			if(c_firmware == 4.55f)
			{
				idps_offset1 = 0x8000000000407930ULL;
				idps_offset2 = 0x8000000000494F1CULL;
			}
			else if(c_firmware >= 4.60f && c_firmware <= 4.66f)
			{
				idps_offset1 = 0x80000000004095B0ULL;
				idps_offset2 = 0x800000000049CF1CULL;
			}
			else if(c_firmware == 4.70f)
			{
				idps_offset1 = 0x80000000004098B0ULL;
				idps_offset2 = 0x800000000049CAF4ULL;
			}
			else if(c_firmware >= 4.75f && c_firmware <= 4.78f)
			{
				idps_offset1 = 0x8000000000409930ULL;
				idps_offset2 = 0x800000000049CAF4ULL;
			}
			else if(c_firmware == 4.80f)
			{
				idps_offset1 = 0x8000000000409A30ULL;
				idps_offset2 = 0x800000000049CAF4ULL;
			}

			if(idps_offset2) psid_offset = idps_offset2 + 0x18ULL;
		}
		else if(c_firmware >= 4.21f && c_firmware <= 4.53f)
		{
				get_fan_policy_offset = 0x8000000000009EA8ULL; // sys 409 get_fan_policy  4.21/4.30/4.31/4.40/4.41/4.46/4.50/4.53
				set_fan_policy_offset = 0x800000000000A3A4ULL; // sys 389 set_fan_policy
		}
		else if(c_firmware == 3.55f)
		{
				get_fan_policy_offset = 0x8000000000008D3CULL; // sys 409 get_fan_policy
				set_fan_policy_offset = 0x8000000000009238ULL; // sys 389 set_fan_policy
		}
	}
#endif

#ifndef COBRA_ONLY
	install_peek_poke();
#endif
}

#ifndef COBRA_ONLY
static void add_to_map(char *path1, char *path2)
{
	if(max_mapped == 0) pokeq(MAP_BASE + 0x00, 0x0000000000000000ULL);

	if(max_mapped < 10)
	{
		for(u8 n = 0; n < max_mapped; n++)
		{
			if(!strcmp(file_to_map[n].src, path1)) return;
		}

		sprintf(file_to_map[max_mapped].src, "%s", path1);
		sprintf(file_to_map[max_mapped].dst, "%s", path2);
		max_mapped++;
	}
}

static void string_to_lv2(char* path, uint64_t addr)
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
}
#endif

#ifdef COBRA_ONLY
static void cache_icon0_and_param_sfo(char *templn)
{
	strcat(templn, ".SFO\0");

	// cache PARAM.SFO
	if(file_exists(templn) == false)
	{
		for(u8 n = 0; n < 10; n++)
		{
			if(file_copy((char*)"/dev_bdvd/PS3_GAME/PARAM.SFO", templn, _4KB_) == CELL_FS_SUCCEEDED) break;
			sys_timer_usleep(500000);
		}
	}

	// cache ICON0.PNG
	templn[strlen(templn)-4] = NULL; strcat(templn, ".PNG");
	if((webman_config->nocov!=2) && file_exists(templn) == false)
	{
		for(u8 n = 0; n < 10; n++)
		{
			if(file_copy((char*)"/dev_bdvd/PS3_GAME/ICON0.PNG", templn, COPY_WHOLE_FILE) == CELL_FS_SUCCEEDED) break;
			sys_timer_usleep(500000);
		}
	}
}

 #ifndef LITE_EDITION
static void cache_file_to_hdd(char *source, char *target, const char *basepath, char *msg)
{
	sprintf(target, "/dev_hdd0%s", basepath);
	cellFsMkdir(basepath, MODE);
	strcat(target, strrchr(source, '/'));

	if((copy_in_progress || fix_in_progress) == false && file_exists(target) == false)
	{
		sprintf(msg, "%s %s\n"
					 "%s %s", STR_COPYING, source, STR_CPYDEST, basepath);
		show_msg(msg);

		copy_in_progress = true; copied_count = 1;
		file_copy(source, target, COPY_WHOLE_FILE);
		copy_in_progress = false;

		if(copy_aborted)
		{
			cellFsUnlink(target);
			show_msg((char*)STR_CPYABORT);
		}
	}
}
 #endif
#endif

static void game_mount(char *buffer, char *templn, char *param, char *tempstr, u8 is_binary, bool mount_ps3, bool forced_mount)
{
	//unmount game
	if(strstr(param, "ps3/unmount"))
	{
		do_umount(true);

		strcat(buffer, STR_GAMEUM);
	}
#ifdef PS2_DISC
	else if(strstr(param, "ps2/unmount"))
	{
		do_umount_ps2disc(false);

		strcat(buffer, STR_GAMEUM);
	}
#endif
	else
	{
		if(strstr(param, "?random="))
			param[strrchr(param, '?')-param] = NULL;

		int plen = 10;
#ifdef COPY_PS3
		char target[MAX_PATH_LEN] = "", *pos;
		if(islike(param, "/copy.ps3")) {plen = IS_COPY; pos = strstr(param, "&to="); if(pos) {strcpy(target, pos + 4); pos[0] = NULL;}}
#endif
		char enc_dir_name[1024], *source = param + plen;
		bool mounted = false; max_mapped = 0;

		is_binary = 1;

		// mount url
		urlenc(templn, source);

		if(!((plen == IS_COPY) && !copy_in_progress))
		{
			char *p = strstr(param, "/PS3_"); if(p) p[0] = NULL;

#ifdef PS2_DISC
			if(islike(param, "/mount.ps2"))
			{
				mounted = mount_ps2disc(source);
			}
			else
			if(islike(param, "/mount_ps2"))
			{
				do_umount(true);
				mounted = mount_ps2disc(source);
			}
			else
#endif
			if(!forced_mount && get_game_info())
			{
				sprintf(tempstr, "<H3>%s : <a href=\"/mount.ps3/unmount\">%s %s</a></H3><hr><a href=\"/mount_ps3%s\">", STR_UNMOUNTGAME, _game_TitleID, _game_Title, templn); strcat(buffer, tempstr);
			}
			else
				mounted = mount_with_mm(source, 1);
		}

		// auto-play
		if(mount_ps3)
		{
			if(mounted && IS_ON_XMB && strstr(param, "/PSPISO") == NULL && extcmp(param, ".BIN.ENC", 8)!=0)
			{
				CellPadData pad_data = pad_read();
				bool atag = (strcasestr(param, AUTOPLAY_TAG)!=NULL) || (webman_config->autoplay);
#ifdef REMOVE_SYSCALLS
				bool l2 = (pad_data.len>0 && (pad_data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_L2));
#else
				bool l2 = (pad_data.len>0 && (pad_data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & (CELL_PAD_CTRL_L2 | CELL_PAD_CTRL_R2)));
#endif

#ifdef FAKEISO
				if(!l2 && !extcmp(param, ".ntfs[BDFILE]", 13))
				{
					int view = View_Find("explore_plugin");

					if(!(webman_config->nogrp) && webman_config->rxvid && view != 0)
					{
						explore_interface = (explore_plugin_interface *)plugin_GetInterface(view,1);
						if(!extcmp(param, ".pkg", 4))
						{
							explore_interface->DoUnk6("close_all_list",0,0);
							explore_interface->DoUnk6("focus_segment_index seg_package_files",0,0);
						}
						else
						{
							explore_interface->DoUnk6("focus_index rx_video",0,0);
							sys_timer_usleep(200000);
							explore_interface->DoUnk6("exec_push",0,0);
							sys_timer_usleep(200000);
							explore_interface->DoUnk6("focus_index 0",0,0);

							if(!webman_config->autoplay || strcasestr(param, ".mkv")) {is_busy=false; return;}

							sys_timer_sleep(2);
							explore_interface->DoUnk6("exec_push",0,0);
						}

						if(webman_config->autoplay)
						{
							sys_timer_sleep(2);
							explore_interface->DoUnk6("exec_push",0,0);
						}
					}
				}
				else
#endif
				{
					char category[16] = "game", seg_name[80] = "seg_device";
					if((atag && !l2) || (!atag && l2)) {sys_timer_sleep(1); launch_disc(category, seg_name);} // L2 + X
				}
			}

			is_busy=false;
			return;
		}
		else
		{
			char _path[MAX_PATH_LEN];

			// mount label
			if(islike(templn, "/net"))
			{
				utf8enc(_path, source, 0);
			}
			else
			{
				sprintf(_path, "%s", source);
			}

			char *filename = strrchr(_path, '/'), *icon = tempstr;
			sprintf(icon, "%s/PS3_GAME/ICON0.PNG", source);

			if(file_exists(icon) == false)
			{
				char fpath[MAX_PATH_LEN], tempID[10]; icon[0] = tempID[0] = NULL;

				// get iso name
				strcpy(fpath, _path); fpath[strlen(fpath)-strlen(filename)] = NULL;

				get_default_icon(icon, fpath, filename + 1, 0, tempID, -1, 0);
			}

			urlenc(enc_dir_name, icon);
			htmlenc(_path, source, 0);

#ifdef COPY_PS3
			if(plen == IS_COPY)
			{
				bool is_copying_from_hdd = islike(source, "/dev_hdd0");

				if(cp_mode)
				{
					sprintf(target, "%s", cp_path);
				}
				else
				if(target[0]) {if(!isDir(source) && isDir(target)) strcat(target, filename);}
				else
#ifdef SWAP_KERNEL
				if(strstr(source, "/lv2_kernel"))
				{
					struct CellFsStat buf;
					if(cellFsStat(source, &buf) != CELL_FS_SUCCEEDED)
						sprintf(target, "%s", STR_ERROR);
					else
					{
						uint64_t size = buf.st_size;

						enable_dev_blind(source);

						// for cobra req: /dev_flash/sys/stage2.bin & /dev_flash/sys/lv2_self
						sprintf(target, SYS_COBRA_PATH "stage2.bin");
						if(isDir("/dev_flash/rebug/cobra"))
						{
							if(!extcmp(source, ".dex", 4))
								sprintf(target, "%s/stage2.dex", "/dev_flash/rebug/cobra");
							else if(!extcmp(source, ".cex", 4))
								sprintf(target, "%s/stage2.cex", "/dev_flash/rebug/cobra");
						}

						if(file_exists(target) == false)
						{
							sprintf(tempstr, "%s", source);
							strcpy(strrchr(tempstr, '/'), "/stage2.bin");
							if(file_exists(tempstr)) file_copy(tempstr, target, COPY_WHOLE_FILE);
						}

						// copy: /dev_flash/sys/lv2_self
						sprintf(target, "/dev_blind/sys/lv2_self");
						if((cellFsStat(target, &buf) != CELL_FS_SUCCEEDED) || (buf.st_size != size))
							file_copy(source, target, COPY_WHOLE_FILE);

						if((cellFsStat(target, &buf) == CELL_FS_SUCCEEDED) && (buf.st_size == size))
						{
							uint64_t lv2_offset = 0x15DE78; // 4.xx CFW LV1 memory location for: /flh/os/lv2_kernel.self
							if(peek_lv1(lv2_offset) != 0x2F666C682F6F732FULL)
								for(uint64_t addr = 0x100000ULL; addr<0xFFFFF8ULL; addr+=4) // Find in 16MB
									if(peek_lv1(addr) == 0x2F6F732F6C76325FULL)           // /os/lv2_
									{
										lv2_offset=addr-4; break; // 0x12A2C0 on 3.55
									}

							if(peek_lv1(lv2_offset) == 0x2F666C682F6F732FULL)  // Original: /flh/os/lv2_kernel.self
							{
								poke_lv1(lv2_offset + 0x00, 0x2F6C6F63616C5F73ULL); // replace:	/flh/os/lv2_kernel.self -> /local_sys0/sys/lv2_self
								poke_lv1(lv2_offset + 0x08, 0x7973302F7379732FULL);
								poke_lv1(lv2_offset + 0x10, 0x6C76325F73656C66ULL);

								working = 0;
								{ DELETE_TURNOFF }
								savefile((char*)WMNOSCAN, NULL, 0);
								{system_call_3(SC_SYS_POWER, SYS_REBOOT, NULL, 0);} /*load LPAR id 1*/
								sys_ppu_thread_exit(0);
							}
						}
					}
					plen = 0; //do not copy
				}
				else
#endif // #ifdef SWAP_KERNEL
				if(!extcmp(source, ".pkg", 4))
				{
					if(is_copying_from_hdd)
						sprintf(target, "/dev_usb000/Packages");
					else
						sprintf(target, "/dev_hdd0/packages");

					strcat(target, filename);
				}
				else if(!extcmp(source, ".bmp", 4))
				{
					if(is_copying_from_hdd)
						sprintf(target, "%s/PICTURE", "/dev_usb000");
					else
						sprintf(target, "%s/PICTURE", "/dev_hdd0");

					strcat(target, filename);
				}
				else if(strcasestr(source, ".JPG") || strcasestr(source, ".PNG"))
				{
					if(is_copying_from_hdd)
						sprintf(target, "%s/PICTURE", "/dev_usb000");
					else if(strstr(source, "BL") || strstr(param, "BC") || strstr(source, "NP"))
						sprintf(target, "/dev_hdd0/GAMES/covers");
					else
						sprintf(target, "%s/PICTURE", "/dev_hdd0");

					strcat(target, filename);
				}
				else if(strcasestr(source, "/covers"))
				{
					if(is_copying_from_hdd)
						sprintf(target, "/dev_usb000/COVERS");
					else
						sprintf(target, "/dev_hdd0/GAMES/covers");
				}
				else if(strcasestr(source, ".mp4") || strcasestr(source, ".mkv") || strcasestr(source, ".avi"))
				{
					if(is_copying_from_hdd)
						sprintf(target, "/dev_usb000/VIDEO");
					else
						sprintf(target, "/dev_hdd0/VIDEO");

					strcat(target, filename);
				}
				else if(strcasestr(source, ".mp3"))
				{
					if(is_copying_from_hdd)
						sprintf(target, "%s/MUSIC", "/dev_usb000");
					else
						sprintf(target, "%s/MUSIC", "/dev_hdd0");

					strcat(target, filename);
				}
				else if(!extcmp(source, ".p3t", 4))
				{
					if(is_copying_from_hdd)
						sprintf(target, "/dev_usb000/PS3/THEME");
					else
						sprintf(target, "/dev_hdd0/theme");

					strcat(target, filename);
				}
				else if(!extcmp(source, ".edat", 5))
				{
					if(is_copying_from_hdd)
						sprintf(target, "/dev_usb000/exdata");
					else
						sprintf(target, "%s/%s/exdata", "/dev_hdd0/home", webman_config->uaccount);

					strcat(target, filename);
				}
				else if(!extcmp(source, ".rco", 4) || strstr(param, "/coldboot"))
				{
					enable_dev_blind(NO_MSG);
					sprintf(target, "/dev_blind/vsh/resource");

					if(!extcmp(param, ".raf", 4))
						strcat(target, "/coldboot.raf");
					else
						strcat(target, filename);
				}
				else if(!extcmp(source, ".qrc", 4))
				{
					enable_dev_blind(NO_MSG);
					sprintf(target, "%s/qgl", "/dev_blind/vsh/resource");

					if(strstr(param, "/lines"))
						strcat(target, "/lines.qrc");
					else
						strcat(target, filename);
				}
				else if(strstr(source, "/exdata"))
				{
					if(is_copying_from_hdd)
						sprintf(target, "/dev_usb000/exdata");
					else
						sprintf(target, "%s/%s/exdata", "/dev_hdd0/home", webman_config->uaccount);
				}
				else if(strstr(source, "/PS3/THEME"))
					sprintf(target, "/dev_hdd0/theme");
				else if(strcasestr(source, "/savedata/"))
				{
					if(is_copying_from_hdd)
						sprintf(target, "/dev_usb000/PS3/SAVEDATA");
					else
						sprintf(target, "%s/%s/savedata", "/dev_hdd0/home", webman_config->uaccount);

					strcat(target, filename);
				}
				else if(strcasestr(source, "/trophy/"))
				{
					if(is_copying_from_hdd)
						sprintf(target, "/dev_usb000/PS3/TROPHY");
					else
						sprintf(target, "%s/%s/trophy", "/dev_hdd0/home", webman_config->uaccount);

					strcat(target, filename);
				}
				else if(strstr(source, "/webftp_server"))
				{
					sprintf(target, "%s/webftp_server.sprx", "/dev_hdd0/plugins");
					if(file_exists(target) == false) sprintf(target, "%s/webftp_server_ps3mapi.sprx", "/dev_hdd0/plugins");
					if(file_exists(target) == false) sprintf(target, "%s/webftp_server.sprx", "/dev_hdd0");
					if(file_exists(target) == false) sprintf(target, "%s/webftp_server_ps3mapi.sprx", "/dev_hdd0");
				}
				else if(strstr(source, "/boot_plugins_"))
					sprintf(target, "/dev_hdd0/boot_plugins.txt");
				else if(islike(source, "/dev_usb"))
					sprintf(target, "/dev_hdd0%s", source+11);
				else if(islike(source, "/net"))
					sprintf(target, "/dev_hdd0%s", source+5);
				else if(is_copying_from_hdd)
					sprintf(target, "/dev_usb000%s", source+9);
				else
				{
					if(islike(source, "/dev_bdvd"))
					{
						{system_call_1(36, (uint64_t) "/dev_bdvd");} // decrypt dev_bdvd files

						sprintf(target, "%s/%s", "/dev_hdd0/GAMES", "My Disc Backup");

						char titleid[10], title[64]; memset(titleid, 0, 10);
						sprintf(title, "/dev_bdvd/PS3_GAME/PARAM.SFO");
						if(file_exists(title))
						{
							getTitleID(title, titleid, GET_TITLE_AND_ID);
							if(titleid[0] && titleid[8]>='0')
							{
								if(strstr(title, " ["))
									sprintf(target, "%s/%s", "/dev_hdd0/GAMES", title);
								else
									sprintf(target, "%s/%s [%s]", "/dev_hdd0/GAMES", title, titleid);
							}
						}
					}
					else
						sprintf(target, "/dev_hdd0");

					char *p = strstr(source + 9, "/");
					if(p) strcat(target, p);
				}

				bool is_error = ((islike(target, "/dev_usb000") && file_exists("/dev_usb000") == false)) || islike(target, source);

				// show source path
				sprintf(tempstr, "%s ", STR_COPYING); strcat(buffer, tempstr);
				add_breadcrumb_trail(buffer, source); strcat(buffer, "<hr>");

				// show image
				urlenc(_path, target);
				sprintf(tempstr, "<a href=\"%s\"><img src=\"%s\" border=0></a><hr>%s %s: ",
								 _path, enc_dir_name, is_error ? STR_ERROR : "", STR_CPYDEST); strcat(buffer, tempstr);

				// show target path
				add_breadcrumb_trail(buffer, target); tempstr[0] = NULL;

				if(strstr(target, "/webftp_server")) {sprintf(tempstr, "<HR>%s", STR_SETTINGSUPD);} else
				if(cp_mode) {char *p = strrchr(_path, '/'); p[0] = NULL; sprintf(tempstr, HTML_REDIRECT_TO_URL, _path, HTML_REDIRECT_WAIT);}

				if(is_error) {show_msg((char*)STR_CPYABORT); cp_mode = 0; return;}
			}
			else
#endif // #ifdef COPY_PS3

			{
				bool is_movie = strstr(param, "/BDISO") || strstr(param, "/DVDISO") || !extcmp(param, ".ntfs[BDISO]", 12) || !extcmp(param, ".ntfs[DVDISO]", 13);
				strcat(buffer, is_movie ? STR_MOVIETOM : STR_GAMETOM); strcat(buffer, ": "); add_breadcrumb_trail(buffer, source);

				if(is_movie)
					sprintf(tempstr, "<hr><a href=\"/play.ps3\"><img src=\"%s\" onerror=\"this.src='%s';\" border=0></a>"
									 "<hr><a href=\"/dev_bdvd\">%s</a>", enc_dir_name, wm_icons[strstr(param,"BDISO") ? 5 : 9], mounted ? STR_MOVIELOADED : STR_ERROR);
				else if(!extcmp(param, ".BIN.ENC", 8))
					sprintf(tempstr, "<hr><img src=\"%s\" onerror=\"this.src='%s';\" height=%i>"
									 "<hr>%s", enc_dir_name, wm_icons[7], 300, mounted ? STR_PS2LOADED : STR_ERROR);
				else if((strstr(param, "/PSPISO") || strstr(param, "/ISO/")) && !extcasecmp(param, ".iso", 4))
					sprintf(tempstr, "<hr><img src=\"%s\" onerror=\"this.src='%s';\" height=%i>"
									 "<hr>%s", enc_dir_name, wm_icons[8], strcasestr(enc_dir_name,".png") ? 200 : 300, mounted ? STR_PSPLOADED : STR_ERROR);
				else
					sprintf(tempstr, "<hr><a href=\"/play.ps3\"><img src=\"%s\" onerror=\"this.src='%s';\" border=0></a>"
									 "<hr><a href=\"/dev_bdvd\">%s</a>", enc_dir_name, wm_icons[5], mounted ? STR_GAMELOADED : STR_ERROR);
			}

			strcat(buffer, tempstr);

#ifdef PS2_DISC
			if(mounted && (strstr(source, "/GAME") || strstr(source, "/PS3ISO") || strstr(source, ".ntfs[PS3ISO]")))
			{
				CellFsDirent entry; uint64_t read_e; int fd2; u16 pcount=0; u32 tlen=strlen(buffer)+8;

				sprintf(target, "%s", source); u8 is_iso = 0;
				if(strstr(target, "Sing"))
				{
					if(strstr(target, "/PS3ISO")) {strcpy(strstr(target, "/PS3ISO"), "/PS2DISC\0"); is_iso = 1;}
					if(strstr(target, ".ntfs[PS3ISO]")) {strcpy(target, "/dev_hdd0/PS2DISC\0"); is_iso = 1;}
				}

				// check for [PS2] extracted folders
				if(cellFsOpendir(target, &fd2) == CELL_FS_SUCCEEDED)
				{
					while((cellFsReaddir(fd2, &entry, &read_e) == CELL_FS_SUCCEEDED) && (read_e > 0))
					{
						if((entry.d_name[0] == '.')) continue;

						if(is_iso || strstr(entry.d_name, "[PS2") != NULL)
						{
							if(pcount == 0) strcat(buffer, "<br><HR>");
							urlenc(enc_dir_name, entry.d_name);
							sprintf(templn, "<a href=\"/mount.ps2%s/%s\">%s</a><br>", target, enc_dir_name, entry.d_name);

							tlen += strlen(tempstr);
							if(tlen > (BUFFER_SIZE - _2KB_)) break;
							strcat(buffer, templn); pcount++;
						}
					}
				}
			}
#endif // #ifdef PS2_DISC
		}
#ifdef COPY_PS3
		if((plen == IS_COPY) && !copy_in_progress)
		{
			if(islike(target, source) || ((!islike(source, "/net")) && file_exists(source) == false) )
				sprintf(templn, "%s", STR_ERROR);
			else
			{
				// show msg begin
				sprintf(templn, "%s %s\n%s %s", STR_COPYING, source, STR_CPYDEST, target);
				show_msg((char*)templn);
				copy_in_progress = true; copied_count = 0;

				// make target dir tree
				for(u16 p=12; p<strlen(target); p++)
					if(target[p]=='/') {target[p] = NULL; cellFsMkdir((char*)target, MODE); target[p]='/';}

				// copy folder to target
				if(strstr(source,"/exdata"))
					import_edats(source, target);
				else if(isDir(source))
					folder_copy(source, target);
				else
					file_copy(source, target, COPY_WHOLE_FILE);

				copy_in_progress = false;

				// show msg end
				if(copy_aborted)
					show_msg((char*)STR_CPYABORT);
				else
					show_msg((char*)STR_CPYFINISH);
			}

			if(!copy_aborted && (cp_mode == 2) && file_exists(target)) del(source, true);
			if(cp_mode) {cp_mode = 0; memset(cp_path, 0, MAX_PATH_LEN);}
		}
#endif //#ifdef COPY_PS3
	}
}

#ifdef COBRA_ONLY
static void do_umount_iso(void)
{
	unsigned int real_disctype, effective_disctype, iso_disctype;

	cobra_get_disc_type(&real_disctype, &effective_disctype, &iso_disctype);

	// If there is an effective disc in the system, it must be ejected
	if(effective_disctype != DISC_TYPE_NONE)
	{
		cobra_send_fake_disc_eject_event();
		for(u8 m = 0; m < 250; m++)
		{
			sys_timer_usleep(4000);

			if(!isDir("/dev_bdvd")) break;
		}
	}

	if(iso_disctype != DISC_TYPE_NONE) cobra_umount_disc_image();

	// If there is a real disc in the system, issue an insert event
	if(real_disctype != DISC_TYPE_NONE)
	{
		cobra_send_fake_disc_insert_event();
		for(u8 m = 0; m < 250; m++)
		{
			sys_timer_usleep(4000);

			if(isDir("/dev_bdvd")) break;
		}
		cobra_disc_auth();
	}
}
#endif

static void do_umount(bool clean)
{
	if(clean) cellFsUnlink(WMTMP "/last_game.txt");

	if(fan_ps2_mode) reset_fan_mode();

#ifdef COBRA_ONLY
	//if(cobra_mode)
	{
		{ PS3MAPI_ENABLE_ACCESS_SYSCALL8 }

		do_umount_iso();
 #ifdef PS2_DISC
		do_umount_ps2disc(false);
 #endif
		sys_timer_usleep(20000);

		cobra_unload_vsh_plugin(0); // unload rawseciso / netiso plugins
		cobra_unset_psp_umd();

		{sys_map_path((char*)"/dev_bdvd", NULL);}
		{sys_map_path((char*)"/app_home", isDir("/dev_hdd0/packages") ? (char*)"/dev_hdd0/packages" : NULL);}

		{sys_map_path((char*)"//dev_bdvd", NULL);}
		//{sys_map_path((char*)"//app_home", NULL);}

		{
			sys_ppu_thread_t t;
			uint64_t exit_code;
 #ifndef LITE_EDITION
			sys_ppu_thread_create(&t, netiso_stop_thread, NULL, THREAD_PRIO_STOP, THREAD_STACK_SIZE_8KB, SYS_PPU_THREAD_CREATE_JOINABLE, STOP_THREAD_NAME);
			sys_ppu_thread_join(t, &exit_code);
 #endif
			sys_ppu_thread_create(&t, rawseciso_stop_thread, NULL, THREAD_PRIO_STOP, THREAD_STACK_SIZE_8KB, SYS_PPU_THREAD_CREATE_JOINABLE, STOP_THREAD_NAME);
			sys_ppu_thread_join(t, &exit_code);
		}

 #ifndef LITE_EDITION
		while(netiso_loaded || rawseciso_loaded) {sys_timer_usleep(100000);}
 #else
		while(rawseciso_loaded) {sys_timer_usleep(100000);}
 #endif

		{ PS3MAPI_DISABLE_ACCESS_SYSCALL8 }
	}
#else
	{
		pokeq(0x8000000000000000ULL+MAP_ADDR, 0x0000000000000000ULL);
		pokeq(0x8000000000000008ULL+MAP_ADDR, 0x0000000000000000ULL);
		//eject_insert(1, 1);

		if(isDir("/dev_flash/pkg"))
			mount_with_mm((char*)"/dev_flash/pkg", 0);
	}
#endif //#ifdef COBRA_ONLY
}


#ifdef COBRA_ONLY
 #ifndef LITE_EDITION
static u32 detect_cd_sector_size(int fd)
{
	char buffer[0x10]; buffer[0xD] = NULL;

	cellFsLseek(fd, 0x9320, CELL_FS_SEEK_SET, NULL); cellFsRead(fd, (void *)buffer, 0xC, NULL); if(islike(buffer, PLAYSTATION)) return 2352; else {
	cellFsLseek(fd, 0x8020, CELL_FS_SEEK_SET, NULL); cellFsRead(fd, (void *)buffer, 0xC, NULL); if(islike(buffer, PLAYSTATION)) return 2048; else {
	cellFsLseek(fd, 0x9220, CELL_FS_SEEK_SET, NULL); cellFsRead(fd, (void *)buffer, 0xC, NULL); if(islike(buffer, PLAYSTATION)) return 2336; else {
	cellFsLseek(fd, 0x9920, CELL_FS_SEEK_SET, NULL); cellFsRead(fd, (void *)buffer, 0xC, NULL); if(islike(buffer, PLAYSTATION)) return 2448; }}}

	return 2352;
}
 #endif
#endif

static void mount_autoboot(void)
{
	char path[MAX_PATH_LEN];

	// get autoboot path
	if(webman_config->autob &&
		((cobra_mode && islike(webman_config->autoboot_path, "/net")) || islike(webman_config->autoboot_path, "http") || file_exists(webman_config->autoboot_path))) // autoboot
		strcpy(path, (char *) webman_config->autoboot_path);
	else
	{	// get last game path
		uint64_t read_e = 0;

		if(webman_config->lastp && file_exists(WMTMP "/last_game.txt"))
		{
			int fd = 0;
			if(cellFsOpen(WMTMP "/last_game.txt", CELL_FS_O_RDONLY, &fd, NULL, 0) == CELL_FS_SUCCEEDED)
			{
				if(cellFsRead(fd, (void *)path, MAX_PATH_LEN, &read_e) != CELL_FS_SUCCEEDED) read_e = 0;
				cellFsClose(fd);
			}
		}

		path[read_e] = NULL;
	}

	bool do_mount = false;

	if(from_reboot && !path[0] && strstr(path, "/PS2")) return; //avoid re-launch PS2 returning to XMB

	// wait few seconds until path becomes ready
	if((strlen(path) > 8) || (cobra_mode && islike(path, "/net")))
	{
		waitfor(path, 2 * (webman_config->boots + webman_config->bootd));
		do_mount = ((cobra_mode && islike(path, "/net")) || islike(path, "http") || file_exists(path));
	}

	if(do_mount)
	{   // add some delay
		if(webman_config->delay)      {sys_timer_sleep(10); waitfor(path, 2*(webman_config->boots+webman_config->bootd));}
		else if(islike(path, "/net"))  sys_timer_sleep(10);
#ifndef COBRA_ONLY
		if(strstr(path, ".ntfs[") == NULL)
#endif
		mount_with_mm(path, 1); // mount path
	}
}

static bool mount_with_mm(const char *_path0, u8 do_eject)
{
	if(is_mounting) return false;

	// show message if syscalls are fully disabled
#ifdef COBRA_ONLY
	if(syscalls_removed || peekq(TOC) == SYSCALLS_UNAVAILABLE)
	{
		syscalls_removed = true;
		{ PS3MAPI_ENABLE_ACCESS_SYSCALL8 }

		int ret_val = -1; { system_call_2(SC_COBRA_SYSCALL8, SYSCALL8_OPCODE_PS3MAPI, PS3MAPI_OPCODE_PCHECK_SYSCALL8); ret_val = (int)p1;}
		if(ret_val < 0) { show_msg((char*)STR_CFWSYSALRD); { PS3MAPI_DISABLE_ACCESS_SYSCALL8 } return false; }
		if(ret_val > 1) { system_call_3(SC_COBRA_SYSCALL8, SYSCALL8_OPCODE_PS3MAPI, PS3MAPI_OPCODE_PDISABLE_SYSCALL8, 1); }
	}
#else
	if(syscalls_removed || peekq(TOC) == SYSCALLS_UNAVAILABLE) { show_msg(STR_CFWSYSALRD); return false; }
#endif

	bool ret = true;

	u8 mount_unk = EMU_OFF;

	if(fan_ps2_mode) reset_fan_mode();

	is_mounting = true;

#ifndef COBRA_ONLY
	uint64_t sc_600=0;
	uint64_t sc_604=0;
	uint64_t sc_142=0;
#endif

	if(!dex_mode)
	{ // CEX
		if(c_firmware==3.55f)
		{
			pokeq(0x80000000002909B0ULL, 0x4E80002038600000ULL );
			pokeq(0x80000000002909B8ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x8000000000055EA0ULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x8000000000055F64ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x8000000000055F10ULL, 0x419E00D860000000ULL );
			pokeq(0x8000000000055F18ULL, 0x2F84000448000098ULL );
			pokeq(0x800000000007AF64ULL, 0x2F83000060000000ULL );
			pokeq(0x800000000007AF78ULL, 0x2F83000060000000ULL );

#ifndef COBRA_ONLY
			sc_600 = 0x325B50;
			sc_604 = 0x325C58;
			sc_142 = 0x2E8FF8;
#endif
		}
		else
		if(c_firmware == 4.21f)
		{
			pokeq(0x8000000000296264ULL, 0x4E80002038600000ULL );
			pokeq(0x800000000029626CULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x8000000000057020ULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x80000000000570E4ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x8000000000057090ULL, 0x419E00D860000000ULL );
			pokeq(0x8000000000057098ULL, 0x2F84000448000098ULL );
			pokeq(0x800000000005AA54ULL, 0x2F83000060000000ULL ); // fix 80010009 error
			pokeq(0x800000000005AA68ULL, 0x2F83000060000000ULL ); // fix 80010019 error

#ifndef COBRA_ONLY
			sc_600 = 0x33B2E0;
			sc_604 = 0x33B448;
			sc_142 = 0x2FD810;
#endif
		}
		else
		if(c_firmware == 4.30f)
		{
			pokeq(0x80000000002979D8ULL, 0x4E80002038600000ULL );
			pokeq(0x80000000002979E0ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x8000000000057170ULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x8000000000057234ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error
			pokeq(0x8000000000057238ULL, 0x3BE000004BFFFF0CULL ); // introduced by me bug

			pokeq(0x80000000000571E0ULL, 0x419E00D860000000ULL );
			pokeq(0x80000000000571E8ULL, 0x2F84000448000098ULL );
			pokeq(0x800000000005ABA4ULL, 0x2F83000060000000ULL );
			pokeq(0x800000000005ABB8ULL, 0x2F83000060000000ULL );

#ifndef COBRA_ONLY
			sc_600 = 0x33D158; //35EEA0
			sc_604 = 0x33D2C0; //35EEC0
			sc_142 = 0x2FF460; //35E050
#endif
		}
		else
		if(c_firmware == 4.31f)
		{
			pokeq(0x80000000002979E0ULL, 0x4E80002038600000ULL );
			pokeq(0x80000000002979E8ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x8000000000057174ULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x800000000005723CULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x80000000000571E8ULL, 0x600000002F840004ULL );
			pokeq(0x80000000000571F0ULL, 0x48000098E8629870ULL );
			pokeq(0x800000000005ABACULL, 0x60000000E8610188ULL );
			pokeq(0x800000000005ABA0ULL, 0x600000005463063EULL );

#ifndef COBRA_ONLY
			sc_600 = 0x33D168;
			sc_604 = 0x33D2D0;
			sc_142 = 0x2FF470;
#endif
		}
		else
		if(c_firmware == 4.40f)
		{
			pokeq(0x8000000000296DE8ULL, 0x4E80002038600000ULL );
			pokeq(0x8000000000296DF0ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x80000000000560BCULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x8000000000056180ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error +C4

			pokeq(0x800000000005612CULL, 0x419E00D860000000ULL );
			pokeq(0x8000000000056134ULL, 0x2F84000448000098ULL );
			pokeq(0x8000000000059AF0ULL, 0x2F83000060000000ULL );
			pokeq(0x8000000000059B04ULL, 0x2F83000060000000ULL );

#ifndef COBRA_ONLY
			sc_600 = 0x33D720;
			sc_604 = 0x33D888;
			sc_142 = 0x2FF9E0;
#endif
		}
		else
		if(c_firmware == 4.41f)
		{
			pokeq(0x8000000000296DF0ULL, 0x4E80002038600000ULL ); // fix 8001003C error
			pokeq(0x8000000000296DF8ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x80000000000560C0ULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x8000000000056184ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x8000000000056130ULL, 0x419E00D860000000ULL );
			pokeq(0x8000000000056138ULL, 0x2F84000448000098ULL );
			pokeq(0x8000000000059AF4ULL, 0x2F83000060000000ULL );
			pokeq(0x8000000000059B08ULL, 0x2F83000060000000ULL );

#ifndef COBRA_ONLY
			sc_600 = 0x33D730;
			sc_604 = 0x33D898;
			sc_142 = 0x2FF9F0;
#endif
		}
		else
		if(c_firmware == 4.46f)
		{
			pokeq(0x8000000000297310ULL, 0x4E80002038600000ULL ); // fix 8001003C error
			pokeq(0x8000000000297318ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x80000000000560C0ULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x8000000000056184ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x8000000000056130ULL, 0x419E00D860000000ULL );
			pokeq(0x8000000000056138ULL, 0x2F84000448000098ULL );
			pokeq(0x8000000000059AF4ULL, 0x2F83000060000000ULL );
			pokeq(0x8000000000059B08ULL, 0x2F83000060000000ULL );

#ifndef COBRA_ONLY
			sc_600 = 0x33DD40;
			sc_604 = 0x33DEA8;
			sc_142 = 0x2FFF58;
#endif
		}
		else
		if(c_firmware == 4.50f)
		{
			pokeq(0x800000000026F61CULL, 0x4E80002038600000ULL ); // fix 8001003C error
			pokeq(0x800000000026F624ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x80000000000560BCULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x8000000000056180ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x800000000005612CULL, 0x419E00D860000000ULL );
			pokeq(0x8000000000056134ULL, 0x2F84000448000098ULL );
			pokeq(0x8000000000059AF0ULL, 0x2F83000060000000ULL );
			pokeq(0x8000000000059B04ULL, 0x2F83000060000000ULL );

#ifndef COBRA_ONLY
			sc_600 = 0x33C180;
			sc_604 = 0x33C2E8;
			sc_142 = 0x302100;
#endif
		}
		else
		if(c_firmware == 4.53f)
		{
			pokeq(0x800000000026F7F0ULL, 0x4E80002038600000ULL ); // fix 8001003C error
			pokeq(0x800000000026F7F8ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x80000000000560C0ULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x8000000000056184ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x8000000000056130ULL, 0x419E00D860000000ULL );
			pokeq(0x8000000000056138ULL, 0x2F84000448000098ULL );
			pokeq(0x8000000000059AF4ULL, 0x2F83000060000000ULL );
			pokeq(0x8000000000059B08ULL, 0x2F83000060000000ULL );

#ifndef COBRA_ONLY
			sc_600 = 0x33C308;
			sc_604 = 0x33C470;
			sc_142 = 0x302108;
#endif
		}
		else
		if(c_firmware == 4.55f)
		{
			pokeq(0x800000000027103CULL, 0x4E80002038600000ULL ); // fix 8001003C error
			pokeq(0x8000000000271044ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x8000000000056380ULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x8000000000056444ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x80000000000563F0ULL, 0x419E00D860000000ULL );
			pokeq(0x80000000000563F8ULL, 0x2F84000448000098ULL );
			pokeq(0x800000000005A2ECULL, 0x2F83000060000000ULL );
			pokeq(0x800000000005A300ULL, 0x2F83000060000000ULL );

#ifndef COBRA_ONLY
			sc_600 = 0x33F5C8;
			sc_604 = 0x33F730;
			sc_142 = 0x3051D0;
#endif
		}
		else
		if(c_firmware == 4.60f)
		{
			pokeq(0x80000000002925D8ULL, 0x4E80002038600000ULL ); // fix 8001003C error
			pokeq(0x80000000002925E0ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x8000000000056588ULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x800000000005664CULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x80000000000565F8ULL, 0x419E00D860000000ULL );
			pokeq(0x8000000000056600ULL, 0x2F84000448000098ULL );
			pokeq(0x800000000005A654ULL, 0x2F83000060000000ULL );
			pokeq(0x800000000005A668ULL, 0x2F83000060000000ULL );

			pokeq(0x80000000002A1054ULL, 0x386000014E800020ULL); // fix 0x80010017 error   Original: 0xFBC1FFF0EBC225B0ULL
			pokeq(0x8000000000055C58ULL, 0x386000004E800020ULL); // fix 0x8001002B error   Original: 0xF821FE917C0802A6ULL

			//lv2poke32(0x8000000000058DACULL, 0x60000000);      // fix 0x80010017 error (found by @smhabib)

			// Booting of game discs and backups speed increased
			//lv2poke32(0x8000000000058DA0ULL, 0x38600001);
			//lv2poke32(0x800000000005A96CULL, 0x38600000);

			// enable new habib patches
			pokeq(0x8000000000058DACULL + 0x00, 0x60000000E8610098ULL);
			pokeq(0x8000000000058DACULL + 0x08, 0x2FA30000419E000CULL);
			pokeq(0x8000000000058DACULL + 0x10, 0x388000334800BE15ULL);
			pokeq(0x8000000000058DACULL + 0x18, 0xE80100F07FE307B4ULL);

			pokeq(0x8000000000055C5CULL + 0x00, 0x386000004E800020ULL);
			pokeq(0x8000000000055C5CULL + 0x08, 0xFBC10160FBE10168ULL);
			pokeq(0x8000000000055C5CULL + 0x10, 0xFB610148FB810150ULL);
			pokeq(0x8000000000055C5CULL + 0x18, 0xFBA10158F8010180ULL);

#ifndef COBRA_ONLY
			sc_600 = 0x340630; //0x363A18 + 600*8 = 00364CD8 -> 80 00 00 00 00 34 06 30
			sc_604 = 0x340798; //0x363A18 + 604*8 = 00364CF8 -> 80 00 00 00 00 34 07 98
			sc_142 = 0x306478; //0x363A18 + 142*8 = 00363E88 -> 80 00 00 00 00 30 64 78
#endif
		}
		else
		if(c_firmware == 4.65f || c_firmware == 4.66f)
		{
			//patches by deank
			pokeq(0x800000000026FDDCULL, 0x4E80002038600000ULL ); // fix 8001003C error  Original: 0x4E8000208003026CULL
			pokeq(0x800000000026FDE4ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error  Original: 0x3D6000463D201B43ULL
			pokeq(0x800000000005658CULL, 0x63FF003D60000000ULL ); // fix 8001003D error  Original: 0x63FF003D419EFFD4ULL
			pokeq(0x8000000000056650ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error  Original: 0x3FE0800163FF003EULL

			pokeq(0x80000000000565FCULL, 0x419E00D860000000ULL ); // Original: 0x419E00D8419D00C0ULL
			pokeq(0x8000000000056604ULL, 0x2F84000448000098ULL ); // Original: 0x2F840004409C0048ULL //PATCH_JUMP
			pokeq(0x800000000005A658ULL, 0x2F83000060000000ULL ); // fix 80010009 error  Original: 0x2F830000419E00ACULL
			pokeq(0x800000000005A66CULL, 0x2F83000060000000ULL ); // fix 80010009 error  Original: 0x2F830000419E00ACULL

			pokeq(0x8000000000056230ULL, 0x386000012F830000ULL ); // ignore LIC.DAT check
			pokeq(0x80000000002302F0ULL, 0x38600000F8690000ULL ); // fix 0x8001002B / 80010017 errors (2015-01-03)

			pokeq(0x8000000000055C5CULL, 0xF821FE917C0802A6ULL ); // just restore the original
			pokeq(0x8000000000058DB0ULL, 0x419E0038E8610098ULL ); // just restore the original

		 /*
			//anti-ode patches by deank
			if(!is_rebug)
			{
				//pokeq(0x8000000000055C5CULL, 0xF821FE917C0802A6ULL ); //replaced by deank's patch (2015-01-03)
				pokeq(0x8000000000055C84ULL, 0x6000000060000000ULL );
				pokeq(0x8000000000055C8CULL, 0x600000003BA00000ULL );
			}

			//pokeq(0x80000000002A1060ULL, 0x386000014E800020ULL); // fix 0x80010017 error   Original: 0xFBC1FFF0EBC225B0ULL
			//pokeq(0x8000000000055C5CULL, 0x386000004E800020ULL); // fix 0x8001002B error   Original: 0xF821FE917C0802A6ULL

			// Booting of game discs and backups speed increased
			//lv2poke32(0x8000000000058DA4ULL, 0x38600001);
			//lv2poke32(0x800000000005A970ULL, 0x38600000);

			// habib patches (newest ones)
			pokeq(0x8000000000055C98ULL, 0x38600000EB610148ULL); //Original: 0x7FA307B4EB610148EB8101507C0803A6
			pokeq(0x8000000000058DCCULL, 0x38600000EBA100C8ULL); //Original: 0x7FE307B4EBA100C8EBC100D07C0803A6

			// enable new habib patches (now obsolete) //replaced by deank's patch (2015-01-03)
			pokeq(0x8000000000058DB0ULL + 0x00, 0x60000000E8610098ULL);
			pokeq(0x8000000000058DB0ULL + 0x08, 0x2FA30000419E000CULL);
			pokeq(0x8000000000058DB0ULL + 0x10, 0x388000334800BE15ULL);
			pokeq(0x8000000000058DB0ULL + 0x18, 0xE80100F07FE307B4ULL);

			pokeq(0x8000000000055C5CULL + 0x00, 0x386000004E800020ULL);
			pokeq(0x8000000000055C5CULL + 0x08, 0xFBC10160FBE10168ULL);
			pokeq(0x8000000000055C5CULL + 0x10, 0xFB610148FB810150ULL);
			pokeq(0x8000000000055C5CULL + 0x18, 0xFBA10158F8010180ULL);

			//patch to prevent blackscreen on usb games in jb format
			pokeq(0x8000000000055C84ULL, 0x386000002F830001ULL); //Original: 0x481DA6692F830001ULL
			pokeq(0x8000000000055C8CULL, 0x419E00303BA00000ULL); //Original: 0x419E00303BA00000ULL
		 */

#ifndef COBRA_ONLY
			sc_600 = 0x340640; //0x363A18 + 600*8 = 00364CD8 -> 80 00 00 00 00 34 06 40
			sc_604 = 0x3407A8; //0x363A18 + 604*8 = 00364CF8 -> 80 00 00 00 00 34 07 A8
			sc_142 = 0x306488; //0x363A18 + 142*8 = 00363E88 -> 80 00 00 00 00 30 64 88
#endif
		}
		else
		if(c_firmware == 4.70f)
		{
			//patches by deank
			pokeq(0x80000000002670D8ULL, 0x4E80002038600000ULL ); // fix 8001003C error  Original: 0x4E8000208003026CULL //0x80000000002898DCULL??
			pokeq(0x80000000002670E0ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error  Original: 0x3D6000463D201B43ULL //0x80000000002898E4ULL??
			pokeq(0x8000000000056588ULL, 0x63FF003D60000000ULL ); // fix 8001003D error  Original: 0x63FF003D419EFFD4ULL
			pokeq(0x800000000005664CULL, 0x3FE080013BE00000ULL ); // fix 8001003E error  Original: 0x3FE0800163FF003EULL

			pokeq(0x80000000000565F8ULL, 0x419E00D860000000ULL ); // Original: 0x419E00D8419D00C0ULL
			pokeq(0x8000000000056600ULL, 0x2F84000448000098ULL ); // Original: 0x2F840004409C0048ULL //PATCH_JUMP
			pokeq(0x800000000005A6DCULL, 0x2F83000060000000ULL ); // fix 80010009 error  Original: 0x2F830000419E00ACULL
			pokeq(0x800000000005A6F0ULL, 0x2F83000060000000ULL ); // fix 80010009 error  Original: 0x2F830000419E00ACULL

			pokeq(0x800000000005622CULL, 0x386000012F830000ULL ); // ignore LIC.DAT check
			pokeq(0x80000000002275ECULL, 0x38600000F8690000ULL ); // fix 0x8001002B / 80010017 errors (2015-01-03)

			pokeq(0x8000000000055C58ULL, 0xF821FE917C0802A6ULL ); // just restore the original
			pokeq(0x8000000000058E18ULL, 0x419E0038E8610098ULL ); // just restore the original

#ifndef COBRA_ONLY
			sc_600 = 0x33FE88;
			sc_604 = 0x33FFF0;
			sc_142 = 0x306618;
#endif
		}
		else
		if(c_firmware >= 4.75f && c_firmware <= 4.78f)
		{
			//patches by deank
			pokeq(0x800000000026714CULL, 0x4E80002038600000ULL ); // fix 8001003C error  Original: 0x4E8000208003026CULL
			pokeq(0x8000000000267154ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error  Original: 0x3D6000473D201B43ULL
			pokeq(0x800000000005658CULL, 0x63FF003D60000000ULL ); // fix 8001003D error  Original: 0x63FF003D419EFFD4ULL
			pokeq(0x8000000000056650ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error  Original: 0x3FE0800163FF003EULL

			pokeq(0x80000000000565FCULL, 0x419E00D860000000ULL ); // Original: 0x419E00D8419D00C0ULL
			pokeq(0x8000000000056604ULL, 0x2F84000448000098ULL ); // Original: 0x2F840004409C0048ULL //PATCH_JUMP
			pokeq(0x800000000005A6E0ULL, 0x2F83000060000000ULL ); // fix 80010009 error  Original: 0x2F830000419E00ACULL
			pokeq(0x800000000005A6F4ULL, 0x2F83000060000000ULL ); // fix 80010009 error  Original: 0x2F830000419E00ACULL

			pokeq(0x8000000000056230ULL, 0x386000012F830000ULL ); // ignore LIC.DAT check
			pokeq(0x80000000002275F4ULL, 0x38600000F8690000ULL ); // fix 0x8001002B / 80010017 errors (2015-01-03)

			pokeq(0x8000000000055C5CULL, 0xF821FE917C0802A6ULL ); // just restore the original
			pokeq(0x8000000000058E1CULL, 0x419E0038E8610098ULL ); // just restore the original

#ifndef COBRA_ONLY
			sc_600 = 0x33FF28;
			sc_604 = 0x340090;
			sc_142 = 0x3066B8;
#endif
		}
		else
		if(c_firmware == 4.80f)
		{
			//patches by deank
			pokeq(0x8000000000267144ULL, 0x4E80002038600000ULL ); // fix 8001003C error  Original: 0x4E8000208003026CULL
			pokeq(0x800000000026714CULL, 0x7C6307B44E800020ULL ); // fix 8001003C error  Original: 0x3D201B433C608001ULL
			pokeq(0x800000000005688CULL, 0x63FF003D60000000ULL ); // fix 8001003D error  Original: 0x63FF003D419EFFD4ULL
			pokeq(0x800000000005664CULL, 0x3FE080013BE00000ULL ); // fix 8001003E error  Original: 0x3FE0800163FF003EULL

			pokeq(0x80000000000565F8ULL, 0x419E00D860000000ULL ); // Original: 0x419E00D8419D00C0ULL
			pokeq(0x8000000000056600ULL, 0x2F84000448000098ULL ); // Original: 0x2F840004409C0048ULL //PATCH_JUMP
			pokeq(0x800000000005A6DCULL, 0x2F83000060000000ULL ); // fix 80010009 error  Original: 0x2F830000419E00ACULL
			pokeq(0x800000000005A6F0ULL, 0x2F83000060000000ULL ); // fix 80010009 error  Original: 0x2F830000419E00ACULL

			pokeq(0x800000000005622CULL, 0x386000012F830000ULL ); // ignore LIC.DAT check
			pokeq(0x80000000002275ECULL, 0x38600000F8690000ULL ); // fix 0x8001002B / 80010017 errors

			pokeq(0x8000000000055C58ULL, 0xF821FE917C0802A6ULL ); // just restore the original
			pokeq(0x8000000000058E18ULL, 0x419E0038E8610098ULL ); // just restore the original

#ifndef COBRA_ONLY
			sc_600 = 0x33FF18;
			sc_604 = 0x340080;
			sc_142 = 0x3066A8;
#endif
		}
	}

#ifdef DECR_SUPPORT
	else if(dex_mode == 1)
	{ // DEH
		if(c_firmware==3.55f)
		{
			pokeq(0x8000000000291794ULL, 0x4E80002038600000ULL ); // fix 8001003C error  Original: 0x4E8000208003026CULL
			pokeq(0x800000000029179CULL, 0x7C6307B44E800020ULL ); // fix 8001003C error  Original: 0x3D6000353D201B43ULL
			pokeq(0x8000000000059C58ULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x8000000000059D1CULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x8000000000059CC8ULL, 0x419E00D860000000ULL );
			pokeq(0x8000000000059CD0ULL, 0x2F84000448000098ULL );
			pokeq(0x800000000007F6D0ULL, 0x2F83000060000000ULL );
			pokeq(0x800000000007F6E4ULL, 0x2F83000060000000ULL );

#ifndef COBRA_ONLY
			sc_600 = 0x371138;
			sc_604 = 0x371228;
			sc_142 = 0x32FE88;
#endif
		}
		else
/*
		if(c_firmware == 4.50f)
		{
			pokeq(0x8000000000276A00ULL, 0x4E80002038600000ULL ); // fix 8001003C error
			pokeq(0x8000000000276A08ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x8000000000059E74ULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x8000000000059F38ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x8000000000059EE4ULL, 0x419E00D860000000ULL );
			pokeq(0x8000000000059EECULL, 0x2F84000448000098ULL );
			pokeq(0x800000000005D8A8ULL, 0x2F83000060000000ULL );
			pokeq(0x800000000005D8BCULL, 0x2F83000060000000ULL );

			pokeq(0x8000000000059B40ULL, 0x386000012F830000ULL ); // ignore LIC.DAT check
			pokeq(0x800000000023705CULL, 0x38600000F8690000ULL ); // fix 0x8001002B / 80010017 errors (ported for DEX 2015-01-03)		}

#ifndef COBRA_ONLY
			sc_600 = 0x360208;
			sc_604 = 0x388E10;
			sc_142 = 0x34CA38;
#endif
		}
		else
*/
		if(c_firmware == 4.60f)
		{
			pokeq(0x80000000002771B8ULL, 0x4E80002038600000ULL ); // fix 8001003C error
			pokeq(0x80000000002771C0ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x800000000005A340ULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x800000000005A404ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x800000000005A3B0ULL, 0x419E00D860000000ULL );
			pokeq(0x800000000005A3B8ULL, 0x2F84000448000098ULL );
			pokeq(0x800000000005E408ULL, 0x2F83000060000000ULL );
			pokeq(0x800000000005E420ULL, 0x2F83000060000000ULL );

			pokeq(0x8000000000059FE4ULL, 0x386000012F830000ULL ); // ignore LIC.DAT check
			pokeq(0x800000000023740CULL, 0x38600000F8690000ULL ); // fix 0x8001002B / 80010017 errors (ported for DEX 2015-01-03)		}

#ifndef COBRA_ONLY
			sc_600 = 0x364628;
			sc_604 = 0x38D2C0;
			sc_142 = 0x350ED0;
#endif
		}
		else
		if(c_firmware >= 4.75f && c_firmware <= 4.78f)
		{
			pokeq(0x800000000026E530ULL, 0x4E80002038600000ULL ); // fix 8001003C error
			pokeq(0x800000000026E538ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x800000000005A344ULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x800000000005A408ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x800000000005A3B4ULL, 0x419E00D860000000ULL ); // Original: 0x419E00D8419D00C0ULL
			pokeq(0x800000000005A3BCULL, 0x2F84000448000098ULL ); // Original: 0x2F840004409C0048ULL //PATCH_JUMP
			pokeq(0x800000000005E498ULL, 0x2F83000060000000ULL ); // fix 80010009 error
			pokeq(0x800000000005E4ACULL, 0x2F83000060000000ULL ); // fix 80010009 error

			pokeq(0x8000000000059FE8ULL, 0x386000012F830000ULL ); // ignore LIC.DAT check
			pokeq(0x800000000022E718ULL, 0x38600000F8690000ULL ); // fix 0x8001002B / 80010017 ERROR FIX

#ifndef COBRA_ONLY
			sc_600 = 0x38CB60; //003B28F8 + 600*8 = 003B3BB8 -> 80 00 00 00 00 38 CB 60
			sc_604 = 0x38CC50; //003B28F8 + 604*8 = 003B3BD8 -> 80 00 00 00 00 38 CC 50
			sc_142 = 0x34ED30; //003B28F8 + 142*8 = 003B2D68 -> 80 00 00 00 00 34 ED 30
#endif
		}
		else
		if(c_firmware == 4.80f)
		{
			//patches by deank
			pokeq(0x800000000026E528ULL, 0x4E80002038600000ULL ); // fix 8001003C error  Original: 0x4E8000208003026CULL
			pokeq(0x800000000026E530ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error  Original: 0x3D201B433C608001ULL
			pokeq(0x800000000005A340ULL, 0x63FF003D60000000ULL ); // fix 8001003D error  Original: 0x63FF003D419EFFD4ULL
			pokeq(0x800000000005A020ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error  Original: 0x3FE0800163FF003EULL

			pokeq(0x800000000005A3B0ULL, 0x419E00D860000000ULL ); // Original: 0x419E00D8419D00C0ULL
			pokeq(0x800000000005A3B8ULL, 0x2F84000448000098ULL ); // Original: 0x2F840004409C0048ULL //PATCH_JUMP
			pokeq(0x800000000005E494ULL, 0x2F83000060000000ULL ); // fix 80010009 error  Original: 0x2F830000419E00ACULL
			pokeq(0x800000000005E4A8ULL, 0x2F83000060000000ULL ); // fix 80010009 error  Original: 0x2F830000419E00ACULL

			pokeq(0x8000000000059FE4ULL, 0x386000012F830000ULL ); // ignore LIC.DAT check
			pokeq(0x800000000022E710ULL, 0x38600000F8690000ULL ); // fix 0x8001002B / 80010017 errors (ported for DEX 2015-08-14)

			pokeq(0x8000000000059A10ULL, 0xF821FE917C0802A6ULL ); // just restore the original
			pokeq(0x800000000005CBD0ULL, 0x419E0038E8610098ULL ); // just restore the original

#ifndef COBRA_ONLY
			sc_600 = 0x38CB60; //0x3B28F8 + 600*8 = 003B3BB8 -> 80 00 00 00 00 38 CB 60
			sc_604 = 0x38CC50; //0x3B28F8 + 604*8 = 003B3BD8 -> 80 00 00 00 00 38 CC 50
			sc_142 = 0x34ED30; //0x3B28F8 + 142*8 = 003B2D68 -> 80 00 00 00 00 34 ED 30
#endif
		}
	}
#endif
#ifdef DEX_SUPPORT
	else if(dex_mode == 2)
	{ // DEX
		if(c_firmware==3.55f)
		{
			pokeq(0x8000000000298790ULL, 0x4E80002038600000ULL );
			pokeq(0x8000000000298798ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x800000000005978CULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x8000000000059850ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x80000000000597FCULL, 0x419E00D860000000ULL );
			pokeq(0x8000000000059804ULL, 0x2F84000448000098ULL );
			pokeq(0x800000000007EF5CULL, 0x2F83000060000000ULL );
			pokeq(0x800000000007EF70ULL, 0x2F83000060000000ULL );
#ifndef COBRA_ONLY
			sc_600 = 0x33FAC8;
			sc_604 = 0x33FC30;
			sc_142 = 0x3010E0;
#endif
		}
		else
		if(c_firmware == 4.21f)
		{
			pokeq(0x800000000029C8C0ULL, 0x4E80002038600000ULL );
			pokeq(0x800000000029C8C8ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x800000000005A938ULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x800000000005A9FCULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x800000000005A9A8ULL, 0x419E00D860000000ULL );
			pokeq(0x800000000005A9B0ULL, 0x2F84000448000098ULL );
			pokeq(0x800000000005E36CULL, 0x2F83000060000000ULL );
			pokeq(0x800000000005E380ULL, 0x2F83000060000000ULL );

#ifndef COBRA_ONLY
			sc_600 = 0x3583F8;
			sc_604 = 0x3584D0;
			sc_142 = 0x318BA0;
#endif
		}
		else
		if(c_firmware == 4.30f)
		{
			pokeq(0x800000000029E034ULL, 0x4E80002038600000ULL );
			pokeq(0x800000000029E03CULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x800000000005AA88ULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x800000000005AB4CULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x800000000005AAF8ULL, 0x419E00D860000000ULL );
			pokeq(0x800000000005AB00ULL, 0x2F84000448000098ULL );
			pokeq(0x800000000005E4BCULL, 0x2F83000060000000ULL );
			pokeq(0x800000000005E4D0ULL, 0x2F83000060000000ULL );

#ifndef COBRA_ONLY
			sc_600 = 0x35A220;
			sc_604 = 0x35A2F8;
			sc_142 = 0x31A7A0;
#endif
		}
		else
		if(c_firmware == 4.31f)
		{
			pokeq(0x800000000029E03CULL, 0x4E80002038600000ULL ); // fix 8001003C error
			pokeq(0x800000000029E044ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x800000000005AA8CULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x800000000005AB50ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x800000000005AAFCULL, 0x419E00D860000000ULL );
			pokeq(0x800000000005AB04ULL, 0x2F84000448000098ULL );
			pokeq(0x800000000005E4C0ULL, 0x2F83000060000000ULL );
			pokeq(0x800000000005E4D4ULL, 0x2F83000060000000ULL );

#ifndef COBRA_ONLY
			sc_600 = 0x35A230;
			sc_604 = 0x35A308;
			sc_142 = 0x31A7B0;
#endif
		}
		else
		if(c_firmware == 4.40f)
		{
			pokeq(0x800000000029D444ULL, 0x4E80002038600000ULL ); // fix 8001003C error
			pokeq(0x800000000029D44CULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x80000000000599D4ULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x8000000000059A98ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x8000000000059A44ULL, 0x419E00D860000000ULL );
			pokeq(0x8000000000059A4CULL, 0x2F84000448000098ULL );
			pokeq(0x800000000005D408ULL, 0x2F83000060000000ULL );
			pokeq(0x800000000005D41CULL, 0x2F83000060000000ULL );

#ifndef COBRA_ONLY
			sc_600 = 0x35AB30;
			sc_604 = 0x35AC08;
			sc_142 = 0x31B050;
#endif
		}
		else
		if(c_firmware == 4.41f)
		{
			pokeq(0x800000000029D44CULL, 0x4E80002038600000ULL );
			pokeq(0x800000000029D454ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x80000000000599D8ULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x8000000000059A9CULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x8000000000059A48ULL, 0x419E00D860000000ULL );
			pokeq(0x8000000000059A50ULL, 0x2F84000448000098ULL );
			pokeq(0x800000000005D40CULL, 0x2F83000060000000ULL );
			pokeq(0x800000000005D420ULL, 0x2F83000060000000ULL );

#ifndef COBRA_ONLY
			sc_600 = 0x35AB40;
			sc_604 = 0x35AC18;
			sc_142 = 0x31B060;
#endif
		}
		else
		if(c_firmware == 4.46f)
		{
			pokeq(0x800000000029D96CULL, 0x4E80002038600000ULL ); // fix 8001003C error
			pokeq(0x800000000029D974ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x80000000000599D8ULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x8000000000059A9CULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x8000000000059A48ULL, 0x419E00D860000000ULL );
			pokeq(0x8000000000059A50ULL, 0x2F84000448000098ULL );
			pokeq(0x800000000005D40CULL, 0x2F83000060000000ULL );
			pokeq(0x800000000005D420ULL, 0x2F83000060000000ULL );

#ifndef COBRA_ONLY
			sc_600 = 0x35B150;
			sc_604 = 0x35B228;
			sc_142 = 0x31B5C8;
#endif
		}
		else
		if(c_firmware == 4.50f)
		{
			pokeq(0x8000000000275D38ULL, 0x4E80002038600000ULL ); // fix 8001003C error
			pokeq(0x8000000000275D40ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x8000000000059A8CULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x8000000000059B50ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x8000000000059AFCULL, 0x419E00D860000000ULL );
			pokeq(0x8000000000059B04ULL, 0x2F84000448000098ULL );
			pokeq(0x800000000005D4C0ULL, 0x2F83000060000000ULL );
			pokeq(0x800000000005D4D4ULL, 0x2F83000060000000ULL );

#ifndef COBRA_ONLY
			sc_600 = 0x35EA90;
			sc_604 = 0x35EB68;
			sc_142 = 0x322B38;
#endif
		}
		else
		if(c_firmware == 4.53f)
		{
			pokeq(0x8000000000275F0CULL, 0x4E80002038600000ULL ); // fix 8001003C error
			pokeq(0x8000000000275F14ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x8000000000059A90ULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x8000000000059B54ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x8000000000059B00ULL, 0x419E00D860000000ULL );
			pokeq(0x8000000000059B08ULL, 0x2F84000448000098ULL );
			pokeq(0x800000000005D4C4ULL, 0x2F83000060000000ULL );
			pokeq(0x800000000005D4D8ULL, 0x2F83000060000000ULL );

#ifndef COBRA_ONLY
			sc_600 = 0x3602A8; //0x385108 + 600*8 = 003863C8 -> 80 00 00 00 00 36 02 A8
			sc_604 = 0x360380; //0x385108 + 604*8 = 003863E8 -> 80 00 00 00 00 36 03 80
			sc_142 = 0x3242F0; //0x385108 + 142*8 = 00385578 -> 80 00 00 00 00 32 42 F0
#endif
		}
		else
		if(c_firmware == 4.55f)
		{
			pokeq(0x8000000000277758ULL, 0x4E80002038600000ULL ); // fix 8001003C error
			pokeq(0x8000000000277760ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x8000000000059D50ULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x8000000000059E14ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x8000000000059DC0ULL, 0x419E00D860000000ULL );
			pokeq(0x8000000000059DC8ULL, 0x2F84000448000098ULL );
			pokeq(0x800000000005DCB8ULL, 0x2F83000060000000ULL );
			pokeq(0x800000000005DCD0ULL, 0x2F83000060000000ULL );

#ifndef COBRA_ONLY
			sc_600 = 0x3634F8; //0x388488 + 600*8 = 00389748 -> 80 00 00 00 00 36 34 F8
			sc_604 = 0x3635D0; //0x388488 + 604*8 = 00389768 -> 80 00 00 00 00 36 35 D0
			sc_142 = 0x327348; //0x388488 + 142*8 = 003888F8 -> 80 00 00 00 00 32 73 48
#endif
		}
		else
		if(c_firmware == 4.60f)
		{
			pokeq(0x80000000002764F0ULL, 0x4E80002038600000ULL ); // fix 8001003C error
			pokeq(0x80000000002764F8ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error
			pokeq(0x8000000000059F58ULL, 0x63FF003D60000000ULL ); // fix 8001003D error
			pokeq(0x800000000005A01CULL, 0x3FE080013BE00000ULL ); // fix 8001003E error

			pokeq(0x8000000000059FC8ULL, 0x419E00D860000000ULL );
			pokeq(0x8000000000059FD0ULL, 0x2F84000448000098ULL );
			pokeq(0x800000000005E020ULL, 0x2F83000060000000ULL );
			pokeq(0x800000000005E038ULL, 0x2F83000060000000ULL );

			pokeq(0x8000000000059BFCULL, 0x386000012F830000ULL ); // ignore LIC.DAT check
			pokeq(0x80000000002367C4ULL, 0x38600000F8690000ULL ); // fix 0x8001002B / 80010017 errors (ported for DEX 2015-01-03)

			pokeq(0x8000000000059628ULL, 0xF821FE917C0802A6ULL ); // just restore the original
			pokeq(0x800000000005C77CULL, 0x419E0038E8610098ULL ); // just restore the original

#ifndef COBRA_ONLY
			sc_600 = 0x364DE0; //0x38A120 + 600*8 = 0038B3E0 -> 80 00 00 00 00 36 4D E0
			sc_604 = 0x364EB8; //0x38A120 + 604*8 = 0038B400 -> 80 00 00 00 00 36 4E B8
			sc_142 = 0x328E70; //0x38A120 + 142*8 = 0038A590 -> 80 00 00 00 00 32 8E 70
#endif
		}
		else
		if(c_firmware == 4.65f || c_firmware == 4.66f)
		{
			//patches by deank
			pokeq(0x80000000002764F8ULL, 0x4E80002038600000ULL ); // fix 8001003C error  Original: 0x4E8000208003026CULL
			pokeq(0x8000000000276500ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error  Original: 0x3D6000463D201B43ULL
			pokeq(0x8000000000059F5CULL, 0x63FF003D60000000ULL ); // fix 8001003D error  Original: 0x63FF003D419EFFD4ULL
			pokeq(0x800000000005A020ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error  Original: 0x3FE0800163FF003EULL

			pokeq(0x8000000000059FCCULL, 0x419E00D860000000ULL ); // Original: 0x419E00D8419D00C0ULL
			pokeq(0x8000000000059FD4ULL, 0x2F84000448000098ULL ); // Original: 0x2F840004409C0048ULL //PATCH_JUMP
			pokeq(0x800000000005E028ULL, 0x2F83000060000000ULL ); // fix 80010009 error  Original: 0x2F830000419E00ACULL
			pokeq(0x800000000005E03CULL, 0x2F83000060000000ULL ); // fix 80010009 error  Original: 0x2F830000419E00ACULL

			pokeq(0x8000000000059C00ULL, 0x386000012F830000ULL ); // ignore LIC.DAT check
			pokeq(0x80000000002367CCULL, 0x38600000F8690000ULL ); // fix 0x8001002B / 80010017 errors (ported for DEX 2015-01-03)

			pokeq(0x800000000005962CULL, 0xF821FE917C0802A6ULL ); // just restore the original
			pokeq(0x800000000005C780ULL, 0x419E0038E8610098ULL ); // just restore the original

#ifndef COBRA_ONLY
			sc_600 = 0x364DF0; //0x38A120 + 600*8 = 0038B3E0 -> 80 00 00 00 00 36 4D F0
			sc_604 = 0x364EC8; //0x38A120 + 604*8 = 0038B400 -> 80 00 00 00 00 36 4E C8
			sc_142 = 0x328E80; //0x38A120 + 142*8 = 0038A590 -> 80 00 00 00 00 32 8E 80
#endif
		}
		else
		if(c_firmware == 4.70f)
		{
			//patches by deank
			pokeq(0x800000000026D7F4ULL, 0x4E80002038600000ULL ); // fix 8001003C error  Original: 0x4E8000208003026CULL
			pokeq(0x800000000026D7FCULL, 0x7C6307B44E800020ULL ); // fix 8001003C error  Original: 0x3D201B433C608001ULL
			pokeq(0x8000000000059F58ULL, 0x63FF003D60000000ULL ); // fix 8001003D error  Original: 0x63FF003D419EFFD4ULL
			pokeq(0x800000000005A01CULL, 0x3FE080013BE00000ULL ); // fix 8001003E error  Original: 0x3FE0800163FF003EULL

			pokeq(0x8000000000059FC8ULL, 0x419E00D860000000ULL ); // Original: 0x419E00D8419D00C0ULL
			pokeq(0x8000000000059FD0ULL, 0x2F84000448000098ULL ); // Original: 0x2F840004409C0048ULL //PATCH_JUMP
			pokeq(0x800000000005E0ACULL, 0x2F83000060000000ULL ); // fix 80010009 error  Original: 0x2F830000419E00ACULL
			pokeq(0x800000000005E0C0ULL, 0x2F83000060000000ULL ); // fix 80010009 error  Original: 0x2F830000419E00ACULL

			pokeq(0x8000000000059BFCULL, 0x386000012F830000ULL ); // ignore LIC.DAT check
			pokeq(0x800000000022DAC8ULL, 0x38600000F8690000ULL ); // fix 0x8001002B / 80010017 errors (ported for DEX 2015-04)

			pokeq(0x8000000000059628ULL, 0xF821FE917C0802A6ULL ); // just restore the original
			pokeq(0x800000000005C7E8ULL, 0x419E0038E8610098ULL ); // just restore the original

#ifndef COBRA_ONLY
			sc_600 = 0x3647B8; //0x38A368 + 600*8 = 0038B628 -> 80 00 00 00 00 36 47 B8
			sc_604 = 0x364890; //0x38A368 + 604*8 = 0038B648 -> 80 00 00 00 00 36 48 90
			sc_142 = 0x329190; //0x38A368 + 142*8 = 0038A7D8 -> 80 00 00 00 00 32 91 90
#endif
		}
		else
		if(c_firmware >= 4.75f && c_firmware <= 4.78f)
		{
			//patches by deank
			pokeq(0x800000000026D868ULL, 0x4E80002038600000ULL ); // fix 8001003C error  Original: 0x4E8000208003026CULL
			pokeq(0x800000000026D870ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error  Original: 0x3D6000473D201B43ULL
			pokeq(0x8000000000059F5CULL, 0x63FF003D60000000ULL ); // fix 8001003D error  Original: 0x63FF003D419EFFD4ULL
			pokeq(0x800000000005A020ULL, 0x3FE080013BE00000ULL ); // fix 8001003E error  Original: 0x3FE0800163FF003EULL

			pokeq(0x8000000000059FCCULL, 0x419E00D860000000ULL ); // Original: 0x419E00D8419D00C0ULL
			pokeq(0x8000000000059FD4ULL, 0x2F84000448000098ULL ); // Original: 0x2F840004409C0048ULL //PATCH_JUMP
			pokeq(0x800000000005E0B0ULL, 0x2F83000060000000ULL ); // fix 80010009 error  Original: 0x2F830000419E00ACULL
			pokeq(0x800000000005E0C4ULL, 0x2F83000060000000ULL ); // fix 80010009 error  Original: 0x2F830000419E00ACULL

			pokeq(0x8000000000059C00ULL, 0x386000012F830000ULL ); // ignore LIC.DAT check
			pokeq(0x800000000022DAD0ULL, 0x38600000F8690000ULL ); // fix 0x8001002B / 80010017 errors (ported for DEX 2015-08-14)

			pokeq(0x800000000005962CULL, 0xF821FE917C0802A6ULL ); // just restore the original
			pokeq(0x800000000005C7ECULL, 0x419E0038E8610098ULL ); // just restore the original

#ifndef COBRA_ONLY
			sc_600 = 0x364848; //0x38A3E8 + 600*8 = 0038B6A8 -> 80 00 00 00 00 36 48 48
			sc_604 = 0x364920; //0x38A3E8 + 604*8 = 0038B6C8 -> 80 00 00 00 00 36 49 20
			sc_142 = 0x329220; //0x38A3E8 + 142*8 = 0038A858 -> 80 00 00 00 00 32 92 20
#endif
		}
		else
		if(c_firmware == 4.80f)
		{
			//patches by deank
			pokeq(0x800000000026D860ULL, 0x4E80002038600000ULL ); // fix 8001003C error  Original: 0x4E8000208003026CULL
			pokeq(0x800000000026D868ULL, 0x7C6307B44E800020ULL ); // fix 8001003C error  Original: 0x3D201B433C608001ULL
			pokeq(0x8000000000059F58ULL, 0x63FF003D60000000ULL ); // fix 8001003D error  Original: 0x63FF003D419EFFD4ULL
			pokeq(0x800000000005A01CULL, 0x3FE080013BE00000ULL ); // fix 8001003E error  Original: 0x3FE0800163FF003EULL

			pokeq(0x8000000000059FC8ULL, 0x419E00D860000000ULL ); // Original: 0x419E00D8419D00C0ULL
			pokeq(0x8000000000059FD0ULL, 0x2F84000448000098ULL ); // Original: 0x2F840004409C0048ULL //PATCH_JUMP
			pokeq(0x800000000005E0ACULL, 0x2F83000060000000ULL ); // fix 80010009 error  Original: 0x2F830000419E00ACULL
			pokeq(0x800000000005E0C0ULL, 0x2F83000060000000ULL ); // fix 80010009 error  Original: 0x2F830000419E00ACULL

			pokeq(0x8000000000059BFCULL, 0x386000012F830000ULL ); // ignore LIC.DAT check
			pokeq(0x800000000022DAC8ULL, 0x38600000F8690000ULL ); // fix 0x8001002B / 80010017 errors (ported for DEX 2015-08-14)

			pokeq(0x8000000000059628ULL, 0xF821FE917C0802A6ULL ); // just restore the original
			pokeq(0x800000000005C7E8ULL, 0x419E0038E8610098ULL ); // just restore the original

#ifndef COBRA_ONLY
			sc_600 = 0x364918; //0x38A4E8 + 600*8 = 0038B7A8 -> 80 00 00 00 00 36 49 18
			sc_604 = 0x3649F0; //0x38A4E8 + 604*8 = 0038B7C8 -> 80 00 00 00 00 36 49 F0
			sc_142 = 0x3292F0; //0x38A4E8 + 142*8 = 0038A958 -> 80 00 00 00 00 32 92 F0
#endif
		}
	}
#endif

	char _path[MAX_PATH_LEN];

	if(islike(_path0, "http") || !extcmp(_path0, ".htm", 4))
	{
		char *p = strstr(_path0, "http");
		if(!p) sprintf(_path, "http://%s%s", local_ip, _path0); else sprintf(_path, "%s", p);

		is_mounting = false;

		if(IS_ON_XMB)
		{
			while(View_Find("explore_plugin") == 0) sys_timer_sleep(1); // wait for explore_plugin

			do_umount(false);
			open_browser(_path, 0);
			return true;
		}

		return false;
	}

	strcpy(_path, _path0);


	if(_path[0]=='/')
	{
		char *p = strstr(_path, "/PS3_"); if(p) p[0] = NULL;
	}

#ifndef LITE_EDITION
	if(islike(_path, "/net") && (_path[4] >= '0' && _path[4] <= '4') && _path[5] == NULL) strcat(_path, "/.");
#endif

	char titleID[10];

	if(islike(_path, "/dev_bdvd")) {do_umount(false); is_mounting = false; return false;}

#ifndef COBRA_ONLY
 #ifdef EXT_GDATA
	if(do_eject==MOUNT_EXT_GDATA) goto patch;
 #endif
#endif

	// save lastgame.bin / process _next & _prev commands
	if(do_eject)
	{
		bool _prev=false, _next=false;

		_next=(bool)(!strcmp(_path, "_next"));
		_prev=(bool)(!strcmp(_path, "_prev"));

		int fd = 0;
		_lastgames lastgames; memset(&lastgames, 0, sizeof(_lastgames)); lastgames.last = 0xFF;

		if(cellFsOpen(WMTMP "/last_games.bin", CELL_FS_O_RDONLY, &fd, NULL, 0) == CELL_FS_SUCCEEDED)
		{
			cellFsRead(fd, (void *)&lastgames, sizeof(_lastgames), NULL);
			cellFsClose(fd);
		}

		if(_next || _prev)
		{
			if(lastgames.last >= MAX_LAST_GAMES) lastgames.last = 0;
			if(_prev)
			{
				if(lastgames.last == 0) lastgames.last = LAST_GAMES_UPPER_BOUND; else lastgames.last--;
			}
			else
			if(_next)
			{
				if(lastgames.last == LAST_GAMES_UPPER_BOUND) lastgames.last = 0; else lastgames.last++;
			}
			if(lastgames.game[lastgames.last][0]!='/') lastgames.last = 0;
			if(lastgames.game[lastgames.last][0]!='/' || strlen(lastgames.game[lastgames.last]) < 7) goto exit_mount;

			strcpy(_path, lastgames.game[lastgames.last]);
		}
		else
		if(lastgames.last >= MAX_LAST_GAMES)
		{
			lastgames.last = 0;
			strcpy(lastgames.game[lastgames.last], _path);
		}
		else
		{
			bool found = false;
			for(u8 n = 0; n < MAX_LAST_GAMES; n++)
			{
				if(!strcmp(lastgames.game[n], _path)) {found = true; break;}
			}
			if(!found)
			{
				lastgames.last++;
				if(lastgames.last>=MAX_LAST_GAMES) lastgames.last = 0;
				strcpy(lastgames.game[lastgames.last], _path);
			}
		}

		savefile(WMTMP "/last_games.bin", (char *)&lastgames, sizeof(_lastgames));
	}

	// last mounted game
	if(_path[0]=='_' || strrchr(_path, '/') == NULL) goto exit_mount;
	else
	{
		char path2[MAX_PATH_LEN]; int fd;
		sprintf(path2, WMTMP "/last_game.txt");

		if(cellFsOpen(path2, CELL_FS_O_CREAT | CELL_FS_O_TRUNC | CELL_FS_O_WRONLY, &fd, NULL, 0) == CELL_FS_SUCCEEDED)
		{
			uint64_t written = 0;
			cellFsWrite(fd, (void *)_path, strlen(_path), &written);
			cellFsClose(fd);
			cellFsChmod(path2, MODE);
		}
	}

	if(do_eject) show_msg(_path);

#ifdef COBRA_ONLY
 #ifdef FAKEISO
	// launch ntfs fake iso
	{
		if(!extcmp(_path, ".ntfs[PSXISO]", 13) || (strstr(_path, ".ntfs[") != NULL && strstr(_path, "[raw]") != NULL))
		{
			bool found=false; u8 n;
			const char raw_iso_sprx[4][40] = {  "/dev_flash/vsh/module/raw_iso.sprx",
												"/dev_hdd0/raw_iso.sprx",
												"/dev_hdd0/plugins/raw_iso.sprx",
												"/dev_hdd0/game/IRISMAN00/sprx_iso" };

			for(n = 0; n < 4; n++)
				if(file_exists(raw_iso_sprx[n])) {found = true; break;}

			if(found)
			{
				cellFsChmod(_path, MODE);

				int fdw;
				if(cellFsOpen((char*)_path, CELL_FS_O_RDONLY, &fdw, NULL, 0) == CELL_FS_SUCCEEDED)
				{
					sys_addr_t addr = 0;
					if(sys_memory_allocate(_64KB_, SYS_MEMORY_PAGE_SIZE_64K, &addr) == 0)
					{
						u8* sprx_data = (u8*)addr; uint64_t msiz = 0;

						cellFsLseek(fdw, 0, CELL_FS_SEEK_SET, &msiz);
						cellFsRead(fdw, sprx_data, _64KB_, &msiz);
						cellFsClose(fdw);

						do_umount(false);

						if(cobra_load_vsh_plugin(0, (char*)raw_iso_sprx[n], sprx_data, msiz) != 0) ret = false;

						sys_memory_free(addr);
					}
					goto exit_mount;
				}
			}
		}
	}
 #endif
#endif

	// Launch PS2 Classic
	if(!extcmp(_path, ".BIN.ENC", 8))
	{
		char temp[MAX_PATH_LEN];

		if(file_exists(PS2_CLASSIC_PLACEHOLDER))
		{
			sprintf(temp, "PS2 Classic\n%s", strrchr(_path, '/') + 1);
			copy_in_progress = true; copied_count = 0;
			show_msg(temp);

			if(c_firmware >= 4.65f)
			{   // Auto create "classic_ps2 flag" for PS2 Classic (.BIN.ENC) on rebug 4.65.2
				do_umount(false);
				enable_classic_ps2_mode();
			}

			cellFsUnlink(PS2_CLASSIC_ISO_PATH);
			if(file_copy(_path, (char*)PS2_CLASSIC_ISO_PATH, COPY_WHOLE_FILE) == 0)
			{
				if(file_exists(PS2_CLASSIC_ISO_ICON ".bak") == false)
					file_copy((char*)PS2_CLASSIC_ISO_ICON, (char*)(PS2_CLASSIC_ISO_ICON ".bak"), COPY_WHOLE_FILE);

				sprintf(temp, "%s.png", _path);
				if(file_exists(temp) == false) sprintf(temp, "%s.PNG", _path);

				cellFsUnlink(PS2_CLASSIC_ISO_ICON);
				if(file_exists(temp))
					file_copy(temp, (char*)PS2_CLASSIC_ISO_ICON, COPY_WHOLE_FILE);
				else
					file_copy((char*)(PS2_CLASSIC_ISO_ICON ".bak"), (char*)PS2_CLASSIC_ISO_ICON, COPY_WHOLE_FILE);

				if(webman_config->fanc) restore_fan(1); //fan_control( ((webman_config->ps2temp*255)/100), 0);

				// create "wm_noscan" to avoid re-scan of XML returning to XMB from PS2
				savefile((char*)WMNOSCAN, NULL, 0);

				sprintf(temp, "\"%s\" %s", strrchr(_path, '/') + 1, STR_LOADED2);
			}
			else
				{sprintf(temp, "PS2 Classic\n%s", STR_ERROR); ret = false;}

			show_msg(temp);
			copy_in_progress = false;
		}
		else
		{
			sprintf(temp, "PS2 Classic Placeholder %s", STR_NOTFOUND);
			show_msg(temp);
			ret = false;
		}

		goto patch;
	}

	if((c_firmware >= 4.65f) && strstr(_path, "/PS2ISO")!=NULL)
	{   // Auto remove "classic_ps2" flag for PS2 ISOs on rebug 4.65.2
		disable_classic_ps2_mode();
	}

#ifdef COBRA_ONLY
	//if(cobra_mode)
	{
 #ifdef EXT_GDATA
		{
			// auto-enable external GD
			if(do_eject != 1) ;
			else if(strstr(_path, "/GAME"))
			{
				int fdd = 0; char extgdfile[MAX_PATH_LEN];
				sprintf(extgdfile, "%s/PS3_GAME/PS3GAME.INI", _path);
				if(cellFsOpen(extgdfile, CELL_FS_O_RDONLY, &fdd, NULL, 0) == CELL_FS_SUCCEEDED)
				{
					uint64_t read_e = 0;
					if(cellFsRead(fdd, (void *)&extgdfile, 12, &read_e) == CELL_FS_SUCCEEDED) extgdfile[read_e] = NULL;
					cellFsClose(fdd);
					if((extgd == 0) &&  (extgdfile[10] & (1<<1))) set_gamedata_status(1, false); else
					if((extgd == 1) && !(extgdfile[10] & (1<<1))) set_gamedata_status(0, false);
				}
				else if(extgd) set_gamedata_status(0, false);
			}
			else if((extgd == 0) && (strstr(_path, "/PS3ISO")!=NULL) && (strstr(_path, "[gd]")!=NULL))
				set_gamedata_status(1, false);
			else if(extgd) set_gamedata_status(0, false);
		}
 #endif //#ifdef EXT_GDATA

 mount_again:
		do_umount(false);

		sys_timer_usleep(4000);

		cobra_send_fake_disc_eject_event();

		sys_timer_usleep(4000);

		if( !extcasecmp(_path, ".iso", 4) ||
			strstr(_path, "/PS3ISO") || strstr(_path, "/BDISO")    || strstr(_path, "/DVDISO") || strstr(_path, "/PS2ISO") ||
			strstr(_path, "/PSXISO") || strstr(_path, "/PSXGAMES") || strstr(_path, "/PSPISO") || strstr(_path, "/ISO/")   || strstr(_path, ".ntfs[") ||
			(strstr(_path, "/net") == _path && _path[4]>='0' && _path[4]<='4') ||
			!extcasecmp(_path, ".mdf", 4) || !extcasecmp(_path, ".bin", 4) || !extcasecmp(_path, ".img", 4) )
		{
			if( strstr(_path, "/PSXISO") || strstr(_path, "/PSXGAMES") || !extcmp(_path, ".ntfs[PSXISO]", 13) ) select_ps1emu();

			//if(_next || _prev)
				sys_timer_sleep(1);
			//else
			//	sys_timer_usleep(50000);

			u8 iso_parts = 1;
			char templn[MAX_LINE_LEN];
			char iso_list[MAX_ISO_PARTS][MAX_PATH_LEN];
			char *cobra_iso_list[MAX_ISO_PARTS];
			char path2[MAX_PATH_LEN];
			strcpy(iso_list[0], _path);
			cobra_iso_list[0] = (char*)iso_list[0];

			CD_SECTOR_SIZE_2352 = 2352;

			if(!extcasecmp(_path, ".iso.0", 6))
			{
				sprintf(path2, "%s", _path);
				path2[strlen(path2)-2] = NULL;
				for(u8 n = 1; n < MAX_ISO_PARTS; n++)
				{
					sprintf(templn, "%s.%i", path2, n);
					if(file_exists(templn))
					{
						iso_parts++;
						strcpy(iso_list[n], templn);
						cobra_iso_list[n] = (char*)iso_list[n];
					}
					else
						break;
				}
			}

			if(strstr(_path, ".ntfs["))
			{
				sys_addr_t addr = 0;
				if(sys_memory_allocate(_64KB_, SYS_MEMORY_PAGE_SIZE_64K, &addr) == 0)
				{
					int fdw;
					if(cellFsOpen(_path, CELL_FS_O_RDONLY, &fdw, NULL, 0) == CELL_FS_SUCCEEDED)
					{
						u8* sprx_data = (u8*)addr; uint64_t msiz = 0;

						cellFsLseek(fdw, 0, CELL_FS_SEEK_SET, &msiz);
						cellFsRead(fdw, sprx_data, _64KB_, &msiz);
						cellFsClose(fdw);

						sys_ppu_thread_create(&thread_id_ntfs, rawseciso_thread, (uint64_t)addr, THREAD_PRIO, THREAD_STACK_SIZE_8KB, SYS_PPU_THREAD_CREATE_JOINABLE, THREAD_NAME_NTFS);

						if(!extcmp(_path, ".ntfs[PS3ISO]", 13))
						{
							get_name(templn, _path, 0);
							cache_icon0_and_param_sfo(templn);
 #ifdef FIX_GAME
							fix_game(_path, titleID, webman_config->fixgame);
 #endif
						}
					}
				}
				goto patch;
				//return;
			}

 #ifndef LITE_EDITION
			if(islike(_path, "/net") && _path[4]>='0' && _path[4]<='4')
			{
				sys_addr_t _netiso_args = 0; netiso_svrid = -1;
				if(sys_memory_allocate(_64KB_, SYS_MEMORY_PAGE_SIZE_64K, &_netiso_args) == 0)
				{
					netiso_args *mynet_iso = (netiso_args*)_netiso_args;
					memset(mynet_iso, 0, _64KB_);

					if( (_path[4]=='0' && webman_config->netd0 && webman_config->neth0[0] && webman_config->netp0>0)
					||	(_path[4]=='1' && webman_config->netd1 && webman_config->neth1[0] && webman_config->netp1>0)
					||	(_path[4]=='2' && webman_config->netd2 && webman_config->neth2[0] && webman_config->netp2>0)
#ifdef NET3NET4
					||	(_path[4]=='3' && webman_config->netd3 && webman_config->neth3[0] && webman_config->netp3>0)
					||	(_path[4]=='4' && webman_config->netd4 && webman_config->neth4[0] && webman_config->netp4>0)
 #endif
					  )
					{
						if(_path[4]=='1')
						{
							sprintf(mynet_iso->server, "%s", webman_config->neth1);
							mynet_iso->port=webman_config->netp1; netiso_svrid = 1;
						}
						else
						if(_path[4]=='2')
						{
							sprintf(mynet_iso->server, "%s", webman_config->neth2);
							mynet_iso->port=webman_config->netp2; netiso_svrid = 2;
						}
 #ifdef NET3NET4
						else
						if(_path[4]=='3')
						{
							sprintf(mynet_iso->server, "%s", webman_config->neth3);
							mynet_iso->port=webman_config->netp3; netiso_svrid = 3;
						}
						else
						if(_path[4]=='4')
						{
							sprintf(mynet_iso->server, "%s", webman_config->neth4);
							mynet_iso->port=webman_config->netp4; netiso_svrid = 4;
						}
 #endif
						else
						{
							sprintf(mynet_iso->server, "%s", webman_config->neth0);
							mynet_iso->port=webman_config->netp0; netiso_svrid = 0;
						}
					}
					else
					{
						sys_memory_free(_netiso_args);
						goto patch;
					}

					char *netpath = _path + 5;

					strcpy(mynet_iso->path, netpath);
					if(strstr(netpath, "/PS3ISO") == netpath) mynet_iso->emu_mode=EMU_PS3; else
					if(strstr(netpath, "/PS2ISO") == netpath) goto copy_ps2iso_to_hdd0;    else
					if(strstr(netpath, "/PSPISO") == netpath) goto copy_pspiso_to_hdd0;    else
					if(strstr(netpath, "/BDISO" ) == netpath) mynet_iso->emu_mode=EMU_BD;  else
					if(strstr(netpath, "/DVDISO") == netpath) mynet_iso->emu_mode=EMU_DVD; else
					if(strstr(netpath, "/PSX")    == netpath)
					{
						TrackDef tracks[1];
						tracks[0].lba = 0;
						tracks[0].is_audio = 0;
						mynet_iso->emu_mode=EMU_PSX;
						mynet_iso->num_tracks=1;

						memcpy(mynet_iso->tracks, tracks, sizeof(TrackDef));

						// detect sector size (default 2352)
						{
							cellFsUnlink(TEMP_NET_PSXISO);

							int ns = connect_to_remote_server(netiso_svrid);

							if(!extcasecmp(mynet_iso->path, ".cue", 4))
							{
								int flen = strlen(mynet_iso->path) - 4;
								char extensions[8][8] = {".bin", ".iso", ".img", ".mdf", ".BIN", ".ISO", ".IMG", ".MDF"};
								for(u8 e = 0; e < 8; e++)
								{
									mynet_iso->path[flen] = NULL; strcat(mynet_iso->path, extensions[e]);
									copy_net_file(TEMP_NET_PSXISO, mynet_iso->path, ns, _64KB_);
									if(file_exists(TEMP_NET_PSXISO)) break;
								}
							}
							else
								copy_net_file(TEMP_NET_PSXISO, mynet_iso->path, ns, _64KB_);

							if(ns >= 0) {shutdown(ns, SHUT_RDWR); socketclose(ns);}

							if(cellFsOpen(TEMP_NET_PSXISO, CELL_FS_O_RDONLY, &ns, NULL, 0) == CELL_FS_SUCCEEDED)
							{
								CD_SECTOR_SIZE_2352 = detect_cd_sector_size(ns);
								cellFsClose(ns);
							}

							cellFsUnlink(TEMP_NET_PSXISO);
						}
					}
					else if(strstr(netpath, "/GAMES") == netpath || strstr(netpath, "/GAMEZ") == netpath)
					{
						mynet_iso->emu_mode = EMU_PS3;
						sprintf(mynet_iso->path, "/***PS3***%s", netpath);
					}
					else
					{
						mynet_iso->emu_mode=EMU_DVD;
						if(!extcasecmp(netpath, ".iso", 4) || !extcasecmp(netpath, ".mdf", 4) || !extcasecmp(netpath, ".img", 4) || !extcasecmp(netpath, ".bin", 4)) ;
						else
							sprintf(mynet_iso->path, "/***DVD***%s", netpath);
					}

					sys_ppu_thread_create(&thread_id_net, netiso_thread, (uint64_t)_netiso_args, THREAD_PRIO, THREAD_STACK_SIZE_8KB, SYS_PPU_THREAD_CREATE_JOINABLE, THREAD_NAME_NET);

					if(mynet_iso->emu_mode==EMU_PS3)
					{
						get_name(templn, _path, 1);
						cache_icon0_and_param_sfo(templn);
 #ifdef FIX_GAME
						fix_game(_path, titleID, webman_config->fixgame);
 #endif
					}
				}
				goto patch;
				//return;
			}
			else
 #endif //#ifndef LITE_EDITION
			{
				cellFsUnlink((char*)WMNOSCAN); // remove wm_noscan if PS2ISO was already mounted

				if(strstr(_path, "/PS3ISO") || mount_unk == EMU_PS3)
				{
 #ifdef FIX_GAME
					if(webman_config->fixgame!=FIX_GAME_DISABLED)
					{
						fix_in_progress=true; fix_aborted = false;
						fix_iso(_path, 0x100000UL, true);
						fix_in_progress=false;
					}
 #endif //#ifdef FIX_GAME

					cobra_mount_ps3_disc_image(cobra_iso_list, iso_parts);
					sys_timer_usleep(2500);
					cobra_send_fake_disc_insert_event();

					{
						get_name(templn, _path, 1);
						cache_icon0_and_param_sfo(templn);
					}
					goto patch;
					//return;
				}
				else if(strstr(_path, "/PSPISO") || strstr(_path, "/ISO/") || mount_unk == EMU_PSP)
				{
 #ifndef LITE_EDITION
					if(islike(_path, "/net"))
					{
 copy_pspiso_to_hdd0:
						cache_file_to_hdd(_path, iso_list[0], "/PSPISO", templn);
					}
 #endif //#ifndef LITE_EDITION

					mount_unk = EMU_PSP;

					cellFsUnlink((char*)"/dev_hdd0/game/PSPC66820/PIC1.PNG");
					cobra_unset_psp_umd();

					if(file_exists(iso_list[0]))
					{
						int result = cobra_set_psp_umd2(iso_list[0], NULL, (char*)"/dev_hdd0/tmp/psp_icon.png", 2);

						if(result==ENOTSUP || result==EABORT)
						{
							ret = false;
						}
						else if(!result)
						{
							cobra_send_fake_disc_insert_event();
							ret = true;
						}
					}
					else
						ret = false;

					goto exit_mount;
				}
				else if(strstr(_path, "/PS2ISO") || mount_unk == EMU_PS2_DVD)
				{
 #ifndef LITE_EDITION
					if(!islike(_path, "/dev_hdd0"))
					{
 copy_ps2iso_to_hdd0:
						cache_file_to_hdd(_path, iso_list[0], "/PS2ISO", templn);
					}
 #endif //#ifndef LITE_EDITION

					if(file_exists(iso_list[0]))
					{
						TrackDef tracks[1];
						tracks[0].lba = 0;
						tracks[0].is_audio = 0;
						cobra_mount_ps2_disc_image(cobra_iso_list, 1, tracks, 1);
						if(webman_config->fanc) restore_fan(1); //fan_control( ((webman_config->ps2temp*255)/100), 0);

						// create "wm_noscan" to avoid re-scan of XML returning to XMB from PS2
						savefile((char*)WMNOSCAN, NULL, 0);
					}
					else
						ret = false;
				}
				else if(strstr(_path, "/PSXISO") || strstr(_path, "/PSXGAMES") || mount_unk == EMU_PSX)
				{
					int flen = strlen(_path) - 4;

					if(flen < 0) ;

					else if(!extcasecmp(_path, ".cue", 4))
					{
						char extensions[8][8] = {".bin", ".iso", ".img", ".mdf", ".BIN", ".ISO", ".IMG", ".MDF"};
						for(u8 e = 0; e < 8; e++)
						{
							cobra_iso_list[0][flen] = NULL; strcat(cobra_iso_list[0], extensions[e]);
							if(file_exists(cobra_iso_list[0])) break;
						}
					}
					else if(_path[flen] == '.')
					{
						_path[flen] = NULL; strcat(_path, ".cue");
						if(file_exists(_path) == false) {_path[flen] = NULL; strcat(_path, ".CUE");
						if(file_exists(_path) == false) sprintf(_path, "%s", cobra_iso_list[0]);}
					}

					if(!extcasecmp(_path, ".cue", 4))
					{
						unsigned int num_tracks = 0;
						int fdw;

						sys_addr_t sysmem = 0;
						if(sys_memory_allocate(_64KB_, SYS_MEMORY_PAGE_SIZE_64K, &sysmem) == 0)
						{
							if(cellFsOpen(_path, CELL_FS_O_RDONLY, &fdw, NULL, 0) == CELL_FS_SUCCEEDED)
							{
								char *buf = (char*)sysmem; uint64_t msiz = 0;
								cellFsLseek(fdw, 0, CELL_FS_SEEK_SET, NULL);
								cellFsRead(fdw, (void *)buf, 65535, &msiz);
								cellFsClose(fdw);

								if(msiz > 10)
								{
									TrackDef tracks[32];
									tracks[0].lba = 0;
									tracks[0].is_audio = 0;

									char tcode[MAX_LINE_LEN];
									u8 tmin = 0, tsec = 0, tfrm = 0;
									u8 use_pregap = 0;
									u32 lp = 0;

									while(lp < msiz)// get_line ( templn, 512, sysmem ) != NULL )
									{
										u8 line_found = 0;
										templn[0] = NULL;
										for(u32 l = 0; l < MAX_LINE_LEN; l++)
										{
											if(l>=msiz) break;
											if(lp<msiz && buf[lp] && buf[lp]!='\n' && buf[lp]!='\r')
											{
												templn[l]=buf[lp];
												templn[l+1] = NULL;
											}
											else
											{
												templn[l] = NULL;
											}
											if(buf[lp]=='\n' || buf[lp]=='\r') line_found = 1;
											lp++;
											if(buf[lp]=='\n' || buf[lp]=='\r') lp++;

											if(templn[l] == NULL) break;
										}

										if(!line_found) break;

										if(strstr(templn, "PREGAP")) {use_pregap = 1; continue;}
										if(!strstr(templn, "INDEX 01") && !strstr(templn, "INDEX 1 ")) continue;

										sprintf(tcode, "%s", strrchr(templn, ' ') + 1); tcode[8] = NULL;
										if(strlen(tcode)!=8 || tcode[2]!=':' || tcode[5]!=':') continue;
										tmin = (tcode[0]-'0')*10 + (tcode[1]-'0');
										tsec = (tcode[3]-'0')*10 + (tcode[4]-'0');
										tfrm = (tcode[6]-'0')*10 + (tcode[7]-'0');
										if(use_pregap && num_tracks) tsec += 2;

										if(num_tracks) tracks[num_tracks].is_audio = 1;
										tracks[num_tracks].lba = (tmin * 60 + tsec) * 75 + tfrm;

										num_tracks++; if(num_tracks >= 32) break;
									}

									if(!num_tracks) num_tracks++;
									cobra_mount_psx_disc_image(cobra_iso_list[0], tracks, num_tracks);
								}
							}
							else
							{
								TrackDef tracks[1];
								tracks[0].lba = 0;
								tracks[0].is_audio = 0;
								cobra_mount_psx_disc_image_iso(cobra_iso_list[0], tracks, 1);
							}

							sys_memory_free(sysmem);
						}
					}
					else
					{
						TrackDef tracks[1];
						tracks[0].lba = 0;
						tracks[0].is_audio = 0;
						cobra_mount_psx_disc_image_iso(cobra_iso_list[0], tracks, 1);
					}
				}
				else if(strstr(_path, "/DVDISO") || mount_unk == EMU_DVD)
					cobra_mount_dvd_disc_image(cobra_iso_list, iso_parts);
				else if(strstr(_path, "/BDISO")  || mount_unk == EMU_BD)
					cobra_mount_bd_disc_image(cobra_iso_list, iso_parts);
				else
				{
					// mount iso as data
					cobra_mount_bd_disc_image(cobra_iso_list, iso_parts);
					sys_timer_usleep(2500);
					cobra_send_fake_disc_insert_event();

					waitfor("/dev_bdvd", 3);

					// re-mount with media type
					if(isDir("/dev_bdvd/PS3_GAME")) mount_unk = EMU_PS3; else
					if(isDir("/dev_bdvd/VIDEO_TS")) mount_unk = EMU_DVD; else
					if(file_exists("/dev_bdvd/SYSTEM.CNF") || strcasestr(_path, "PS2")) mount_unk = EMU_PS2_DVD; else
					if(strcasestr(_path, "PSP")!=NULL && !extcasecmp(_path, ".iso", 4)) mount_unk = EMU_PSP; else
					if(!file_exists("/dev_bdvd")) mount_unk = EMU_PSX; // failed to mount PSX CD as bd disc

					if(mount_unk) goto mount_again;
					mount_unk = EMU_BD;
				}

				sys_timer_usleep(2500);
				cobra_send_fake_disc_insert_event();
			}
		}
		else
		{
			int special_mode=0;

 #ifdef EXTRA_FEAT
			CellPadData pad_data = pad_read();

			if(pad_data.len > 0 && (pad_data.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_SELECT)) special_mode=true; //mount also app_home / eject disc
			sys_timer_usleep(10000);

			if(special_mode) eject_insert(1, 0);
 #endif //#ifdef EXTRA_FEAT

			// -- get TitleID from PARAM.SFO
 #ifndef FIX_GAME
			memset(titleID, 0, 10);

			char filename[MAX_PATH_LEN];
			sprintf(filename, "%s/PS3_GAME/PARAM.SFO", _path);
			getTitleID(filename, titleID, GET_TITLE_ID_ONLY);
 #else
			fix_game(_path, titleID, webman_config->fixgame);
 #endif //#ifndef FIX_GAME
			// ----

			// -- reset USB bus
			if(!webman_config->bus)
			{
				if(islike(_path, "/dev_usb") && isDir(_path))
				{
					reset_usb_ports(_path);
				}
			}

			// -- mount game folder
			if(titleID[0] && titleID[8]>='0')
				cobra_map_game(_path, titleID, &special_mode);
			else
				cobra_map_game(_path, (char*)"TEST00000", &special_mode);
		}
		//return;
	}
#endif //#ifdef COBRA_ONLY

patch:

#ifndef COBRA_ONLY

	if(c_firmware==0.0f) {ret = false; goto exit_mount;}

	install_peek_poke();

	if(do_eject) eject_insert(1, 1);

	pokeq(0x8000000000000000ULL+MAP_ADDR, 0x0000000000000000ULL);
	pokeq(0x8000000000000008ULL+MAP_ADDR, 0x0000000000000000ULL);

	if(cobra_mode) goto exit_mount;

	if(base_addr==0 || SYSCALL_TABLE == 0) {ret = false; goto exit_mount;}

	// restore syscall table
	{
		uint64_t sc_null = peekq(SYSCALL_TABLE);

		if(peekq(SYSCALL_PTR(79)) == sc_null)
		{
			for(u8 sc=35; sc<39; sc++)
				if(peekq(SYSCALL_PTR(sc)) != sc_null) pokeq(SYSCALL_PTR(sc), sc_null);
			//pokeq(SYSCALL_PTR(1023), sc_null);

			if(sc_600)
			{   // restore original values
				sc_600|= 0x8000000000000000ULL;
				sc_604|= 0x8000000000000000ULL;
				sc_142|= 0x8000000000000000ULL;

				if(peekq(SYSCALL_PTR(600)) != sc_600) pokeq(SYSCALL_PTR(600), sc_600); // sys_storage_open 600
				if(peekq(SYSCALL_PTR(604)) != sc_604) pokeq(SYSCALL_PTR(604), sc_604); // sys_storage_send_device_cmd 604
				if(peekq(SYSCALL_PTR(142)) != sc_142) pokeq(SYSCALL_PTR(142), sc_142); // sys_timer_sleep 142
			}
		}
	}

	// disable mM path table
	pokeq(0x8000000000000000ULL+MAP_ADDR, 0x0000000000000000ULL);
	pokeq(0x8000000000000008ULL+MAP_ADDR, 0x0000000000000000ULL);

	// disable Iris path table
	pokeq(0x80000000007FD000ULL,		  0x0000000000000000ULL);

	// restore hook used by all payloads)
	pokeq(open_hook + 0x00, 0xF821FF617C0802A6ULL);
	pokeq(open_hook + 0x08, 0xFB810080FBA10088ULL);
	pokeq(open_hook + 0x10, 0xFBE10098FB410070ULL);
	pokeq(open_hook + 0x18, 0xFB610078F80100B0ULL);
	pokeq(open_hook + 0x20, 0x7C9C23787C7D1B78ULL);

	// poke mM payload
	pokeq(base_addr + 0x00, 0x7C7D1B783B600001ULL);
	pokeq(base_addr + 0x08, 0x7B7BF806637B0000ULL | MAP_ADDR);
	pokeq(base_addr + 0x10, 0xEB5B00002C1A0000ULL);
	pokeq(base_addr + 0x18, 0x4D820020EBFB0008ULL);
	pokeq(base_addr + 0x20, 0xE8BA00002C050000ULL);
	pokeq(base_addr + 0x28, 0x418200CC7FA3EB78ULL);
	pokeq(base_addr + 0x30, 0xE89A001089640000ULL);
	pokeq(base_addr + 0x38, 0x892300005560063EULL);
	pokeq(base_addr + 0x40, 0x7F895800409E0040ULL);
	pokeq(base_addr + 0x48, 0x2F8000007CA903A6ULL);
	pokeq(base_addr + 0x50, 0x409E002448000030ULL);
	pokeq(base_addr + 0x58, 0x8964000089230000ULL);
	pokeq(base_addr + 0x60, 0x5560063E7F895800ULL);
	pokeq(base_addr + 0x68, 0x2F000000409E0018ULL);
	pokeq(base_addr + 0x70, 0x419A001438630001ULL);
	pokeq(base_addr + 0x78, 0x388400014200FFDCULL);
	pokeq(base_addr + 0x80, 0x4800000C3B5A0020ULL);
	pokeq(base_addr + 0x88, 0x4BFFFF98E89A0018ULL);
	pokeq(base_addr + 0x90, 0x7FE3FB7888040000ULL);
	pokeq(base_addr + 0x98, 0x2F80000098030000ULL);
	pokeq(base_addr + 0xA0, 0x419E00187C691B78ULL);
	pokeq(base_addr + 0xA8, 0x8C0400012F800000ULL);
	pokeq(base_addr + 0xB0, 0x9C090001409EFFF4ULL);
	pokeq(base_addr + 0xB8, 0xE8BA00087C632A14ULL);
	pokeq(base_addr + 0xC0, 0x7FA4EB78E8BA0000ULL);
	pokeq(base_addr + 0xC8, 0x7C842A1488040000ULL);
	pokeq(base_addr + 0xD0, 0x2F80000098030000ULL);
	pokeq(base_addr + 0xD8, 0x419E00187C691B78ULL);
	pokeq(base_addr + 0xE0, 0x8C0400012F800000ULL);
	pokeq(base_addr + 0xE8, 0x9C090001409EFFF4ULL);
	pokeq(base_addr + 0xF0, 0x7FFDFB787FA3EB78ULL);
	pokeq(base_addr + 0xF8, 0x4E8000204D4D504CULL); //blr + "MMPL"

	pokeq(MAP_BASE  + 0x00, 0x0000000000000000ULL);
	pokeq(MAP_BASE  + 0x08, 0x0000000000000000ULL);
	pokeq(MAP_BASE  + 0x10, 0x8000000000000000ULL);
	pokeq(MAP_BASE  + 0x18, 0x8000000000000000ULL);

	pokeq(0x8000000000000000ULL+MAP_ADDR, MAP_BASE);
	pokeq(0x8000000000000008ULL+MAP_ADDR, 0x80000000007FDBE0ULL);

	pokeq(open_hook + 0x20, (0x7C9C237848000001ULL | (base_addr-open_hook-0x24)));

	char expplg[128];
	char app_sys[128];

	char path[MAX_PATH_LEN];

 #ifdef EXT_GDATA

	//------------------
	// re-load last game
	//------------------

	if(do_eject == MOUNT_EXT_GDATA) // extgd
	{
		sprintf(_path, WMTMP "/last_game.txt"); int fd=0;
		if(cellFsOpen(_path, CELL_FS_O_RDONLY, &fd, NULL, 0) == CELL_FS_SUCCEEDED)
		{
			uint64_t read_e = 0;
			if(cellFsRead(fd, (void *)_path, MAX_PATH_LEN, &read_e) == CELL_FS_SUCCEEDED) _path[read_e] = NULL;
			cellFsClose(fd);
		}
		else
			_path[0] = NULL;
	}

 #endif //#ifdef EXT_GDATA

	//if(_path[0] && (strstr(_path, "/PS3_GAME/USRDIR/EBOOT.BIN")!=NULL)) _path[strlen(_path)-26] = NULL;

	sprintf(path, "%s", _path);

	if(!isDir(path)) _path[0] = path[0] = NULL;

	// -- get TitleID from PARAM.SFO
 #ifndef FIX_GAME
	char filename[MAX_PATH_LEN];

	sprintf(filename, "%s/PS3_GAME/PARAM.SFO", _path);
	getTitleID(filename, titleID, GET_TITLE_ID_ONLY);
 #else
	fix_game(_path, titleID, webman_config->fixgame);
 #endif //#ifndef FIX_GAME
	// ----

	//----------------------------------
	// map game to /dev_bdvd & /app_home
	//----------------------------------

	if(path[0])
	{
		if(do_eject)
		{
			add_to_map((char*)"/dev_bdvd", path);
			add_to_map((char*)"//dev_bdvd", path);

			char path2[strlen(_path)+24];

			sprintf(path2, "%s/PS3_GAME", _path);
			add_to_map((char*)"/app_home/PS3_GAME", path2);

			sprintf(path2, "%s/PS3_GAME/USRDIR", _path);
			add_to_map((char*)"/app_home/USRDIR", path2);

			sprintf(path2, "%s/PS3_GAME/USRDIR/", _path);
			add_to_map((char*)"/app_home/", path2);
		}

		add_to_map((char*)"/app_home", path);
	}

 #ifdef EXT_GDATA

	//--------------------------------------------
	// auto-map /dev_hdd0/game to dev_usbxxx/GAMEI
	//---------------------------------------------

	if(do_eject!=1) ;
	else if(strstr(_path, "/GAME"))
	{
		int fdd =0; char extgdfile[MAX_PATH_LEN];
		sprintf(extgdfile, "%s/PS3_GAME/PS3GAME.INI", _path);
		if(cellFsOpen(extgdfile, CELL_FS_O_RDONLY, &fdd, NULL, 0) == CELL_FS_SUCCEEDED)
		{
			uint64_t read_e = 0;
			if(cellFsRead(fdd, (void *)&extgdfile, 12, &read_e) == CELL_FS_SUCCEEDED) extgdfile[read_e] = NULL;
			cellFsClose(fdd);
			if((extgd == 0) &&  (extgdfile[10] & (1<<1))) set_gamedata_status(1, false); else
			if((extgd == 1) && !(extgdfile[10] & (1<<1))) set_gamedata_status(0, false);
		}
		else if(extgd) set_gamedata_status(0, false);
	}

 #endif

	sprintf(app_sys, MM_ROOT_STD "/sys");
	if(!isDir(app_sys))
		sprintf(app_sys, MM_ROOT_STL "/sys");
	if(!isDir(app_sys))
		sprintf(app_sys, MM_ROOT_SSTL "/sys");


	//----------------------------
	// Patched explore_plugin.sprx
	//----------------------------

	if(c_firmware==3.55f)
		sprintf(expplg, "%s/IEXP0_355.BIN", app_sys);
	else if(c_firmware == 4.21f)
		sprintf(expplg, "%s/IEXP0_420.BIN", app_sys);
	else if(c_firmware == 4.30f || c_firmware == 4.31f)
		sprintf(expplg, "%s/IEXP0_430.BIN", app_sys);
	else if(c_firmware == 4.40f || c_firmware == 4.41f)
		sprintf(expplg, "%s/IEXP0_440.BIN", app_sys);
	else if(c_firmware == 4.46f)
		sprintf(expplg, "%s/IEXP0_446.BIN", app_sys);
	else if(c_firmware == 4.50f || c_firmware == 4.53f || c_firmware == 4.55f)
		sprintf(expplg, "%s/IEXP0_450.BIN", app_sys);
	else if(c_firmware == 4.60f || c_firmware == 4.65f || c_firmware == 4.66f)
		sprintf(expplg, "%s/IEXP0_460.BIN", app_sys);
	else if(c_firmware == 4.70f || c_firmware == 4.75f || c_firmware == 4.76f || c_firmware == 4.78f)
		sprintf(expplg, "%s/IEXP0_470.BIN", app_sys);
	else if(c_firmware == 4.80f)
		sprintf(expplg, "%s/IEXP0_480.BIN", app_sys);
	else
		sprintf(expplg, "%s/none", app_sys);

	if(do_eject && file_exists(expplg))
		add_to_map( (char*)"/dev_flash/vsh/module/explore_plugin.sprx", expplg);


	//---------------
	// New libfs.sprx
	//---------------
	if((do_eject>0) && (c_firmware >= 4.20f) && file_exists(NEW_LIBFS_PATH))
		add_to_map((char*) ORG_LIBFS_PATH, (char*)NEW_LIBFS_PATH);

	//-----------------------------------------------//
	uint64_t map_data  = (MAP_BASE);
	uint64_t map_paths = (MAP_BASE) + (max_mapped + 1) * 0x20;

	for(u16 n = 0; n < 0x400; n += 8) pokeq(map_data + n, 0);

	if(!max_mapped) {ret = false; goto exit_mount;}

	for(u8 n = 0; n < max_mapped; n++)
	{
		if(map_paths > 0x80000000007FE800ULL) break;
		pokeq(map_data + (n * 0x20) + 0x10, map_paths);
		string_to_lv2(file_to_map[n].src, map_paths);
		map_paths += (strlen(file_to_map[n].src)+8)&0x7f8;

		pokeq(map_data + (n * 0x20) + 0x18, map_paths);
		string_to_lv2(file_to_map[n].dst, map_paths);
		map_paths += (strlen(file_to_map[n].dst)+8)&0x7f8;

		pokeq(map_data + (n * 0x20) + 0x08, strlen(file_to_map[n].dst));
		pokeq(map_data + (n * 0x20) + 0x00, strlen(file_to_map[n].src));
	}

	if(isDir("/dev_bdvd")) sys_timer_sleep(2);

	//if(do_eject) eject_insert(0, 1);
#endif //#ifndef COBRA_ONLY

exit_mount:

	if(ret)
	{
		// show loaded path
		char path2[MAX_PATH_LEN];
		char temp[MAX_PATH_LEN];
		sprintf(path2, "\"%s", (strrchr(_path, '/') + 1));
		if(strstr(path2, ".ntfs[")) path2[strrchr(path2, '.')-path2] = NULL;
		if(strrchr(path2, '.')!=NULL) path2[strrchr(path2, '.')-path2] = NULL;
		if(path2[1]==NULL) sprintf(path2, "\"%s", _path);
		sprintf(temp, "\" %s", STR_LOADED2); strcat(path2, temp);
		show_msg(path2);
	}

	delete_history(false);

	if(mount_unk == EMU_PSP) {is_mounting = false; return ret;}

	// wait few seconds until the game is mounted
	if(ret && extcmp(_path, ".BIN.ENC", 8))
	{
		waitfor("/dev_bdvd", (islike(_path, "/dev_hdd0") ? 6 : islike(_path, "/net") ? 20 : 15));
		if(!isDir("/dev_bdvd")) ret = false;
	}

#ifdef FIX_GAME
	// re-check PARAM.SFO to notify if game needs to be fixed
	if(ret && (c_firmware<LATEST_CFW))
	{
		char filename[64];
		sprintf(filename, "/dev_bdvd/PS3_GAME/PARAM.SFO");
		getTitleID(filename, titleID, GET_TITLE_ID_ONLY);

		// check update folder
		sprintf(filename, "%s%s%s", HDD0_GAME_DIR, titleID, "/PARAM.SFO");

		if(file_exists(filename) == false)
			sprintf(filename, "/dev_bdvd/PS3_GAME/PARAM.SFO");

		getTitleID(filename, titleID, SHOW_WARNING);
	}
#endif

	if(!ret && !isDir("/dev_bdvd")) {char msg[MAX_PATH_LEN]; sprintf(msg, "%s %s", STR_ERROR, _path); show_msg(msg);}
#ifdef REMOVE_SYSCALLS
	else
	{
		CellPadData pad_data = pad_read();
		bool otag = (strcasestr(_path, "[online]")!=NULL);
		bool r2 = (pad_data.len>0 && (pad_data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_R2));
		if((!r2 && otag) || (r2 && !otag)) disable_cfw_syscalls(webman_config->keep_ccapi);
	}
#endif

#ifdef COBRA_ONLY
 #ifdef EXT_GDATA
	if((extgd == 0) && isDir("/dev_bdvd/GAMEI")) set_gamedata_status(2, true); // auto-enable external gameDATA (if GAMEI exists on /bdvd)
 #endif
	{
		if(ret && (strstr(_path, ".PUP.ntfs[BD") || file_exists("/dev_bdvd/PS3UPDAT.PUP")))
			sys_map_path((char*)"/dev_bdvd/PS3/UPDATE", (char*)"/dev_bdvd"); //redirect root of bdvd to /dev_bdvd/PS3/UPDATE

		if(ret && (islike(_path, "/net") && isDir("/dev_bdvd/PKG")))
			sys_map_path((char*)"/app_home", (char*)"/dev_bdvd/PKG"); //redirect net_host/PKG to app_home

		{sys_map_path((char*)"/dev_bdvd/PS3_UPDATE", (char*)SYSMAP_PS3_UPDATE);} //redirect firmware update to empty folder

		is_mounting = false;

		{ PS3MAPI_DISABLE_ACCESS_SYSCALL8 }
	}
#else
	is_mounting = false;
#endif

	max_mapped = 0;
	return ret;
}
