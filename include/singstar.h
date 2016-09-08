#ifdef NOSINGSTAR
static void no_singstar_icon(void)
{
	int fd;

	if(cellFsOpendir("/dev_hdd0/tmp/explore/xil2/game", &fd) == CELL_FS_SUCCEEDED)
	{
		char xmlpath[64];
		CellFsDirent dir; u64 read_e;

		while((cellFsReaddir(fd, &dir, &read_e) == CELL_FS_SUCCEEDED) && (read_e > 0))
		{
			if(dir.d_name[0] == '.') continue;
			if(dir.d_name[2] == '\0' && dir.d_name[1] != '\0')
			{
				sprintf(xmlpath, "%s/%s/c/db.xml", "/dev_hdd0/tmp/explore/xil2/game", dir.d_name);
				savefile(xmlpath, NULL, 0);
				cellFsChmod(xmlpath, 0444);
			}
		}
		cellFsClosedir(fd);
	}
}
#endif
