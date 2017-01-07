#define MAX_LAST_GAMES			(5)
#define LAST_GAMES_UPPER_BOUND	(4)

// File name TAGS:
// [auto]    Auto-play
// [online]  Auto-disable syscalls
// [offline] Auto-disable network
// [gd]      Auto-enable external gameDATA
// [raw]     Use raw_iso.sprx to mount the ISO (ntfs)
// [PS2]     PS2 extracted folders in /PS2DISC (needs PS2_DISC compilation flag)
// [netemu]  Mount ps2/psx game with netemu


char map_title_id[10];

typedef struct
{
	uint8_t last;
	char game[MAX_LAST_GAMES][MAX_PATH_LEN];
} __attribute__((packed)) _lastgames;

#define IS_COPY		9

// /mount_ps3/<path>[?random=<x>[&emu={ ps1_netemu.self / ps1_emu.self / ps2_netemu.self / ps2_emu.self }][offline={0/1}]
// /mount.ps3/<path>[?random=<x>[&emu={ ps1_netemu.self / ps1_emu.self / ps2_netemu.self / ps2_emu.self }][offline={0/1}]
// /mount.ps3/unmount
// /mount.ps2/<path>[?random=<x>]
// /mount.ps2/unmount
// /copy.ps3/<path>[&to=<destination>]

#ifdef COPY_PS3
u8 usb = 1; // first connected usb drive [used by /copy.ps3 & in the tooltips for /copy.ps3 links in the file manager]. 1 = /dev_usb000
#endif

