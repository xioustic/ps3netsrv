#define SLAUNCH_FILE			"/dev_hdd0/tmp/wmtmp/slaunch.bin"

#ifdef SLAUNCH_FILE
typedef struct
{
   char path[128];
   char icon[128];
   char name[128];
} __attribute__((packed)) _slaunch;

static int create_slaunch_file(void)
{
	int fd;
	if(cellFsOpen(SLAUNCH_FILE, CELL_FS_O_CREAT | CELL_FS_O_TRUNC | CELL_FS_O_WRONLY, &fd, NULL, 0) == CELL_FS_SUCCEEDED)
		return fd;
	else
		return 0;
}

static void add_slaunch_entry(int fd, const char *neth, const char *path, const char *filename, const char *icon, const char *name)
{
	if(!fd) return;

	char sLaunch[sizeof(_slaunch)];
	_slaunch *slaunch = (_slaunch*) sLaunch;

	memset(sLaunch, 0, sizeof(_slaunch));
	snprintf(slaunch->path, 128, "/mount_ps3%s%s/%s", neth, path, filename);
	snprintf(slaunch->icon, 128, "%s", icon);
	snprintf(slaunch->name, 128, "%s", name);

	cellFsWrite(fd, (char*)sLaunch, 384, NULL);
}

static void close_slaunch_file(int fd)
{
	if(!fd) return;

	cellFsClose(fd);
	cellFsChmod(SLAUNCH_FILE, MODE);
}
#endif
