#ifndef __MISC_H__
#define __MISC_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cell/pad.h>

#include <sys/types.h>
#include <sys/memory.h>
#include <sys/syscall.h>

#define _ES32(v)((uint32_t)(((((uint32_t)v) & 0xFF000000) >> 24) | \
							((((uint32_t)v) & 0x00FF0000) >> 8 ) | \
							((((uint32_t)v) & 0x0000FF00) << 8 ) | \
							((((uint32_t)v) & 0x000000FF) << 24)))


int32_t rsx_fifo_pause(uint8_t pause);

void play_rco_sound(const char *plugin, const char *sound);
void buzzer(uint8_t mode);

uint64_t lv2peek(uint64_t addr);
uint64_t lv2poke(uint64_t addr, uint64_t value);
uint64_t lv1peek(uint64_t addr);
uint64_t lv1poke(uint64_t addr, uint64_t value);

#endif // __MISC_H__