static void game_mount(char *buffer, char *templn, char *param, char *tempstr, bool mount_ps3, bool forced_mount)
{

	// ---------------------
	// unmount current game
	// ---------------------
	if(strstr(param, "ps3/unmount") || strstr(param, "ps3/dev_bdvd") || strstr(param, "ps3/app_home"))
	{
		do_umount(true);

		strcat(buffer, STR_GAMEUM);
	}

	// -----------------
	// unmount ps2_disc
	// -----------------
#ifdef PS2_DISC
	else if(strstr(param, "ps2/unmount"))
	{
		do_umount_ps2disc(false);

		strcat(buffer, STR_GAMEUM);
	}
#endif

	// -----------------------
	// mount game / copy file
	// -----------------------
	else
	{
		// ---------------
		// init variables
		// ---------------
		uint8_t plen = 10; // /mount.ps3
		enum icon_type default_icon = iPS3;

#ifdef COPY_PS3
		char target[MAX_PATH_LEN], *pos; *target = NULL;
		if(islike(param, "/copy.ps3")) {plen = IS_COPY; pos = strstr(param, "&to="); if(pos) {strcpy(target, pos + 4); *pos = NULL;}}
		bool is_copy = ((plen == IS_COPY) && (copy_in_progress == false));
#endif
		char enc_dir_name[1024], *source = param + plen;
		bool mounted = false; max_mapped = 0;
		bool is_gamei = false;

		// ----------------------------
		// remove url query parameters
		// ----------------------------
		char *purl = strstr(source, "emu="); // e.g. ?emu=ps1_netemu.self / ps1_emu.self / ps2_netemu.self / ps2_emu.self
		if(purl)
		{
			char *is_netemu = strstr(purl, "net");
			if(strcasestr(source, "ps2"))
				webman_config->ps2emu = is_netemu ? 1 : 0;
			else
				webman_config->ps1emu = is_netemu ? 1 : 0;
			purl--, *purl = NULL;
		}

#ifdef OFFLINE_INGAME
		purl = strstr(source, "offline=");
		if(purl) net_status = (*(purl + 8) == '0') ? 1 : 0;
#endif
		purl = strstr(source, "?random=");
		if(purl) *purl = NULL;

		// -------------------------
		// use relative source path
		// -------------------------
		if(file_exists(source) == false) {sprintf(templn, "%s/%s", html_base_path, source + 1); if(file_exists(templn)) sprintf(source, "%s", templn);}

		// --------------
		// set mount url
		// --------------
		urlenc(templn, source);

		// -----------
		// mount game
		// -----------
#ifdef COPY_PS3
		if(!is_copy)
#endif
		{
			char *p = strstr(param, "/PS3_"); if(p) *p = NULL;

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

#ifdef PKG_LAUNCHER
			is_gamei = strstr(param, "/GAMEI/");
#endif
		}

		// -----------------
		// auto-play & exit
		// -----------------
		if(mount_ps3)
		{
#ifdef OFFLINE_INGAME
			if(mounted && (strstr(param, OFFLINE_TAG) != NULL)) net_status = 0;
#endif
			if(mounted && IS_ON_XMB && (strstr(param, "/PSPISO") == NULL) && (extcmp(param, ".BIN.ENC", 8) != 0))
			{
				uint8_t autoplay = webman_config->autoplay;

				CellPadData pad_data = pad_read();
				bool atag = (strcasestr(param, AUTOPLAY_TAG)!=NULL) || (autoplay);
 #ifdef REMOVE_SYSCALLS
				bool l2 = (pad_data.len > 0 && (pad_data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_L2));
 #else
				bool l2 = (pad_data.len > 0 && (pad_data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & (CELL_PAD_CTRL_L2 | CELL_PAD_CTRL_R2)));
 #endif

 #if defined(FAKEISO) || defined(PKG_LAUNCHER)
				int view = View_Find("explore_plugin");
				if(view) explore_interface = (explore_plugin_interface *)plugin_GetInterface(view, 1);
 #endif

 #ifdef PKG_LAUNCHER
				if(is_gamei)
				{
					if(!(webman_config->nogrp) && webman_config->ps3l && (view != 0))
					{
						explore_interface->ExecXMBcommand("focus_index pkg_launcher", 0, 0);
						explore_exec_push(200000, true); // open pkg_launcher folder
					}
				}
				else
 #endif
 #ifdef FAKEISO
				if(!l2 && !extcmp(param, ".ntfs[BDFILE]", 13))
				{
					if(!(webman_config->nogrp) && webman_config->rxvid && (view != 0))
					{
						if(strcasestr(param, ".pkg"))
						{
							explore_interface->ExecXMBcommand("close_all_list", 0, 0);
							explore_interface->ExecXMBcommand("focus_segment_index seg_package_files", 0, 0);
						}
						else
						{
							explore_interface->ExecXMBcommand("focus_index rx_video", 0, 0);
							explore_exec_push(200000, true);  // open rx_video folder

							if(!autoplay || strcasestr(param, ".mkv")) {is_busy = false; return;}

							explore_exec_push(2000000, true); // open Data Disc
						}
					}
				}
				else
 #endif
				{
					char category[16], seg_name[40]; *category = *seg_name = NULL;
					if((atag && !l2) || (!atag && l2)) {sys_timer_sleep(1); launch_disc(category, seg_name);} // L2 + X

					autoplay = false;
				}

				if(autoplay)
				{
					explore_exec_push(2000000, false);
				}
			}

			is_busy = false;
			return;
		}

		/////////////////
		// show result //
		/////////////////
		else if(*source == '/')
		{
			char _path[MAX_PATH_LEN];
			size_t slen = 0;

			// ----------------
			// set mount label
			// ----------------

			if(islike(templn, "/net"))
			{
				utf8enc(_path, source, 0);
				slen = strlen(source);
			}
			else
			{
				slen = sprintf(_path, "%s", source);
			}

			// -----------------
			// get display icon
			// -----------------
			char *filename = strrchr(_path, '/'), *icon = tempstr;
			{
				char tempstr[_4KB_], tempID[10], *d_name; *icon = *tempID = NULL;
				u8 f0 = strstr(filename, ".ntfs[") ? NTFS : 0, f1 = strstr(_path, "PS2") ? 5 : strstr(_path, "PSX") ? 6 : strstr(_path, "PSP") ? 8 : 2, is_dir = isDir(source);

				check_cover_folders(templn);

				// get iso name
				*filename = NULL; // sets _path
				d_name = filename + 1;

				if(is_dir)
				{
					sprintf(templn, "%s/%s/PS3_GAME/PARAM.SFO", _path, d_name);
					get_title_and_id_from_sfo(templn, tempID, d_name, icon, tempstr, 0); f1 = 0;
				}
#ifdef COBRA_ONLY
				else
				{
					get_name_iso_or_sfo(templn, tempID, icon, _path, d_name, f0, f1, FROM_MOUNT, strlen(d_name), tempstr);
				}
#endif
				default_icon = get_default_icon(icon, _path, d_name, is_dir, tempID, NONE, 0, f0, f1);

				*filename = '/';
			}

			urlenc(enc_dir_name, icon);
			htmlenc(_path, source, 0);

			// ----------------
			// set target path
			// ----------------
#ifdef COPY_PS3
			if(plen == IS_COPY)
			{
				bool is_copying_from_hdd = islike(source, "/dev_hdd0");

				usb = get_default_usb_drive(0);

				#ifdef USE_UACCOUNT
				if(!webman_config->uaccount[0])
				#endif
					sprintf(webman_config->uaccount, "%08i", xsetting_CC56EB2D()->GetCurrentUserNumber());

				if(cp_mode)
				{
					sprintf(target, "%s", cp_path);
				}
				else
				if(*target) {if(!isDir(source) && isDir(target)) strcat(target, filename);} // &to=<destination>
				else
				{
					char *ext4 = source + slen - 4;
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
								if(IS(ext4, ".dex"))
									sprintf(target, "%s/stage2.dex", "/dev_flash/rebug/cobra");
								else if(IS(ext4, ".cex"))
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
										if(peek_lv1(addr) == 0x2F6F732F6C76325FULL)             // /os/lv2_
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
									save_file(WMNOSCAN, NULL, 0);
									{system_call_3(SC_SYS_POWER, SYS_REBOOT, NULL, 0);} /*load LPAR id 1*/
									sys_ppu_thread_exit(0);
								}
							}
						}
						plen = 0; //do not copy
					}
					else
 #endif // #ifdef SWAP_KERNEL
					if(strstr(source, "/***PS3***/"))
					{
						sprintf(target, "/dev_hdd0/PS3ISO%s.iso", filename); // /copy.ps3/net0/***PS3***/GAMES/BLES12345  -> /dev_hdd0/PS3ISO/BLES12345.iso
					}
					else
					if(strstr(source, "/***DVD***/"))
					{
						sprintf(target, "/dev_hdd0/DVDISO%s.iso", filename); // /copy.ps3/net0/***DVD***/folder  -> /dev_hdd0/DVDISO/folder.iso
					}
					else if(IS(ext4, ".pkg"))
					{
						if(is_copying_from_hdd)
							sprintf(target, "%s/Packages", drives[usb]);
						else
							sprintf(target, "/dev_hdd0/packages");

						strcat(target, filename);
					}
					else if(_IS(ext4, ".bmp") || _IS(ext4, ".gif"))
					{
						if(is_copying_from_hdd)
							sprintf(target, "%s/PICTURE", drives[usb]);
						else
							sprintf(target, "%s/PICTURE", "/dev_hdd0");

						strcat(target, filename);
					}
					else if(_IS(ext4, ".jpg") || _IS(ext4, ".png"))
					{
						if(is_copying_from_hdd)
							sprintf(target, "%s/PICTURE", drives[usb]);
						else if(strstr(source, "BL") || strstr(param, "BC") || strstr(source, "NP"))
							sprintf(target, "/dev_hdd0/GAMES/covers");
						else
							sprintf(target, "%s/PICTURE", "/dev_hdd0");

						strcat(target, filename);
					}
					else if(strcasestr(source, "/covers"))
					{
						if(is_copying_from_hdd)
							sprintf(target, "%s/COVERS", drives[usb]);
						else
							sprintf(target, "/dev_hdd0/GAMES/covers");
					}
					else if(_IS(ext4, ".mp4") || _IS(ext4, ".mkv") || _IS(ext4, ".avi"))
					{
						if(is_copying_from_hdd)
							sprintf(target, "%s/VIDEO", drives[usb]);
						else
							sprintf(target, "/dev_hdd0/VIDEO");

						strcat(target, filename);
					}
					else if(_IS(ext4, ".mp3"))
					{
						if(is_copying_from_hdd)
							sprintf(target, "%s/MUSIC", drives[usb]);
						else
							sprintf(target, "%s/MUSIC", "/dev_hdd0");

						strcat(target, filename);
					}
					else if(IS(ext4, ".p3t"))
					{
						if(is_copying_from_hdd)
							sprintf(target, "%s/PS3/THEME", drives[usb]);
						else
							sprintf(target, "/dev_hdd0/theme");

						strcat(target, filename);
					}
					else if(!extcmp(source, ".edat", 5))
					{
						if(is_copying_from_hdd)
							sprintf(target, "%s/exdata", drives[usb]);
						else
							sprintf(target, "%s/%s/exdata", "/dev_hdd0/home", webman_config->uaccount);

						strcat(target, filename);
					}
					else if(IS(ext4, ".rco") || strstr(source, "/coldboot"))
					{
						enable_dev_blind(NO_MSG);
						sprintf(target, "/dev_blind/vsh/resource");

						if(IS(ext4, ".raf"))
							strcat(target, "/coldboot.raf");
						else
							strcat(target, filename);
					}
					else if(IS(ext4, ".qrc"))
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
							sprintf(target, "%s/exdata", drives[usb]);
						else
							sprintf(target, "%s/%s/exdata", "/dev_hdd0/home", webman_config->uaccount);
					}
					else if(strstr(source, "/PS3/THEME"))
						sprintf(target, "/dev_hdd0/theme");
					else if(strcasestr(source, "/savedata/"))
					{
						if(is_copying_from_hdd)
							sprintf(target, "%s/PS3/SAVEDATA", drives[usb]);
						else
							sprintf(target, "%s/%s/savedata", "/dev_hdd0/home", webman_config->uaccount);

						strcat(target, filename);
					}
					else if(strcasestr(source, "/trophy/"))
					{
						if(is_copying_from_hdd)
							sprintf(target, "%s/PS3/TROPHY", drives[usb]);
						else
							sprintf(target, "%s/%s/trophy", "/dev_hdd0/home", webman_config->uaccount);

						strcat(target, filename);
					}
					else if(strstr(source, "/webftp_server"))
					{
						sprintf(target, "%s/webftp_server.sprx",         "/dev_hdd0/plugins"); if(file_exists(target) == false)
						sprintf(target, "%s/webftp_server_ps3mapi.sprx", "/dev_hdd0/plugins"); if(file_exists(target) == false)
						sprintf(target, "%s/webftp_server.sprx",         "/dev_hdd0");         if(file_exists(target) == false)
						sprintf(target, "%s/webftp_server_ps3mapi.sprx", "/dev_hdd0");
					}
					else if(strstr(source, "/boot_plugins_"))
						sprintf(target, "/dev_hdd0/boot_plugins.txt");
					else if(is_copying_from_hdd)
						sprintf(target, "%s%s", drives[usb], source + 9);
					else if(islike(source, "/dev_usb"))
						sprintf(target, "%s%s", "/dev_hdd0", source + 11);
					else if(islike(source, "/net"))
						sprintf(target, "%s%s", "/dev_hdd0", source + 5);
					else
					{
						if(islike(source, "/dev_bdvd"))
						{
							{system_call_1(36, (uint64_t) "/dev_bdvd");} // decrypt dev_bdvd files

							sprintf(target, "%s/%s", "/dev_hdd0/GAMES", "My Disc Backup");

							char title[80];
							sprintf(title, "/dev_bdvd/PS3_GAME/PARAM.SFO");
							if(file_exists(title))
							{
								char titleid[10];
								getTitleID(title, titleid, GET_TITLE_AND_ID);
								if(*titleid && (titleid[8] >= '0'))
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
				}

				// ------------------
				// show copying file
				// ------------------
				filepath_check(target);

				bool is_error = ((islike(target, drives[usb]) && isDir(drives[usb]) == false)) || islike(target, source) || !sys_admin;

				// show source path
				sprintf(tempstr, "%s ", STR_COPYING); strcat(buffer, tempstr);
				add_breadcrumb_trail(buffer, source); strcat(buffer, "<hr>");

				// show image
				urlenc(_path, target);
				sprintf(tempstr, "<a href=\"%s\"><img src=\"%s\" border=0></a><hr>%s %s: ",
								 _path, enc_dir_name, is_error ? STR_ERROR : "", STR_CPYDEST); strcat(buffer, tempstr);

				// show target path
				add_breadcrumb_trail(buffer, target); *tempstr = NULL;

				if(strstr(target, "/webftp_server")) {sprintf(tempstr, "<HR>%s", STR_SETTINGSUPD);} else
				if(cp_mode) {char *p = strrchr(_path, '/'); *p = NULL; sprintf(tempstr, HTML_REDIRECT_TO_URL, _path, HTML_REDIRECT_WAIT);}

				if(is_error) {show_msg((char*)STR_CPYABORT); cp_mode = CP_MODE_NONE; return;}
			}
			else
#endif // #ifdef COPY_PS3

			// ------------------
			// show mounted game
			// ------------------
			{
#ifndef ENGLISH_ONLY
				char STR_GAMETOM[48];//		= "Game to mount";
				char STR_GAMELOADED[288];//	= "Game loaded successfully. Start the game from the disc icon<br>or from <b>/app_home</b>&nbsp;XMB entry.</a><hr>Click <a href=\"/mount.ps3/unmount\">here</a> to unmount the game.";
				char STR_PSPLOADED[232]; //	= "Game loaded successfully. Start the game using <b>PSP Launcher</b>.<hr>";
				char STR_PS2LOADED[240]; //	= "Game loaded successfully. Start the game using <b>PS2 Classic Launcher</b>.<hr>";

				char STR_MOVIETOM[48];//	= "Movie to mount";
				char STR_MOVIELOADED[272];//= "Movie loaded successfully. Start the movie from the disc icon<br>under the Video column.</a><hr>Click <a href=\"/mount.ps3/unmount\">here</a> to unmount the movie.";

				sprintf(STR_PSPLOADED,   "Game %s%s%s</b>.<hr>",
										 "loaded successfully. Start the ", "game using <b>", "PSP Launcher");
				sprintf(STR_PS2LOADED,   "Game %s%s%s</b>.<hr>",
										 "loaded successfully. Start the ", "game using <b>", "PS2 Classic Launcher");
				sprintf(STR_GAMELOADED,  "Game %s%s%sgame.",
										 "loaded successfully. Start the ", "game from the disc icon<br>or from <b>/app_home</b>&nbsp;XMB entry", ".</a><hr>Click <a href=\"/mount.ps3/unmount\">here</a> to unmount the ");
				sprintf(STR_MOVIELOADED, "Movie %s%s%smovie.",
										 "loaded successfully. Start the ", "movie from the disc icon<br>under the Video column"                , ".</a><hr>Click <a href=\"/mount.ps3/unmount\">here</a> to unmount the ");

				language("STR_GAMETOM", STR_GAMETOM, "Game to mount");
				language("STR_GAMELOADED", STR_GAMELOADED, STR_GAMELOADED);
				language("STR_PSPLOADED", STR_PSPLOADED, STR_PSPLOADED);
				language("STR_PS2LOADED", STR_PS2LOADED, STR_PS2LOADED);

				language("STR_MOVIETOM", STR_MOVIETOM, "Movie to mount");
				language("STR_MOVIELOADED", STR_MOVIELOADED, STR_MOVIELOADED);

				language("/CLOSEFILE", NULL, NULL);
#endif
				bool is_movie = strstr(param, "/BDISO") || strstr(param, "/DVDISO") || !extcmp(param, ".ntfs[BDISO]", 12) || !extcmp(param, ".ntfs[DVDISO]", 13);
				strcat(buffer, is_movie ? STR_MOVIETOM : STR_GAMETOM); strcat(buffer, ": "); add_breadcrumb_trail(buffer, source);

				//if(strstr(param, "/PSX")) {sprintf(tempstr, " <font size=2>[CD %i • %s]</font>", CD_SECTOR_SIZE_2352, (webman_config->ps1emu) ? "ps1_netemu.self" : "ps1_emu.self"); strcat(buffer, tempstr);}
#ifdef PKG_LAUNCHER
				if(is_gamei)
				{
					char *pos = strstr(STR_PSPLOADED, "PSP Launcher"); if(pos) strcpy(pos, "PKG Launcher");
				}
#endif
				if(is_movie)
					sprintf(tempstr, "<hr><a href=\"/play.ps3\"><img src=\"%s\" onerror=\"this.src='%s';\" border=0></a>"
									 "<hr><a href=\"/dev_bdvd\">%s</a>", enc_dir_name, wm_icons[strstr(param,"BDISO") ? iBDVD : iDVD], mounted ? STR_MOVIELOADED : STR_ERROR);
				else if(!extcmp(param, ".BIN.ENC", 8))
					sprintf(tempstr, "<hr><img src=\"%s\" onerror=\"this.src='%s';\" height=%i>"
									 "<hr>%s", enc_dir_name, wm_icons[iPS2], 300, mounted ? STR_PS2LOADED : STR_ERROR);
				else if(((strstr(param, "/PSPISO") || strstr(param, "/ISO/")) && !extcasecmp(param, ".iso", 4)) || is_gamei)
					sprintf(tempstr, "<hr><img src=\"%s\" onerror=\"this.src='%s';\" height=%i>"
									 "<hr>%s", enc_dir_name, wm_icons[iPSP], strcasestr(enc_dir_name,".png") ? 200 : 300, mounted ? STR_PSPLOADED : STR_ERROR);
				else
					sprintf(tempstr, "<hr><a href=\"/play.ps3\"><img src=\"%s\" onerror=\"this.src='%s';\" border=0></a>"
									 "<hr><a href=\"/dev_bdvd\">%s</a>", enc_dir_name, wm_icons[default_icon], mounted ? STR_GAMELOADED : STR_ERROR);
			}

			strcat(buffer, tempstr);

			// ----------------------------
			// show associated [PS2] games
			// ----------------------------
#ifdef PS2_DISC
			if(mounted && (strstr(source, "/GAME") || strstr(source, "/PS3ISO") || strstr(source, ".ntfs[PS3ISO]")))
			{
				CellFsDirent entry; u64 read_e;
				int fd2; u16 pcount = 0; u32 tlen = strlen(buffer) + 8; u8 is_iso = 0;

				sprintf(target, "%s", source);
				if(strstr(target, "Sing"))
				{
					if(strstr(target, "/PS3ISO")) {strcpy(strstr(target, "/PS3ISO"), "/PS2DISC\0"); is_iso = 1;}
					if(strstr(target, ".ntfs[PS3ISO]")) {strcpy(target, "/dev_hdd0/PS2DISC\0"); is_iso = 1;}
				}

				// -----------------------------
				// get [PS2] extracted folders
				// -----------------------------
				if(cellFsOpendir(target, &fd2) == CELL_FS_SUCCEEDED)
				{
					while((cellFsReaddir(fd2, &entry, &read_e) == CELL_FS_SUCCEEDED) && (read_e > 0))
					{
						if((entry.d_name[0] == '.')) continue;

						if(is_iso || strstr(entry.d_name, "[PS2") != NULL)
						{
							if(pcount == 0) strcat(buffer, "<br><HR>");
							urlenc(enc_dir_name, entry.d_name);
							tlen += sprintf(templn, "<a href=\"/mount.ps2%s/%s\">%s</a><br>", target, enc_dir_name, entry.d_name);

							if(tlen > (BUFFER_SIZE - _2KB_)) break;
							strcat(buffer, templn); pcount++;
						}
					}
					cellFsClosedir(fd2);
				}
			}
#endif // #ifdef PS2_DISC
		}

		// -------------
		// perform copy
		// -------------
