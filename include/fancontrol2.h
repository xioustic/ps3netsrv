static void poll_start_play_time(void)
{
	#ifdef OFFLINE_INGAME
	int32_t status = 0;
	#endif

	if(IS_ON_XMB)
	{
		//if(gTick.tick != rTick.tick) vshnet_setUpdateUrl("http://127.0.0.1/dev_hdd0/ps3-updatelist.txt"); // re-apply redirection of custom update file returning to XMB

		gTick = rTick;

	#ifdef OFFLINE_INGAME
		if(net_status >= 0)
		{
			xsetting_F48C0548()->GetSettingNet_enable(&status);
			xsetting_F48C0548()->SetSettingNet_enable(net_status);
			net_status = NONE; if(net_status && !status) show_msg((char*)ONLINE_TAG);
			cellFsUnlink(WMNET_DISABLED);
		}
	#endif
	}
	else if(gTick.tick == rTick.tick) /* the game started a moment ago */
	{
		cellRtcGetCurrentTick(&gTick);

	#ifdef OFFLINE_INGAME
		if((webman_config->spp & 4) || (net_status >= 0))
		{
			get_game_info();

			if(strlen(_game_TitleID) == 9 && View_Find("nas_plugin_module") == 0)
			{
				bool set_net_setatus = true;
				if(net_status < 0)
				{
					char online_title_ids[1024];
					read_file(WMOFFLINE_GAMES, online_title_ids, 1024, 0); // auto-disable network only on these title ids
					if(*online_title_ids) set_net_setatus = strstr(online_title_ids, _game_TitleID);
					else
					{
						read_file(WMONLINE_GAMES, online_title_ids, 1024, 0);  // auto-disable network except on these title ids
						set_net_setatus = (strstr(online_title_ids, _game_TitleID) == NULL);
					}
				}

				if(set_net_setatus)
				{
					xsetting_F48C0548()->GetSettingNet_enable(&status);
					xsetting_F48C0548()->SetSettingNet_enable(net_status < 0 ? 0 : net_status);
					if(status && (net_status <= 0)) {save_file(WMNET_DISABLED, NULL, 0); show_msg((char*)OFFLINE_TAG);}
					net_status = status;
				}
			}
		}
	#endif
	}
}

