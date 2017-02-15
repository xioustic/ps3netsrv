static void sys_ppu_thread_sleep(uint32_t seconds)
{
	sys_ppu_thread_yield();
	sys_timer_sleep(seconds);
}

#define sys_ppu_thread_usleep    sys_timer_usleep