#ifdef COPY_PS3
		if(sys_admin && is_copy)
		{
			if(islike(target, source) || ((!islike(source, "/net")) && file_exists(source) == false) )
				{sprintf(templn, "<hr>%s", STR_ERROR); strcat(buffer, templn);}
			else
			{
				setPluginActive();

				// show msg begin
				sprintf(templn, "%s %s\n%s %s", STR_COPYING, source, STR_CPYDEST, target);
				show_msg(templn);
				copy_in_progress = true, copied_count = 0;

				if(islike(target, "/dev_blind")) enable_dev_blind(NO_MSG);

				// make target dir tree
				mkdir_tree(target);

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
				{
					show_msg((char*)STR_CPYFINISH);
					if(do_restart) { { DELETE_TURNOFF } { BEEP2 } vsh_reboot();}
				}

				setPluginInactive();
			}

			if(!copy_aborted && (cp_mode == CP_MODE_MOVE) && file_exists(target)) del(source, true);
			if(cp_mode) {cp_mode = CP_MODE_NONE, *cp_path = NULL;}
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
		waitfor("/dev_bdvd", 1);
		cobra_disc_auth();
	}
}
#endif

static void do_umount(bool clean)
{
	if(clean) cellFsUnlink(WMTMP "/last_game.txt");

	cellFsUnlink("/dev_hdd0/tmp/game/ICON0.PNG");

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

		sys_map_path("/dev_bdvd", NULL);
		sys_map_path("//dev_bdvd", NULL);

		sys_map_path("/app_home", isDir("/dev_hdd0/packages") ? (char*)"/dev_hdd0/packages" : NULL);

		sys_map_path("/dev_bdvd/PS3/UPDATE", NULL);

 #ifdef PKG_LAUNCHER
		if(*map_title_id)
		{
			char gamei_mapping[32];
			sprintf(gamei_mapping, "/dev_hdd0/game/%s", map_title_id);
			sys_map_path(gamei_mapping, NULL);
			sys_map_path("/dev_hdd0/game/PKGLAUNCH", NULL);
		}
 #endif
		{
			sys_ppu_thread_t t_id;
			uint64_t exit_code;

 #ifndef LITE_EDITION
			sys_ppu_thread_create(&t_id, netiso_stop_thread, NULL, THREAD_PRIO_STOP, THREAD_STACK_SIZE_8KB, SYS_PPU_THREAD_CREATE_JOINABLE, STOP_THREAD_NAME);
			sys_ppu_thread_join(t_id, &exit_code);
 #endif
			sys_ppu_thread_create(&t_id, rawseciso_stop_thread, NULL, THREAD_PRIO_STOP, THREAD_STACK_SIZE_8KB, SYS_PPU_THREAD_CREATE_JOINABLE, STOP_THREAD_NAME);
			sys_ppu_thread_join(t_id, &exit_code);
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
		pokeq(0x8000000000000000ULL + MAP_ADDR, 0x0000000000000000ULL);
		pokeq(0x8000000000000008ULL + MAP_ADDR, 0x0000000000000000ULL);

		//eject_insert(1, 1);

		if(isDir("/dev_flash/pkg"))
			mount_with_mm((char*)"/dev_flash/pkg", 0);
	}

#endif //#ifdef COBRA_ONLY
}

