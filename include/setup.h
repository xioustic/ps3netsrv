//combo
#define FAIL_SAFE (1<<0)
#define SHOW_TEMP (1<<1)
#define PREV_GAME (1<<2)
#define NEXT_GAME (1<<3)
#define SHUT_DOWN (1<<4)
#define RESTARTPS (1<<5)
#define UNLOAD_WM (1<<6)
#define MANUALFAN (1<<7)
#define SHOW_IDPS (1<<8)
#define DISABLESH (1<<9)
#define DISABLEFC (1<<10)
#define MINDYNFAN (1<<11)
#define DISACOBRA (1<<12)


//combo2
#define EXTGAMDAT (1<<0)
#define MOUNTNET0 (1<<1)
#define MOUNTNET1 (1<<2)
#define PS2TOGGLE (1<<3)
#define PS2SWITCH (1<<4)
#define CUSTOMCMB (1<<5)
#define XMLREFRSH (1<<6)
#define UMNT_GAME (1<<7)
#define VIDRECORD (1<<8)
#define PLAY_DISC (1<<9)
#define INSTALPKG (1<<10)

#define REBUGMODE (1<<13)
#define NORMAMODE (1<<14)
#define DEBUGMENU (1<<15)

static void setup_parse_settings(char *param)
{
	memset(webman_config, 0, sizeof(WebmanCfg));

	char *pos;

	if(strstr(param, "u0=1")) webman_config->usb0 = 1;
	if(strstr(param, "u1=1")) webman_config->usb1 = 1;
	if(strstr(param, "u2=1")) webman_config->usb2 = 1;
	if(strstr(param, "u3=1")) webman_config->usb3 = 1;
	if(strstr(param, "u6=1")) webman_config->usb6 = 1;
	if(strstr(param, "u7=1")) webman_config->usb7 = 1;

	if(strstr(param, "x0=1")) webman_config->dev_sd = 1;
	if(strstr(param, "x1=1")) webman_config->dev_ms = 1;
	if(strstr(param, "x2=1")) webman_config->dev_cf = 1;

	if(strstr(param, "lp=1")) webman_config->lastp = 1;
	if(strstr(param, "ab=1")) webman_config->autob = 1;
	if(strstr(param, "dy=1")) webman_config->delay = 1;

	//Wait for any USB device to be ready
	webman_config->bootd=get_valuen(param, "&b=", 0, 30);

	//Wait additionally for each selected USB device to be ready
	webman_config->boots=get_valuen(param, "&s=", 0, 30);

	if(strstr(param, "bl=1")) webman_config->blind = 1;
	if(webman_config->blind)
		enable_dev_blind(NO_MSG);
	else
		disable_dev_blind();

	if(strstr(param, "ns=1")) webman_config->nosetup = 1;
	if(strstr(param, "ng=1")) webman_config->nogrp   = 1;
#ifdef NOSINGSTAR
	if(strstr(param, "ss=1")) {webman_config->noss = 1; no_singstar_icon();}
#endif

#ifdef COBRA_ONLY
	webman_config->cmask=0;
	if(!strstr(param, "ps1=1")) webman_config->cmask|=PS1;
	if(!strstr(param, "psp=1")) webman_config->cmask|=PSP;
	if(!strstr(param, "blu=1")) webman_config->cmask|=BLU;
	if(!strstr(param, "dvd=1")) webman_config->cmask|=DVD;
#else
	webman_config->cmask=(PSP | PS1 | BLU | DVD);
#endif
	if(!strstr(param, "ps3=1")) webman_config->cmask|=PS3;
	if(!strstr(param, "ps2=1")) webman_config->cmask|=PS2;

	if(strstr(param, "psl=1")) webman_config->pspl = 1;
	if(strstr(param, "p2l=1")) webman_config->ps2l = 1;
	if(strstr(param, "rxv=1")) webman_config->rxvid = 1;
	if(strstr(param, "pse=1")) webman_config->ps1emu = 1;

	webman_config->combo=webman_config->combo2=0;
	if(!strstr(param, "pfs=1")) webman_config->combo|=FAIL_SAFE;
	if(!strstr(param, "pss=1")) webman_config->combo|=SHOW_TEMP;
	if(!strstr(param, "ppv=1")) webman_config->combo|=PREV_GAME;
	if(!strstr(param, "pnx=1")) webman_config->combo|=NEXT_GAME;
	if(!strstr(param, "psd=1")) webman_config->combo|=SHUT_DOWN;
	if(!strstr(param, "pid=1")) webman_config->combo|=SHOW_IDPS;
	if(!strstr(param, "prs=1")) webman_config->combo|=RESTARTPS;
	if(!strstr(param, "puw=1")) webman_config->combo|=UNLOAD_WM;
	if(!strstr(param, "pf1=1")) webman_config->combo|=MANUALFAN;
	if(!strstr(param, "pf2=1")) webman_config->combo|=MINDYNFAN;
	if(!strstr(param, "pdf=1")) webman_config->combo|=DISABLEFC;
	if(!strstr(param, "psc=1")) webman_config->combo|=DISABLESH;
	if(!strstr(param, "kcc=1")) webman_config->keep_ccapi = true;

#ifdef COBRA_ONLY
 #ifndef LITE_EDITION
	if(!strstr(param, "pdc=1")) webman_config->combo|=DISACOBRA;
 #endif

	if(strstr(param, "bus=1")) webman_config->bus = 1;
#endif
	if(strstr(param, "apd=1")) webman_config->autoplay = 1;
#ifdef REX_ONLY
	if(!strstr(param, "pr0=1")) webman_config->combo2|=REBUGMODE;
	if(!strstr(param, "pr1=1")) webman_config->combo2|=NORMAMODE;
	if(!strstr(param, "pr2=1")) webman_config->combo2|=DEBUGMENU;

	if(!strstr(param, "p2c=1")) webman_config->combo2|=PS2TOGGLE;
#endif

#ifdef PKG_HANDLER
	if(!strstr(param, "pkg=1")) webman_config->combo2|=INSTALPKG;
#endif
	if(!strstr(param, "pgd=1")) webman_config->combo2|=EXTGAMDAT;
#ifndef LITE_EDITION
	if(!strstr(param, "p2s=1")) webman_config->combo2|=PS2SWITCH;
	if(!strstr(param, "pn0=1")) webman_config->combo2|=MOUNTNET0;
	if(!strstr(param, "pn1=1")) webman_config->combo2|=MOUNTNET1;
#endif
	if(!strstr(param, "psv=1")) webman_config->combo2|=CUSTOMCMB;
	if(!strstr(param, "pxr=1")) webman_config->combo2|=XMLREFRSH;
	if(!strstr(param, "umt=1")) webman_config->combo2|=UMNT_GAME;
	if(!strstr(param, "pld=1")) webman_config->combo2|=PLAY_DISC;

#ifdef VIDEO_REC
	if(!strstr(param, "vrc=1")) webman_config->combo2|=VIDRECORD;
#endif

	if( strstr(param, "wn=1"))  webman_config->wmstart = 1;
	if( strstr(param, "tid=1")) webman_config->tid  = 1;
	if(!strstr(param, "sfo=1")) webman_config->use_filename = 1; // show filename instead of title in PARAM.SFO
	if( strstr(param, "pl=1" )) webman_config->poll = 1;
	if( strstr(param, "ft=1" )) webman_config->ftpd = 1;
	if( strstr(param, "np=1" )) webman_config->nopad = 1;
	if( strstr(param, "nc=1" )) webman_config->nocov = SHOW_ICON0;    else // (0 = Use MM covers, 1 = Use ICON0.PNG, 2 = No game icons, 3 = Online Covers)
	if( strstr(param, "ic=1" )) webman_config->nocov = SHOW_ICON0;    else
	if( strstr(param, "ic=2" )) webman_config->nocov = SHOW_DISC;     else
	if( strstr(param, "ic=3" )) webman_config->nocov = ONLINE_COVERS;

	if( strstr(param, "nd=1" )) webman_config->netd = 1;
	webman_config->netp=get_valuen16(param, "netp=");
	if(webman_config->netp == 0) webman_config->netp = NETPORT;

#ifdef FIX_GAME
	if(strstr(param, "fm=0")) webman_config->fixgame=FIX_GAME_AUTO;
	if(strstr(param, "fm=1")) webman_config->fixgame=FIX_GAME_QUICK;
	if(strstr(param, "fm=2")) webman_config->fixgame=FIX_GAME_FORCED;
	if(strstr(param, "nf=3")) webman_config->fixgame=FIX_GAME_DISABLED;
#endif

	if(strstr(param, "nsp=1")) webman_config->nospoof = 1; //don't spoof fw version
	if(c_firmware==4.53f) webman_config->nospoof = 1;

	if(strstr(param, "fc=1")) webman_config->fanc = 1;

	webman_config->temp1=MY_TEMP;

	webman_config->minfan=get_valuen(param, "mfan=", MIN_FANSPEED, 99); //%

	webman_config->bind=0;
	if(strstr(param, "bind")) webman_config->bind = 1;

	pos = strstr(param, "pwd=");
	if(pos) get_value(webman_config->ftp_password, pos + 4, 20);

	webman_config->refr=0;
	if(strstr(param, "rf=1")) webman_config->refr = 1;

#ifdef LAUNCHPAD
	if(strstr(param, "lx=1")) webman_config->launchpad_xml = 1;
#endif

	webman_config->temp0=0;

	webman_config->temp1   = get_valuen(param, "step=", 40, MAX_TEMPERATURE); //°C
	webman_config->ps2temp = get_valuen(param, "fsp0=", 20, 99); // %
	webman_config->manu    = get_valuen(param, "manu=", 20, 95); // %

	if(strstr(param, "temp=1"))
		webman_config->temp0= (u8)(((float)(webman_config->manu+1) * 255.f)/100.f); // manual fan speed
	else
		webman_config->temp0=0; // dynamic fan control mode

	max_temp=0;
	if(webman_config->fanc)
	{
		if(webman_config->temp0==0) max_temp=webman_config->temp1; // dynamic fan max temperature in °C
		fan_control(webman_config->temp0, 0);
	}
	else
		restore_fan(0); //restore syscon fan control mode

	webman_config->warn=0;
	if(strstr(param, "warn=1")) webman_config->warn = 1;

	webman_config->foot=get_valuen(param, "fp=", 0, 6);

	webman_config->spp=0;
#ifdef COBRA_ONLY
	#ifdef REMOVE_SYSCALLS
	if(strstr(param, "spp=1"))  webman_config->spp|=1;  //remove syscalls & history
	#endif
	if(strstr(param, "shh=1"))  webman_config->spp|=2;  //remove history & block psn servers (offline mode)
	if(strstr(param, "shh=2"))  webman_config->spp|=4;  //offline mode in game
#endif

#ifdef SPOOF_CONSOLEID
	if(strstr(param, "id1=1"))  webman_config->sidps = 1; //spoof IDPS
	if(strstr(param, "id2=1"))  webman_config->spsid = 1; //spoof PSID

	pos = strstr(param, "vID1=");
	if(pos) get_value(webman_config->vIDPS1, pos + 5, 16);

	pos = strstr(param, "vID2=");
	if(pos) get_value(webman_config->vIDPS2, pos + 5, 16);

	pos = strstr(param, "vPS1=");
	if(pos) get_value(webman_config->vPSID1, pos + 5, 16);

	pos = strstr(param, "vPS2=");
	if(pos) get_value(webman_config->vPSID2, pos + 5, 16);

	spoof_idps_psid();
#endif

#ifdef VIDEO_REC
	char value[8];

	// set video format
	pos = strstr(param, "vif=");
	if(pos)
	{
		get_value(value, pos + 4, 4);
		rec_video_format = webman_config->rec_video_format = convertH(value);
	}
	// set audio format
	pos = strstr(param, "auf=");
	if(pos)
	{
		get_value(value, pos + 4, 4);
		rec_audio_format = webman_config->rec_audio_format = convertH(value);
	}
#endif

	webman_config->lang=0; //English

#ifndef ENGLISH_ONLY
	if(strstr(param, "&l=99")) webman_config->lang=99; // Unknown LANG_XX.TXT
	else
		webman_config->lang=get_valuen(param, "&l=", 0, 22);

	update_language();
#endif
	webman_config->neth0[0] = webman_config->neth1[0] = webman_config->neth2[0] = NULL;
	webman_config->netp0    = webman_config->netp1    = webman_config->netp2 = NETPORT;
#ifdef NET3NET4
	webman_config->neth3[0] = webman_config->neth4[0] = 0;
	webman_config->netp3    = webman_config->netp4 = NETPORT;
#endif
#ifdef COBRA_ONLY
 #ifndef LITE_EDITION
	{
		if(strstr(param, "nd0="))  webman_config->netd0 = 1;
		if(strstr(param, "nd1="))  webman_config->netd1 = 1;
		if(strstr(param, "nd2="))  webman_config->netd2 = 1;
#ifdef NET3NET4
		if(strstr(param, "nd3="))  webman_config->netd3 = 1;
		if(strstr(param, "nd4="))  webman_config->netd4 = 1;
#endif
		pos = strstr(param, "neth0=");
		if(pos)
		{
			get_value(webman_config->neth0, pos + 6, 16);
			webman_config->netp0=get_valuen16(param, "netp0=");
		}

		pos = strstr(param, "neth1=");
		if(pos)
		{
			get_value(webman_config->neth1, pos + 6, 16);
			webman_config->netp1=get_valuen16(param, "netp1=");
		}

		pos = strstr(param, "neth2=");
		if(pos)
		{
			get_value(webman_config->neth2, pos + 6, 16);
			webman_config->netp2=get_valuen16(param, "netp2=");
		}
#ifdef NET3NET4
		pos = strstr(param, "neth3=");
		if(pos)
		{
			get_value(webman_config->neth3, pos + 6, 16);
			webman_config->netp3=get_valuen16(param, "netp3=");
		}

		pos = strstr(param, "neth4=");
		if(pos)
		{
			get_value(webman_config->neth4, pos + 6, 16);
			webman_config->netp4=get_valuen16(param, "netp4=");
		}
#endif
		pos = strstr(param, "aip=");
		if(pos) get_value(webman_config->allow_ip, pos + 4, 16);
	}
 #endif
#endif
	pos = strstr(param, "autop=");
	if(pos) get_value(webman_config->autoboot_path, pos + 6, 255);
	if(webman_config->autoboot_path[0] == NULL) strcpy(webman_config->autoboot_path, DEFAULT_AUTOBOOT_PATH);

#ifndef LITE_EDITION
	pos = strstr(param, "uacc=");
	if(pos) get_value(webman_config->uaccount, pos + 5, 8);

	if(strstr(param, "hm=")) webman_config->homeb = 1;

	pos = strstr(param, "hurl=");
	if(pos) get_value(webman_config->home_url, pos + 5, 255);
#endif

#ifdef COBRA_ONLY
#ifdef BDVD_REGION
		//if(cobra_mode)
		{
			u8 cconfig[15];
			CobraConfig *cobra_config = (CobraConfig*) cconfig;
			memset(cobra_config, 0, 15);
			cobra_read_config(cobra_config);

			cobra_config->bd_video_region  = get_valuen(param, "bdr=", 0, 4);  //BD Region
			cobra_config->dvd_video_region = get_valuen(param, "dvr=", 0, 32); //DVD Region

			cobra_write_config(cobra_config);

		}
#endif
#endif

#if defined(WM_CUSTOM_COMBO) || defined(WM_REQUEST)
	char command[256]; size_t cmdlen = 0; memset(command, 0, 256);

	pos = strstr(param, "ccbo=");
	if(pos) cmdlen = get_value(command, pos + 5, 255);

 #ifdef WM_CUSTOM_COMBO
	if(savefile(WM_CUSTOM_COMBO "r2_square", command, cmdlen) != CELL_FS_SUCCEEDED)
 #endif
	savefile("/dev_hdd0/tmp/wm_custom_combo", command, cmdlen);
#endif
}

