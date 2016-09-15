#ifndef LITE_EDITION
#ifdef COBRA_ONLY

typedef struct _netiso_args
{
	char server[0x40];
	char path[0x420];
	uint32_t emu_mode;
	uint32_t num_tracks;
	uint16_t port;
	uint8_t pad[6];
	ScsiTrackDescriptor tracks[1];
} __attribute__((packed)) netiso_args;

static int g_socket = -1;
static sys_event_queue_t command_queue_net = -1;

#define MAX_RETRIES    3

#define TEMP_NET_PSXISO  WMTMP "/~netpsx.iso"
#define PLAYSTATION      "PLAYSTATION "

static u8 netiso_loaded = 0;
static int netiso_svrid = -1;

static int read_remote_file(int s, void *buf, uint64_t offset, uint32_t size, int *abort_connection)
{
	netiso_read_file_cmd cmd;
	netiso_read_file_result res;

	*abort_connection = 1;

	memset(&cmd, 0, sizeof(cmd));
	cmd.opcode = (NETISO_CMD_READ_FILE);
	cmd.offset = (offset);
	cmd.num_bytes = (size);

	if(send(s, &cmd, sizeof(cmd), 0) != sizeof(cmd))
	{
		//DPRINTF("send failed (read_remote_file) (errno=%d)!\n", get_network_error());
		return FAILED;
	}

	if(recv(s, &res, sizeof(res), MSG_WAITALL) != sizeof(res))
	{
		//DPRINTF("recv failed (read_remote_file) (errno=%d)!\n", get_network_error());
		return FAILED;
	}

	*abort_connection = 0;

	int bytes_read = (res.bytes_read);
	if(bytes_read <= 0)
		return bytes_read;

	if(recv(s, buf, bytes_read, MSG_WAITALL) != bytes_read)
	{
		//DPRINTF("recv failed (read_remote_file) (errno=%d)!\n", get_network_error());
		*abort_connection = 1;
		return FAILED;
	}

	return bytes_read;
}

static u32 detect_cd_sector_size(int fd)
{
	char buffer[0x10]; buffer[0xD] = NULL; u64 pos;

	cellFsLseek(fd, 0x8020, CELL_FS_SEEK_SET, &pos); cellFsRead(fd, (void *)buffer, 0xC, NULL); if(islike(buffer, PLAYSTATION)) return 2048; else {
	cellFsLseek(fd, 0x9220, CELL_FS_SEEK_SET, &pos); cellFsRead(fd, (void *)buffer, 0xC, NULL); if(islike(buffer, PLAYSTATION)) return 2336; else {
	cellFsLseek(fd, 0x9320, CELL_FS_SEEK_SET, &pos); cellFsRead(fd, (void *)buffer, 0xC, NULL); if(islike(buffer, PLAYSTATION)) return 2352; else {
	cellFsLseek(fd, 0x9920, CELL_FS_SEEK_SET, &pos); cellFsRead(fd, (void *)buffer, 0xC, NULL); if(islike(buffer, PLAYSTATION)) return 2448; }}}

	return 2352;
}

static int64_t open_remote_file(int s, const char *path, int *abort_connection)
{
	netiso_open_cmd cmd;
	netiso_open_result res;
	int len, emu_mode = *abort_connection;

	*abort_connection = 1;

	len = strlen(path);

	memset(&cmd, 0, sizeof(cmd));
	cmd.opcode = BE16(NETISO_CMD_OPEN_FILE);
	cmd.fp_len = BE16(len);

	if(send(s, &cmd, sizeof(cmd), 0) != sizeof(cmd))
	{
		//DPRINTF("send failed (open_remote_file) (errno=%d)!\n", get_network_error());
		return FAILED;
	}

	if(send(s, path, len, 0) != len)
	{
		//DPRINTF("send failed (open_remote_file) (errno=%d)!\n", get_network_error());
		return FAILED;
	}

	if(recv(s, &res, sizeof(res), MSG_WAITALL) != sizeof(res))
	{
		//DPRINTF("recv failed (open_remote_file) (errno=%d)!\n", get_network_error());
		return FAILED;
	}

	if(res.file_size == -1)
	{
		//DPRINTF("Remote file %s doesn't exist!\n", path);
		return FAILED;
	}

	// detect CD sector size
	if((emu_mode == EMU_PSX) && (res.file_size >= _64KB_) && (res.file_size <= 0x35000000UL))
	{
		sys_addr_t sysmem = NULL; uint64_t chunk_size = _64KB_;
		if(sys_memory_allocate(chunk_size, SYS_MEMORY_PAGE_SIZE_64K, &sysmem) == CELL_OK)
		{
			char *chunk = (char*)sysmem;

			int bytes_read, fd = 0;

			bytes_read = read_remote_file(s, (char*)chunk, 0, chunk_size, abort_connection);
			if(bytes_read)
			{
				save_file(TEMP_NET_PSXISO, chunk, bytes_read);

				if(cellFsOpen(TEMP_NET_PSXISO, CELL_FS_O_RDONLY, &fd, NULL, 0) == CELL_FS_SUCCEEDED)
				{
					CD_SECTOR_SIZE_2352 = detect_cd_sector_size(fd);
					cellFsClose(fd);
				}

				cellFsUnlink(TEMP_NET_PSXISO);
			}

			sys_memory_free(sysmem);
		}
	}

	*abort_connection = 0;

	return (res.file_size);
}