#ifdef COBRA_ONLY
static void do_umount_eject(void)
{
	do_umount(false);

	sys_timer_usleep(4000);

	cobra_send_fake_disc_eject_event();

	sys_timer_usleep(4000);
}
#endif //#ifdef COBRA_ONLY

static void get_last_game(char *last_path)
{
	read_file(WMTMP "/last_game.txt", last_path, MAX_PATH_LEN, 0);
}

#ifdef COBRA_ONLY
static void cache_file_to_hdd(char *source, char *target, const char *basepath, char *msg)
{
	if(*source == '/')
	{
		sprintf(target, "/dev_hdd0%s", basepath);
		cellFsMkdir(basepath, MODE);

		strcat(target, strrchr(source, '/')); // add file name

		if((copy_in_progress || fix_in_progress) == false && file_exists(target) == false)
		{
			sprintf(msg, "%s %s\n"
						 "%s %s", STR_COPYING, source, STR_CPYDEST, basepath);
			show_msg(msg);

			copy_in_progress = true, copied_count = 1;
			file_copy(source, target, COPY_WHOLE_FILE);
			copy_in_progress = false;

			if(copy_aborted)
			{
				cellFsUnlink(target);
				show_msg((char*)STR_CPYABORT);
			}
		}

		if(file_exists(target)) strcpy(source, target);
	}

	do_umount(false);
}

static void cache_icon0_and_param_sfo(char *destpath)
{
	waitfor("/dev_bdvd", 15);

	char *ext = destpath + strlen(destpath);
	strcat(ext, ".SFO\0");

	// cache PARAM.SFO
	if(file_exists(destpath) == false)
	{
		for(u8 n = 0; n < 10; n++)
		{
			if(file_copy("/dev_bdvd/PS3_GAME/PARAM.SFO", destpath, _4KB_) >= CELL_FS_SUCCEEDED) break;
			sys_timer_usleep(500000);
		}
	}

	// cache ICON0.PNG
	*ext = NULL; strcat(ext, ".PNG");
	if((webman_config->nocov!=2) && file_exists(destpath) == false)
	{
		for(u8 n = 0; n < 10; n++)
		{
			if(file_copy("/dev_bdvd/PS3_GAME/ICON0.PNG", destpath, COPY_WHOLE_FILE) >= CELL_FS_SUCCEEDED) break;
			sys_timer_usleep(500000);
		}
	}
}
#endif

static void mount_autoboot(void)
{
	char path[MAX_PATH_LEN];

	// get autoboot path
	if(webman_config->autob &&
		((cobra_mode && islike(webman_config->autoboot_path, "/net")) || islike(webman_config->autoboot_path, "http") || file_exists(webman_config->autoboot_path))) // autoboot
		strcpy(path, (char *) webman_config->autoboot_path);
	else if(webman_config->lastp)
	{
		get_last_game(path);
	}
	else return;

	bool do_mount = false;

	CellPadData pad_data = pad_read();

	// prevent auto-mount on startup if L2+R2 is pressed
	if(pad_data.len > 0 && (pad_data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] == (CELL_PAD_CTRL_L2 | CELL_PAD_CTRL_R2))) { BEEP2; return;}

	if(from_reboot && *path && (strstr(path, "/PS2") != NULL)) return; //avoid re-launch PS2 returning to XMB

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

	// --------------------------------------------
	// show message if syscalls are fully disabled
	// --------------------------------------------
#ifdef COBRA_ONLY

	if(syscalls_removed || peekq(TOC) == SYSCALLS_UNAVAILABLE)
	{
		syscalls_removed = true;
		{ PS3MAPI_ENABLE_ACCESS_SYSCALL8 }

		int ret_val = NONE; { system_call_2(SC_COBRA_SYSCALL8, SYSCALL8_OPCODE_PS3MAPI, PS3MAPI_OPCODE_PCHECK_SYSCALL8); ret_val = (int)p1;}

		if(ret_val < 0) { show_msg((char*)STR_CFWSYSALRD); { PS3MAPI_DISABLE_ACCESS_SYSCALL8 } return false; }
		if(ret_val > 1) { system_call_3(SC_COBRA_SYSCALL8, SYSCALL8_OPCODE_PS3MAPI, PS3MAPI_OPCODE_PDISABLE_SYSCALL8, 1); }
	}

#else

	if(syscalls_removed || peekq(TOC) == SYSCALLS_UNAVAILABLE) { show_msg(STR_CFWSYSALRD); return false; }

#endif

	is_mounting = true;

	// -----------------
	// fix mount errors
	// -----------------

	if(fan_ps2_mode) reset_fan_mode();

	// -----------------------------
	// fix mount errors (patch lv2)
	// -----------------------------

	patch_lv2();


	// ---------------------------------------
	// exit if mounting a path from /dev_bdvd
	// ---------------------------------------

	if(islike(_path0, "/dev_bdvd")) {do_umount(false); is_mounting = false; return false;}


	// ---------------
	// init variables
	// ---------------

	char _path[MAX_PATH_LEN], titleID[10];

	bool ret = true;

	u8 mount_unk = EMU_OFF;

	led(GREEN, BLINK_FAST);

	// ----------------
	// open url & exit
	// ----------------
	if(islike(_path0 + 1, "http") || islike(_path0, "http") || !extcmp(_path0, ".htm", 4))
	{
		char *url = strstr(_path0, "http");

		if(!url) {url = _path; sprintf(url, "http://%s%s", local_ip, _path0);}

		if(IS_ON_XMB)
		{
			while(View_Find("explore_plugin") == 0) sys_timer_sleep(1); // wait for explore_plugin

			do_umount(false);
			open_browser(url, 0);
		}
		else
			ret = false;

		is_mounting = false;

		led(GREEN, ON);
		return ret;
	}

	// -----------------
	// remove /PS3_GAME
	// -----------------

	sprintf(_path, "%s", _path0);

	if(*_path == '/')
	{
		char *p = strstr(_path, "/PS3_"); if(p) *p = NULL;
	}


	// ------------
	// get /net id
	// ------------

	char netid = NULL;

