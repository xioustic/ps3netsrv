#ifdef ENGLISH_ONLY

static char STR_HOME[8] = "Home";

#define STR_TRADBY	"<br>"

#define STR_FILES		"Files"
#define STR_GAMES		"Games"
#define STR_SETUP		"Setup"

#define STR_EJECT		"Eject"
#define STR_INSERT		"Insert"
#define STR_UNMOUNT		"Unmount"
#define STR_COPY		"Copy Folder"
#define STR_REFRESH		"Refresh"
#define STR_SHUTDOWN	"Shutdown"
#define STR_RESTART		"Restart"

#define STR_BYTE		"b"
#define STR_KILOBYTE	"KB"
#define STR_MEGABYTE	"MB"
#define STR_GIGABYTE	"GB"

#define STR_COPYING		"Copying"
#define STR_CPYDEST		"Destination"
#define STR_CPYFINISH	"Copy Finished!"
#define STR_CPYABORT	"Copy aborted!"
#define STR_DELETE		"Delete"

#define STR_SCAN1		"Scan these devices"
#define STR_SCAN2		"Scan for content"
#define STR_PSPL		"Show PSP Launcher"
#define STR_PS2L		"Show PS2 Classic Launcher"
#define STR_RXVID		"Show Video sub-folder"
#define STR_VIDLG		"Video"
#define STR_LPG			"Load last-played game on startup"
#define STR_AUTOB		"Check for /dev_hdd0/PS3ISO/AUTOBOOT.ISO on startup"
#define STR_DELAYAB		"Delay loading of AUTOBOOT.ISO/last-game (Disc Auto-start)"
#define STR_DEVBL		"Enable /dev_blind (writable /dev_flash) on startup"
#define STR_CONTSCAN	"Disable content scan on startup"
#define STR_USBPOLL		"Disable USB polling"
#define STR_FTPSVC		"Disable FTP service"
#define STR_FIXGAME		"Disable auto-fix game"
#define STR_COMBOS		"Disable all PAD shortcuts"
#define STR_MMCOVERS	"Disable multiMAN covers"
#define STR_ACCESS		"Disable remote access to FTP/WWW services"
#define STR_NOSETUP		"Disable webMAN Setup entry in \"webMAN Games\""
#define STR_NOSPOOF		"Disable firmware version spoofing"
#define STR_NOGRP		"Disable grouping of content in \"webMAN Games\""
#define STR_NOWMDN		"Disable startup notification of WebMAN on the XMB"
#ifdef NOSINGSTAR
 #define STR_NOSINGSTAR	"Remove SingStar icon"
#endif
#define STR_RESET_USB	"Disable Reset USB Bus"
#define STR_AUTO_PLAY	"Auto-Play"
#define STR_TITLEID		"Include the ID as part of the title of the game"
#define STR_FANCTRL		"Enable dynamic fan control"
#define STR_NOWARN		"Disable temperature warnings"
#define STR_AUTOAT		"Auto at"
#define STR_LOWEST		"Lowest"
#define STR_FANSPEED	"fan speed"
#define STR_MANUAL		"Manual"
#define STR_PS2EMU		"PS2 Emulator"
#define STR_LANGAMES	"Scan for LAN games/videos"
#define STR_ANYUSB		"Wait for any USB device to be ready"
#define STR_ADDUSB		"Wait additionally for each selected USB device to be ready"
#define STR_SPOOFID		"Change idps and psid in lv2 memory at system startup"
#define STR_DELCFWSYS	"Disable CFW syscalls and delete history files at system startup"
#define STR_MEMUSAGE	"Plugin memory usage"
#define STR_PLANG		"Plugin language"
#define STR_PROFILE		"Profile"
#define STR_DEFAULT		"Default"
#define STR_COMBOS2		"XMB/In-Game PAD SHORTCUTS"
#define STR_FAILSAFE	"FAIL SAFE"
#define STR_SHOWTEMP	"SHOW TEMP"
#define STR_SHOWIDPS	"SHOW IDPS"
#define STR_PREVGAME	"PREV GAME"
#define STR_NEXTGAME	"NEXT GAME"
#define STR_SHUTDOWN2	"SHUTDOWN "
#define STR_RESTART2	"RESTART&nbsp; "
#define STR_DELCFWSYS2	"DEL CFW SYSCALLS"
#define STR_UNLOADWM	"UNLOAD WM"
#define STR_FANCTRL2	"CTRL FAN"
#define STR_FANCTRL4	"CTRL DYN FAN"
#define STR_FANCTRL5	"CTRL MIN FAN"
#define STR_UPDN		"&#8593;/&#8595;" //↑/↓
#define STR_LFRG		"&#8592;/&#8594;" //←/→

#define STR_DISCOBRA	"COBRA TOGGLE"

#define STR_RBGMODE		"RBG MODE TOGGLE"
#define STR_RBGNORM		"NORM MODE TOGGLE"
#define STR_RBGMENU		"MENU TOGGLE"

#define STR_SAVE		"Save"
#define STR_SETTINGSUPD	"Settings updated.<br><br>Click <a href=\"/restart.ps3\">here</a> to restart your PLAYSTATION®3 system."
#define STR_ERROR		"Error!"

#define STR_MYGAMES		"webMAN Games"
#define STR_LOADGAMES	"Load games with webMAN"
#define STR_FIXING		"Fixing"

#define STR_WMSETUP		"webMAN Setup"
#define STR_WMSETUP2	"Setup webMAN options"

#define STR_EJECTDISC	"Eject Disc"
#define STR_UNMOUNTGAME	"Unmount current game"

