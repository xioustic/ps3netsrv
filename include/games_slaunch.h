#define SLAUNCH_FILE		"/dev_hdd0/tmp/wmtmp/slaunch.bin"

#define MAX_SLAUNCH_ITEMS	1000

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

	_slaunch slaunch; char enc_filename[MAX_PATH_LEN];

	memset(&slaunch, 0, sizeof(_slaunch)); urlenc_ex(enc_filename, filename, false);
	snprintf(slaunch.path, sizeof(slaunch.path), "/mount_ps3%s%s/%s", neth, path, enc_filename);
	snprintf(slaunch.icon, sizeof(slaunch.icon), "%s", icon);
	snprintf(slaunch.name, sizeof(slaunch.name), "%s", name);

	cellFsWrite(fd, (void *)&slaunch, sizeof(_slaunch), NULL);
}

static void close_slaunch_file(int fd)
{
	if(!fd) return;

	cellFsClose(fd);
	cellFsChmod(SLAUNCH_FILE, MODE);
}
#endif
