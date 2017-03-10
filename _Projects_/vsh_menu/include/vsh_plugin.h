static uint8_t wm_unload = 0;

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

#define SC_COBRA_SYSCALL8 8
#define SYSCALL8_OPCODE_LOAD_VSH_PLUGIN          0x1EE7
#define SYSCALL8_OPCODE_UNLOAD_VSH_PLUGIN        0x364F
#define SYSCALL8_OPCODE_PS3MAPI                  0x7777
#define PS3MAPI_OPCODE_GET_VSH_PLUGIN_INFO       0x0047
#define PS3MAPI_OPCODE_UNLOAD_VSH_PLUGIN         0x0046

static int cobra_load_vsh_plugin(unsigned int slot, char *path, void *arg, uint32_t arg_size)
{
	system_call_5(SC_COBRA_SYSCALL8, SYSCALL8_OPCODE_LOAD_VSH_PLUGIN, slot, (uint64_t)(uint32_t)path, (uint64_t)(uint32_t)arg, arg_size);
	return (int)p1;
}

static int cobra_unload_vsh_plugin(unsigned int slot)
{
	system_call_2(SC_COBRA_SYSCALL8, SYSCALL8_OPCODE_UNLOAD_VSH_PLUGIN, slot);
	return (int)p1;
}

static unsigned int get_vsh_plugin_slot_by_name(const char *name, bool unload)
{
	char tmp_name[30];
	char tmp_filename[256];
	unsigned int slot, unused_slot = 0;

	for (slot = 1; slot < 7; slot++)
	{
		memset(tmp_name, 0, sizeof(tmp_name));
		memset(tmp_filename, 0, sizeof(tmp_filename));
		{system_call_5(SC_COBRA_SYSCALL8, SYSCALL8_OPCODE_PS3MAPI, PS3MAPI_OPCODE_GET_VSH_PLUGIN_INFO, (uint64_t)slot, (uint64_t)(uint32_t)tmp_name, (uint64_t)(uint32_t)tmp_filename); }

		if(strstr(tmp_filename, name) || !strcmp(tmp_name, name))
		{
			if(strstr(tmp_filename, "webftp_server"))
			{
				if(unload) {if(wm_unload) continue; send_wm_request("/quit.ps3"); return 0;}
				return wm_unload ? 0 : slot;
			}
			else
			if(unload)
			{
				cobra_unload_vsh_plugin(slot);
				return 0;
			}
			else
				return slot;
		}

		if(unused_slot == 0 && strlen(tmp_name)==0) unused_slot = slot;
	}
	return unload ? unused_slot : 0;
}