#define STR_WMSTART		"webMAN loaded!"
#define STR_WMUNL		"webMAN unloaded!"
#define STR_CFWSYSALRD	"CFW Syscalls already disabled"
#define STR_CFWSYSRIP	"Removal History files & CFW Syscalls in progress..."
#define STR_RMVCFWSYS	"History files & CFW Syscalls deleted OK!"
#define STR_RMVCFWSYSF	"Failed to remove CFW Syscalls"

#define STR_RMVWMCFG	"webMAN config reset in progress..."
#define STR_RMVWMCFGOK	"Done! Restart within 3 seconds"

#define STR_PS3FORMAT	"PS3 format games"
#define STR_PS2FORMAT	"PS2 format games"
#define STR_PS1FORMAT	"PSOne format games"
#define STR_PSPFORMAT	"PSP\xE2\x84\xA2 format games"

#define STR_VIDFORMAT	"Blu-ray\xE2\x84\xA2 and DVD"
#define STR_VIDEO		"Video content"

#define STR_LAUNCHPSP	"Launch PSP ISO mounted through webMAN or mmCM"
#define STR_LAUNCHPS2	"Launch PS2 Classic"

#define STR_GAMEUM		"Game unmounted."

#define STR_EJECTED		"Disc ejected."
#define STR_LOADED		"Disc inserted."

#define STR_GAMETOM		"Game to mount"
#define STR_GAMELOADED	"Game loaded successfully. Start the game from the disc icon<br>or from <b>/app_home</b>&nbsp;XMB entry.</a><hr>Click <a href=\"/mount.ps3/unmount\">here</a> to unmount the game."
#define STR_PSPLOADED	"Game loaded successfully. Start the game using <b>PSP Launcher</b>.<hr>"
#define STR_PS2LOADED	"Game loaded successfully. Start the game using <b>PS2 Classic Launcher</b>.<hr>"
#define STR_LOADED2		"loaded   "

#define STR_MOVIETOM	"Movie to mount"
#define STR_MOVIELOADED	"Movie loaded successfully. Start the movie from the disc icon<br>under the Video column.</a><hr>Click <a href=\"/mount.ps3/unmount\">here</a> to unmount the movie."

#define STR_XMLRF		"Game list refreshed (<a href=\"" MY_GAMES_XML "\">mygames.xml</a>).<br>Click <a href=\"/restart.ps3\">here</a> to restart your PLAYSTATION®3 system now."

#define STR_STORAGE		"System storage"
#define STR_MEMORY		"Memory"
#define STR_MBFREE		"MB free"
#define STR_KBFREE		"KB free"

#define STR_FANCTRL3	"Fan control:"

#define STR_ENABLED 	"Enabled"
#define STR_DISABLED	"Disabled"

#define STR_FANCH0		"Fan setting changed:"
#define STR_FANCH1		"MAX TEMP: "
#define STR_FANCH2		"FAN SPEED: "
#define STR_FANCH3		"MIN FAN SPEED: "

#define STR_OVERHEAT	"System overheat warning!"
#define STR_OVERHEAT2	"  OVERHEAT DANGER!\nFAN SPEED INCREASED!"

#define STR_NOTFOUND	"Not found!"

#define COVERS_PATH		"http://xmbmods.co/wmlp/covers/%s.JPG"

#else
static int fh;

static char lang_code[3]			= "";

static char STR_TRADBY[120];//		= "<br>";

static char STR_FILES[24];//		= "Files";
static char STR_GAMES[24];//		= "Games";
static char STR_SETUP[24];//		= "Setup";
static char STR_HOME[16];//			= "Home";
static char STR_EJECT[40];//		= "Eject";
static char STR_INSERT[40];//		= "Insert";
static char STR_UNMOUNT[40];//		= "Unmount";
static char STR_COPY[40];//			= "Copy Folder";
static char STR_REFRESH[24];//		= "Refresh";
static char STR_SHUTDOWN[32];//		= "Shutdown";
static char STR_RESTART[32];//		= "Restart";

static char STR_BYTE[8]				= "b";
static char STR_KILOBYTE[8]			= "KB";
static char STR_MEGABYTE[8]			= "MB";
static char STR_GIGABYTE[8]			= "GB";

static char STR_COPYING[24];//		= "Copying";
static char STR_CPYDEST[24];//		= "Destination";
static char STR_CPYFINISH[48];//	= "Copy Finished!";
static char STR_CPYABORT[48];//		= "Copy aborted!";
static char STR_DELETE[24];//		= "Delete";

