/*
 FAIL SAFE    : SELECT+L3+L2+R2
 RESET SAFE   : SELECT+R3+L2+R2

 REFRESH XML  : SELECT+L3 (+R2=profile1, +L2=profile2)
 UNLOAD WM    : L3+R3+R2

 PLAY_DISC    : L2+START

 PREV GAME    : SELECT+L1                       *or* Custom Combo -> /dev_hdd0/tmp/wm_combo/wm_custom_select_l1
 NEXT GAME    : SELECT+R1                       *or* Custom Combo -> /dev_hdd0/tmp/wm_combo/wm_custom_select_r1
 UMNT_GAME    : SELECT+O (unmount)              *or* Custom Combo -> /dev_hdd0/tmp/wm_combo/wm_custom_select_circle

 RESERVED     : SELECT+TRIANGLE                 *or* Custom Combo -> /dev_hdd0/tmp/wm_combo/wm_custom_select_triangle

 EXT GAME DATA: SELECT+□                        *or* Custom Combo -> /dev_hdd0/tmp/wm_combo/wm_custom_select_square
 MOUNT net0/  : SELECT+R2+□
 MOUNT net1/  : SELECT+L2+□

 SHUTDOWN     : L3+R2+X
 SHUTDOWN  *2 : L3+R1+X (vsh shutdown) <- alternative shutdown method
 RESTART      : L3+R2+O (lpar restart)
 RESTART   *2 : L3+R1+O (vsh restart)  <- alternative restart method

 FAN CNTRL    : L3+R2+START  (enable/disable fancontrol)
 SHOW TEMP    : SELECT+START (SELECT+START+R2 will show only copy progress) / SELECT+R3 (if rec video flag is disabled)
 DYNAMIC TEMP : SELECT+LEFT/RIGHT
 MANUAL TEMP  : SELECT+UP/DOWN

 REC VIDEO    : SELECT+R3
 REC VIDEO SET: SELECT+R3+L2  Select video rec setting
 REC VIDEO VAL: SELECT+R3+R2  Change value of video rec setting
 XMB SCRNSHOT : L2+R2+SELECT+START

 USER/ADMIN   : L2+R2+TRIANGLE                  *or* Custom Combo -> /dev_hdd0/tmp/wm_combo/wm_custom_l2_r2_triangle

 SYSCALLS     : R2+TRIANGLE                     *or* Custom Combo -> /dev_hdd0/tmp/wm_combo/wm_custom_r2_triangle
 SHOW IDPS    : R2+O  (Abort copy/fix process)  *or* Custom Combo -> /dev_hdd0/tmp/wm_combo/wm_custom_r2_circle
 OFFLINE MODE : R2+□                            *or* Custom Combo -> /dev_hdd0/tmp/wm_combo/wm_custom_r2_square

 QUICK INSTALL: SELECT+R2+O                     *or* Custom Combo -> /dev_hdd0/tmp/wm_combo/wm_custom_select_r2_circle

 TOGGLE PS2CLASSIC    : SELECT+L2+TRIANGLE
 SWITCH PS2EMU        : SELECT+L2+R2

 COBRA TOGGLE         : L3+L2+TRIANGLE
 REBUG  Mode Switcher : L3+L2+□
 Normal Mode Switcher : L3+L2+O
 DEBUG  Menu Switcher : L3+L2+X

 SKIP AUTO-MOUNT   : L2+R2  (at startup only)   *or* Custom Combo -> /dev_hdd0/tmp/wm_combo/wm_custom_l2_r2

 Open File Manager : L2+R2+O                    *or* Custom Combo -> /dev_hdd0/tmp/wm_combo/wm_custom_l2_r2_circle
 Open Games List   : L2+R2+R1+O                 *or* Custom Combo -> /dev_hdd0/tmp/wm_combo/wm_custom_l2_r2_r1_circle
 Open System Info  : L2+R2+L1+O                 *or* Custom Combo -> /dev_hdd0/tmp/wm_combo/wm_custom_l2_r2_l1_circle
 Open Setup        : L2+R2+L1+R1+O              *or* Custom Combo -> /dev_hdd0/tmp/wm_combo/wm_custom_l2_r2_l1_r1_circle
*/
		bool reboot = false;

		u8 n;

		CellPadData data;

		#define PERSIST  100

		for(n = 0; n < 10; n++)
		{
			if(show_persistent_popup == PERSIST) {goto show_persistent_popup;}
			if(show_info_popup) {show_info_popup = false; goto show_popup;}

			if(!webman_config->nopad)
			{
#ifdef VIRTUAL_PAD
				if(vcombo)
				{
					data.len = 16; data.button[CELL_PAD_BTN_OFFSET_DIGITAL1] = (vcombo & 0xFF); data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] = (vcombo & 0xFF00) >> 8; vcombo = 0;
				}
				else
#endif
				{
					data.len = 0;
					for(u8 p = 0; p < 8; p++)
						if(cellPadGetData(p, &data) == CELL_PAD_OK && data.len > 0) break;
				}

				if(data.len > 0)
				{
					if(!(webman_config->combo2 & PLAY_DISC) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL1] == CELL_PAD_CTRL_START) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] == CELL_PAD_CTRL_L2))
					{
						char category[16], seg_name[40]; *category = *seg_name = NULL;
						launch_disc(category, seg_name, true); // L2+START
						break;
					}

					if((data.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_SELECT))
					{
						if( !(webman_config->combo2 & (EXTGAMDAT | MOUNTNET0 | MOUNTNET1))         // Toggle External Game Data
							&& (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_SQUARE)) // SELECT+SQUARE
						{
#ifdef COBRA_ONLY
 #ifndef LITE_EDITION
							if(!(webman_config->combo2 & MOUNTNET0) &&
								(data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_R2))
							{if(is_netsrv_enabled(0)) mount_with_mm((char*)"/net0", 1);} // SELECT+SQUARE+R2 / SELECT+R2+SQUARE
							else
							if(!(webman_config->combo2 & MOUNTNET1) &&
								(data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_L2))
							{if(is_netsrv_enabled(1)) mount_with_mm((char*)"/net1", 1);} // SELECT+SQUARE+L2 / SELECT+L2+SQUARE
							else
 #endif
#endif
							{
#ifdef WM_CUSTOM_COMBO
								if(do_custom_combo("select_square")) break;
								else
#endif
								{
#ifdef EXT_GDATA
									set_gamedata_status(extgd^1, true); // SELECT+SQUARE
#endif
									n = 0;
									break;
								}
							}
						}
						else
						if( !(webman_config->combo & FAIL_SAFE) &&
							(data.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_L3) &&                    // fail-safe mode
							(data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] == (CELL_PAD_CTRL_L2 | CELL_PAD_CTRL_R2)) // SELECT+L3+L2+R2
							)
						{
							//// startup time /////
							CellRtcTick pTick; cellRtcGetCurrentTick(&pTick);
							u32 ss = (u32)((pTick.tick - rTick.tick)/1000000);
							///////////////////////

							if((!sys_admin && (ss > 60)) || IS_INGAME) continue;

							cellFsUnlink("/dev_hdd0/boot_plugins.txt");
							goto reboot; // vsh reboot
						}
						else
						if( (data.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_R3) &&                    // reset-safe mode
							(data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] == (CELL_PAD_CTRL_L2 | CELL_PAD_CTRL_R2)) // SELECT+R3+L2+R2
							)
						{
							if(!sys_admin || IS_INGAME) continue;
#ifndef ENGLISH_ONLY
							char STR_RMVWMCFG[96];//		= "webMAN config reset in progress...";
							char STR_RMVWMCFGOK[112];//	= "Done! Restart within 3 seconds";

							language("STR_RMVWMCFG", STR_RMVWMCFG, "webMAN config reset in progress...");
							language("STR_RMVWMCFGOK", STR_RMVWMCFGOK, "Done! Restart within 3 seconds");

							language("/CLOSEFILE", NULL, NULL);
#endif
							cellFsUnlink(WMCONFIG);
							{ BEEP1 }
							show_msg((char*)STR_RMVWMCFG);
							sys_ppu_thread_sleep(2);
							show_msg((char*)STR_RMVWMCFGOK);
							sys_ppu_thread_sleep(3);
							goto reboot; // vsh reboot
						}
