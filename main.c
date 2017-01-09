#include <sdk_version.h>
#include <cellstatus.h>
#include <cell/cell_fs.h>
#include <cell/rtc.h>
#include <cell/gcm.h>
#include <cell/pad.h>
#include <sys/vm.h>
#include <sysutil/sysutil_common.h>

#include <sys/prx.h>
#include <sys/ppu_thread.h>
#include <sys/event.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/memory.h>
#include <sys/timer.h>
#include <sys/process.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netex/net.h>
#include <netex/errno.h>
#include <netex/libnetctl.h>
#include <netex/sockinfo.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "flags.h"

#ifdef REX_ONLY
 #ifndef DEX_SUPPORT
 #define DEX_SUPPORT	1
 #endif
#endif

#include "types.h"
#include "common.h"
#include "cobra/cobra.h"
#include "cobra/storage.h"
#include "vsh/game_plugin.h"
#include "vsh/netctl_main.h"
#include "vsh/xregistry.h"
#include "vsh/vshnet.h"
#include "vsh/explore_plugin.h"

#define _game_TitleID  _game_info+0x04
#define _game_Title    _game_info+0x14

static char _game_info[0x120];
static char search_url[50];

#ifdef COBRA_ONLY
 #include "cobra/netiso.h"

 #ifdef LITE_EDITION
	#define EDITION " [Lite]"
 #elif defined(PS3NET_SERVER) && defined(NET3NET4) && defined(XMB_SCREENSHOT)
	#define EDITION " [Full]"
 #else
  #ifdef PS3MAPI
	#ifdef REX_ONLY
		#define EDITION " [Rebug-PS3MAPI]"
	#else
		#define EDITION " [PS3MAPI]"
	#endif
  #else
   #ifdef REX_ONLY
	#define EDITION " [Rebug]"
   #else
	#define EDITION ""
   #endif
  #endif
 #endif
#else
 #ifdef CCAPI
	#define EDITION " [CCAPI]"
 #else
	#define EDITION " [nonCobra]"
 #endif
 #undef PS3MAPI
#endif

#ifdef LAST_FIRMWARE_ONLY
 #undef DECR_SUPPORT
 #undef FIX_GAME
#endif

#ifdef PKG_LAUNCHER
 #define MOUNT_ROMS 1
#endif

SYS_MODULE_INFO(WWWD, 0, 1, 0);
SYS_MODULE_START(wwwd_start);
SYS_MODULE_STOP(wwwd_stop);

#define VSH_MODULE_PATH 	"/dev_blind/vsh/module/"
#define VSH_ETC_PATH		"/dev_blind/vsh/etc/"
#define PS2_EMU_PATH		"/dev_blind/ps2emu/"
#define REBUG_COBRA_PATH	"/dev_blind/rebug/cobra/"
#define HABIB_COBRA_PATH	"/dev_blind/habib/cobra/"
#define SYS_COBRA_PATH		"/dev_blind/sys/"
#define REBUG_TOOLBOX		"/dev_hdd0/game/RBGTLBOX2/USRDIR/"
#define COLDBOOT_PATH		"/dev_blind/vsh/resource/coldboot.raf"
#define ORG_LIBFS_PATH		"/dev_flash/sys/external/libfs.sprx"
#define NEW_LIBFS_PATH		"/dev_hdd0/tmp/libfs.sprx"

#define WM_VERSION			"1.45.09 MOD"						// webMAN version

#define MM_ROOT_STD			"/dev_hdd0/game/BLES80608/USRDIR"	// multiMAN root folder
#define MM_ROOT_SSTL		"/dev_hdd0/game/NPEA00374/USRDIR"	// multiman SingStar® Stealth root folder
#define MM_ROOT_STL			"/dev_hdd0/tmp/game_repo/main"		// stealthMAN root folder

#define TMP_DIR				"/dev_hdd0/tmp"

#define WMCONFIG			TMP_DIR "/wm_config.bin"		// webMAN config file
#define WMTMP				TMP_DIR "/wmtmp"				// webMAN work/temp folder
#define WM_LANG_PATH		TMP_DIR "/wm_lang"				// webMAN language folder
#define WM_ICONS_PATH		TMP_DIR "/wm_icons"				// webMAN icons folder
#define WM_COMBO_PATH		TMP_DIR "/wm_combo"				// webMAN custom combos folder
#define WMNOSCAN			TMP_DIR "/wm_noscan"			// webMAN config file to skip on boot
#define WMREQUEST_FILE		TMP_DIR "/wm_request"			// webMAN request file
#define WMNET_DISABLED		TMP_DIR "/wm_netdisabled"		// webMAN config file to re-enable network

#define WMONLINE_GAMES		TMP_DIR "/wm_online_ids.txt"	// webMAN config file to skip disable network setting on these title ids
#define WMOFFLINE_GAMES		TMP_DIR "/wm_offline_ids.txt"	// webMAN config file to disable network setting on specific title ids (overrides wm_online_ids.txt)

#define VSH_MENU_IMAGES		"/dev_hdd0/plugins/images"

#define HDD0_GAME_DIR		"/dev_hdd0/game/"

#define SYSMAP_PS3_UPDATE	"/dev_flash/vsh/resource/AAA"		//redirect firmware update to empty folder (formerly redirected to "/dev_bdvd")

#define PS2_CLASSIC_TOGGLER "/dev_hdd0/classic_ps2"

#define PS2_CLASSIC_PLACEHOLDER  "/dev_hdd0/game/PS2U10000/USRDIR"
#define PS2_CLASSIC_ISO_PATH     "/dev_hdd0/game/PS2U10000/USRDIR/ISO.BIN.ENC"
#define PS2_CLASSIC_ISO_ICON     "/dev_hdd0/game/PS2U10000/ICON0.PNG"

#define NONE -1
#define SYS_PPU_THREAD_NONE        (sys_ppu_thread_t)NONE
#define SYS_EVENT_QUEUE_NONE       (sys_event_queue_t)NONE
#define SYS_DEVICE_HANDLE_NONE     (sys_device_handle_t)NONE
#define SYS_MEMORY_CONTAINER_NONE  (sys_memory_container_t)NONE

///////////// PS3MAPI BEGIN //////////////
#ifdef COBRA_ONLY
 #define SYSCALL8_OPCODE_PS3MAPI					0x7777

 #define PS3MAPI_OPCODE_SET_ACCESS_KEY				0x2000
 #define PS3MAPI_OPCODE_REQUEST_ACCESS				0x2001
 #define PS3MAPI_OPCODE_PCHECK_SYSCALL8 			0x0094
 #define PS3MAPI_OPCODE_PDISABLE_SYSCALL8 			0x0093

// static uint64_t ps3mapi_key = 0;
 static int pdisable_sc8 = NONE;
 #define PS3MAPI_ENABLE_ACCESS_SYSCALL8		//if(syscalls_removed) { system_call_3(SC_COBRA_SYSCALL8, SYSCALL8_OPCODE_PS3MAPI, PS3MAPI_OPCODE_REQUEST_ACCESS, ps3mapi_key); }
 #define PS3MAPI_DISABLE_ACCESS_SYSCALL8	//if(syscalls_removed && !is_mounting) { system_call_3(SC_COBRA_SYSCALL8, SYSCALL8_OPCODE_PS3MAPI, PS3MAPI_OPCODE_SET_ACCESS_KEY, ps3mapi_key); }

 #define PS3MAPI_REENABLE_SYSCALL8			{ system_call_2(SC_COBRA_SYSCALL8, SYSCALL8_OPCODE_PS3MAPI, PS3MAPI_OPCODE_PCHECK_SYSCALL8); pdisable_sc8 = (int)p1;} \
											if(pdisable_sc8 > 0) { system_call_3(SC_COBRA_SYSCALL8, SYSCALL8_OPCODE_PS3MAPI, PS3MAPI_OPCODE_PDISABLE_SYSCALL8, 0); }
 #define PS3MAPI_RESTORE_SC8_DISABLE_STATUS	if(pdisable_sc8 > 0) { system_call_3(SC_COBRA_SYSCALL8, SYSCALL8_OPCODE_PS3MAPI, PS3MAPI_OPCODE_PDISABLE_SYSCALL8, pdisable_sc8); }
#else
 #define PS3MAPI_ENABLE_ACCESS_SYSCALL8
 #define PS3MAPI_DISABLE_ACCESS_SYSCALL8
 #define PS3MAPI_REENABLE_SYSCALL8
 #define PS3MAPI_RESTORE_SC8_DISABLE_STATUS
#endif
///////////// PS3MAPI END //////////////

#ifdef WM_REQUEST
 #ifdef WEB_CHAT
  #define DELETE_TURNOFF	{do_umount(false); cellFsUnlink("/dev_hdd0/tmp/turnoff"); cellFsUnlink(WMREQUEST_FILE); cellFsUnlink(WMCHATFILE);}
 #else
  #define DELETE_TURNOFF	{do_umount(false); cellFsUnlink("/dev_hdd0/tmp/turnoff"); cellFsUnlink(WMREQUEST_FILE);}
 #endif
#else
 #ifdef WM_CUSTOM_COMBO
  #undef WM_CUSTOM_COMBO
 #endif

 #ifdef WEB_CHAT
  #define DELETE_TURNOFF	{do_umount(false); cellFsUnlink("/dev_hdd0/tmp/turnoff"); cellFsUnlink(WMCHATFILE);}
 #else
  #define DELETE_TURNOFF	{do_umount(false); cellFsUnlink("/dev_hdd0/tmp/turnoff");}
 #endif
#endif

#define THREAD_NAME_SVR			"wwwdt"
#define THREAD_NAME_WEB			"wwwd"
#define THREAD_NAME_CMD			"wwwd2"
#define THREAD_NAME_FTP			"ftpdt"
#define THREAD_NAME_FTPD		"ftpd"
#define THREAD_NAME_NET			"netiso"
#define THREAD_NAME_NTFS		"ntfsd"
#define THREAD_NAME_PSX_EJECT	"ntfsd_eject"
#define THREAD_NAME_POLL		"poll_thread"
#define THREAD_NAME_INSTALLPKG	"install_pkg"
#define THREAD_NAME_NETSVR		"netsvr"
#define THREAD_NAME_NETSVRD		"netsvrd"

#define STOP_THREAD_NAME 		"wwwds"

#define THREAD_PRIO				-0x1d8
#define THREAD_PRIO_FTP			-0x10
#define THREAD_PRIO_NET			-0x1d8
#define THREAD_PRIO_STOP		 0x000

#define THREAD_STACK_SIZE_8KB		0x2000
#define THREAD_STACK_SIZE_64KB		0x10000

#define SYS_PPU_THREAD_CREATE_NORMAL	0x000

////////////

#define HTML_BASE_PATH			"/dev_hdd0/xmlhost/game_plugin"

#define FB_XML					HTML_BASE_PATH "/fb.xml"
#define MY_GAMES_XML			HTML_BASE_PATH "/mygames.xml"
#define MOBILE_HTML				HTML_BASE_PATH "/mobile.html"
#define GAMELIST_JS				HTML_BASE_PATH "/gamelist.js"

#ifndef EMBED_JS
#define COMMON_CSS				HTML_BASE_PATH "/common.css"
#define COMMON_SCRIPT_JS		HTML_BASE_PATH "/common.js"
#define FM_SCRIPT_JS			HTML_BASE_PATH "/fm.js"
#define GAMES_SCRIPT_JS			HTML_BASE_PATH "/games.js"
#endif

#define JQUERY_LIB_JS			HTML_BASE_PATH "/jquery.min.js"
#define JQUERY_UI_LIB_JS		HTML_BASE_PATH "/jquery-ui.min.js"

#define DELETE_CACHED_GAMES		{cellFsUnlink(WMTMP "/games.html"); cellFsUnlink(GAMELIST_JS);}

////////////

#define SC_GET_FREE_MEM 				(352)
#define SC_GET_PLATFORM_INFO			(387)
#define SC_RING_BUZZER  				(392)

#define SC_FS_MOUNT  					(837)
#define SC_FS_UMOUNT 					(838)
#define SC_GET_CONSOLE_TYPE				(985)

#define SC_GET_PRX_MODULE_BY_ADDRESS	(461)
#define SC_STOP_PRX_MODULE 				(482)
#define SC_UNLOAD_PRX_MODULE 			(483)
#define SC_PPU_THREAD_EXIT				(41)

#define SC_SYS_POWER 					(379)
#define SYS_SOFT_REBOOT 				0x0200
#define SYS_HARD_REBOOT					0x1200
#define SYS_REBOOT						0x8201 /*load LPAR id 1*/
#define SYS_SHUTDOWN					0x1100

#define SYS_NET_EURUS_POST_COMMAND		(726)
#define CMD_GET_MAC_ADDRESS				0x103f

#define SYSCALL8_OPCODE_GET_MAMBA		0x7FFFULL

#define BEEP1 { system_call_3(SC_RING_BUZZER, 0x1004, 0x4,   0x6); }
#define BEEP2 { system_call_3(SC_RING_BUZZER, 0x1004, 0x7,  0x36); }
#define BEEP3 { system_call_3(SC_RING_BUZZER, 0x1004, 0xa, 0x1b6); }

////////////

#define WWWPORT			(80)
#define FTPPORT			(21)
#define NETPORT			(38008)

#define KB			   1024UL
#define   _2KB_		   2048UL
#define   _4KB_		   4096UL
#define   _6KB_		   6144UL
#define   _8KB_		   8192UL
#define  _12KB_		  12288UL
#define  _32KB_		  32768UL
#define  _64KB_		  65536UL
#define _128KB_		 131072UL
#define _192KB_		 196608UL
#define _256KB_		 262144UL
#define  _1MB_		1048576UL
#define _32MB_		33554432UL

#define MIN_MEM		_192KB_

static u32 BUFFER_SIZE_FTP	= ( _128KB_);
static u32 BUFFER_SIZE_ALL	= ( 896*KB);

static u32 BUFFER_SIZE		= ( 448*KB);
static u32 BUFFER_SIZE_PSX	= ( 160*KB);
static u32 BUFFER_SIZE_PSP	= (  _32KB_);
static u32 BUFFER_SIZE_PS2	= (  _64KB_);
static u32 BUFFER_SIZE_DVD	= ( _192KB_);

#ifdef MOUNT_ROMS
static u32 BUFFER_SIZE_ROM	= (  _32KB_ / 2);
#endif

#define MAX_PAGES   (BUFFER_SIZE_ALL / _64KB_)

////////////

#define MODE		0777
#define DMODE		(CELL_FS_S_IFDIR | MODE)

#define LINELEN			512 // file listing
#define MAX_LINE_LEN	640 // html games
#define MAX_PATH_LEN	512 // do not change!
#define MAX_TEXT_LEN	2000 // should not exceed HTML_RECV_SIZE

#define FAILED		-1

#define HTML_RECV_SIZE	2048
#define HTML_RECV_LAST	2047
#define ip_size			0x10


#define CODE_HTTP_OK         200
#define CODE_BAD_REQUEST     400
#define CODE_SERVER_BUSY     503
#define CODE_VIRTUALPAD     1200
#define CODE_INSTALL_PKG    1201
#define CODE_DOWNLOAD_FILE  1202
#define CODE_RETURN_TO_ROOT 1203
#define CODE_GOBACK         1222
#define CODE_CLOSE_BROWSER  1223

#define IS_ON_XMB		(View_Find("game_plugin") == 0)
#define IS_INGAME		(View_Find("game_plugin") != 0)

////////////

#ifdef COBRA_ONLY
 #ifndef LITE_EDITION
 static sys_ppu_thread_t thread_id_net	= SYS_PPU_THREAD_NONE;
 #endif
 static sys_ppu_thread_t thread_id_ntfs	= SYS_PPU_THREAD_NONE;
 #ifdef PS3NET_SERVER
 static sys_ppu_thread_t thread_id_netsvr = SYS_PPU_THREAD_NONE;
 #endif
#endif
static sys_ppu_thread_t thread_id_wwwd	= SYS_PPU_THREAD_NONE;
static sys_ppu_thread_t thread_id_ftpd	= SYS_PPU_THREAD_NONE;
static sys_ppu_thread_t thread_id_poll	= SYS_PPU_THREAD_NONE;


#define START_DAEMON		(0xC0FEBABE)
#define REFRESH_CONTENT		(0xC0FEBAB0)
#define WM_FILE_REQUEST		(0xC0FEBEB0)

typedef struct {
	uint32_t total;
	uint32_t avail;
} _meminfo;

static uint8_t profile = 0;

static uint8_t loading_html = 0;
static uint8_t refreshing_xml = 0;

#ifdef SYS_BGM
static uint8_t system_bgm = 0;
#endif

#define NTFS 		 	(12)

#define PERSIST  100

static bool show_info_popup = false;
static bool do_restart = false;

#ifdef USE_DEBUG
 static int debug_s = -1;
 static char debug[256];
#endif

static volatile uint8_t wm_unload_combo = 0;
static volatile uint8_t working = 1;
static uint8_t max_mapped = 0;

#ifdef COBRA_ONLY
 static const uint8_t cobra_mode = 1;
#else
 static const uint8_t cobra_mode = 0;
#endif

static bool syscalls_removed = false;

static float c_firmware = 0.0f;
static uint8_t dex_mode = 0;

#ifdef SYS_ADMIN_MODE
static uint8_t sys_admin = 0;
static uint8_t pwd_tries = 0;
#else
static uint8_t sys_admin = 1;
#endif

#ifdef OFFLINE_INGAME
static int32_t net_status = NONE;
#endif

static u64 SYSCALL_TABLE = 0;
static u64 LV2_OFFSET_ON_LV1; // value is set on detect_firmware -> 0x1000000 on 4.46, 0x8000000 on 4.76/4.78

enum is_binary_options
{
	WEB_COMMAND = 0,
	BINARY_FILE = 1,
	FOLDER_LISTING = 2
};

