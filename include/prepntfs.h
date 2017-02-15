#ifdef USE_NTFS
#define MAX_SECTIONS		(int)((0x10000-sizeof(rawseciso_args))/8)

enum ntfs_folders
{
	mPS3 = 0,
	mPSX = 1,
	mBLU = 2,
	mDVD = 3,
};

static u8 prepntfs_working = false;

static int prepNTFS(u8 towait)
{
	if(prepntfs_working) return 0;
	prepntfs_working = true;

	char path[256];

	int i, parts, count = 0;
	sys_ppu_thread_t t;

	unsigned int num_tracks;
	int emu_mode = 0;
	TrackDef tracks[32];
	ScsiTrackDescriptor *scsi_tracks;

	rawseciso_args *p_args;

	CellFsDirent dir;

	DIR_ITER *pdir = NULL, *psubdir= NULL;
	struct stat st;
	bool has_dirs, is_iso = false;

	char c_path[4][8]={"PS3ISO", "PSXISO", "BDISO", "DVDISO"};
	char prefix[2][8]={"/", "/PS3/"};

	uint8_t  *plugin_args = NULL;
	uint32_t *sectionsP = NULL;
	uint32_t *sections_sizeP = NULL;

	snprintf(path, sizeof(path), WMTMP);
	cellFsMkdir(path, S_IRWXO | S_IRWXU | S_IRWXG | S_IFDIR);
	cellFsChmod(path, S_IFDIR | 0777);
	cellFsUnlink((char*)WMTMP "/games.html");
	int fd = NONE;
	u64 read = 0;
	char path0[MAX_PATH_LEN], subpath[256], filename[256];

	if(mountCount == -2)
		for(i = 0; i < 2; i++)
		{
			if(towait) sys_ppu_thread_sleep(2 * towait);
			mount_all_ntfs_volumes();
			if(mountCount) break;
		}

	if(!towait || mountCount <= 0)
	{
		if(cellFsOpendir(path, &fd) == CELL_FS_SUCCEEDED)
		{
			char *ext;
			while(!cellFsReaddir(fd, &dir, &read) && read)
			{
				ext = strstr(dir.d_name, ".ntfs[");
				if(ext && !IS(ext, ".ntfs[BDFILE]")) {sprintf(path0, "%s/%s", path, dir.d_name); cellFsUnlink(path0);}
			}
			cellFsClosedir(fd);
		}
	}

	if(mountCount <= 0) {mountCount=-2; goto exit_prepntfs;}

	plugin_args = (uint8_t *)malloc(_64KB_); if(!plugin_args) goto exit_prepntfs;
	sectionsP = (uint32_t *)malloc(_32KB_); if(!sectionsP) goto exit_prepntfs;
	sections_sizeP = (uint32_t *)malloc(_32KB_); if(!sections_sizeP) goto exit_prepntfs;

	size_t plen, nlen, extlen;

	for(u8 profile = 0; profile < 5; profile++)
	{
		for(i = 0; i < mountCount; i++)
		{
			for(u8 n = 0; n < 2; n++)
			{
				for(u8 m = 0; m < 4; m++)
				{
					has_dirs = false;

					snprintf(path, sizeof(path), "%s:%s%s", mounts[i].name, prefix[n], c_path[m]);
					pdir = ps3ntfs_diropen(path);
					if(pdir!=NULL)
					{
						while(ps3ntfs_dirnext(pdir, dir.d_name, &st) == 0)
						{
							is_iso = (  !extcasecmp(dir.d_name, ".iso", 4) ||
										!extcasecmp(dir.d_name, ".iso.0", 6) ||
										!extcasecmp(dir.d_name, ".img", 4) ||
										!extcasecmp(dir.d_name, ".mdf", 4) ||
										(m > mPS3 && !extcasecmp(dir.d_name, ".bin", 4)) );

////////////////////////////////////////////////////////
							//--- is SUBFOLDER?
							if(!is_iso)
							{
								sprintf(subpath, "%s:%s%s%s/%s", mounts[i].name, prefix[n], c_path[m], SUFIX(profile), dir.d_name);
								psubdir = ps3ntfs_diropen(subpath);
								if(psubdir==NULL) continue;
								sprintf(subpath, "%s", dir.d_name); has_dirs = true;
next_ntfs_entry:
								if(ps3ntfs_dirnext(psubdir, dir.d_name, &st) < 0) {has_dirs = false; continue;}
								if(dir.d_name[0]=='.') goto next_ntfs_entry;

								sprintf(path, "%s", dir.d_name);

								is_iso = (  !extcasecmp(path, ".iso", 4) ||
											!extcasecmp(path, ".iso.0", 6) ||
											(m > mPS3 && !extcasecmp(path, ".bin", 4)) ||
											!extcasecmp(path, ".img", 4) ||
											!extcasecmp(path, ".mdf", 4) );

								if(is_iso)
								{
									sprintf(dir.d_name, "%s/%s", subpath, path);
									sprintf(filename, "[%s] %s", subpath, path);
								}
							}
							else
								sprintf(filename, "%s", dir.d_name);

////////////////////////////////////////////////////////////

							if(is_iso)
							{
								plen = snprintf(path, sizeof(path), "%s:%s%s%s/%s", mounts[i].name, prefix[n], c_path[m], SUFIX(profile), dir.d_name);
								extlen = 4;

								parts = ps3ntfs_file_to_sectors(path, sectionsP, sections_sizeP, MAX_SECTIONS, 1);

								// get multi-part file sectors
								if(!extcasecmp(dir.d_name, ".iso.0", 6))
								{
									char iso_name[MAX_PATH_LEN], iso_path[MAX_PATH_LEN];

									size_t nlen = sprintf(iso_name, "%s", path);
									iso_name[nlen - 1] = '\0';

									extlen = 6;

									for(u8 o = 1; o < 64; o++)
									{
										if(parts >= MAX_SECTIONS) break;

										sprintf(iso_path, "%s%i", iso_name, o);
										if(file_exists(iso_path) == false) break;

										parts += ps3ntfs_file_to_sectors(iso_path, sectionsP + (parts * sizeof(uint32_t)), sections_sizeP + (parts * sizeof(uint32_t)), MAX_SECTIONS - parts, 1);
									}
								}

								if(parts >= MAX_SECTIONS)
								{
									continue;
								}
								else if(parts > 0)
								{
									u8 cue = 0;
									num_tracks = 1;
										 if(m == mPS3) emu_mode = EMU_PS3;
									else if(m == mBLU) emu_mode = EMU_BD;
									else if(m == mDVD) emu_mode = EMU_DVD;
									else if(m == mPSX)
									{
										emu_mode = EMU_PSX;

										path[plen-3] = 'C', path[plen-2] = 'U', path[plen-1] = 'E';

										fd = ps3ntfs_open(path, O_RDONLY, 0);
										if(fd < 0)
										{
											path[plen-3] = 'c', path[plen-2] = 'u', path[plen-1] = 'e';
											fd = ps3ntfs_open(path, O_RDONLY, 0);
										}

										if(fd >= 0)
										{
											char *cue_buf = malloc(_2KB_);
											int cue_size = ps3ntfs_read(fd, cue_buf, _2KB_);
											ps3ntfs_close(fd);

											if(cue_size > 13)
											{
												cue = 1;
												num_tracks = 0;

												tracks[0].lba = 0;
												tracks[0].is_audio = 0;

												u8 use_pregap = 0;
												int lba, lp = 0; char *templn = path;

												while (lp < cue_size)
												{
													lp = get_line(templn, cue_buf, cue_size, lp);
													if(lp < 1) break;

													if(strstr(templn, "PREGAP")) {use_pregap = 1; continue;}
													if(!strstr(templn, "INDEX 01") && !strstr(templn, "INDEX 1 ")) continue;

													lba = parse_lba(templn, use_pregap && num_tracks); if(lba < 0) continue;

													tracks[num_tracks].lba = lba;
													if(num_tracks) tracks[num_tracks].is_audio = 1;

													num_tracks++; if(num_tracks>=32) break;
												}

												num_tracks++; if(num_tracks >= 32) break;
											}
											free(cue_buf);
										}
									}

									p_args = (rawseciso_args *)plugin_args; memset(p_args, 0x0, _64KB_);
									p_args->device = USB_MASS_STORAGE((mounts[i].interface->ioType & 0xff) - '0');
									p_args->emu_mode = emu_mode;
									p_args->num_sections = parts;

									memcpy(plugin_args + sizeof(rawseciso_args), sectionsP, parts * sizeof(uint32_t));
									memcpy(plugin_args + sizeof(rawseciso_args) + (parts * sizeof(uint32_t)), sections_sizeP, parts * sizeof(uint32_t));

									if(emu_mode == EMU_PSX)
									{
										int max = MAX_SECTIONS - ((num_tracks * sizeof(ScsiTrackDescriptor)) / 8);

										if(parts >= max)
										{
											continue;
										}

										p_args->num_tracks = num_tracks;
										scsi_tracks = (ScsiTrackDescriptor *)(plugin_args +sizeof(rawseciso_args)+(2*parts*sizeof(uint32_t)));

										if(!cue)
										{
											scsi_tracks[0].adr_control = 0x14;
											scsi_tracks[0].track_number = 1;
											scsi_tracks[0].track_start_addr = 0;
										}
										else
										{
											for(u8 j = 0; j < num_tracks; j++)
											{
												scsi_tracks[j].adr_control = (tracks[j].is_audio) ? 0x10 : 0x14;
												scsi_tracks[j].track_number = j+1;
												scsi_tracks[j].track_start_addr = tracks[j].lba;
											}
										}
									}

									snprintf(path, sizeof(path), "%s/%s%s.ntfs[%s]", WMTMP, filename, SUFIX2(profile), c_path[m]);

									save_file(path, (char*)plugin_args, (sizeof(rawseciso_args)+(parts*sizeof(uint32_t))*2)+(num_tracks*sizeof(ScsiTrackDescriptor))); count++;

									plen = snprintf(path, sizeof(path), "%s/%s", WMTMP, dir.d_name);
									nlen = snprintf(path0, sizeof(path0), "%s:%s%s%s/%s", mounts[i].name, prefix[n], c_path[m], SUFIX(profile), dir.d_name);

									if(get_image_file(path, plen - extlen)) goto for_sfo;
									if(get_image_file(path0, nlen - extlen) == false) goto for_sfo;

									// copy external image
									path[plen-3]=path0[nlen-3], path[plen-2]=path0[nlen-2], path[plen-1]=path0[nlen-1];
									file_copy(path0, path, COPY_WHOLE_FILE);
for_sfo:
									if(m == mPS3) // mount PS3ISO
									{
										path[plen-3]='S', path[plen-2]='F', path[plen-1]='O';
										if(file_exists(path) == false)
										{
											if(isDir("/dev_bdvd")) do_umount(false);

											sys_ppu_thread_create(&thread_id_ntfs, rawseciso_thread, (uint64_t)plugin_args, -0x1d8, THREAD_STACK_SIZE_8KB, SYS_PPU_THREAD_CREATE_JOINABLE, THREAD_NAME_NTFS);

											wait_for("/dev_bdvd/PS3_GAME/PARAM.SFO", 2);
											if(file_exists("/dev_bdvd/PS3_GAME/PARAM.SFO"))
											{
												file_copy("/dev_bdvd/PS3_GAME/PARAM.SFO", path, COPY_WHOLE_FILE);

												path[plen-3]='P', path[plen-2]='N', path[plen-1]='G';
												file_copy("/dev_bdvd/PS3_GAME/ICON0.PNG", path, COPY_WHOLE_FILE);
											}

											sys_ppu_thread_create(&t, rawseciso_stop_thread, 0, 0, THREAD_STACK_SIZE_8KB, SYS_PPU_THREAD_CREATE_JOINABLE, STOP_THREAD_NAME);
											while(rawseciso_loaded) {sys_ppu_thread_usleep(50000);}
										}
									}
								}
							}
//////////////////////////////////////////////////////////////
							if(has_dirs) goto next_ntfs_entry;
//////////////////////////////////////////////////////////////
						}
						ps3ntfs_dirclose(pdir);
					}
				}
			}
		}
	}

exit_prepntfs:
	if(plugin_args) free(plugin_args);
	if(sectionsP) free(sectionsP);
	if(sections_sizeP) free(sections_sizeP);
	prepntfs_working = false;
	return count;
}
#endif
