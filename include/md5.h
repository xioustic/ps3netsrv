#ifdef CALC_MD5

#include <cell/hash/libmd5.h>

static void calc_md5(char *filename, char *md5)
{
	int fd;

	uint8_t _md5[16];
	for(uint8_t i = 0; i < 16; i++) _md5[i] = 0;

	sys_addr_t sysmem = NULL;

	if(sys_memory_allocate(_128KB_, SYS_MEMORY_PAGE_SIZE_64K, &sysmem) == CELL_OK)
	{
		if (cellFsOpen(filename, CELL_FS_O_RDONLY, &fd, NULL, 0) == 0)
		{
			CellMd5WorkArea workarea;

			cellMd5BlockInit(&workarea);

			uint8_t *buf = (uint8_t *)sysmem;

			for( ; ; )
			{
				uint64_t nread;

				cellFsRead(fd, buf, _128KB_, &nread);

				if (nread == 0) break;

				cellMd5BlockUpdate(&workarea, buf, nread);
			}

			cellFsClose(fd);
			cellMd5BlockResult(&workarea, _md5);
		}

		sys_memory_free(sysmem);
	}

	// return md5 hash as a string
	for(uint8_t i = 0; i < 16; i++) sprintf(md5 + 2*i, "%02x", _md5[i]);
}

#endif
