#include "include/misc.h"
#include "include/vsh_exports.h"

/***********************************************************************
* pause/continue rsx fifo
*
* uint8_t pause    = pause fifo (1), continue fifo (0)
***********************************************************************/
int32_t rsx_fifo_pause(uint8_t pause)
{
	// lv2 sys_rsx_context_attribute()
	system_call_6(0x2A2, 0x55555555ULL, (uint64_t)(pause ? 2 : 3), 0, 0, 0, 0);

	return (int32_t)p1;
}

/***********************************************************************
* play a rco sound file
*
* const char *plugin = plugin name own the rco file we would play a sound from
* const char *sound  = sound recource file in rco we would play
***********************************************************************/
void play_rco_sound(const char *plugin, const char *sound)
{
	paf_B93AFE7E((paf_F21655F3(plugin)), sound, 1, 0);
}

/***********************************************************************
* ring buzzer
***********************************************************************/
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


/***********************************************************************
* peek & poke
***********************************************************************/
/*
uint64_t lv2peek(uint64_t addr)
{
  system_call_1(6, addr);
  return_to_user_prog(uint64_t);
}

uint64_t lv2poke(uint64_t addr, uint64_t value)
{
  system_call_2(7, addr, value);
  return_to_user_prog(uint64_t);
}

uint64_t lv1peek(uint64_t addr)
{
  system_call_1(8, addr);
  return_to_user_prog(uint64_t);
}

uint64_t lv1poke(uint64_t addr, uint64_t value)
{
  system_call_2(9, addr, value);
  return_to_user_prog(uint64_t);
}
*/