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

struct timeval {
	int64_t tv_sec;			/* seconds */
	int64_t tv_usec;		/* and microseconds */
};

#define MAX_GAMES 1000
#define WMTMP				"/dev_hdd0/tmp/wmtmp"				// webMAN work/temp folder
#define WM_ICONS_PATH		"/dev_hdd0/tmp/wm_icons"			// webMAN icons folder

#define XMLMANPLS_DIR        "/dev_hdd0/game/XMBMANPLS"
#define XMLMANPLS_IMAGES_DIR XMLMANPLS_DIR "/USRDIR/IMAGES"

static char wm_icons[5][40] = {
								WM_ICONS_PATH "/icon_wm_ps3.png",       //024.png  [0]
								WM_ICONS_PATH "/icon_wm_psx.png",       //026.png  [1]
								WM_ICONS_PATH "/icon_wm_ps2.png",       //025.png  [2]
								WM_ICONS_PATH "/icon_wm_psp.png",       //022.png  [3]
								WM_ICONS_PATH "/icon_wm_dvd.png",       //023.png  [4]
								};

typedef struct
{
	char path[128];
	char icon[128];
	char name[112];
	char padding[5];
	char id[10];
	uint8_t type;
} __attribute__((packed)) _slaunch;

#define TYPE_ALL 0
#define TYPE_PS1 1
#define TYPE_PS2 2
#define TYPE_PS3 3
#define TYPE_PSP 4
#define TYPE_VID 5
#define TYPE_ROM 6
#define TYPE_MAX 7

static _slaunch *slaunch = NULL;

static uint32_t games = 0;
static uint32_t cur_game=0, _cur_game=0, cur_game_=0;

int32_t w=0;
int32_t h=0;
static uint64_t tick=0x80;
static int8_t   delta=5;

#define NONE   -1
#define SYS_PPU_THREAD_NONE        (sys_ppu_thread_t)NONE

static sys_ppu_thread_t slaunch_tid = SYS_PPU_THREAD_NONE;
static int32_t running = 1;
static uint8_t menu_running = 0;	// vsh menu off(0) or on(1)
static uint8_t menu_mode = 0;

static void return_to_xmb(void);
int32_t slaunch_start(uint64_t arg);
int32_t slaunch_stop(void);

static void finalize_module(void);
static void slaunch_stop_thread(uint64_t arg);
static void slaunch_thread(uint64_t arg);

static void draw_selection(uint32_t game_idx);

enum gameModes
{
	modeALL = 0,
	modePS3 = 1,
	modePSX = 2,
	modePS2 = 3,
	modePSP = 4,
	modeDVD = 5,
	modeROM = 6,
	modeLAST= 6,
	devsLAST= 4,
};

#define NTFS  3

static char drives[4][12] = {"dev_hdd0", "dev_usb", "ntfs", "net"};
static char gmodes[6][8]  = {"PS3", "PSX", "PS2", "PSP", "VIDEO", "ROMS"};

static uint8_t gmode = modeALL;
static uint8_t dmode = modeALL;
static uint8_t cpu_rsx = 0;

static uint32_t frame = 0;

#define SC_COBRA_SYSCALL8			 				(8)
#define SYSCALL8_OPCODE_PS3MAPI			 			0x7777
#define PS3MAPI_OPCODE_GET_VSH_PLUGIN_INFO			0x0047

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

		if(!strstr(tmp_name, name)) break;
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

