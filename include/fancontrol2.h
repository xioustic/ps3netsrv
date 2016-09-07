#ifdef PKG_HANDLER
typedef struct {
   u32 magic; // 0x53434500//
   u32 version;
   u16 sdk_type;
   u16 SCE_header_type;
   u32 meta_offset;
   u64 size; // size of sce_hdr + sizeof meta_hdr
   u64 pkg_size;
 } _pkg_header;
#endif

static void poll_thread(uint64_t poll)
{
	/*u8 d0[157];
	u8 d1[157];
	u8 d0t[157];
	u8 d1t[157];
	int u0=0, u1=0;
	char un[128];

	while(working)
	{
		if(u0<128)
		{
			int fd;
			bool toupd0, toupd1;
			cellFsOpen("/dev_hdd0/vsh/task/00000001/d0.pdb", CELL_FS_O_RDONLY, &fd, NULL, 0);
			cellFsRead(fd, (void *)&d0, 157, NULL); cellFsClose(fd);
			cellFsOpen("/dev_hdd0/vsh/task/00000001/d1.pdb", CELL_FS_O_RDONLY, &fd, NULL, 0);
			cellFsRead(fd, (void *)&d1, 157, NULL); cellFsClose(fd);
			toupd0=0;
			toupd1=0;
			for(u8 b=0;b<157;b++)
			{
				if(d0[b]!=d0t[b]) toupd0=1;
				if(d1[b]!=d1t[b]) toupd1=1;
				d0t[b]=d0[b];
				d1t[b]=d1[b];
			}
			if(toupd0)
			{
				u0++;
				sprintf(un, "/dev_hdd0/tmp/d0-%03i.bin", u0);
				cellFsOpen(un, CELL_FS_O_CREAT|CELL_FS_O_WRONLY, &fd, NULL, 0);
				cellFsWrite(fd, (void *)d0, 157, NULL);
				cellFsClose(fd);
			}
			if(toupd1)
			{
				u1++;
				sprintf(un, "/dev_hdd0/tmp/d1-%03i.bin", u1);
				cellFsOpen(un, CELL_FS_O_CREAT|CELL_FS_O_WRONLY, &fd, NULL, 0);
				cellFsWrite(fd, (void *)d1, 157, NULL);
				cellFsClose(fd);
			}
		}
	}
	*/

	u8 to = 0;
	u8 sec = 0;
	u32 t1 = 0, t2 = 0;
	u8 lasttemp = 0;
	u8 stall = 0;
	const u8 step = 3;
	const u8 step_up = 5;
	//u8 step_down=2;
	u8 smoothstep = 0;
	int delta = 0;
	char msg[256];

	u8 show_persistent_popup = 0; // combos.h

	old_fan = 0;
	while(working)
	{
		if(fan_ps2_mode) /* skip dynamic fan control */; else

		// dynamic fan control
		if(max_temp)
		{
			t1 = t2 = 0;
			get_temperature(0, &t1); // CPU: 3E030000 -> 3E.03°C -> 62.(03/256)°C
			sys_timer_usleep(300000);

			get_temperature(1, &t2); // RSX: 3E030000 -> 3E.03°C -> 62.(03/256)°C
			sys_timer_usleep(200000);

			if(!max_temp || fan_ps2_mode) continue; // if fan mode was changed to manual by another thread while doing usleep

			if(t2 > t1) t1 = t2;

			if(!lasttemp) lasttemp = t1;

			delta = (lasttemp - t1);

			lasttemp = t1;

			if((t1 >= max_temp) || (t1 >= MAX_TEMPERATURE))
			{
				if(delta  < 0) fan_speed += 2;
				if(delta == 0 && t1 != (max_temp-1)) fan_speed++;
				if(delta == 0 && t1 >= (max_temp+1)) fan_speed+=(2 + (t1 - max_temp));
				if(delta  > 0)
				{
					smoothstep++;
					if(smoothstep > 1)
					{
						fan_speed--;
						smoothstep=0;
					}
				}
				if(t1 >= MAX_TEMPERATURE)	 fan_speed+=step_up;
				if(delta< 0 && (t1 - max_temp)>=2) fan_speed+=step_up;
			}
			else
			{
				if(delta< 0 && t1>=(max_temp-1)) fan_speed+=2;
				if(delta==0 && t1<=(max_temp-2))
				{
					smoothstep++;
					if(smoothstep>1)
					{
						fan_speed--;
						if(t1<=(max_temp-3)) {fan_speed--; if(fan_speed>0xA8) fan_speed--;} // 66%
						if(t1<=(max_temp-5)) {fan_speed--; if(fan_speed>0x80) fan_speed--;} // 50%
						smoothstep=0;
					}
				}
				//if(delta==0 && t1>=(max_temp-1)) fan_speed++;
				if(delta> 0)
				{
					//smoothstep++;
					//if(smoothstep)
					{
						fan_speed--;
						if(t1 <= (max_temp-3)) {fan_speed--; if(fan_speed>0xA8) fan_speed--;} // 66%
						if(t1 <= (max_temp-5)) {fan_speed--; if(fan_speed>0x80) fan_speed--;} // 50%
						smoothstep=0;
					}
				}
			}

			if(t1 > 76 && old_fan < 0x43) fan_speed++; // <26%
			if(t1 >= MAX_FANSPEED && fan_speed < 0xB0) {old_fan = 0, fan_speed = 0xB0;} // <69%

			if(fan_speed < ((webman_config->minfan*255)/100)) fan_speed = (webman_config->minfan*255)/100;
			if(fan_speed > MAX_FANSPEED) fan_speed = MAX_FANSPEED;

			//sprintf(debug, "OFAN: %x | CFAN: %x | TEMP: %i | STALL: %i\r\n", old_fan, fan_speed, t1, stall);	ssend(data_s, mytxt);
			//if(abs(old_fan-fan_speed)>=0x0F || stall>35 || (abs(old_fan-fan_speed) /*&& webman_config->aggr*/))
			if(old_fan!=fan_speed || stall>35)
			{
				//if(t1>76 && fan_speed<0x50) fan_speed=0x50;
				//if(t1>77 && fan_speed<0x58) fan_speed=0x58;
				if(t1>78 && fan_speed < 0x50) fan_speed += 2; // <31%
				if(old_fan!=fan_speed)
				{
					old_fan = fan_speed;
					fan_control(fan_speed, 1);
					//sprintf(debug, "OFAN: %x | CFAN: %x | TEMP: %i | SPEED APPLIED!\r\n", old_fan, fan_speed, t1); ssend(data_s, mytxt);
					stall=0;
				}
			}
			else
				if( (old_fan > fan_speed) && ((old_fan - fan_speed) > 8) && (t1 < (max_temp-3)) )
					stall++;
		}

		// poll combos for 3 seconds
		#include "combos.h"

		// Overheat control (over 83°C)
		to++;
		if(to == 20)
		{
			get_temperature(0, &t1); // CPU
			get_temperature(1, &t2); // RSX

			if(t1 > (MAX_TEMPERATURE-2) || t2 > (MAX_TEMPERATURE-2))
			{
				if(!webman_config->warn)
				{
					sprintf(msg, "%s\n CPU: %i°C   RSX: %i°C", STR_OVERHEAT, t1, t2);
					show_msg(msg);
					sys_timer_sleep(2);
				}
				if((t1 > MAX_TEMPERATURE) || (t2 > MAX_TEMPERATURE))
				{
					if(!max_temp) max_temp = (MAX_TEMPERATURE - 3);
					if(fan_speed < 0xB0) fan_speed = 0xB0; // 69%
					else
						if(fan_speed < MAX_FANSPEED) fan_speed += 8;

					old_fan = fan_speed;
					fan_control(fan_speed, 0);
					if(!webman_config->warn) show_msg((char*)STR_OVERHEAT2);
				}
			}
		}
		if(to > 40) to = 0;

		// detect aprox. time when a game is launched
		if((sec % 10) == 0) {if(IS_ON_XMB) gTick = rTick; else if(gTick.tick == rTick.tick) cellRtcGetCurrentTick(&gTick);}

		// USB Polling
		if(poll == 0 && sec >= 120) // check USB drives each 120 seconds
		{
			uint8_t tmp[2048], f0 = 0;
			uint32_t usb_handle = -1, r;

			for(u8 i = 0; i < 6; i++)
			{
				f0 = (u8)val(drives[i + 1] + 8);
				if(sys_storage_open(((f0 < 6) ? USB_MASS_STORAGE_1(f0) : USB_MASS_STORAGE_2(f0)), 0, &usb_handle, 0) == CELL_OK)
				{
					sys_storage_read(usb_handle, 0, to, 1, tmp, &r, 0);
					sys_storage_close(usb_handle);
					//sprintf(tmp, "/dev_usb00%i: Read %i sectors @ %i offset", f0, r, to); show_msg(tmp);
				}
			}
			sec=0;
		}
		sec+=step;

#ifdef PKG_HANDLER
		// Poll downloaded pkg files
		if((pkg_dcount > 0) && (sec & 1) && (gTick.tick == rTick.tick))
		{
			CellFsDirent entry; u64 read_e; int fd; u16 pkg_count = 0;

			if(cellFsOpendir(TEMP_DOWNLOAD_PATH, &fd) == CELL_FS_SUCCEEDED)
			{
				while((cellFsReaddir(fd, &entry, &read_e) == CELL_FS_SUCCEEDED) && (read_e > 0))
				{
					if(!extcmp(entry.d_name, ".pkg", 4))
					{
						int fdl = 0; char *dlfile = msg; _pkg_header pkg_header;
						sprintf(dlfile, "%s%s", TEMP_DOWNLOAD_PATH, entry.d_name); pkg_count++;
						cellFsChmod(dlfile, MODE);

						if(cellFsOpen(dlfile, CELL_FS_O_RDONLY, &fdl, NULL, 0) == CELL_FS_SUCCEEDED)
						{
							if(cellFsRead(fdl, (void *)&pkg_header, sizeof(pkg_header), NULL) == CELL_FS_SUCCEEDED)
							{
								cellFsClose(fdl);

								struct CellFsStat s;
								if(cellFsStat(dlfile, &s) == CELL_FS_SUCCEEDED && pkg_header.pkg_size == s.st_size)
								{
									char pkgfile[MAX_PATH_LEN];
									sprintf(pkgfile, "%s%s", DEFAULT_PKG_PATH, dlfile + strlen(TEMP_DOWNLOAD_PATH));
									cellFsRename(dlfile, pkgfile);
									pkg_dcount--;

									if(pkg_auto_install) installPKG(pkgfile, msg);
								}
							}
							else
								cellFsClose(fdl);
						}
					}
				}
				cellFsClosedir(fd);

				if(pkg_count == 0) pkg_auto_install = pkg_dcount = 0; // disable polling if no pkg files were found (e.g. changed to background download)
			}
		}
#endif

#ifdef WM_REQUEST
		// Poll requests via local file
		if((sec & 1) && (gTick.tick > rTick.tick)) continue; // slowdown polling if ingame
		if(file_exists(WMREQUEST_FILE))
		{
			loading_html++;
			sys_ppu_thread_t id;
			if(working) sys_ppu_thread_create(&id, handleclient, WM_FILE_REQUEST, THREAD_PRIO, THREAD_STACK_SIZE_64KB, SYS_PPU_THREAD_CREATE_NORMAL, THREAD_NAME_WEB);
		}
#endif
	}

	sys_ppu_thread_exit(0);
}