static int read_remote_file_critical(uint64_t offset, void *buf, uint32_t size)
{
	netiso_read_file_critical_cmd cmd;

	memset(&cmd, 0, sizeof(cmd));
	cmd.opcode = NETISO_CMD_READ_FILE_CRITICAL;
	cmd.num_bytes = size;
	cmd.offset = offset;

	if(send(g_socket, &cmd, sizeof(cmd), 0) != sizeof(cmd))
	{
		//DPRINTF("send failed (read file) (errno=%d)!\n", sys_net_errno);
		return FAILED;
	}

	if(recv(g_socket, buf, size, MSG_WAITALL) != (int)size)
	{
		//DPRINTF("recv failed (recv file)  (errno=%d)!\n", sys_net_errno);
		return FAILED;
	}

	return CELL_OK;
}

static int process_read_cd_2048_cmd(uint8_t *buf, uint32_t start_sector, uint32_t sector_count)
{
	netiso_read_cd_2048_critical_cmd cmd;

	memset(&cmd, 0, sizeof(cmd));
	cmd.opcode = NETISO_CMD_READ_CD_2048_CRITICAL;
	cmd.start_sector = start_sector;
	cmd.sector_count = sector_count;

	if(send(g_socket, &cmd, sizeof(cmd), 0) != sizeof(cmd))
	{
		//DPRINTF("send failed (read 2048) (errno=%d)!\n", sys_net_errno);
		return FAILED;
	}

	if(recv(g_socket, buf, sector_count * CD_SECTOR_SIZE_2048, MSG_WAITALL) != (int)(sector_count * CD_SECTOR_SIZE_2048))
	{
		//DPRINTF("recv failed (read 2048)  (errno=%d)!\n", sys_net_errno);
		return FAILED;
	}

	return CELL_OK;
}

static int process_read_iso_cmd(uint8_t *buf, uint64_t offset, uint32_t size)
{
	uint64_t read_end;

	//DPRINTF("read iso: %p %lx %x\n", buf, offset, size);
	read_end = offset + size;

	if(read_end >= discsize)
	{
		//DPRINTF("Read beyond limits: %llx %x (discsize=%llx)!\n", offset, size, discsize);

		if(offset >= discsize)
		{
			memset(buf, 0, size);
			return CELL_OK;
		}

		memset(buf + (discsize - offset), 0, read_end - discsize);
		size = discsize - offset;
	}

	return read_remote_file_critical(offset, buf, size);
}