static void setup_form(char *buffer, char *templn)
{
 #ifndef ENGLISH_ONLY
	char STR_SCAN1[48];//		= "Scan these devices";
	char STR_PSPL[40];//			= "Show PSP Launcher";
	char STR_PS2L[48];//			= "Show PS2 Classic Launcher";
	char STR_RXVID[64];//		= "Show Video sub-folder";
	char STR_LPG[128];//			= "Load last-played game on startup";
	char STR_AUTOB[96];//		= "Check for /dev_hdd0/PS3ISO/AUTOBOOT.ISO on startup";
	char STR_DELAYAB[168];//		= "Delay loading of AUTOBOOT.ISO/last-game (Disc Auto-start)";
	char STR_DEVBL[112];//		= "Enable /dev_blind (writable /dev_flash) on startup";
	char STR_CONTSCAN[120];//	= "Disable content scan on startup";
	char STR_USBPOLL[88];//		= "Disable USB polling";
	char STR_FTPSVC[64];//		= "Disable FTP service";
	char STR_FIXGAME[56];//		= "Disable auto-fix game";
	char STR_COMBOS[88];//		= "Disable all PAD shortcuts";
	char STR_MMCOVERS[72];//		= "Disable multiMAN covers";
	char STR_ACCESS[88];//		= "Disable remote access to FTP/WWW services";
	char STR_NOSETUP[120];//		= "Disable webMAN Setup entry in \"webMAN Games\"";
	char STR_NOSPOOF[96];//		= "Disable firmware version spoofing";
	char STR_NOGRP[104];//		= "Disable grouping of content in \"webMAN Games\"";
	char STR_NOWMDN[112];//		= "Disable startup notification of WebMAN on the XMB";
  #ifdef NOSINGSTAR
	static char STR_NOSINGSTAR[48];//	= "Remove SingStar icon";
  #endif
	char STR_AUTO_PLAY[24];//	= "Auto-Play";
	char STR_RESET_USB[48];//	= "Disable Reset USB Bus";
	char STR_TITLEID[128];//		= "Include the ID as part of the title of the game";
	char STR_FANCTRL[96];//		= "Enable dynamic fan control";
	char STR_NOWARN[96];//		= "Disable temperature warnings";
	char STR_AUTOAT[32];//		= "Auto at";
	char STR_LOWEST[24];//		= "Lowest";
	char STR_FANSPEED[48];//		= "fan speed";

	char STR_PS2EMU[32];//		= "PS2 Emulator";
	char STR_LANGAMES[96];//		= "Scan for LAN games/videos";
	char STR_ANYUSB[88];//		= "Wait for any USB device to be ready";
	char STR_ADDUSB[136];//		= "Wait additionally for each selected USB device to be ready";
	char STR_SPOOFID[112];//		= "Change idps and psid in lv2 memory at system startup";
	char STR_DELCFWSYS[144];//	= "Disable CFW syscalls and delete history files at system startup";
	char STR_MEMUSAGE[80];//		= "Plugin memory usage";
	char STR_PLANG[40];//		= "Plugin language";
	char STR_PROFILE[16];//		= "Profile";
	char STR_DEFAULT[32];//		= "Default";
	char STR_COMBOS2[80];//		= "XMB/In-Game PAD SHORTCUTS";
	char STR_FAILSAFE[40];//		= "FAIL SAFE";
	char STR_SHOWTEMP[56];//		= "SHOW TEMP";
	char STR_SHOWIDPS[24];//		= "SHOW IDPS";
	char STR_PREVGAME[64];//		= "PREV GAME";
	char STR_NEXTGAME[56];//		= "NEXT GAME";
	char STR_SHUTDOWN2[32];//	= "SHUTDOWN ";
	char STR_RESTART2[32];//		= "RESTART&nbsp; ";
#ifdef REMOVE_SYSCALLS
	char STR_DELCFWSYS2[48];//	= "DEL CFW SYSCALLS";
#endif
	char STR_UNLOADWM[64];//		= "UNLOAD WM";
	char STR_FANCTRL2[48];//		= "CTRL FAN";
	char STR_FANCTRL4[72];//		= "CTRL DYN FAN";
	char STR_FANCTRL5[88];//		= "CTRL MIN FAN";
	char STR_UPDN[16]			= "&#8593;/&#8595;"; //↑/↓
	char STR_LFRG[16]			= "&#8592;/&#8594;"; //←/→

	sprintf(STR_SCAN1,       "Scan these devices");
	sprintf(STR_PSPL,        "Show PSP Launcher");
	sprintf(STR_PS2L,        "Show PS2 Classic Launcher");
	sprintf(STR_RXVID,       "Show Video sub-folder");
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
	sprintf(STR_NOSPOOF,     "Disable firmware version spoofing");
	sprintf(STR_NOGRP,       "Disable grouping of content in \"webMAN Games\"");
	sprintf(STR_NOWMDN,      "Disable startup notification of WebMAN on the XMB");
  #ifdef NOSINGSTAR
	sprintf(STR_NOSINGSTAR,  "Remove SingStar icon");
  #endif
	sprintf(STR_AUTO_PLAY,   "Auto-Play");
	sprintf(STR_RESET_USB,   "Disable Reset USB Bus");
	sprintf(STR_TITLEID,     "Include the ID as part of the title of the game");
	sprintf(STR_FANCTRL,     "Enable dynamic fan control");
	sprintf(STR_NOWARN,      "Disable temperature warnings");
	sprintf(STR_AUTOAT,      "Auto at");
	sprintf(STR_LOWEST,      "Lowest");
	sprintf(STR_FANSPEED,    "fan speed");

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

	language("STR_SCAN1", STR_SCAN1);
	language("STR_PSPL", STR_PSPL);
	language("STR_PS2L", STR_PS2L);
	language("STR_RXVID", STR_RXVID);
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
	//language("STR_UPDN", STR_UPDN);
	//language("STR_LFRG", STR_LFRG);

	language("/CLOSEFILE", NULL);
 #endif

	sprintf(templn, "<style>td+td{text-align:left;white-space:nowrap}</style>"
					"<form action=\"/setup.ps3\" method=\"get\" enctype=\"application/x-www-form-urlencoded\" target=\"_self\">"
					"<table width=\"820\" border=\"0\" cellspacing=\"2\" cellpadding=\"0\">"
					"<tr><td width=\"250\"><u>%s:</u><br>", STR_SCAN1); strcat(buffer, templn);

	//Scan these devices
	add_check_box("u0", "1", drives[1], _BR_, (webman_config->usb0), buffer);
	add_check_box("u1", "1", drives[2], _BR_, (webman_config->usb1), buffer);
	add_check_box("u2", "1", drives[3], _BR_, (webman_config->usb2), buffer);
	add_check_box("u3", "1", drives[4], _BR_, (webman_config->usb3), buffer);
	add_check_box("u6", "1", drives[5], _BR_, (webman_config->usb6), buffer);
	add_check_box("u7", "1", drives[6], _BR_, (webman_config->usb7), buffer);

	if(isDir(drives[11])) add_check_box("x0", "1", drives[11], _BR_, (webman_config->dev_sd), buffer);
	if(isDir(drives[12])) add_check_box("x1", "1", drives[12], _BR_, (webman_config->dev_ms), buffer);
	if(isDir(drives[13])) add_check_box("x2", "1", drives[13], _BR_, (webman_config->dev_cf), buffer);

	//Scan for content
	sprintf(templn, "<td nowrap valign=top><u>%s:</u><br>", STR_SCAN2); strcat(buffer, templn);

	add_check_box("ps3", "1", "PLAYSTATION\xC2\xAE\x33"    , _BR_     , !(webman_config->cmask & PS3), buffer);
	add_check_box("ps2", "1", "PLAYSTATION\xC2\xAE\x32"    , " ("     , !(webman_config->cmask & PS2), buffer);
	add_check_box("p2l", "1", STR_PS2L                     , ")<br>"  ,  (webman_config->ps2l)       , buffer);
#ifdef COBRA_ONLY
	add_check_box("ps1", "1", "PLAYSTATION\xC2\xAE&nbsp;"  , " ("     , !(webman_config->cmask & PS1), buffer);
	add_check_box("pse", "1", "ps1_netemu"                 , ")<br>"  ,  (webman_config->ps1emu)     , buffer);

    add_check_box("psp", "1", "PLAYSTATION\xC2\xAEPORTABLE", " ("     , !(webman_config->cmask & PSP), buffer);
    add_check_box("psl", "1", STR_PSPL                     , ")<br>"    ,(webman_config->pspl)       , buffer);
	add_check_box("blu", "1", "Blu-ray\xE2\x84\xA2"        , " ("     , !(webman_config->cmask & BLU), buffer);
	add_check_box("rxv", "1", STR_RXVID                    , ")<br>"  ,  (webman_config->rxvid)      , buffer);

	add_check_box("dvd", "1", "DVD "                       , STR_VIDLG, !(webman_config->cmask & DVD), buffer);
#endif

	//general settings
	strcat(buffer, "</td></tr></table>" HTML_BLU_SEPARATOR);

	add_check_box("lp", "1", STR_LPG    , _BR_, (webman_config->lastp), buffer);
	add_check_box("ab", "1", STR_AUTOB  , _BR_, (webman_config->autob), buffer);
	add_check_box("dy", "1", STR_DELAYAB, _BR_, (webman_config->delay), buffer);

	add_check_box("bl", "1", STR_DEVBL,   _BR_, (webman_config->blind),   buffer);
	add_check_box("wn", "1", STR_NOWMDN,  _BR_, (webman_config->wmstart), buffer);

#ifdef LAUNCHPAD
	add_check_box("rf", "1", STR_CONTSCAN, " & ", (webman_config->refr), buffer);
	add_check_box("lx", "1", "LaunchPad.xml", _BR_, (webman_config->launchpad_xml), buffer);
#else
	add_check_box("rf", "1",  STR_CONTSCAN, _BR_, (webman_config->refr), buffer);
#endif

	add_check_box("pl", "1", STR_USBPOLL,  _BR_, (webman_config->poll) , buffer);
	add_check_box("ft", "1", STR_FTPSVC,   _BR_, (webman_config->ftpd) , buffer);
 #ifdef PS3NET_SERVER
	sprintf(templn, "%s", STR_FTPSVC); char *pos = strcasestr(templn, "FTP"); if(pos) {pos[0] = 'N'; pos[1] = 'E'; pos[2] = 'T';}
	add_check_box("nd", "1", templn,   " : ", (webman_config->netd) , buffer);
	sprintf(templn, HTML_NUMBER("ndp", "%i", "5", "6", "0", "65535") "<br>", webman_config->netp); strcat(buffer, templn);
 #endif

#ifdef LITE_EDITION
	add_check_box("ip", "bind",  STR_ACCESS,   _BR_, (webman_config->bind) , buffer);
#else
	add_check_box("ip", "bind",  STR_ACCESS,  " : ", (webman_config->bind) , buffer);

	sprintf(templn, HTML_INPUT("aip", "%s", "15", "16") " Pwd: "
					HTML_PASSW("pwd", "%s", "20", "20") "<br>", webman_config->allow_ip, webman_config->ftp_password); strcat(buffer, templn);
#endif

#ifdef COBRA_ONLY
	if(c_firmware != 4.53f)
		add_check_box("nsp", "1", STR_NOSPOOF, _BR_, (webman_config->nospoof), buffer);
#endif

#ifdef NOSINGSTAR
	add_check_box("ss", "1", STR_NOSINGSTAR,   _BR_, (webman_config->noss), buffer);
#endif

	add_check_box("np", "1", STR_COMBOS,   _BR_, (webman_config->nopad), buffer);

	//game listing
	strcat(buffer, HTML_BLU_SEPARATOR);
	add_check_box("ng" , "1", STR_NOGRP,     _BR_, (webman_config->nogrp  ), buffer);
	add_check_box("ns" , "1", STR_NOSETUP,   _BR_, (webman_config->nosetup), buffer);
	add_check_box("nc" , "1\" onclick=\"if(nc.checked)ic.value=1; else ic.value=0;", STR_MMCOVERS, " : ", (webman_config->nocov == SHOW_ICON0), buffer);

	// icon type
	strcat(buffer, "<select name=\"ic\" onchange=\"nc.checked=(ic.value==1);\" accesskey=\"C\">");
	add_option_item("0" , "MM COVERS",     (webman_config->nocov == SHOW_MMCOVERS), buffer);
	add_option_item("1" , "ICON0.PNG",     (webman_config->nocov == SHOW_ICON0),    buffer);
	add_option_item("2" , "No ICON0.PNG",  (webman_config->nocov == SHOW_DISC),     buffer);
	add_option_item("3" , "ONLINE COVERS", (webman_config->nocov == ONLINE_COVERS), buffer);
	strcat(buffer, "</select><br>");

	add_check_box("tid", "1", STR_TITLEID, " • ", (webman_config->tid),  buffer);
	add_check_box("sfo", "1", "PARAM.SFO",  _BR_,!(webman_config->use_filename), buffer);

	//game mounting
#ifdef COBRA_ONLY
	add_check_box("bus", "1", STR_RESET_USB, _BR_, (webman_config->bus), buffer);
#endif
	add_check_box("apd", "1", STR_AUTO_PLAY, _BR_, (webman_config->autoplay), buffer);

#ifdef FIX_GAME
	if(c_firmware>=4.20f && c_firmware<=4.78f)
	{
		add_check_box("nf", "3", STR_FIXGAME,  " : <select name=\"fm\">", (webman_config->fixgame==FIX_GAME_DISABLED), buffer);
		add_option_item("0", "Auto"  , (webman_config->fixgame==FIX_GAME_AUTO), buffer);
		add_option_item("1", "Quick" , (webman_config->fixgame==FIX_GAME_QUICK), buffer);
		add_option_item("2", "Forced", (webman_config->fixgame==FIX_GAME_FORCED), buffer);
		strcat(buffer, "</select><br>");
	}
#endif

	//fan control settings
	strcat(buffer, HTML_BLU_SEPARATOR "<table width=\"900\" border=\"0\" cellspacing=\"2\" cellpadding=\"0\"><tr class=\"propfont\"><td>");

	add_check_box("fc"  , "1", STR_FANCTRL, " </td><td>", (webman_config->fanc), buffer);
	add_check_box("warn", "1" , STR_NOWARN, " </td></tr>", (webman_config->warn), buffer);

	strcat(buffer, "<tr class=\"propfont\"><td>");
	add_radio_button("temp", "0", "t_0", STR_AUTOAT , " : ", (webman_config->temp0==0), buffer);
	sprintf(templn, HTML_NUMBER("step\"  accesskey=\"T", "%i", "2", "3", "40", "80") " °C</td><td><label><input type=\"checkbox\"%s/> %s</label> : " HTML_NUMBER("mfan", "%i", "2", "3", "20", "95") " %% %s </td></tr>", webman_config->temp1, (webman_config->fanc && webman_config->temp0==0)?ITEM_CHECKED:"", STR_LOWEST, webman_config->minfan, STR_FANSPEED); strcat(buffer, templn);

	strcat(buffer, "<tr class=\"propfont\"><td>");
	add_radio_button("temp", "1", "t_1", STR_MANUAL , " : ", (webman_config->temp0!=0), buffer);
	sprintf(templn, HTML_NUMBER("manu", "%i", "2", "3", "20", "95") " %% %s </td><td> %s : " HTML_NUMBER("fsp0", "%i", "2", "3", "20", "99") " %% %s </td></tr></table>", (webman_config->manu), STR_FANSPEED, STR_PS2EMU, webman_config->ps2temp, STR_FANSPEED); strcat(buffer, templn);

#ifdef COBRA_ONLY
 #ifndef LITE_EDITION
	//ps3netsvr settings
	strcat(buffer, HTML_BLU_SEPARATOR);
	add_check_box("nd0", "1", STR_LANGAMES,  " &nbsp; PS3NETSRV#1 IP:", (webman_config->netd0), buffer);
	sprintf(templn, HTML_INPUT("neth0", "%s", "15", "16") ":" HTML_NUMBER("netp0", "%i", "5", "6", "0", "65535") "<br>", webman_config->neth0, webman_config->netp0); strcat(buffer, templn);
	add_check_box("nd1", "1", STR_LANGAMES,  " &nbsp; PS3NETSRV#2 IP:", (webman_config->netd1), buffer);
	sprintf(templn, HTML_INPUT("neth1", "%s", "15", "16") ":" HTML_NUMBER("netp1", "%i", "5", "6", "0", "65535") "<br>", webman_config->neth1, webman_config->netp1); strcat(buffer, templn);
	add_check_box("nd2", "1", STR_LANGAMES,  " &nbsp; PS3NETSRV#3 IP:", (webman_config->netd2), buffer);
	sprintf(templn, HTML_INPUT("neth2", "%s", "15", "16") ":" HTML_NUMBER("netp2", "%i", "5", "6", "0", "65535") "<br>", webman_config->neth2, webman_config->netp2); strcat(buffer, templn);
  #ifdef NET3NET4
	add_check_box("nd3", "1", STR_LANGAMES,  " &nbsp; PS3NETSRV#4 IP:", (webman_config->netd3), buffer);
	sprintf(templn, HTML_INPUT("neth3", "%s", "15", "16") ":" HTML_NUMBER("netp3", "%i", "5", "6", "0", "65535") "<br>", webman_config->neth3, webman_config->netp3); strcat(buffer, templn);
	add_check_box("nd4", "1", STR_LANGAMES,  " &nbsp; PS3NETSRV#5 IP:", (webman_config->netd4), buffer);
	sprintf(templn, HTML_INPUT("neth4", "%s", "15", "16") ":" HTML_NUMBER("netp4", "%i", "5", "6", "0", "65535") "<br>", webman_config->neth4, webman_config->netp4); strcat(buffer, templn);
  #endif
 #endif
#endif

	//Wait for any USB device to be ready
	sprintf(templn, HTML_BLU_SEPARATOR "<u> %s:</u><br>", STR_ANYUSB); strcat(buffer, templn);

	add_radio_button("b", "0",  "b_0", "0 sec" , _BR_, (webman_config->bootd==0), buffer);
	add_radio_button("b", "5",  "b_1", "5 sec" , _BR_, (webman_config->bootd==5), buffer);
	add_radio_button("b", "9",  "b_2", "10 sec", _BR_, (webman_config->bootd==9), buffer);
	add_radio_button("b", "15", "b_3", "15 sec", _BR_, (webman_config->bootd==15), buffer);

	//Wait additionally for each selected USB device to be ready
	sprintf(templn, HTML_BLU_SEPARATOR "<u> %s:</u><br>", STR_ADDUSB); strcat(buffer, templn);

	add_radio_button("s", "0",  "s_0", "0 sec" , _BR_, (webman_config->boots==0), buffer);
	add_radio_button("s", "3",  "s_1", "3 sec" , _BR_, (webman_config->boots==3), buffer);
	add_radio_button("s", "5",  "s_2", "5 sec" , _BR_, (webman_config->boots==5), buffer);
	add_radio_button("s", "10", "s_3", "10 sec", _BR_, (webman_config->boots==10), buffer);
	add_radio_button("s", "15", "s_4", "15 sec", _BR_, (webman_config->boots==15), buffer);

#ifdef SPOOF_CONSOLEID
	//Change idps and psid in lv2 memory at system startup
	sprintf(templn, HTML_BLU_SEPARATOR "<u> %s:</u><br>", STR_SPOOFID); strcat(buffer, templn);

	if(!webman_config->vIDPS1[0] && !webman_config->vIDPS1[1]) {get_idps_psid(); sprintf(webman_config->vIDPS1, "%016llX", IDPS[0]); sprintf(webman_config->vIDPS2, "%016llX", IDPS[1]);}
	if(!webman_config->vPSID1[0] && !webman_config->vPSID1[1]) {get_idps_psid(); sprintf(webman_config->vPSID1, "%016llX", PSID[0]); sprintf(webman_config->vPSID2, "%016llX", PSID[1]);}

	add_check_box("id1", "1", "IDPS", " : ", (webman_config->sidps), buffer);
	sprintf(templn, HTML_INPUT("vID1", "%s", "16", "22"), webman_config->vIDPS1); strcat(buffer, templn);
	sprintf(templn, HTML_INPUT("vID2", "%s", "16", "22"), webman_config->vIDPS2); strcat(buffer, templn);
	sprintf(templn, HTML_BUTTON_FMT "<br>", HTML_BUTTON, " ", "onclick=\"vID2.value=", "1000000000000000"); strcat(buffer, templn);

	add_check_box("id2", "1", "PSID", " : ", (webman_config->spsid), buffer);
	sprintf(templn, HTML_INPUT("vPS1", "%s", "16", "22"), webman_config->vPSID1); strcat(buffer, templn);
	sprintf(templn, HTML_INPUT("vPS2", "%s", "16", "22"), webman_config->vPSID2); strcat(buffer, templn);
	sprintf(templn, HTML_BUTTON_FMT "<br><br>", HTML_BUTTON, " ", "onclick=\"vPS1.value=vPS2.value=", "0000000000000000"); strcat(buffer, templn);
#else
	strcat(buffer, HTML_BLU_SEPARATOR);
#endif

#ifndef LITE_EDITION
	//Home
	sprintf(templn, " : " HTML_INPUT("hurl", "%s", "255", "50") "<br>", webman_config->home_url);
	add_check_box("hm", "hom", STR_HOME, templn, webman_config->homeb, buffer);
#endif

	//Disable lv1&lv2 peek&poke syscalls (6,7,9,10,36) and delete history files at system startup
#ifdef COBRA_ONLY
	#ifdef REMOVE_SYSCALLS
	add_check_box("spp", "1", STR_DELCFWSYS, " ", (webman_config->spp & 1), buffer);
	#endif

	//add_check_box("shh", "1", "Offline [Lock PSN]", _BR_, (webman_config->spp & 2), buffer);
	strcat(buffer, " • Offline  : <select name=\"shh\">");
	add_option_item("0", STR_DISABLED, !(webman_config->spp & 6), buffer);
	add_option_item("1", "Lock PSN",    (webman_config->spp & 2), buffer);
	add_option_item("2", STR_GAMES,     (webman_config->spp & 4), buffer);
	strcat(buffer, "</select>");

#endif
	strcat(buffer, HTML_BLU_SEPARATOR);

#ifndef LITE_EDITION
	//default content profile
	sprintf(templn, "%s : <select name=\"usr\">", STR_PROFILE); strcat(buffer, templn);
	add_option_item("0" , STR_DEFAULT, (profile==0) , buffer);
	add_option_item("1", "1", (profile==1) , buffer);
	add_option_item("2", "2", (profile==2) , buffer);
	add_option_item("3", "3", (profile==3) , buffer);
	add_option_item("4", "4", (profile==4) , buffer);

	int fd;

	//default user account
	strcat(buffer, "</select> : hdd0/home/<select name=\"uacc\">");
	{
		if(cellFsOpendir("/dev_hdd0/home", &fd) == CELL_FS_SUCCEEDED)
		{
			CellFsDirent dir; u64 read_e;

			while((cellFsReaddir(fd, &dir, &read_e) == CELL_FS_SUCCEEDED) && (read_e > 0))
			{
				if(dir.d_namlen == 8)
					add_option_item(dir.d_name, dir.d_name, IS(dir.d_name, webman_config->uaccount), buffer);
			}
			cellFsClosedir(fd);
		}

	}

	sprintf(templn, "</select> &nbsp; %s : [<a href=\"/delete.ps3?wmconfig\">wmconfig</a>] [<a href=\"/delete.ps3?wmtmp\">wmtmp</a>] [<a href=\"/delete.ps3?history\">history</a>] • [<a href=\"/rebuild.ps3\">rebuild</a>] [<a href=\"/recovery.ps3\">recovery</a>]<p>", STR_DELETE); strcat(buffer, templn);
#endif

	//memory usage
	sprintf(templn, " %s [%iKB]: <select name=\"fp\" accesskey=\"M\">", STR_MEMUSAGE, (int)(BUFFER_SIZE_ALL / KB)); strcat(buffer, templn);

	add_option_item("0", "Standard (896KB)"                , (webman_config->foot==0), buffer);
	add_option_item("1", "Min (320KB)"                     , (webman_config->foot==1), buffer);
	add_option_item("3", "Min+ (512KB)"                    , (webman_config->foot==3), buffer);
	add_option_item("2", "Max (1280KB)"                    , (webman_config->foot==2), buffer);
	add_option_item("4", "Max PS3+ (1088K PS3)"            , (webman_config->foot==4), buffer);
	add_option_item("5", "Max PSX+ ( 368K PS3 + 720K PSX)" , (webman_config->foot==5), buffer);
	add_option_item("6", "Max BLU+ ( 368K PS3 + 720K BLU)" , (webman_config->foot==6), buffer);
	strcat(buffer, "</select><p>");

#ifndef ENGLISH_ONLY
	//language
	sprintf(templn, " %s: <select name=\"l\">", STR_PLANG); strcat(buffer, templn);

	add_option_item("0" , "English"													, (webman_config->lang==0) , buffer);
	add_option_item("1" , "Fran&ccedil;ais"											, (webman_config->lang==1) , buffer);
	add_option_item("2" , "Italiano"												, (webman_config->lang==2) , buffer);
	add_option_item("3" , "Espa&ntilde;ol"											, (webman_config->lang==3) , buffer);
	add_option_item("4" , "Deutsch"													, (webman_config->lang==4) , buffer);
	add_option_item("5" , "Nederlands"												, (webman_config->lang==5) , buffer);
	add_option_item("6" , "Portugu&ecirc;s"											, (webman_config->lang==6) , buffer);
	add_option_item("7" , "&#1056;&#1091;&#1089;&#1089;&#1082;&#1080;&#1081"		, (webman_config->lang==7) , buffer);
	add_option_item("8" , "Magyar"													, (webman_config->lang==8) , buffer);
	add_option_item("9" , "Polski"													, (webman_config->lang==9) , buffer);
	add_option_item("10", "&Epsilon;&lambda;&lambda;&eta;&nu;&iota;&kappa;&alpha;"	, (webman_config->lang==10), buffer);
	add_option_item("11", "Hrvatski"												, (webman_config->lang==11), buffer);
	add_option_item("12", "&#1041;&#1098;&#1083;&#1075;&#1072;&#1088;&#1089;&#1082;&#1080;", (webman_config->lang==12), buffer);
	add_option_item("20", "Dansk"													, (webman_config->lang==20), buffer);
	add_option_item("21", "&#268;e&scaron;tina"										, (webman_config->lang==21), buffer);
	add_option_item("22", "Sloven&#269;ina"											, (webman_config->lang==22), buffer);

	add_option_item("13", "Indonesian"												, (webman_config->lang==13), buffer);
	add_option_item("14", "T&uuml;rk&ccedil;e"										, (webman_config->lang==14), buffer);
	add_option_item("15", "&#1575;&#1604;&#1593;&#1585;&#1576;&#1610;&#1577;"		, (webman_config->lang==15), buffer);
	add_option_item("16", "&#20013;&#25991;"										, (webman_config->lang==16), buffer);
	add_option_item("19", "&#32321;&#39636;&#20013;&#25991;"						, (webman_config->lang==19), buffer);
	add_option_item("17", "&#54620;&#44397;&#50612;"								, (webman_config->lang==17), buffer);
	add_option_item("18", "&#26085;&#26412;&#35486;"								, (webman_config->lang==18), buffer);
	add_option_item("99", "Unknown"													, (webman_config->lang==99), buffer);

	strcat(buffer, "</select> ");
#endif

#ifdef COBRA_ONLY
#ifdef BDVD_REGION
	u8 cconfig[15];
	CobraConfig *cobra_config = (CobraConfig*) cconfig;
	memset(cobra_config, 0, 15);
	cobra_read_config(cobra_config);

	//BD Region
	strcat(buffer, " • BD Region: <select name=\"bdr\">");
	add_option_item("0" , STR_DEFAULT , (cobra_config->bd_video_region==0) , buffer);
	add_option_item("1" , "A- America", (cobra_config->bd_video_region==1) , buffer);
	add_option_item("2" , "B- Europe" , (cobra_config->bd_video_region==2) , buffer);
	add_option_item("4" , "C- Asia"   , (cobra_config->bd_video_region==4) , buffer);

	//DVD Region
	strcat(buffer, "</select> • DVD Region: <select name=\"dvr\">");
	add_option_item("0"  , STR_DEFAULT          , (cobra_config->dvd_video_region==0)  , buffer);
	add_option_item("1"  , "1- US/Canada"       , (cobra_config->dvd_video_region==1)  , buffer);
	add_option_item("2"  , "2- Europe/Japan"    , (cobra_config->dvd_video_region==2)  , buffer);
	add_option_item("4"  , "3- Korea/HK"        , (cobra_config->dvd_video_region==4)  , buffer);
	add_option_item("8"  , "4- Latino/Australia", (cobra_config->dvd_video_region==8)  , buffer);
	add_option_item("16" , "5- Asia"            , (cobra_config->dvd_video_region==16) , buffer);
	add_option_item("32" , "6- China"           , (cobra_config->dvd_video_region==32) , buffer);
	strcat(buffer, "</select>");
#endif
#endif

#ifdef VIDEO_REC
	strcat(buffer, " • Rec Video: <select name=\"vif\">");
	add_option_item("1110" , "AVC MP 272p" , (rec_video_format==0x1110) , buffer);
	add_option_item("2110" , "AVC BL 272p" , (rec_video_format==0x2110) , buffer);
	add_option_item("0000" , "MPEG4 240p"  , (rec_video_format==0x0000) , buffer);
	add_option_item("0110" , "MPEG4 272p"  , (rec_video_format==0x0110) , buffer);
	add_option_item("0240" , "MPEG4 368p"  , (rec_video_format==0x0240) , buffer);
	add_option_item("3160" , "M4HD  272p"  , (rec_video_format==0x3160) , buffer);
	add_option_item("3270" , "M4HD  368p"  , (rec_video_format==0x3270) , buffer);
	add_option_item("4660" , "M4HD  720p"  , (rec_video_format==0x4660) , buffer);
	add_option_item("3670" , "MJPEG 720p"  , (rec_video_format==0x3670) , buffer);
	strcat(buffer, "</select> • Audio: <select name=\"auf\">");
	add_option_item("0002" , "AAC 64K"   , (rec_audio_format==0x0002) , buffer);
	add_option_item("0000" , "AAC 96K"   , (rec_audio_format==0x0000) , buffer);
	add_option_item("0001" , "AAC 128K"  , (rec_audio_format==0x0001) , buffer);
	add_option_item("2007" , "PCM 384K"  , (rec_audio_format==0x2007) , buffer);
	add_option_item("2008" , "PCM 768K"  , (rec_audio_format==0x2008) , buffer);
	add_option_item("2009" , "PCM 1536K" , (rec_audio_format==0x2009) , buffer);
	strcat(buffer, "</select>");
#endif

	//combos
	sprintf(templn, HTML_BLU_SEPARATOR "<b><u> %s :</u></b><br><table width=\"800\" border=\"0\" cellspacing=\"2\" cellpadding=\"0\"><tr><td nowrap valign=top>", STR_COMBOS2); strcat(buffer, templn);

	add_check_box("pfs", "1", STR_FAILSAFE,   " : <b>SELECT+L3+L2+R2</b><br>"  , !(webman_config->combo & FAIL_SAFE), buffer);
	add_check_box("pss", "1", STR_SHOWTEMP,   " : <b>SELECT+START</b><br>"     , !(webman_config->combo & SHOW_TEMP), buffer);
	add_check_box("ppv", "1", STR_PREVGAME,   " : <b>SELECT+L1</b><br>"        , !(webman_config->combo & PREV_GAME), buffer);
	add_check_box("pnx", "1", STR_NEXTGAME,   " : <b>SELECT+R1</b><br>"        , !(webman_config->combo & NEXT_GAME), buffer);
	add_check_box("pdf", "1", STR_FANCTRL4,   " : <b>L3+R2+START</b><br>"      , !(webman_config->combo & DISABLEFC), buffer);

	add_check_box("umt", "1", STR_UNMOUNT,    " : <b>SELECT+O</b><br>"         , !(webman_config->combo2 & UMNT_GAME), buffer);
	add_check_box("pgd", "1", "gameDATA",     " : <b>SELECT+&#9633;</b><br>"   , !(webman_config->combo2 & EXTGAMDAT), buffer);

	sprintf(templn, "%s XML", STR_REFRESH);
	add_check_box("pxr", "1", templn,         " : <b>SELECT+L3</b><br>"        , !(webman_config->combo2 & XMLREFRSH), buffer);

#ifdef VIDEO_REC
	add_check_box("vrc", "1", "VIDEO REC (in-game)", " : <b>SELECT+R3</b><br>" , !(webman_config->combo2 & VIDRECORD), buffer);
#endif

#ifdef REX_ONLY
	add_check_box("pid", "1", STR_SHOWIDPS,   " : <b>R2+O</b><br>"             , !(webman_config->combo & SHOW_IDPS), buffer);
	add_check_box("psd", "1", STR_SHUTDOWN2,  " : <b>L3+R2+X</b><br>"          , !(webman_config->combo & SHUT_DOWN), buffer);
	add_check_box("prs", "1", STR_RESTART2,   " : <b>L3+R2+O</b><br>"          , !(webman_config->combo & RESTARTPS), buffer);
 #ifdef WM_REQUEST
	add_check_box("psv", "1", "CUSTOM COMBO", " : <b>R2+&#9633;</b></td><td>", !(webman_config->combo2 & CUSTOMCMB), buffer);
 #else
	add_check_box("psv", "1", "BLOCK SERVERS"," : <b>R2+&#9633;</b></td><td>", !(webman_config->combo2 & CUSTOMCMB), buffer);
 #endif
#else
	add_check_box("pid", "1", STR_SHOWIDPS,   " : <b>R2+O</b><br>"             , !(webman_config->combo & SHOW_IDPS), buffer);
 #ifdef WM_REQUEST
	add_check_box("psv", "1", "CUSTOM COMBO", " : <b>R2+&#9633;</b></td><td>", !(webman_config->combo2 & CUSTOMCMB), buffer);
 #else
	add_check_box("psv", "1", "BLOCK SERVERS"," : <b>R2+&#9633;</b></td><td>", !(webman_config->combo2 & CUSTOMCMB), buffer);
 #endif
	add_check_box("psd", "1", STR_SHUTDOWN2,  " : <b>L3+R2+X</b><br>"          , !(webman_config->combo & SHUT_DOWN), buffer);
	add_check_box("prs", "1", STR_RESTART2,   " : <b>L3+R2+O</b><br>"          , !(webman_config->combo & RESTARTPS), buffer);
#endif
	add_check_box("puw", "1", STR_UNLOADWM,   " : <b>L3+R2+R3</b><br>"         , !(webman_config->combo & UNLOAD_WM), buffer);
	add_check_box("pf1", "1", STR_FANCTRL2,   " : <b>SELECT+"                  , !(webman_config->combo & MANUALFAN), buffer); sprintf(templn, "%s</b><br>", STR_UPDN); strcat(buffer, templn);
	add_check_box("pf2", "1", STR_FANCTRL5,   " : <b>SELECT+"                  , !(webman_config->combo & MINDYNFAN), buffer); sprintf(templn, "%s</b><br>", STR_LFRG); strcat(buffer, templn);
#ifdef REMOVE_SYSCALLS
	add_check_box("psc", "1", STR_DELCFWSYS2, " : <b>R2+&#8710;</b> &nbsp; ("  , !(webman_config->combo & DISABLESH), buffer);
	add_check_box("kcc", "1", "CCAPI)", _BR_, !(webman_config->keep_ccapi), buffer);
#endif

#ifndef LITE_EDITION
 #ifdef COBRA_ONLY
	add_check_box("pdc", "1", STR_DISCOBRA,   " : <b>L3+L2+&#8710;</b><br>"    , !(webman_config->combo & DISACOBRA), buffer);
 #endif
	add_check_box("pn0", "1", "NET0",       " : <b>SELECT+R2+&#9633;</b><br>"  , !(webman_config->combo2 & MOUNTNET0), buffer);
	add_check_box("pn1", "1", "NET1",       " : <b>SELECT+L2+&#9633;</b><br>"  , !(webman_config->combo2 & MOUNTNET1), buffer);
#endif

#ifdef REX_ONLY
	add_check_box("pr0", "1", STR_RBGMODE, 	" : <b>L3+L2+&#9633;</b><br>"      , !(webman_config->combo2 & REBUGMODE), buffer);
	add_check_box("pr1", "1", STR_RBGNORM, 	" : <b>L3+L2+O</b><br>"            , !(webman_config->combo2 & NORMAMODE), buffer);
	add_check_box("pr2", "1", STR_RBGMENU, 	" : <b>L3+L2+X</b><br>"            , !(webman_config->combo2 & DEBUGMENU), buffer);

	if(c_firmware>=4.65f)
	add_check_box("p2c", "1", "PS2 CLASSIC",  " : <b>SELECT+L2+&#8710;</b><br>", !(webman_config->combo2 & PS2TOGGLE), buffer);
#endif

#ifndef LITE_EDITION
	add_check_box("p2s", "1", "PS2 SWITCH",   " : <b>SELECT+L2+R2</b><br>"     , !(webman_config->combo2 & PS2SWITCH), buffer);
#endif

#ifdef PKG_HANDLER
	add_check_box("pkg", "1", "INSTALL PKG",  " : <b>SELECT+R2+O</b><br>"      , !(webman_config->combo2 & INSTALPKG), buffer);
#endif
	add_check_box("pld", "1", "PLAY DISC",    " : <b>L2+START</b><br>"
							  "</td></tr></table>"                             , !(webman_config->combo2 & PLAY_DISC), buffer);

#if defined(WM_CUSTOM_COMBO) || defined(WM_REQUEST)
	char command[256]; memset(command, 0, 256);

	if(
 #ifdef WM_CUSTOM_COMBO
		(cellFsOpen(WM_CUSTOM_COMBO "r2_square", CELL_FS_O_RDONLY, &fd, NULL, 0) == CELL_FS_SUCCEEDED) ||
 #endif
		(cellFsOpen("/dev_hdd0/tmp/wm_custom_combo", CELL_FS_O_RDONLY, &fd, NULL, 0) == CELL_FS_SUCCEEDED)
	)
	{
		cellFsRead(fd, (void *)command, 255, NULL);
		cellFsClose(fd);
	}

	sprintf(templn, "&nbsp; &nbsp;" HTML_INPUT("ccbo\" list=\"cmds", "%s", "255", "50") "<br>", command); strcat(buffer, templn);

	strcat(buffer,
					"<div style=\"display:none\"><datalist id=\"cmds\">"
 #ifdef PS3_BROWSER
					"<option>GET /browser.ps3$block_servers"
  #ifdef REMOVE_SYSCALLS
					"<option>GET /browser.ps3$disable_syscalls?keep_ccapi"
  #endif
  #ifdef XMB_SCREENSHOT
					"<option>GET /browser.ps3$screenshot_xmb"
  #endif
 #endif
					"<option>GET /cpursx.ps3?mode"
 #ifdef GET_KLICENSEE
					"<option>GET /klic.ps3?log"
 #endif
					"</datalist></div>");
#endif // #ifdef WM_REQUEST

	sprintf(templn, HTML_RED_SEPARATOR "<input type=\"submit\" accesskey=\"S\" value=\" %s \"/>"
					"</form>", STR_SAVE); strcat(buffer, templn);

#ifndef LITE_EDITION
	strcat(buffer,  HTML_RED_SEPARATOR
					"<a href=\"http://github.com/aldostools/webMAN-MOD/releases\">webMAN-MOD - Latest version of webMAN-MOD on Github</a><br>"
					"<a href=\"http://www.psx-place.com/threads/webman-mod-general-information-thread.27/\">webMAN-MOD - Main thread of webMAN-MOD on Psx-place</a><br>");
#else
	strcat(buffer,  HTML_BLU_SEPARATOR
					"webMAN - Simple Web Server" EDITION "<br>");
#endif

/*
	#define VSH_GCM_OBJ			0x70A8A8 // 4.53cex
	//#define VSH_GCM_OBJ		0x71A5F8 // 4.46dex

	uint32_t *gcm_obj0 = VSH_GCM_OBJ + ((uint32_t) 0 << 4);
	uint32_t *gcm_obj1 = VSH_GCM_OBJ + ((uint32_t) 1 << 4); // offset, pitch, width, height


	_cellGcmIoOffsetToAddress = getNIDfunc("sdk", 0x2a6fba9c, 0);

	void *buf_adr[2];

	if(_cellGcmIoOffsetToAddress)
	{
		_cellGcmIoOffsetToAddress(gcm_obj0[0], &buf_adr[0]);
		_cellGcmIoOffsetToAddress(gcm_obj1[0], &buf_adr[1]); //0x37ee5ac
	}

	sprintf(templn, "OFFSET#0: %x, P: %i, W: %i, H: %i, E: %x <br>",
		gcm_obj0[0], gcm_obj0[1], gcm_obj0[2], gcm_obj0[3], buf_adr[0]); strcat(buffer, templn);

	sprintf(templn, "OFFSET#1: %x, P: %i, W: %i, H: %i, E: %x <br>",
		gcm_obj1[0], gcm_obj1[1], gcm_obj1[2], gcm_obj1[3], buf_adr[1]); strcat(buffer, templn);
*/
}