static char STR_SCAN1[48];//		= "Scan these devices";
static char STR_SCAN2[48];//		= "Scan for content";
static char STR_PSPL[40];//			= "Show PSP Launcher";
static char STR_PS2L[48];//			= "Show PS2 Classic Launcher";
static char STR_RXVID[64];//		= "Show Video sub-folder";
static char STR_VIDLG[24];//		= "Video";
static char STR_LPG[128];//			= "Load last-played game on startup";
static char STR_AUTOB[96];//		= "Check for /dev_hdd0/PS3ISO/AUTOBOOT.ISO on startup";
static char STR_DELAYAB[168];//		= "Delay loading of AUTOBOOT.ISO/last-game (Disc Auto-start)";
static char STR_DEVBL[112];//		= "Enable /dev_blind (writable /dev_flash) on startup";
static char STR_CONTSCAN[120];//	= "Disable content scan on startup";
static char STR_USBPOLL[88];//		= "Disable USB polling";
static char STR_FTPSVC[64];//		= "Disable FTP service";
static char STR_FIXGAME[56];//		= "Disable auto-fix game";
static char STR_COMBOS[88];//		= "Disable all PAD shortcuts";
static char STR_MMCOVERS[72];//		= "Disable multiMAN covers";
static char STR_ACCESS[88];//		= "Disable remote access to FTP/WWW services";
static char STR_NOSETUP[120];//		= "Disable webMAN Setup entry in \"webMAN Games\"";
static char STR_NOSPOOF[96];//		= "Disable firmware version spoofing";
static char STR_NOGRP[104];//		= "Disable grouping of content in \"webMAN Games\"";
static char STR_NOWMDN[112];//		= "Disable startup notification of WebMAN on the XMB";
#ifdef NOSINGSTAR
static char STR_NOSINGSTAR[48];//	= "Remove SingStar icon";
#endif
static char STR_RESET_USB[48];//	= "Disable Reset USB Bus";
static char STR_AUTO_PLAY[24];//	= "Auto-Play";
static char STR_TITLEID[128];//		= "Include the ID as part of the title of the game";
static char STR_FANCTRL[96];//		= "Enable dynamic fan control";
static char STR_NOWARN[96];//		= "Disable temperature warnings";
static char STR_AUTOAT[32];//		= "Auto at";
static char STR_LOWEST[24];//		= "Lowest";
static char STR_FANSPEED[48];//		= "fan speed";
static char STR_MANUAL[32];//		= "Manual";
static char STR_PS2EMU[32];//		= "PS2 Emulator";
static char STR_LANGAMES[96];//		= "Scan for LAN games/videos";
static char STR_ANYUSB[88];//		= "Wait for any USB device to be ready";
static char STR_ADDUSB[136];//		= "Wait additionally for each selected USB device to be ready";
static char STR_SPOOFID[112];//		= "Change idps and psid in lv2 memory at system startup";
static char STR_DELCFWSYS[144];//	= "Disable CFW syscalls and delete history files at system startup";
static char STR_MEMUSAGE[80];//		= "Plugin memory usage";
static char STR_PLANG[40];//		= "Plugin language";
static char STR_PROFILE[16];//		= "Profile";
static char STR_DEFAULT[32];//		= "Default";
static char STR_COMBOS2[80];//		= "XMB/In-Game PAD SHORTCUTS";
static char STR_FAILSAFE[40];//		= "FAIL SAFE";
static char STR_SHOWTEMP[56];//		= "SHOW TEMP";
static char STR_SHOWIDPS[24];//		= "SHOW IDPS";
static char STR_PREVGAME[64];//		= "PREV GAME";
static char STR_NEXTGAME[56];//		= "NEXT GAME";
static char STR_SHUTDOWN2[32];//	= "SHUTDOWN ";
static char STR_RESTART2[32];//		= "RESTART&nbsp; ";
#ifdef REMOVE_SYSCALLS
static char STR_DELCFWSYS2[48];//	= "DEL CFW SYSCALLS";
#endif
static char STR_UNLOADWM[64];//		= "UNLOAD WM";
static char STR_FANCTRL2[48];//		= "CTRL FAN";
static char STR_FANCTRL4[72];//		= "CTRL DYN FAN";
static char STR_FANCTRL5[88];//		= "CTRL MIN FAN";
static char STR_UPDN[16]			= "&#8593;/&#8595;"; //↑/↓
static char STR_LFRG[16]			= "&#8592;/&#8594;"; //←/→

static char STR_SAVE[24];//			= "Save";
static char STR_SETTINGSUPD[192];//	= "Settings updated.<br><br>Click <a href=\"/restart.ps3\">here</a> to restart your PLAYSTATION®3 system.";
static char STR_ERROR[16];//			= "Error!";

static char STR_MYGAMES[32];//		= "webMAN Games";
static char STR_LOADGAMES[72];//	= "Load games with webMAN";
static char STR_FIXING[32];//		= "Fixing";

static char STR_WMSETUP[40];//		= "webMAN Setup";
static char STR_WMSETUP2[56];//		= "Setup webMAN options";

static char STR_EJECTDISC[32];//	= "Eject Disc";
static char STR_UNMOUNTGAME[56];//	= "Unmount current game";

static char STR_WMSTART[32];//		= "webMAN loaded!";
static char STR_WMUNL[72];//		= "webMAN unloaded!";
static char STR_CFWSYSALRD[72];//	= "CFW Syscalls already disabled";
static char STR_CFWSYSRIP[128];//	= "Removal History files & CFW Syscalls in progress...";
static char STR_RMVCFWSYS[136];//	= "History files & CFW Syscalls deleted OK!";
static char STR_RMVCFWSYSF[80];//	= "Failed to remove CFW Syscalls";

static char STR_RMVWMCFG[96];//		= "webMAN config reset in progress...";
static char STR_RMVWMCFGOK[112];//	= "Done! Restart within 3 seconds";

static char STR_PS3FORMAT[40];//	= "PS3 format games";
static char STR_PS2FORMAT[48];//	= "PS2 format games";
static char STR_PS1FORMAT[48];//	= "PSOne format games";
static char STR_PSPFORMAT[48];//	= "PSP\xE2\x84\xA2 format games";

static char STR_VIDFORMAT[56];//	= "Blu-ray\xE2\x84\xA2 and DVD";
static char STR_VIDEO[40];//		= "Video content";

static char STR_LAUNCHPSP[144];//	= "Launch PSP ISO mounted through webMAN or mmCM";
static char STR_LAUNCHPS2[48];//	= "Launch PS2 Classic";

