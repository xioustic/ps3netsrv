#ifdef NOSINGSTAR

#define XIL2_DIR	"/dev_hdd0/tmp/explore/xil2/game"

static void no_singstar_icon(void)
{
	int fd;

	if(cellFsOpendir(XIL2_DIR, &fd) == CELL_FS_SUCCEEDED)
	{
		char *country_id;

		CellFsDirent dir; u64 read_e;

		while(working && (cellFsReaddir(fd, &dir, &read_e) == CELL_FS_SUCCEEDED) && (read_e > 0))
		{
			country_id = dir.d_name;

			if(country_id[0] == '.') continue;
			if(country_id[2] == '\0' && country_id[1] != '\0')
			{
				char country_path[48];

				if(webman_config->noss)
				{
					int fd2;

					sprintf(country_path, "%s/%s/c", XIL2_DIR, country_id);

					if(cellFsOpendir(country_path, &fd2) == CELL_FS_SUCCEEDED)
					{
						char xmlpath[96]; to_upper(country_id);

						CellFsDirent dir2;

						while(working && (cellFsReaddir(fd2, &dir2, &read_e) == CELL_FS_SUCCEEDED) && (read_e > 0))
						{
							if(dir2.d_name[0] == '.') continue;

							sprintf(xmlpath, "%s/%s/NSXWSV-PN.P3.%s-XMB_COLUMN_GAME.xml", country_path, dir2.d_name, country_id);

							if(file_exists(xmlpath))
							{
								save_file(xmlpath, NULL, 0);
								cellFsChmod(xmlpath, 0444); // make blank xml read only
							}
						}
						cellFsClosedir(fd2);
					}
				}
				else
				{
					sprintf(country_path, "%s/%s", XIL2_DIR, country_id);
					del(country_path, RECURSIVE_DELETE); // allow restore singstar icon
				}
			}
		}
		cellFsClosedir(fd);
	}
}
#endif
