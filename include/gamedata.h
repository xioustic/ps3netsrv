#ifdef EXT_GDATA

#define MOUNT_EXT_GDATA		2

static u8 extgd = 0;       //external gameDATA

static int set_gamedata_status(u8 status, bool do_mount)
{
	char msg[100];
	char gamei_path[MAX_PATH_LEN]; u8 n = 0;

	{ PS3MAPI_ENABLE_ACCESS_SYSCALL8 }

#ifndef COBRA_ONLY
	sprintf(gamei_path, "/%s", "/dev_hdd0/game");
	if(do_mount) max_mapped = 0;
#endif

	if(status)
	{
		if(status == 2)
		{
			sprintf(gamei_path, "/dev_bdvd/GAMEI");
			status = 1;
		}
		else
		{
			for(n = 1; n < 7; n++) {sprintf(gamei_path, "%s/GAMEI", drives[n]); if(isDir(gamei_path)) break;} // find first USB HDD with /GAMEI

			if(n > 7)
			{
				for(n = 1; n < 7; n++) {if(isDir(drives[n])) break;} // find first USB HDD

				if(n < 7) {sprintf(gamei_path, "%s/GAMEI", drives[n]); if(cellFsMkdir(gamei_path, MODE) != CELL_FS_SUCCEEDED) n = 99;}
			}
		}

		if(n < 7)
		{
#ifdef COBRA_ONLY
			sys_map_path("/dev_hdd0/game", gamei_path);
			if(isDir(MM_ROOT_STD)) sys_map_path(MM_ROOT_STD, "/" MM_ROOT_STD);
#else
			if(isDir(MM_ROOT_STD)) add_to_map(MM_ROOT_STD, MM_ROOT_STD);
			add_to_map("/dev_hdd0/game", gamei_path);
#endif
			sprintf(msg, "gameDATA %s (%s)", STR_ENABLED, drives[n]);
		}
		else
		{
			extgd = 0;
#ifdef COBRA_ONLY
			sys_map_path("/dev_hdd0/game", NULL);
			{ PS3MAPI_DISABLE_ACCESS_SYSCALL8 }
#endif
			sprintf(msg, "gameDATA %s (no usb)", STR_ERROR);
			show_msg(msg);
			return FAILED;
		}
	}
	else
	{
		sprintf(msg, "gameDATA %s", STR_DISABLED);

#ifdef COBRA_ONLY
		sys_map_path("/dev_hdd0/game", NULL);
#else
		add_to_map("/dev_hdd0/game", gamei_path);
#endif
	}

	extgd = status;

	{ PS3MAPI_DISABLE_ACCESS_SYSCALL8 }

	if(do_mount)
	{
		show_msg(msg);
#ifndef COBRA_ONLY
		mount_with_mm(gamei_path, MOUNT_EXT_GDATA);
#endif
	}

	return CELL_OK;
}

#endif //#ifdef EXT_GDATA
