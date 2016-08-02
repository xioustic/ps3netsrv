#ifdef PKG_HANDLER

// /install.ps3<pkg-path>
// /download.ps3?url=<url>
// /download.ps3?to=<path>&url=<url>

#include <cell/http.h>

#include "../vsh/xmb_plugin.h"
#include "../vsh/game_ext_plugin.h"

#include "../vsh/webbrowser_plugin.h"
#include "../vsh/webrender_plugin.h"
#include "../vsh/download_plugin.h"
#include "../vsh/stdc.h"

#define MAX_URL_LEN    360
#define MAX_DLPATH_LEN 240

#define MAX_PKGPATH_LEN 240
static char pkg_path[MAX_PKGPATH_LEN];

static wchar_t pkg_durl[MAX_URL_LEN];
static wchar_t pkg_dpath[MAX_DLPATH_LEN];

static bool wmget = false;

#define INT_HDD_ROOT_PATH		"/dev_hdd0/"
#define DEFAULT_PKG_PATH		"/dev_hdd0/packages/"

static int LoadPluginById(int id, void *handler)
{
	if(xmm0_interface == 0) // getting xmb_plugin xmm0 interface for loading plugin sprx
	{
		xmm0_interface = (xmb_plugin_xmm0 *)plugin_GetInterface(View_Find("xmb_plugin"), 'XMM0');
	}
	return xmm0_interface->LoadPlugin3(id, handler,0);
}

static int UnloadPluginById(int id, void *handler)
{
	if(xmm0_interface == 0) // getting xmb_plugin xmm0 interface for loading plugin sprx
	{
		xmm0_interface = (xmb_plugin_xmm0 *)plugin_GetInterface(View_Find("xmb_plugin"), 'XMM0');
	}
	return xmm0_interface->Shutdown(id, handler,1);
}

static void unloadSysPluginCallback(void)
{
	//Add potential callback process
	//show_msg((char *)"plugin shutdown via xmb call launched");
}

static void downloadPKG_thread(void)
{

	if(download_interface == 0) // test if download_interface is loaded for interface access
	{
		download_interface = (download_plugin_interface *)plugin_GetInterface(View_Find("download_plugin"),1);
	}

	download_interface->DoUnk5(0, pkg_durl, pkg_dpath);
}

static void installPKG_thread(void)
{
	if(game_ext_interface == 0) // test if game_ext_plugin is loaded for interface access
	{
		game_ext_interface = (game_ext_plugin_interface *)plugin_GetInterface(View_Find("game_ext_plugin"),1);
	}
	game_ext_interface->DoUnk0(); // Load Page
	game_ext_interface->DoUnk34(pkg_path); // install PKG from path
}