static int process_read_cd_2352_cmd(uint8_t *buf, uint32_t sector, uint32_t remaining)
{
	int cache = 0;

	if(remaining <= CD_CACHE_SIZE)
	{
		int dif = (int)cached_cd_sector-sector;

		if(ABS(dif) < CD_CACHE_SIZE)
		{
			uint8_t *copy_ptr = NULL;
			uint32_t copy_offset = 0;
			uint32_t copy_size = 0;

			if(dif > 0)
			{
				if(dif < (int)remaining)
				{
					copy_ptr = cd_cache;
					copy_offset = dif;
					copy_size = remaining-dif;
				}
			}
			else
			{
				copy_ptr = cd_cache+((-dif) * CD_SECTOR_SIZE_2352);
				copy_size = MIN((int)remaining, CD_CACHE_SIZE+dif);
			}

			if(copy_ptr)
			{
				memcpy(buf + (copy_offset * CD_SECTOR_SIZE_2352), copy_ptr, copy_size * CD_SECTOR_SIZE_2352);

				if(remaining == copy_size)
				{
					return CELL_OK;
				}

				remaining -= copy_size;

				if(dif <= 0)
				{
					uint32_t newsector = cached_cd_sector + CD_CACHE_SIZE;
					buf += ((newsector-sector) * CD_SECTOR_SIZE_2352);
					sector = newsector;
				}
			}
		}

		cache = 1;
	}

	if(!cache)
	{
		return process_read_iso_cmd(buf, sector * CD_SECTOR_SIZE_2352, remaining * CD_SECTOR_SIZE_2352);
	}

	if(!cd_cache)
	{
		sys_addr_t addr = NULL;

		int ret = sys_memory_allocate(_192KB_, SYS_MEMORY_PAGE_SIZE_64K, &addr);
		if(ret != CELL_OK)
		{
			//DPRINTF("sys_memory_allocate failed: %x\n", ret);
			return ret;
		}

		cd_cache = (uint8_t *)addr;
	}

	if(process_read_iso_cmd(cd_cache, sector * CD_SECTOR_SIZE_2352, CD_CACHE_SIZE * CD_SECTOR_SIZE_2352) != 0)
		return FAILED;

	memcpy(buf, cd_cache, remaining * CD_SECTOR_SIZE_2352);
	cached_cd_sector = sector;

	return CELL_OK;
}

static int remote_stat(int s, const char *path, int *is_directory, int64_t *file_size, uint64_t *mtime, uint64_t *ctime, uint64_t *atime, int *abort_connection)
{
	netiso_stat_cmd cmd;
	netiso_stat_result res;
	int len;

	*abort_connection = 1;

	len = strlen(path);
	memset(&cmd, 0, sizeof(cmd));
	cmd.opcode = (NETISO_CMD_STAT_FILE);
	cmd.fp_len = (len);

	if(send(s, &cmd, sizeof(cmd), 0) != sizeof(cmd))
	{
		//DPRINTF("send failed (remote_stat) (errno=%d)!\n", get_network_error());
		return FAILED;
	}

	if(send(s, path, len, 0) != len)
	{
		//DPRINTF("send failed (remote_stat) (errno=%d)!\n", get_network_error());
		return FAILED;
	}

	if(recv(s, &res, sizeof(res), MSG_WAITALL) != sizeof(res))
	{
		//DPRINTF("recv failed (remote_stat) (errno=%d)!\n", get_network_error());
		return FAILED;
	}

	*abort_connection = 0;

	*file_size = (res.file_size);
	if(*file_size == -1)
		return FAILED;

	*is_directory = res.is_directory;
	*mtime = (res.mtime);
	*ctime = (res.ctime);
	*atime = (res.atime);

	return CELL_OK;
}