#ifndef LITE_EDITION
	if(islike(_path, "/net"))
	{
		netid = _path[4];
		if((netid >= '0' && netid <= '4') && _path[5] == NULL) strcat(_path, "/.");
	}
#endif

	// ---------------------------------------------
	// skip last game if mounting /GAMEI (nonCobra)
	// ---------------------------------------------

#ifndef COBRA_ONLY
 #ifdef EXT_GDATA
	if(do_eject == MOUNT_EXT_GDATA) goto install_mm_payload;
 #endif
#endif

	// ----------
	// last game
	// ----------
	if(do_eject)
	{
		// load last_games.bin
		_lastgames lastgames;
		if(read_file(WMTMP "/last_games.bin", (char*)&lastgames, sizeof(_lastgames), 0) == 0) lastgames.last = 0xFF;

		// find game being mounted in last_games.bin
		bool _prev = false, _next = false;

		_next = IS(_path, "_next");
		_prev = IS(_path, "_prev");

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

			sprintf(_path, "%s", lastgames.game[lastgames.last]);
		}
		else
		if(lastgames.last >= MAX_LAST_GAMES)
		{
			lastgames.last = 0;
			snprintf(lastgames.game[lastgames.last], MAX_PATH_LEN, "%s", _path);
		}
		else
		{
			bool found = false;
			for(u8 n = 0; n < MAX_LAST_GAMES; n++)
			{
				if(IS(lastgames.game[n], _path)) {found = true; break;}
			}

			if(!found)
			{
				lastgames.last++;
				if(lastgames.last >= MAX_LAST_GAMES) lastgames.last = 0;
				snprintf(lastgames.game[lastgames.last], MAX_PATH_LEN, "%s", _path);
			}
		}

		// save last_games.bin
		save_file(WMTMP "/last_games.bin", (char *)&lastgames, sizeof(_lastgames));
	}

	// -----------------------
	// save last mounted game
	// -----------------------

	if(*_path == '_' || strchr(_path, '/') == NULL) goto exit_mount;
	else
	{
		save_file(WMTMP "/last_game.txt", _path, SAVE_ALL);
	}


	// ----------------------------------------
	// show start mounting message (game path)
	// ----------------------------------------

	if(do_eject) show_msg(_path);


	// ------------------------------------------------------------------------------------------------------------
	// launch ntfs psx & isos tagged [raw] with external rawseciso sprx (if available) (due support for multi PSX)
	// ------------------------------------------------------------------------------------------------------------
#ifdef COBRA_ONLY
 #ifdef FAKEISO
	{
		if(!extcmp(_path, ".ntfs[PSXISO]", 13) || (strstr(_path, ".ntfs[") != NULL && strstr(_path, "[raw]") != NULL))
		{
			u8 n;
			const char raw_iso_sprx[4][40] = {  "/dev_flash/vsh/module/raw_iso.sprx",
												"/dev_hdd0/raw_iso.sprx",
												"/dev_hdd0/plugins/raw_iso.sprx",
												"/dev_hdd0/game/IRISMAN00/sprx_iso" };

			for(n = 0; n < 4; n++)
				if(file_exists(raw_iso_sprx[n])) break;

			if(n < 4)
			{
				cellFsChmod(_path, MODE);

				sys_addr_t addr = 0;
				if(sys_memory_allocate(_64KB_, SYS_MEMORY_PAGE_SIZE_64K, &addr) == CELL_OK)
				{
					char *sprx_data = (char *)addr;
					uint64_t msiz = read_file(_path, sprx_data, _64KB_, 0);
					if(msiz > _4KB_)
					{
						do_umount(false); if(!extcmp(_path, ".ntfs[PSXISO]", 13)) {mount_unk = EMU_PSX; select_ps1emu(_path);}

						if(cobra_load_vsh_plugin(0, (char*)raw_iso_sprx[n], (u8*)sprx_data, msiz) != CELL_OK) ret = false;

						sys_memory_free(addr);
						goto exit_mount;
					}
					sys_memory_free(addr);
				}
			}
		}
	}
 #endif

	// ------------------
	// mount GAMEI game
	// ------------------
 #ifdef PKG_LAUNCHER
	{
		char *pos = strstr(_path, "/GAMEI/");
		if(pos)
		{
			sys_map_path("/dev_hdd0/game/PKGLAUNCH", _path0);
			strncpy(map_title_id, pos + 7, 9); map_title_id[9] = NULL;
			sprintf(_path, "/dev_hdd0/game/%s", map_title_id);
			sys_map_path(_path, _path0);

			mount_unk = EMU_MAX;
			goto exit_mount;
		}
	}
 #endif

	// ------------------
	// mount ROMS game
	// ------------------
 #ifdef MOUNT_ROMS
	if(isDir("/dev_hdd0/game/PKGLAUNCH"))
	{
		int plen = strlen(_path) - 4;
		if(plen < 0) plen = 0;
		else if(_path[plen + 2] == '.') plen+=2;
		else if(_path[plen + 1] == '.') plen++;

		if((strstr(_path, "/ROMS/") != NULL) || (strcasestr(_path, ".SELF") != NULL) || (strcasestr(ROMS_EXTENSIONS, _path + plen) != NULL))
		{
			do_umount_eject();

			sys_map_path("/dev_hdd0/game/PKGLAUNCH", NULL);
			sys_map_path("/dev_hdd0/game/PKGLAUNCH/PS3_GAME/USRDIR/cores", "/dev_hdd0/game/SSNE10000/USRDIR/cores");

			cobra_map_game("/dev_hdd0/game/PKGLAUNCH", "PKGLAUNCH", 0);

			save_file("/dev_hdd0/game/PKGLAUNCH/USRDIR/launch.txt", _path, 0);
			mount_unk = EMU_MAX;
			goto exit_mount;
		}
	}
 #endif