enum get_name_options
{
	NO_EXT    = 0,
	GET_WMTMP = 1,
	NO_PATH   = 2,
};

enum cp_mode_options
{
	CP_MODE_NONE = 0,
	CP_MODE_COPY = 1,
	CP_MODE_MOVE = 2,
};

////////////////////////////////
typedef struct
{
	uint16_t version;

	uint8_t padding0[14];

	uint8_t lang;

	// scan devices settings

	uint8_t usb0;
	uint8_t usb1;
	uint8_t usb2;
	uint8_t usb3;
	uint8_t usb6;
	uint8_t usb7;
	uint8_t dev_sd;
	uint8_t dev_ms;
	uint8_t dev_cf;

	uint8_t padding1[6];

	// scan content settings

	uint8_t refr;
	uint8_t foot;
	uint8_t cmask;

	uint8_t nogrp;
	uint8_t nocov;
	uint8_t nosetup;
	uint8_t rxvid;
	uint8_t ps2l;
	uint8_t pspl;
	uint8_t tid;
	uint8_t use_filename;
	uint8_t launchpad_xml;
	uint8_t launchpad_grp;
	uint8_t ps3l;
	uint8_t roms;

	uint8_t padding2[17];

	// start up settings

	uint8_t wmstart;
	uint8_t lastp;
	uint8_t autob;
	char    autoboot_path[256];
	uint8_t delay;
	uint8_t bootd;
	uint8_t boots;
	uint8_t nospoof;
	uint8_t blind;
	uint8_t spp;    //disable syscalls, offline: lock PSN, offline ingame
	uint8_t noss;   //no singstar
	uint8_t nosnd0; //no snd0.at3

	uint8_t padding3[5];

	// fan control settings

	uint8_t fanc;
	uint8_t temp0;
	uint8_t temp1;
	uint8_t manu;
	uint8_t ps2temp;
	uint8_t nowarn;
	uint8_t minfan;

	uint8_t padding4[9];

	// combo settings

	uint8_t  nopad;
	uint8_t  keep_ccapi;
	uint32_t combo;
	uint32_t combo2;

	uint8_t padding5[22];

	// ftp server settings

	uint8_t  bind;
	uint8_t  ftpd;
	uint16_t ftp_port;
	uint8_t  ftp_timeout;
	char     ftp_password[20];
	char     allow_ip[16];

	uint8_t padding6[7];

	// net server settings

	uint8_t  netsrvd;
	uint16_t netsrvp;

	uint8_t padding7[13];

	// net client settings

	uint8_t  netd[5];
	uint16_t netp[5];
	char     neth[5][16];

	uint8_t padding8[33];

	// mount settings

	uint8_t bus;
	uint8_t fixgame;
	uint8_t ps1emu;
	uint8_t autoplay;
	uint8_t ps2emu;

	uint8_t padding9[11];

	// profile settings

	uint8_t profile;
	char uaccount[9];
	uint8_t admin_mode;

	uint8_t padding10[5];

	// misc settings

	uint8_t default_restart;
	uint8_t poll; // poll usb

	uint32_t rec_video_format;
	uint32_t rec_audio_format;

	uint8_t auto_power_off; // 0 = prevent auto power off on ftp, 1 = allow auto power off on ftp (also on install.ps3, download.ps3)

	uint8_t padding12[5];

	uint8_t homeb;
	char home_url[255];

	uint8_t padding11[32];

	// spoof console id

	uint8_t sidps;
	uint8_t spsid;
	char vIDPS1[17];
	char vIDPS2[17];
	char vPSID1[17];
	char vPSID2[17];

	uint8_t padding13[34];
} /*__attribute__((packed))*/ WebmanCfg;

static uint8_t wmconfig[sizeof(WebmanCfg)];
static WebmanCfg *webman_config = (WebmanCfg*) wmconfig;

static int save_settings(void);
////////////////////////////////


#define AUTOBOOT_PATH				"/dev_hdd0/PS3ISO/AUTOBOOT.ISO"

#ifdef COBRA_ONLY
 #define DEFAULT_AUTOBOOT_PATH		"/dev_hdd0/PS3ISO/AUTOBOOT.ISO"
#else
 #define DEFAULT_AUTOBOOT_PATH		"/dev_hdd0/GAMES/AUTOBOOT"
#endif

#define MAX_ISO_PARTS				(16)
#define ISO_EXTENSIONS				".iso.0|.cue|.img|.mdf|.bin"

static CellRtcTick rTick, gTick;

#ifdef GET_KLICENSEE
int npklic_struct_offset = 0; uint8_t klic_polling = 0;

#define KLICENSEE_SIZE          0x10
#define KLICENSEE_OFFSET        (npklic_struct_offset)
#define KLIC_PATH_OFFSET        (npklic_struct_offset+0x10)
#define KLIC_CONTENT_ID_OFFSET  (npklic_struct_offset-0xA4)
#endif


static bool is_mamba = false;
static uint16_t cobra_version = 0;

static bool is_mounting = false;
static bool copy_aborted = false;

#ifndef EMBED_JS
static bool css_exists = false;
static bool common_js_exists = false;
#endif

static char html_base_path[MAX_PATH_LEN];

static char smonth[12][4]  = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

static char drives[16][12] = {"/dev_hdd0", "/dev_usb000", "/dev_usb001", "/dev_usb002", "/dev_usb003", "/dev_usb006", "/dev_usb007", "/net0", "/net1", "/net2", "/net3", "/net4", "/ext", "/dev_sd", "/dev_ms", "/dev_cf"};
static char paths [13][12] = {"GAMES", "GAMEZ", "PS3ISO", "BDISO", "DVDISO", "PS2ISO", "PSXISO", "PSXGAMES", "PSPISO", "ISO", "video", "GAMEI", "ROMS"};

#ifdef COPY_PS3
static char    cp_path[MAX_PATH_LEN];   // cut/copy/paste buffer
static uint8_t cp_mode = CP_MODE_NONE;  // 0 = none / 1 = copy / 2 = cut/move
#endif

#define ONLINE_TAG		"[online]"
#define OFFLINE_TAG		"[offline]"
#define AUTOPLAY_TAG	" [auto]"

static char wm_icons[12][60] = {WM_ICONS_PATH "/icon_wm_album_ps3.png", //024.png  [0]
								WM_ICONS_PATH "/icon_wm_album_psx.png", //026.png  [1]
								WM_ICONS_PATH "/icon_wm_album_ps2.png", //025.png  [2]
								WM_ICONS_PATH "/icon_wm_album_psp.png", //022.png  [3]
								WM_ICONS_PATH "/icon_wm_album_dvd.png", //023.png  [4]

								WM_ICONS_PATH "/icon_wm_ps3.png",       //024.png  [5]
								WM_ICONS_PATH "/icon_wm_psx.png",       //026.png  [6]
								WM_ICONS_PATH "/icon_wm_ps2.png",       //025.png  [7]
								WM_ICONS_PATH "/icon_wm_psp.png",       //022.png  [8]
								WM_ICONS_PATH "/icon_wm_dvd.png",       //023.png  [9]

								WM_ICONS_PATH "/icon_wm_settings.png",  //icon/icon_home.png  [10]
								WM_ICONS_PATH "/icon_wm_eject.png",     //icon/icon_home.png  [11]
								};

#ifndef ENGLISH_ONLY
static bool use_custom_icon_path = false, use_icon_region = false;
static bool is_xmbmods_server = false;
#endif

static bool covers_exist[8];
static char fw_version[8] = "4.xx";
static char local_ip[16] = "127.0.0.1";

static void show_msg(char* msg);

static bool file_exists(const char* path);
static int isDir(const char* path);

size_t read_file(const char *file, char *data, size_t size, int32_t offset);
int save_file(const char *file, const char *mem, int64_t size);
int waitfor(const char *path, uint8_t timeout);

#include "include/html.h"
#include "include/peek_poke.h"
#include "include/idps.h"
#include "include/led.h"
#include "include/vpad.h"
#include "include/socket.h"
#include "include/language.h"
#include "include/fancontrol.h"
#include "include/firmware.h"

int wwwd_start(uint64_t arg);
int wwwd_stop(void);
static void stop_prx_module(void);
static void unload_prx_module(void);

#ifdef REMOVE_SYSCALLS
static void remove_cfw_syscalls(bool keep_ccapi);
#ifdef PS3MAPI
static void restore_cfw_syscalls(void);
#endif
#endif

#ifdef PKG_HANDLER
static int installPKG(const char *pkgpath, char *msg);
#endif

static void handleclient(u64 conn_s_p);

static void do_umount(bool clean);
static void mount_autoboot(void);
static bool mount_with_mm(const char *_path, u8 do_eject);
#ifdef COBRA_ONLY
static void do_umount_iso(void);
#endif

static size_t get_name(char *name, const char *filename, u8 cache);
static void add_breadcrumb_trail(char *buffer, char *param);
static void get_cpursx(char *cpursx);
static void get_last_game(char *last_path);
static void add_game_info(char *buffer, char *templn, bool is_cpursx);

static bool from_reboot = false;
static bool is_busy = false;

#ifdef COPY_PS3
static char current_file[MAX_PATH_LEN];
#endif

#include "include/eject_insert.h"

#ifdef COBRA_ONLY

#include "include/rawseciso.h"
#include "include/netclient.h"
#include "include/netserver.h"

#endif //#ifdef COBRA_ONLY

#include "include/webchat.h"
#include "include/file.h"
#include "include/vsh.h"
#include "include/ps2_disc.h"
#include "include/ps2_classic.h"
#include "include/xmb_savebmp.h"
#include "include/singstar.h"
#include "include/autopoweroff.h"

#include "include/gamedata.h"
#include "include/psxemu.h"

#include "include/debug_mem.h"
#include "include/fix_game.h"
#include "include/ftp.h"
#include "include/ps3mapi.h"
#include "include/stealth.h"
#include "include/video_rec.h"

#include "include/games_html.h"
#include "include/games_xml.h"

#include "include/cpursx.h"
#include "include/setup.h"
#include "include/togglers.h"

#include "include/_mount.h"
#include "include/file_manager.h"

#include "include/pkg_handler.h"
#include "include/fancontrol2.h"
#include "include/md5.h"

static inline void _sys_ppu_thread_exit(uint64_t val)
{
	system_call_1(SC_PPU_THREAD_EXIT, val); // prxloader = mandatory; cobra = optional; ccapi = don't use !!!
}

static inline sys_prx_id_t prx_get_module_id_by_address(void *addr)
{
	system_call_1(SC_GET_PRX_MODULE_BY_ADDRESS, (uint64_t)(uint32_t)addr);
	return (int)p1;
}

static void http_response(int conn_s, char *header, const char *url, int code, const char *msg)
{
	u16 slen; *header = NULL;

	if(code == CODE_VIRTUALPAD || code == CODE_GOBACK || code == CODE_CLOSE_BROWSER)
	{
		slen = sprintf(header,  HTML_RESPONSE_FMT,
								CODE_HTTP_OK, url, HTTP_RESPONSE_TITLE_LEN + strlen(msg), HTML_BODY, HTML_RESPONSE_TITLE, msg);
	}
	else
	{
		char templn[_2KB_];

		if(*msg == '/')
			{sprintf(templn, "%s : OK", msg+1); show_msg(templn);}
		else if(islike(msg, "http"))
			sprintf(templn, "<a style=\"%s\" href=\"%s\">%s</a>", HTML_URL_STYLE, msg, msg);
#ifdef PKG_HANDLER
		else if(code == CODE_INSTALL_PKG || code == CODE_DOWNLOAD_FILE)
		{
			sprintf(templn, "<style>a{%s}</style>%s", HTML_URL_STYLE, (code == CODE_INSTALL_PKG) ? "Installing " : "");
			char *p = strchr((char*)msg, '\n');
			if(p)
			{
				*p = NULL;
				if(code == CODE_INSTALL_PKG) add_breadcrumb_trail(templn, (char *)msg + 11); else strcat(templn, msg);
				if(code == CODE_DOWNLOAD_FILE || extcasecmp(pkg_path, ".p3t", 4) != 0) strcat(templn, "<p>To: \0"); add_breadcrumb_trail(templn, p + 5);
			}
			else
				strcat(templn, msg);

			code = CODE_HTTP_OK;
		}
#endif
		else
			sprintf(templn, "%s", msg);

		if(ISDIGIT(*msg) && ( (code == CODE_SERVER_BUSY || code == CODE_BAD_REQUEST) )) show_msg((char*)templn + 4);

#ifndef EMBED_JS
		if(css_exists)
		{
			sprintf(header, "<LINK href=\"%s\" rel=\"stylesheet\" type=\"text/css\">", COMMON_CSS); strcat(templn, header);
		}
		if(common_js_exists)
		{
			sprintf(header, SCRIPT_SRC_FMT, COMMON_SCRIPT_JS); strcat(templn, header);
		}
#endif

		sprintf(header, "<hr>" HTML_BUTTON_FMT "%s",
						HTML_BUTTON, " &#9664;  ", HTML_ONCLICK, ((code == CODE_RETURN_TO_ROOT) ? "/" : "javascript:window.history.back();"), HTML_BODY_END); strcat(templn, header);

		slen = sprintf(header,  HTML_RESPONSE_FMT,
								(code == CODE_RETURN_TO_ROOT) ? CODE_HTTP_OK : code, url, HTTP_RESPONSE_TITLE_LEN + strlen(templn), HTML_BODY, HTML_RESPONSE_TITLE, templn);
	}

	send(conn_s, header, slen, 0);
	sclose(&conn_s);
}

#ifdef SYS_ADMIN_MODE
static u8 check_password(char *param)
{
	u8 ret = 0;

	if((pwd_tries < 3) && (webman_config->ftp_password[0] != NULL))
	{
		char *pos = strstr(param, "pwd=");
		if(pos > param)
		{
			pwd_tries++;
			if(IS(pos + 4, webman_config->ftp_password)) {pwd_tries = 0, ret = 1;}
			--pos; *pos = NULL;
		}
	}

	return ret;
}
#endif

static char *prepare_html(char *pbuffer, char *templn, char *param, u8 is_ps3_http, u8 is_cpursx, bool mount_ps3)
{
	sprintf(pbuffer, HTML_HEADER); char *buffer = pbuffer + HTML_HEADER_SIZE;

	if(is_cpursx)
		buffer += concat(buffer, "<meta http-equiv=\"refresh\" content=\"6;URL=/cpursx.ps3\">");

	if(mount_ps3) {strcat(buffer, HTML_BODY); return  buffer;}

	buffer += concat(buffer,
								"<head><title>webMAN MOD</title>"
								"<style>"
								"a{" HTML_URL_STYLE "}"
								"#rxml,#rhtm,#rcpy,#wmsg{position:fixed;top:40%;left:30%;width:40%;height:90px;z-index:5;border:5px solid #ccc;border-radius:25px;padding:10px;color:#fff;text-align:center;background-image:-webkit-gradient(linear,0 0,0 100%,color-stop(0,#999),color-stop(0.02,#666),color-stop(1,#222));background-image:-moz-linear-gradient(top,#999,#666 2%,#222);display:none;}"
								"</style>"); // fallback style if external css fails

#ifndef EMBED_JS
	if(param[1] == 0)
	{
		// minimize times that these files are checked (at startup & root)
		css_exists = file_exists(COMMON_CSS);
		common_js_exists = file_exists(COMMON_SCRIPT_JS);
	}
	if(css_exists)
	{
		sprintf(templn, "<LINK href=\"%s\" rel=\"stylesheet\" type=\"text/css\">", COMMON_CSS); buffer += concat(buffer, templn);
	}
	else
#endif
	{
		buffer += concat(buffer,
						"<style type=\"text/css\"><!--\r\n"

						"a.s:active{color:#F0F0F0;}"
						"a:link{color:#909090;}"

						"a.f:active{color:#F8F8F8;}"
						"a,a.f:link,a:visited{color:#D0D0D0;}");

		if(!is_cpursx)
				buffer += concat(buffer,
						"a.d:link{color:#D0D0D0;background:0px 2px url('data:image/gif;base64,R0lGODlhEAAMAIMAAOenIumzLbmOWOuxN++9Me+1Pe+9QvDAUtWxaffKXvPOcfTWc/fWe/fWhPfckgAAACH5BAMAAA8ALAAAAAAQAAwAAARQMI1Agzk4n5Sa+84CVNUwHAz4KWzLMo3SzDStOkrHMO8O2zmXsAXD5DjIJEdxyRie0KfzYChYr1jpYVAweb/cwrMbAJjP54AXwRa433A2IgIAOw==') no-repeat;padding:0 0 0 20px;}"
						"a.w:link{color:#D0D0D0;background:url('data:image/gif;base64,R0lGODlhDgAQAIMAAAAAAOfn5+/v7/f39////////////////////////////////////////////wAAACH5BAMAAA8ALAAAAAAOABAAAAQx8D0xqh0iSHl70FxnfaDohWYloOk6papEwa5g37gt5/zO475fJvgDCW8gknIpWToDEQA7') no-repeat;padding:0 0 0 20px;}");

		buffer += concat(buffer,
						"a:active,a:active:hover,a:visited:hover,a:link:hover{color:#FFFFFF;}"
						".list{display:inline;}"
#ifdef PS3MAPI
						"table{border-spacing:0;border-collapse:collapse;}"
						".la{text-align:left;float:left}.ra{text-align:right;float:right;}"
#endif
						"input:focus{border:2px solid #0099FF;}"
						".propfont{font-family:\"Courier New\",Courier,monospace;text-shadow:1px 1px #101010;}"
						"body{background-color:#101010}body,a.s,td,th{color:#F0F0F0;white-space:nowrap");

		//if(file_exists("/dev_hdd0/xmlhost/game_plugin/background.jpg"))
		//	buffer += concat(buffer, "background-image: url(\"/dev_hdd0/xmlhost/game_plugin/background.jpg\");");

		if(is_ps3_http == 2)
			buffer += concat(buffer, "width:800px;}");
		else
			buffer += concat(buffer, "}");

		if(!islike(param, "/setup.ps3")) buffer += concat(buffer, "td+td{text-align:right;white-space:nowrap}");

		if(islike(param, "/index.ps3"))
		{
			buffer += concat(buffer,
							".gc{float:left;overflow:hidden;position:relative;text-align:center;width:280px;height:260px;margin:3px;border:1px dashed grey;}"
							".ic{position:absolute;top:5px;right:5px;left:5px;bottom:40px;}");

			if(is_ps3_http == 1)
				buffer += concat(buffer, ".gi{height:210px;width:267px;");
			else
				buffer += concat(buffer, ".gi{max-height:210px;max-width:260px;");

			buffer += concat(buffer,
							"position:absolute;bottom:0px;top:0px;left:0px;right:0px;margin:auto;}"
							".gn{position:absolute;height:38px;bottom:0px;right:7px;left:7px;text-align:center;}");
		}

		buffer += concat(buffer, ".bu{background:#444;}.bf{background:#121;}--></style>");
	}

	if(param[1] != NULL && !strstr(param, ".ps3")) {buffer += concat(buffer, "<base href=\""); urlenc(templn, param); strcat(templn, "/\">"); buffer += concat(buffer, templn);}

	if(is_ps3_http == 1)
		{sprintf(templn, "<style>%s</style>", ".gi{height:210px;width:267px"); buffer += concat(buffer, templn);}

	sprintf(templn, "</head>%s", HTML_BODY); buffer += concat(buffer, templn);

	char slider[40]; if(file_exists(MOBILE_HTML)) sprintf(slider, " [<a href=\"/games.ps3\">Slider</a>]"); else *slider = NULL;


	if(sys_admin)
	{
#ifdef PS3MAPI
	#ifdef WEB_CHAT
		sprintf(templn, "webMAN " WM_VERSION " %s <font style=\"font-size:18px\">[<a href=\"/\">%s</a>] [<a href=\"/index.ps3\">%s</a>]%s [<a href=\"/chat.ps3\">Chat</a>] [<a href=\"/home.ps3mapi\">PS3MAPI</a>] [<a href=\"/setup.ps3\">%s</a>]</b>", STR_TRADBY, STR_FILES, STR_GAMES, slider, STR_SETUP);
	#else
		sprintf(templn, "webMAN " WM_VERSION " %s <font style=\"font-size:18px\">[<a href=\"/\">%s</a>] [<a href=\"/index.ps3\">%s</a>]%s [<a href=\"/home.ps3mapi\">PS3MAPI</a>] [<a href=\"/setup.ps3\">%s</a>]</b>", STR_TRADBY, STR_FILES, STR_GAMES, slider, STR_SETUP );
	#endif
#else
	#ifdef WEB_CHAT
		sprintf(templn, "webMAN " WM_VERSION " %s <font style=\"font-size:18px\">[<a href=\"/\">%s</a>] [<a href=\"/index.ps3\">%s</a>]%s [<a href=\"/chat.ps3\">Chat</a>] [<a href=\"/setup.ps3\">%s</a>]</b>", STR_TRADBY, STR_FILES, STR_GAMES, slider, STR_SETUP);
	#else
		sprintf(templn, "webMAN " WM_VERSION " %s <font style=\"font-size:18px\">[<a href=\"/\">%s</a>] [<a href=\"/index.ps3\">%s</a>]%s [<a href=\"/setup.ps3\">%s</a>]</b>", STR_TRADBY, STR_FILES, STR_GAMES, slider, STR_SETUP );
	#endif
#endif
	}
#ifdef SYS_ADMIN_MODE
	else
		sprintf(templn, "webMAN " WM_VERSION " %s <font style=\"font-size:18px\">[<a href=\"/\">%s</a>] [<a href=\"/index.ps3\">%s</a>]%s</b>", STR_TRADBY, STR_FILES, STR_GAMES, slider);
#endif

	buffer += concat(buffer, templn);
	return buffer;
}