static void netiso_thread(uint64_t arg)
{
	netiso_args *args;
	sys_event_port_t result_port;
	sys_event_queue_attribute_t queue_attr;
	unsigned int real_disctype;
	int64_t ret64;
	ScsiTrackDescriptor *tracks;
	int emu_mode, num_tracks;
	unsigned int cd_sector_size_param = 0;

	args = (netiso_args *)(uint32_t)arg;

	emu_mode = args->emu_mode & 0xF;

	//DPRINTF("Hello VSH\n");

	g_socket = connect_to_server(args->server, args->port);
	if(g_socket < 0 && !IS(webman_config->allow_ip, args->server))
	{
		// retry using ip of the remote connection
		g_socket = connect_to_server(webman_config->allow_ip, args->port);
	}

	if(g_socket < 0)
	{
		sys_memory_free((sys_addr_t)args);
		sys_ppu_thread_exit(0);
	}

	int ret = emu_mode;

	ret64 = open_remote_file(g_socket, args->path, &ret);
	if(ret64 < 0)
	{
		sys_memory_free((sys_addr_t)args);
		sys_ppu_thread_exit(0);
	}

	discsize = (uint64_t)ret64;

	ret = sys_event_port_create(&result_port, 1, SYS_EVENT_PORT_NO_NAME);
	if(ret != 0)
	{
		//DPRINTF("sys_event_port_create failed: %x\n", ret);
		sys_memory_free((sys_addr_t)args);
		sys_ppu_thread_exit(ret);
	}

	sys_event_queue_attribute_initialize(queue_attr);
	ret = sys_event_queue_create(&command_queue_net, &queue_attr, 0, 1);
	if(ret != 0)
	{
		//DPRINTF("sys_event_queue_create failed: %x\n", ret);
		sys_memory_free((sys_addr_t)args);
		sys_ppu_thread_exit(ret);
	}

	if(emu_mode == EMU_PSX)
	{
		num_tracks = args->num_tracks;
		tracks = &args->tracks[0];

		is_cd2352 = 1;

		if(CD_SECTOR_SIZE_2352 != 2352) cd_sector_size_param = CD_SECTOR_SIZE_2352<<4;
	}
	else
	{
		num_tracks = 0;
		tracks = NULL;
		is_cd2352 = 0;
	}

	sys_memory_free((sys_addr_t)args);
	sys_storage_ext_get_disc_type(&real_disctype, NULL, NULL);

	if(real_disctype != 0)
	{
		fake_eject_event(BDVD_DRIVE);
	}

	if(is_cd2352)
	{
		if(discsize % CD_SECTOR_SIZE_2352)
		{
			discsize = discsize - (discsize % CD_SECTOR_SIZE_2352);
		}
	}

	ret = sys_storage_ext_mount_discfile_proxy(result_port, command_queue_net, emu_mode, discsize, _256KB_, (num_tracks | cd_sector_size_param), tracks);
	//DPRINTF("mount = %x\n", ret);

	fake_insert_event(BDVD_DRIVE, real_disctype);

	if(ret != 0)
	{
		sys_event_port_destroy(result_port);
		sys_ppu_thread_exit(0);
	}

	netiso_loaded = 1;

	while(netiso_loaded)
	{
		sys_event_t event;

		ret = sys_event_queue_receive(command_queue_net, &event, 0);
		if(ret != 0)
		{
			//DPRINTF("sys_event_queue_receive failed: %x\n", ret);
			break;
		}

		if(!netiso_loaded) break;

		void *buf = (void *)(uint32_t)(event.data3>>32ULL);
		uint64_t offset = event.data2;
		uint32_t size = event.data3&0xFFFFFFFF;

		switch(event.data1)
		{
			case CMD_READ_ISO:
			{
				if(is_cd2352)
				{
					ret = process_read_cd_2048_cmd(buf, offset / CD_SECTOR_SIZE_2048, size / CD_SECTOR_SIZE_2048);
				}
				else
				{
					ret = process_read_iso_cmd(buf, offset, size);
				}
			}
			break;

			case CMD_READ_CD_ISO_2352:
			{
				ret = process_read_cd_2352_cmd(buf, offset / CD_SECTOR_SIZE_2352, size / CD_SECTOR_SIZE_2352);
			}
			break;
		}

		while(netiso_loaded)
		{
			ret = sys_event_port_send(result_port, ret, 0, 0);
			if(ret == 0) break;

			if(ret == (int) 0x8001000A)
			{   // EBUSY
				sys_timer_usleep(100000);
				continue;
			}

			break;
		}

		//DPRINTF("sys_event_port_send failed: %x\n", ret);
		if(ret != 0) break;
	}

	sys_storage_ext_get_disc_type(&real_disctype, NULL, NULL);
	fake_eject_event(BDVD_DRIVE);
	sys_storage_ext_umount_discfile();

	if(real_disctype != 0)
	{
		fake_insert_event(BDVD_DRIVE, real_disctype);
	}

	if(cd_cache)
	{
		sys_memory_free((sys_addr_t)cd_cache);
	}

	if(g_socket >= 0)
	{
		shutdown(g_socket, SHUT_RDWR);
		socketclose(g_socket);
		g_socket = -1;
	}

	sys_event_port_disconnect(result_port);
	if(sys_event_port_destroy(result_port) != 0)
	{
		//DPRINTF("Error destroyng result_port\n");
	}

	//DPRINTF("Exiting main thread!\n");
	netiso_loaded = 0;
	netiso_svrid = -1;

	sys_ppu_thread_exit(0);
}

static void netiso_stop_thread(uint64_t arg)
{
	uint64_t exit_code;
	netiso_loaded = 0;
	netiso_svrid = -1;

	if(g_socket >= 0)
	{
		shutdown(g_socket, SHUT_RDWR);
		socketclose(g_socket);
		g_socket = -1;
	}

	if(command_queue_net != (sys_event_queue_t)-1)
	{
		if(sys_event_queue_destroy(command_queue_net, SYS_EVENT_QUEUE_DESTROY_FORCE) != 0)
		{
			//DPRINTF("Failed in destroying command_queue_net\n");
		}
	}

	if(thread_id_net != SYS_PPU_THREAD_NONE)
	{
		sys_ppu_thread_join(thread_id_net, &exit_code);
	}

	sys_ppu_thread_exit(0);
}

