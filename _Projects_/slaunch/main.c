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
typedef struct
{
	char path[128];
	char icon[128];
	char name[128];
} __attribute__((packed)) _slaunch;

static _slaunch *slaunch = NULL;

static uint32_t games = 0;

static sys_ppu_thread_t slaunch_tid = -1;
static int32_t running = 1;
static uint8_t menu_running = 0;	// vsh menu off(0) or on(1)

static void return_to_xmb(void);
int32_t slaunch_start(uint64_t arg);
int32_t slaunch_stop(void);

static void finalize_module(void);
static void slaunch_stop_thread(uint64_t arg);
static void slaunch_thread(uint64_t arg);
void draw_selection(uint32_t game_idx);

static uint8_t gmode = 0;
static uint8_t cpu_rsx = 0;
static uint32_t frame = 0;

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
		return -1;
	}

	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		return -1;
	}

	return s;
}

static void sclose(int *socket_e)
{
	if(*socket_e != -1)
	{
		shutdown(*socket_e, SHUT_RDWR);
		socketclose(*socket_e);
		*socket_e = -1;
	}
}

static void send_wm_request(const char *cmd)
{
	// send command
	int conn_s = -1;
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
	if(!games) return;
	uint8_t slot = 0;
	int px=48, py=90;
	uint32_t i, j;

	ctx.canvas = ctx.img[0].addr;
	flip_frame((uint64_t*)ctx.img[0].addr);
	memcpy((uint8_t *)ctx.menu, (uint8_t *)(ctx.canvas)+900*CANVAS_W*4, CANVAS_W*96*4);

	set_textbox(0xff808080ff808080, 0, 890, CANVAS_W, 2);
	set_textbox(0xff808080ff808080, 0, 1000, CANVAS_W, 2);

	j=(game_idx/10)*10;
	for(i=j;(slot<10&&i<games);i++)
	{
		slot++;
		load_img_bitmap(slot, slaunch[i].icon);
		py=((i-j)/5)*400+90+(300-ctx.img[slot].h)/2;
		set_texture(slot, ((px+(320-ctx.img[slot].w)/2)/2)*2, py);
		set_backdrop(slot, 0);
		px+=(320+48); if(px>1600) px=48;
	}
	draw_selection(game_idx);
}

void draw_selection(uint32_t game_idx)
{
	uint8_t slot = 1 + game_idx % 10;
	char one_of[32];

	// game name
	ctx.fg_color=0xffc0c0c0;
	set_font(32.f, 32.f, 1.5f, 1); print_text(ctx.menu, -1, 0, slaunch[game_idx].name );

	// game path
	ctx.fg_color=0xff808080;
	set_font(24.f, 16.f, 1.0f, 1); print_text(ctx.menu, -1, 40, slaunch[game_idx].path+10 );

	// game index
	ctx.fg_color=0xffA0A0A0;
	sprintf(one_of, "%i / %i", game_idx+1, games);
	set_font(20.f, 20.f, 1.5f, 1); print_text(ctx.menu, -1, 64, one_of );

	if(gmode)
		print_text(ctx.menu, 80, 64, (gmode == 1) ? "PS3" : (gmode == 2) ? "PSX" : (gmode == 3) ? "PS2" : (gmode == 4) ? "PSP" : (gmode == 5) ? "BD/DVD" : "ROMS");

	// temperature
	char s_temp[64];
	uint32_t temp_c = 0, temp_f = 0;
	get_temperature(cpu_rsx, &temp_c);
	temp_f = (uint32_t)(1.8f * (float)temp_c + 32.f);
	sprintf(s_temp, "%s :  %i C  /  %i F", cpu_rsx ? "RSX" : "CPU", temp_c, temp_f);
	print_text(ctx.menu, CANVAS_W - 300, 64, s_temp);

	set_texture_direct(ctx.menu, 0, 900, CANVAS_W, 96, CANVAS_W);
	memcpy((uint8_t *)ctx.menu, (uint8_t *)(ctx.canvas)+900*CANVAS_W*4, CANVAS_W*96*4);

	set_frame(slot);
}