static void handleclient(u64 conn_s_p)
{
	int conn_s = (int)conn_s_p; // main communications socket

	size_t header_len;
	sys_addr_t sysmem = NULL;

	char param[HTML_RECV_SIZE];
	int fd;

	char *file_query = param + HTML_RECV_LAST; *file_query = NULL;

	if(conn_s_p == START_DAEMON || conn_s_p == REFRESH_CONTENT)
	{
		if(conn_s_p == START_DAEMON)
		{
			vshnet_setUpdateUrl("http://127.0.0.1/dev_hdd0/ps3-updatelist.txt"); // custom update file

 #ifndef ENGLISH_ONLY
			update_language();
 #endif
			if(profile || (!(webman_config->wmstart) && STR_WMSTART[0] != NULL))
			{
				sys_timer_sleep(10);
				sprintf(param, "%s%s", STR_WMSTART, SUFIX2(profile));
				show_msg(param);
			}

			if(webman_config->bootd) waitfor("/dev_usb", webman_config->bootd); // wait for any usb
		}
		else //if(conn_s_p == REFRESH_CONTENT)
		{
			{DELETE_CACHED_GAMES} // refresh XML will force "refresh HTML" to rebuild the cache file
		}

		//////////// usb ports ////////////
		if(cellFsOpendir("/", &fd) == CELL_FS_SUCCEEDED)
		{
			CellFsDirent entry; u64 read_e; u8 indx = 5; // 5 = /dev_usb006, 6 = /dev_usb007

			while(cellFsReaddir(fd, &entry, &read_e) == 0 && read_e > 0)
			{
				if(islike(entry.d_name, "dev_usb"))
				{
					if(entry.d_name[7] == '0' && entry.d_name[8] == '0' && entry.d_name[9] < '4') continue;
					sprintf(drives[indx], "/%s", entry.d_name);
					indx++; if(indx > 6) break;
				}
			}
			cellFsClosedir(fd);
		}
		///////////////////////////////////

		cellFsMkdir(WMTMP, DMODE);

		check_cover_folders(param);

		for(u8 i = 0; i < 12; i++)
		{
			if(file_exists(wm_icons[i]) == false)
			{
				sprintf(param, "/dev_flash/vsh/resource/explore/icon/%s", wm_icons[i] + 23); strcpy(wm_icons[i], param);
				if(file_exists(param)) continue;

				char *icon = wm_icons[i] + 32;
				if(i == gPS3 || i == iPS3)	strcpy(icon, "user/024.png\0"); else // ps3
				if(i == gPSX || i == iPSX)	strcpy(icon, "user/026.png\0"); else // psx
				if(i == gPS2 || i == iPS2)	strcpy(icon, "user/025.png\0"); else // ps2
				if(i == gPSP || i == iPSP)	strcpy(icon, "user/022.png\0"); else // psp
				if(i == gDVD || i == iDVD)	strcpy(icon, "user/023.png\0"); else // dvd
											strcpy(icon + 5, "icon_home.png\0"); // setup / eject
			}
		}

#ifndef EMBED_JS
		css_exists = file_exists(COMMON_CSS);
		common_js_exists = file_exists(COMMON_SCRIPT_JS);
#endif

#ifdef NOSINGSTAR
		no_singstar_icon();
#endif

		sys_ppu_thread_t t_id;
		sys_ppu_thread_create(&t_id, update_xml_thread, conn_s_p, THREAD_PRIO, THREAD_STACK_SIZE_64KB, SYS_PPU_THREAD_CREATE_NORMAL, THREAD_NAME_CMD);

		if(conn_s_p == START_DAEMON)
		{
#ifdef COBRA_ONLY
			uint8_t cconfig[15];
			CobraConfig *cobra_config = (CobraConfig*) cconfig;
			memset(cobra_config, 0, 15);
			cobra_read_config(cobra_config);

			if(webman_config->nospoof)
			{
				cobra_config->spoof_version  = 0;
				cobra_config->spoof_revision = 0;
			}
			else
			{   // cobra spoofer not working on 4.53
				if(c_firmware!=4.53f)
				{
					cobra_config->spoof_version = 0x0481;
					cobra_config->spoof_revision = 66786;
				}
			}

			if( cobra_config->ps2softemu == 0 && cobra_get_ps2_emu_type() == PS2_EMU_SW )
				cobra_config->ps2softemu =  1;

			cobra_write_config(cobra_config);

 #endif
 #ifdef SPOOF_CONSOLEID
			spoof_idps_psid();
 #endif
 #ifdef COBRA_ONLY
			#ifdef REMOVE_SYSCALLS
			if(webman_config->spp & 1) //remove syscalls & history
			{
				sys_timer_sleep(5);

				remove_cfw_syscalls(webman_config->keep_ccapi);
				delete_history(true);
			}
			else
			#endif
			if(webman_config->spp & 2) //remove history & block psn servers (offline mode)
			{
				delete_history(false);
				block_online_servers(false);
			}
			#ifdef OFFLINE_INGAME
			if(file_exists(WMNET_DISABLED)) //re-enable network (force offline in game)
			{
				net_status = 1;
				poll_start_play_time();
			}
			#endif
 #endif
		}

		sys_ppu_thread_exit(0);
	}

 #ifdef USE_DEBUG
	ssend(debug_s, "waiting...");
 #endif

	if(loading_html > 10) loading_html = 0;

	bool is_local = true;
	sys_net_sockinfo_t conn_info_main;

	char cmd[16], header[HTML_RECV_SIZE], *mc = NULL;

 #ifdef WM_REQUEST
	struct CellFsStat buf; u8 wm_request = (cellFsStat(WMREQUEST_FILE, &buf) == CELL_FS_SUCCEEDED);;

	if(!wm_request)
 #endif
	{
		sys_net_get_sockinfo(conn_s, &conn_info_main, 1);

		char *ip_address = cmd;
		is_local = (conn_info_main.local_adr.s_addr == conn_info_main.remote_adr.s_addr);

		sprintf(ip_address, "%s", inet_ntoa(conn_info_main.remote_adr));
		if(webman_config->bind && (!is_local) && !islike(ip_address, webman_config->allow_ip))
		{
			http_response(conn_s, header, param, CODE_BAD_REQUEST, STR_ERROR);

			goto exit_handleclient;
		}

		if(!webman_config->netd[0] && !webman_config->neth[0][0]) strcpy(webman_config->neth[0], ip_address); // show client IP if /net0 is empty
		if(!webman_config->bind) strcpy(webman_config->allow_ip, ip_address);
	}

	// check available free memory
	{
		_meminfo meminfo;
		u8 retries = 0;

again3:
		{ system_call_1(SC_GET_FREE_MEM, (uint64_t)(u32) &meminfo); }

		if((meminfo.avail) < ( _64KB_ + MIN_MEM )) //leave if less than min memory
		{
			#ifdef USE_DEBUG
			ssend(debug_s, "!!! NOT ENOUGH MEMORY!\r\n");
			#endif

			retries++;
			sys_timer_sleep(1);
			if((retries < 5) && working) goto again3;

			http_response(conn_s, header, param, CODE_SERVER_BUSY, STR_ERROR); BEEP3;

			#ifdef WM_REQUEST
			if(wm_request) cellFsUnlink(WMREQUEST_FILE);
			#endif

			goto exit_handleclient;
		}
	}

	#ifdef WM_REQUEST
	if(!wm_request)
	#endif
	{
		struct timeval tv;
		tv.tv_usec = 0;
		tv.tv_sec = 3;
		setsockopt(conn_s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

		int optval = HTML_RECV_SIZE;
		setsockopt(conn_s, SOL_SOCKET, SO_RCVBUF, &optval, sizeof(optval));
	}

parse_request:

  {
	u8 served = 0, is_binary = WEB_COMMAND;	// served http request?, is_binary: 0 = http command, 1 = file, 2 = folder listing
	int8_t sort_order = 1, sort_by = 0;
	u64 c_len = 0;

	u8 is_cpursx = 0;
	u8 is_popup = 0, auto_mount = 0;
	u8 is_ps3_http = 0;

//// process commands ////

	while(!served && working)
	{
		served++;
		*header = NULL;

		if(!mc)
		{
			#ifdef USE_DEBUG
			ssend(debug_s, "ACC - ");
			#endif

 #ifdef WM_REQUEST
			if(wm_request)
			{
				if(buf.st_size > 5 && buf.st_size < HTML_RECV_SIZE && read_file(WMREQUEST_FILE, header, buf.st_size, 0) > 4)
				{
					if(*header == '/') {strcpy(param, header); buf.st_size = sprintf(header, "GET %s", param);}
					for(size_t n = buf.st_size; n > 4; n--) if(header[n] == ' ') header[n] = '+';
					if(islike(header, "GET /play.ps3")) {if(IS_INGAME) {sys_timer_sleep(1); served = 0; is_ps3_http = 1; continue;}}
				}
				cellFsUnlink(WMREQUEST_FILE);
			}
 #endif
		}
		else sprintf(header, "GET %s", mc + 1);

		mc = NULL;

		if(((*header == 'G') || recv(conn_s, header, HTML_RECV_SIZE, 0) > 0) && *header == 'G' && header[4] == '/') // serve only GET /xxx requests
		{
			if(strstr(header, "x-ps3-browser")) is_ps3_http = 1; else
			if(strstr(header, "Gecko/36"))  	is_ps3_http = 2; else
												is_ps3_http = 0;

			for(size_t n = 0; header[n]; n++) {if(header[n] == '\r' || header[n] == '\n' || n >= HTML_RECV_SIZE) {header[n] = 0; break;}}

			ssplit(header, cmd, 15, header, HTML_RECV_LAST);
			ssplit(header, param, HTML_RECV_LAST, cmd, 15);

			char *param_original = header; // used in /download.ps3

 #ifdef WM_REQUEST
			if(wm_request) { for(size_t n = 0; param[n]; n++) {if(param[n] == 9) param[n] = ' ';} } wm_request = 0;
 #endif
			if(!islike(param, "/setup.ps3?")) { mc = strstr(param, ";/"); if(mc) {*mc = NULL; strcpy(header, param);} }

			bool allow_retry_response = true, small_alloc = true; u8 mobile_mode = false;

 #ifdef USE_DEBUG
	ssend(debug_s, param);
	ssend(debug_s, "\r\n");
 #endif

 #ifdef SYS_ADMIN_MODE
			if(!sys_admin)
			{
				bool accept = false;
				if(*param != '/') accept = false; else
				if(islike(param, "/admin.ps3")) accept = true; else
				{
					accept = ( IS(param, "/")

							|| islike(param, "/cpursx")

							|| islike(param, "/mount")
							|| islike(param, "/index.ps3")
							|| islike(param, "/games.ps3")
							|| islike(param, "/play.ps3")

							|| islike(param, "/refresh.ps3")
							|| islike(param, "/eject.ps3")
							|| islike(param, "/insert.ps3")
	#ifdef EXT_GDATA
							|| islike(param, "/extgd.ps3")
	#endif
	#ifdef WEB_CHAT
							|| islike(param, "/chat.ps3")
	#endif
	#ifndef LITE_EDITION
							|| islike(param, "/popup.ps3")
	#endif
							 );

					if(!accept)
					{
	#ifndef LITE_EDITION
						if(check_password(param)) {is_local = true, sys_admin = 1;}
	#endif
						if(is_local)
							accept = ( islike(param, "/setup.ps3")
	#ifdef PKG_HANDLER
									|| islike(param, "/install")
									|| islike(param, "/download.ps3")
	#endif
	#ifdef PS3_BROWSER
									|| islike(param, "/browser.ps3")
	#endif
									|| islike(param, "/restart.ps3")
									|| islike(param, "/shutdown.ps3"));
					}

					if(!accept)
					{
						int ext_pos = strlen(param) - 4; if(ext_pos < 0) ext_pos = 0; sys_admin = 0;
						char *ext = param + ext_pos;
						if(!accept && (islike(param, "/net") || file_exists(param)) && (_IS(ext, ".jpg") || _IS(ext, ".png") || IS(ext, ".css") || strstr(ext, ".js") || IS(ext, "html")) ) accept = true;
					}
				}

				if(!accept)
				{
					sprintf(param, "%s\nADMIN %s", STR_ERROR, STR_DISABLED);
					http_response(conn_s, header, param, CODE_BAD_REQUEST, param);
					goto exit_handleclient;
				}
			}
 #endif

 #ifdef VIRTUAL_PAD
			bool is_pad = islike(param, "/pad.ps3");

			if(!is_pad)
 #endif
			{
	redirect_url:

				urldec(param, param_original);

				if(islike(param, "/setup.ps3")) goto html_response;
			}

 #ifdef VIRTUAL_PAD
			if(is_pad || islike(param, "/combo.ps3") || islike(param, "/play.ps3"))
			{
				// /pad.ps3                      (see vpad.h for details)
				// /combo.ps3                    simulates a combo without actually send the buttons
				// /play.ps3                     start game from disc icon
				// /play.ps3?col=<col>&seg=<seg>  click item on XMB

				u8 ret = 0, is_combo = (param[2] == 'a') ? 0 : (param[1] == 'c') ? 2 : 1; // 0 = /pad.ps3   1 = /play.ps3   2 = /combo.ps3

				char *buttons = param + 9 + is_combo;

				if(is_combo != 1) {if(!webman_config->nopad) ret = parse_pad_command(buttons, is_combo);}
				else
				{   // default: play.ps3?col=game&seg=seg_device
					char *pos, col[16], seg[80], *param2 = buttons; *col = *seg = NULL;
					pos = strstr(param2, "col="); if(pos) get_value(col, pos + 4, 16); // game / video / friend / psn / network / music / photo / tv
					pos = strstr(param2, "seg="); if(pos) get_value(seg, pos + 4, 80);
					launch_disc(col, seg);
				}

				if(is_combo == 1 && param[10] != '?') sprintf(param, "/cpursx.ps3");
				else
				{
					if((ret == 'X') && IS_ON_XMB) goto reboot;

					if(!mc) http_response(conn_s, header, param, CODE_VIRTUALPAD, buttons);

					goto exit_handleclient;
				}
			}
 #elif defined(LITE_EDITION)
			if(islike(param, "/play.ps3"))
			{
				// /play.ps3                     start game from disc icon

				// default: play.ps3?col=game&seg=seg_device
				char col[16], seg[40]; *col = *seg = NULL;
				launch_disc(col, seg);

				sprintf(param, "/cpursx.ps3");
			}
 #endif //  #ifdef VIRTUAL_PAD

 #ifdef SYS_ADMIN_MODE
			if(islike(param, "/admin.ps3"))
			{
				// /admin.ps3?enable&pwd=<password>  enable admin mode
				// /admin.ps3?disable                disable admin mode
				// /admin.ps3?0                      disable admin mode

				if(param[10] == 0 || param[11] == 0) ; else
				if(~param[11] & 1) sys_admin = 0; else
				{
					sys_admin = check_password(param);
				}

				sprintf(param, "ADMIN %s", sys_admin ? STR_ENABLED : STR_DISABLED);

				if(!mc) http_response(conn_s, header, param, CODE_RETURN_TO_ROOT, param);

				goto exit_handleclient;
			}
 #endif
			{char *pos = strstr(param, "?restart.ps3"); if(pos) {*pos = NULL; do_restart = sys_admin;}}

			if(islike(param, "/cpursx_ps3"))
			{
				char *cpursx = header, *buffer = param; get_cpursx(cpursx);
 /*				// prevents flickering but cause error 80710336 in ps3 browser (silk mode)
				sprintf(buffer, "<meta http-equiv=\"refresh\" content=\"15;URL=%s\">"
								"<script>parent.document.getElementById('lbl_cpursx').innerHTML = \"%s\";</script>",
								"/cpursx_ps3", cpursx);
 */
				int buf_len = sprintf(buffer, "<meta http-equiv=\"refresh\" content=\"15;URL=%s\">"
											  "%s"
											  "<a href=\"%s\" target=\"_parent\" style=\"text-decoration:none;\">"
											  "<font color=\"#fff\">%s</a>",
											  "/cpursx_ps3", HTML_BODY, "/cpursx.ps3", cpursx);
 #ifndef EMBED_JS
				if(css_exists)
				{
					buf_len += sprintf(buffer + buf_len, "<LINK href=\"%s\" rel=\"stylesheet\" type=\"text/css\">", COMMON_CSS);
				}
				if(common_js_exists)
				{
					buf_len += sprintf(buffer + buf_len, SCRIPT_SRC_FMT, COMMON_SCRIPT_JS);
				}
 #endif
				buf_len = sprintf(header, HTML_RESPONSE_FMT,
										  CODE_HTTP_OK, "/cpursx_ps3", HTML_HEADER_SIZE + HTML_BODY_END_SIZE + buf_len, HTML_HEADER, buffer, HTML_BODY_END);

				send(conn_s, header, buf_len, 0);

				goto exit_handleclient;
			}

			if(sys_admin && islike(param, "/dev_blind"))
			{
				// /dev_blind          auto-enable & access /dev_blind
				// /dev_blind?         shows status of /dev_blind
				// /dev_blind?0        mounts /dev_blind
				// /dev_blind?enable   mounts /dev_blind
				// /dev_blind?1        unmounts /dev_blind
				// /dev_blind?disable  unmounts /dev_blind

				is_binary = FOLDER_LISTING, small_alloc = false;
				goto html_response;
			}

 #ifdef PKG_HANDLER
			if(islike(param, "/download.ps3"))
			{
				// /download.ps3?url=<url>            (see pkg_handler.h for details)
				// /download.ps3?to=<path>&url=<url>

				char msg[MAX_LINE_LEN], filename[MAX_PATH_LEN]; memset(msg, 0, MAX_LINE_LEN); *filename = NULL;

				setPluginActive();

				int ret = download_file(strchr(param_original, '%') ? (param_original + 13) : (param + 13), msg);

				char *dlpath = strchr(msg, '\n'); // get path in "...\nTo: /path/"

				if(dlpath)
				{
					*dlpath = NULL; // limit string to url in "Downloading http://blah..."

					char *dlfile = strrchr(msg, '/');
					if(dlfile) sprintf(filename, "%s%s", dlpath + 5, dlfile);

					*dlpath = '\n'; // restore line break
				}

				#ifdef WM_REQUEST
				if(!wm_request)
				#endif
				{
					if(!mc) http_response(conn_s, header, param, (ret == FAILED) ? CODE_BAD_REQUEST : CODE_DOWNLOAD_FILE, msg);
				}

				show_msg(msg);

				wait_for_xml_download(filename, param);

				setPluginInactive();
				goto exit_handleclient;
			}

			if(islike(param, "/install.ps3") || islike(param, "/install_ps3"))
			{
				// /install.ps3<pkg-path>  (see pkg_handler.h for details)
				// /install.ps3<pkg-path>? conditional install
				// /install_ps3<pkg-path>  install & keep pkg
				// /install.ps3?url=<url>  download, auto-install pkg & delete pkg
				// /install_ps3?url=<url>  download, auto-install pkg & keep pkg in /dev_hdd0/packages
				// /install.ps3<pkg-path>?restart.ps3
				// /install.ps3?url=<url>?restart.ps3

				size_t last_char = strlen(param) - 1;
				if(param[last_char] == '?')
				{
					param[last_char] = NULL;
					get_pkg_size_and_install_time(param + 12);
					if(isDir(install_path))
					{
						strcpy(param, install_path);
						is_binary = FOLDER_LISTING, small_alloc = false;
						goto html_response;
					}
				}

				pkg_delete_after_install = (param[8] == '.');

				char msg[MAX_LINE_LEN]; memset(msg, 0, MAX_LINE_LEN);

				setPluginActive();

				int ret = installPKG(param + 12, msg);

				#ifdef WM_REQUEST
				if(!wm_request)
				#endif
				{
					if(!mc) http_response(conn_s, header, param, (ret == FAILED) ? CODE_BAD_REQUEST : CODE_INSTALL_PKG, msg);
				}

				show_msg(msg);

				if(pkg_delete_after_install || do_restart)
				{
					if(loading_html) loading_html--;

					wait_for_pkg_install();

					setPluginInactive();
					if(do_restart) goto reboot;
					if(mc) goto exit_handleclient;
					sys_ppu_thread_exit(0);
				}

				setPluginInactive();

				goto exit_handleclient;
			}
 #endif // #ifdef PKG_HANDLER

 #ifdef PS3_BROWSER
			if(islike(param, "/browser.ps3"))
			{
				// /browser.ps3$rsx_pause                  pause rsx processor
				// /browser.ps3$rsx_continue               continue rsx processor
				// /browser.ps3$block_servers              block url of PSN servers in lv2
				// /browser.ps3$restore_servers            restore url of PSN servers in lv2
				// /browser.ps3$show_idps                  show idps/psid (same as R2+O)
				// /browser.ps3$ingame_screenshot          enable screenshot in-game on CFW without the feature (same as R2+O)
				// /browser.ps3$disable_syscalls           disable CFW syscalls
				// /browser.ps3$toggle_rebug_mode          toggle rebug mode (swap VSH REX/DEX)
				// /browser.ps3$toggle_normal_mode         toggle normal mode (swap VSH REX/CEX)
				// /browser.ps3$toggle_debug_menu          toggle debug menu (DEX/CEX)
				// /browser.ps3$toggle_cobra               toggle Cobra (swap stage2)
				// /browser.ps3$toggle_ps2emu              toggle ps2emu
				// /browser.ps3$enable_classic_ps2_mode    creates 'classic_ps2_mode' to enable PS2 classic in PS2 Launcher (old rebug)
				// /browser.ps3$disable_classic_ps2_mode   deletes 'classic_ps2_mode' to enable PS2 ISO in PS2 Launcher (old rebug)
				// /browser.ps3$screenshot_xmb<path>       capture XMB screen
				// /browser.ps3?<url>                      open url on PS3 browser
				// /browser.ps3/<webman_cmd>               execute webMAN command on PS3 browser
				// /browser.ps3$<explore_plugin_command>   execute explore_plugin command on XMB (http://www.psdevwiki.com/ps3/Explore_plugin#Example_XMB_Commands)
				// /browser.ps3*<xmb_plugin_command>       execute xmb_plugin commands on XMB (http://www.psdevwiki.com/ps3/Xmb_plugin#Function_23_Examples)

				char *param2 = param + 12, *url = param + 13;

				if(islike(param2, "$rsx"))
				{
					static u8 rsx = 1;
					if(strstr(param2, "pause")) rsx = 1;
					if(strstr(param2, "cont"))  rsx = 0;
					rsx_fifo_pause(rsx); // rsx_pause / rsx_continue
					rsx ^= 1;
				}
				else
				if(islike(param2, "$block_servers"))
				{
					block_online_servers(true);
				}
				else
 				if(islike(param2, "$restore_servers"))
				{
					restore_blocked_urls();
				}
				else
				if(islike(param2, "$show_idps"))
				{
					show_idps(header);
				}
				else
/*
				if(islike(param2, "$registryInt(0x"))
				{
					int id, value;
					id = convertH(param2 + 15);

					char *pos = strstr(param2 + 16, "=");
					if(pos)
					{
						value = val(pos + 1);
						xsetting_D0261D72()->saveRegistryIntValue(id, value);
					}

					xsetting_D0261D72()->loadRegistryIntValue(id, &value);
					sprintf(param2 + strlen(param2), " => %i", value);
				}
				else
				if(islike(param2, "$registryString(0x"))
				{
					int id, len, size = 0;
					id = convertH(param2 + 18);

					char *pos = strstr(param2 + 19, "=");
					if(pos)
					{
						pos++, len = strlen(pos);
						xsetting_D0261D72()->saveRegistryStringValue(id, pos, len);
					}

					len = strlen(param2); char *value = param2 + len + 8;
					char *pos2 = strstr(param2 + 19, ","); if(pos2) size = val(pos2 + 1); if(size <= 0) size = 0x80;
					xsetting_D0261D72()->loadRegistryStringValue(id, value, size);
					sprintf(param2 + len, " => %s", value);
				}
				else
*/
   #ifndef LITE_EDITION
				if(islike(param2, "$ingame_screenshot"))
				{
					enable_ingame_screenshot();
				}
				else
   #endif
   #ifdef REMOVE_SYSCALLS
				if(islike(param2, "$disable_syscalls"))
				{
					disable_cfw_syscalls(strcasestr(param, "ccapi")!=NULL);
				}
				else
   #endif
   #ifdef REX_ONLY
				if(islike(param2, "$toggle_rebug_mode"))
				{
					if(toggle_rebug_mode()) goto reboot;
				}
				else
				if(islike(param2, "$toggle_normal_mode"))
				{
					if(toggle_normal_mode()) goto reboot;
				}
				else
				if(islike(param2, "$toggle_debug_menu"))
				{
					toggle_debug_menu();
				}
				else
   #endif
   #ifdef COBRA_ONLY
    #ifndef LITE_EDITION
				if(islike(param2, "$toggle_cobra"))
				{
					if(toggle_cobra()) goto reboot;
				}
				else
				if(islike(param2, "$toggle_ps2emu"))
				{
					toggle_ps2emu();
				}
				else
				if(strstr(param2, "le_classic_ps2_mode"))
				{
					bool classic_ps2_enabled;

					if(islike(param2, "$disable_"))
					{
						// $disable_classic_ps2_mode
						classic_ps2_enabled = true;
					}
					else
					if(islike(param2, "$enable_"))
					{
						// $enable_classic_ps2_mode
						classic_ps2_enabled = false;
					}
					else
					{
						// $toggle_classic_ps2_mode
						classic_ps2_enabled = file_exists(PS2_CLASSIC_TOGGLER);
					}

					if(classic_ps2_enabled)
						disable_classic_ps2_mode();
					else
						enable_classic_ps2_mode();

					sprintf(header, "PS2 Classic %s", classic_ps2_enabled ? STR_DISABLED : STR_ENABLED);
					show_msg(header);
					sys_timer_sleep(3);
				}
				else
    #endif //#ifndef LITE_EDITION
   #endif // #ifdef COBRA_ONLY
				if(IS_ON_XMB)
				{   // in-XMB
   #ifdef XMB_SCREENSHOT
					if(islike(param2, "$screenshot_xmb")) {sprintf(header, "%s", param + 27); saveBMP(header, false); sprintf(url, HTML_URL, header, header);} else
   #endif
					{
						if(*param2 == NULL) sprintf(param2, "/");
						if(*param2 == '/' ) {do_umount(false); sprintf(header, "http://%s%s", local_ip, param2); open_browser(header, 0);} else
						if(*param2 == '$' ) {int view = View_Find("explore_plugin"); if(view) {explore_interface = (explore_plugin_interface *)plugin_GetInterface(view, 1); explore_interface->ExecXMBcommand(url,0,0);}} else
						if(*param2 == '?' ) {do_umount(false);  open_browser(url, 0);} else
											{					open_browser(url, 1);} // example: /browser.ps3*regcam:reg?   More examples: http://www.psdevwiki.com/ps3/Xmb_plugin#Function_23

						show_msg(url);
					}
				}
				else
 					sprintf(url, "ERROR: Not in XMB!");

				if(!mc) http_response(conn_s, header, param, CODE_HTTP_OK, url);

				goto exit_handleclient;
			}
 #endif // #ifdef PS3_BROWSER

 #if defined(FIX_GAME) || defined(COPY_PS3)
			if(strstr(param, ".ps3$abort"))
			{
				// /copy.ps3$abort
				// /fixgame.ps3$abort
				do_restart = false;

				if(copy_in_progress) {copy_aborted = true; show_msg((char*)STR_CPYABORT);}   // /copy.ps3$abort
				else
				if(fix_in_progress)  {fix_aborted = true;  show_msg((char*)"Fix aborted!");} // /fixgame.ps3$abort

				sprintf(param, "/");
			}
 #endif

 #ifdef GET_KLICENSEE
			if(islike(param, "/klic.ps3"))
			{
				// /klic.ps3           Show status of auto log klicensees
				// /klic.ps3?log       Toggle auto log of klicensees
				// /klic.ps3?auto      Auto-Log: Started
				// /klic.ps3?off       Auto-Log: Stopped
				// /dev_hdd0/klic.log  Log file

				if(npklic_struct_offset == 0)
				{
					// get klicensee struct
					vshnet_5EE098BC = getNIDfunc("vshnet", 0x5EE098BC, 0);
					int* func_start = (int*)(*((int*)vshnet_5EE098BC));
					npklic_struct_offset = (((*func_start) & 0x0000FFFF) << 16) + ((*(func_start+5)) & 0x0000FFFF) + 0xC;//8;
				}

				#define KL_OFF     0
				#define KL_GET     1
				#define KL_AUTO    2

				u8 klic_polling_status = klic_polling;

				if(param[10] == 'o') klic_polling = KL_OFF;  else
				if(param[10] == 'a') klic_polling = KL_AUTO; else
				if(param[10] == 'l') klic_polling = klic_polling ? KL_OFF : KL_GET; // toggle

				if((klic_polling_status == KL_OFF) && (klic_polling == KL_AUTO))
				{
					if(IS_ON_XMB) http_response(conn_s, header, param, CODE_HTTP_OK, (char*)"/KLIC: Waiting for game...");

					// wait until game start
					while((klic_polling == KL_AUTO) && IS_ON_XMB && working) {sys_timer_usleep(500000);}
				}

				char kl[0x120], prev[0x200], buffer[0x200]; memset(kl, 0, 120);

				if(IS_INGAME)
				{
					hex_dump(kl, KLICENSEE_OFFSET, KLICENSEE_SIZE);
					get_game_info(); sprintf(buffer, "%s %s</H2>"
													 "%s%s<br>"
													 "%s%s<br>"
													 "%s%s<p>",
													 _game_TitleID, _game_Title,
													 "KLicensee: ", hex_dump(kl, KLICENSEE_OFFSET, KLICENSEE_SIZE),
													 "Content ID: ", (char*)(KLIC_CONTENT_ID_OFFSET),
													 "File: ", (char*)(KLIC_PATH_OFFSET));
				}
				else
					{sprintf(buffer, "ERROR: <a style=\"%s\" href=\"play.ps3\">%s</a><p>", HTML_URL_STYLE, "KLIC: Not in-game!"); klic_polling = KL_OFF; show_msg((char*)"KLIC: Not in-game!");}

				sprintf(prev, "%s", ((klic_polling_status) ? (klic_polling ? "Auto-Log: Running" : "Auto-Log: Stopped") :
															((klic_polling == KL_GET)  ? "Added to Log" :
															 (klic_polling == KL_AUTO) ? "Auto-Log: Started" : "Enable Auto-Log")));

				sprintf(header, "<a style=\"%s\" href=\"%s\">%s</a>", HTML_URL_STYLE,
							(klic_polling_status > 0 && klic_polling > 0) ? "klic.ps3?off"  :
							( (klic_polling_status | klic_polling)  == 0) ? "klic.ps3?auto" : "dev_hdd0/klic.log", prev); strcat(buffer, header);

				http_response(conn_s, header, param, CODE_HTTP_OK, buffer);

				if(*kl && (klic_polling != KL_OFF))
				{
					get_game_info(); sprintf(header, "%s [%s]", _game_Title, _game_TitleID);

					sprintf(buffer, "%s\n\n%s", header, (char*)(KLIC_PATH_OFFSET));
					show_msg(buffer);

					if(klic_polling == KL_GET)
					{
						sprintf(buffer, "%s%s\n%s%s", "KLicensee: ", kl, "Content ID: ", (char*)(KLIC_CONTENT_ID_OFFSET));
						show_msg(buffer);
					}

					if(klic_polling_status == KL_OFF)
					{
						while((klic_polling != KL_OFF) && IS_INGAME && working)
						{
							hex_dump(kl, (int)KLICENSEE_OFFSET, KLICENSEE_SIZE);
							sprintf(buffer, "%s %s %s %s\r\n", kl, (char*)(KLIC_CONTENT_ID_OFFSET), header, (char*)(KLIC_PATH_OFFSET));

							if(klic_polling == KL_AUTO && !strcmp(buffer, prev)) {sys_timer_usleep(10000); continue;}

							save_file("/dev_hdd0/klic.log", buffer, APPEND_TEXT);

							if(klic_polling == KL_GET) break; strcpy(prev, buffer);
						}

						klic_polling = KL_OFF;
					}
				}

				goto exit_handleclient;
			}
 #endif

 #ifndef LITE_EDITION

			#ifdef WEB_CHAT
			if(islike(param, "/chat.ps3"))
			{
				// /chat.ps3    webchat

				is_popup = 1;
				goto html_response;
			}
			#endif
			if(islike(param, "/popup.ps3"))
			{
				// /popup.ps3
				// /popup.ps3<msg>

				if(param[10] == NULL) show_info_popup = true; else is_popup = 1;

				goto html_response;
			}
			if(islike(param, "/remap.ps3") || islike(param, "/unmap.ps3"))
			{
				// /remap.ps3<path1>&to=<path2>       files on path1 is accessed from path2
				// /remap.ps3?src=<path1>&to=<path2>
				// /unmap.ps3<path>
				// /unmap.ps3?src=<path>

				char *pos, *path1 = header, *path2 = header + MAX_PATH_LEN, *url = header + 2 * MAX_PATH_LEN, *title = header + 2 * MAX_PATH_LEN;

				memset(header, 0, HTML_RECV_SIZE);

				if(param[10] == '/') get_value(path1, param + 10, MAX_PATH_LEN); else
				if(param[11] == '/') get_value(path1, param + 11, MAX_PATH_LEN); else
				{
					pos = strstr(param, "src=");
					if(pos) get_value(path1, pos + 4, MAX_PATH_LEN);
				}

				bool isremap = (param[1] == 'r');

				if(isremap)
				{
					pos = strstr(param, "to="); sprintf(path2, "/dev_bdvd");
					if(pos) get_value(path2, pos + 3, MAX_PATH_LEN);
				}

				if(file_exists(isremap ? path2 : path1))
				{
					sys_map_path(path1, path2);

					htmlenc(url, path2, 1); urlenc(url, path1); htmlenc(title, path1, 0);

					if(isremap && *path2 != NULL)
					{
						htmlenc(path1, path2, 0);
						sprintf(param,  "Remap: " HTML_URL "<br>"
										"To: "    HTML_URL "<p>"
										"Unmap: " HTML_URL2, url, title, path1, path2, "/unmap.ps3", url, title);
					}
					else
					{
						sprintf(param, "Unmap: " HTML_URL, url, title);
					}
				}
				else
					sprintf(param, "%s", STR_ERROR);

				if(!mc) http_response(conn_s, header, param, CODE_HTTP_OK, param);

				goto exit_handleclient;
			}
			if(islike(param, "/wait.ps3"))
			{
				// /wait.ps3?<secs>
				// /wait.ps3/<path>

				if(param[9] == '/')
					waitfor(param + 9, 30);
				else
					sys_timer_sleep(val(param + 10));

				if(!mc) http_response(conn_s, header, param, CODE_HTTP_OK, param);

				goto exit_handleclient;
			}
			if(islike(param, "/netstatus.ps3"))
			{
				// /netstatus.ps3          toggle network access in registry
				// /netstatus.ps3?1        enable network access in registry
				// /netstatus.ps3?enable   enable network access in registry
				// /netstatus.ps3?0        disable network access in registry
				// /netstatus.ps3?disable  disable network access in registry

				if( param[15] & 1) xsetting_F48C0548()->SetSettingNet_enable(1); else //enable
				if(~param[15] & 1) xsetting_F48C0548()->SetSettingNet_enable(0);      //disable

				int32_t status = 0;
				xsetting_F48C0548()->GetSettingNet_enable(&status);

				if(param[14] != '?') {status ^= 1; xsetting_F48C0548()->SetSettingNet_enable(status);}

				sprintf(param, "Network : %s", status ? STR_ENABLED : STR_DISABLED);

				#ifdef WM_REQUEST
				if(!wm_request)
				#endif
				{
					if(!mc) http_response(conn_s, header, "/netstatus.ps3", CODE_HTTP_OK, param);
				}

				show_msg(param);

				goto exit_handleclient;
			}
			if(islike(param, "/edit.ps3"))
			{
				// /edit.ps3<file>              open text file (up to 2000 bytes)
				// /edit.ps3?f=<file>&t=<txt>   saves text to file

				is_popup = 1;
				goto html_response;
			}
	#ifdef COPY_PS3
			if(islike(param, "/copy")) {if(!copy_in_progress) dont_copy_same_size = (param[5] == '.'); param[5] = '.';}

			if(islike(param, "/rmdir.ps3"))
			{
				// /rmdir.ps3        deletes history files & remove empty ISO folders
				// /rmdir.ps3<path>  removes empty folder

				if(param[10] == '/')
				{
					sprintf(param, "%s", param + 10); cellFsRmdir(param);
					char *p = strrchr(param, '/'); *p = NULL;
				}
				else
					{delete_history(true); sprintf(param, "/dev_hdd0");}

				is_binary = FOLDER_LISTING, small_alloc = false;
				goto html_response;
			}
			if(islike(param, "/mkdir.ps3"))
			{
				// /mkdir.ps3        creates ISO folders in hdd0
				// /mkdir.ps3<path>  creates a folder & parent folders

				if(param[10] == '/')
				{
					sprintf(param, "%s", param + 10);

					filepath_check(param);

					mkdir_tree(param);
					cellFsMkdir(param, DMODE);
				}
				else
				{
					sprintf(param, "/dev_hdd0");

					cellFsMkdir("/dev_hdd0/packages", DMODE);
					cellFsMkdir("/dev_hdd0/GAMES",    DMODE);
					cellFsMkdir("/dev_hdd0/PS3ISO",   DMODE);
					cellFsMkdir("/dev_hdd0/PSXISO",   DMODE);
					cellFsMkdir("/dev_hdd0/PS2ISO",   DMODE);
					cellFsMkdir("/dev_hdd0/PSPISO",   DMODE);
					cellFsMkdir("/dev_hdd0/DVDISO",   DMODE);
					cellFsMkdir("/dev_hdd0/BDISO",    DMODE);

					// Let the user create these folders manually or enable webman_config->autoplay
					//cellFsMkdir("/dev_hdd0/GAMES"  AUTOPLAY_TAG, DMODE);
					//cellFsMkdir("/dev_hdd0/PS3ISO" AUTOPLAY_TAG, DMODE);
					//cellFsMkdir("/dev_hdd0/PSXISO" AUTOPLAY_TAG, DMODE);
					//cellFsMkdir("/dev_hdd0/PS2ISO" AUTOPLAY_TAG, DMODE);
				}

				is_binary = FOLDER_LISTING, small_alloc = false;
				goto html_response;
			}
			else
			if(islike(param, "/rename.ps3") || islike(param, "/swap.ps3"))
			{
				// /rename.ps3<path>|<target>     rename <path> to <target>
				// /rename.ps3<path>&to=<target>  rename <path> to <target>
				// /rename.ps3<path>.bak          removes .bak extension
				// /rename.ps3<path>.bak          removes .bak extension
				// /rename.ps3<path>|<target>?restart.ps3
				// /rename.ps3<path>&to=<target>?restart.ps3
				// /swap.ps3<file1>|<file2>       swap <file1> & <file2>
				// /swap.ps3<file1>&to=<file2>    swap <file1> & <file2>
				// /swap.ps3<file1>|<file2>?restart.ps3
				// /swap.ps3<file1>&to=<file2>?restart.ps3

				size_t cmd_len = islike(param, "/swap.ps3") ? 9 : 11;

				char *source = param + cmd_len, *target = strstr(source, "|");
				if(target) {*target = NULL; target++;} else {target = strstr(source, "&to="); if(target) {target = NULL; target+=4;}}

				if((*target != '/') && !extcmp(source, ".bak", 4)) {size_t flen = strlen(source); *target = *param + flen; strncpy(target, source, flen - 4);}

				if(*target == '/')
				{
					filepath_check(target);

					if((cmd_len == 9) && file_exists(source) && file_exists(target))
					{
						sprintf(header, "%s.bak", source);
						cellFsRename(source, header);
						cellFsRename(target, source);
						cellFsRename(header, target);
					}
					else
						cellFsRename(source, target);

					char *p = strrchr(target, '/'); *p = NULL;
					sprintf(param, "%s", target);
					if(do_restart) goto reboot;
				}
			}
			else
			if(islike(param, "/cpy.ps3") || islike(param, "/cut.ps3"))
			{
				// /cpy.ps3<path>  stores <path> in <cp_path> clipboard buffer for copy with /paste.ps3 (cp_mode = 1)
				// /cut.ps3<path>  stores <path> in <cp_path> clipboard buffer for move with /paste.ps3 (cp_mode = 2)

				cp_mode = islike(param, "/cut.ps3") ? CP_MODE_MOVE : CP_MODE_COPY;
				sprintf(cp_path, "%s", param + 8);
				sprintf(param, "%s", cp_path);
				char *p = strrchr(param, '/'); *p = NULL;
				if(file_exists(cp_path) == false) cp_mode = CP_MODE_NONE;

				is_binary = FOLDER_LISTING, small_alloc = false;
				goto html_response;
			}
			else
			if(islike(param, "/paste.ps3"))
			{
				// /paste.ps3<path>  performs a copy or move of path stored in <cp_path clipboard> to <path> indicated in url

				char *source = header, *target = cp_path;
				if(file_exists(cp_path))
				{
					sprintf(source, "/copy.ps3%s", cp_path);
					sprintf(target, "%s", param + 10);
					sprintf(param, "%s", source); strcat(target, strrchr(param, '/'));
					is_binary = WEB_COMMAND, small_alloc = false;
					goto html_response;
				}
				else
					if(!mc) {http_response(conn_s, header, "/", CODE_GOBACK, HTML_REDIRECT_TO_BACK); goto exit_handleclient;}
			}
	#endif // #ifdef COPY_PS3

 #endif //#ifndef LITE_EDITION

			if(islike(param, "/quit.ps3"))
			{
				// quit.ps3        Stops webMAN and sets fan to fixed speed specified in PS2 mode
				// quit.ps3?0      Stops webMAN and sets fan to syscon mode

				http_response(conn_s, header, param, CODE_HTTP_OK, param);
 #ifdef LOAD_PRX
 quit:
 #endif
				if(!webman_config->fanc || (webman_config->ps2temp < 33) || strstr(param, "?0"))
					restore_fan(0); //restore syscon fan control mode
				else
					restore_fan(1); //set ps2 fan control mode

				working = plugin_active = 0;

				#ifdef AUTO_POWER_OFF
				setAutoPowerOff(false);
				#endif

				sclose(&conn_s);
				if(sysmem) sys_memory_free(sysmem);

				stop_prx_module();
				sys_ppu_thread_exit(0);
				break;
			}

			if(islike(param, "/shutdown.ps3"))
			{
				// /shutdown.ps3        Shutsdown
				// /shutdown.ps3?vsh    Shutsdown using VSH

				#ifndef EMBED_JS
				css_exists = common_js_exists = false;
				#endif

				http_response(conn_s, header, param, CODE_HTTP_OK, param);
				setPluginExit();

				{ DELETE_TURNOFF } { BEEP1 }

				if(param[13] == '?')
					vsh_shutdown(); // shutdown using VSH
				else
					{system_call_4(SC_SYS_POWER, SYS_SHUTDOWN, 0, 0, 0);}

				goto exit_handleclient;
			}
			if(islike(param, "/rebuild.ps3"))
			{
				// /rebuild.ps3  reboots & start rebuilding file system

				cmd[0] = cmd[1] = 0; cmd[2] = 0x03; cmd[3] = 0xE9; // 00 00 03 E9
				save_file("/dev_hdd0/mms/db.err", cmd, 4);
				goto reboot; // hard reboot
			}
			if(islike(param, "/recovery.ps3"))
			{
				// /recovery.ps3  reboots in pseudo-recovery mode

				#define SC_UPDATE_MANAGER_IF				863
				#define UPDATE_MGR_PACKET_ID_READ_EPROM		0x600B
				#define UPDATE_MGR_PACKET_ID_WRITE_EPROM	0x600C
				#define RECOVER_MODE_FLAG_OFFSET			0x48C61

				{system_call_7(SC_UPDATE_MANAGER_IF, UPDATE_MGR_PACKET_ID_WRITE_EPROM, RECOVER_MODE_FLAG_OFFSET, 0x00, 0, 0, 0, 0);} // set recovery mode
				goto reboot; // hard reboot
			}
			if(islike(param, "/restart.ps3") || islike(param, "/reboot.ps3"))
			{
				// /reboot.ps3           Hard reboot
				// /restart.ps3          Reboot using default mode (VSH reboot is the default); skip content scan on reboot
				// /restart.ps3?0        Allow scan content on restart
				// /restart.ps3?quick    Quick reboot (load LPAR id 1)
				// /restart.ps3?vsh      VSH Reboot
				// /restart.ps3?soft     Soft reboot
				// /restart.ps3?hard     Hard reboot
				// /restart.ps3?<mode>$  Sets the default restart mode for /restart.ps3
				// /restart.ps3?min      Reboot & show min version
 reboot:
				#ifndef EMBED_JS
				css_exists = common_js_exists = false;
				#endif

				http_response(conn_s, header, param, CODE_HTTP_OK, param);
				setPluginExit();

				{ DELETE_TURNOFF } { BEEP2 }

				char *allow_scan = strstr(param,"?0");
				if(allow_scan) *allow_scan = NULL; else save_file(WMNOSCAN, NULL, 0);

				bool is_restart = IS(param, "/restart.ps3");

				char mode = 'h', *param2 = strstr(param, "?");
 #ifndef LITE_EDITION
				if(param2) {mode = param2[1] | 0x20; if(strstr(param, "$")) {webman_config->default_restart = mode; save_settings();}} else if(is_restart) mode = webman_config->default_restart;
 #else
				if(param2)  mode = param2[1] | 0x20; else if(is_restart) mode = webman_config->default_restart;
 #endif
				if(mode == 'q')
					{system_call_3(SC_SYS_POWER, SYS_REBOOT, NULL, 0);} // (quick reboot) load LPAR id 1
				else
				if(mode == 's')
					{system_call_3(SC_SYS_POWER, SYS_SOFT_REBOOT, NULL, 0);} // soft reboot
				else
				if(mode == 'h')
					{system_call_3(SC_SYS_POWER, SYS_HARD_REBOOT, NULL, 0);} // hard reboot
				else
				if(mode == 'v' || is_restart)
					vsh_reboot(); // VSH reboot
				else
 #ifndef LITE_EDITION
				if(mode == 'm')
					reboot_show_min_version(""); // show min version
 #endif
				goto exit_handleclient;
			}

 #ifdef CALC_MD5
			if(islike(param, "/md5.ps3"))
			{
				char *filename = param + 8, *buffer = header;

				sprintf(buffer, "File: ");
				add_breadcrumb_trail(buffer, filename);

				struct CellFsStat buf; cellFsStat(filename, &buf);
				unsigned long long sz = (unsigned long long)buf.st_size;

				char md5[33];
				calc_md5(filename, md5);

				sprintf(param, "%s<p>Size: %llu bytes<p>MD5: %s<p>", buffer, sz, md5);

				http_response(conn_s, header, "/md5.ps3", CODE_HTTP_OK, param);

				goto exit_handleclient;
			}
 #endif

 #ifdef FIX_GAME
			if(islike(param, "/fixgame.ps3"))
			{
				// /fixgame.ps3<path>  fix PARAM.SFO and EBOOT.BIN / SELF / SPRX in ISO or folder

				// fix game folder
				char *game_path = param + 12, titleID[10];
				fix_game(game_path, titleID, FIX_GAME_FORCED);

				is_popup = 1;
				goto html_response;
			}
 #endif

 #ifndef LITE_EDITION
			if(islike(param, "/mount.ps3?"))
			{
				// /mount.ps3?<query>  search game & mount if only 1 match is found

				if(!islike(param, "/mount.ps3?http")) {param[1] = 'i', param[2] = 'n', param[3] = 'd', param[4] = 'e', param[5] = 'x', auto_mount = true;}
			}
 #endif

			if(islike(param, "/games.ps3"))
			{
				// /games.ps3
				// /index.ps3?mobile
				// /dev_hdd0/xmlhost/game_plugin/mobile.html

 mobile_response:
				mobile_mode = true; char *param2 = param + 10;

				if(file_exists(MOBILE_HTML) == false)
					{sprintf(param, "/index.ps3%s", param2); mobile_mode = false;}
				else if(strstr(param, "?g="))
					sprintf(param, MOBILE_HTML);
				else if(strstr(param, "?"))
					sprintf(param, "/index.ps3%s", param2);
				else if(file_exists(GAMELIST_JS) == false)
					sprintf(param, "/index.ps3?mobile");
				else
					sprintf(param, MOBILE_HTML);
			}
			else mobile_mode = false;

			if(!is_busy && (islike(param, "/index.ps3?") || islike(param, "/refresh.ps3"))) ; else

			if(!is_busy && sys_admin && (islike(param, "/mount.ps3?http")
 #ifdef DEBUG_MEM
							|| islike(param, "/peek.lv2?")
							|| islike(param, "/poke.lv2?")
							|| islike(param, "/find.lv2?")
							|| islike(param, "/peek.lv1?")
							|| islike(param, "/poke.lv1?")
							|| islike(param, "/find.lv1?")
							|| islike(param, "/dump.ps3")
 #endif

 #ifndef LITE_EDITION
							|| islike(param, "/delete.ps3")
							|| islike(param, "/delete_ps3")
 #endif

 #ifdef PS3MAPI
							|| islike(param, "/home.ps3mapi")
							|| islike(param, "/setmem.ps3mapi")
							|| islike(param, "/getmem.ps3mapi")
							|| islike(param, "/led.ps3mapi")
							|| islike(param, "/buzzer.ps3mapi")
							|| islike(param, "/notify.ps3mapi")
							|| islike(param, "/syscall.ps3mapi")
							|| islike(param, "/syscall8.ps3mapi")
							|| islike(param, "/setidps.ps3mapi")
							|| islike(param, "/vshplugin.ps3mapi")
							|| islike(param, "/gameplugin.ps3mapi")
 #endif

 #ifdef COPY_PS3
							|| islike(param, "/copy.ps3/")
 #endif
			)) ;

			else if(islike(param, "/cpursx.ps3")
				||  islike(param, "/index.ps3")
				||  islike(param, "/mount_ps3/")
				||  islike(param, "/mount.ps3/")
 #ifdef PS2_DISC
				||  islike(param, "/mount.ps2/")
				||  islike(param, "/mount_ps2/")
 #endif

 #ifdef VIDEO_REC
				||  islike(param, "/videorec.ps3")
 #endif

 #ifdef EXT_GDATA
				||  islike(param, "/extgd.ps3")
 #endif

 #ifdef SYS_BGM
				||  islike(param, "/sysbgm.ps3")
 #endif

 #ifdef LOAD_PRX
				||  islike(param, "/loadprx.ps3")
				||  islike(param, "/unloadprx.ps3")
 #endif
				||  islike(param, "/eject.ps3")
				||  islike(param, "/insert.ps3")) ;

			else
			{
				struct CellFsStat buf; bool is_net = false;

				if(islike(param, "/net") && (param[4] >= '0' && param[4] <= '4')) //net0/net1/net2/net3/net4
				{
					is_binary = FOLDER_LISTING, small_alloc = false, is_net = true;
				}
				else
					is_binary = (*param == '/') && (cellFsStat(param, &buf) == CELL_FS_SUCCEEDED);

				if(is_binary == BINARY_FILE) ;

				else if(*param == '/')
				{
					char *sort = strstr(param, "?sort=");
					if(sort) {sort_by = sort[6]; if(strstr(sort, "desc")) sort_order = -1; *sort = NULL;}

					sort = strchr(param, '?');
					if(sort)
					{
						file_query = sort + 1;
						*sort = NULL;
					}

					if(is_net || file_exists(param) == false)
					{
						sort = strrchr(param, '#');
						if(sort) *sort = NULL;
					}

					if(is_net) goto html_response;

					if(islike(param, "/favicon.ico")) {sprintf(param, "%s", wm_icons[iPS3]);} else
					if(file_exists(param) == false && *html_base_path == '/') {strcpy(header, param); sprintf(param, "%s/%s", html_base_path, header);} // use html path (if path is omitted)

					is_binary = (cellFsStat(param, &buf) == CELL_FS_SUCCEEDED);
				}

				if(is_binary)
				{
					c_len = buf.st_size;
					if((buf.st_mode & S_IFDIR) != 0) {is_binary = FOLDER_LISTING, small_alloc = false;} // folder listing
				}
				else
				{
					http_response(conn_s, header, param, is_busy ? CODE_SERVER_BUSY : CODE_BAD_REQUEST, is_busy ? "503 Server is Busy" : "400 Bad Request");

					goto exit_handleclient;
				}
			}

 html_response:
			header_len = prepare_header(header, param, is_binary);

			char templn[1024];
			{u16 ulen = strlen(param); if((ulen > 1) && (param[ulen - 1] == '/')) ulen--, param[ulen] = NULL;}
			//sprintf(templn, "X-PS3-Info: %llu [%s]\r\n", (unsigned long long)c_len, param); strcat(header, templn);

			//-- select content profile
			if(strstr(param, ".ps3?"))
			{
				u8 uprofile = profile; char url[10]; bool is_index_ps3 = islike(param, "/index.ps3?");

				if(is_index_ps3 || islike(param, "/refresh.ps3")) {char mode, *cover_mode = strstr(param, "?cover="); if(cover_mode) {mode = *(cover_mode + 7) | 0x20, *cover_mode = NULL; webman_config->nocov = (mode == 'o') ? ONLINE_COVERS : (mode == 'd' || mode == 'n') ? SHOW_DISC : (mode == 'i') ? SHOW_ICON0 : SHOW_MMCOVERS;}}

				for(u8 i = 0; i < 5; i++)
				{
					sprintf(url, "?%i",    i); if(strstr(param, url)) {profile = i; break;}
					sprintf(url, "usr=%i", i); if(strstr(param, url)) {profile = i; break;}

					if(is_index_ps3) {sprintf(url, "_%i", i); if(strstr(param, url)) {profile = i; break;}}
				}

				if (uprofile != profile) {webman_config->profile = profile; save_settings();}
				if((uprofile != profile) || is_index_ps3) {DELETE_CACHED_GAMES}
			}
			//--

			if(is_binary == BINARY_FILE) // binary file
			{
				header_len += sprintf(header + header_len, "Content-Length: %llu\r\n\r\n", (unsigned long long)c_len);
				send(conn_s, header, header_len, 0);

				size_t buffer_size = 0; if(sysmem) sys_memory_free(sysmem);

				for(uint8_t n = MAX_PAGES; n > 0; n--)
					if(c_len >= ((n-1) * _64KB_) && sys_memory_allocate(n * _64KB_, SYS_MEMORY_PAGE_SIZE_64K, &sysmem) == CELL_OK) {buffer_size = n * _64KB_; break;}

				//if(!sysmem && sys_memory_allocate(_64KB_, SYS_MEMORY_PAGE_SIZE_64K, &sysmem)!=0)
				if(buffer_size < _64KB_)
				{
					http_response(conn_s, header, param, CODE_SERVER_BUSY, STR_ERROR);
					goto exit_handleclient;
				}

				if(islike(param, "/dev_bdvd"))
					{system_call_1(36, (uint64_t) "/dev_bdvd");} // decrypt dev_bdvd files

				char *buffer = (char*)sysmem;
				if(cellFsOpen(param, CELL_FS_O_RDONLY, &fd, NULL, 0) == CELL_FS_SUCCEEDED)
				{
					u64 read_e = 0, pos;
					cellFsLseek(fd, 0, CELL_FS_SEEK_SET, &pos);

					while(working)
					{
						//sys_timer_usleep(500);
						if(cellFsRead(fd, (void *)buffer, buffer_size, &read_e) == CELL_FS_SUCCEEDED)
						{
							if(read_e > 0)
							{
								if(send(conn_s, buffer, (size_t)read_e, 0) < 0) break;
							}
							else
								break;
						}
						else
							break;
					}
					cellFsClose(fd);
				}

				goto exit_handleclient;
			}

			u32 BUFFER_SIZE_HTML = _64KB_;

			if(islike(param, "/cpursx.ps3") || show_info_popup)
			{
				if(!sysmem && sys_memory_allocate(_64KB_, SYS_MEMORY_PAGE_SIZE_64K, &sysmem) != CELL_OK)
				{
					http_response(conn_s, header, param, CODE_SERVER_BUSY, STR_ERROR);
					goto exit_handleclient;
				}

				is_cpursx = 1;
			}
			else
			{
				if(!small_alloc || islike(param, "/index.ps3"))
				{
					BUFFER_SIZE_HTML = get_buffer_size(webman_config->foot);

					_meminfo meminfo;
					{system_call_1(SC_GET_FREE_MEM, (uint64_t)(u32) &meminfo);}

					if((meminfo.avail)<( (BUFFER_SIZE_HTML) + MIN_MEM)) BUFFER_SIZE_HTML = get_buffer_size(3); //MIN+
					if((meminfo.avail)<( (BUFFER_SIZE_HTML) + MIN_MEM)) BUFFER_SIZE_HTML = get_buffer_size(1); //MIN
				}

				while((!sysmem) && (BUFFER_SIZE_HTML > 0) && sys_memory_allocate(BUFFER_SIZE_HTML, SYS_MEMORY_PAGE_SIZE_64K, &sysmem) != CELL_OK) BUFFER_SIZE_HTML -= _64KB_;

				if(!sysmem)
				{
					http_response(conn_s, header, param, CODE_SERVER_BUSY, STR_ERROR);
					goto exit_handleclient;
				}
			}

			char *buffer = (char*)sysmem;

			//else	// text page
			{
				if((is_binary != FOLDER_LISTING) && islike(param, "/setup.ps3?"))
				{
					setup_parse_settings(param + 11);
				}

				bool mount_ps3 = !is_popup && islike(param, "/mount_ps3"), forced_mount = false;

				if(mount_ps3 && IS_INGAME) {mount_ps3 = false; forced_mount = true;}

				char *pbuffer = prepare_html(buffer, templn, param, is_ps3_http, is_cpursx, mount_ps3);

				size_t buf_len = (pbuffer - buffer);

				char *tempstr = buffer + BUFFER_SIZE_HTML - _4KB_;

				if(is_cpursx)
				{
					cpu_rsx_stats(pbuffer, templn, param, is_ps3_http);

					is_cpursx = 0; goto send_response;

					//CellGcmConfig config; cellGcmGetConfiguration(&config);
					//sprintf(templn, "localAddr: %x", (u32) config.localAddress); strcat(pbuffer, templn);
				}
				else if(!mount_ps3)
				{
					{
						char cpursx[32]; get_cpursx(cpursx);

						sprintf(templn, " [<a href=\"%s\">"
										// prevents flickering but cause error 80710336 in ps3 browser (silk mode)
										//"<span id=\"lbl_cpursx\">%s</span></a>]<iframe src=\"/cpursx_ps3\" style=\"display:none;\"></iframe>"
										"<span id=\"err\" style=\"display:none\">%s&nbsp;</span>%s</a>]"
										"<script>function no_error(o){try{var doc=o.contentDocument||o.contentWindow.document;}catch(e){err.style.display='inline-block';o.style.display='none';}}</script>"
										//
										"<hr width=\"100%%\">"
										"<div id=\"rxml\"><H1>%s XML ...</H1></div>"
										"<div id=\"rhtm\"><H1>%s HTML ...</H1></div>"
 #ifdef COPY_PS3
										"<div id=\"rcpy\"><H1><a href=\"/copy.ps3$abort\">&#9746;</a> %s ...</H1></div>"
										//"<form action=\"\">", cpursx, STR_REFRESH, STR_REFRESH, STR_COPYING); strcat(pbuffer, templn);
										"<form action=\"\">", "/cpursx.ps3", cpursx, is_ps3_http ? cpursx : "<iframe src=\"/cpursx_ps3\" style=\"border:0;overflow:hidden;\" width=\"230\" height=\"23\" frameborder=\"0\" scrolling=\"no\" onload=\"no_error(this)\"></iframe>", STR_REFRESH, STR_REFRESH, STR_COPYING);
 #else
										//"<form action=\"\">", cpursx, STR_REFRESH, STR_REFRESH); strcat(pbuffer, templn);
										"<form action=\"\">", "/cpursx.ps3", cpursx, is_ps3_http ? cpursx : "<iframe src=\"/cpursx_ps3\" style=\"border:0;overflow:hidden;\" width=\"230\" height=\"23\" frameborder=\"0\" scrolling=\"no\" onload=\"no_error(this)\"></iframe>", STR_REFRESH, STR_REFRESH);
 #endif
						pbuffer += concat(pbuffer, templn);
					}

					if((webman_config->homeb) && (strlen(webman_config->home_url)>0))
					{sprintf(templn, HTML_BUTTON_FMT, HTML_BUTTON, STR_HOME, HTML_ONCLICK, webman_config->home_url); strcat(pbuffer, templn);}

					sprintf(templn, HTML_BUTTON_FMT
									HTML_BUTTON_FMT
									HTML_BUTTON_FMT
 #ifdef EXT_GDATA
									HTML_BUTTON_FMT
 #endif
									, HTML_BUTTON, STR_EJECT, HTML_ONCLICK, "/eject.ps3"
									, HTML_BUTTON, STR_INSERT, HTML_ONCLICK, "/insert.ps3"
									, HTML_BUTTON, STR_UNMOUNT, HTML_ONCLICK, "/mount.ps3/unmount"
 #ifdef EXT_GDATA
									, HTML_BUTTON, "gameDATA", HTML_ONCLICK, "/extgd.ps3"
 #endif
					);

					pbuffer += concat(pbuffer, templn);

 #ifdef COPY_PS3
					if(((islike(param, "/dev_") && strlen(param) > 12 && !strstr(param,"?")) || islike(param, "/dev_bdvd")) && !strstr(param,".ps3/") && !strstr(param,".ps3?"))
					{
						if(copy_in_progress)
							sprintf(templn, "%s&#9746; %s\" %s'%s';\">", HTML_BUTTON, STR_COPY, HTML_ONCLICK, "/copy.ps3$abort");
						else
							sprintf(templn, "%s%s\" onclick='rcpy.style.display=\"block\";location.href=\"/copy.ps3%s\";'\">", HTML_BUTTON, STR_COPY, param);

						pbuffer += concat(pbuffer, templn);
					}

					if((islike(param, "/dev_") && !strstr(param,"?")) && !islike(param,"/dev_flash") && !strstr(param,".ps3/") && !strstr(param,".ps3?"))
					{	// add buttons + javascript code to handle delete / cut / copy / paste (requires fm.js)
	#ifdef EMBED_JS
						sprintf(templn, "<script>"
										"function tg(b,m,x,c){"
										"var i,p,o,h,l=document.querySelectorAll('.d,.w'),s=m.length,n=1;"
										"for(i=1;i<l.length;i++){o=l[i];"
										"h=o.href;p=h.indexOf('/cpy.ps3');if(p>0){n=0;s=8;bCpy.value='Copy';}"
										"if(p<1){p=h.indexOf('/cut.ps3');if(p>0){n=0;s=8;bCut.value='Cut';}}"
										"if(p<1){p=h.indexOf('/delete.ps3');if(p>0){n=0;s=11;bDel.value='%s';}}"
										"if(p>0){o.href=h.substring(p+s,h.length);o.style.color='#ccc';}"
										"else{p=h.indexOf('/',8);o.href=m+h.substring(p,h.length);o.style.color=c;}"
										"}if(n)b.value=(b.value == x)?x+' %s':x;"
										"}</script>", STR_DELETE, STR_ENABLED); pbuffer += concat(pbuffer, templn);
	#else
						if(file_exists(FM_SCRIPT_JS))
	#endif
						{
							sprintf(templn, "%s%s\" id=\"bDel\" onclick=\"tg(this,'%s','%s','red');\">", HTML_BUTTON, STR_DELETE, "/delete.ps3", STR_DELETE); pbuffer += concat(pbuffer, templn);
							sprintf(templn, "%s%s\" id=\"bCut\" onclick=\"tg(this,'%s','%s','magenta');\">", HTML_BUTTON, "Cut", "/cut.ps3", "Cut"); pbuffer += concat(pbuffer, templn);
							sprintf(templn, "%s%s\" id=\"bCpy\" onclick=\"tg(this,'%s','%s','blue');\">", HTML_BUTTON, "Copy", "/cpy.ps3", "Copy"); pbuffer += concat(pbuffer, templn);

							if(cp_mode) {char *url = tempstr, *title = tempstr + MAX_PATH_LEN; urlenc(url, param); htmlenc(title, cp_path, 0); sprintf(templn, "%s%s\" id=\"bPst\" %s'/paste.ps3%s'\" title=\"%s\">", HTML_BUTTON, "Paste", HTML_ONCLICK, url, title); pbuffer += concat(pbuffer, templn);}
						}
					}

 #endif // #ifdef COPY_PS3

					sprintf(templn,  "%s%s XML%s\" %s'%s';\"> "
									 "%s%s HTML%s\" %s'%s';\">",
									 HTML_BUTTON, STR_REFRESH, SUFIX2(profile), HTML_ONCLICK, "/refresh.ps3';rxml.style.display='block",
									 HTML_BUTTON, STR_REFRESH, SUFIX2(profile), HTML_ONCLICK, "/index.ps3?html';rhtm.style.display='block");

					pbuffer += concat(pbuffer, templn);

 #ifdef SYS_ADMIN_MODE
					if(sys_admin)
 #endif
					{
						sprintf(templn,  HTML_BUTTON_FMT
										 HTML_BUTTON_FMT,
										 HTML_BUTTON, STR_SHUTDOWN, HTML_ONCLICK, "/shutdown.ps3",
										 HTML_BUTTON, STR_RESTART,  HTML_ONCLICK, "/restart.ps3");

						pbuffer += concat(pbuffer, templn);
					}

 #ifndef LITE_EDITION
					char *nobypass = strstr(param, "$nobypass");
					if(!nobypass) { PS3MAPI_REENABLE_SYSCALL8 } else *nobypass = NULL;

					// game list resizer
					if(!is_ps3_http && islike(param, "/index.ps3"))
						sprintf( templn, "<script>function rz(z){document.cookie=z + '; expires=Tue, 19 Jan 2038 03:14:07 UTC;';var i,el=document.getElementsByClassName('gc');for(i=0;i<el.length;++i)el[i].style.zoom=z/100;}</script>"
										 "&nbsp;<input id=\"sz\" type=\"range\" value=\"100\" min=\"20\" max=\"200\" style=\"width:80px;position:relative;top:7px;\" ondblclick=\"this.value=100;rz(100);\" onchange=\"rz(this.value);\">"
										 "<script>var d=document,z=d.cookie.split(';');css=d.styleSheets[0];css.insertRule('.gc{zoom:'+z+'%%}',css.cssRules.length);d.getElementById('sz').value=z;</script>"
/*
										 // select icon type
										 "&nbsp;"
										 "<select name=\"cov\" onchange=\"wmsg.style.display='block';window.location='/index.ps3?cover='+cov.value;\" accesskey=\"C\" style=\"font-size:12px;\">"
										 "<option value=m>MM COVERS</option>"
										 "<option value=i>ICON0.PNG</option>"
										 "<option value=n>DISC ICONS</option>"
#ifndef ENGLISH_ONLY
										 "<option value=o>ONLINE COVERS</option>"
#endif
										 "</select>"
*/
										 "</form><hr>");
					else
 #endif
						sprintf( templn, "</form><hr>");

					pbuffer += concat(pbuffer, templn);
 #ifdef COPY_PS3
					if(copy_in_progress)
					{
						sprintf(templn, "%s%s %s (%i %s)", "<div id=\"cps\"><font size=2>", STR_COPYING, current_file, copied_count, STR_FILES);
					}
					else if(fix_in_progress)
					{
						sprintf(templn, "%s%s %s (%i %s)", "<div id=\"cps\"><font size=2>", STR_FIXING, current_file, fixed_count, STR_FILES);
					}
					if((copy_in_progress || fix_in_progress) && file_exists(current_file))
					{
						strcat(templn, "</font><p></div><script>setTimeout(function(){cps.style.display='none'},15000);</script>"); pbuffer += concat(pbuffer, templn);
					}
 #endif
				}

 #ifndef LITE_EDITION
				if(is_popup)
				{
					if(islike(param, "/edit.ps3"))
					{
						// /edit.ps3<file>              open text file (up to 2000 bytes)
						// /edit.ps3?f=<file>&t=<txt>   saves text to file

						char *filename = templn, *txt = buffer + BUFFER_SIZE_HTML - _6KB_, *backup = txt; memset(txt, 0, _2KB_);

						// get file name
						get_value(filename, param + ((param[9] == '/') ? 9 : 12), MAX_PATH_LEN); // /edit.ps3<file>  *or* /edit.ps3?f=<file>&t=<txt>

						filepath_check(filename);

						char *pos = strstr(param, "&t=");
						if(pos)
						{
							// backup the original text file
							sprintf(backup, "%s.bak", filename);
							cellFsUnlink(backup); // delete previous backup
							cellFsRename(filename, backup);

							// save text file
							sprintf(txt, "%s", pos + 3);
							save_file(filename, txt, SAVE_ALL);
						}
						else
						{
							// load text file
							read_file(filename, txt, MAX_TEXT_LEN, 0);
						}

						// show text box
						sprintf(tempstr,"<form action=\"/edit.ps3\">"
										"<input type=hidden name=\"f\" value=\"%s\">"
										"<textarea name=\"t\" maxlength=%i style=\"width:800px;height:400px;\">%s</textarea><br>"
										"<input type=submit value=\" %s \">",
										filename, MAX_TEXT_LEN, txt, STR_SAVE); strcat(pbuffer, tempstr);

						// show filename link
						char *p = strrchr(filename, '/');
						if(p) {strcpy(txt, p); *p = NULL; sprintf(tempstr," &nbsp; " HTML_URL HTML_URL2 "</form>", filename, filename, filename, txt, txt); strcat(pbuffer, tempstr);}

						is_popup = 0; goto send_response;
					}

  #ifdef WEB_CHAT
					if(islike(param, "/chat.ps3"))
					{
						// /chat.ps3    webchat

						webchat(buffer, templn, param, tempstr, conn_info_main);
					}
					else
  #endif

  #ifdef FIX_GAME
					if(islike(param, "/fixgame.ps3"))
					{
						// /fixgame.ps3<path>  fix PARAM.SFO and EBOOT.BIN / SELF / SPRX in ISO or folder

						char *game_path = param + 12;
						sprintf(templn, "Fixed: %s", game_path);
						show_msg(templn);

						urlenc(templn, game_path);
						sprintf(tempstr, "Fixed: " HTML_URL, templn, game_path); strcat(pbuffer, tempstr);

						sprintf(tempstr, HTML_REDIRECT_TO_URL, templn, HTML_REDIRECT_WAIT); strcat(pbuffer, tempstr);
					}
					else
  #endif
					{
						char *msg = (param + 11); // /popup.ps3?<msg>
						show_msg(msg);
						sprintf(templn, "Message sent: %s", msg); strcat(pbuffer, templn);
					}

					is_popup = 0; goto send_response;
				}
 #endif // #ifndef LITE_EDITION

				////////////////////////////////////

				if(is_binary == FOLDER_LISTING) // folder listing
				{
					if(folder_listing(buffer, BUFFER_SIZE_HTML, templn, param, conn_s, tempstr, header, is_ps3_http, sort_by, sort_order, file_query) == false)
					{
						goto exit_handleclient;
					}
				}
				else
				{
					{ PS3MAPI_ENABLE_ACCESS_SYSCALL8 }
 #ifndef LITE_EDITION
					if(!strstr(param, "$nobypass")) { PS3MAPI_REENABLE_SYSCALL8 }
 #endif
					is_busy = true;

					if(islike(param, "/refresh.ps3") && refreshing_xml == 0)
					{
						// /refresh.ps3               refresh XML
						// /refresh.ps3?cover=<mode>  refresh XML using cover type (icon0, mm, disc, online)

						refresh_xml(templn);
 #ifndef ENGLISH_ONLY
						char STR_XMLRF[280];

						sprintf(STR_XMLRF, "Game list refreshed (<a href=\"%s\">mygames.xml</a>).%s", MY_GAMES_XML, "<br>Click <a href=\"/restart.ps3\">here</a> to restart your PLAYSTATION®3 system.");

						language("STR_XMLRF", STR_XMLRF, STR_XMLRF);
						language("/CLOSEFILE", NULL, NULL);
 #endif
						sprintf(templn,  "<br>%s", STR_XMLRF); strcat(pbuffer, templn);
					}
					else
					if(islike(param, "/setup.ps3?"))
					{
						// /setup.ps3?          reset webman settings
						// /setup.ps3?<params>  save settings

						if(strstr(param, "&") == NULL)
						{
							cellFsUnlink(WMCONFIG);
							reset_settings();
						}
						if(save_settings() == CELL_FS_SUCCEEDED)
						{
							sprintf(templn, "<br> %s", STR_SETTINGSUPD); strcat(pbuffer, templn);
						}
						else
							strcat(pbuffer, STR_ERROR);
					}
					else
					if(islike(param, "/setup.ps3"))
					{
						// /setup.ps3    setup form with webman settings

						setup_form(pbuffer, templn);
					}
					else
					if(islike(param, "/eject.ps3"))
					{
						// /eject.ps3   eject physical disc from bd drive

						eject_insert(1, 0);
						strcat(pbuffer, STR_EJECTED);
					}
					else
					if(islike(param, "/insert.ps3"))
					{
						// /insert.ps3   insert physical disc into bd drive

						eject_insert(0, 1);
						strcat(pbuffer, STR_LOADED);
					}
 #ifdef LOAD_PRX
					else
					if(islike(param, "/loadprx.ps3") || islike(param, "/unloadprx.ps3"))
					{
						// /loadprx.ps3<path-sprx>
						// /loadprx.ps3?prx=<path-sprx>
						// /loadprx.ps3?prx=<path-sprx>&slot=<slot>
						// /unloadprx.ps3?prx=<path-sprx>
						// /unloadprx.ps3?slot=<slot>

						char *pos; unsigned int slot = 7; bool prx_found;

						if(param[12] == '/') sprintf(templn, "%s", param + 12); else
						if(param[14] == '/') sprintf(templn, "%s", param + 14); else
						{
															 sprintf(templn, "%s/%s", "/dev_hdd0/plugins", "webftp_server.sprx");
							if(file_exists(templn) == false) sprintf(templn, "%s/%s", "/dev_hdd0/plugins", "webftp_server_ps3mapi.sprx");
							if(file_exists(templn) == false) sprintf(templn, "%s/%s", "/dev_hdd0", "webftp_server.sprx");
							if(file_exists(templn) == false) sprintf(templn, "%s/%s", "/dev_hdd0", "webftp_server_ps3mapi.sprx");

							pos = strstr(param, "prx=");
							if(pos) get_value(templn, pos + 4, MAX_PATH_LEN);
						}

						prx_found = file_exists(templn);
  #ifdef COBRA_ONLY
						if(*templn)
						{
							slot = get_vsh_plugin_slot_by_name(templn, false);
							if(islike(param, "/unloadprx.ps3")) prx_found = false;
						}
  #endif
						if((slot < 1) || (slot > 6))
						{
  #ifdef COBRA_ONLY
							slot = get_valuen(param, "slot=", 0, 6);
							if(!slot) slot = get_vsh_plugin_slot_by_name(PS3MAPI_FIND_FREE_SLOT, false); // find first free slot if slot == 0
  #else
							slot = get_valuen(param, "slot=", 1, 6);
  #endif
						}

						if(prx_found)
							sprintf(param, "slot: %i<br>load prx: %s%s", slot, templn, HTML_BODY_END);
						else
							sprintf(param, "unload slot: %i%s", slot, HTML_BODY_END);

						strcat(pbuffer, param);

						if(slot < 7)
						{
							cobra_unload_vsh_plugin(slot);

							if(prx_found)
								{cobra_load_vsh_plugin(slot, templn, NULL, 0); if(strstr(templn, "/webftp_server")) goto quit;}
						}
					}
 #endif

 #ifdef VIDEO_REC
					else
					if(islike(param, "/videorec.ps3"))
					{
						// /videorec.ps3
						// /videorec.ps3<path>
						// /videorec.ps3?<video-rec-params> {mp4, jpeg, psp, hd, avc, aac, pcm, 64, 128, 384, 512, 768, 1024, 1536, 2048}
						// /videorec.ps3?<path>&video=<format>&audio=<format>

						toggle_video_rec(param + 13);
						strcat(pbuffer,	"<a class=\"f\" href=\"/dev_hdd0\">/dev_hdd0/</a><a href=\"/dev_hdd0/VIDEO\">VIDEO</a>:<p>"
										"Video recording: <a href=\"/videorec.ps3\">");
						strcat(pbuffer, recording ? STR_ENABLED : STR_DISABLED);
						strcat(pbuffer, "</a><p>");
						if(!recording) {sprintf(param, "<a class=\"f\" href=\"%s\">%s</a><br>", (char*)recOpt[0x6], (char*)recOpt[0x6]); strcat(pbuffer, param);}
					}
 #endif

 #ifdef EXT_GDATA
					else
					if(islike(param, "/extgd.ps3"))
					{
						// /extgd.ps3          toggle external GAME DATA
						// /extgd.ps3?         show status of external GAME DATA
						// /extgd.ps3?1        enable external GAME DATA
						// /extgd.ps3?enable   enable external GAME DATA
						// /extgd.ps3?0        disable external GAME DATA
						// /extgd.ps3?disable  disable external GAME DATA

						if( param[10] != '?') extgd ^= 1; else //toggle
						if( param[11] & 1)    extgd  = 1; else //enable
						if(~param[11] & 1)    extgd  = 0;      //disable

						strcat(pbuffer, "External Game DATA: ");

						if(set_gamedata_status(extgd, true))
							strcat(pbuffer, STR_ERROR);
						else
							strcat(pbuffer, extgd ? STR_ENABLED : STR_DISABLED);
					}
 #endif

 #ifdef SYS_BGM
					else
					if(islike(param, "/sysbgm.ps3"))
					{
						// /sysbgm.ps3          toggle in-game background music flag
						// /sysbgm.ps3?         show status of in-game background music flag
						// /sysbgm.ps3?1        enable in-game background music flag
						// /sysbgm.ps3?enable   enable in-game background music flag
						// /sysbgm.ps3?0        disable in-game background music flag
						// /sysbgm.ps3?disable  disable in-game background music flag

						if(param[11] == '?')
						{
							if( param[12] & 1) system_bgm = 0; else //enable
							if(~param[12] & 1) system_bgm = 1;      //disable
						}

						if(param[12] != 's')
						{
							int * arg2;
							if(system_bgm)  {BgmPlaybackDisable(0, &arg2); system_bgm = 0;} else
											{BgmPlaybackEnable(0, &arg2);  system_bgm = 1;}
						}

						sprintf(templn, "System BGM: %s", system_bgm ? STR_ENABLED : STR_DISABLED);
						strcat(pbuffer, templn);
						show_msg(templn);
					}
 #endif

 #ifndef LITE_EDITION
					else
					if(islike(param, "/delete.ps3") || islike(param, "/delete_ps3"))
					{
						// /delete_ps3<path>      deletes <path>
						// /delete.ps3<path>      deletes <path> and recursively delete subfolders
						// /delete_ps3<path>?restart.ps3
						// /delete.ps3<path>?restart.ps3

						// /delete.ps3?wmreset    deletes wmconfig & clear /dev_hdd0/tmp/wmtmp
						// /delete.ps3?wmconfig   deletes wmconfig
						// /delete.ps3?wmtmp      clear /dev_hdd0/tmp/wmtmp
						// /delete.ps3?history    deletes history files & remove empty ISO folders
						// /delete.ps3?uninstall  uninstall webMAN MOD & delete files installed by updater

						bool is_reset = false; char *param2 = param + 11; int ret = 0;
						if(strstr(param, "?wmreset")) is_reset=true;
						if(is_reset || strstr(param, "?wmconfig")) {cellFsUnlink(WMCONFIG); reset_settings(); sprintf(param, "/delete_ps3%s", WMCONFIG);}
						if(is_reset || strstr(param, "?wmtmp")) sprintf(param, "/delete_ps3%s", WMTMP);

						bool is_dir = isDir(param2);

						if(islike(param2 , "?history"))
						{
							delete_history(true);
							sprintf(tempstr, "%s : history", STR_DELETE);
							sprintf(param, "/");
						}
						else if(islike(param2 , "?uninstall"))
						{
							struct CellFsStat buf;
							if(cellFsStat("/dev_hdd0/boot_plugins.txt", &buf) == CELL_FS_SUCCEEDED && buf.st_size < 40) cellFsUnlink("/dev_hdd0/boot_plugins.txt");

							// delete files
							unlink_file("/dev_hdd0", "webftp_server.sprx", "");
							unlink_file("/dev_hdd0", "webftp_server_ps3mapi.sprx", "");
							unlink_file("/dev_hdd0", "webftp_server_noncobra.sprx", "");
							unlink_file("/dev_hdd0", "raw_iso.sprx", "");

							unlink_file("/dev_hdd0", "plugins/", "webftp_server.sprx");
							unlink_file("/dev_hdd0", "plugins/", "webftp_server_ps3mapi.sprx");
							unlink_file("/dev_hdd0", "plugins/", "webftp_server_noncobra.sprx");
							unlink_file("/dev_hdd0", "plugins/", "raw_iso.sprx");
							unlink_file("/dev_hdd0", "plugins/", "wm_vsh_menu.sprx");

							unlink_file("/dev_hdd0", "tmp/", "wm_vsh_menu.cfg");
							unlink_file("/dev_hdd0", "tmp/", "wm_custom_combo");

							cellFsUnlink(WMCONFIG);

							// delete folders & subfolders
							del(WMTMP, RECURSIVE_DELETE);
							del(WM_LANG_PATH, RECURSIVE_DELETE);
							del(WM_ICONS_PATH, RECURSIVE_DELETE);
							del(WM_COMBO_PATH, RECURSIVE_DELETE);
							del(HTML_BASE_PATH, RECURSIVE_DELETE);
							del(VSH_MENU_IMAGES, RECURSIVE_DELETE);
							goto reboot;
						}
						else
						{
							ret = del(param2, islike(param, "/delete.ps3"));

							sprintf(tempstr, "%s %s : ", STR_DELETE, ret ? STR_ERROR : ""); strcat(pbuffer, tempstr);
							add_breadcrumb_trail(pbuffer, param2); sprintf(tempstr, "<br>");
							char *pos = strrchr(param2, '/'); if(pos) *pos = NULL;
						}

						strcat(pbuffer, tempstr);
						sprintf(tempstr, HTML_REDIRECT_TO_URL, param2, (is_dir | ret) ? HTML_REDIRECT_WAIT : 0); strcat(pbuffer, tempstr);

						if(do_restart) goto reboot;
					}
 #endif

 #ifdef PS3MAPI
					else
					if(islike(param, "/home.ps3mapi"))
					{
						ps3mapi_home(pbuffer, templn);
					}
					else
					if(islike(param, "/buzzer.ps3mapi"))
					{
						ps3mapi_buzzer(pbuffer, templn, param);
					}
					else
					if(islike(param, "/led.ps3mapi"))
					{
						ps3mapi_led(pbuffer, templn, param);
					}
					else
					if(islike(param, "/notify.ps3mapi"))
					{
						ps3mapi_notify(pbuffer, templn, param);
					}
					else
					if(islike(param, "/syscall.ps3mapi"))
					{
						ps3mapi_syscall(pbuffer, templn, param);
					}
					else
					if(islike(param, "/syscall8.ps3mapi"))
					{
						ps3mapi_syscall8(pbuffer, templn, param);
					}
					else
					if(islike(param, "/getmem.ps3mapi"))
					{
						ps3mapi_getmem(pbuffer, templn, param);
					}
					else
					if(islike(param, "/setmem.ps3mapi"))
					{
						ps3mapi_setmem(pbuffer, templn, param);
					}
					else
					if(islike(param, "/setidps.ps3mapi"))
					{
						ps3mapi_setidps(pbuffer, templn, param);
					}
					else
					if(islike(param, "/vshplugin.ps3mapi"))
					{
						ps3mapi_vshplugin(pbuffer, templn, param);
					}
					else
					if(islike(param, "/gameplugin.ps3mapi"))
					{
						ps3mapi_gameplugin(pbuffer, templn, param);
					}
 #endif // #ifdef PS3MAPI

 #ifdef DEBUG_MEM
					else
					if(islike(param, "/dump.ps3"))
					{
						// /dump.ps3?lv1
						// /dump.ps3?lv2
						// /dump.ps3?rsx
						// /dump.ps3?mem
						// /dump.ps3?full
						// /dump.ps3?<start-address>
						// /dump.ps3?<start-address>&size=<mb>

						ps3mapi_mem_dump(pbuffer, templn, param);
					}
					else
					if(islike(param, "/peek.lv") || islike(param, "/poke.lv") || islike(param, "/find.lv"))
					{
						// /peek.lv1?<address>
						// /poke.lv1?<address>=<value>
						// /find.lv1?<value>
						// /find.lv1?<start-address>=<value>
						// /peek.lv2?<address>
						// /poke.lv2?<address>=<value>
						// /find.lv2?<value>
						// /find.lv2?<start-address>=<value>

						ps3mapi_find_peek_poke(pbuffer, templn, param);
					}
 #endif
					else
					if(mount_ps3)
					{
						// /mount_ps3/<path>[?random=<x>[&emu={ps1_netemu.self/ps1_netemu.self}][offline={0/1}]

						if(!mc) http_response(conn_s, header, param, CODE_CLOSE_BROWSER, HTML_CLOSE_BROWSER); //auto-close browser (don't wait for mount)

						if(IS_ON_XMB && !(webman_config->combo2 & PLAY_DISC) && (strstr(param, ".ntfs[BD") == NULL) && (strstr(param, "/PSPISO") == NULL))
						{
							sys_timer_sleep(1);
							int view = View_Find("explore_plugin");

							if(view)
							{
								explore_interface = (explore_plugin_interface *)plugin_GetInterface(view, 1);
 #ifdef PKG_LAUNCHER
								if(webman_config->ps3l && strstr(param, "/GAMEI/"))
									explore_interface->ExecXMBcommand("focus_index 0", 0, 0);
								else
 #endif
									explore_interface->ExecXMBcommand("close_all_list", 0, 0);
							}
						}

						game_mount(pbuffer, templn, param, tempstr, mount_ps3, forced_mount);
						goto exit_handleclient;
					}
					else
 #ifdef PS2_DISC
					if(forced_mount || islike(param, "/mount.ps3") || islike(param, "/mount.ps2") || islike(param, "/mount_ps2") || islike(param, "/copy.ps3"))
 #else
					if(forced_mount || islike(param, "/mount.ps3") || islike(param, "/copy.ps3"))
 #endif
					{
						// /mount.ps3/<path>[?random=<x>[&emu={ps1_netemu.self/ps1_netemu.self}][offline={0/1}]
						// /mount.ps3/unmount
						// /mount.ps2/<path>[?random=<x>]
						// /mount.ps2/unmount
						// /copy.ps3/<path>[&to=<destination>]
						// /copy.ps3/<path>[&to=<destination>]?restart.ps3

						game_mount(pbuffer, templn, param, tempstr, mount_ps3, forced_mount);
					}

					else
					{
						// /index.ps3                  show game list in HTML (refresh if cache file is not found)
						// /index.ps3?html             refresh game list in HTML
						// /index.ps3?launchpad        refresh game list in LaunchPad xml
						// /index.ps3?mobile           show game list in slider mode
						// /index.ps3?<query>          search game by device name, path or name of game
						// /index.ps3?<device>?<name>  search game by device name and name
						// /index.ps3?<query>&mobile   search game by device name, path or name of game in slider mode
						// /index.ps3?cover=<mode>     refresh game list in HTML using cover type (icon0, mm, disc, online)

						mobile_mode |= ((strstr(param, "?mob") != NULL) || (strstr(param, "&mob") != NULL));
#ifdef LAUNCHPAD
						char *launchpad = strstr(param, "?launchpad");
						if(launchpad) {*launchpad = NULL; mobile_mode = LAUNCHPAD_MODE, auto_mount = false; sprintf(templn, "%s LaunchPad: %s", STR_REFRESH, STR_SCAN2); show_msg(templn);}
#endif
						if(game_listing(buffer, templn, param, tempstr, mobile_mode, auto_mount) == false)
						{
							{ PS3MAPI_RESTORE_SC8_DISABLE_STATUS }
							{ PS3MAPI_DISABLE_ACCESS_SYSCALL8 }

							http_response(conn_s, header, param, CODE_SERVER_BUSY, STR_ERROR);

							is_busy = false;

							goto exit_handleclient;
						}

						if(auto_mount && islike(buffer, "/mount.ps3")) {auto_mount = false; sprintf(param, "%s", buffer); goto redirect_url;}

						if(is_ps3_http)
						{
							char *pos = strstr(pbuffer, "&#x1F50D;"); // hide search icon
							if(pos) for(u8 c = 0; c < 9; c++) pos[c] = ' ';
						}
					}

					{ PS3MAPI_RESTORE_SC8_DISABLE_STATUS }
					{ PS3MAPI_DISABLE_ACCESS_SYSCALL8 }

					is_busy = false;
#ifdef LAUNCHPAD
					if(mobile_mode == LAUNCHPAD_MODE) {sprintf(templn, "%s LaunchPad: OK", STR_REFRESH); if(!mc) http_response(conn_s, header, param, CODE_HTTP_OK, templn); show_msg(templn); goto exit_handleclient;}
#endif
				}

send_response:
				if(mobile_mode && allow_retry_response) {allow_retry_response = false; goto mobile_response;}

				if(islike(param, "/mount.ps3?http"))
					{http_response(conn_s, header, param, CODE_HTTP_OK, param + 11); break;}
				else
				{
					// add bdvd & go to top links to the footer
					sprintf(templn, "<div style=\"position:fixed;right:20px;bottom:10px;opacity:0.2\">"); strcat(pbuffer, templn);
					if(isDir("/dev_bdvd")) {sprintf(templn, "<a href=\"%s\"><img src=\"%s\" height=\"12\"></a> ", "/dev_bdvd", wm_icons[iPS3]); strcat(pbuffer, templn);}
					strcat(pbuffer, "<a href=\"#Top\">&#9650;</a></div><b>");

	#ifndef EMBED_JS
					// extend web content using custom javascript
					if(common_js_exists)
					{
						sprintf(templn, SCRIPT_SRC_FMT, COMMON_SCRIPT_JS); strcat(pbuffer, templn);
					}
	#endif
					strcat(pbuffer, HTML_BODY_END); //end-html
				}

				if(!mc)
				{
					c_len = buf_len + strlen(buffer + buf_len);

					header_len += sprintf(header + header_len, "Content-Length: %llu\r\n\r\n", (unsigned long long)c_len);
					send(conn_s, header, header_len, 0);

					send(conn_s, buffer, c_len, 0);
				}

				*buffer = NULL;
			}
		}

		break;
	}
  }

exit_handleclient:

	if(sysmem) sys_memory_free(sysmem); sysmem = NULL;
	if(mc) goto parse_request;

	#ifdef USE_DEBUG
	ssend(debug_s, "Request served.\r\n");
	#endif

	sclose(&conn_s);

	if(loading_html) loading_html--;

	sys_ppu_thread_exit(0);
}