static bool is_netsrv_enabled(u8 server_id)
{
	return( (server_id == 0 && (webman_config->netd0 && webman_config->neth0[0] && webman_config->netp0))
		||	(server_id == 1 && (webman_config->netd1 && webman_config->neth1[0] && webman_config->netp1))
		||	(server_id == 2 && (webman_config->netd2 && webman_config->neth2[0] && webman_config->netp2))
#ifdef NET3NET4
		||	(server_id == 3 && (webman_config->netd3 && webman_config->neth3[0] && webman_config->netp3))
		||	(server_id == 4 && (webman_config->netd4 && webman_config->neth4[0] && webman_config->netp4))
#endif
		  );
}

static int connect_to_remote_server(u8 server_id)
{
	int ns = FAILED;

	if( is_netsrv_enabled(server_id) )
	{
		// check duplicated connections
		if(server_id == 1 && webman_config->netd0 && IS(webman_config->neth0, webman_config->neth1) && webman_config->netp0 == webman_config->netp1) return FAILED;

		if(server_id == 2 && webman_config->netd0 && IS(webman_config->neth0, webman_config->neth2) && webman_config->netp0 == webman_config->netp2) return FAILED;
		if(server_id == 2 && webman_config->netd1 && IS(webman_config->neth1, webman_config->neth2) && webman_config->netp1 == webman_config->netp2) return FAILED;
#ifdef NET3NET4
		if(server_id == 3 && webman_config->netd0 && IS(webman_config->neth0, webman_config->neth3) && webman_config->netp0 == webman_config->netp3) return FAILED;
		if(server_id == 3 && webman_config->netd1 && IS(webman_config->neth1, webman_config->neth3) && webman_config->netp1 == webman_config->netp3) return FAILED;
		if(server_id == 3 && webman_config->netd2 && IS(webman_config->neth2, webman_config->neth3) && webman_config->netp2 == webman_config->netp3) return FAILED;

		if(server_id == 4 && webman_config->netd0 && IS(webman_config->neth0, webman_config->neth4) && webman_config->netp0 == webman_config->netp4) return FAILED;
		if(server_id == 4 && webman_config->netd1 && IS(webman_config->neth1, webman_config->neth4) && webman_config->netp1 == webman_config->netp4) return FAILED;
		if(server_id == 4 && webman_config->netd2 && IS(webman_config->neth2, webman_config->neth4) && webman_config->netp2 == webman_config->netp4) return FAILED;
		if(server_id == 4 && webman_config->netd3 && IS(webman_config->neth3, webman_config->neth4) && webman_config->netp3 == webman_config->netp4) return FAILED;
#endif
		u8 retries = 0;

	reconnect:
		if(server_id == 0) ns = connect_to_server(webman_config->neth0, webman_config->netp0);
		if(server_id == 1) ns = connect_to_server(webman_config->neth1, webman_config->netp1);
		if(server_id == 2) ns = connect_to_server(webman_config->neth2, webman_config->netp2);
#ifdef NET3NET4
		if(server_id == 3) ns = connect_to_server(webman_config->neth3, webman_config->netp3);
		if(server_id == 4) ns = connect_to_server(webman_config->neth4, webman_config->netp4);
#endif
		if(ns<0)
		{
			if(retries < MAX_RETRIES)
			{
				retries++;
				sys_timer_sleep(1);
				goto reconnect;
			}

			// retry using IP of client (/net0 only) - update IP in neth0 if connection is successful
			if(server_id == 0 && webman_config->netd0 && !IS(webman_config->allow_ip, webman_config->neth1) && !IS(webman_config->allow_ip, webman_config->neth2))
			{
				ns = connect_to_server(webman_config->allow_ip, webman_config->netp0);
				if(ns >= 0) strcpy(webman_config->neth0, webman_config->allow_ip);
			}
		}
	}
	return ns;
}