#ifdef COBRA_ONLY
 #ifndef LITE_EDITION
						else
						if( !(webman_config->combo2 & PS2TOGGLE)
							&& (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_L2)
							&& (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_TRIANGLE) // SELECT+L2+TRIANGLE
							&& (c_firmware >= 4.65f) )
						{
							bool classic_ps2_enabled = file_exists(PS2_CLASSIC_TOGGLER);

							if(classic_ps2_enabled)
							{
								disable_classic_ps2_mode();
							}
							else
							{
								enable_classic_ps2_mode();
							}

							sprintf(msg, "PS2 Classic %s", classic_ps2_enabled ? STR_DISABLED : STR_ENABLED);
							show_msg(msg);

							n = 0;
							break;
						}
						else
						if( !(webman_config->combo2 & PS2SWITCH)
							&& (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_L2) // Clone ps2emu habib's switcher
							&& (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_R2) // SELECT+L2+R2
							&& (c_firmware>=4.53f) )
						{
								toggle_ps2emu();
						}
 #endif //#ifndef LITE_EDITION
#endif //#ifdef COBRA_ONLY

						else
						if(!(webman_config->combo2 & XMLREFRSH) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_L3) ) // SELECT+L3 refresh XML
						{
							if(data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] == CELL_PAD_CTRL_R2) profile=1; else
							if(data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] == CELL_PAD_CTRL_L2) profile=2; else
							if(data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] == CELL_PAD_CTRL_R1) profile=3; else
							if(data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] == CELL_PAD_CTRL_L1) profile=4; else profile=0;

							refresh_xml(msg);
						}