#endif

	// ------------------
	// mount PS2 Classic
	// ------------------
	if(!extcmp(_path, ".BIN.ENC", 8))
	{
		char temp[MAX_PATH_LEN];

		if(file_exists(PS2_CLASSIC_PLACEHOLDER))
		{
			copy_in_progress = true, copied_count = 0;

			sprintf(temp, "PS2 Classic\n%s", strrchr(_path, '/') + 1);
			show_msg(temp);

 #ifndef LITE_EDITION
			if(c_firmware >= 4.65f)
			{   // Auto create "classic_ps2 flag" for PS2 Classic (.BIN.ENC) on rebug 4.65.2
				do_umount(false);
				enable_classic_ps2_mode();
			}
 #endif
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
				save_file(WMNOSCAN, NULL, 0);

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

		goto exit_mount;
	}

 #ifndef LITE_EDITION
	if((c_firmware >= 4.65f) && strstr(_path, "/PS2ISO")!=NULL)
	{   // Auto remove "classic_ps2" flag for PS2 ISOs on rebug 4.65.2
		disable_classic_ps2_mode();
	}
 #endif



	///////////////////////
	// MOUNT ISO OR PATH //
	///////////////////////

#ifdef COBRA_ONLY
	{
		// --------------------------------------------
		// auto-map /dev_hdd0/game to dev_usbxxx/GAMEI
		// --------------------------------------------
		 #ifdef EXT_GDATA
		{
			// auto-enable external GD
			if(do_eject != 1) ;

			else if(strstr(_path, "/GAME"))
			{
				char extgdfile[MAX_PATH_LEN], *extgdini = extgdfile;
				sprintf(extgdfile, "%s/PS3_GAME/PS3GAME.INI", _path);
				if(read_file(extgdfile, extgdini, 12, 0))
				{
					if((extgd == 0) &&  (extgdini[10] & (1<<1))) set_gamedata_status(1, false); else
					if((extgd == 1) && !(extgdini[10] & (1<<1))) set_gamedata_status(0, false);
				}
				else if(extgd) set_gamedata_status(0, false);
			}
			else if((strstr(_path, "PS3ISO") != NULL) && (strstr(_path, "[gd]") != NULL))
			{
				if(extgd == 0) set_gamedata_status(1, false);
			}
			else if(extgd)
				set_gamedata_status(0, false);
		}
		 #endif //#ifdef EXT_GDATA

	mount_again:

		// ---------------------
		// unmount current game
		// ---------------------

		do_umount_eject();

		// ----------
		// mount iso
		// ----------
		if(!isDir(_path))
		{
			if( strstr(_path, "/PSXISO") || strstr(_path, "/PSXGAMES") || !extcmp(_path, ".ntfs[PSXISO]", 13) || mount_unk == EMU_PSX) {mount_unk = EMU_PSX; select_ps1emu(_path);}

			//if(_next || _prev)
				sys_timer_sleep(1);
			//else
			//	sys_timer_usleep(50000);


			// --------------
			// get ISO parts
			// --------------

			u8 iso_parts = 1;
			char iso_list[MAX_ISO_PARTS][MAX_PATH_LEN];
			char *cobra_iso_list[MAX_ISO_PARTS];
			char path2[MAX_PATH_LEN];
			char templn[MAX_LINE_LEN];

			sprintf(iso_list[0], "%s", _path);
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
						sprintf(iso_list[n], "%s", templn);
						cobra_iso_list[n] = (char*)iso_list[n];
					}
					else
						break;
				}
			}

			// ---------------
			// mount NTFS ISO
			// ---------------

			if(strstr(_path, ".ntfs["))
			{
				sys_addr_t addr = 0;
				if(sys_memory_allocate(_64KB_, SYS_MEMORY_PAGE_SIZE_64K, &addr) == CELL_OK)
				{
					char *rawiso_data = (char*)addr;
					if(read_file(_path, rawiso_data, _64KB_, 0) > _4KB_)
					{
						sys_ppu_thread_create(&thread_id_ntfs, rawseciso_thread, (uint64_t)addr, THREAD_PRIO, THREAD_STACK_SIZE_8KB, SYS_PPU_THREAD_CREATE_JOINABLE, THREAD_NAME_NTFS);

						waitfor("/dev_bdvd", 3);

						if(!extcmp(_path, ".ntfs[PS3ISO]", 13))
						{
							get_name(templn, _path, NO_EXT);
							cache_icon0_and_param_sfo(templn);
	#ifdef FIX_GAME
							fix_game(_path, titleID, webman_config->fixgame);
	#endif
						}

						// cache PS2ISO or PSPISO to HDD0
						bool is_ps2 = (strstr(_path, ".ntfs[PS2ISO]") != NULL);
						bool is_psp = (strstr(_path, ".ntfs[PSPISO]") != NULL);

						if(is_psp || is_ps2)
						{
							int fd;

							if(cellFsOpendir("/dev_bdvd", &fd) == CELL_FS_SUCCEEDED)
							{
								CellFsDirent entry; u64 read_e;

								while((cellFsReaddir(fd, &entry, &read_e) == CELL_FS_SUCCEEDED) && (read_e > 0))
								{
									if(entry.d_name[0] != '.') break;
								}
								cellFsClosedir(fd);

								if(entry.d_name[0] == NULL) goto exit_mount;

								sprintf(_path, "/dev_bdvd/%s", entry.d_name);

								if(file_exists(_path) == false) goto exit_mount;

								if(is_ps2)
									goto copy_ps2iso_to_hdd0;
								else
									goto copy_pspiso_to_hdd0;
							}
						}
					}
					else
						ret = false;
				}
				goto exit_mount;
			}

	#ifndef LITE_EDITION

			// -----------------------
			// mount /net ISO or path
			// -----------------------

			if(netid >= '0' && netid <= '4')
			{
				sys_addr_t sysmem = 0; netiso_svrid = NONE;
				if(sys_memory_allocate(_64KB_, SYS_MEMORY_PAGE_SIZE_64K, &sysmem) == CELL_OK)
				{
					netiso_svrid = netid - '0';
					netiso_args *_netiso_args = (netiso_args*)sysmem;
					memset(_netiso_args, 0, _64KB_);

					if( is_netsrv_enabled(netiso_svrid) )
					{
						sprintf(_netiso_args->server, "%s", webman_config->neth[netiso_svrid]);
						_netiso_args->port = webman_config->netp[netiso_svrid];
					}
					else
					{
						sys_memory_free(sysmem); ret = false;
						goto exit_mount;
					}

					char *netpath = _path + 5;

					sprintf(_netiso_args->path, "%s", netpath);
					if(islike(netpath, "/PS3ISO")) _netiso_args->emu_mode = EMU_PS3; else
					if(islike(netpath, "/PS2ISO")) goto copy_ps2iso_to_hdd0;         else
					if(islike(netpath, "/PSPISO")) goto copy_pspiso_to_hdd0;         else
					if(islike(netpath, "/BDISO" )) _netiso_args->emu_mode = EMU_BD;  else
					if(islike(netpath, "/DVDISO")) _netiso_args->emu_mode = EMU_DVD; else
					if(islike(netpath, "/PSX")   )
					{
						TrackDef tracks[1];
						tracks[0].lba = 0;
						tracks[0].is_audio = 0;

						_netiso_args->emu_mode = EMU_PSX;
						_netiso_args->num_tracks = 1;

						memcpy(_netiso_args->tracks, tracks, sizeof(TrackDef));
					}
					else if(islike(netpath, "/GAMES") || islike(netpath, "/GAMEZ"))
					{
						_netiso_args->emu_mode = EMU_PS3;
						sprintf(_netiso_args->path, "/***PS3***%s", netpath);
					}
					else
					{
						_netiso_args->emu_mode = EMU_DVD;
						if(!extcasecmp(netpath, ".iso", 4) || !extcasecmp(netpath, ".mdf", 4) || !extcasecmp(netpath, ".img", 4) || !extcasecmp(netpath, ".bin", 4)) ;
						else
							sprintf(_netiso_args->path, "/***DVD***%s", netpath);
					}

					sys_ppu_thread_create(&thread_id_net, netiso_thread, (uint64_t)sysmem, THREAD_PRIO, THREAD_STACK_SIZE_8KB, SYS_PPU_THREAD_CREATE_JOINABLE, THREAD_NAME_NET);

					if(_netiso_args->emu_mode == EMU_PS3)
					{
						waitfor("/dev_bdvd", 15);

						get_name(templn, _path, GET_WMTMP);
						cache_icon0_and_param_sfo(templn);

						#ifdef FIX_GAME
						fix_game(_path, titleID, webman_config->fixgame);
						#endif
					}
				}
				else
					ret = false;

				goto exit_mount;
			}
			else
	#endif //#ifndef LITE_EDITION
			{
				cellFsUnlink(WMNOSCAN); // remove wm_noscan if PS2ISO was already mounted

				ret = file_exists(cobra_iso_list[0]); if(!ret) goto exit_mount;


				// --------------
				// mount PS3 ISO
				// --------------

				if(strstr(_path, "/PS3ISO") || mount_unk == EMU_PS3)
				{
	#ifdef FIX_GAME
					if(webman_config->fixgame != FIX_GAME_DISABLED)
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
						get_name(templn, _path, GET_WMTMP);
						cache_icon0_and_param_sfo(templn);
					}
				}

				// --------------
				// mount PSP ISO
				// --------------

				else if(strstr(_path, "/PSPISO") || strstr(_path, "/ISO/") || mount_unk == EMU_PSP)
				{
					if(netid)
					{
	copy_pspiso_to_hdd0:
						cache_file_to_hdd(_path, iso_list[0], "/PSPISO", templn);
					}

					mount_unk = EMU_PSP;

					cellFsUnlink("/dev_hdd0/game/PSPC66820/PIC1.PNG");
					cobra_unset_psp_umd();

					if(file_exists(iso_list[0]))
					{
						int result = cobra_set_psp_umd(iso_list[0], NULL, (char*)"/dev_hdd0/tmp/psp_icon.png");

						if(result) ret = false;
					}
					else
						ret = false;
				}

				// --------------
				// mount PS2 ISO
				// --------------

				else if(strstr(_path, "/PS2ISO") || mount_unk == EMU_PS2_DVD)
				{
					if(!islike(_path, "/dev_hdd0"))
					{
	copy_ps2iso_to_hdd0:
						cache_file_to_hdd(_path, iso_list[0], "/PS2ISO", templn);
					}

					if(webman_config->ps2emu || strstr(_path, "[netemu]")) enable_ps2netemu_cobra(1); else enable_ps2netemu_cobra(0);

					if(file_exists(iso_list[0]))
					{
						TrackDef tracks[1];
						tracks[0].lba = 0;
						tracks[0].is_audio = 0;
						cobra_mount_ps2_disc_image(cobra_iso_list, 1, tracks, 1);
						if(webman_config->fanc) restore_fan(1); //fan_control( ((webman_config->ps2temp*255)/100), 0);

						// create "wm_noscan" to avoid re-scan of XML returning to XMB from PS2
						save_file(WMNOSCAN, NULL, 0);
					}
					else
						ret = false;
				}

				// --------------
				// mount PSX ISO
				// --------------

				else if(strstr(_path, "/PSXISO") || strstr(_path, "/PSXGAMES") || mount_unk == EMU_PSX)
				{
					int flen = strlen(_path) - 4; bool mount_iso = false;

					if(flen < 0) ;

					else if(!extcasecmp(_path, ".cue", 4))
					{
						char extensions[8][8] = {".bin", ".iso", ".img", ".mdf", ".BIN", ".ISO", ".IMG", ".MDF"};
						for(u8 e = 0; e < 8; e++)
						{
							cobra_iso_list[0][flen] = NULL; strcat(cobra_iso_list[0], extensions[e]);
							mount_iso = file_exists(cobra_iso_list[0]); if(mount_iso) break;
						}
					}
					else if(_path[flen] == '.')
					{
						_path[flen] = NULL; strcat(_path, ".cue");
						if(file_exists(_path) == false) {_path[flen] = NULL; strcat(_path, ".CUE");
						if(file_exists(_path) == false) sprintf(_path, "%s", cobra_iso_list[0]);}
					}

					mount_iso = mount_iso || file_exists(cobra_iso_list[0]); ret = mount_iso;

					if(!extcasecmp(_path, ".cue", 4))
					{
						sys_addr_t sysmem = 0;
						if(sys_memory_allocate(_64KB_, SYS_MEMORY_PAGE_SIZE_64K, &sysmem) == CELL_OK)
						{
							char *buf = (char*)sysmem;
							uint64_t msiz = read_file(_path, buf, 65535, 0);
							if(msiz > 10)
							{
								unsigned int num_tracks = 0;

								TrackDef tracks[32];
								tracks[0].lba = 0;
								tracks[0].is_audio = 0;

								char tcode[MAX_LINE_LEN];
								u8 tmin = 0, tsec = 0, tfrm = 0;
								u8 use_pregap = 0;
								u32 lp = 0, tcode_len;

								while(lp < msiz)// get_line ( templn, 512, sysmem ) != NULL )
								{
									u8 line_found = 0;
									*templn = NULL;
									for(u32 l = 0; l < MAX_LINE_LEN; l++)
									{
										if(l>=msiz) break;
										if(lp<msiz && buf[lp] && buf[lp]!='\n' && buf[lp]!='\r')
										{
											templn[l] = buf[lp];
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

									tcode_len = sprintf(tcode, "%s", strrchr(templn, ' ') + 1); tcode[8] = NULL;
									if((tcode_len != 8) || tcode[2]!=':' || tcode[5]!=':') continue;
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
								mount_iso = false;
							}

							sys_memory_free(sysmem);
						}
					}

					if(mount_iso)
					{
						TrackDef tracks[1];
						tracks[0].lba = 0;
						tracks[0].is_audio = 0;
						cobra_mount_psx_disc_image_iso(cobra_iso_list[0], tracks, 1);
					}
				}

				// -------------------
				// mount DVD / BD ISO
				// ------------------

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

					waitfor("/dev_bdvd", 5);

					// re-mount with media type
					if(isDir("/dev_bdvd/PS3_GAME")) mount_unk = EMU_PS3; else
					if(isDir("/dev_bdvd/VIDEO_TS")) mount_unk = EMU_DVD; else
					if(file_exists("/dev_bdvd/SYSTEM.CNF") || strcasestr(_path, "PS2")) mount_unk = EMU_PS2_DVD; else
					if(strcasestr(_path, "PSP")!=NULL && !extcasecmp(_path, ".iso", 4)) mount_unk = EMU_PSP; else
					if(!isDir("/dev_bdvd")) mount_unk = EMU_PSX; // failed to mount PSX CD as bd disc

					if(mount_unk) goto mount_again;

					mount_unk = EMU_BD;
				}

				// ----------------------------------------------------------------------------------------
				// send_fake_disc_insert_event for mounted ISOs (PS3ISO/PS2ISO/PSXISO/PSPISO/BDISO/DVDISO)
				// ----------------------------------------------------------------------------------------
				sys_timer_usleep(2500);
				cobra_send_fake_disc_insert_event();

				//goto exit_mount;
			}
		}

		// ------------------
		// mount folder (JB)
		// ------------------

		else
		{
			int special_mode = 0;

		#ifdef EXTRA_FEAT
			CellPadData pad_data = pad_read();

			if(pad_data.len > 0 && (pad_data.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_SELECT)) special_mode = true; //mount also app_home / eject disc

			if(special_mode) eject_insert(1, 0);
		#endif

			// -- fix game & get TitleID from PARAM.SFO
		#ifdef FIX_GAME
			fix_game(_path, titleID, webman_config->fixgame);
		#else
			char filename[MAX_PATH_LEN];
			sprintf(filename, "%s/PS3_GAME/PARAM.SFO", _path);

			getTitleID(filename, titleID, GET_TITLE_ID_ONLY);
		#endif
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
			if((*titleID > ' ') && (titleID[8] >= '0'))
				cobra_map_game(_path, titleID, &special_mode);
			else
				cobra_map_game(_path, "TEST00000", &special_mode);
		}

		//goto exit_mount;
	}