static int open_remote_dir(int s, const char *path, int *abort_connection)
{
	netiso_open_dir_cmd cmd;
	netiso_open_dir_result res;
	int len;

	*abort_connection = 0;

	len = strlen(path);
	memset(&cmd, 0, sizeof(cmd));
	cmd.opcode = (NETISO_CMD_OPEN_DIR);
	cmd.dp_len = (len);

	if(send(s, &cmd, sizeof(cmd), 0) != sizeof(cmd))
	{
		//DPRINTF("send failed (open_remote_dir) (errno=%d)!\n", get_network_error());
		*abort_connection = 1;
		return FAILED;
	}

	if(send(s, path, len, 0) != len)
	{
		//DPRINTF("send failed (open_remote_dir) (errno=%d)!\n", get_network_error());
		*abort_connection = 1;
		return FAILED;
	}

	if(recv(s, &res, sizeof(res), MSG_WAITALL) != sizeof(res))
	{
		//DPRINTF("recv failed (open_remote_dir) (errno=%d)!\n", get_network_error());
		*abort_connection = 1;
		return FAILED;
	}

	return (res.open_result);
}

static int read_remote_dir(int s, sys_addr_t *data /*netiso_read_dir_result_data **data*/, int *abort_connection)
{
	netiso_read_dir_entry_cmd cmd;
	netiso_read_dir_result res;
	int len;

	*abort_connection = 1;

	memset(&cmd, 0, sizeof(cmd));
	cmd.opcode = (NETISO_CMD_READ_DIR);

	//MM_LOG("Sending request...(%i) ", s);
	if(send(s, &cmd, sizeof(cmd), 0) != sizeof(cmd))
	{
	//MM_LOG("FAILED!\n");
		return FAILED;
	}
	//MM_LOG("Receiving response...");
	if(recv(s, &res, sizeof(res), MSG_WAITALL) != sizeof(res))
	{
	//MM_LOG("FAILED!\n");
		return FAILED;
	}

	//MM_LOG("OK (%i entries)\n", res.dir_size );
	if(res.dir_size > 0)
	{
		sys_addr_t data1 = NULL;
		for(int64_t retry = 16; retry > 0; retry--)
		{
			if(res.dir_size > (retry * 123)) res.dir_size = retry * 123;

			len = (sizeof(netiso_read_dir_result_data)*res.dir_size);
			int len2 = ((len + _64KB_) / _64KB_) * _64KB_;
			if(sys_memory_allocate(len2, SYS_MEMORY_PAGE_SIZE_64K, &data1) == CELL_OK)
			{
				*data = data1;
				u8 *data2 = (u8*)data1;

				if(recv(s, data2, len, MSG_WAITALL) != len)
				{
					sys_memory_free(data1);
					*data = NULL;
					return FAILED;
				}
				break;
			}
			else
				*data = NULL;
		}
	}
	else
		*data = NULL;

	*abort_connection = 0;

	return (res.dir_size);
}

static int copy_net_file(const char *local_file, const char *remote_file, int ns, uint64_t maxbytes)
{
	copy_aborted = false;

	if(ns < 0) return FAILED;

	if(file_exists(local_file)) return CELL_OK; // local file already exists

	int abort_connection=0, is_directory=0, fdw = 0; int64_t file_size; u64 mtime, ctime, atime;

	if(remote_stat(ns, remote_file, &is_directory, &file_size, &mtime, &ctime, &atime, &abort_connection)!=0) return FAILED;

	if(file_size <= 0) return FAILED;

	if(maxbytes > 0UL && (uint64_t)file_size > maxbytes) file_size = maxbytes;

	sys_addr_t sysmem = NULL; uint64_t chunk_size = _64KB_;

	if(sys_memory_allocate(chunk_size, SYS_MEMORY_PAGE_SIZE_64K, &sysmem) == CELL_OK)
	{
		char *chunk = (char*)sysmem;

		if(cellFsOpen(local_file, CELL_FS_O_CREAT | CELL_FS_O_TRUNC | CELL_FS_O_WRONLY, &fdw, NULL, 0) == CELL_FS_SUCCEEDED)
		{
			open_remote_file(ns, remote_file, &abort_connection);

			int bytes_read, boff = 0;
			while(boff < file_size)
			{
				if(copy_aborted) break;

				bytes_read = read_remote_file(ns, (char*)chunk, boff, chunk_size, &abort_connection);
				if(bytes_read)
					cellFsWrite(fdw, (char*)chunk, bytes_read, NULL);

				boff += bytes_read;
				if((uint64_t)bytes_read < chunk_size || abort_connection) break;
			}

			open_remote_file(ns, (char*)"/CLOSEFILE", &abort_connection);
			cellFsClose(fdw);
			sys_memory_free(sysmem);

			cellFsChmod(local_file, MODE);
			return CELL_OK;
		}

		sys_memory_free(sysmem);
	}

	return FAILED;
}
#endif
#endif //#ifndef LITE_EDITION