#ifdef VIDEO_REC
						else
						if(!(webman_config->combo2 & VIDRECORD) && data.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_R3) // SELECT + R3
						{
							if(data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_L2)
							{
								rec_setting_to_change++; if(rec_setting_to_change>5) rec_setting_to_change = 0; 	// SELECT+R3+L2  Select video rec setting
								set_setting_to_change(msg, "Change : ");

								strcat(msg, "\n\nCurrent recording format:");
								show_rec_format(msg);
							}
							else
							if(data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_R2)
							{
								set_setting_to_change(msg, "Changed : ");									// SELECT+R3+R2  Change value of video rec setting

								strcat(msg, "\n\nCurrent recording format:");
								if(rec_setting_to_change == 0)
								{
									rec_audio_format = CELL_REC_PARAM_AUDIO_FMT_AAC_64K;
									if(rec_video_format == CELL_REC_PARAM_VIDEO_FMT_M4HD_HD720_5000K_30FPS)   {rec_video_format = CELL_REC_PARAM_VIDEO_FMT_MPEG4_LARGE_2048K_30FPS; } else
									if(rec_video_format == CELL_REC_PARAM_VIDEO_FMT_MPEG4_LARGE_2048K_30FPS)  {rec_video_format = CELL_REC_PARAM_VIDEO_FMT_AVC_MP_MIDDLE_768K_30FPS; rec_audio_format = CELL_REC_PARAM_AUDIO_FMT_PCM_768K;} else
									if(rec_video_format == CELL_REC_PARAM_VIDEO_FMT_AVC_MP_MIDDLE_768K_30FPS) {rec_video_format = CELL_REC_PARAM_VIDEO_FMT_MPEG4_SMALL_512K_30FPS;  } else
									if(rec_video_format == CELL_REC_PARAM_VIDEO_FMT_MPEG4_SMALL_512K_30FPS)   {rec_video_format = CELL_REC_PARAM_VIDEO_FMT_MJPEG_HD720_11000K_30FPS; rec_audio_format = CELL_REC_PARAM_AUDIO_FMT_AAC_96K;} else
																											  {rec_video_format = CELL_REC_PARAM_VIDEO_FMT_M4HD_HD720_5000K_30FPS;  }

									show_rec_format(msg);
								}
								if(rec_setting_to_change == 1) {rec_video_format += 0x1000; if((rec_video_format & 0xF000) > 0x4000) rec_video_format &= 0x0FFF;} else
								if(rec_setting_to_change == 2) {rec_video_format += 0x0100; if((rec_video_format & 0x0F00) > 0x0300) {rec_video_format += 0x0200; if((rec_video_format & 0x0F00) > 0x0600) rec_video_format &= 0xF0FF;}} else
								if(rec_setting_to_change == 3) {rec_video_format += 0x0010; if((rec_video_format & 0x00F0) > 0x0090) rec_video_format &= 0xFF0F; else if((rec_video_format & 0x00F0) == 0x0050) rec_video_format += 0x0010;} else
								if(rec_setting_to_change == 4) {rec_audio_format += 0x1000; if((rec_audio_format & 0xF000) > 0x2000) rec_audio_format &= 0x0FFF;} else
								if(rec_setting_to_change == 5) {rec_audio_format += 0x0001; if((rec_audio_format & 0x000F) > 0x0002) {rec_audio_format += 0x0004; if((rec_audio_format & 0x000F) > 0x0009) rec_audio_format &= 0xFFF0;}}

								show_rec_format(msg);
							}
							else
								{memset(msg, 0, 256); toggle_video_rec(msg); n = 0;}

							break;
						}