static void load_data(void)
{
	games=(file_exists(WMTMP "/slaunch.bin"))/sizeof(_slaunch);
	if(games>MAX_GAMES) games=MAX_GAMES;

reload:

	load_img_bitmap(0, "/dev_flash/vsh/resource/explore/icon/cinfo-bg-storegame.jpg");

	int fd = 0;
	if(games && (cellFsOpen((char*)WMTMP "/slaunch.bin", CELL_FS_O_RDONLY, &fd, NULL, 0) == CELL_FS_SUCCEEDED))
	{
		if(slaunch) free(slaunch);
		slaunch = (_slaunch*)malloc((games+1)*sizeof(_slaunch));

		_slaunch swap;
		cellFsRead(fd, (void *)slaunch, sizeof(_slaunch)*games, NULL);
		cellFsClose(fd);

		uint32_t ngames = games;

		if(gmode)
		{
			for(int32_t n=games-1; n >= 0; n--)
			{
				if( (gmode == 1 && (!strstr(slaunch[n].path, "PS3") && !strstr(slaunch[n].path, "/GAME"))) ||
					(gmode == 2 && (!strstr(slaunch[n].path, "PSX"))) ||
					(gmode == 3 && (!strstr(slaunch[n].path, "PS2"))) ||
					(gmode == 4 && (!strstr(slaunch[n].path, "PSP"))) ||
					(gmode == 5 && (!strstr(slaunch[n].path, "BDISO") && !strstr(slaunch[n].path, "DVDISO"))) ||
					(gmode == 6 && (!strstr(slaunch[n].path, "ROMS"))) )
					{memset(slaunch[n].name, 0xFF, sizeof(_slaunch)); ngames--;}
			}

			if(ngames == 0) {gmode++; if(gmode > 6) gmode = 0; goto reload;}
		}

		// sort games
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
		games = ngames;
	}
	else
		games = 0;

	// no content
	if(!games)
	{
		ctx.canvas = ctx.img[0].addr;
		flip_frame((uint64_t*)ctx.img[0].addr);
		memcpy((uint8_t *)ctx.menu, (uint8_t *)(ctx.canvas)+900*CANVAS_W*4, CANVAS_W*96*4);
		set_textbox(0xff808080ff808080, 0, 890, CANVAS_W, 2);
		set_textbox(0xff808080ff808080, 0, 1000, CANVAS_W, 2);
		ctx.fg_color=0xffc0c0c0;
		set_font(32.f, 32.f, 1.5f, 1); print_text(ctx.canvas, -1, 520, "There is no content.");
		flip_frame((uint64_t*)ctx.img[0].addr);
		sys_timer_sleep(2);
		return_to_xmb();
	}
}

//////////////////////////////////////////////////////////////////////
//                       START VSH MENU                             //
//////////////////////////////////////////////////////////////////////

static void start_VSH_Menu(bool fx)
{

	if(fx) rsx_fifo_pause(1);

	int32_t ret, mem_size;

	//CANVAS_W = getDisplayWidth(), CANVAS_H = getDisplayHeight();

	// create VSH Menu heap memory from memory container 1("app")
	mem_size = (((CANVAS_W * CANVAS_H * 4 * 4) + (FONT_CACHE_MAX * 32 * 32)) + MB(2)) / MB(1);
	ret = create_heap(mem_size);

	if(ret) return;

	// initialize VSH Menu graphic
	init_graphic(fx);

	if(fx) dim_bg();

	// stop vsh pad
	start_stop_vsh_pad(0);

	load_data();

	// set menu_running on
	menu_running = 1;
}

//////////////////////////////////////////////////////////////////////
//                       STOP VSH MENU                              //
//////////////////////////////////////////////////////////////////////

static void stop_VSH_Menu(bool fx)
{
	// menu off
	menu_running = 0;

	// unbind renderer and kill font-instance
	font_finalize();

	// free heap memory
	destroy_heap();

	// continue rsx rendering
	if(fx) rsx_fifo_pause(0);

	// restart vsh pad
	start_stop_vsh_pad(1);
}

static void return_to_xmb(void)
{
	dump_bg();
	dim_bg();
	if(slaunch) free(slaunch);
	stop_VSH_Menu(true);
}