#endif //#ifdef COBRA_ONLY

#ifndef COBRA_ONLY
install_mm_payload:

	if(c_firmware == 0.0f) {ret = false; goto exit_mount;}

	install_peek_poke();

	if(do_eject) eject_insert(1, 1);

	pokeq(0x8000000000000000ULL+MAP_ADDR, 0x0000000000000000ULL);
	pokeq(0x8000000000000008ULL+MAP_ADDR, 0x0000000000000000ULL);

	if(cobra_mode) goto exit_mount;

	if(base_addr == 0 || SYSCALL_TABLE == 0) {ret = false; goto exit_mount;}

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


	char path[MAX_PATH_LEN];

	#ifdef EXT_GDATA

	//------------------
	// re-load last game
	//------------------

	if(do_eject == MOUNT_EXT_GDATA) // extgd
	{
		// get last game path
		get_last_game(_path);
	}

	#endif //#ifdef EXT_GDATA

	sprintf(path, "%s", _path);

	if(!isDir(path)) *_path = *path = NULL;

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

	if(*path)
	{
		if(do_eject)
		{
			add_to_map("/dev_bdvd", path);
			add_to_map("//dev_bdvd", path);

			char path2[strlen(_path) + 24];

			sprintf(path2, "%s/PS3_GAME", _path);
			add_to_map("/app_home/PS3_GAME", path2);

			sprintf(path2, "%s/PS3_GAME/USRDIR", _path);
			add_to_map("/app_home/USRDIR", path2);

			sprintf(path2, "%s/PS3_GAME/USRDIR/", _path);
			add_to_map("/app_home/", path2);
		}

		add_to_map("/app_home", path);
	}

	#ifdef EXT_GDATA

	//---------------------------------------------
	// auto-map /dev_hdd0/game to dev_usbxxx/GAMEI
	//---------------------------------------------

	if(do_eject != 1) ;

	else if(strstr(_path, "/GAME"))
	{
		char extgdfile[MAX_PATH_LEN], *extgdini = extgdfile;
		sprintf(extgdfile, "%s/PS3_GAME/PS3GAME.INI", _path);
		if(read_file(extgdfile, extgdini, 12, 0))
		{
			if((extgd == 0) &&  (extgdini[10] & (1<<1))) set_gamedata_status(1, false); else
			if((extgd == 1) && !(extgdini[10] & (1<<1))) set_gamedata_status(0, false);
		}
		else if(extgd) set_gamedata_status(0, false);
	}

	#endif

	//----------------------------
	// Patched explore_plugin.sprx
	//----------------------------
	{
		char expplg[128];
		char app_sys[128];

		sprintf(app_sys, MM_ROOT_STD "/sys");
		if(!isDir(app_sys))
			sprintf(app_sys, MM_ROOT_STL "/sys");
		if(!isDir(app_sys))
			sprintf(app_sys, MM_ROOT_SSTL "/sys");

		if(c_firmware == 3.55f)
			sprintf(expplg, "%s/IEXP0_355.BIN", app_sys);
		else if(c_firmware == 4.21f)
			sprintf(expplg, "%s/IEXP0_420.BIN", app_sys);
		else if(c_firmware == 4.30f || c_firmware == 4.31f)
			sprintf(expplg, "%s/IEXP0_430.BIN", app_sys);
		else if(c_firmware == 4.40f || c_firmware == 4.41f)
			sprintf(expplg, "%s/IEXP0_440.BIN", app_sys);
		else if(c_firmware == 4.46f)
			sprintf(expplg, "%s/IEXP0_446.BIN", app_sys);
		else if(c_firmware >= 4.50f && c_firmware <= 4.55f)
			sprintf(expplg, "%s/IEXP0_450.BIN", app_sys);
		else if(c_firmware >= 4.60f && c_firmware <= 4.66f)
			sprintf(expplg, "%s/IEXP0_460.BIN", app_sys);
		else if(c_firmware >= 4.70f)
			sprintf(expplg, "%s/IEXP0_470.BIN", app_sys);
		else
			sprintf(expplg, "%s/none", app_sys);

		if(do_eject && file_exists(expplg))
			add_to_map("/dev_flash/vsh/module/explore_plugin.sprx", expplg);
	}

	//---------------
	// New libfs.sprx
	//---------------
	if(do_eject && (c_firmware >= 4.20f) && file_exists(NEW_LIBFS_PATH))
		add_to_map(ORG_LIBFS_PATH, NEW_LIBFS_PATH);

	//-----------------------------------------------//
	uint64_t map_data  = (MAP_BASE);
	uint64_t map_paths = (MAP_BASE) + (max_mapped + 1) * 0x20;

	for(u16 n = 0; n < 0x400; n += 8) pokeq(map_data + n, 0);

	if(!max_mapped) {ret = false; goto exit_mount;}

	for(u8 n = 0; n < max_mapped; n++)
	{
		size_t src_len, dst_len;

		if(map_paths > 0x80000000007FE800ULL) break;
		pokeq(map_data + (n * 0x20) + 0x10, map_paths);
		src_len = string_to_lv2(file_to_map[n].src, map_paths);
		map_paths += (src_len + 8) & 0x7f8;

		pokeq(map_data + (n * 0x20) + 0x18, map_paths);
		dst_len = string_to_lv2(file_to_map[n].dst, map_paths);
		map_paths += (dst_len + 8) & 0x7f8;

		pokeq(map_data + (n * 0x20) + 0x00, src_len);
		pokeq(map_data + (n * 0x20) + 0x08, dst_len);
	}

	if(isDir("/dev_bdvd")) sys_timer_sleep(2);

	//if(do_eject) eject_insert(0, 1);