#endif
						else
						if( (!(webman_config->combo & SHOW_TEMP) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & (CELL_PAD_CTRL_R3 | CELL_PAD_CTRL_START)))) // SELECT+START show temperatures / hdd space
						{
#ifdef XMB_SCREENSHOT
							if(data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] == (CELL_PAD_CTRL_R2 | CELL_PAD_CTRL_L2) && IS_ON_XMB)
								{memset(msg, 0, 256); saveBMP(msg, true); n = 0; break;} // L2 + R2 + SELECT + START
							else
#endif
							{
								if(show_persistent_popup == 0)        show_persistent_popup = 1;               else
								if(show_persistent_popup  < PERSIST) {BEEP1; show_persistent_popup = PERSIST;} else
																	 {BEEP2; show_persistent_popup = 0;}
show_persistent_popup:
								/////////////////////////////
#if defined(FIX_GAME) || defined(COPY_PS3)
								if(copy_in_progress || fix_in_progress)
								{
 #ifdef FIX_GAME
									if(fix_in_progress)
										sprintf(msg, "%s %s", STR_FIXING, current_file);
									else
 #endif
										sprintf(msg, "%s %s (%i %s)", STR_COPYING, current_file, copied_count, STR_FILES);

									show_msg(msg);
									sys_ppu_thread_sleep(2);
									if(data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & (CELL_PAD_CTRL_R2 | CELL_PAD_CTRL_L2) ) break;
								}
#endif
								/////////////////////////////
show_popup:
								{ PS3MAPI_ENABLE_ACCESS_SYSCALL8 }

								CellRtcTick pTick; u32 dd, hh, mm, ss; char tmp[256];

								cellRtcGetCurrentTick(&pTick);
								get_temperature(0, &t1); // CPU
								get_temperature(1, &t2); // RSX

								u8 speed = fan_speed;
								if(fan_ps2_mode) speed=(int)(255.f*(float)(webman_config->ps2temp+1)/100.f); else
								if((webman_config->fanc==0) && (get_fan_policy_offset>0))
								{
									u8 st, mode, unknown;
									backup[2]=peekq(get_fan_policy_offset);
									lv2poke32(get_fan_policy_offset, 0x38600001); // sys 409 get_fan_policy
									sys_sm_get_fan_policy(0, &st, &mode, &fan_speed, &unknown);
									pokeq(get_fan_policy_offset, backup[2]);
									speed = fan_speed;
								}

								_meminfo meminfo;
								{system_call_1(SC_GET_FREE_MEM, (uint64_t)(u32) &meminfo);}

								// detect aprox. time when a game is launched
								poll_start_play_time();

								bool R2 = (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] == CELL_PAD_CTRL_R2), bb;

								///// startup/play time /////
								bb = (!R2 && gTick.tick>rTick.tick); // show play time
								ss = (u32)((pTick.tick-(bb?gTick.tick:rTick.tick))/1000000); dd = (u32)(ss / 86400);
								if(dd>100) {bb=false; ss = (u32)((pTick.tick-rTick.tick)/1000000); dd = (u32)(ss / 86400);}
								ss = ss % 86400; hh = (u32)(ss / 3600); ss = ss % 3600; mm = (u32)(ss / 60); ss = ss % 60;
								/////////////////////////////

								char net_type[8] = "", ip[ip_size] = "-";
								get_net_info(net_type, ip);

								char cfw_info[20];
								get_cobra_version(cfw_info);

								char smax[32]; if(fan_ps2_mode) sprintf(smax, "   PS2 Mode"); else if(max_temp) sprintf(smax, "   MAX: %i°C", max_temp); else if(webman_config->fanc==0) sprintf(smax, "   SYSCON"); else memset(smax, 0, 16);

								sprintf(tmp, "CPU: %i°C  RSX: %i°C  FAN: %i%%   \n"
											 "%s: %id %02d:%02d:%02d%s\n"
											 "Firmware : %s %s\n"
											 "IP: %s  %s%s",
											 t1, t2, (int)(((int)speed*100)/255),
											 bb ? "Play":"Startup", dd, hh, mm, ss, smax,
											 fw_version, cfw_info, ip, net_type, syscalls_removed ? "  [noSC]" : "");

								int hdd_free = (int)(get_free_space("/dev_hdd0")>>20);

								sprintf(msg, "%s\n%s: %i %s\n"
											 "%s: %i %s\n", tmp,
											 STR_STORAGE, hdd_free, STR_MBFREE,
											 STR_MEMORY, meminfo.avail>>10, STR_KBFREE);

								if(R2 && (gTick.tick>rTick.tick))
								{
									////// play time //////
									ss = (u32)((pTick.tick-gTick.tick)/1000000);
									dd = (u32)(ss / 86400); ss = ss % 86400; hh = (u32)(ss / 3600); ss = ss % 3600; mm = (u32)(ss / 60); ss = ss % 60;

									if(dd<100) {char gname[200]; get_game_info(); sprintf(gname, "%s %s\n\n", _game_TitleID, _game_Title); sprintf(msg, "%sPlay: %id %02d:%02d:%02d\n%s", gname, dd, hh, mm, ss, tmp); }
								}

								{ PS3MAPI_DISABLE_ACCESS_SYSCALL8 }

								show_msg(msg);
								sys_ppu_thread_sleep(2);
							}
						}
						else
						if(webman_config->fanc && !(webman_config->combo & MANUALFAN) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_UP) ) // SELECT+UP increase TEMP/FAN
						{
#ifdef WM_CUSTOM_COMBO
							if(do_custom_combo("select_up")) break;
							else
#endif
							{
								if(webman_config->fanc==0) enable_fan_control(3, msg);

								if(max_temp) //auto mode
								{
									if(data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_R2) max_temp+=5; else max_temp+=1;
									if(max_temp>85) max_temp=85;
									webman_config->temp1=max_temp;
									sprintf(msg, "%s\n%s %i°C", STR_FANCH0, STR_FANCH1, max_temp);
								}
								else
								{
									if(data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_R2) webman_config->manu+=5; else webman_config->manu+=1;
									webman_config->manu=RANGE(webman_config->manu, 20, 95); //%
									webman_config->temp0= (u8)(((float)(webman_config->manu+1) * 255.f)/100.f);
									webman_config->temp0=RANGE(webman_config->temp0, 0x33, MAX_FANSPEED);
									fan_control(webman_config->temp0, 0);
									sprintf(msg, "%s\n%s %i%%", STR_FANCH0, STR_FANCH2, webman_config->manu);
								}
								save_settings();
								show_msg(msg);

								n = 0;
								break;
							}
						}
						else
						if(webman_config->fanc && !(webman_config->combo & MANUALFAN) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_DOWN) ) // SELECT+DOWN increase TEMP/FAN
						{
#ifdef WM_CUSTOM_COMBO
							if(do_custom_combo("select_down")) break;
							else
#endif
							{
								if(webman_config->fanc==0) enable_fan_control(3, msg);

								if(max_temp) //auto mode
								{
									if(max_temp>30) {if(data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_R2) max_temp-=5; else max_temp-=1;}
									webman_config->temp1=max_temp;
									sprintf(msg, "%s\n%s %i°C", STR_FANCH0, STR_FANCH1, max_temp);
								}
								else
								{
									if(webman_config->manu>20) {if(data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_R2) webman_config->manu-=5; else webman_config->manu-=1;}
									webman_config->temp0 = (u8)(((float)(webman_config->manu+1) * 255.f)/100.f);
									if(webman_config->temp0<0x33) webman_config->temp0=0x33;
									if(webman_config->temp0>MAX_FANSPEED) webman_config->temp0=MAX_FANSPEED;
									fan_control(webman_config->temp0, 0);
									sprintf(msg, "%s\n%s %i%%", STR_FANCH0, STR_FANCH2, webman_config->manu);
								}
								save_settings();
								show_msg(msg);

								n = 0;
								break;
							}
						}
						else
						if(webman_config->minfan && !(webman_config->combo & MINDYNFAN) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_LEFT) ) // SELECT+LEFT decrease Minfan
						{
#ifdef WM_CUSTOM_COMBO
							if(do_custom_combo("select_left")) break;
							else
#endif
							{
								if(webman_config->fanc==0) enable_fan_control(3, msg);

								if(webman_config->minfan-5>=MIN_FANSPEED) webman_config->minfan-=5;
								sprintf(msg, "%s\n%s %i%%", STR_FANCH0, STR_FANCH3, webman_config->minfan);

								save_settings();
								show_msg(msg);

								n = 0;
								break;
							}
						}
						else
						if(webman_config->minfan && !(webman_config->combo & MINDYNFAN) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_RIGHT) ) // SELECT+RIGHT increase Minfan
						{
#ifdef WM_CUSTOM_COMBO
							if(do_custom_combo("select_right")) break;
							else
#endif
							{
								if(webman_config->fanc==0) enable_fan_control(3, msg);

								if(webman_config->minfan+5<100) webman_config->minfan+=5;
								sprintf(msg, "%s\n%s %i%%", STR_FANCH0, STR_FANCH3, webman_config->minfan);

								save_settings();
								show_msg(msg);

								n = 0;
								break;
							}
						}
						else
						if(!(webman_config->combo & PREV_GAME) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] == CELL_PAD_CTRL_L1) ) // SELECT+L1 (previous title)
						{
#ifdef WM_CUSTOM_COMBO
							if(do_custom_combo("select_l1")) break;
							else
#endif
							{
								mount_with_mm((char*)"_prev", EXPLORE_CLOSE_ALL);

								n = 0;
								break;
							}
						}
						else
						if(!(webman_config->combo & NEXT_GAME) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] == CELL_PAD_CTRL_R1) ) // SELECT+R1 (next title)
						{
#ifdef WM_CUSTOM_COMBO
							if(do_custom_combo("select_r1")) break;
							else
#endif
							{
								mount_with_mm((char*)"_next", EXPLORE_CLOSE_ALL);

								n = 0;
								break;
							}
						}
