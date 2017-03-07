#ifndef __MISC_H__
#define __MISC_H__

#include <sys/syscall.h>
#include <cell/cell_fs.h>
#include <sys/prx.h>

//uint32_t load_rco_texture(uint32_t* texture, const char *plugin, const char *texture_name);
void play_rco_sound(const char *sound);
//void buzzer(uint8_t mode);

uint64_t peekq(uint64_t addr);
//uint64_t pokeq(uint64_t addr, uint64_t value);
//uint64_t poke_lv1(uint64_t addr, uint64_t value);
//uint64_t peek_lv1(uint64_t addr);

void _sys_ppu_thread_exit(uint64_t val);
sys_prx_id_t prx_get_module_id_by_address(void *addr);

void get_temperature(uint32_t _dev, uint32_t *_temp);
unsigned int get_vsh_plugin_slot_by_name(const char *name);

int load_plugin_by_id(int id, void *handler);
void web_browser(void);
//int unload_plugin_by_id(int id, void *handler);
//void web_browser_stop(void);

uint64_t file_len(const char* path);

#endif // __MISC_H__

#define file_exists  file_len
