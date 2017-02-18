#ifdef CALC_MD5

#include <cell/hash/libmd5.h>

static void calc_md5(char *filename, char *md5)
{
	int fd;

	uint8_t _md5[16]; memset(_md5, 0, 16);

	sys_addr_t sysmem = NULL; size_t buffer_size = _256KB_;

	if(!webman_config->mc_app)
	{
		sys_memory_container_t mc_app = get_app_memory_container();
		if(mc_app)	sys_memory_allocate_from_container(buffer_size, mc_app, SYS_MEMORY_PAGE_SIZE_64K, &sysmem);
	}

	if(!sysmem) buffer_size = _128KB_;

	if(sysmem || (!sysmem && sys_memory_allocate(buffer_size, SYS_MEMORY_PAGE_SIZE_64K, &sysmem) == CELL_OK))
	{
		if (cellFsOpen(filename, CELL_FS_O_RDONLY, &fd, NULL, 0) == 0)
		{
			CellMd5WorkArea workarea;

			cellMd5BlockInit(&workarea);

			uint8_t *buf = (uint8_t *)sysmem;

			for( ; ; )
			{
				uint64_t nread;

				cellFsRead(fd, buf, buffer_size, &nread);

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