#ifdef PKG_HANDLER
						else
						if(!(webman_config->combo2 & INSTALPKG) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] == (CELL_PAD_CTRL_CIRCLE | CELL_PAD_CTRL_R2)) ) // SELECT+R2+O
						{
#ifdef WM_CUSTOM_COMBO
							if(do_custom_combo("select_r2_circle")) break;
							else
#endif
							if(!install_in_progress && IS_ON_XMB)
							{
								sys_ppu_thread_t thread_id;
								sys_ppu_thread_create(&thread_id, installPKG_combo_thread, NULL, THREAD_PRIO, THREAD_STACK_SIZE_INSTALL_PKG, SYS_PPU_THREAD_CREATE_NORMAL, THREAD_NAME_INSTALLPKG);
							}

							n = 0;
							break;
						}
#endif
						else
						if(!(webman_config->combo & UMNT_GAME) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] == CELL_PAD_CTRL_CIRCLE) ) // SELECT+O (unmount)
						{
#ifdef WM_CUSTOM_COMBO
							if(do_custom_combo("select_circle")) break;
							else
#endif
								do_umount(true);

							n = 0;
							break;
						}
#ifdef WM_CUSTOM_COMBO
						else
						if(!(webman_config->combo & UMNT_GAME) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] == CELL_PAD_CTRL_TRIANGLE) ) // SELECT+TRIANGLE
						{
							if(do_custom_combo("select_triangle")) break; // RESERVED
						}
