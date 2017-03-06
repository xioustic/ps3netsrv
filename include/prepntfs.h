#ifdef USE_NTFS
#define MAX_SECTIONS	(int)((_64KB_-sizeof(rawseciso_args))/8)

//static char paths [13][12] = {"GAMES", "GAMEZ", "PS3ISO", "BDISO", "DVDISO", "PS2ISO", "PSXISO", "PSXGAMES", "PSPISO", "ISO", "video", "GAMEI", "ROMS"};

enum ntfs_folders
{
	mPS3 = 2,
	mBLU = 3,
	mDVD = 4,
	mPS2 = 5,
	mPSX = 6,
	mMAX = 7,
};

static u8 prepntfs_working = false;

static int prepNTFS(u8 towait)
{
	if(prepntfs_working) return 0;
	prepntfs_working = true;

	int i, parts, dlen, count = 0;

	unsigned int num_tracks;
	int emu_mode = 0;
	TrackDef tracks[32];
	ScsiTrackDescriptor *scsi_tracks;

	rawseciso_args *p_args;

	CellFsDirent dir;

	DIR_ITER *pdir = NULL, *psubdir= NULL;
	struct stat st;
	bool has_dirs, is_iso = false;

	char prefix[2][8]={"/", "/PS3/"};

	uint8_t  *plugin_args = NULL;
	uint32_t *sectionsP = NULL;
	uint32_t *sections_sizeP = NULL;

	cellFsMkdir(WMTMP, S_IRWXO | S_IRWXU | S_IRWXG | S_IFDIR);
	cellFsChmod(WMTMP, S_IFDIR | 0777);
	cellFsUnlink((char*)WMTMP "/games.html");
	int fd = NONE;
	u64 read = 0;
	char path[STD_PATH_LEN], subpath[STD_PATH_LEN], sufix[8], filename[STD_PATH_LEN];

	if(mountCount == NTFS_UNMOUNTED)
		for(i = 0; i < 2; i++)
		{
			if(towait) sys_ppu_thread_sleep(2 * towait);
			mount_all_ntfs_volumes();
			if(mountCount) break;
		}

	if(!towait || mountCount <= 0)
	{
		if(cellFsOpendir(WMTMP, &fd) == CELL_FS_SUCCEEDED)
		{
			char *ext;
			while(!cellFsReaddir(fd, &dir, &read) && read)
			{
				ext = strstr(dir.d_name, ".ntfs[");
				if(ext && !IS(ext, ".ntfs[BDFILE]")) {sprintf(path, "%s/%s", WMTMP, dir.d_name); cellFsUnlink(path);}
			}
			cellFsClosedir(fd);
		}
	}

	sys_addr_t addr = NULL;
	sys_addr_t sysmem = NULL;

	if(mountCount <= 0) {mountCount = NTFS_UNMOUNTED; goto exit_prepntfs;}
	{
		sys_memory_container_t mc_app = get_app_memory_container();
		if(mc_app) sys_memory_allocate_from_container(_64KB_, mc_app, SYS_MEMORY_PAGE_SIZE_64K, &addr);
		if(!addr && sys_memory_allocate(_64KB_, SYS_MEMORY_PAGE_SIZE_64K, &addr) != CELL_OK) goto exit_prepntfs;
	}

	plugin_args    = (uint8_t *)(addr);
	sectionsP      = (uint32_t*)(addr + sizeof(rawseciso_args));
	sections_sizeP = (uint32_t*)(addr + sizeof(rawseciso_args) + _32KB_);

	size_t plen;

	for(i = 0; i < mountCount; i++)
	{
		for(u8 n = 0; n < 2; n++)
		{
			for(u8 profile = 0; profile < 5; profile++)
			{
				sprintf(sufix, "%s", SUFIX(profile));

				for(u8 m = mPS3; m < mMAX; m++)
				{
					has_dirs = false; if(m == mPS2) continue;

					snprintf(path, sizeof(path), "%s:%s%s", mounts[i].name, prefix[n], paths[m]);
					pdir = ps3ntfs_diropen(path);
					if(pdir!=NULL)
					{
						while(ps3ntfs_dirnext(pdir, dir.d_name, &st) == 0)
						{
							if(dir.d_name[0] == '.') continue;

////////////////////////////////////////////////////////
							//--- is SUBFOLDER?
							if(st.st_mode & S_IFDIR)
							{
								sprintf(subpath, "%s:%s%s%s/%s", mounts[i].name, prefix[n], paths[m], sufix, dir.d_name);
								psubdir = ps3ntfs_diropen(subpath);
								if(psubdir==NULL) continue;
								sprintf(subpath, "%s", dir.d_name); has_dirs = true;
next_ntfs_entry:
								if(ps3ntfs_dirnext(psubdir, dir.d_name, &st) < 0) {has_dirs = false; continue;}
								if(dir.d_name[0]=='.') goto next_ntfs_entry;

								dlen = sprintf(path, "%s", dir.d_name);

								is_iso = is_iso_file(dir.d_name, dlen, m, NTFS);

								if(is_iso)
								{
									dlen -= _IS(path + dlen - 6, ".iso.0") ? 6 : 4;

									if((dlen < 0) || strncmp(subpath, path, dlen))
										sprintf(filename, "[%s] %s", subpath, path);
									else
										sprintf(filename, "%s", path);

									sprintf(dir.d_name, "%s/%s", subpath, path);
								}
							}
							else
							{
								dlen = sprintf(filename, "%s", dir.d_name);

								is_iso = is_iso_file(dir.d_name, dlen, m, NTFS);
							}
////////////////////////////////////////////////////////////

							if(is_iso)
							{
								plen = snprintf(path, sizeof(path), "%s:%s%s%s/%s", mounts[i].name, prefix[n], paths[m], sufix, dir.d_name);

								parts = ps3ntfs_file_to_sectors(path, sectionsP, sections_sizeP, MAX_SECTIONS, 1);

								// get multi-part file sectors
								if(!extcasecmp(dir.d_name, ".iso.0", 6))
								{
									char iso_name[MAX_PATH_LEN], iso_path[MAX_PATH_LEN];

									size_t nlen = sprintf(iso_name, "%s", path);
									iso_name[nlen - 1] = '\0';

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
									num_tracks = 1;

										 if(m == mPS3) emu_mode = EMU_PS3;
									else if(m == mBLU) emu_mode = EMU_BD;
									else if(m == mDVD) emu_mode = EMU_DVD;
									else if(m == mPSX)
									{
										emu_mode = EMU_PSX;

										strcpy(path + plen - 3, "CUE");

										fd = ps3ntfs_open(path, O_RDONLY, 0);
										if(fd < 0)
										{
											strcpy(path + plen - 3, "cue");
											fd = ps3ntfs_open(path, O_RDONLY, 0);
										}

										if(fd >= 0)
										{
											if(sysmem || sys_memory_allocate(_64KB_, SYS_MEMORY_PAGE_SIZE_64K, &sysmem) == CELL_OK)
											{
												char *cue_buf = (char*)sysmem;
												int cue_size = ps3ntfs_read(fd, (void *)cue_buf, _4KB_);
												ps3ntfs_close(fd);

												char *templn = path;
												num_tracks = parse_cue(templn, cue_buf, cue_size, tracks);
											}
										}
									}

									p_args = (rawseciso_args *)plugin_args; memset(p_args, 0x0, sizeof(rawseciso_args));
									p_args->device = USB_MASS_STORAGE((mounts[i].interface->ioType & 0x0F));
									p_args->emu_mode = emu_mode;
									p_args->num_sections = parts;

									memcpy(plugin_args + sizeof(rawseciso_args) + (parts * sizeof(uint32_t)), sections_sizeP, parts * sizeof(uint32_t));

									if(emu_mode == EMU_PSX)
									{
										int max = MAX_SECTIONS - ((num_tracks * sizeof(ScsiTrackDescriptor)) / 8);

										if(parts >= max)
										{
											continue;
										}

										p_args->num_tracks = num_tracks;
										scsi_tracks = (ScsiTrackDescriptor *)(plugin_args + sizeof(rawseciso_args) + (2 * (parts * sizeof(uint32_t))));

										if(num_tracks <= 1)
										{
											scsi_tracks[0].adr_control = 0x14;
											scsi_tracks[0].track_number = 1;
											scsi_tracks[0].track_start_addr = 0;
										}
										else
										{
											for(unsigned int t = 0; t < num_tracks; t++)
											{
												scsi_tracks[t].adr_control = (tracks[t].is_audio) ? 0x10 : 0x14;
												scsi_tracks[t].track_number = t + 1;
												scsi_tracks[t].track_start_addr = tracks[t].lba;
											}
										}
									}

									snprintf(path, sizeof(path), "%s/%s%s.ntfs[%s]", WMTMP, filename, SUFIX2(profile), paths[m]);

									save_file(path, (char*)plugin_args, (sizeof(rawseciso_args) + (2 * (parts * sizeof(uint32_t))) + (num_tracks * sizeof(ScsiTrackDescriptor)))); count++;
/*
									plen = snprintf(path, sizeof(path), "%s/%s", WMTMP, dir.d_name);
									nlen = snprintf(path0, sizeof(path0), "%s:%s%s%s/%s", mounts[i].name, prefix[n], paths[m], sufix, dir.d_name);

									if(get_image_file(path, plen - extlen)) goto for_sfo;
									if(get_image_file(path0, nlen - extlen) == false) goto for_sfo;

									// copy external image
									path[plen-3]=path0[nlen-3], path[plen-2]=path0[nlen-2], path[plen-1]=path0[nlen-1];
									file_copy(path0, path, COPY_WHOLE_FILE);
for_sfo:
									if(m == mPS3) // mount PS3ISO
									{
										strcpy(path + plen - 3, "SFO");
										if(file_exists(path) == false)
										{
											if(isDir("/dev_bdvd")) do_umount(false);

											sys_ppu_thread_create(&thread_id_ntfs, rawseciso_thread, (uint64_t)plugin_args, THREAD_PRIO, THREAD_STACK_SIZE_NTFS_ISO, SYS_PPU_THREAD_CREATE_JOINABLE, THREAD_NAME_NTFS);

											wait_for("/dev_bdvd/PS3_GAME/PARAM.SFO", 2);

											if(file_exists("/dev_bdvd/PS3_GAME/PARAM.SFO"))
											{
												file_copy("/dev_bdvd/PS3_GAME/PARAM.SFO", path, COPY_WHOLE_FILE);

												strcpy(path + plen - 3, "PNG");
												file_copy("/dev_bdvd/PS3_GAME/ICON0.PNG", path, COPY_WHOLE_FILE);
											}

											sys_ppu_thread_t t;
											sys_ppu_thread_create(&t, rawseciso_stop_thread, 0, 0, THREAD_STACK_SIZE_STOP_THREAD, SYS_PPU_THREAD_CREATE_JOINABLE, STOP_THREAD_NAME);
											while(rawseciso_loaded) {sys_ppu_thread_usleep(50000);}
										}
									}
*/
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
	if(sysmem) sys_memory_free(sysmem);
	if(addr) sys_memory_free(addr);

	prepntfs_working = false;
	return count;
}
#endif