static char STR_GAMEUM[64];//		= "Game unmounted.";

static char STR_EJECTED[40];//		= "Disc ejected.";
static char STR_LOADED[40];//		= "Disc inserted.";

static char STR_GAMETOM[48];//		= "Game to mount";
static char STR_GAMELOADED[288];//	= "Game loaded successfully. Start the game from the disc icon<br>or from <b>/app_home</b>&nbsp;XMB entry.</a><hr>Click <a href=\"/mount.ps3/unmount\">here</a> to unmount the game.";
static char STR_PSPLOADED[232]; //	= "Game loaded successfully. Start the game using <b>PSP Launcher</b>.<hr>";
static char STR_PS2LOADED[240]; //	= "Game loaded successfully. Start the game using <b>PS2 Classic Launcher</b>.<hr>";
static char STR_LOADED2[48];//		= "loaded   ";

static char STR_MOVIETOM[48];//		= "Movie to mount";
static char STR_MOVIELOADED[272];//	= "Movie loaded successfully. Start the movie from the disc icon<br>under the Video column.</a><hr>Click <a href=\"/mount.ps3/unmount\">here</a> to unmount the movie.";

static char STR_XMLRF[280];//		= "Game list refreshed (<a href=\"" MY_GAMES_XML "\">mygames.xml</a>).<br>Click <a href=\"/restart.ps3\">here</a> to restart your PLAYSTATION®3 system now.";

static char STR_STORAGE[40];//		= "System storage";
static char STR_MEMORY[48];//		= "Memory available";
static char STR_MBFREE[24];//		= "MB free";
static char STR_KBFREE[24];//		= "KB free";

static char STR_FANCTRL3[48];//		= "Fan control:";
static char STR_ENABLED[24];//		= "Enabled";
static char STR_DISABLED[24];//		= "Disabled";

static char STR_FANCH0[64];//		= "Fan setting changed:";
static char STR_FANCH1[48];//		= "MAX TEMP: ";
static char STR_FANCH2[48];//		= "FAN SPEED: ";
static char STR_FANCH3[72];//		= "MIN FAN SPEED: ";

static char STR_OVERHEAT[80];//		= "System overheat warning!";
static char STR_OVERHEAT2[120];//	= "  OVERHEAT DANGER!\nFAN SPEED INCREASED!";

static char STR_NOTFOUND[40];//		= "Not found!";

static char COVERS_PATH[100];//		= "";

#ifdef COBRA_ONLY
static const char *STR_DISCOBRA		= "COBRA TOGGLE";
#endif
#ifdef REX_ONLY
static const char *STR_RBGMODE		= "RBG MODE TOGGLE";
static const char *STR_RBGNORM		= "NORM MODE TOGGLE";
static const char *STR_RBGMENU		= "MENU TOGGLE";
#endif

#endif

#ifndef ENGLISH_ONLY

/*
static uint32_t get_xreg_value(const char *key, u32 default_value)
{
	int reg = -1;
	u32 reg_value = default_value;
	u16 off_string, len_data, len_string;
	u64 read, pos, off_val_data;
	CellFsStat stat;
	char string[256];

	if(cellFsOpen("/dev_flash2/etc/xRegistry.sys", CELL_FS_O_RDONLY, &reg, NULL, 0) != CELL_FS_SUCCEEDED || reg == -1)
	{
		return reg_value;
	}

	cellFsStat("/dev_flash2/etc/xRegistry.sys", &stat);

	u64 entry_name = 0x10000;

	while(true)
	{
	//// Data entries ////
		//unk
		entry_name += 2;

		//relative entry offset
		cellFsLseek(reg, entry_name, 0, &pos);
		cellFsRead(reg, &off_string, 2, &read);
		entry_name += 4;

		//data lenght
		cellFsLseek(reg, entry_name, 0, &pos);
		cellFsRead(reg, &len_data, 2, &read);
		entry_name += 3;

		//data
		off_val_data = entry_name;
		entry_name += len_data + 1;

	//// String Entries ////
		off_string += 0x12;

		//string length
		cellFsLseek(reg, off_string, 0, &pos);
		cellFsRead(reg, &len_string, 2, &read);
		off_string += 3;

		//string
		memset(string, 0, sizeof(string));
		cellFsLseek(reg, off_string, 0, &pos);
		cellFsRead(reg, string, len_string, &read);

		//Find language
		if(IS(string, key))
		{
			cellFsLseek(reg, off_val_data, 0, &pos);
			cellFsRead(reg, &reg_value, 4, &read);
			break;
		}

		if(off_string == 0xCCDD || entry_name >= stat.st_size) break;
	}

	cellFsClose(reg);

	return reg_value;
}
*/

