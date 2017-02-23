#include <arpa/inet.h>

#include <sys/prx.h>
#include <sys/ppu_thread.h>
#include <sys/process.h>
#include <sys/event.h>
#include <sys/syscall.h>
#include <sys/memory.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/sys_time.h>
#include <sys/timer.h>
#include <cell/pad.h>
#include <cell/cell_fs.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include "include/vsh_exports.h"

#include "include/misc.h"
#include "include/mem.h"
#include "include/blitting.h"

SYS_MODULE_INFO(sLaunch, 0, 1, 1);
SYS_MODULE_START(slaunch_start);
SYS_MODULE_STOP(slaunch_stop);

#define THREAD_NAME         "slaunch_thread"
#define STOP_THREAD_NAME    "slaunch_stop_thread"

#define STR_UNMOUNT		"Unmount"
#define STR_REFRESH		"Refresh"
#define STR_GAMEDATA	"gameDATA"
#define STR_RESTART		"Restart"
#define STR_SHUTDOWN	"Shutdown"
#define STR_SETUP		"Setup"

struct timeval {
	int64_t tv_sec;			/* seconds */
	int64_t tv_usec;		/* and microseconds */
};

#define MAX_GAMES 1000
#define WMTMP				"/dev_hdd0/tmp/wmtmp"				// webMAN work/temp folder
#define WM_ICONS_PATH		"/dev_hdd0/tmp/wm_icons"			// webMAN icons folder

#define XMLMANPLS_DIR        "/dev_hdd0/game/XMBMANPLS"
#define XMLMANPLS_IMAGES_DIR XMLMANPLS_DIR "/USRDIR/IMAGES"

static const char *wm_icons[5] = {
									WM_ICONS_PATH "/icon_wm_dvd.png",       //023.png  [0]
									WM_ICONS_PATH "/icon_wm_psx.png",       //026.png  [1]
									WM_ICONS_PATH "/icon_wm_ps2.png",       //025.png  [2]
									WM_ICONS_PATH "/icon_wm_ps3.png",       //024.png  [3]
									WM_ICONS_PATH "/icon_wm_psp.png",       //022.png  [4]
								};

#define SLIST	"slist"
/*
typedef struct // 1MB for 2000+1 titles
{
	uint8_t type;
	char id[10];
	char name[141]; // 128+12+1 for added ' [BXXX12345]'
	char icon[160];
	char path[160];
	char padd[52];
} __attribute__((packed)) _slaunch;
*/

typedef struct // 1MB for 2000+1 titles
{
	uint8_t  type;
	char     id[10];
	uint8_t  path_pos; // start position of path
	uint16_t icon_pos; // start position of icon
	uint16_t padd;
	char     name[508]; // name + path + icon
} __attribute__((packed)) _slaunch;

#define TYPE_ALL 0
#define TYPE_PS1 1
#define TYPE_PS2 2
#define TYPE_PS3 3
#define TYPE_PSP 4
#define TYPE_VID 5
#define TYPE_DVD 0
#define TYPE_ROM 6
#define TYPE_MAX 7
#define DEVS_MAX 5

static char game_type[TYPE_MAX][8]=
{
	"\0",
	"PS1",
	"PS2",
	"PS3",
	"PSP",
	"video",
	"ROMS"
};

static _slaunch *slaunch = NULL;


#define GRAY_TEXT   0xff808080
#define LIGHT_TEXT  0xffa0a0a0
#define WHITE_TEXT  0xffc0c0c0
#define BRIGHT_TEXT 0xffe0e0e0

#define RED         0xffc00000ffc00000
#define BLUE        0xff0000ffff0000ff
#define DARK_RED    0xff500000ff500000
#define DARK_BLUE   0xff000050ff000050
#define GRAY        0xff808080ff808080

// globals
static uint32_t oldpad=0, curpad=0;
static uint16_t init_delay=0;
static uint16_t games = 0;
static uint16_t cur_game=0, _cur_game=0, cur_game_=0;

int32_t w=0;
int32_t h=0;

static uint8_t key_repeat=0, can_skip=0;

static uint64_t tick=0x80;
static int8_t   delta=5;

CellPadData pdata;

#define NONE   -1
#define SYS_PPU_THREAD_NONE        (sys_ppu_thread_t)NONE

static sys_ppu_thread_t slaunch_tid = SYS_PPU_THREAD_NONE;
static int32_t running = 1;
static uint8_t menu_running = 0;	// vsh menu off(0) or on(1)
static uint8_t fav_mode = 0;

static void return_to_xmb(void);
int32_t slaunch_start(uint64_t arg);
int32_t slaunch_stop(void);