static void send_wm_request(const char *cmd)
{
	// send command
	int conn_s = NONE;
	conn_s = connect_to_webman();

	struct timeval tv;
	tv.tv_usec = 0;
	tv.tv_sec = 3;
	setsockopt(conn_s, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	setsockopt(conn_s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

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
}

static uint64_t file_exists(const char* path)
{
	struct CellFsStat s;
	s.st_size=0;
	cellFsStat(path, &s);
	return(s.st_size);
}

static void draw_page(uint32_t game_idx)
{
	if(!games || game_idx>=games) return;

	const int left_margin=56;

	uint8_t slot = 0;
	uint32_t i, j;
	int px=left_margin, py=90;	// top-left

	// draw background and menu strip
	flip_frame((uint64_t*)ctx.canvas);
	memcpy((uint8_t *)ctx.menu, (uint8_t *)(ctx.canvas)+900*CANVAS_W*4, CANVAS_W*96*4);

	set_textbox(0xff808080ff808080, 0, 890, CANVAS_W, 2);
	set_textbox(0xff808080ff808080, 0, 1000, CANVAS_W, 2);

	// draw game icons (5x2)
	j=(game_idx/10)*10;
	for(i=j;(slot<10&&i<games);i++)
	{
		slot++;

		if(file_exists(slaunch[i].icon)==false)
		{
			if((slaunch[i] == TYPE_PS1) || strstr(slaunch[i].path, "PSX"))	sprintf(slaunch[i].icon, wm_icons[1]); else
			if((slaunch[i] == TYPE_PS2) || strstr(slaunch[i].path, "PS2"))	sprintf(slaunch[i].icon, wm_icons[2]); else
			if((slaunch[i] == TYPE_PSP) || strstr(slaunch[i].path, "PSP"))	sprintf(slaunch[i].icon, wm_icons[3]); else
			if((slaunch[i] == TYPE_VID) || strstr(slaunch[i].path, "DVD"))	sprintf(slaunch[i].icon, wm_icons[4]); else
																			sprintf(slaunch[i].icon, wm_icons[0]);
		}

		load_img_bitmap(slot, slaunch[i].icon);
		py=((i-j)/5)*400+90+(300-ctx.img[slot].h)/2;
		ctx.img[slot].x=((px+(320-ctx.img[slot].w)/2)/2)*2;
		ctx.img[slot].y=py;
		set_backdrop(slot, 0);
		set_texture(slot, ctx.img[slot].x, ctx.img[slot].y);
		px+=(320+48); if(px>1600) px=left_margin;
	}
	draw_selection(game_idx);
}

static void draw_selection(uint32_t game_idx)
{
	uint8_t slot = 1 + game_idx % 10;
	char one_of[32], mode[8], *path = slaunch[game_idx].path;

	// game name
	if(ISHD(w))	set_font(32.f, 32.f, 1.0f, 1);
	else		set_font(32.f, 32.f, 3.0f, 1);
	ctx.fg_color=0xffc0c0c0;
	print_text(ctx.menu, CENTER_TEXT, 0, slaunch[game_idx].name );

	// game path
	if(ISHD(w))	set_font(24.f, 16.f, 1.0f, 1);
	else		set_font(32.f, 16.f, 2.0f, 1);
	ctx.fg_color=0xff808080;

	if(*path == '/' && path[10] == '/') path += 10;
	if(*path == '/') print_text(ctx.menu, CENTER_TEXT, 40, path);

	// game index
	if(ISHD(w))	set_font(20.f, 20.f, 1.0f, 1);
	else		set_font(32.f, 20.f, 2.0f, 1);
	ctx.fg_color=0xffA0A0A0;
	sprintf(one_of, "%i / %i", game_idx+1, games);
	print_text(ctx.menu, CENTER_TEXT, 64, one_of );

	// game list mode
	if(gmode) sprintf(mode, "%s", gmodes[gmode-1]);

	if(menu_mode)
		print_text(ctx.menu, 80, 64, "MENU");
	else if(dmode && gmode)
	{
		sprintf(one_of, "/%s/%s", drives[dmode-1], mode);
		print_text(ctx.menu, 80, 64, one_of);
	}
	else if(dmode)
		print_text(ctx.menu, 80, 64, drives[dmode-1]);
	else if(gmode)
		print_text(ctx.menu, 80, 64, mode);

	// temperature
	if(ISHD(w) || (h==720))
	{
		char s_temp[64];
		uint32_t temp_c = 0, temp_f = 0;
		get_temperature(cpu_rsx, &temp_c);
		temp_f = (uint32_t)(1.8f * (float)temp_c + 32.f);
		sprintf(s_temp, "%s :  %i C  /  %i F", cpu_rsx ? "RSX" : "CPU", temp_c, temp_f);
		print_text(ctx.menu, CANVAS_W - ((h==720) ? 450 : 300), 64, s_temp);
	}

	// set frame buffer
	set_texture_direct(ctx.menu, 0, 900, CANVAS_W, 96, CANVAS_W);
	memcpy((uint8_t *)ctx.menu, (uint8_t *)(ctx.canvas)+900*CANVAS_W*4, CANVAS_W*96*4);
	set_frame(slot, 0xffc00000ffc00000);
}

static void load_data(void)
{
	int fd;

	char *err_msg = (char*)"There is no content.";
	if(get_vsh_plugin_slot_by_name("WWWD") >= 7) {games=menu_mode=0, err_msg = (char*)"webMAN is not loaded."; goto exit_load;}

	char filename[64];
	if(menu_mode) sprintf(filename, "%s/slaunch%i.bin", WMTMP, menu_mode); else sprintf(filename, "%s/slaunch.bin", WMTMP);

	uint64_t size = file_exists(filename);
	if(menu_mode && (size == 0)) {menu_mode = modeALL; cur_game = cur_game_; goto exit_load;}
	if(menu_mode == 0) cur_game = cur_game_;

	games=size/sizeof(_slaunch);

	if(games>=MAX_GAMES) games=MAX_GAMES-1;
	if(cur_game>=games) _cur_game=cur_game=0;

	size_t name_len = 127;

reload:
	reset_heap();

	if(games && (cellFsOpen(filename, CELL_FS_O_RDONLY, &fd, NULL, 0) == CELL_FS_SUCCEEDED))
	{
		// load game list in MC memory
		slaunch = (_slaunch*)mem_alloc((games+1)*sizeof(_slaunch));

		cellFsRead(fd, (void *)slaunch, sizeof(_slaunch)*games, NULL);
		cellFsClose(fd);

		bool old_struct = (slaunch[0].type == TYPE_ALL);

		if(old_struct) name_len = 127; else name_len = 111;

		if(!menu_mode)
		{
			uint32_t ngames = games;

			if(gmode)
			{
				// filter games
				if(old_struct)
					for(int32_t n=games-1; n >= 0; n--)
					{
						if(
							((gmode == modePS3) && (!strstr(slaunch[n].path, "PS3") && !strstr(slaunch[n].path, "/GAME"))) ||
							((gmode == modePSX) && (!strstr(slaunch[n].path, "PSX"))) ||
							((gmode == modePS2) && (!strstr(slaunch[n].path, "PS2"))) ||
							((gmode == modePSP) && (!strstr(slaunch[n].path, "PSP"))) ||
							((gmode == modeDVD) && (!strstr(slaunch[n].path, "BDISO") && !strstr(slaunch[n].path, "DVDISO"))) ||
							((gmode == modeROM) && (!strstr(slaunch[n].path, "ROMS")))
						  )
							{memset(slaunch[n].name, 0xFF, name_len); ngames--;}
					}
				else
					for(int32_t n=games-1; n >= 0; n--)
					{
						if(slaunch[n].type != gmode) {memset(slaunch[n].name, 0xFF, name_len); ngames--;}
					}

				if(ngames == 0) {gmode++; if(gmode > modeLAST) gmode = modeALL; goto reload;}
			}

			if(dmode)
			{
				int8_t dlen = strlen(drives[dmode-1]);
				for(int32_t n=games-1; n >= 0; n--)
				{
					if((slaunch[n].name[0] & 0xFF) == 0xFF) continue; // skip filtered content
					if(((dmode == NTFS) && !strstr(slaunch[n].path+11, drives[dmode-1])) || ((dmode != NTFS) && memcmp(slaunch[n].path+11, drives[dmode-1], dlen))) {memset(slaunch[n].name, 0xFF, name_len); ngames--;}
				}
				if(ngames == 0) {dmode++; if(dmode > devsLAST) dmode = modeALL; goto reload;}
			}

			// sort game list
			if(games>1)
			{
				_slaunch swap;
				for(uint32_t n=0; n<(games-1); n++)
				{
					for(uint32_t m=(n+1); m<games; m++)
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

			games = ngames;
		}
	}
	else
		games = 0;

exit_load:

	if(games)
	{
		menu_running = 1;
		draw_page(cur_game);
	}
	else	// no content
	{
		if(menu_mode) {menu_mode = 0; return;}

		// no games - show "no content" message
		ctx.fg_color=0xffc0c0c0;
		set_font(32.f, 32.f, 1.5f, 1); print_text(ctx.canvas, CENTER_TEXT, 520, err_msg);
		flip_frame((uint64_t*)ctx.canvas);
		sys_timer_sleep(2);
		return_to_xmb();
	}
}

static void reload_data(uint32_t curpad)
{
	play_rco_sound("snd_cursor");

	if(curpad & PAD_L2) gmode=dmode=modeALL;

	_cur_game=cur_game=0;

	load_data();
}

//////////////////////////////////////////////////////////////////////
//                       START VSH MENU                             //
//////////////////////////////////////////////////////////////////////

static void start_VSH_Menu(void)
{

	rsx_fifo_pause(1);

	int32_t ret, mem_size;

	uint64_t gamelist_size = file_exists(WMTMP "/slaunch.bin");

//	mem_size = (((CANVAS_W * CANVAS_H * 4) + (CANVAS_W * 96 * 4) + (FONT_CACHE_MAX * 32 * 32) + (MAX_WH4)*2 + ((MAX_GAMES+1)*sizeof(_slaunch))) + MB(1)) / MB(1);
	mem_size = ((CANVAS_W * CANVAS_H * 4) + (CANVAS_W * 96 * 4) + (FONT_CACHE_MAX * 32 * 32) + (MAX_WH4)   + (gamelist_size) + MB(1)) / MB(1);

	// create VSH Menu heap memory from memory container 1("app")
	ret = create_heap(mem_size);

	if(ret) {rsx_fifo_pause(0); return;}

	w = getDisplayWidth();	// display width
	h = getDisplayHeight();

	// initialize VSH Menu graphic
	init_graphic();

	// stop vsh pad
	start_stop_vsh_pad(0);

	// load background
	load_img_bitmap(0, "/dev_flash/vsh/resource/explore/icon/cinfo-bg-storegame.jpg");

	// load game list
	load_data();
}

//////////////////////////////////////////////////////////////////////
//                       STOP VSH MENU                              //
//////////////////////////////////////////////////////////////////////

static void stop_VSH_Menu(void)
{
	// menu off
	menu_mode = menu_running = 0;

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

////////////////////////////////////////////////////////////////////////
//                      PLUGIN MAIN PPU THREAD                        //
////////////////////////////////////////////////////////////////////////
static void slaunch_thread(uint64_t arg)
{
	uint32_t oldpad = 0, curpad = 0;
	uint32_t init_delay=0;
	CellPadData pdata;

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
				if( (pdata.button[CELL_PAD_BTN_OFFSET_DIGITAL1] == CELL_PAD_CTRL_START) &&
					(pdata.button[CELL_PAD_BTN_OFFSET_DIGITAL2] == 0))
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
						sys_timer_sleep(2);
					}
				}
			}
		}
		else // menu is running
		{
			while(menu_running && running)
			{
				// check only pad ports 0 and 1
				for(int32_t port=0; port<2; port++)
					{MyPadGetData(port, &pdata); curpad = (pdata.button[2] | (pdata.button[3] << 8)); if(curpad && (pdata.len > 0)) break;}  // use MyPadGetData() during VSH menu

				if(curpad)
				{
					if(curpad==oldpad)	// key-repeat
					{
						init_delay++;
						if(init_delay<=40) continue;
						else sys_timer_usleep(40000);
					}
					else
						init_delay=0;

					oldpad = curpad;
					_cur_game=cur_game;

						 if(curpad & PAD_DOWN)	cur_game+=5;
					else if(curpad & PAD_UP)	cur_game-=5;
					else if(curpad & PAD_RIGHT) cur_game+=1;
					else if(curpad & PAD_LEFT)	if(!cur_game) cur_game=games-1; else cur_game-=1;
					else if(curpad & PAD_R1)	cur_game+=10;
					else if(curpad & PAD_L1)	if(!cur_game) cur_game=games-1; else cur_game-=10;

					else if((curpad & PAD_TRIANGLE) && !menu_mode) {gmode++; if(gmode>modeLAST) gmode=modeALL; reload_data(curpad);}
					else if((curpad & PAD_SQUARE)   && !menu_mode) {dmode++; if(dmode>devsLAST) dmode=modeALL; reload_data(curpad);}

					else if(curpad & PAD_R3)	{return_to_xmb(); send_wm_request("/popup.ps3"); break;}
					else if(curpad & PAD_L3)	{return_to_xmb(); send_wm_request("/refresh.ps3"); break;}
					else if(curpad & PAD_SELECT)
					{
						play_rco_sound("snd_cursor");
						if(menu_mode == 0) cur_game_ = cur_game;
						menu_mode++; if(menu_mode > 1) menu_mode = modeALL;
						reload_data(curpad);
						sys_timer_usleep(40000);
						continue;
					}

					else if(curpad & PAD_CIRCLE)					// back to XMB
					{
						if(menu_mode) {menu_mode=0; reload_data(curpad); cur_game = cur_game;}
						else
						{
							curpad=oldpad=0;
							play_rco_sound("snd_cancel");
							stop_VSH_Menu(); /*return_to_xmb();*/
						}
						break;
					}
					else if(curpad & PAD_CROSS && games)			// execute action & return to XMB
					{
						play_rco_sound("snd_system_ok");
						for(uint8_t u=0;u<6;u++)
						{
							set_frame(1 + cur_game % 10, (u & 1) ? 0xffff0000ffff0000 : 0xff400000ff400000);
							sys_timer_usleep(75000);
						}

						char path[128];
						sprintf(path, "%s", slaunch[cur_game].path);

						return_to_xmb();
						send_wm_request(path);
						break;
					}

					if(cur_game!=_cur_game && games)
					{
						play_rco_sound("snd_cursor");
						tick=0xc0;
						set_backdrop((1+_cur_game%10), 1);
						if(cur_game>=games) cur_game=0;
						if((cur_game/10)*10 != (_cur_game/10)*10)
							draw_page(cur_game);
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