static uint32_t get_system_language(uint8_t *lang)
{
	//u32 val_lang = get_xreg_value("/setting/system/language", 1);

	int val_lang = 1;
	xsetting_0AF1F161()->GetSystemLanguage(&val_lang);

	switch(val_lang)
	{
		case 0x0:
			*lang = 4;		//ger;
			break;
		//case 0x1:
		//	lang = 0;		//eng-us
		//	break;
		case 0x2:
			*lang = 3;		//spa
			break;
		case 0x3:
			*lang = 1;		//fre
			break;
		case 0x4:
			*lang = 2;		//ita
			break;
		case 0x5:
			*lang = 5;		//dut //Olandese
			break;
		case 0x6:
			*lang = 6;		//por-por
			break;
		case 0x7:
			*lang = 7;		//rus
			break;
		case 0x8:
			*lang = 18;		//jap
			break;
		case 0x9:
			*lang = 17;		//kor
			break;
		case 0xA:
		case 0xB:
			*lang = 19;		//chi-tra / chi-sim
			break;
		//case 0xC:
		//	*lang = 0;		//fin /** missing **/
		//	break;
		//case 0xD:
		//	*lang = 0;		//swe /** missing**/
		//	break;
		case 0xE:
			*lang = 20;		//dan
			break;
		//case 0xF:
		//	*lang = 0;		//nor /** missing**/
		//	break;
		case 0x10:
			*lang = 9;		//pol
			break;
		case 0x11:
			*lang = 12;		//por-bra
			break;
		//case 0x12:
		//	*lang = 0;		//eng-uk
		//	break;
		default:
			*lang = 0;
			break;
	}

	return val_lang;
}

#define CHUNK_SIZE 512
#define GET_NEXT_BYTE  {if(p < CHUNK_SIZE) c = buffer[p++]; else {cellFsRead(fd, buffer, CHUNK_SIZE, &bytes_read); c = buffer[0], p = 1;} lang_pos++;}

static bool language(const char *key_name, char *default_str)
{
	uint8_t c, i, key_len = strlen(key_name);
	uint64_t bytes_read = 0;
	char *buffer = html_base_path;
	static size_t p = 0, lang_pos = 0, size = 0;

	bool do_retry = true;

	if(fh == 0)
	{
		if(webman_config->lang > 22 && (webman_config->lang != 99)) return false;

		const char lang_codes[24][3]={"EN", "FR", "IT", "ES", "DE", "NL", "PT", "RU", "HU", "PL", "GR", "HR", "BG", "IN", "TR", "AR", "CN", "KR", "JP", "ZH", "DK", "CZ", "SK", "XX"};
		char lang_path[34];

		i = webman_config->lang; if(i > 23) i = 23;

		sprintf(lang_code, "_%s", lang_codes[i]);
		sprintf(lang_path, "%s/LANG%s.TXT", "/dev_hdd0/tmp/wm_lang", lang_code);

		struct CellFsStat buf;

		if(cellFsStat(lang_path, &buf) != CELL_FS_SUCCEEDED) return false; size = (size_t)buf.st_size;

		if(cellFsOpen(lang_path, CELL_FS_O_RDONLY, &fh, NULL, 0) != CELL_FS_SUCCEEDED) return false;

		lang_pos = 0;

 retry:
		cellFsLseek(fh, lang_pos, CELL_FS_SEEK_SET, NULL); p = CHUNK_SIZE;
	}

	int fd = fh;

	do {
		for(i = 0; i < key_len; )
		{
			{ GET_NEXT_BYTE }

			if(c != key_name[i]) break; i++;

			if(i == key_len)
			{
				// skip blanks
				while(lang_pos < size)
				{
					if(c == '[') break;

					{ GET_NEXT_BYTE }
				};

				size_t str_len = 0;

				// set value
				while(lang_pos < size)
				{
					{ GET_NEXT_BYTE }

					if(c == ']' || lang_pos >= size) break;

					default_str[str_len++] = c;
				}

				default_str[str_len] = NULL;
				return true;
			}
		}

	} while(lang_pos < size);

	if(do_retry) {do_retry = false, lang_pos = 0; goto retry;}

	return true;
}

#undef CHUNK_SIZE
#undef GET_NEXT_BYTE

