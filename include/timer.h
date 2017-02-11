static void sys_ppu_thread_sleep(uint32_t seconds)
{
	sys_ppu_thread_yield();
	sys_timer_sleep(seconds);
}

static void sys_ppu_thread_usleep(uint32_t microseconds)
{
	sys_ppu_thread_yield();
	sys_timer_usleep(microseconds);
}