static int download_file(char *param, char *msg)
{
	int ret = FAILED;

	if(View_Find("game_plugin"))
	{
		sprintf(msg, (const char *)"ERROR: download from XMB");
		return ret;
	}

	char *msg_durl = msg;
	char *msg_dpath = msg + MAX_URL_LEN + 16;

	char pdurl[MAX_URL_LEN] = "";
	char pdpath[MAX_DLPATH_LEN] = "";

	size_t conv_num_durl = 0;
	size_t conv_num_dpath = 0;

	size_t ptemp_len;
	int pdurl_len;

	size_t dparam_len;
	int pdpath_len;

	wmemset(pkg_durl, 0, MAX_URL_LEN); // Use wmemset from stdc.h instead of reinitialising wchar_t with a loop.
	wmemset(pkg_dpath, 0, MAX_DLPATH_LEN);

	memset(pdurl, 0, MAX_URL_LEN);
	memset(pdpath, 0, MAX_DLPATH_LEN);

	sprintf(msg_durl,  (const char *)"ERROR: Invalid URL");
	sprintf(msg_dpath, (const char *)"Download canceled");

	if(islike(param + 13, "?to="))  //Use of the optional parameter
	{
		char *ptemp = strstr(param,(const char *)"&url=");

		if(ptemp != NULL)
		{
			ptemp_len = strlen((const char *)ptemp);
			pdurl_len = ptemp_len - 5;
			if((pdurl_len > 0) && (pdurl_len < MAX_URL_LEN))
			{
				strncpy(pdurl, ptemp + 5, pdurl_len);
			}
			else
				goto end_download_process;
		}
		else
			goto end_download_process;

		dparam_len = strlen((const char *)param + 13);
		pdpath_len = dparam_len - ptemp_len - 4;

		if(pdpath_len > 0) strncpy(pdpath, (const char *)param + 17, pdpath_len);

		conv_num_durl = mbstowcs((wchar_t *)pkg_durl, (const char *)pdurl, pdurl_len + 1);  //size_t stdc_FCAC2E8E(wchar_t *dest, const char *src, size_t max)

	}
	else if(islike(param + 13, "?url="))
	{
		pdurl_len = strlen(param) - 18;
		if((pdurl_len>0) && (pdurl_len<MAX_URL_LEN))
		{
			pdpath_len = strlen((const char *)DEFAULT_PKG_PATH);
			strncpy(pdpath, (const char *)DEFAULT_PKG_PATH, pdpath_len);
			strncpy(pdurl, param + 18, pdurl_len);
			conv_num_durl = mbstowcs((wchar_t *)pkg_durl,(const char *)pdurl, pdurl_len + 1);  //size_t stdc_FCAC2E8E(wchar_t *dest, const char *src, size_t max)
		}
		else
			goto end_download_process;
	}
	else
		goto end_download_process;

	if(conv_num_durl > 0)
	{
		if((pdpath_len > 0) && (pdpath_len < MAX_DLPATH_LEN) && (isDir((const char *)pdpath) || cellFsMkdir(pdpath, DMODE) == CELL_FS_SUCCEEDED))
		{
			conv_num_dpath = mbstowcs((wchar_t *)pkg_dpath, (const char *)pdpath, pdpath_len + 1);
			sprintf(msg_dpath, (const char *)"To: %s", (const char *)pdpath);
		}
		else if(isDir((const char *)DEFAULT_PKG_PATH) || cellFsMkdir(pdpath, DMODE) == CELL_FS_SUCCEEDED)
		{
			conv_num_dpath = mbstowcs((wchar_t *)pkg_dpath, (const char *)DEFAULT_PKG_PATH, strlen((const char *)DEFAULT_PKG_PATH) + 1);
			sprintf(msg_dpath, (const char *)"To: %s", (const char *)DEFAULT_PKG_PATH);
		}
		else
		{
			conv_num_dpath = mbstowcs((wchar_t *)pkg_dpath, (const char *)INT_HDD_ROOT_PATH, strlen((const char *)INT_HDD_ROOT_PATH) + 1);
			sprintf(msg_dpath, (const char *)"To: %s", (const char *)INT_HDD_ROOT_PATH);
		}

		if(conv_num_dpath > 0)
		{
			if (View_Find("webrender_plugin"))
			{
				ret = UnloadPluginById(0x1C,(void *)unloadSysPluginCallback);
				sys_timer_usleep(5);
			}
			if (View_Find("webbrowser_plugin"))
			{
				ret = UnloadPluginById(0x1B,(void *)unloadSysPluginCallback);
				sys_timer_usleep(5);
			}

			sprintf(msg_durl, (const char *)"Downloading: %s", (const char *)pdurl);

			ret = LoadPluginById(0x29, (void *)downloadPKG_thread);
		}
		else
			sprintf(msg_durl, (const char *)"ERROR: Setting storage location");
	}

end_download_process:
	sprintf(msg, "%s\n%s", msg_durl, msg_dpath);
	return ret;
}

static int installPKG(const char *pkgpath, char *msg)
{
	int ret = FAILED;
	if(View_Find("game_plugin"))
	{
		sprintf(msg, (const char *)"ERROR: install from XMB");
		return ret;
	}

	sprintf(msg, (const char *)"ERROR: invalid pkg path");

	size_t pkg_path_len = strlen(pkgpath);

	if (pkg_path_len < MAX_PKGPATH_LEN)
	{
		snprintf(pkg_path, MAX_PKGPATH_LEN, "%s", pkgpath);

		if( file_exists(pkg_path) )
		{
			if(!extcasecmp(pkg_path, ".pkg", 4)) //check if file has a .pkg extension or not and treat accordingly
			{
				if (View_Find("webrender_plugin"))
				{
					ret = UnloadPluginById(0x1C, (void *)unloadSysPluginCallback);
					sys_timer_usleep(5);
				}
				if (View_Find("webbrowser_plugin"))
				{
					ret = UnloadPluginById(0x1B, (void *)unloadSysPluginCallback);
					sys_timer_usleep(5);
				}
				ret = LoadPluginById(0x16, (void *)installPKG_thread);
				sprintf(msg,(const char *)"Installing %s", pkg_path);
				ret = 0;
			}
		}
	}

	return ret;
}

static int installPKG_combo(char *msg)
{
	int fd, ret = FAILED;

	if(cellFsOpendir(DEFAULT_PKG_PATH, &fd) == CELL_FS_SUCCEEDED)
	{
		char pkgfile[MAX_PKGPATH_LEN] = "";

		CellFsDirent dir; u64 read = sizeof(CellFsDirent);

		if(file_exists(pkg_path)) {sprintf(pkgfile, "%s.bak", pkg_path); cellFsRename(pkg_path, pkgfile); pkg_path[0] = NULL;}

		while(!cellFsReaddir(fd, &dir, &read))
		{
			if(!read) break;
			if(!extcasecmp(dir.d_name, ".pkg", 4))
			{
				sprintf(pkgfile, "%s%s", DEFAULT_PKG_PATH, dir.d_name); ret = 0; { BEEP1 }

				installPKG(pkgfile, msg); show_msg(msg);
				break;
			}
		}
		cellFsClosedir(fd);

		if(ret == FAILED) { BEEP2 } else sys_timer_sleep(2);
	}

	return ret;
}

#endif // #ifdef PKG_HANDLER
