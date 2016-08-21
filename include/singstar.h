#ifdef NOSINGSTAR
static void no_singstar_icon(void)
{
	int fd;

	if(cellFsOpendir("/dev_hdd0/tmp/explore/xil2/game", &fd) == CELL_FS_SUCCEEDED)
	{
		u64 read; CellFsDirent dir; char xmlpath[64];
		read = sizeof(CellFsDirent);
		while(!cellFsReaddir(fd, &dir, &read))
		{
			if(!read) break;
			if(dir.d_name[0] == '.') continue;
			if(dir.d_name[2] == '\0' && dir.d_name[1] != '\0')
			{
				sprintf(xmlpath, "%s/%s/c/db.xml", "/dev_hdd0/tmp/explore/xil2/game", dir.d_name);
				cellFsUnlink(xmlpath);
			}
		}
		cellFsClosedir(fd);
	}
}
#endif