static int save_settings()
{
	return savefile(WMCONFIG, (char*)wmconfig, sizeof(WebmanCfg));
}

static void reset_settings()
{
	memset(webman_config, 0, sizeof(WebmanCfg));

	webman_config->usb0 = 1;
	webman_config->usb1 = 1;
	//webman_config->usb2 = 0;
	//webman_config->usb3 = 0;
	webman_config->usb6 = 1;
	//webman_config->usb7 = 0;

	//webman_config->dev_sd = 0;
	//webman_config->dev_ms = 0;
	//webman_config->dev_cf = 0;

	//webman_config->lastp = 0;      //disable last play
	//webman_config->autob = 0;      //disable check for AUTOBOOT.ISO
	//webman_config->delay = 0;      //don't delay loading of AUTOBOOT.ISO/last-game (Disc Auto-start)

	//webman_config->bootd = 0;      //don't wait for any USB device to be ready
	webman_config->boots = 3;        //wait 3 additional seconds for each selected USB device to be ready

	//webman_config->nogrp = 0;      //group content on XMB
	//webman_config->wmstart = 0;    //enable start up message (webMAN Loaded!)
	//webman_config->tid = 0;        //don't include the ID as part of the title of the game
	//webman_config->nosetup = 0;    //enable webMAN Setup entry in "webMAN Games"

#ifdef COBRA_ONLY
	webman_config->cmask = 0;
#else
	webman_config->cmask = (PSP | PS1 | BLU | DVD);
#endif

	webman_config->poll    = 1;    //disable USB polling
	//webman_config->nopad = 0;    //enable all PAD shortcuts
	//webman_config->nocov = 0;    //enable multiMAN covers    (0 = Use MM covers, 1 = Use ICON0.PNG, 2 = No game icons, 3 = Online Covers)

	webman_config->fanc    = 1;    //fan control enabled
	//webman_config->temp0 = 0;    //auto
	webman_config->temp1   = MY_TEMP;
	webman_config->manu    = 35;   //manual temp
	webman_config->ps2temp = 37;   //ps2 temp

	webman_config->minfan = DEFAULT_MIN_FANSPEED; // %

	//webman_config->bind = 0;       //enable remote access to FTP/WWW services
	//webman_config->ftpd = 0;       //enable ftp server
	//webman_config->refr = 0;       //enable content scan on startup
	//webman_config->ftp_password  =  "";

	//webman_config->netd0    = webman_config->netd1    = webman_config->netd2    = webman_config->netd3    = webman_config->netd4    = 0;
	//webman_config->neth0[0] = webman_config->neth1[0] = webman_config->neth2[0] = webman_config->neth3[0] = webman_config->neth4[0] = NULL;
	webman_config->netp = webman_config->netp0 = webman_config->netp1 = webman_config->netp2 = webman_config->netp3 = webman_config->netp4 = NETPORT;

	webman_config->foot    = 1;    //MIN
	webman_config->nospoof = 1;    //don't spoof fw version

	webman_config->pspl = 1;       //Show PSP Launcher
	webman_config->ps2l = 1;       //Show PS2 Classic Launcher

	//webman_config->spp   = 0;    //disable removal of syscalls
	webman_config->fixgame = FIX_GAME_AUTO;

	//webman_config->sidps = 0;    //spoof IDPS
	//webman_config->spsid = 0;    //spoof PSID

	//webman_config->vIDPS1[0] = webman_config->vIDPS2[0] = 0;
	//webman_config->vPSID1[0] = webman_config->vPSID2[0] = 0;

	//webman_config->bus = 0;        //enable reset USB bus

	//webman_config->autoplay = 0;   //enable global autoplay

	webman_config->combo   = DISACOBRA; //disable combo for cobra toggle
	webman_config->combo2 |= (REBUGMODE|NORMAMODE|DEBUGMENU|PS2SWITCH|VIDRECORD); //disable combos for rebug/ps2 switch/video record

	//webman_config->rec_video_format = CELL_REC_PARAM_VIDEO_FMT_MPEG4_SMALL_512K_30FPS;
	//webman_config->rec_audio_format = CELL_REC_PARAM_AUDIO_FMT_AAC_96K;

	// default first user account
	char upath[24];
	sprintf(webman_config->uaccount, "%08i", 1);
	for(u8 acc = 1; acc < 100; acc++)
	{
		sprintf(upath, "%s/%08i", "/dev_hdd0/home", acc);
		if(file_exists(upath)) {sprintf(webman_config->uaccount, "%08i", acc); break;}
	}

	// set default language
#ifndef ENGLISH_ONLY
	get_system_language(&webman_config->lang);
#else
	webman_config->lang = 0; // english
#endif

	int fdwm = 0;

	// read current settings
	for(u8 retry = 0; retry < 10; retry++)
	{
		if(cellFsOpen(WMCONFIG, CELL_FS_O_RDONLY, &fdwm, NULL, 0) == CELL_FS_SUCCEEDED)
		{
			cellFsRead(fdwm, (void *)wmconfig, sizeof(WebmanCfg), NULL);
			cellFsClose(fdwm);
			break;
		}

		sys_timer_usleep(500000);
	}

#ifndef COBRA_ONLY
	webman_config->spp = 0; //disable removal of syscalls on nonCobra
#endif

	// set default autoboot path
	if(webman_config->autoboot_path[0] == NULL) strcpy(webman_config->autoboot_path, DEFAULT_AUTOBOOT_PATH);

	if(webman_config->warn > 1) webman_config->warn = 0;
	webman_config->minfan = RANGE(webman_config->minfan, MIN_FANSPEED, 99);

	// settings
	save_settings();

	profile = webman_config->profile;
}
