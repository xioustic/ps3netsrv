#include "include/misc.h"
#include "include/vsh_exports.h"

/*
uint32_t load_rco_texture(uint32_t* texture, const char *plugin, const char *texture_name)
{
	return (paf_3A8454FC(texture, (paf_F21655F3(plugin)), texture_name));
}
*/

/***********************************************************************
* play a rco sound file
*
* const char *sound  = sound recource file in rco we would play
***********************************************************************/
void play_rco_sound(const char *sound)
{
	paf_B93AFE7E((paf_F21655F3("system_plugin")), sound, 1, 0);
}

/***********************************************************************
* ring buzzer
***********************************************************************/
/*
void buzzer(uint8_t mode)
{
	uint16_t param = 0;

	switch(mode)
	{
		case 1: param = 0x0006; break;		// single beep
		case 2: param = 0x0036; break;		// double beep
		case 3: param = 0x01B6; break;		// triple beep
		case 4: param = 0x0FFF; break;		// continuous beep, gruesome!!!
	}

	system_call_3(392, 0x1007, 0xA, param);
}
*/

/***********************************************************************
* peek & poke
***********************************************************************/

uint64_t peekq(uint64_t addr)
{
	system_call_1(6, addr);
	return_to_user_prog(uint64_t);
}
/*
uint64_t pokeq(uint64_t addr, uint64_t value)
{
	system_call_2(7, addr, value);
	return_to_user_prog(uint64_t);
}

uint64_t poke_lv1(uint64_t addr, uint64_t value)
{
	system_call_2(9, addr, value);
	return_to_user_prog(uint64_t);
}

uint64_t peek_lv1(uint64_t addr)
{
	system_call_1(8, addr);
	return_to_user_prog(uint64_t);
}
*/


/***********************************************************************
* file exists
***********************************************************************/
uint64_t file_len(const char* path)
{
	struct CellFsStat s;
	s.st_size=0;
	cellFsStat(path, &s);
	return(s.st_size);
}

////////////////////////////////////////////////////////////////////////
//			SYS_PPU_THREAD_EXIT, DIRECT OVER SYSCALL				//
////////////////////////////////////////////////////////////////////////
void _sys_ppu_thread_exit(uint64_t val)
{
	system_call_1(41, val);
}

////////////////////////////////////////////////////////////////////////
//						 GET MODULE BY ADDRESS						//
////////////////////////////////////////////////////////////////////////
sys_prx_id_t prx_get_module_id_by_address(void *addr)
{
	system_call_1(461, (uint64_t)(uint32_t)addr);
	return (int32_t)p1;
}

////////////////////////////////////////////////////////////////////////
//                      GET CPU & RSX TEMPERATURES                  //
////////////////////////////////////////////////////////////////////////
void get_temperature(uint32_t _dev, uint32_t *_temp)
{
	system_call_2(383, (uint64_t)(uint32_t) _dev, (uint64_t)(uint32_t) _temp); *_temp >>= 24;
}


#define SC_COBRA_SYSCALL8			 				(8)
#define SYSCALL8_OPCODE_PS3MAPI			 			0x7777
#define PS3MAPI_OPCODE_GET_VSH_PLUGIN_INFO			0x0047

unsigned int get_vsh_plugin_slot_by_name(const char *name)
{
	char tmp_name[30];
	char tmp_filename[256];
	unsigned int slot;

	for(slot = 1; slot < 7; slot++)
	{
		memset(tmp_name, 0, sizeof(tmp_name));
		memset(tmp_filename, 0, sizeof(tmp_filename));

		{system_call_5(SC_COBRA_SYSCALL8, SYSCALL8_OPCODE_PS3MAPI, PS3MAPI_OPCODE_GET_VSH_PLUGIN_INFO, (uint64_t)slot, (uint64_t)(uint32_t)tmp_name, (uint64_t)(uint32_t)tmp_filename); }

		if(strstr(tmp_name, name)) break;
	}
	return slot;
}

int load_plugin_by_id(int id, void *handler)
{
	xmm0_interface = (xmb_plugin_xmm0 *)paf_23AFB290((uint32_t)paf_F21655F3("xmb_plugin"), 0x584D4D30);
	return xmm0_interface->LoadPlugin3(id, handler,0);
}

extern uint8_t web_page;

void web_browser(void)
{
	webbrowser_interface = (webbrowser_plugin_interface *)paf_23AFB290((uint32_t)paf_F21655F3("webbrowser_plugin"), 1);
	if(webbrowser_interface) webbrowser_interface->PluginWakeupWithUrl(web_page ? "http://127.0.0.1/" : "http://127.0.0.1/setup.ps3");
}

/*
int unload_plugin_by_id(int id, void *handler)
{
	xmm0_interface = (xmb_plugin_xmm0 *)paf_23AFB290((uint32_t)paf_F21655F3("xmb_plugin"), 0x584D4D30);//'XMM0'
	if(xmm0_interface) return xmm0_interface->Shutdown(id, handler, 1); else return 0;
}

void web_browser_stop(void)
{
	webbrowser_interface = (webbrowser_plugin_interface *)paf_23AFB290((uint32_t)paf_F21655F3("webbrowser_plugin"), 1);
	if(webbrowser_interface) webbrowser_interface->Shutdown();
}
*/