static void update_language(void)
{
	fh = 0;

	// initialize variables with default values
	static bool do_once = true;

	if(do_once)
	{
		sprintf(STR_TRADBY,      "<br>");

		sprintf(STR_FILES,       "Files");
		sprintf(STR_GAMES,       "Games");
		sprintf(STR_SETUP,       "Setup");
		sprintf(STR_HOME,        "Home");
		sprintf(STR_EJECT,       "Eject");
		sprintf(STR_INSERT,      "Insert");
		sprintf(STR_UNMOUNT,     "Unmount");
		sprintf(STR_COPY,        "Copy Folder");
		sprintf(STR_REFRESH,     "Refresh");
		sprintf(STR_SHUTDOWN,    "Shutdown");
		sprintf(STR_RESTART,     "Restart");

		sprintf(STR_NOSPOOF,     "Disable firmware version spoofing");
		sprintf(STR_NOGRP,       "Disable grouping of content in \"webMAN Games\"");
		sprintf(STR_NOWMDN,      "Disable startup notification of WebMAN on the XMB");

#ifdef NOSINGSTAR
		sprintf(STR_NOSINGSTAR,  "Remove SingStar icon");
#endif

		sprintf(STR_RESET_USB,   "Disable Reset USB Bus");
		sprintf(STR_AUTO_PLAY,   "Auto-Play");

		sprintf(STR_TITLEID,     "Include the ID as part of the title of the game");
		sprintf(STR_FANCTRL,     "Enable dynamic fan control");
		sprintf(STR_NOWARN,      "Disable temperature warnings");

		sprintf(STR_AUTOAT,      "Auto at");
		sprintf(STR_LOWEST,      "Lowest");
		sprintf(STR_FANSPEED,    "fan speed");
		sprintf(STR_MANUAL,      "Manual");
		sprintf(STR_PS2EMU,      "PS2 Emulator");

		sprintf(STR_LANGAMES,    "Scan for LAN games/videos");
		sprintf(STR_ANYUSB,      "Wait for any USB device to be ready");
		sprintf(STR_ADDUSB,      "Wait additionally for each selected USB device to be ready");
		sprintf(STR_SPOOFID,     "Change idps and psid in lv2 memory at system startup");
		sprintf(STR_DELCFWSYS,   "Disable CFW syscalls and delete history files at system startup");
		sprintf(STR_MEMUSAGE,    "Plugin memory usage");
		sprintf(STR_PLANG,       "Plugin language");
		sprintf(STR_PROFILE,     "Profile");
		sprintf(STR_DEFAULT,     "Default");
		sprintf(STR_COMBOS2,     "XMB/In-Game PAD SHORTCUTS");
		sprintf(STR_FAILSAFE,    "FAIL SAFE");
		sprintf(STR_SHOWTEMP,    "SHOW TEMP");
		sprintf(STR_SHOWIDPS,    "SHOW IDPS");
		sprintf(STR_PREVGAME,    "PREV GAME");
		sprintf(STR_NEXTGAME,    "NEXT GAME");
		sprintf(STR_SHUTDOWN2,   "SHUTDOWN ");
		sprintf(STR_RESTART2,    "RESTART&nbsp; ");

#ifdef REMOVE_SYSCALLS
		sprintf(STR_DELCFWSYS2,  "DEL CFW SYSCALLS");
#endif

		sprintf(STR_UNLOADWM,    "UNLOAD WM");
		sprintf(STR_FANCTRL2,    "CTRL FAN");
		sprintf(STR_FANCTRL4,    "CTRL DYN FAN");
		sprintf(STR_FANCTRL5,    "CTRL MIN FAN");

		sprintf(STR_COPYING,     "Copying");
		sprintf(STR_CPYDEST,     "Destination");
		sprintf(STR_CPYFINISH,   "Copy Finished!");
		sprintf(STR_CPYABORT,    "Copy aborted!");
		sprintf(STR_DELETE,      "Delete");

		sprintf(STR_SCAN1,       "Scan these devices");
		sprintf(STR_SCAN2,       "Scan for content");
		sprintf(STR_PSPL,        "Show PSP Launcher");
		sprintf(STR_PS2L,        "Show PS2 Classic Launcher");
		sprintf(STR_RXVID,       "Show Video sub-folder");
		sprintf(STR_VIDLG,       "Video");
		sprintf(STR_LPG,         "Load last-played game on startup");
		sprintf(STR_AUTOB,       "Check for /dev_hdd0/PS3ISO/AUTOBOOT.ISO on startup");
		sprintf(STR_DELAYAB,     "Delay loading of AUTOBOOT.ISO/last-game (Disc Auto-start)");
		sprintf(STR_DEVBL,       "Enable /dev_blind (writable /dev_flash) on startup");
		sprintf(STR_CONTSCAN,    "Disable content scan on startup");

		sprintf(STR_USBPOLL,     "Disable USB polling");
		sprintf(STR_FTPSVC,      "Disable FTP service");
		sprintf(STR_FIXGAME,     "Disable auto-fix game");
		sprintf(STR_COMBOS,      "Disable all PAD shortcuts");
		sprintf(STR_MMCOVERS,    "Disable multiMAN covers");
		sprintf(STR_ACCESS,      "Disable remote access to FTP/WWW services");
		sprintf(STR_NOSETUP,     "Disable webMAN Setup entry in \"webMAN Games\"");

		sprintf(STR_SAVE,        "Save");
		sprintf(STR_SETTINGSUPD, "%s%s", "Settings updated.<br>", "<br>Click <a href=\"/restart.ps3\">here</a> to restart your PLAYSTATION®3 system.");
		sprintf(STR_ERROR,       "Error!");

		sprintf(STR_MYGAMES,     "webMAN Games");
		sprintf(STR_LOADGAMES,   "Load games with webMAN");
		sprintf(STR_FIXING,      "Fixing");

		sprintf(STR_WMSETUP,     "webMAN Setup");
		sprintf(STR_WMSETUP2,    "Setup webMAN options");

		sprintf(STR_EJECTDISC,   "Eject Disc");
		sprintf(STR_UNMOUNTGAME, "Unmount current game");

		sprintf(STR_WMSTART,     "webMAN loaded!");
		sprintf(STR_WMUNL,       "webMAN unloaded!");
		sprintf(STR_CFWSYSALRD,  "CFW Syscalls already disabled");
		sprintf(STR_CFWSYSRIP,   "Removal History files & CFW Syscalls in progress...");
		sprintf(STR_RMVCFWSYS,   "History files & CFW Syscalls deleted OK!");
		sprintf(STR_RMVCFWSYSF,  "Failed to remove CFW Syscalls");

		sprintf(STR_RMVWMCFG,    "webMAN config reset in progress...");
		sprintf(STR_RMVWMCFGOK,  "Done! Restart within 3 seconds");

		sprintf(STR_PS3FORMAT,   "PS3 format games");
		sprintf(STR_PS2FORMAT,   "PS2 format games");
		sprintf(STR_PS1FORMAT,   "PSOne format games");
		sprintf(STR_PSPFORMAT,   "PSP\xE2\x84\xA2 format games");

		sprintf(STR_VIDFORMAT,   "Blu-ray\xE2\x84\xA2 and DVD");
		sprintf(STR_VIDEO,       "Video content");

		sprintf(STR_LAUNCHPSP,   "Launch PSP ISO mounted through webMAN or mmCM");
		sprintf(STR_LAUNCHPS2,   "Launch PS2 Classic");

		sprintf(STR_GAMEUM,      "Game unmounted.");

		sprintf(STR_EJECTED,     "Disc ejected.");
		sprintf(STR_LOADED,      "Disc inserted.");

		sprintf(STR_GAMETOM,     "Game to mount");
		sprintf(STR_MOVIETOM,    "Movie to mount");
		sprintf(STR_LOADED2,     "loaded   ");

		sprintf(STR_PSPLOADED,   "Game %s%s%s</b>.<hr>",
								 "loaded successfully. Start the ", "game using <b>", "PSP Launcher");
		sprintf(STR_PS2LOADED,   "Game %s%s%s</b>.<hr>",
								 "loaded successfully. Start the ", "game using <b>", "PS2 Classic Launcher");
		sprintf(STR_GAMELOADED,  "Game %s%s%sgame.",
								 "loaded successfully. Start the ", "game from the disc icon<br>or from <b>/app_home</b>&nbsp;XMB entry", ".</a><hr>Click <a href=\"/mount.ps3/unmount\">here</a> to unmount the ");
		sprintf(STR_MOVIELOADED, "Movie %s%s%smovie.",
								 "loaded successfully. Start the ", "movie from the disc icon<br>under the Video column"                , ".</a><hr>Click <a href=\"/mount.ps3/unmount\">here</a> to unmount the ");

		sprintf(STR_XMLRF, "%s%s", "Game list refreshed (<a href=\"" MY_GAMES_XML "\">mygames.xml</a>).", "<br>Click <a href=\"/restart.ps3\">here</a> to restart your PLAYSTATION®3 system.");

		sprintf(STR_STORAGE,     "System storage");
		sprintf(STR_MEMORY,      "Memory available");
		sprintf(STR_MBFREE,      "MB free");
		sprintf(STR_KBFREE,      "KB free");

		sprintf(STR_FANCTRL3,    "Fan control:");
		sprintf(STR_ENABLED,     "Enabled");
		sprintf(STR_DISABLED,    "Disabled");

		sprintf(STR_FANCH0,      "Fan setting changed:");
		sprintf(STR_FANCH1,      "MAX TEMP: ");
		sprintf(STR_FANCH2,      "FAN SPEED: ");
		sprintf(STR_FANCH3,      "MIN FAN SPEED: ");

		sprintf(STR_OVERHEAT,     "System overheat warning!");
		sprintf(STR_OVERHEAT2,    "  OVERHEAT DANGER!\nFAN SPEED INCREASED!");

		sprintf(STR_NOTFOUND,     "Not found!");

		COVERS_PATH[0] = NULL;

		sprintf(search_url,       "http://google.com/search?q=");

		do_once = false;
	}

	if(language("STR_TRADBY", STR_TRADBY))
	{
		language("STR_FILES", STR_FILES);
		language("STR_GAMES", STR_GAMES);
		language("STR_SETUP", STR_SETUP);
		language("STR_HOME", STR_HOME);
		language("STR_EJECT", STR_EJECT);
		language("STR_INSERT", STR_INSERT);
		language("STR_UNMOUNT", STR_UNMOUNT);
		language("STR_COPY", STR_COPY);
		language("STR_REFRESH", STR_REFRESH);
		language("STR_SHUTDOWN", STR_SHUTDOWN);
		language("STR_RESTART", STR_RESTART);

		language("STR_BYTE", STR_BYTE);
		language("STR_KILOBYTE", STR_KILOBYTE);
		language("STR_MEGABYTE", STR_MEGABYTE);
		language("STR_GIGABYTE", STR_GIGABYTE);

		language("STR_COPYING", STR_COPYING);
		language("STR_CPYDEST", STR_CPYDEST);
		language("STR_CPYFINISH", STR_CPYFINISH);
		language("STR_CPYABORT", STR_CPYABORT);
		language("STR_DELETE", STR_DELETE);

		language("STR_SCAN1", STR_SCAN1);
		language("STR_SCAN2", STR_SCAN2);
		language("STR_PSPL", STR_PSPL);
		language("STR_PS2L", STR_PS2L);
		language("STR_RXVID", STR_RXVID);
		language("STR_VIDLG", STR_VIDLG	);
		language("STR_LPG", STR_LPG);
		language("STR_AUTOB", STR_AUTOB);
		language("STR_DELAYAB", STR_DELAYAB);
		language("STR_DEVBL", STR_DEVBL);
		language("STR_CONTSCAN", STR_CONTSCAN);
		language("STR_USBPOLL", STR_USBPOLL);
		language("STR_FTPSVC", STR_FTPSVC);
		language("STR_FIXGAME", STR_FIXGAME);
		language("STR_COMBOS", STR_COMBOS);
		language("STR_MMCOVERS", STR_MMCOVERS);
		language("STR_ACCESS", STR_ACCESS);
		language("STR_NOSETUP", STR_NOSETUP);
		language("STR_NOSPOOF", STR_NOSPOOF);
		language("STR_NOGRP", STR_NOGRP);
		language("STR_NOWMDN", STR_NOWMDN);
#ifdef NOSINGSTAR
		language("STR_NOSINGSTAR", STR_NOSINGSTAR);
#endif
		language("STR_AUTO_PLAY", STR_AUTO_PLAY);
		language("STR_RESET_USB", STR_RESET_USB);
		language("STR_TITLEID", STR_TITLEID);
		language("STR_FANCTRL", STR_FANCTRL);
		language("STR_NOWARN", STR_NOWARN);
		language("STR_AUTOAT", STR_AUTOAT);
		language("STR_LOWEST", STR_LOWEST);
		language("STR_FANSPEED", STR_FANSPEED);
		language("STR_MANUAL", STR_MANUAL);
		language("STR_PS2EMU", STR_PS2EMU);
		language("STR_LANGAMES", STR_LANGAMES);
		language("STR_ANYUSB", STR_ANYUSB);
		language("STR_ADDUSB", STR_ADDUSB);
		language("STR_SPOOFID", STR_SPOOFID);
		language("STR_DELCFWSYS", STR_DELCFWSYS);
		language("STR_MEMUSAGE", STR_MEMUSAGE);
		language("STR_PLANG", STR_PLANG);
		language("STR_PROFILE", STR_PROFILE);
		language("STR_DEFAULT", STR_DEFAULT);
		language("STR_COMBOS2", STR_COMBOS2);
		language("STR_FAILSAFE", STR_FAILSAFE);
		language("STR_SHOWTEMP", STR_SHOWTEMP);
		language("STR_SHOWIDPS", STR_SHOWIDPS);
		language("STR_PREVGAME", STR_PREVGAME);
		language("STR_NEXTGAME", STR_NEXTGAME);
		language("STR_SHUTDOWN2", STR_SHUTDOWN2);
		language("STR_RESTART2", STR_RESTART2);
#ifdef REMOVE_SYSCALLS
		language("STR_DELCFWSYS2", STR_DELCFWSYS2);
#endif
		language("STR_UNLOADWM", STR_UNLOADWM);
		language("STR_FANCTRL2", STR_FANCTRL2);
		language("STR_FANCTRL4", STR_FANCTRL4);
		language("STR_FANCTRL5", STR_FANCTRL5);
		language("STR_UPDN", STR_UPDN);
		language("STR_LFRG", STR_LFRG);

		language("STR_SAVE", STR_SAVE);
		language("STR_SETTINGSUPD", STR_SETTINGSUPD);
		language("STR_ERROR", STR_ERROR);

		language("STR_MYGAMES", STR_MYGAMES);
		language("STR_LOADGAMES", STR_LOADGAMES);
		language("STR_FIXING", STR_FIXING);

		language("STR_WMSETUP", STR_WMSETUP);
		language("STR_WMSETUP2", STR_WMSETUP2);

		language("STR_EJECTDISC", STR_EJECTDISC);
		language("STR_UNMOUNTGAME", STR_UNMOUNTGAME);

		language("STR_WMSTART", STR_WMSTART);
		language("STR_WMUNL", STR_WMUNL);
		language("STR_CFWSYSALRD", STR_CFWSYSALRD);
		language("STR_CFWSYSRIP", STR_CFWSYSRIP);
		language("STR_RMVCFWSYS", STR_RMVCFWSYS);
		language("STR_RMVCFWSYSF", STR_RMVCFWSYSF);

		language("STR_RMVWMCFG", STR_RMVWMCFG);
		language("STR_RMVWMCFGOK", STR_RMVWMCFGOK);

		language("STR_PS3FORMAT", STR_PS3FORMAT);
		language("STR_PS2FORMAT", STR_PS2FORMAT);
		language("STR_PS1FORMAT", STR_PS1FORMAT);
		language("STR_PSPFORMAT", STR_PSPFORMAT);

		language("STR_VIDFORMAT", STR_VIDFORMAT);
		language("STR_VIDEO", STR_VIDEO);

		language("STR_LAUNCHPSP", STR_LAUNCHPSP);
		language("STR_LAUNCHPS2", STR_LAUNCHPS2);

		language("STR_GAMEUM", STR_GAMEUM);

		language("STR_EJECTED", STR_EJECTED);
		language("STR_LOADED", STR_LOADED);

		language("STR_GAMETOM", STR_GAMETOM);
		language("STR_GAMELOADED", STR_GAMELOADED);
		language("STR_PSPLOADED", STR_PSPLOADED);
		language("STR_PS2LOADED", STR_PS2LOADED);
		language("STR_LOADED2", STR_LOADED2);

		language("STR_MOVIETOM", STR_MOVIETOM);
		language("STR_MOVIELOADED", STR_MOVIELOADED);

		language("STR_XMLRF", STR_XMLRF);

		language("STR_STORAGE", STR_STORAGE);
		language("STR_MEMORY", STR_MEMORY);
		language("STR_MBFREE", STR_MBFREE);
		language("STR_KBFREE", STR_KBFREE);

		language("STR_FANCTRL3", STR_FANCTRL3);
		language("STR_ENABLED", STR_ENABLED);
		language("STR_DISABLED", STR_DISABLED);

		language("STR_FANCH0", STR_FANCH0);
		language("STR_FANCH1", STR_FANCH1);
		language("STR_FANCH2", STR_FANCH2);
		language("STR_FANCH3", STR_FANCH3);

		language("STR_OVERHEAT", STR_OVERHEAT);
		language("STR_OVERHEAT2", STR_OVERHEAT2);

		language("STR_NOTFOUND", STR_NOTFOUND);

		language("COVERS_PATH", COVERS_PATH);
		language("IP_ADDRESS", local_ip);
		language("SEARCH_URL", search_url);
/*
#ifdef COBRA_ONLY
		language("STR_DISCOBRA", STR_DISCOBRA);
#endif
#ifdef REX_ONLY
		language("STR_RBGMODE", STR_RBGMODE);
		language("STR_RBGNORM", STR_RBGNORM);
		language("STR_RBGMENU", STR_RBGMENU);
#endif
*/
	}

	if(fh) {cellFsClose(fh); fh = 0;}

	html_base_path[0] = NULL;
}
#endif //#ifndef ENGLISH_ONLY