static void finalize_module(void);
static void slaunch_stop_thread(uint64_t arg);
static void slaunch_thread(uint64_t arg);

static void draw_selection(uint16_t game_idx);

#define HDD0  1
#define NTFS  2

static char drives[4][12] = {"dev_hdd0", "ntfs", "dev_usb", "net"};

static uint8_t gmode = TYPE_ALL;
static uint8_t dmode = TYPE_ALL;
static uint8_t cpu_rsx = 0;

static uint32_t frame = 0;

#define SC_SYS_POWER 					(379)
#define SYS_SOFT_REBOOT 				0x0200
#define SYS_HARD_REBOOT					0x1200
#define SYS_REBOOT						0x8201 /*load LPAR id 1*/
#define SYS_SHUTDOWN					0x1100

#define SC_COBRA_SYSCALL8			 				(8)
#define SYSCALL8_OPCODE_PS3MAPI			 			0x7777
#define PS3MAPI_OPCODE_GET_VSH_PLUGIN_INFO			0x0047

static int load_plugin_by_id(int id, void *handler)
{
	xmm0_interface = (xmb_plugin_xmm0 *)paf_23AFB290((uint32_t)paf_F21655F3("xmb_plugin"), 0x584D4D30);
	return xmm0_interface->LoadPlugin3(id, handler,0);
}

static void web_browser(void)
{
	webbrowser_interface = (webbrowser_plugin_interface *)paf_23AFB290((uint32_t)paf_F21655F3("webbrowser_plugin"), 1);
	if(webbrowser_interface) webbrowser_interface->PluginWakeupWithUrl("http://127.0.0.1/setup.ps3");
}

/*
static int unload_plugin_by_id(int id, void *handler)
{
	xmm0_interface = (xmb_plugin_xmm0 *)paf_23AFB290((uint32_t)paf_F21655F3("xmb_plugin"), 0x584D4D30);//'XMM0'
	if(xmm0_interface) return xmm0_interface->Shutdown(id, handler, 1); else return 0;
}

static void web_browser_stop(void)
{
	webbrowser_interface = (webbrowser_plugin_interface *)paf_23AFB290((uint32_t)paf_F21655F3("webbrowser_plugin"), 1);
	if(webbrowser_interface) webbrowser_interface->Shutdown();
}
*/

static unsigned int get_vsh_plugin_slot_by_name(const char *name)
{
	char tmp_name[30];
	char tmp_filename[256];
	unsigned int slot;

	for (slot = 1; slot < 7; slot++)
	{
		memset(tmp_name, 0, sizeof(tmp_name));
		memset(tmp_filename, 0, sizeof(tmp_filename));

		{system_call_5(SC_COBRA_SYSCALL8, SYSCALL8_OPCODE_PS3MAPI, PS3MAPI_OPCODE_GET_VSH_PLUGIN_INFO, (uint64_t)slot, (uint64_t)(uint32_t)tmp_name, (uint64_t)(uint32_t)tmp_filename); }

		if(strstr(tmp_name, name)) break;
	}
	return slot;
}

////////////////////////////////////////////////////////////////////////
//			SYS_PPU_THREAD_EXIT, DIRECT OVER SYSCALL				//
////////////////////////////////////////////////////////////////////////
static inline void _sys_ppu_thread_exit(uint64_t val)
{
	system_call_1(41, val);
}

////////////////////////////////////////////////////////////////////////
//						 GET MODULE BY ADDRESS						//
////////////////////////////////////////////////////////////////////////
static inline sys_prx_id_t prx_get_module_id_by_address(void *addr)
{
	system_call_1(461, (uint64_t)(uint32_t)addr);
	return (int32_t)p1;
}

////////////////////////////////////////////////////////////////////////
//                      GET CPU & RSX TEMPERATURES                  //
////////////////////////////////////////////////////////////////////////
static void get_temperature(uint32_t _dev, uint32_t *_temp)
{
	system_call_2(383, (uint64_t)(uint32_t) _dev, (uint64_t)(uint32_t) _temp); *_temp >>= 24;
}

static int connect_to_webman(void)
{
	struct sockaddr_in sin;
	int s;

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0x7F000001; //127.0.0.1 (localhost)
	sin.sin_port = htons(80);         //http port (80)
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0)
	{
		return NONE;
	}

	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		return NONE;
	}

	return s;
}

static void sclose(int *socket_e)
{
	if(*socket_e != NONE)
	{
		shutdown(*socket_e, SHUT_RDWR);
		socketclose(*socket_e);
		*socket_e = NONE;
	}
}