#endif
					}
					else
					if((data.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_L3) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_R2))
					{
						if(!(webman_config->combo & SHUT_DOWN) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_CROSS) ) // L3+R2+X (shutdown)
						{
							// power off
							working = 0;
							{ DELETE_TURNOFF } { BEEP1 }

							{system_call_4(SC_SYS_POWER, SYS_SHUTDOWN, 0, 0, 0);}

							sys_ppu_thread_exit(0);
						}
						else if(!(webman_config->combo & RESTARTPS) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_CIRCLE) ) // L3+R2+O (lpar restart)
						{
							// lpar restart
							working = 0;
							{ DELETE_TURNOFF }{ BEEP2 }

							{system_call_3(SC_SYS_POWER, SYS_REBOOT, NULL, 0);}

							sys_ppu_thread_exit(0);
						}
						else if(!(webman_config->combo & UNLOAD_WM) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_R3) ) // L3+R3+R2 (quit webMAN)
						{
							wm_unload_combo = 1;

							restore_settings(!webman_config->fanc || (webman_config->ps2temp < 33));

							stop_prx_module();
							sys_ppu_thread_exit(0);
							break;
						}
						else if(!(webman_config->combo & DISABLEFC) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_START) ) // L3+R2+START (enable/disable fancontrol)
						{
							enable_fan_control(2, msg);

							n = 0;
							break;
						}
					}
					else
					if((data.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_L3) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_R1))
					{
						if(!(webman_config->combo & SHUT_DOWN) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_CROSS)) // L3+R1+X (vsh shutdown)
						{
							// vsh shutdown
							working = 0;
							{ DELETE_TURNOFF }{ BEEP1 }

							vsh_shutdown(); // VSH shutdown

							sys_ppu_thread_exit(0);
						}

						if(!(webman_config->combo & RESTARTPS) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_CIRCLE)) // L3+R1+O (vsh restart)
						{
							// vsh reboot
							working = 0;
							{ DELETE_TURNOFF }{ BEEP2 }

							vsh_reboot(); // VSH reboot

							sys_ppu_thread_exit(0);
						}
					}