/*
	u64 sa=0x03300000ULL;
	u64 offset=0;
	for(u64 i=0;i<0x7700000;i+=8)
	{
		if( peek(0x8000000000000000ULL+i+sa	 ) == 0xFFFFFF00FFFFFF00ULL &&
			peek(0x8000000000000000ULL+i+sa+8) == 0xFFFFFF00FFFFFF00ULL )
		{
			show_msg((char*)"Found match!");
			offset=i+sa+16;
			for(u64 i2=offset;i2<(offset+320*4*90);i2+=8)
				poke(0x8000000000000000ULL+i2, 0x30405060708090A0ULL);
		}
	}
*/

static void wwwd_thread(uint64_t arg)
{
	*backup = NULL;

	detect_firmware();

#ifdef PS3MAPI
 #ifdef REMOVE_SYSCALLS
	backup_cfw_syscalls();
 #endif
#endif

	View_Find = getNIDfunc("paf", 0xF21655F3, 0);
	plugin_GetInterface = getNIDfunc("paf", 0x23AFB290, 0);

#ifdef SYS_BGM
	BgmPlaybackEnable  = getNIDfunc("vshmain", 0xEDAB5E5E, 16*2);
	BgmPlaybackDisable = getNIDfunc("vshmain", 0xEDAB5E5E, 17*2);
#endif

	//pokeq(0x8000000000003560ULL, 0x386000014E800020ULL); // li r3, 0 / blr
	//pokeq(0x8000000000003D90ULL, 0x386000014E800020ULL); // li r3, 0 / blr

	led(YELLOW, BLINK_FAST);

	//WebmanCfg *webman_config = (WebmanCfg*) wmconfig;
	reset_settings();

#ifdef COPY_PS3
	memset(cp_path, 0, MAX_PATH_LEN);
#endif

#ifdef WM_REQUEST
	cellFsUnlink(WMREQUEST_FILE);
#endif

	{from_reboot = file_exists(WMNOSCAN);}

	if(webman_config->blind) enable_dev_blind(NO_MSG);

#ifdef COBRA_ONLY
	{sys_map_path("/app_home", NULL);}
	{sys_map_path("/dev_bdvd/PS3_UPDATE", SYSMAP_PS3_UPDATE);} // redirect firmware update on BD disc to empty folder
#endif

	#ifdef AUTO_POWER_OFF
	restoreAutoPowerOff();
	#endif

	set_buffer_sizes(webman_config->foot);

	if(!webman_config->ftpd)
		sys_ppu_thread_create(&thread_id_ftpd, ftpd_thread, NULL, THREAD_PRIO, THREAD_STACK_SIZE_8KB, SYS_PPU_THREAD_CREATE_NORMAL, THREAD_NAME_FTP); // start ftp daemon immediately

	sys_ppu_thread_t t_id;
	sys_ppu_thread_create(&t_id, handleclient, (u64)START_DAEMON, THREAD_PRIO, THREAD_STACK_SIZE_64KB, (webman_config->ftpd ? SYS_PPU_THREAD_CREATE_NORMAL : SYS_PPU_THREAD_CREATE_JOINABLE), THREAD_NAME_CMD);

#ifdef PS3NET_SERVER
	if(!webman_config->netsrvd)
		sys_ppu_thread_create(&thread_id_netsvr, netsvrd_thread, NULL, THREAD_PRIO, THREAD_STACK_SIZE_8KB, SYS_PPU_THREAD_CREATE_JOINABLE, THREAD_NAME_NETSVR);
#endif

#ifdef PS3MAPI
	///////////// PS3MAPI BEGIN //////////////
	sys_ppu_thread_create(&thread_id_ps3mapi, ps3mapi_thread, NULL, THREAD_PRIO, THREAD_STACK_SIZE_8KB, SYS_PPU_THREAD_CREATE_JOINABLE, THREAD_NAME_PS3MAPI);
	///////////// PS3MAPI END //////////////
#endif

	led(YELLOW, OFF);
	sys_timer_sleep(5);

#ifdef USE_DEBUG
	u8 d_retries = 0;
again_debug:
	debug_s = connect_to_server("192.168.100.209", 38009);
	if(debug_s <  0) {d_retries++; sys_timer_sleep(2); if(d_retries < 10) goto again_debug;}
	if(debug_s >= 0) ssend(debug_s, "Connected...\r\n");
	sprintf(debug, "FC=%i T0=%i T1=%i\r\n", webman_config->fanc, webman_config->temp0, webman_config->temp1);
	ssend(debug_s, debug);
#endif

	max_temp = 0;

	if(webman_config->fanc)
	{
		if(webman_config->temp0 == FAN_AUTO) max_temp = webman_config->temp1;
		fan_control(webman_config->temp0, 0);
	}

	sys_ppu_thread_create(&thread_id_poll, poll_thread, (u64)webman_config->poll, THREAD_PRIO, THREAD_STACK_SIZE_64KB, SYS_PPU_THREAD_CREATE_JOINABLE, THREAD_NAME_POLL);

	led(GREEN, ON);

	int list_s = FAILED;

relisten:
#ifdef USE_DEBUG
	ssend(debug_s, "Listening on port 80...");
#endif

	if(working) list_s = slisten(WWWPORT, 4);
	else goto end;

	if(list_s < 0)
	{
		if(working) {sys_timer_sleep(2); goto relisten;}
		else goto end;
	}

	if((list_s >= 0) && working)
	{
		#ifdef USE_DEBUG
		ssend(debug_s, " OK!\r\n");
		#endif

		u8 timeout = 0;

		while(working)
		{
			sys_timer_usleep(10000); timeout = 0;

			while(working && (loading_html > 2))
			{
				#ifdef USE_DEBUG
				sprintf(debug, "THREADS: %i\r\n", loading_html);
				ssend(debug_s, debug);
				#endif

				sys_timer_usleep(300000);
				if(++timeout > 100) loading_html = 0; // continue after 30 seconds
			}

			int conn_s;
			if(!working) goto end;

			if((conn_s = accept(list_s, NULL, NULL)) > 0)
			{
				loading_html++;

				#ifdef USE_DEBUG
				ssend(debug_s, "*** Incoming connection... ");
				#endif

				sys_ppu_thread_t t_id;
				if(working) sys_ppu_thread_create(&t_id, handleclient, (u64)conn_s, THREAD_PRIO, THREAD_STACK_SIZE_64KB, SYS_PPU_THREAD_CREATE_NORMAL, THREAD_NAME_WEB);
				else {sclose(&conn_s); break;}
			}
			else
			if((sys_net_errno == SYS_NET_EBADF) || (sys_net_errno == SYS_NET_ENETDOWN))
			{
				sclose(&list_s);
				list_s = FAILED;
				if(working) goto relisten;
				else break;
			}
		}

	}
end:
	sclose(&list_s);
	sys_ppu_thread_exit(0);
}