static int send_wm_request(const char *cmd)
{
	// send command
	int conn_s = NONE;
	conn_s = connect_to_webman();

	struct timeval tv;
	tv.tv_usec = 0;
	tv.tv_sec = 10;
	setsockopt(conn_s, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
//	setsockopt(conn_s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	if(conn_s >= 0)
	{
		int pa=0;
		char proxy_action[512];
		proxy_action[pa++] = 'G';
		proxy_action[pa++] = 'E';
		proxy_action[pa++] = 'T';
		proxy_action[pa++] = ' ';

		for(uint16_t i=0;(i < strlen(cmd)) && (pa < 500); i++)
		{
			if(cmd[i] != 0x20)
				proxy_action[pa++] = cmd[i];
			else
			{
				proxy_action[pa++] = '%';
				proxy_action[pa++] = '2';
				proxy_action[pa++] = '0';
			}
		}

		proxy_action[pa++] = '\r';
		proxy_action[pa++] = '\n';
		proxy_action[pa] = 0;
		send(conn_s, proxy_action, pa, 0);
		sclose(&conn_s);
	}
	return conn_s;
}

static void pad_read(void)
{
	// check only pad ports 0 and 1
	for(int32_t port=0; port<2; port++)
		{MyPadGetData(port, &pdata); curpad = (pdata.button[2] | (pdata.button[3] << 8)); if(curpad && (pdata.len > 0)) break;}  // use MyPadGetData() during VSH menu
}

static uint64_t file_exists(const char* path)
{
	struct CellFsStat s;
	s.st_size=0;
	cellFsStat(path, &s);
	return(s.st_size);
}

static void load_background(void)
{
	char path[64];

	if(fav_mode)
		sprintf(path, "%s_fav.jpg", "/dev_hdd0/plugins/images/slaunch");
	else if(gmode)
	{
		sprintf(path, "%s_%s.jpg", "/dev_hdd0/plugins/images/slaunch", game_type[gmode]);
		if(!gmode || !file_exists(path)) sprintf(path, "%s.jpg", "/dev_hdd0/plugins/images/slaunch");
	}

	reset_heap();

	if(file_exists(path))
		load_img_bitmap(0, path);
	else if(fav_mode)
		load_img_bitmap(0, "/dev_flash/vsh/resource/explore/icon/cinfo-bg-whatsnew.jpg");
	else if(!gmode)
		load_img_bitmap(0, "/dev_flash/vsh/resource/explore/icon/cinfo-bg-storemain.jpg");
	else
		load_img_bitmap(0, "/dev_flash/vsh/resource/explore/icon/cinfo-bg-storegame.jpg");
}

static void draw_page(uint16_t game_idx, uint8_t key_repeat)
{
	if(!games || game_idx>=games) return;

	const int left_margin=56;

	uint8_t slot = 0;
	uint16_t i, j;
	int px=left_margin, py=90;	// top-left

	// draw background and menu strip
	flip_frame((uint64_t*)ctx.canvas);
	memcpy((uint8_t *)ctx.menu, (uint8_t *)(ctx.canvas)+900*CANVAS_W*4, CANVAS_W*96*4);

	set_textbox(GRAY, 0,  890, CANVAS_W, 2);
	set_textbox(GRAY, 0, 1000, CANVAS_W, 2);

	// draw game icons (5x2)
	j=(game_idx/10)*10;
	for(i=j;(slot<10&&i<games);i++)
	{
		slot++;

		if(file_exists(slaunch[i].name + slaunch[i].icon_pos)==false)
		{
			sprintf(slaunch[i].name + slaunch[i].icon_pos, wm_icons[slaunch[i].type]);
		}

		if(load_img_bitmap(slot, slaunch[i].name + slaunch[i].icon_pos)<0) break;
		py=((i-j)/5)*400+90+(300-ctx.img[slot].h)/2;
		ctx.img[slot].x=((px+(320-ctx.img[slot].w)/2)/2)*2;
		ctx.img[slot].y=py;
		set_backdrop(slot, 0);
		set_texture(slot, ctx.img[slot].x, ctx.img[slot].y);
		px+=(320+48); if(px>1600) px=left_margin;
		if(key_repeat) break;
	}
	draw_selection(game_idx);
}

static void draw_selection(uint16_t game_idx)
{
	uint8_t slot = 1 + (game_idx % 10);
	char one_of[32], mode[8];

	if(games)
	{
		char *path = slaunch[game_idx].name + slaunch[game_idx].path_pos;

		// game name
		if(ISHD(w))	set_font(32.f, 32.f, 1.0f, 1);
		else		set_font(32.f, 32.f, 3.0f, 1);
		ctx.fg_color=WHITE_TEXT;
		print_text(ctx.menu, CANVAS_W, CENTER_TEXT, 0, slaunch[game_idx].name );

		// game path
		if(ISHD(w))	set_font(24.f, 16.f, 1.0f, 1);
		else		set_font(32.f, 16.f, 2.0f, 1);
		ctx.fg_color=GRAY_TEXT;

		if(*path == '/' && path[10] == '/') path += 10;
		if(*path == '/') print_text(ctx.menu, CANVAS_W, CENTER_TEXT, 40, path);
	}

	// game index
	if(ISHD(w))	set_font(20.f, 20.f, 1.0f, 1);
	else		set_font(32.f, 20.f, 2.0f, 1);
	ctx.fg_color=LIGHT_TEXT;
	sprintf(one_of, "%i / %i", game_idx+1, games);
	print_text(ctx.menu, CANVAS_W, CENTER_TEXT, 64, one_of );

	// game list mode
	if(gmode) sprintf(mode, "%s", game_type[gmode]);

	if(fav_mode)
		print_text(ctx.menu, CANVAS_W, 80, 64, "Favorites");
	else if(dmode && gmode)
	{
		sprintf(one_of, "/%s/%s", drives[dmode-1], mode);
		print_text(ctx.menu, CANVAS_W, 80, 64, one_of);
	}
	else if(dmode)
		print_text(ctx.menu, CANVAS_W, 80, 64, drives[dmode-1]);
	else if(gmode)
		print_text(ctx.menu, CANVAS_W, 80, 64, mode);

	// temperature
	if(ISHD(w) || (h==720))
	{
		char s_temp[64];
		uint32_t temp_c = 0, temp_f = 0;
		get_temperature(cpu_rsx, &temp_c);
		temp_f = (uint32_t)(1.8f * (float)temp_c + 32.f);
		sprintf(s_temp, "%s :  %i C  /  %i F", cpu_rsx ? "RSX" : "CPU", temp_c, temp_f);
		print_text(ctx.menu, CANVAS_W, CANVAS_W - ((h==720) ? 450 : 300), 64, s_temp);
	}

	// set frame buffer
	set_texture_direct(ctx.menu, 0, 900, CANVAS_W, 96);
	memcpy((uint8_t *)ctx.menu, (uint8_t *)(ctx.canvas)+900*CANVAS_W*4, CANVAS_W*96*4);
	if(games) set_frame(slot, RED);
}

static void draw_side_menu_option(uint8_t option)
{
	memset((uint8_t *)ctx.side, 0x40, SM_M);
	ctx.fg_color=BRIGHT_TEXT;
	set_font(28.f, 24.f, 1.f, 0); print_text(ctx.side, (CANVAS_W-SM_X), SM_TO, SM_Y, "sLaunch MOD 1.06");

	ctx.fg_color=(option==1 ? WHITE_TEXT : GRAY_TEXT);
	print_text(ctx.side, (CANVAS_W-SM_X), SM_TO+(option!=1)*32, SM_Y+4*24, STR_UNMOUNT);
	ctx.fg_color=(option==2 ? WHITE_TEXT : GRAY_TEXT);
	print_text(ctx.side, (CANVAS_W-SM_X), SM_TO+(option!=2)*32, SM_Y+6*24, STR_REFRESH);
	ctx.fg_color=(option==3 ? WHITE_TEXT : GRAY_TEXT);
	print_text(ctx.side, (CANVAS_W-SM_X), SM_TO+(option!=3)*32, SM_Y+8*24, STR_GAMEDATA);
	ctx.fg_color=(option==4 ? WHITE_TEXT : GRAY_TEXT);
	print_text(ctx.side, (CANVAS_W-SM_X), SM_TO+(option!=4)*32, SM_Y+10*24, STR_RESTART);
	ctx.fg_color=(option==5 ? WHITE_TEXT : GRAY_TEXT);
	print_text(ctx.side, (CANVAS_W-SM_X), SM_TO+(option!=5)*32, SM_Y+12*24, STR_SHUTDOWN);
	ctx.fg_color=(option==6 ? WHITE_TEXT : GRAY_TEXT);
	print_text(ctx.side, (CANVAS_W-SM_X), SM_TO+(option!=6)*32, SM_Y+19*24, STR_SETUP);

	set_texture_direct(ctx.side, SM_X, 0, (CANVAS_W-SM_X), CANVAS_H);
	set_textbox(GRAY, SM_X+SM_TO, SM_Y+28,    CANVAS_W-SM_X-SM_TO*2, 1);
	set_textbox(GRAY, SM_X+SM_TO, SM_Y+16*24, CANVAS_W-SM_X-SM_TO*2, 1);
}

static uint8_t draw_side_menu(void)
{
	uint8_t option=1;
	play_rco_sound("snd_cursor");

	set_textbox(0xffe0e0e0ffd0d0d0, SM_X-6, 0, 2, CANVAS_H);
	set_textbox(0xffc0c0c0ffb0b0b0, SM_X-4, 0, 2, CANVAS_H);
	set_textbox(0xffa0a0a0ff909090, SM_X-2, 0, 2, CANVAS_H);

	draw_side_menu_option(option);

	while(menu_running)
	{
		pad_read();

		if(curpad)
		{
			if(curpad==oldpad)	// key-repeat
			{
				init_delay++;
				if(init_delay<=40) continue;
				else { sys_timer_usleep(40000); key_repeat=1; }
			}
			else
			{
				init_delay=0;
				key_repeat=0;
			}

			oldpad = curpad;

			if(curpad & PAD_UP)		option--;
			if(curpad & PAD_DOWN)	option++;

			if(option<1) option=6;
			if(option>6) option=1;

			if(curpad & PAD_TRIANGLE || curpad & PAD_CIRCLE) {option=0; play_rco_sound("snd_cancel"); break;}

			if(curpad & PAD_CROSS) {play_rco_sound("snd_system_ok"); break;}

			play_rco_sound("snd_cursor");
			draw_side_menu_option(option);
		}
		else
		{
			init_delay=0; oldpad=0;
		}
	}
	init_delay=key_repeat=0;
	return option;
}

static void show_no_content(void)
{
	char no_content[32];
	if(gmode >= TYPE_MAX)
		{sprintf(no_content, "webMAN is not loaded."); gmode = TYPE_ALL;}
	else
		sprintf(no_content, "There are no %s%stitles.", game_type[gmode], gmode ? " " : "\0");

	ctx.fg_color=WHITE_TEXT;
	set_font(32.f, 32.f, 1.5f, 1); print_text(ctx.canvas, CANVAS_W, 0, 520, no_content);
	flip_frame((uint64_t*)ctx.canvas);

	sys_timer_usleep(50000);

	if(gmode) load_background();
}

static void show_content(void)
{
	menu_running = 1;

	if(games)
	{
		draw_page(cur_game, 0);
	}
	else	// no content
	{
		show_no_content();
	}
}

static void load_data(void)
{
	int fd;

	if(get_vsh_plugin_slot_by_name("WWWD") >= 7) {games=fav_mode=0, gmode = TYPE_MAX; goto exit_load;}

	char filename[64];
	if(fav_mode) sprintf(filename, WMTMP "/" SLIST "1.bin"); else sprintf(filename, WMTMP "/" SLIST ".bin");

	uint64_t size = file_exists(filename);
	if(fav_mode && (size == 0)) {sprintf(filename, WMTMP "/" SLIST ".bin"); size = file_exists(filename); fav_mode = TYPE_ALL; cur_game = cur_game_; if(cur_game>=games) _cur_game=cur_game=0;}
	if(fav_mode == 0) cur_game = cur_game_;

	games=size/sizeof(_slaunch);

	if(games>=MAX_GAMES) games=MAX_GAMES-1;
	if(cur_game>=games) _cur_game=cur_game=0;

reload:
	load_background(); slaunch = NULL;

	if(games && (cellFsOpen(filename, CELL_FS_O_RDONLY, &fd, NULL, 0) == CELL_FS_SUCCEEDED))
	{
		// load game list in MC memory
		slaunch = (_slaunch*)mem_alloc((games+1)*sizeof(_slaunch));

		cellFsRead(fd, (void *)slaunch, sizeof(_slaunch)*games, NULL);
		cellFsClose(fd);

		if(!fav_mode)
		{
			uint16_t ngames = games;

			if(gmode)		// filter games by type
			{
				ngames = 0;

				for(uint16_t n=0; n < games; n++)
				{
					if(slaunch[n].type == gmode) slaunch[ngames++] = slaunch[n];
				}

				if(ngames == 0) {gmode++; if(gmode >= TYPE_MAX) gmode = TYPE_ALL; goto reload;}
			}

			if(dmode)		// filter games by device
			{
				int8_t dlen = strlen(drives[dmode-1]);

				games = ngames; ngames = 0;

				for(uint16_t n=0; n < games; n++)
				{
					char *path = slaunch[n].name + slaunch[n].path_pos;

					if( ((dmode == NTFS) && (strncmp(path+10, "/dev_hdd0/tmp", 13)  ==0)) ||
						((dmode == HDD0) && (strncmp(path+11, drives[dmode-1], dlen)==0) && (path[10+10]!='t')) ||
						((dmode >  NTFS) && (strncmp(path+11, drives[dmode-1], dlen)==0))
					  )
						slaunch[ngames++] = slaunch[n];
				}

				if(ngames == 0) {dmode++; if(dmode >= DEVS_MAX) dmode = TYPE_ALL; goto reload;}
			}

			if(games > ngames) mem_free((games - ngames) * sizeof(_slaunch));

			games = ngames;

			// sort game list
			if(games>1)
			{
				_slaunch swap;
				for(uint16_t n=0; n<(games-1); n++)
				{
					for(uint16_t m=(n+1); m<games; m++)
					{
						if(strcasecmp(slaunch[n].name, slaunch[m].name)>0)
						{
							swap=slaunch[n];
							slaunch[n]=slaunch[m];
							slaunch[m]=swap;
						}
					}
				}
			}
		}
	}
	else
		games = 0;

exit_load:
	show_content();
}

static void reload_data(uint32_t curpad)
{
	play_rco_sound("snd_cursor");

	if(curpad & PAD_R2) gmode=dmode=TYPE_ALL;

	_cur_game=cur_game=0;

	load_data();
}

//////////////////////////////////////////////////////////////////////
//                       START VSH MENU                             //
//////////////////////////////////////////////////////////////////////

static void start_VSH_Menu(void)
{
	int32_t ret, mem_size;

	uint64_t gamelist_size = file_exists(WMTMP "/" SLIST ".bin");

	mem_size = ((CANVAS_W * CANVAS_H * 4) + (CANVAS_W * 96 * 4) + (FONT_CACHE_MAX * 32 * 32) + (MAX_WH4) + (SM_M) + (gamelist_size) + MB(1)) / MB(1);

	// create VSH Menu heap memory from memory container 1("app")
	ret = create_heap(mem_size);

	if(ret) return;

	rsx_fifo_pause(1);

	w = getDisplayWidth();	// display width
	h = getDisplayHeight();

	// initialize VSH Menu graphic
	init_graphic();

	// stop vsh pad
	start_stop_vsh_pad(0);

	// load background
	load_background();

	// load game list
	load_data();

	if(!games)
	{
		send_wm_request("/refresh_ps3");
		return_to_xmb();
		load_plugin_by_id(0x1B, (void *)web_browser);
	}
}

//////////////////////////////////////////////////////////////////////
//                       STOP VSH MENU                              //
//////////////////////////////////////////////////////////////////////

static void stop_VSH_Menu(void)
{
	// menu off
	menu_running = 0;

	// unbind renderer and kill font-instance
	font_finalize();

	// free heap memory
	destroy_heap();

	// continue rsx rendering
	rsx_fifo_pause(0);

	// restart vsh pad
	start_stop_vsh_pad(1);
}

static void return_to_xmb(void)
{
	dump_bg();
	dim_bg(0.5f, 0.0f);
	stop_VSH_Menu();
}

static void blink_option(uint64_t color, uint64_t color2, uint32_t msecs)
{
	for(uint8_t u = 0; u < 10; u++)
	{
		set_frame(1 + cur_game % 10, (u & 1) ? color : color2);
		sys_timer_usleep(msecs);
	}
}
static void add_game(void)
{
	int fd;

	blink_option(BLUE, DARK_BLUE, 25000);

	play_rco_sound("snd_cursor"); if(!games) return;

	if(cellFsOpen(WMTMP "/" SLIST "1.bin", CELL_FS_O_RDONLY, &fd, NULL, 0) == CELL_FS_SUCCEEDED)
	{
		_slaunch temp; uint64_t bytes_read;

		char *path = slaunch[cur_game].name + slaunch[cur_game].path_pos;

		while(cellFsRead(fd, (void *)&temp, sizeof(_slaunch), &bytes_read) == CELL_FS_SUCCEEDED)
		{
			if(bytes_read < sizeof(_slaunch)) break;

			if(!strcmp(temp.name + temp.path_pos, path)) {cellFsClose(fd); return;}
		}
		cellFsClose(fd);
	}

	play_rco_sound("snd_system_ok");

	if(cellFsOpen(WMTMP "/" SLIST "1.bin", CELL_FS_O_APPEND | CELL_FS_O_CREAT | CELL_FS_O_WRONLY, &fd, NULL, 0) == CELL_FS_SUCCEEDED)
	{
		cellFsWrite(fd, (void *)&slaunch[cur_game], sizeof(_slaunch), NULL);
		cellFsClose(fd);
	}
}

static void remove_game(void)
{
	int fd;

	if(games && (cellFsOpen(WMTMP "/" SLIST "1.bin", CELL_FS_O_CREAT | CELL_FS_O_WRONLY | CELL_FS_O_TRUNC, &fd, NULL, 0) == CELL_FS_SUCCEEDED))
	{
		play_rco_sound("snd_cancel");

		uint16_t n;

		for(n = 0; n < games; n++)
		{
			if(n!=cur_game) cellFsWrite(fd, (void *)&slaunch[n], sizeof(_slaunch), NULL);
		}
		cellFsClose(fd);

		games--;

		blink_option(RED, DARK_RED, 25000);

		for(n = cur_game; n < games; n++)
		{
			slaunch[n] = slaunch[n+1];
		}

		memset(&slaunch[games], 0, sizeof(_slaunch)); mem_free(sizeof(_slaunch)); if(cur_game>=games) {cur_game--; draw_page(cur_game, 0);}

		show_content();
	}
	else
		play_rco_sound("snd_cursor");
}

////////////////////////////////////////////////////////////////////////
//                      PLUGIN MAIN PPU THREAD                        //
////////////////////////////////////////////////////////////////////////
static void slaunch_thread(uint64_t arg)
{
	sys_timer_sleep(15);												// wait 15s and not interfere with boot process
	play_rco_sound("snd_system_ng");

	while(running)
	{
		if(!menu_running)												// VSH menu is not running, normal XMB execution
		{
			sys_timer_usleep(500000);
			pdata.len = 0;
			for(uint8_t p = 0; p < 2; p++)
				if(cellPadGetData(p, &pdata) == CELL_PAD_OK && pdata.len > 0) break;

			if(pdata.len)					// if pad data and we are on XMB
			{
				if( ((pdata.button[CELL_PAD_BTN_OFFSET_DIGITAL1] == 0) &&
					 (pdata.button[CELL_PAD_BTN_OFFSET_DIGITAL2] == (CELL_PAD_CTRL_L2 | CELL_PAD_CTRL_R2)))
					||
					((pdata.button[CELL_PAD_BTN_OFFSET_DIGITAL2] == 0) &&
					 (pdata.button[CELL_PAD_BTN_OFFSET_DIGITAL1] == (CELL_PAD_CTRL_START))) )
				{
					if(xsetting_CC56EB2D()->GetCurrentUserNumber()>=0) // user logged in
					{
						if(vshmain_EB757101() == 0)
						{
							start_VSH_Menu();
							init_delay=0;
						}
					}
					else
					{
						send_wm_request("/popup.ps3?Not%20logged%20in!");
					}
					sys_timer_sleep(2);
				}
			}
		}
		else // menu is running
		{
			while(menu_running && running)
			{
				pad_read();

				if(curpad)
				{
					if(curpad==oldpad)				// key-repeat
					{
						init_delay++;
						if(init_delay<=40) continue;
						else sys_timer_usleep(40000);
					}
					else
						init_delay=0;

					can_skip=0;
					oldpad = curpad;
					_cur_game=cur_game;

					if(curpad & PAD_TRIANGLE)		// open side-menu
					{
						uint8_t option=draw_side_menu();
						if(option)
						{
							if(option==1) send_wm_request("/mount_ps3/unmount");
							if(option==2) send_wm_request("/refresh_ps3");
							if(option==3) send_wm_request("/extgd.ps3");
							return_to_xmb();
							if(option==4) {system_call_3(SC_SYS_POWER, SYS_REBOOT, NULL, 0);}   //send_wm_request("/restart.ps3");
							if(option==5) {system_call_4(SC_SYS_POWER, SYS_SHUTDOWN, 0, 0, 0);} //send_wm_request("/shutdown.ps3");
							if(option==6) {load_plugin_by_id(0x1B, (void *)web_browser);}
							break;
						}
						show_content();
					}
					else if(curpad & PAD_CIRCLE)	// back to XMB
					{
						//if(fav_mode) {fav_mode=0; reload_data(curpad);} else
						{
							cur_game_ = cur_game;
							play_rco_sound("snd_cancel");
							stop_VSH_Menu(); /*return_to_xmb();*/
						}
						break;
					}
					else if(curpad & PAD_SELECT)	// alternate menu
					{
						play_rco_sound("snd_cursor");
						if(fav_mode == 0) cur_game_ = cur_game;
						fav_mode^=1;
						reload_data(curpad);
						sys_timer_usleep(40000);
						continue;
					}
					else if(curpad & PAD_R3)	{return_to_xmb(); send_wm_request("/popup.ps3"); break;}
					else if(curpad & PAD_L3)	{return_to_xmb(); send_wm_request("/refresh_ps3"); break;}
					else if((curpad & PAD_SQUARE) && !fav_mode && !(curpad & PAD_L2)) {gmode++; if(gmode>=TYPE_MAX) gmode=TYPE_ALL; dmode=TYPE_ALL; reload_data(curpad);}
					else if((curpad & PAD_SQUARE) && !fav_mode &&  (curpad & PAD_L2)) {dmode++; if(dmode>=DEVS_MAX) dmode=TYPE_ALL; reload_data(curpad);}
					else if((curpad == PAD_START) && games)	// favorite game XMB
					{
						if(fav_mode) remove_game(); else add_game();
						continue;
					}

					if(!games) continue;

						if(curpad & PAD_RIGHT)	cur_game+=1;
					else if(curpad & PAD_LEFT)	{if(!cur_game) cur_game=games-1; else cur_game-=1;}
					else if(curpad & PAD_UP)	{if(!cur_game) cur_game=games-1; else if(cur_game<5) cur_game=0; else cur_game-=5;}
					else if(curpad & PAD_DOWN)	{if(cur_game==(games-1)) cur_game=0; else if((cur_game+5)>=games) cur_game=games-1; else cur_game+=5;}
					else if(curpad & PAD_R1)	{can_skip=1; if(cur_game==(games-1)) cur_game=0; else if((cur_game+10)>=games) cur_game=games-1; else cur_game+=10;}
					else if(curpad & PAD_L1)	{can_skip=1; if(!cur_game) cur_game=games-1; else cur_game-=10;}

					else if(curpad & PAD_CROSS && games)	// execute action & return to XMB
					{
						play_rco_sound("snd_system_ok");

						blink_option(RED, DARK_RED, 75000);

						char path[160];
						snprintf(path, 160, "%s", slaunch[cur_game].name + slaunch[cur_game].path_pos);

						cur_game_ = cur_game;

						return_to_xmb();
						send_wm_request(path);
						break;
					}

					if(cur_game!=_cur_game && games)		// draw backgrop
					{
						play_rco_sound("snd_cursor");
						tick=0xc0;
						set_backdrop((1+_cur_game%10), 1);
						if(cur_game>=games) cur_game=0;
						if((cur_game/10)*10 != (_cur_game/10)*10)
							draw_page(cur_game, key_repeat & can_skip);
						else
							draw_selection(cur_game);
					}
				}
				else
				{
					init_delay=0, oldpad=0, tick+=delta;			// pulsing selection frame
					if(games) set_frame(1 + cur_game % 10, 0xff000000ff000000|tick<<48|tick<<16);
					if(tick<0x80 || tick>0xF0)delta=-delta;

					// update temperature
					if(++frame > 300) {frame = 0, cpu_rsx ^= 1; draw_selection(cur_game);}
				}
			}
		}
	}

	if(menu_running)
		stop_VSH_Menu();

	sys_ppu_thread_exit(0);
}

/***********************************************************************
* start thread
***********************************************************************/
int32_t slaunch_start(uint64_t arg)
{
	sys_ppu_thread_create(&slaunch_tid, slaunch_thread, 0, 3000, 0x3000, 1, THREAD_NAME);

	_sys_ppu_thread_exit(0);
	return SYS_PRX_RESIDENT;
}

/***********************************************************************
* stop thread
***********************************************************************/
static void slaunch_stop_thread(uint64_t arg)
{
	if(menu_running) stop_VSH_Menu();

	running = 0;

	uint64_t exit_code;

	if(slaunch_tid != SYS_PPU_THREAD_NONE)
			sys_ppu_thread_join(slaunch_tid, &exit_code);

	sys_ppu_thread_exit(0);
}

/***********************************************************************
*
***********************************************************************/
static void finalize_module(void)
{
	uint64_t meminfo[5];

	sys_prx_id_t prx = prx_get_module_id_by_address(finalize_module);

	meminfo[0] = 0x28;
	meminfo[1] = 2;
	meminfo[3] = 0;

	system_call_3(482, prx, 0, (uint64_t)(uint32_t)meminfo);
}

/***********************************************************************
*
***********************************************************************/
int slaunch_stop(void)
{
	sys_ppu_thread_t t;
	uint64_t exit_code;

	int ret = sys_ppu_thread_create(&t, slaunch_stop_thread, 0, 0, 0x2000, 1, STOP_THREAD_NAME);
	if (ret == 0) sys_ppu_thread_join(t, &exit_code);

	finalize_module();

	_sys_ppu_thread_exit(0);
	return SYS_PRX_STOP_OK;
}
