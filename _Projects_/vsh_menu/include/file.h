#define FAILED              -1

////////////////////////////////////////////////////////////////////////
//                      EJECT/INSERT DISC CMD                         //
////////////////////////////////////////////////////////////////////////

static void eject_insert(uint8_t eject, uint8_t insert)
{
	uint8_t atapi_cmnd2[56];
	uint8_t* atapi_cmnd = atapi_cmnd2;
	int dev_id;

				memset(atapi_cmnd, 0, 56);
				atapi_cmnd[0x00] = 0x1b;
				atapi_cmnd[0x01] = 0x01;
	if(eject)   atapi_cmnd[0x04] = 0x02;
	if(insert)  atapi_cmnd[0x04] = 0x03;
				atapi_cmnd[0x23] = 0x0c;

	{system_call_4(600, 0x101000000000006ULL, 0, (uint64_t)(uint32_t) &dev_id, 0);}      //SC_STORAGE_OPEN
	{system_call_7(616, dev_id, 1, (uint64_t)(uint32_t) atapi_cmnd, 56, NULL, 0, NULL);} //SC_STORAGE_INSERT_EJECT
	{system_call_1(601, dev_id);}                                                        //SC_STORAGE_CLOSE
	sys_timer_sleep(2);
}


////////////////////////////////////////////////////////////////////////
//                      FILE & FOLDER FUNCTIONS                       //
////////////////////////////////////////////////////////////////////////

static int isDir(const char* path)
{
	struct CellFsStat s;
	if(cellFsStat(path, &s)==CELL_FS_SUCCEEDED)
		return ((s.st_mode & CELL_FS_S_IFDIR) != 0);
	else
		return 0;
}

static size_t read_file(const char *file, char *data, size_t size, int32_t offset)
{
	int fd = 0; uint64_t pos, read_e = 0;

	if(offset < 0) offset = 0; else memset(data, 0, size);

	if(cellFsOpen(file, CELL_FS_O_RDONLY, &fd, NULL, 0) == CELL_FS_SUCCEEDED)
	{
		if(cellFsLseek(fd, offset, CELL_FS_SEEK_SET, &pos) == CELL_FS_SUCCEEDED)
		{
			if(cellFsRead(fd, (void *)data, size, &read_e) != CELL_FS_SUCCEEDED) read_e = 0;
		}
		cellFsClose(fd);
	}

	return read_e;
}

static int file_exists(const char* path)
{
	struct CellFsStat s;
	return (cellFsStat(path, &s)==CELL_FS_SUCCEEDED);
}

static int del(char *path, bool recursive)
{
	if(!isDir(path)) {return cellFsUnlink(path);}
	if(strlen(path)<11 || !memcmp(path, "/dev_bdvd", 9) || !memcmp(path, "/dev_flash", 10) || !memcmp(path, "/dev_blind", 10)) return FAILED;

	int fd;
	uint64_t read;
	CellFsDirent dir;
	char entry[MAX_PATH_LEN];

	if(cellFsOpendir(path, &fd) == CELL_FS_SUCCEEDED)
	{
		read = sizeof(CellFsDirent);
		while(!cellFsReaddir(fd, &dir, &read))
		{
			if(!read) break;
			if(dir.d_name[0] == '.' && (dir.d_name[1] == '.' || dir.d_name[1] == 0)) continue;

			sprintf(entry, "%s/%s", path, dir.d_name);

			if(isDir(entry))
				{if(recursive) del(entry, recursive);}
			else
				cellFsUnlink(entry);
		}
		cellFsClosedir(fd);
	}
	else
		return FAILED;

	if(recursive) cellFsRmdir(path);

	return CELL_FS_SUCCEEDED;
}

////////////////////////////////////////////////////////////////////////
//                           URL ENCODING                             //
////////////////////////////////////////////////////////////////////////

static char h2a(char hex)
{
	char c = hex;
	if(c >= 0 && c <= 9)
		c += '0';
	else if(c >= 10 && c <= 15)
		c += 55; //A-F
	return c;
}

static void urlenc(char *dst, char *src)
{
	size_t j=0;
	size_t n=strlen(src);
	for(size_t i=0; i<n; i++,j++)
	{
			 if(src[i] == ' ') {dst[j++] = '%'; dst[j++] = '2'; dst[j] = '0';}
		else if(src[i] == ':') {dst[j++] = '%'; dst[j++] = '3'; dst[j] = 'A';}
		else if(src[i] & 0x80)
		{
			dst[j++] = '%';
			dst[j++] = h2a((unsigned char)src[i]>>4);
			dst[j] = h2a(src[i] & 0xf);
		}
		else if(src[i] == 34) {dst[j++] = '%'; dst[j++] = '2'; dst[j] = '2';}
		else if(src[i] == 39) {dst[j++] = '%'; dst[j++] = '2'; dst[j] = '7';}
		else dst[j] = src[i];
	}
	dst[j] = '\0';
}