int wwwd_start(uint64_t arg)
{
	cellRtcGetCurrentTick(&rTick); gTick = rTick;

	sys_ppu_thread_create(&thread_id_wwwd, wwwd_thread, NULL, THREAD_PRIO, THREAD_STACK_SIZE_8KB, SYS_PPU_THREAD_CREATE_JOINABLE, THREAD_NAME_SVR);
#ifndef CCAPI
	_sys_ppu_thread_exit(0); // remove for ccapi compatibility
#endif
	return SYS_PRX_RESIDENT;
}

static void wwwd_stop_thread(uint64_t arg)
{
	working = 0;

	while(refreshing_xml) sys_timer_usleep(500000); // Prevent unload too fast

	restore_fan(1); // restore & set static fan speed for ps2

	#ifdef AUTO_POWER_OFF
	setAutoPowerOff(false);
	#endif

	sys_timer_usleep(500000);

	uint64_t exit_code;

/*
	sys_ppu_thread_t t_id;

	#ifndef LITE_EDITION
	sys_ppu_thread_create(&t_id, netiso_stop_thread, NULL, THREAD_PRIO_STOP, THREAD_STACK_SIZE_8KB, SYS_PPU_THREAD_CREATE_JOINABLE, STOP_THREAD_NAME);
	sys_ppu_thread_join(t_id, &exit_code);
	#endif

	sys_ppu_thread_create(&t_id, rawseciso_stop_thread, NULL, THREAD_PRIO_STOP, THREAD_STACK_SIZE_8KB, SYS_PPU_THREAD_CREATE_JOINABLE, STOP_THREAD_NAME);
	sys_ppu_thread_join(t_id, &exit_code);

	while(netiso_loaded || rawseciso_loaded) {sys_timer_usleep(100000);}
*/

	sys_ppu_thread_join(thread_id_wwwd, &exit_code);

#ifdef PS3NET_SERVER
	if(thread_id_netsvr != SYS_PPU_THREAD_NONE)
	{
		sys_ppu_thread_join(thread_id_netsvr, &exit_code);
	}
#endif

	if(thread_id_ftpd != SYS_PPU_THREAD_NONE)
	{
		sys_ppu_thread_join(thread_id_ftpd, &exit_code);
	}

#ifdef PS3MAPI
	if(thread_id_ps3mapi != SYS_PPU_THREAD_NONE)
	{
		sys_ppu_thread_join(thread_id_ps3mapi, &exit_code);
	}
#endif

	if(wm_unload_combo != 1)
	{
		if(thread_id_poll != SYS_PPU_THREAD_NONE)
		{
			sys_ppu_thread_join(thread_id_poll, &exit_code);
		}
	}

	sys_ppu_thread_exit(0);
}