static void refresh_VSH_Menu(void)
{
	stop_VSH_Menu(false);
	start_VSH_Menu(false);
}


////////////////////////////////////////////////////////////////////////
//                      PLUGIN MAIN PPU THREAD                        //
////////////////////////////////////////////////////////////////////////
static void slaunch_thread(uint64_t arg)
{
	uint32_t oldpad = 0, curpad = 0;
	uint32_t init_delay=0;
	CellPadData pdata;

	uint32_t cur_game=0, _cur_game=0;

	while(running)
	{
		if(!menu_running)												// VSH menu is not running, normal XMB execution
		{
			sys_timer_sleep(1);
			pdata.len = 0;
			for(uint8_t p = 0; p < 2; p++)
				if(cellPadGetData(p, &pdata) == CELL_PAD_OK && pdata.len > 0) break;

			if(pdata.len)					// if pad data and we are on XMB
			{
				if((pdata.button[CELL_PAD_BTN_OFFSET_DIGITAL1] == CELL_PAD_CTRL_START) &&
					(pdata.button[CELL_PAD_BTN_OFFSET_DIGITAL2] == 0))
				{
					if(vshmain_EB757101() == 0)
					{
						start_VSH_Menu(true);
						if(games) draw_page(cur_game);
						init_delay=0;
					}
				}
			}
		}
		else // menu is running
		{
			while(1)
			{
				for(int32_t port=0; port<2; port++)
					{MyPadGetData(port, &pdata); curpad = (pdata.button[2] | (pdata.button[3] << 8)); if(curpad && (pdata.len > 0)) break;}  // use MyPadGetData() during VSH menu

				if(curpad)
				{
					if(curpad==oldpad)
					{
						init_delay++;
						if(init_delay<=40) continue;
						else sys_timer_usleep(80000);
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

					else if(curpad & PAD_TRIANGLE){if(curpad & PAD_L2) gmode=0; else gmode++; if(gmode>6) gmode=0; cur_game=0, _cur_game=0; refresh_VSH_Menu(); if(games) draw_page(cur_game);}

					else if(curpad & PAD_R3)	{send_wm_request("/popup.ps3"); return_to_xmb(); break;}
					else if(curpad & PAD_L3)	{send_wm_request("/refresh.ps3"); return_to_xmb(); break;}
					else if(curpad & PAD_SELECT){send_wm_request("/browser.ps3/setup.ps3"); return_to_xmb(); break;}
					else if(curpad & PAD_CROSS) {send_wm_request(slaunch[cur_game].path); return_to_xmb(); break;}
					else if(curpad & PAD_CIRCLE){curpad=oldpad=0; return_to_xmb(); break;}
					if(cur_game!=_cur_game)
					{
						set_backdrop((1+_cur_game%10), 1);
						if(cur_game>=games) cur_game=0;
						if((cur_game/10)*10 != (_cur_game/10)*10)
							draw_page(cur_game);
						else
							draw_selection(cur_game);
					}
				}
				else {init_delay=0, oldpad=0, frame++; if(frame > 300) {frame = 0, cpu_rsx ^= 1; draw_selection(cur_game);}}
			}
		}
	}

	finalize_module();

	uint64_t exit_code;
	sys_ppu_thread_join(slaunch_tid, &exit_code);

	sys_ppu_thread_exit(0);
}

/***********************************************************************
* start thread
***********************************************************************/
int32_t slaunch_start(uint64_t arg)
{
	sys_ppu_thread_create(&slaunch_tid, slaunch_thread, 0, 3000, 0x6000, 1, THREAD_NAME);

	_sys_ppu_thread_exit(0);
	return SYS_PRX_RESIDENT;
}

/***********************************************************************
* stop thread
***********************************************************************/
static void slaunch_stop_thread(uint64_t arg)
{
	if(menu_running) stop_VSH_Menu(true);

	running = 0;
	sys_timer_usleep(500000); //Prevent unload too fast

	uint64_t exit_code;

	if(slaunch_tid != (sys_ppu_thread_t)-1)
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

	sys_timer_usleep(500000); // 0.5s

	finalize_module();

	_sys_ppu_thread_exit(0);
	return SYS_PRX_STOP_OK;
}