static void poll_thread(uint64_t poll)
{
	u8 to = 0;
	u8 sec = 0;
	u32 t1 = 0, t2 = 0;
	u8 lasttemp = 0;
	u8 stall = 0;
	const u8 step = 3; // polling every 3 seconds
	const u8 step_up = 5;
	//u8 step_down=2;
	u8 smoothstep = 0;
	int delta = 0;
	char msg[256];

	// combos.h
	u8 show_persistent_popup = 0;

	old_fan = 0;
	while(working)
	{
		if(fan_ps2_mode) /* skip dynamic fan control */; else

		// dynamic fan control
		if(max_temp)
		{
			t1 = t2 = 0;
			get_temperature(0, &t1); // CPU: 3E030000 -> 3E.03°C -> 62.(03/256)°C
			sys_ppu_thread_usleep(300000);

			get_temperature(1, &t2); // RSX: 3E030000 -> 3E.03°C -> 62.(03/256)°C
			sys_ppu_thread_usleep(200000);

			if(!max_temp || fan_ps2_mode) continue; // if fan mode was changed to manual by another thread while doing usleep

			if(t2 > t1) t1 = t2;

			if(!lasttemp) lasttemp = t1;

			delta = (lasttemp - t1);

			lasttemp = t1;

			if((t1 >= max_temp) || (t1 >= MAX_TEMPERATURE))
			{
				if (delta  < 0) fan_speed += 2;
				if((delta == 0) && (t1 != (max_temp - 1))) fan_speed++;
				if((delta == 0) && (t1 >= (max_temp + 1))) fan_speed += (2 + (t1 - max_temp));
				if (delta  > 0)
				{
					smoothstep++;
					if(smoothstep > 1)
					{
						fan_speed--;
						smoothstep = 0;
					}
				}
				if(t1 >= MAX_TEMPERATURE)               fan_speed += step_up;
				if((delta < 0) && (t1 - max_temp) >= 2) fan_speed += step_up;
			}
			else
			{
				if((delta <  0) && (t1 >= (max_temp - 1))) fan_speed += 2;
				if((delta == 0) && (t1 <= (max_temp - 2)))
				{
					smoothstep++;
					if(smoothstep > 1)
					{
						fan_speed--;
						if(t1 <= (max_temp - 3)) {fan_speed--; if(fan_speed > 0xA8) fan_speed--;} // decrease fan speed faster if > 66% & cpu is cool
						if(t1 <= (max_temp - 5)) {fan_speed--; if(fan_speed > 0x80) fan_speed--;} // decrease fan speed faster if > 50% & cpu is very cool
						smoothstep = 0;
					}
				}
				//if(delta==0 && t1>=(max_temp-1)) fan_speed++;
				if(delta > 0)
				{
					//smoothstep++;
					//if(smoothstep)
					{
						fan_speed--;
						if(t1 <= (max_temp - 3)) {fan_speed--; if(fan_speed>0xA8) fan_speed--;} // decrease fan speed faster if > 66% & cpu is cool
						if(t1 <= (max_temp - 5)) {fan_speed--; if(fan_speed>0x80) fan_speed--;} // decrease fan speed faster if > 50% & cpu is very cool
						smoothstep = 0;
					}
				}
			}

			if((t1 > 76) && (old_fan < 0x66)) fan_speed += step_up; // increase fan speed faster if < 40% and cpu is too hot

			if(fan_speed < ((webman_config->minfan * 255) / 100)) fan_speed = (webman_config->minfan * 255) / 100;
			if(fan_speed > MAX_FANSPEED) fan_speed = MAX_FANSPEED;

			//sprintf(debug, "OFAN: %x | CFAN: %x | TEMP: %i | STALL: %i\r\n", old_fan, fan_speed, t1, stall);	ssend(data_s, mytxt);
			//if(abs(old_fan-fan_speed)>=0x0F || stall>35 || (abs(old_fan-fan_speed) /*&& webman_config->aggr*/))
			if(old_fan!=fan_speed || stall>35)
			{
				//if(t1>76 && fan_speed < 0x50) fan_speed=0x50;
				//if(t1>77 && fan_speed < 0x58) fan_speed=0x58;
				if(t1 > 78 && fan_speed < 0x50) fan_speed += 2; // <31%
				if(old_fan != fan_speed)
				{
					old_fan = fan_speed;
					fan_control(fan_speed, 1);
					//sprintf(debug, "OFAN: %x | CFAN: %x | TEMP: %i | SPEED APPLIED!\r\n", old_fan, fan_speed, t1); ssend(data_s, mytxt);
					stall=0;
				}
			}
			else
				if( (old_fan > fan_speed) && ((old_fan - fan_speed) > 8) && (t1 < (max_temp - 3)) )
					stall++;
		}

		// Poll combos for 3 seconds
		#include "combos.h"

		// Overheat control (over 83°C)
		to++;
		if(to == 20)
		{
			if(!webman_config->nowarn)
			{
				get_temperature(0, &t1); // CPU
				get_temperature(1, &t2); // RSX

				if(t1 > (MAX_TEMPERATURE-2) || t2 > (MAX_TEMPERATURE-2))
				{
 #ifndef ENGLISH_ONLY
					char STR_OVERHEAT[80];//		= "System overheat warning!";
					char STR_OVERHEAT2[120];//	= "  OVERHEAT DANGER!\nFAN SPEED INCREASED!";

					language("STR_OVERHEAT", STR_OVERHEAT, "System overheat warning!");
					language("STR_OVERHEAT2", STR_OVERHEAT2, "  OVERHEAT DANGER!\nFAN SPEED INCREASED!");

					language("/CLOSEFILE", NULL, NULL);
 #endif
					sprintf(msg, "%s\n CPU: %i°C   RSX: %i°C", STR_OVERHEAT, t1, t2);
					show_msg(msg);
					sys_ppu_thread_sleep(2);

					if((t1 > MAX_TEMPERATURE) || (t2 > MAX_TEMPERATURE))
					{
						if(!max_temp) max_temp = (MAX_TEMPERATURE - 3);
						if(fan_speed < 0xB0) fan_speed = 0xB0; // 69%
						else
							if(fan_speed < MAX_FANSPEED) fan_speed += 8;

						old_fan = fan_speed;
						fan_control(fan_speed, 0);

						if(!webman_config->nowarn) show_msg((char*)STR_OVERHEAT2);
					}
				}
			}
		}
		if(to > 40) to = 0;

		// detect aprox. time when a game is launched & set network connect status
		#ifndef OFFLINE_INGAME
		if((sec % 10) == 0) poll_start_play_time();
		#else
		if((sec % 10) == 0 || (webman_config->spp & 4)) poll_start_play_time();
		#endif

		// USB Polling
		poll = webman_config->poll;
		if((poll == 0) && (sec >= 120)) // check USB drives each 120 seconds
		{
			uint8_t tmp[2048], f0 = 0;
			uint32_t usb_handle = NONE, r;

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
			sec = 0;
		}
		sec += step;

#ifdef PKG_HANDLER
		// Poll downloaded pkg files (if is on XMB)
		if((sec & 1) && (gTick.tick == rTick.tick)) poll_downloaded_pkg_files(msg);
#endif

		// Auto-mount JB game found on root of USB device
		if((webman_config->autob) && (sec & 1) && (gTick.tick == rTick.tick) && (!is_mounting))
		{
			if(!isDir("/dev_bdvd"))
			{
				for(u8 f0 = 1; f0 < 16; f0++)
				{
					if(IS_NET || IS_NTFS) continue;

					if(!check_drive(f0))
					{
						if(automount != f0)
						{
#ifdef COBRA_ONLY
							sprintf(msg, "%s/AUTOMOUNT.ISO", drives[f0]);
							if(file_exists(msg)) {mount_with_mm(msg, 0); automount = f0; break;}
							else
#endif
							{
								sprintf(msg, "%s/PS3_GAME/PARAM.SFO", drives[f0]);
								if(file_exists(msg)) {mount_with_mm(msg, 0); automount = f0; break;}
							}
						}
						else if(!isDir(drives[f0])) automount = 0;
					}
				}
			}
		}

#ifdef DO_WM_REQUEST_POLLING
		// Poll requests via local file
		if((webman_config->combo2 & CUSTOMCMB) || ((sec & 1) && (gTick.tick > rTick.tick))) continue; // slowdown polling if ingame

		if(file_exists(WMREQUEST_FILE))
		{
			loading_html++;
			sys_ppu_thread_t t_id;
			if(working) sys_ppu_thread_create(&t_id, handleclient, WM_FILE_REQUEST, THREAD_PRIO, THREAD_STACK_SIZE_64KB, SYS_PPU_THREAD_CREATE_NORMAL, THREAD_NAME_WEB);
		}
#endif

	}

	sys_ppu_thread_exit(0);
}