#ifdef SYS_ADMIN_MODE
					else
					if((webman_config->combo & SYS_ADMIN) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] == (CELL_PAD_CTRL_L2 | CELL_PAD_CTRL_R2 | CELL_PAD_CTRL_TRIANGLE) )) // L2+R2+TRIANGLE Toggle user/admin mode
					{
 #ifdef WM_CUSTOM_COMBO
						if(do_custom_combo("l2_r2_triangle")) break;
						else
 #endif
						{
							sys_admin ^= 1, pwd_tries = 0;

							sprintf(msg, "ADMIN %s", sys_admin ? STR_ENABLED : STR_DISABLED);
							show_msg(msg);

							if(sys_admin) { BEEP1 } else { BEEP2 }
						}
						n = 0;
						break;
					}
#endif
					else
					if(data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_R2)
					{
						if(!(webman_config->combo & SHOW_IDPS) && ((data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & (CELL_PAD_CTRL_L2 | CELL_PAD_CTRL_R2 | CELL_PAD_CTRL_CIRCLE)) == (CELL_PAD_CTRL_L2 | CELL_PAD_CTRL_R2 | CELL_PAD_CTRL_CIRCLE)) && IS_ON_XMB) // L2+R2+O
						{
#ifdef WM_CUSTOM_COMBO
								 if(do_custom_combo("l2_r2_circle")) ;
							else if(do_custom_combo("l2_r2_l1_circle")) ;
							else if(do_custom_combo("l2_r2_r1_circle")) ;
							else if(do_custom_combo("l2_r2_l1_r1_circle")) ;
							else
#endif
							{
#ifdef PS3_BROWSER
								do_umount(false); // prevent system freeze on disc icon

								if(data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] == (CELL_PAD_CTRL_L1 | CELL_PAD_CTRL_R1 | CELL_PAD_CTRL_L2 | CELL_PAD_CTRL_R2 | CELL_PAD_CTRL_CIRCLE))
									{open_browser((char*)"http://127.0.0.1/setup.ps3", 0); show_msg((char*)STR_WMSETUP);}     // L2+R2+L1+R1+O
								else if(data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_R1)
									{open_browser((char*)"http://127.0.0.1/index.ps3", 0); show_msg((char*)STR_MYGAMES);}     // L2+R2+R1+O
								else if(data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_L1)
									{open_browser((char*)"http://127.0.0.1/cpursx.ps3", 0); show_msg((char*)"webMAN Info");}  // L2+R2+L1+O
								else
									{open_browser((char*)"http://127.0.0.1/", 0); show_msg((char*)"webMAN " WM_VERSION);}     // L2+R2+O
#endif
							}
						}
						else
						if((copy_in_progress || fix_in_progress) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_CIRCLE)) // R2+O Abort copy process
						{
							fix_aborted = copy_aborted = true;
						}
						else
						if(!(webman_config->combo & DISABLESH) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_TRIANGLE) ) // R2+TRIANGLE Disable CFW Sycalls
						{
#ifdef WM_CUSTOM_COMBO
							if(do_custom_combo("r2_triangle")) break;
							else
#endif
							{
#ifdef REMOVE_SYSCALLS
								disable_cfw_syscalls(webman_config->keep_ccapi);
#endif
							}
						}
						else
						if(!(webman_config->combo2 & CUSTOMCMB) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_SQUARE) ) // R2+SQUARE
						{
#ifdef WM_CUSTOM_COMBO
							if(do_custom_combo("r2_square")) break;
							else
#endif
#ifdef WM_REQUEST
							if(do_custom_combo("/dev_hdd0/tmp/wm_custom_combo")) break;
							else
#endif
							{
								block_online_servers(true);
							}
						}
						else
						if(!(webman_config->combo & SHOW_IDPS) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_CIRCLE) ) // R2+O Show IDPS EID0+LV2
						{
#ifdef WM_CUSTOM_COMBO
							if(do_custom_combo("r2_circle")) break;
							else
#endif
							{
#ifndef LITE_EDITION
								enable_ingame_screenshot();
#endif
#ifdef SPOOF_CONSOLEID
								show_idps(msg);
#endif
							}
						}
