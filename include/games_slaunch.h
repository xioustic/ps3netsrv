#define SLAUNCH_FILE		"/dev_hdd0/tmp/wmtmp/slaunch.bin"

#define MAX_SLAUNCH_ITEMS	1000

#define TYPE_ALL 0
#define TYPE_PS1 1
#define TYPE_PS2 2
#define TYPE_PS3 3
#define TYPE_PSP 4
#define TYPE_VID 5
#define TYPE_ROM 6
#define TYPE_MAX 7

#ifdef SLAUNCH_FILE
typedef struct
{
	char path[128];
	char icon[128];
	char name[112];
	char padding[5];
	char id[10];
	uint8_t type;
} __attribute__((packed)) _slaunch;

static int create_slaunch_file(void)
{
	int fd;
	if(cellFsOpen(SLAUNCH_FILE, CELL_FS_O_CREAT | CELL_FS_O_TRUNC | CELL_FS_O_WRONLY, &fd, NULL, 0) == CELL_FS_SUCCEEDED)
		return fd;
	else
		return 0;
}

static void add_slaunch_entry(int fd, const char *neth, const char *path, const char *filename, const char *icon, const char *name, const char *id, u8 f1)
{
	if(!fd) return;

	_slaunch slaunch; memset(&slaunch, 0, sizeof(_slaunch));

	char enc_filename[MAX_PATH_LEN]; urlenc_ex(enc_filename, filename, false);

	slaunch.type = IS_ROMS_FOLDER ? TYPE_ROM : IS_PS3_TYPE ? TYPE_PS3 : IS_PSX_FOLDER ? TYPE_PS1 : IS_PS2_FOLDER ? TYPE_PS2 : IS_PSP_FOLDER ? TYPE_PSP : TYPE_VID;

	snprintf(slaunch.path, 127, "/mount_ps3%s%s/%s", neth, path, enc_filename);
	snprintf(slaunch.icon, 127, "%s", icon);
	snprintf(slaunch.name, 110, "%s", name);
	snprintf(slaunch.id,   sizeof(slaunch.id),   "%s", id);

	cellFsWrite(fd, (void *)&slaunch, sizeof(_slaunch), NULL);
}

static void close_slaunch_file(int fd)
{
	if(!fd) return;

	cellFsClose(fd);
	cellFsChmod(SLAUNCH_FILE, MODE);
}
#endif