#endif //#ifndef COBRA_ONLY

exit_mount:

	// -------------------------------
	// show 2nd message: "xxx" loaded
	// -------------------------------

	if(ret && *_path == '/')
	{
		char msg[MAX_PATH_LEN], *pos;

		// get file name (without path)
		pos = strrchr(_path, '/');
		sprintf(msg, "\"%s", pos + 1);

		// remove file extension
		pos = strstr(msg, ".ntfs["); if(pos) *pos = NULL;
		pos = strrchr(msg, '.'); if(pos) *pos = NULL;
		if(msg[1] == NULL) sprintf(msg, "\"%s", _path);

		// show loaded path
		strcat(msg, "\" "); strcat(msg, STR_LOADED2);
		show_msg(msg);
	}

	// ---------------
	// delete history
	// ---------------

	delete_history(false);

	if(mount_unk >= EMU_MAX) goto mounting_done;

	// -------------------------------------------
	// wait few seconds until the bdvd is mounted
	// -------------------------------------------

	if(ret && extcmp(_path, ".BIN.ENC", 8))
	{
		waitfor("/dev_bdvd", (islike(_path, "/dev_hdd0") ? 6 : netid ? 20 : 15));
		if(!isDir("/dev_bdvd")) ret = false;
	}

#ifdef FIX_GAME
	// -------------------------------------------------------
	// re-check PARAM.SFO to notify if game needs to be fixed
	// -------------------------------------------------------

	if(ret && (c_firmware < LATEST_CFW))
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

	// -----------------------------------
	// show error if bdvd was not mounted
	// -----------------------------------

	if(!ret && !isDir("/dev_bdvd")) {char msg[MAX_PATH_LEN]; sprintf(msg, "%s %s", STR_ERROR, _path); show_msg(msg);}

	// -------------------------------------------------------------------------------------
	// remove syscalls hodling R2 (or prevent remove syscall if path contains [online] tag)
	// -------------------------------------------------------------------------------------

#ifdef REMOVE_SYSCALLS
	else if(mount_unk != EMU_PSX)
	{
		CellPadData pad_data = pad_read();
		bool otag = (strcasestr(_path, ONLINE_TAG)!=NULL);
		bool r2 = (pad_data.len > 0 && (pad_data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_R2));
		if((!r2 && otag) || (r2 && !otag)) disable_cfw_syscalls(webman_config->keep_ccapi);
	}
#endif

mounting_done:

#ifdef COBRA_ONLY

	// ------------------------------------------------------------------
	// auto-enable gamedata on bdvd if game folder or ISO contains GAMEI
	// ------------------------------------------------------------------

 #ifdef EXT_GDATA
	if((extgd == 0) && isDir("/dev_bdvd/GAMEI")) set_gamedata_status(2, true); // auto-enable external gameDATA (if GAMEI exists on /bdvd)
 #endif

	// -----------------------------------------------
	// redirect system files (PUP, net/PKG, SND0.AT3)
	// -----------------------------------------------
	{
		if(ret && file_exists("/dev_bdvd/PS3UPDAT.PUP"))
		{
			sys_map_path("/dev_bdvd/PS3/UPDATE", (char*)"/dev_bdvd"); //redirect root of bdvd to /dev_bdvd/PS3/UPDATE (allows update from mounted /net folder or fake BDFILE)
		}

		if(ret && ((!netid) && isDir("/dev_bdvd/PKG")))
		{
			sys_map_path("/app_home", (char*)"/dev_bdvd/PKG"); //redirect net_host/PKG to app_home
		}

		{sys_map_path("/dev_bdvd/PS3_UPDATE", (char*)SYSMAP_PS3_UPDATE);} // redirect firmware update on BD disc to empty folder

		if(webman_config->nosnd0) {sys_map_path((char*)"/dev_bdvd/PS3_GAME/SND0.AT3", (char*)SYSMAP_PS3_UPDATE);} // disable SND0.AT3 on startup


		{ PS3MAPI_DISABLE_ACCESS_SYSCALL8 }
	}
#endif

	// --------------
	// exit function
	// --------------

	led(GREEN, ON);
	max_mapped = 0;
	is_mounting = false;

	return ret;
}