#ifdef WM_CUSTOM_COMBO
						else
						if(!(webman_config->combo & SHOW_IDPS) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] == (CELL_PAD_CTRL_L2 | CELL_PAD_CTRL_R2))) // L2+R2
						{
							if(do_custom_combo("l2_r2") == false) continue; // RESERVED
						}
#endif
						n = 0;
						break;
					}
					else
					if((data.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_L3) && (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_L2))
					{
						if(!sys_admin || IS_INGAME) continue;
#ifdef COBRA_ONLY
 #ifndef LITE_EDITION
						if(!(webman_config->combo & DISACOBRA)
							&& (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_TRIANGLE))
						{ // L3+L2+TRIANGLE COBRA Toggle
							reboot = toggle_cobra();
						}
 #endif
#endif //#ifdef COBRA_ONLY

#ifdef REX_ONLY
						if(!(webman_config->combo2 & REBUGMODE)
							&& (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_SQUARE))
						{ // L3+L2+□ REBUG Mode Switcher
							reboot = toggle_rebug_mode();
						}
						else
						if(!(webman_config->combo2 & NORMAMODE)
							&& (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_CIRCLE))
						{ // L3+L2+O Normal Mode Switcher
							reboot = toggle_normal_mode();
						}
						else
						if(!(webman_config->combo2 & DEBUGMENU)
							&& (data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_CROSS))
						{ // L3+L2+X DEBUG Menu Switcher
							toggle_debug_menu();
						}
#endif //#ifdef REX_ONLY
					}
				}

				if(reboot)
				{
					sys_ppu_thread_sleep(1);
					// reboot
					show_msg((char*)"Switching successful! Reboot now...");
					sys_ppu_thread_sleep(3);
					disable_dev_blind();
reboot:
					// vsh reboot
					working = 0;
					{ DELETE_TURNOFF }
					save_file(WMNOSCAN, NULL, 0);

					vsh_reboot(); // VSH reboot

					sys_ppu_thread_exit(0);
				}

			}

			if(!working) break;

			sys_ppu_thread_usleep(300000);

			if(show_persistent_popup)
			{
				show_persistent_popup++;
				if(show_persistent_popup > 10 && show_persistent_popup < PERSIST) show_persistent_popup = 0; else
				if(show_persistent_popup > PERSIST + 6) show_persistent_popup = PERSIST;
			}
		}

		if(working && (n < 10)) sys_ppu_thread_usleep((12 - n) * 150000);