static void stop_prx_module(void)
{
	show_msg((char*)STR_WMUNL);

#ifdef REMOVE_SYSCALLS
	remove_cfw_syscall8(); // remove cobra if syscalls were disabled
#endif

	working = 0;

	sys_prx_id_t prx = prx_get_module_id_by_address(stop_prx_module);

	// int *result = NULL;
	// {system_call_6(SC_STOP_PRX_MODULE, (u64)(u32)prx, 0, NULL, (u64)(u32)result, 0, NULL);}

	uint64_t meminfo[5];

	meminfo[0] = 0x28;
	meminfo[1] = 2;
	meminfo[3] = 0;

	{system_call_3(SC_STOP_PRX_MODULE, prx, 0, (u64)(u32)meminfo);}
}

static void unload_prx_module(void)
{

	sys_prx_id_t prx = prx_get_module_id_by_address(unload_prx_module);

	{system_call_3(SC_UNLOAD_PRX_MODULE, prx, 0, NULL);}
}

int wwwd_stop(void)
{
	sys_ppu_thread_t t_id;
	int ret = sys_ppu_thread_create(&t_id, wwwd_stop_thread, NULL, THREAD_PRIO_STOP, THREAD_STACK_SIZE_8KB, SYS_PPU_THREAD_CREATE_JOINABLE, STOP_THREAD_NAME);

	uint64_t exit_code;
	if (ret == 0) sys_ppu_thread_join(t_id, &exit_code);

	sys_timer_usleep(500000);

	unload_prx_module();

	_sys_ppu_thread_exit(0);

	return SYS_PRX_STOP_OK;
}
