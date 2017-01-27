#ifdef USE_NTFS
#define MAX_SECTIONS		(int)((0x10000-sizeof(rawseciso_args))/8)

static void prepNTFS(u8 towait)
{
	char path[256];

	int i, parts;
	unsigned int num_tracks;
	u8 cue=0;
	int emu_mode=0;
	TrackDef tracks[32];
	ScsiTrackDescriptor *scsi_tracks;

	rawseciso_args *p_args;

	CellFsDirent dir;
	DIR_ITER *pdir = NULL;
	struct stat st;
	char c_path[4][8] = {"PS3ISO", "BDISO", "DVDISO", "PSXISO"};

	snprintf(path, sizeof(path), WMTMP);
	cellFsMkdir(path, S_IRWXO | S_IRWXU | S_IRWXG | S_IFDIR);
	cellFsChmod(path, S_IFDIR | 0777);
	cellFsUnlink((char*)WMTMP "/games.html");
	int fd = -1;
	u64 read = 0;
	char path0[MAX_PATH_LEN];
	if(cellFsOpendir(path, &fd) == CELL_FS_SUCCEEDED)
	{
		while(!cellFsReaddir(fd, &dir, &read) && read)
			if(strstr(dir.d_name, ".ntfs[")) {sprintf(path0, "%s/%s", path, dir.d_name); cellFsUnlink(path0);}
		cellFsClosedir(fd);
	}

	if(mountCount == -2)
	for (i = 0; i < 2; i++)
	{
		if(towait) sys_timer_sleep(2 * towait);
		mount_all_ntfs_volumes();
		if(mountCount) break;
	}

	if (mountCount <= 0) {mountCount=-2; return;}

	uint8_t* plugin_args = (uint8_t *)malloc(_64KB_); if(!plugin_args) return;
	uint32_t* sectionsP = (uint32_t *)malloc(_32KB_); if(!sectionsP) {free(plugin_args); return;}
	uint32_t* sections_sizeP = (uint32_t *)malloc(_32KB_); if(!sections_sizeP) {free(sectionsP); free(plugin_args); return;}

	for (i = 0; i < mountCount; i++)
	{
		{
			for(u8 m = 0; m < 4; m++)
			{
				snprintf(path, sizeof(path), "%s:/%s", mounts[i].name, c_path[m]);
				pdir = ps3ntfs_diropen(path);
				if(pdir!=NULL)
				{
					while(ps3ntfs_dirnext(pdir, dir.d_name, &st) == 0)
					{
						if( !extcasecmp(dir.d_name, ".iso", 4) ||
							!extcasecmp(dir.d_name, ".iso.0", 6) ||
							(m==3 && !extcasecmp(dir.d_name, ".bin", 4)))
						{
							snprintf(path, sizeof(path), "%s:/%s/%s", mounts[i].name, c_path[m], dir.d_name);
							parts = ps3ntfs_file_to_sectors(path, sectionsP, sections_sizeP, MAX_SECTIONS, 1);

							// get multi-part file sectors
							if(!extcasecmp(dir.d_name, ".iso.0", 6))
							{
								char iso_name[MAX_PATH_LEN], iso_path[MAX_PATH_LEN];

								size_t nlen = sprintf(iso_name, "%s", path);
								iso_name[nlen - 1] = '\0';

								for (u8 o = 1; o < 64; o++)
								{
									if(parts >= MAX_SECTIONS) break;

									sprintf(iso_path, "%s%i", iso_name, o);
									if(file_exists(iso_path) == false) break;

									parts += ps3ntfs_file_to_sectors(iso_path, sectionsP + (parts * sizeof(uint32_t)), sections_sizeP + (parts * sizeof(uint32_t)), MAX_SECTIONS - parts, 1);
								}
							}

							if (parts >= MAX_SECTIONS)
							{
								continue;
							}
							else if (parts > 0)
							{
								num_tracks = 1;
									 if(m == 0) emu_mode = EMU_PS3;
								else if(m == 1) emu_mode = EMU_BD;
								else if(m == 2) emu_mode = EMU_DVD;
								else if(m == 3)
								{
									emu_mode = EMU_PSX;
									cue = 0;
									int fd;

									size_t plen = strlen(path);
									path[plen-3]='C'; path[plen-2]='U'; path[plen-1]='E';
									fd = ps3ntfs_open(path, O_RDONLY, 0);
									if(fd<0)
									{
										path[plen-3]='c'; path[plen-2]='u'; path[plen-1]='e';
										fd = ps3ntfs_open(path, O_RDONLY, 0);
									}

									if(fd >= 0)
									{
										uint8_t cue_buf[2048];
										int r = ps3ntfs_read(fd, (char *)cue_buf, sizeof(cue_buf));
										ps3ntfs_close(fd);

										if(r > 0)
										{
											char dummy[64];

											if (cobra_parse_cue(cue_buf, r, tracks, 100, &num_tracks, dummy, sizeof(dummy)-1) != 0)
											{
												num_tracks=1;
												cue=0;
											}
											else
												cue=1;
										}
									}
								}

								p_args = (rawseciso_args *)plugin_args; memset(p_args, 0x0, _64KB_);
								p_args->device = USB_MASS_STORAGE((mounts[i].interface->ioType & 0xff) - '0');
								p_args->emu_mode = emu_mode;
								p_args->num_sections = parts;

								memcpy(plugin_args+sizeof(rawseciso_args), sectionsP, parts*sizeof(uint32_t));
								memcpy(plugin_args+sizeof(rawseciso_args)+(parts*sizeof(uint32_t)), sections_sizeP, parts*sizeof(uint32_t));

								if(emu_mode == EMU_PSX)
								{
									int max = MAX_SECTIONS - ((num_tracks*sizeof(ScsiTrackDescriptor)) / 8);

									if (parts == max)
									{
										continue;
									}

									p_args->num_tracks = num_tracks;
									scsi_tracks = (ScsiTrackDescriptor *)(plugin_args+sizeof(rawseciso_args)+(2*parts*sizeof(uint32_t)));

									if (!cue)
									{
										scsi_tracks[0].adr_control = 0x14;
										scsi_tracks[0].track_number = 1;
										scsi_tracks[0].track_start_addr = 0;
									}
									else
									{
										for (u8 j = 0; j < num_tracks; j++)
										{
											scsi_tracks[j].adr_control = (tracks[j].is_audio) ? 0x10 : 0x14;
											scsi_tracks[j].track_number = j+1;
											scsi_tracks[j].track_start_addr = tracks[j].lba;
										}
									}
								}

								snprintf(path, sizeof(path), WMTMP "/%s.ntfs[%s]", dir.d_name, c_path[m]);
								if(cellFsOpen(path, CELL_FS_O_CREAT | CELL_FS_O_TRUNC | CELL_FS_O_WRONLY, &fd, NULL, 0) == CELL_FS_SUCCEEDED)
								{
									cellFsWrite(fd, plugin_args, _64KB_, NULL);
									cellFsClose(fd);
									cellFsChmod(path, 0666);
								}
							}
						}
					}
					ps3ntfs_dirclose(pdir);
				}
			}
		}
	}

	free(sections_sizeP); free(sectionsP); free(plugin_args);
	return;
}
#endif
