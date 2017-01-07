#include <fcntl.h>
#include <ppu-lv2.h>
#include <sys/file.h>
#include <stdio.h>
#include <string.h>

#include <io/pad.h>

#define SUCCESS 0
#define FAILED -1

#define SC_SYS_POWER 					(379)
#define SYS_REBOOT				 		0x8201

#define BUTTON_SQUARE        128
#define BUTTON_CROSS         64
#define BUTTON_CIRCLE        32
#define BUTTON_TRIANGLE      16
#define BUTTON_R1            8
#define BUTTON_L1            4
#define BUTTON_R2            2
#define BUTTON_L2            1

#define APP_DIR              "/dev_hdd0/game/UPDWEBMOD"
#define APP_USRDIR           APP_DIR "/USRDIR"


#define HDDROOT_DIR          "/dev_hdd0"
#define PLUGINS_DIR          "/dev_hdd0/plugins"

#define HDD0ROOT_DIR         "/dev_hdd0/"

#define TMP_DIR              "/dev_hdd0/tmp"
#define LANG_DIR             TMP_DIR "/wm_lang"
#define COMBO_DIR            TMP_DIR "/wm_combo"
#define ICONS_DIR            TMP_DIR "/wm_icons"

#define XMLHOST_DIR          "/dev_hdd0/xmlhost/game_plugin"

#define XMLMANPLS_DIR        "/dev_hdd0/game/XMBMANPLS"
#define XMLMANPLS_FEATS_DIR  XMLMANPLS_DIR "/USRDIR/FEATURES"
#define XMLMANPLS_IMAGES_DIR XMLMANPLS_DIR "/USRDIR/IMAGES"

#define IRISMAN_USRDIR       "/dev_hdd0/game/IRISMAN01/USRDIR"
#define PRXLOADER_USRDIR     "/dev_hdd0/game/PRXLOADER/USRDIR"

#define REBUG_DIR            "/dev_flash/rebug"
#define FLASH_VSH_MODULE_DIR "/dev_flash/vsh/module"
#define REBUG_VSH_MODULE_DIR "/dev_blind/vsh/module"

#define XMB_CATEGORY_GAME_XML "/vsh/resource/explore/xmb/category_game.xml"


bool full = false;
bool lite = false;
bool vsh_menu = false;

int sys_fs_mount(char const* deviceName, char const* deviceFileSystem, char const* devicePath, int writeProt)
{
	lv2syscall8(837, (u64) deviceName, (u64) deviceFileSystem, (u64) devicePath, 0, (u64) writeProt, 0, 0, 0 );
	return_to_user_prog(int);
}

int CopyFile(char* path, char* path2)
{
	int ret = 0;
	s32 fd = -1;
	s32 fd2 = -1;
	u64 lenght = 0LL;

	u64 pos = 0ULL;
	u64 readed = 0, writed = 0;

	char *mem = NULL;

	sysFSStat stat;

	ret= sysLv2FsStat(path, &stat);
	lenght = stat.st_size;

	if(ret) goto skip;

	if(strstr(path, HDD0ROOT_DIR) != NULL && strstr(path2, HDD0ROOT_DIR) != NULL)
	{
		if(strcmp(path, path2)==0) return ret;

		sysLv2FsUnlink(path2);
		sysLv2FsLink(path, path2);

		if (sysLv2FsStat(path2, &stat) == 0) return 0;
	}

	ret = sysLv2FsOpen(path, 0, &fd, S_IRWXU | S_IRWXG | S_IRWXO, NULL, 0);
	if(ret) goto skip;

	ret = sysLv2FsOpen(path2, SYS_O_WRONLY | SYS_O_CREAT | SYS_O_TRUNC, &fd2, 0777, NULL, 0);
	if(ret) {sysLv2FsClose(fd);goto skip;}

	mem = malloc(0x100000);
	if (mem == NULL) return FAILED;

	while(pos < lenght)
	{
		readed = lenght - pos; if(readed > 0x100000ULL) readed = 0x100000ULL;
		ret=sysLv2FsRead(fd, mem, readed, &writed);
		if(ret<0) goto skip;
		if(readed != writed) {ret = 0x8001000C; goto skip;}

		ret=sysLv2FsWrite(fd2, mem, readed, &writed);
		if(ret<0) goto skip;
		if(readed != writed) {ret = 0x8001000C; goto skip;}

		pos += readed;
	}

skip:

	if(mem) free(mem);
	if(fd >=0) sysLv2FsClose(fd);
	if(fd2>=0) sysLv2FsClose(fd2);
	if(ret) return ret;

	ret = sysLv2FsStat(path2, &stat);
	if((ret == SUCCESS) && (stat.st_size == lenght)) ret = SUCCESS; else ret = FAILED;

	return ret;
}

int sys_get_version(u32 *version)
{
	lv2syscall2(8, 0x7000, (u64)version);
	return_to_user_prog(int);
}

int sys_get_mamba(void)
{
	lv2syscall1(8, 0x7FFF);
	return_to_user_prog(int);
}

bool is_mamba(void)
{
	sysFSStat stat;
	if(sysLv2FsStat(HDDROOT_DIR "/mamba_plugins.txt", &stat) == SUCCESS) return true;

	u32 version = 0x99999999;
	if (sys_get_version(&version) < 0) return false;
	if (sys_get_mamba() == 0x666) return true;
	return false;
}

bool is_disabled(char *filename, char *filename2)
{
	sysFSStat stat;

	if(sysLv2FsStat("/dev_blind", &stat) != SUCCESS)
		sys_fs_mount("CELL_FS_IOS:BUILTIN_FLSH1", "CELL_FS_FAT", "/dev_blind", 0);

	if(sysLv2FsStat(filename, &stat) == SUCCESS)
	{
		sysLv2FsRename(filename, filename2); // re-enable stage2.bin

		return true;
	}

	return false;
}

bool is_cobra(void)
{
	sysFSStat stat; bool ret = false;

	if(is_disabled("/dev_blind/habib/cobra/stage2_disabled.cex", "/dev_blind/habib/cobra/stage2.cex")) return true;
	if(is_disabled("/dev_blind/sys/stage2_disabled.bin", "/dev_blind/sys/stage2.bin")) return true;
	if(is_disabled("/dev_blind/sys/stage2.bin.bak", "/dev_blind/sys/stage2.bin")) return true;

	if(is_disabled("/dev_blind/rebug/cobra/stage2.cex.bak", "/dev_blind/rebug/cobra/stage2.cex")) ret = true;
	if(is_disabled("/dev_blind/rebug/cobra/stage2.dex.bak", "/dev_blind/rebug/cobra/stage2.dex")) ret = true;

	if(sysLv2FsStat("/dev_flash/sys/stage2.bin", &stat) == SUCCESS) return true;
	if(sysLv2FsStat(HDDROOT_DIR "/boot_plugins.txt", &stat) == SUCCESS) return true;
	if(sysLv2FsStat("/dev_flash/rebug/cobra", &stat) == SUCCESS) return true;

	if (is_mamba()) return false;

	u32 version = 0x99999999;
	if (sys_get_version(&version) < 0) return false;
	if ((version & 0xFF00FF) == 0x04000F || (version & 0xFFFFFF) == 0x03550F)  return true;

	return ret;
}

#define SYSCALL8_OPCODE_PS3MAPI			 		0x7777
#define PS3MAPI_OPCODE_GET_CORE_MINVERSION		0x0012

int sys_get_minversion_ps3mapi(void)
{
	lv2syscall2(8, SYSCALL8_OPCODE_PS3MAPI, PS3MAPI_OPCODE_GET_CORE_MINVERSION);
	return_to_user_prog(int);
}


bool is_ps3mapi(void)
{
	if (0x0111 <= sys_get_minversion_ps3mapi()) return true;
	else return false;
}

int add_mygame()
{
// -2 failed and cannot rename the backup
// -1 failed
//  0 already
//  1 done

	char fb[] = {0x0d, 0x0a, 0x09, 0x09, 0x09, 0x3c, 0x51, 0x75, 0x65, 0x72, 0x79, 0x20, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x3d, 0x22, 0x74, 0x79, 0x70, 0x65, 0x3a, 0x78, 0x2d, 0x78, 0x6d, 0x62, 0x2f, 0x66, 0x6f, 0x6c, 0x64, 0x65, 0x72, 0x2d, 0x70, 0x69, 0x78, 0x6d, 0x61, 0x70, 0x22, 0x0d, 0x0a, 0x09, 0x09, 0x09, 0x09, 0x6b, 0x65, 0x79, 0x3d, 0x22, 0x78, 0x6d, 0x62, 0x5f, 0x61, 0x70, 0x70, 0x33, 0x22, 0x20, 0x0d, 0x0a, 0x09, 0x09, 0x09, 0x09, 0x61, 0x74, 0x74, 0x72, 0x3d, 0x22, 0x78, 0x6d, 0x62, 0x5f, 0x61, 0x70, 0x70, 0x33, 0x22, 0x20, 0x0d, 0x0a, 0x09, 0x09, 0x09, 0x09, 0x73, 0x72, 0x63, 0x3d, 0x22, 0x78, 0x6d, 0x62, 0x3a, 0x2f, 0x2f, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x68, 0x6f, 0x73, 0x74, 0x2f, 0x64, 0x65, 0x76, 0x5f, 0x68, 0x64, 0x64, 0x30, 0x2f, 0x78, 0x6d, 0x6c, 0x68, 0x6f, 0x73, 0x74, 0x2f, 0x67, 0x61, 0x6d, 0x65, 0x5f, 0x70, 0x6c, 0x75, 0x67, 0x69, 0x6e, 0x2f, 0x66, 0x62, 0x2e, 0x78, 0x6d, 0x6c, 0x23, 0x73, 0x65, 0x67, 0x5f, 0x66, 0x62, 0x22, 0x0d, 0x0a, 0x09, 0x09, 0x09, 0x09, 0x2f, 0x3e};
	FILE* f;
	long size, gexit_size, len;
	char *cat;
	char cat_path[255];
	size_t result;
	int i, j, pos=0;
	sysFSStat stat;

	//read original cat
	f = fopen("/dev_flash" XMB_CATEGORY_GAME_XML, "r");
	if(f==NULL) return FAILED;
	fseek (f , 0 , SEEK_END);
	size = ftell (f);
	fseek(f, 0, SEEK_SET);

	cat = (char*) malloc (sizeof(char)*size);
	if (cat == NULL) {free(cat); return FAILED;}

	result = fread(cat,1,size, f);
	if (result != size) {free (cat); fclose (f); return FAILED;}
	fclose (f);

	// is fb.xml entry in cat file ?
	if(strstr(cat, "fb.xml")!=NULL) {free(cat); return 0;}

	// search position of game exit
	char gameexit1[] = {0x0d, 0x0a, 0x09, 0x09, 0x09, 0x3c, 0x51, 0x75, 0x65, 0x72, 0x79, 0x0d, 0x0a, 0x09, 0x09, 0x09, 0x09, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x3d, 0x22, 0x74, 0x79, 0x70, 0x65, 0x3a, 0x78, 0x2d, 0x78, 0x6d, 0x62, 0x2f, 0x66, 0x6f, 0x6c, 0x64, 0x65, 0x72, 0x2d, 0x70, 0x69, 0x78, 0x6d, 0x61, 0x70, 0x22, 0x0d, 0x0a, 0x09, 0x09, 0x09, 0x09, 0x6b, 0x65, 0x79, 0x3d, 0x22, 0x73, 0x65, 0x67, 0x5f, 0x67, 0x61, 0x6d, 0x65, 0x65, 0x78, 0x69, 0x74, 0x22, 0x0d, 0x0a, 0x09, 0x09, 0x09, 0x09, 0x73, 0x72, 0x63, 0x3d, 0x22, 0x73, 0x65, 0x6c, 0x3a, 0x2f, 0x2f, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x68, 0x6f, 0x73, 0x74, 0x2f, 0x69, 0x6e, 0x67, 0x61, 0x6d, 0x65, 0x3f, 0x70, 0x61, 0x74, 0x68, 0x3d, 0x63, 0x61, 0x74, 0x65, 0x67, 0x6f, 0x72, 0x79, 0x5f, 0x67, 0x61, 0x6d, 0x65, 0x2e, 0x78, 0x6d, 0x6c, 0x23, 0x73, 0x65, 0x67, 0x5f, 0x67, 0x61, 0x6d, 0x65, 0x65, 0x78, 0x69, 0x74, 0x26, 0x74, 0x79, 0x70, 0x65, 0x3d, 0x67, 0x61, 0x6d, 0x65, 0x22, 0x0d, 0x0a, 0x09, 0x09, 0x09, 0x09, 0x2f, 0x3e};
	gexit_size=sizeof(gameexit1); len = gexit_size - 1;

	for(i = 0; i < size - gexit_size; i++)
	{
		for(j = 0; j < gexit_size; j++)
		{
			if(cat[i+j] != gameexit1[j]) break;
			if(j==len) {pos = i; goto patch_xml;}
		}
	}

	// search position of game exit (Unix style)
	char gameexit2[] = {0x0a, 0x09, 0x09, 0x09, 0x3c, 0x51, 0x75, 0x65, 0x72, 0x79, 0x0a, 0x09, 0x09, 0x09, 0x09, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x3d, 0x22, 0x74, 0x79, 0x70, 0x65, 0x3a, 0x78, 0x2d, 0x78, 0x6d, 0x62, 0x2f, 0x66, 0x6f, 0x6c, 0x64, 0x65, 0x72, 0x2d, 0x70, 0x69, 0x78, 0x6d, 0x61, 0x70, 0x22, 0x0a, 0x09, 0x09, 0x09, 0x09, 0x6b, 0x65, 0x79, 0x3d, 0x22, 0x73, 0x65, 0x67, 0x5f, 0x67, 0x61, 0x6d, 0x65, 0x65, 0x78, 0x69, 0x74, 0x22, 0x0a, 0x09, 0x09, 0x09, 0x09, 0x73, 0x72, 0x63, 0x3d, 0x22, 0x73, 0x65, 0x6c, 0x3a, 0x2f, 0x2f, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x68, 0x6f, 0x73, 0x74, 0x2f, 0x69, 0x6e, 0x67, 0x61, 0x6d, 0x65, 0x3f, 0x70, 0x61, 0x74, 0x68, 0x3d, 0x63, 0x61, 0x74, 0x65, 0x67, 0x6f, 0x72, 0x79, 0x5f, 0x67, 0x61, 0x6d, 0x65, 0x2e, 0x78, 0x6d, 0x6c, 0x23, 0x73, 0x65, 0x67, 0x5f, 0x67, 0x61, 0x6d, 0x65, 0x65, 0x78, 0x69, 0x74, 0x26, 0x74, 0x79, 0x70, 0x65, 0x3d, 0x67, 0x61, 0x6d, 0x65, 0x22, 0x0a, 0x09, 0x09, 0x09, 0x09, 0x2f, 0x3e};
	gexit_size=sizeof(gameexit2); len = gexit_size - 1;

	for(i = 0; i < size - gexit_size; i++)
	{
		for(j = 0; j < gexit_size; j++)
		{
			if(cat[i+j] != gameexit2[j]) break;
			if(j==len) {pos = i; goto patch_xml;}
		}
	}

	free(cat);
	return FAILED;

patch_xml:

	//write patched cat
	f = fopen(APP_USRDIR "/category_game.xml", "w");
	if(f==NULL) {free(cat); return FAILED;}
	fwrite(cat, 1, pos, f);
	fwrite(fb, 1, sizeof(fb), f);
	fwrite(&cat[pos], 1, size-pos, f);
	fclose(f);


	// set target path for category_game.xml
	strcpy(cat_path, "/dev_blind" XMB_CATEGORY_GAME_XML);
	if(sysLv2FsStat(cat_path, &stat) != SUCCESS) {
		strcpy(cat_path, "/dev_habib" XMB_CATEGORY_GAME_XML);
		if(sysLv2FsStat(cat_path, &stat) != SUCCESS) {
			strcpy(cat_path, "/dev_rewrite" XMB_CATEGORY_GAME_XML);

			// mount /dev_blind if category_game.xml is not found
			if(sysLv2FsStat(cat_path, &stat) != SUCCESS) {
				if(sys_fs_mount("CELL_FS_IOS:BUILTIN_FLSH1", "CELL_FS_FAT", "/dev_blind", 0) == SUCCESS) {
					strcpy(cat_path, "/dev_blind" XMB_CATEGORY_GAME_XML);
					if(sysLv2FsStat(cat_path, &stat) != SUCCESS) { free(cat); return FAILED;}
				} else { free(cat); return FAILED;}
			}
		}
	}

	// rename category_game.xml as category_game.xml.bak
	char cat_path_bak[255];
	strcpy(cat_path_bak, cat_path);
	strcat(cat_path_bak, ".bak");
	sysLv2FsUnlink(cat_path_bak);
	if(sysLv2FsRename(cat_path, cat_path_bak) != SUCCESS) {free(cat); return FAILED;} ;

	// update category_game.xml
	if(CopyFile(APP_USRDIR "/category_game.xml", cat_path) != SUCCESS)
	{
		sysLv2FsUnlink(cat_path);
		if(sysLv2FsRename(cat_path_bak, cat_path)) { //restore category_game.xml from category_game.xml.bak
			{lv2syscall3(392, 0x1004, 0xa, 0x1b6);} ///TRIPLE BIP
			free(cat);
			return -2;
		}
	}
	free(cat);
	return 1;
}

int main()
{
	FILE* f=NULL;
	sysFSStat stat;

//--- hold CROSS

	unsigned button = 0;

	padInfo padinfo;
	padData paddata;

	ioPadInit(7);

	int n, r;
	for(r=0; r<10; r++)
	{
		ioPadGetInfo(&padinfo);
		for(n = 0; n < 7; n++)
		{
			if(padinfo.status[n])
			{
				ioPadGetData(n, &paddata);
				button = (paddata.button[2] << 8) | (paddata.button[3] & 0xff);
				break;
			}
		}
		if(button) break; else usleep(20000);
	}
	ioPadEnd();

	if(button & 0x04) full=true; else
	if(button & 0x60) lite=true;  // circle / cross
	if(button & 0x0F) vsh_menu=true; else vsh_menu = (sysLv2FsStat(PLUGINS_DIR "/wm_vsh_menu.sprx", &stat) == SUCCESS);  // r1/r2/l1/l2
//---

	sysLv2FsMkdir(TMP_DIR,   0777);
	sysLv2FsMkdir(LANG_DIR,  0777);
	sysLv2FsMkdir(COMBO_DIR, 0777);

	// remove language files (old location)
	sysLv2FsUnlink(TMP_DIR "/LANG_EN.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_AR.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_CN.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_DE.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_ES.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_FR.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_GR.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_DK.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_HU.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_HR.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_BG.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_CZ.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_SK.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_IN.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_JP.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_KR.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_IT.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_NL.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_PL.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_PT.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_RU.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_TR.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_ZH.TXT");
	sysLv2FsUnlink(TMP_DIR "/LANG_XX.TXT");

	// remove language files
	sysLv2FsUnlink(LANG_DIR "/LANG_EN.TXT");
	sysLv2FsUnlink(LANG_DIR "/LANG_AR.TXT");
	sysLv2FsUnlink(LANG_DIR "/LANG_CN.TXT");
	sysLv2FsUnlink(LANG_DIR "/LANG_DE.TXT");
	sysLv2FsUnlink(LANG_DIR "/LANG_ES.TXT");
	sysLv2FsUnlink(LANG_DIR "/LANG_FR.TXT");
	sysLv2FsUnlink(LANG_DIR "/LANG_GR.TXT");
	sysLv2FsUnlink(LANG_DIR "/LANG_DK.TXT");
	sysLv2FsUnlink(LANG_DIR "/LANG_HU.TXT");
	sysLv2FsUnlink(LANG_DIR "/LANG_HR.TXT");
	sysLv2FsUnlink(LANG_DIR "/LANG_BG.TXT");
	sysLv2FsUnlink(LANG_DIR "/LANG_CZ.TXT");
	sysLv2FsUnlink(LANG_DIR "/LANG_SK.TXT");
	sysLv2FsUnlink(LANG_DIR "/LANG_IN.TXT");
	sysLv2FsUnlink(LANG_DIR "/LANG_JP.TXT");
	sysLv2FsUnlink(LANG_DIR "/LANG_KR.TXT");
	sysLv2FsUnlink(LANG_DIR "/LANG_IT.TXT");
	sysLv2FsUnlink(LANG_DIR "/LANG_NL.TXT");
	sysLv2FsUnlink(LANG_DIR "/LANG_PL.TXT");
	sysLv2FsUnlink(LANG_DIR "/LANG_PT.TXT");
	sysLv2FsUnlink(LANG_DIR "/LANG_RU.TXT");
	sysLv2FsUnlink(LANG_DIR "/LANG_TR.TXT");
	sysLv2FsUnlink(LANG_DIR "/LANG_ZH.TXT");

	// remove old files
	sysLv2FsUnlink(APP_USRDIR "/webftp_server_rebug_cobra_multi19.sprx");
	sysLv2FsUnlink(APP_USRDIR "/webftp_server_rebug_cobra_multi20.sprx");
	sysLv2FsUnlink(APP_USRDIR "/webftp_server_rebug_cobra_multi21.sprx");
	sysLv2FsUnlink(APP_USRDIR "/webftp_server_webchat.sprx");

	sysLv2FsUnlink(XMLHOST_DIR "/jquery-1.11.3.min.js");
	sysLv2FsUnlink(APP_USRDIR  "/jquery-1.11.3.min.js");
	sysLv2FsUnlink(XMLHOST_DIR "/jquery-1.12.3.min.js");
	sysLv2FsUnlink(APP_USRDIR  "/jquery-1.12.3.min.js");

	// update languages
	CopyFile(APP_USRDIR "/LANG_EN.TXT", LANG_DIR "/LANG_EN.TXT");
	CopyFile(APP_USRDIR "/LANG_AR.TXT", LANG_DIR "/LANG_AR.TXT");
	CopyFile(APP_USRDIR "/LANG_CN.TXT", LANG_DIR "/LANG_CN.TXT");
	CopyFile(APP_USRDIR "/LANG_DE.TXT", LANG_DIR "/LANG_DE.TXT");
	CopyFile(APP_USRDIR "/LANG_ES.TXT", LANG_DIR "/LANG_ES.TXT");
	CopyFile(APP_USRDIR "/LANG_FR.TXT", LANG_DIR "/LANG_FR.TXT");
	CopyFile(APP_USRDIR "/LANG_GR.TXT", LANG_DIR "/LANG_GR.TXT");
	CopyFile(APP_USRDIR "/LANG_DK.TXT", LANG_DIR "/LANG_DK.TXT");
	CopyFile(APP_USRDIR "/LANG_HU.TXT", LANG_DIR "/LANG_HU.TXT");
	CopyFile(APP_USRDIR "/LANG_HR.TXT", LANG_DIR "/LANG_HR.TXT");
	CopyFile(APP_USRDIR "/LANG_BG.TXT", LANG_DIR "/LANG_BG.TXT");
	CopyFile(APP_USRDIR "/LANG_CZ.TXT", LANG_DIR "/LANG_CZ.TXT");
	CopyFile(APP_USRDIR "/LANG_SK.TXT", LANG_DIR "/LANG_SK.TXT");
	CopyFile(APP_USRDIR "/LANG_IN.TXT", LANG_DIR "/LANG_IN.TXT");
	CopyFile(APP_USRDIR "/LANG_IT.TXT", LANG_DIR "/LANG_IT.TXT");
	CopyFile(APP_USRDIR "/LANG_JP.TXT", LANG_DIR "/LANG_JP.TXT");
	CopyFile(APP_USRDIR "/LANG_KR.TXT", LANG_DIR "/LANG_KR.TXT");
	CopyFile(APP_USRDIR "/LANG_NL.TXT", LANG_DIR "/LANG_NL.TXT");
	CopyFile(APP_USRDIR "/LANG_PL.TXT", LANG_DIR "/LANG_PL.TXT");
	CopyFile(APP_USRDIR "/LANG_PT.TXT", LANG_DIR "/LANG_PT.TXT");
	CopyFile(APP_USRDIR "/LANG_RU.TXT", LANG_DIR "/LANG_RU.TXT");
	CopyFile(APP_USRDIR "/LANG_TR.TXT", LANG_DIR "/LANG_TR.TXT");
	CopyFile(APP_USRDIR "/LANG_ZH.TXT", LANG_DIR "/LANG_ZH.TXT");

	sysLv2FsMkdir(XMLHOST_DIR, 0777);
	CopyFile(APP_USRDIR "/mobile.html",    XMLHOST_DIR "/mobile.html");
	CopyFile(APP_USRDIR "/background.gif", XMLHOST_DIR "/background.gif");

	// copy javascripts
	CopyFile(APP_USRDIR "/jquery.min.js",    XMLHOST_DIR "/jquery.min.js");  // jQuery v3.1.1
	CopyFile(APP_USRDIR "/jquery-ui.min.js", XMLHOST_DIR "/jquery-ui.min.js"); // jQuery UI v1.12.1

	CopyFile(APP_USRDIR "/fm.js",     XMLHOST_DIR "/fm.js");
	CopyFile(APP_USRDIR "/games.js",  XMLHOST_DIR "/games.js");
	CopyFile(APP_USRDIR "/common.js", XMLHOST_DIR "/common.js");

	// copy css
	CopyFile(APP_USRDIR "/common.css",  XMLHOST_DIR "/common.css");

	sysLv2FsMkdir(TMP_DIR "/wm_icons", 0777);

	// copy new icons
	CopyFile(APP_USRDIR "/icon_wm_album_ps3.png", ICONS_DIR "/icon_wm_album_ps3.png");
	CopyFile(APP_USRDIR "/icon_wm_album_psx.png", ICONS_DIR "/icon_wm_album_psx.png");
	CopyFile(APP_USRDIR "/icon_wm_album_ps2.png", ICONS_DIR "/icon_wm_album_ps2.png");
	CopyFile(APP_USRDIR "/icon_wm_album_psp.png", ICONS_DIR "/icon_wm_album_psp.png");
	CopyFile(APP_USRDIR "/icon_wm_album_dvd.png", ICONS_DIR "/icon_wm_album_dvd.png");
	CopyFile(APP_USRDIR "/icon_wm_album_emu.png", ICONS_DIR "/icon_wm_album_emu.png");

	CopyFile(APP_USRDIR "/icon_wm_ps3.png"      , ICONS_DIR "/icon_wm_ps3.png");
	CopyFile(APP_USRDIR "/icon_wm_psx.png"      , ICONS_DIR "/icon_wm_psx.png");
	CopyFile(APP_USRDIR "/icon_wm_ps2.png"      , ICONS_DIR "/icon_wm_ps2.png");
	CopyFile(APP_USRDIR "/icon_wm_psp.png"      , ICONS_DIR "/icon_wm_psp.png");
	CopyFile(APP_USRDIR "/icon_wm_dvd.png"      , ICONS_DIR "/icon_wm_dvd.png");

	CopyFile(APP_USRDIR "/icon_wm_settings.png" , ICONS_DIR "/icon_wm_settings.png");
	CopyFile(APP_USRDIR "/icon_wm_eject.png"    , ICONS_DIR "/icon_wm_eject.png"   );

	if((sysLv2FsStat(APP_USRDIR "/multiman.png", &stat) == SUCCESS) && (stat.st_size == 9894))
		CopyFile(APP_USRDIR "/icon_wm_root.png" , ICONS_DIR "/icon_wm_root.png"    );
	else
		sysLv2FsUnlink(ICONS_DIR "/icon_wm_root.png");

	CopyFile(APP_USRDIR "/blank.png"            , ICONS_DIR "/blank.png"    );

	CopyFile(APP_USRDIR "/wm_online_ids.txt"	, TMP_DIR "/wm_online_ids.txt");

	// webMAN LaunchPad icons
	CopyFile(APP_USRDIR "/icon_lp_ps3.png"      , ICONS_DIR "/icon_lp_ps3.png");
	CopyFile(APP_USRDIR "/icon_lp_psx.png"      , ICONS_DIR "/icon_lp_psx.png");
	CopyFile(APP_USRDIR "/icon_lp_ps2.png"      , ICONS_DIR "/icon_lp_ps2.png");
	CopyFile(APP_USRDIR "/icon_lp_psp.png"      , ICONS_DIR "/icon_lp_psp.png");
	CopyFile(APP_USRDIR "/icon_lp_dvd.png"      , ICONS_DIR "/icon_lp_dvd.png");
	CopyFile(APP_USRDIR "/icon_lp_blu.png"      , ICONS_DIR "/icon_lp_blu.png");
	CopyFile(APP_USRDIR "/icon_lp_nocover.png"  , ICONS_DIR "/icon_lp_nocover.png");

	CopyFile(APP_USRDIR "/eject.png"  			, XMLMANPLS_IMAGES_DIR "/eject.png");
	CopyFile(APP_USRDIR "/setup.png"  			, XMLMANPLS_IMAGES_DIR "/setup.png");
	CopyFile(APP_USRDIR "/refresh.png"			, XMLMANPLS_IMAGES_DIR "/refresh.png");
	//CopyFile(APP_USRDIR "/clear.png"  		, XMLMANPLS_IMAGES_DIR "/clear.png");
	//CopyFile(APP_USRDIR "/cache.png"  		, XMLMANPLS_IMAGES_DIR "/cache.png");
	//CopyFile(APP_USRDIR "/restart.png"		, XMLMANPLS_IMAGES_DIR "/restart.png");

	// XMBM+ webMAN
	sysLv2FsMkdir(XMLMANPLS_DIR, 0777);
	sysLv2FsMkdir(XMLMANPLS_DIR "/USRDIR", 0777);
	sysLv2FsMkdir(XMLMANPLS_IMAGES_DIR, 0777);
	sysLv2FsMkdir(XMLMANPLS_FEATS_DIR, 0777);

	if(sysLv2FsStat(XMLMANPLS_DIR "/PARAM.SFO", &stat) != SUCCESS)
		CopyFile(APP_USRDIR "/PARAM.SFO", XMLMANPLS_DIR "/PARAM.SFO");

	if(sysLv2FsStat(XMLMANPLS_DIR "/ICON0.PNG", &stat) != SUCCESS)
		CopyFile(APP_DIR "/ICON0.PNG", XMLMANPLS_DIR "/ICON0.PNG");

	CopyFile(APP_USRDIR "/webMAN.xml"    , XMLMANPLS_FEATS_DIR "/webMAN.xml");
	CopyFile(APP_USRDIR "/webMAN_AR.xml" , XMLMANPLS_FEATS_DIR "/webMAN_AR.xml");
	CopyFile(APP_USRDIR "/webMAN_CN.xml" , XMLMANPLS_FEATS_DIR "/webMAN_CN.xml");
	CopyFile(APP_USRDIR "/webMAN_DE.xml" , XMLMANPLS_FEATS_DIR "/webMAN_DE.xml");
	CopyFile(APP_USRDIR "/webMAN_ES.xml" , XMLMANPLS_FEATS_DIR "/webMAN_ES.xml");
	CopyFile(APP_USRDIR "/webMAN_FR.xml" , XMLMANPLS_FEATS_DIR "/webMAN_FR.xml");
	CopyFile(APP_USRDIR "/webMAN_GR.xml" , XMLMANPLS_FEATS_DIR "/webMAN_GR.xml");
	CopyFile(APP_USRDIR "/webMAN_DK.xml" , XMLMANPLS_FEATS_DIR "/webMAN_DK.xml");
	CopyFile(APP_USRDIR "/webMAN_HU.xml" , XMLMANPLS_FEATS_DIR "/webMAN_HU.xml");
	CopyFile(APP_USRDIR "/webMAN_HR.xml" , XMLMANPLS_FEATS_DIR "/webMAN_HR.xml");
	CopyFile(APP_USRDIR "/webMAN_BG.xml" , XMLMANPLS_FEATS_DIR "/webMAN_BG.xml");
	CopyFile(APP_USRDIR "/webMAN_CZ.xml" , XMLMANPLS_FEATS_DIR "/webMAN_CZ.xml");
	CopyFile(APP_USRDIR "/webMAN_SK.xml" , XMLMANPLS_FEATS_DIR "/webMAN_SK.xml");
	CopyFile(APP_USRDIR "/webMAN_IN.xml" , XMLMANPLS_FEATS_DIR "/webMAN_IN.xml");
	CopyFile(APP_USRDIR "/webMAN_JP.xml" , XMLMANPLS_FEATS_DIR "/webMAN_JP.xml");
	CopyFile(APP_USRDIR "/webMAN_KR.xml" , XMLMANPLS_FEATS_DIR "/webMAN_KR.xml");
	CopyFile(APP_USRDIR "/webMAN_IT.xml" , XMLMANPLS_FEATS_DIR "/webMAN_IT.xml");
	CopyFile(APP_USRDIR "/webMAN_NL.xml" , XMLMANPLS_FEATS_DIR "/webMAN_NL.xml");
	CopyFile(APP_USRDIR "/webMAN_PL.xml" , XMLMANPLS_FEATS_DIR "/webMAN_PL.xml");
	CopyFile(APP_USRDIR "/webMAN_PT.xml" , XMLMANPLS_FEATS_DIR "/webMAN_PT.xml");
	CopyFile(APP_USRDIR "/webMAN_RU.xml" , XMLMANPLS_FEATS_DIR "/webMAN_RU.xml");
	CopyFile(APP_USRDIR "/webMAN_TR.xml" , XMLMANPLS_FEATS_DIR "/webMAN_TR.xml");
	CopyFile(APP_USRDIR "/webMAN_ZH.xml" , XMLMANPLS_FEATS_DIR "/webMAN_ZH.xml");

	CopyFile(APP_USRDIR "/bd.png"        	, XMLMANPLS_IMAGES_DIR "/bd.png");
	CopyFile(APP_USRDIR "/blockpsn.png"  	, XMLMANPLS_IMAGES_DIR "/blockpsn.png");
	CopyFile(APP_USRDIR "/cachefiles.png"	, XMLMANPLS_IMAGES_DIR "/cachefiles.png");
	CopyFile(APP_USRDIR "/filemanager.png"	, XMLMANPLS_IMAGES_DIR "/filemanager.png");
	CopyFile(APP_USRDIR "/gamedata.png"  	, XMLMANPLS_IMAGES_DIR "/gamedata.png");
	CopyFile(APP_USRDIR "/gamefix.png"   	, XMLMANPLS_IMAGES_DIR "/gamefix.png");
	CopyFile(APP_USRDIR "/gamesbrowser.png"	, XMLMANPLS_IMAGES_DIR "/gamesbrowser.png");
	CopyFile(APP_USRDIR "/multiman.png"  	, XMLMANPLS_IMAGES_DIR "/multiman.png");
	CopyFile(APP_USRDIR "/network.png"   	, XMLMANPLS_IMAGES_DIR "/network.png");
	CopyFile(APP_USRDIR "/pkgmanager.png"	, XMLMANPLS_IMAGES_DIR "/pkgmanager.png");
	CopyFile(APP_USRDIR "/refreshhtml.png"	, XMLMANPLS_IMAGES_DIR "/refreshhtml.png");
	CopyFile(APP_USRDIR "/refreshxml.png"	, XMLMANPLS_IMAGES_DIR "/refreshxml.png");
	CopyFile(APP_USRDIR "/restartps3.png"	, XMLMANPLS_IMAGES_DIR "/restartps3.png");
	CopyFile(APP_USRDIR "/settings.png"  	, XMLMANPLS_IMAGES_DIR "/settings.png");
	CopyFile(APP_USRDIR "/shutdownps3.png"	, XMLMANPLS_IMAGES_DIR "/shutdownps3.png");
	CopyFile(APP_USRDIR "/sysinfo.png"   	, XMLMANPLS_IMAGES_DIR "/sysinfo.png");
	CopyFile(APP_USRDIR "/tools.png"  		, XMLMANPLS_IMAGES_DIR "/tools.png");
	CopyFile(APP_USRDIR "/unload.png"  		, XMLMANPLS_IMAGES_DIR "/unload.png");
	CopyFile(APP_USRDIR "/usbredirect.png" 	, XMLMANPLS_IMAGES_DIR "/usbredirect.png");
	CopyFile(APP_USRDIR "/vshmenu.png"   	, XMLMANPLS_IMAGES_DIR "/vshmenu.png");
	CopyFile(APP_USRDIR "/webman.png"  		, XMLMANPLS_IMAGES_DIR "/webman.png");

	CopyFile(APP_USRDIR "/icon_wm_eject.png", XMLMANPLS_IMAGES_DIR "/icon_wm_eject.png");
	CopyFile(APP_USRDIR "/icon_wm_ps3.png"  , XMLMANPLS_IMAGES_DIR "/icon_wm_ps3.png");

	sysLv2FsMkdir(PLUGINS_DIR, 0777);

	// install vsh menu
	if(vsh_menu && sysLv2FsStat(PLUGINS_DIR, &stat) == SUCCESS)
	{
		sysLv2FsMkdir(PLUGINS_DIR "/images", 0777);

		// update images
		/* if(sysLv2FsStat(PLUGINS_DIR "/images/wm_vsh_menu.png",   &stat) != SUCCESS) */ CopyFile(APP_USRDIR "/images/wm_vsh_menu.png",   PLUGINS_DIR "/images/wm_vsh_menu.png");
		/* if(sysLv2FsStat(PLUGINS_DIR "/images/wm_vsh_menu_1.png", &stat) != SUCCESS) */ CopyFile(APP_USRDIR "/images/wm_vsh_menu_1.png", PLUGINS_DIR "/images/wm_vsh_menu_1.png");
		/* if(sysLv2FsStat(PLUGINS_DIR "/images/wm_vsh_menu_2.png", &stat) != SUCCESS) */ CopyFile(APP_USRDIR "/images/wm_vsh_menu_2.png", PLUGINS_DIR "/images/wm_vsh_menu_2.png");
		/* if(sysLv2FsStat(PLUGINS_DIR "/images/wm_vsh_menu_3.png", &stat) != SUCCESS) */ CopyFile(APP_USRDIR "/images/wm_vsh_menu_3.png", PLUGINS_DIR "/images/wm_vsh_menu_3.png");
		/* if(sysLv2FsStat(PLUGINS_DIR "/images/wm_vsh_menu_4.png", &stat) != SUCCESS) */ CopyFile(APP_USRDIR "/images/wm_vsh_menu_4.png", PLUGINS_DIR "/images/wm_vsh_menu_4.png");
		/* if(sysLv2FsStat(PLUGINS_DIR "/images/wm_vsh_menu_5.png", &stat) != SUCCESS) */ CopyFile(APP_USRDIR "/images/wm_vsh_menu_5.png", PLUGINS_DIR "/images/wm_vsh_menu_5.png");
		/* if(sysLv2FsStat(PLUGINS_DIR "/images/wm_vsh_menu_6.png", &stat) != SUCCESS) */ CopyFile(APP_USRDIR "/images/wm_vsh_menu_6.png", PLUGINS_DIR "/images/wm_vsh_menu_6.png");
		/* if(sysLv2FsStat(PLUGINS_DIR "/images/wm_vsh_menu_5.png", &stat) != SUCCESS) */ CopyFile(APP_USRDIR "/images/wm_vsh_menu_7.png", PLUGINS_DIR "/images/wm_vsh_menu_7.png");
		/* if(sysLv2FsStat(PLUGINS_DIR "/images/wm_vsh_menu_6.png", &stat) != SUCCESS) */ CopyFile(APP_USRDIR "/images/wm_vsh_menu_8.png", PLUGINS_DIR "/images/wm_vsh_menu_8.png");

		// append path if installing for first time
		if(sysLv2FsStat(PLUGINS_DIR "/wm_vsh_menu.sprx", &stat) != SUCCESS)
		{
			if(is_cobra())
			{
				// append line to boot_plugins.txt
				if(sysLv2FsStat(HDDROOT_DIR "/boot_plugins.txt", &stat) == SUCCESS)
					f = fopen(HDDROOT_DIR "/boot_plugins.txt", "a");
				else
					f = fopen(HDDROOT_DIR "/boot_plugins.txt", "w");
					fputs("\r\n" PLUGINS_DIR "/wm_vsh_menu.sprx", f);
					fclose(f);
			}
			if(is_mamba())
			{
				// append line to mamba_plugins.txt
				if(sysLv2FsStat(HDDROOT_DIR "/mamba_plugins.txt", &stat) == SUCCESS)
					f = fopen(HDDROOT_DIR "/mamba_plugins.txt", "a");
				else
					f = fopen(HDDROOT_DIR "/mamba_plugins.txt", "w");
					fputs("\r\n" PLUGINS_DIR "/wm_vsh_menu.sprx", f);
					fclose(f);
			}
			if(sysLv2FsStat(HDDROOT_DIR "/prx_plugins.txt", &stat) == SUCCESS)
			{
				// append line to prx_plugins.txt
				f = fopen(HDDROOT_DIR "/prx_plugins.txt", "a");
				fputs("\r\n" PLUGINS_DIR "/wm_vsh_menu.sprx", f);
				fclose(f);
			}
		}

		CopyFile(APP_USRDIR "/wm_vsh_menu.sprx", PLUGINS_DIR "/wm_vsh_menu.sprx");
	}

	// skip update custom language file
	if(sysLv2FsStat(LANG_DIR "/LANG_XX.TXT", &stat))
		CopyFile(APP_USRDIR "/LANG_XX.TXT", LANG_DIR "/LANG_XX.TXT");

	// skip update custom combo file
	if(sysLv2FsStat(TMP_DIR "/wm_custom_combo", &stat))
		CopyFile(APP_USRDIR "/wm_custom_combo", TMP_DIR "/wm_custom_combo");

	CopyFile(APP_USRDIR "/libfs.sprx", TMP_DIR "/libfs.sprx");

	sysLv2FsStat(APP_USRDIR "/raw_iso.sprx", &stat);
	u64 raw_iso_size = stat.st_size;

	// copy raw_iso.sprx to dev_flash
	if(sysLv2FsStat(FLASH_VSH_MODULE_DIR "/raw_iso.sprx", &stat) != SUCCESS || (stat.st_size != raw_iso_size))
	{
		if(sysLv2FsStat("/dev_blind", &stat) != SUCCESS)
			sys_fs_mount("CELL_FS_IOS:BUILTIN_FLSH1", "CELL_FS_FAT", "/dev_blind", 0);

		if(sysLv2FsStat("/dev_blind", &stat) == SUCCESS)
			CopyFile(APP_USRDIR "/raw_iso.sprx", REBUG_VSH_MODULE_DIR "/raw_iso.sprx");
	}

	// copy raw_iso.sprx to dev_hdd (if failed to copy it to dev_flash)
	if(sysLv2FsStat(FLASH_VSH_MODULE_DIR "/raw_iso.sprx", &stat) != SUCCESS)
	{
		if(sysLv2FsStat(PLUGINS_DIR, &stat) == SUCCESS)
		{
			CopyFile(APP_USRDIR "/raw_iso.sprx", PLUGINS_DIR "/raw_iso.sprx");
			if(sysLv2FsStat(PLUGINS_DIR "/raw_iso.sprx", &stat) == SUCCESS) sysLv2FsUnlink(HDDROOT_DIR "/raw_iso.sprx");
		}
		else
			CopyFile(APP_USRDIR "/raw_iso.sprx", HDDROOT_DIR "/raw_iso.sprx");
	}

	// copy standalone video recorder plugin (video_rec.sprx) to /plugins folder
	if((sysLv2FsStat(PLUGINS_DIR, &stat) == SUCCESS))
		CopyFile(APP_USRDIR "/video_rec.sprx", PLUGINS_DIR "/video_rec.sprx");

	// update PRX+Mamba Loader
	if((sysLv2FsStat(IRISMAN_USRDIR "/webftp_server.sprx", &stat) == SUCCESS))
	{
		sysLv2FsChmod(IRISMAN_USRDIR "/webftp_server.sprx", 0777);
		sysLv2FsUnlink(IRISMAN_USRDIR "/webftp_server.sprx");

		if(full)
			CopyFile(APP_USRDIR "/webftp_server_full.sprx", IRISMAN_USRDIR "/webftp_server.sprx");
		else if((sysLv2FsStat(REBUG_DIR, &stat) == SUCCESS) && is_ps3mapi())
			CopyFile(APP_USRDIR "/webftp_server_rebug_cobra_ps3mapi.sprx", IRISMAN_USRDIR "/webftp_server.sprx");
		else if((sysLv2FsStat(REBUG_DIR, &stat) == SUCCESS))
			CopyFile(APP_USRDIR "/webftp_server_rebug_cobra_multi23.sprx", IRISMAN_USRDIR "/webftp_server.sprx");
		else if(lite)
			CopyFile(APP_USRDIR "/webftp_server_lite.sprx", IRISMAN_USRDIR "/webftp_server.sprx");
		else if(is_ps3mapi())
			CopyFile(APP_USRDIR "/webftp_server_ps3mapi.sprx", IRISMAN_USRDIR "/webftp_server.sprx");
		else
			CopyFile(APP_USRDIR "/webftp_server.sprx", IRISMAN_USRDIR "/webftp_server.sprx");
	}
	else if((sysLv2FsStat(IRISMAN_USRDIR "/webftp_server_ps3mapi.sprx", &stat) == SUCCESS))
	{
		sysLv2FsChmod(IRISMAN_USRDIR "/webftp_server_ps3mapi.sprx", 0777);
		sysLv2FsUnlink(IRISMAN_USRDIR "/webftp_server_ps3mapi.sprx");

		if((sysLv2FsStat(REBUG_DIR, &stat) == SUCCESS))
			CopyFile(APP_USRDIR "/webftp_server_rebug_cobra_ps3mapi.sprx", IRISMAN_USRDIR "/webftp_server_ps3mapi.sprx");
		else
			CopyFile(APP_USRDIR "/webftp_server_ps3mapi.sprx", IRISMAN_USRDIR "/webftp_server_ps3mapi.sprx");
	}

	char line[255];

	// update PRX Loader
	if(sysLv2FsStat(PRXLOADER_USRDIR "/plugins.txt", &stat) == SUCCESS)
	{
		f = fopen(PRXLOADER_USRDIR "/plugins.txt", "r");
		while(fgets(line, 255, f) != NULL)
		{
			if(strstr(line,"webftp_server") != NULL)
			{
				fclose(f);
				strtok(line, "\r\n");
				sysLv2FsUnlink(line);
				CopyFile(APP_USRDIR "/webftp_server_noncobra.sprx",line);
				goto cont;
			}
		}
		fclose(f);
		f = fopen(PRXLOADER_USRDIR "/plugins.txt", "a");
		fputs("\r\n" PRXLOADER_USRDIR "/webftp_server_noncobra.sprx", f);
		fclose(f);

		sysLv2FsChmod(PRXLOADER_USRDIR "/webftp_server_noncobra.sprx", 0777);
		sysLv2FsUnlink(PRXLOADER_USRDIR "/webftp_server_noncobra.sprx");

		CopyFile(APP_USRDIR "/webftp_server_noncobra.sprx", PRXLOADER_USRDIR "/webftp_server_noncobra.sprx");
	}

cont:

	// update dev_flash (rebug)
	if(sysLv2FsStat(FLASH_VSH_MODULE_DIR "/webftp_server.sprx", &stat) == SUCCESS)
	{
		is_cobra(); // re-enable cobra if it's disabled

		if(sysLv2FsStat("/dev_blind", &stat) != SUCCESS)
			sys_fs_mount("CELL_FS_IOS:BUILTIN_FLSH1", "CELL_FS_FAT", "/dev_blind", 0);

		sysLv2FsChmod(REBUG_VSH_MODULE_DIR "/webftp_server.sprx", 0777);
		sysLv2FsUnlink(REBUG_VSH_MODULE_DIR "/webftp_server.sprx");

		sysLv2FsChmod(REBUG_VSH_MODULE_DIR "/webftp_server.sprx.bak", 0777);
		sysLv2FsUnlink(REBUG_VSH_MODULE_DIR "/webftp_server.sprx.bak");

		if(full)
			CopyFile(APP_USRDIR "/webftp_server_full.sprx", REBUG_VSH_MODULE_DIR "/webftp_server.sprx");
		else if(lite)
			CopyFile(APP_USRDIR "/webftp_server_lite.sprx", REBUG_VSH_MODULE_DIR "/webftp_server.sprx");
		else if(is_ps3mapi())
			CopyFile(APP_USRDIR "/webftp_server_rebug_cobra_ps3mapi.sprx", REBUG_VSH_MODULE_DIR "/webftp_server.sprx");
		else
			CopyFile(APP_USRDIR "/webftp_server_rebug_cobra_multi23.sprx", REBUG_VSH_MODULE_DIR "/webftp_server.sprx");


		// delete webMAN from hdd0
		if((sysLv2FsStat(REBUG_VSH_MODULE_DIR "/webftp_server.sprx", &stat) == SUCCESS))
		{
			sysLv2FsChmod(HDDROOT_DIR "/webftp_server.sprx", 0777);
			sysLv2FsUnlink(HDDROOT_DIR "/webftp_server.sprx");

			sysLv2FsChmod(PLUGINS_DIR "/webftp_server.sprx", 0777);
			sysLv2FsUnlink(PLUGINS_DIR "/webftp_server.sprx");

			if(sysLv2FsStat(HDDROOT_DIR "/boot_plugins.txt", &stat) == SUCCESS)
			{
				f = fopen(HDDROOT_DIR "/boot_plugins.txt", "r");
				while(fgets(line, 255, f) != NULL)
				{
					if(strstr(line,"webftp_server") != NULL && strstr(line,"/dev_blind") == NULL)
					{
						strtok(line, "\r\n");
						sysLv2FsChmod(line, 0777);
						sysLv2FsUnlink(line);
						break;
					}
				}
				fclose(f);
			}
		}

		// reboot
		sysLv2FsUnlink(TMP_DIR "/turnoff");
		//{lv2syscall4(379,0x200,0,0,0); return_to_user_prog(int);}
		//{lv2syscall4(379,0x1200,0,0,0); return_to_user_prog(int);}
		{lv2syscall3(SC_SYS_POWER, SYS_REBOOT, 0, 0); return_to_user_prog(int);}
		//{lv2syscall3(SC_SYS_POWER, SYS_REBOOT, 0, 0);}

		return 0;
	}
	else if(sysLv2FsStat(FLASH_VSH_MODULE_DIR "/webftp_server.sprx.bak", &stat) == SUCCESS)
	{
		if(sysLv2FsStat("/dev_blind", &stat) != SUCCESS)
			sys_fs_mount("CELL_FS_IOS:BUILTIN_FLSH1", "CELL_FS_FAT", "/dev_blind", 0);

		sysLv2FsChmod(REBUG_VSH_MODULE_DIR "/webftp_server.sprx.bak", 0777);
		sysLv2FsUnlink(REBUG_VSH_MODULE_DIR "/webftp_server.sprx.bak");

		if(full)
			CopyFile(APP_USRDIR "/webftp_server_full.sprx", REBUG_VSH_MODULE_DIR "/webftp_server.sprx.bak");
		else if(lite)
			CopyFile(APP_USRDIR "/webftp_server_lite.sprx", REBUG_VSH_MODULE_DIR "/webftp_server.sprx.bak");
		else if(is_ps3mapi())
			CopyFile(APP_USRDIR "/webftp_server_rebug_cobra_ps3mapi.sprx", REBUG_VSH_MODULE_DIR "/webftp_server.sprx.bak");
		else
			CopyFile(APP_USRDIR "/webftp_server_rebug_cobra_multi23.sprx", REBUG_VSH_MODULE_DIR "/webftp_server.sprx.bak");
	}

	// update boot_plugins.txt
	if(lite || full || is_cobra())
	{
		// parse boot_plugins.txt (update existing path)
		if(sysLv2FsStat(HDDROOT_DIR "/boot_plugins.txt", &stat) == SUCCESS)
		{
			f = fopen(HDDROOT_DIR "/boot_plugins.txt", "r");
			while(fgets(line, 255, f) != NULL)
			{
				if(strstr(line,"webftp_server") != NULL)
				{
					fclose(f);
					strtok(line, "\r\n");
					sysLv2FsChmod(line, 0777);
					sysLv2FsUnlink(line);
					if(full)
						CopyFile(APP_USRDIR "/webftp_server_full.sprx", line);
					else if(lite)
						CopyFile(APP_USRDIR "/webftp_server_lite.sprx", line);
					else
					{
						if(is_ps3mapi())
							CopyFile(APP_USRDIR "/webftp_server_ps3mapi.sprx", line);
						else
							CopyFile(APP_USRDIR "/webftp_server.sprx", line);
					}
					goto exit;
				}
			}
			fclose(f);
		}

		// append line to boot_plugins.txt
		if(sysLv2FsStat(HDDROOT_DIR "/boot_plugins.txt", &stat) == SUCCESS)
			f = fopen(HDDROOT_DIR "/boot_plugins.txt", "a");
		else
			f = fopen(HDDROOT_DIR "/boot_plugins.txt", "w");
		if((sysLv2FsStat(PLUGINS_DIR, &stat) == SUCCESS))
		{
			if(is_ps3mapi() && !lite && !full)
				fputs("\r\n" PLUGINS_DIR "/webftp_server_ps3mapi.sprx", f);
			else
				fputs("\r\n" PLUGINS_DIR "/webftp_server.sprx", f);
		}
		else
		{
			if(is_ps3mapi() && !lite && !full)
				fputs("\r\n" HDDROOT_DIR "/webftp_server_ps3mapi.sprx", f);
			else
				fputs("\r\n" HDDROOT_DIR "/webftp_server.sprx", f);
		}
		fclose(f);

		// delete old sprx
		sysLv2FsChmod(HDDROOT_DIR "/webftp_server.sprx", 0777);
		sysLv2FsUnlink(HDDROOT_DIR "/webftp_server.sprx");

		sysLv2FsChmod(PLUGINS_DIR "/webftp_server.sprx", 0777);
		sysLv2FsUnlink(PLUGINS_DIR "/webftp_server.sprx");

		sysLv2FsChmod(HDDROOT_DIR "/webftp_server_ps3mapi.sprx", 0777);
		sysLv2FsUnlink(HDDROOT_DIR "/webftp_server_ps3mapi.sprx");

		sysLv2FsChmod(PLUGINS_DIR "/webftp_server_ps3mapi.sprx", 0777);
		sysLv2FsUnlink(PLUGINS_DIR "/webftp_server_ps3mapi.sprx");

		// copy ps3mapi/cobra/rebug/lite sprx
		if((sysLv2FsStat(PLUGINS_DIR, &stat) == SUCCESS))
		{
			if(full)
				CopyFile(APP_USRDIR "/webftp_server_full.sprx", PLUGINS_DIR "/webftp_server.sprx");
			else if(lite)
				CopyFile(APP_USRDIR "/webftp_server_lite.sprx", PLUGINS_DIR "/webftp_server.sprx");
			else
			{
				if(is_ps3mapi())
				{
					if((sysLv2FsStat(REBUG_DIR, &stat) == SUCCESS))
						CopyFile(APP_USRDIR "/webftp_server_rebug_cobra_ps3mapi.sprx", PLUGINS_DIR "/webftp_server_ps3mapi.sprx");
					else
						CopyFile(APP_USRDIR "/webftp_server_ps3mapi.sprx", PLUGINS_DIR "/webftp_server_ps3mapi.sprx");
				}
				else
					CopyFile(APP_USRDIR "/webftp_server.sprx", PLUGINS_DIR "/webftp_server.sprx");
			}
		}
		else
		{
			if(full)
				CopyFile(APP_USRDIR "/webftp_server_full.sprx", HDDROOT_DIR "/webftp_server.sprx");
			else if(lite)
				CopyFile(APP_USRDIR "/webftp_server_lite.sprx", HDDROOT_DIR "/webftp_server.sprx");
			else
			{
				if(is_ps3mapi())
				{
					if((sysLv2FsStat(REBUG_DIR, &stat) == SUCCESS))
						CopyFile(APP_USRDIR "/webftp_server_rebug_cobra_ps3mapi.sprx", HDDROOT_DIR "/webftp_server_ps3mapi.sprx");
					else
						CopyFile(APP_USRDIR "/webftp_server_ps3mapi.sprx", HDDROOT_DIR "/webftp_server_ps3mapi.sprx");
				}
				else
					CopyFile(APP_USRDIR "/webftp_server.sprx", HDDROOT_DIR "/webftp_server.sprx");
			}
		}
	}

	// update mamba_plugins.txt
	if(is_mamba())
	{
		// parse mamba_plugins.txt (update existing path)
		if(sysLv2FsStat(HDDROOT_DIR "/mamba_plugins.txt", &stat) == SUCCESS)
		{
			f = fopen(HDDROOT_DIR "/mamba_plugins.txt", "r");
			while(fgets(line, 255, f) != NULL)
			{
				if(strstr(line,"webftp_server") != NULL)
				{
					fclose(f);
					strtok(line, "\r\n");
					sysLv2FsChmod(line, 0777);
					sysLv2FsUnlink(line);

					if(full)
						CopyFile(APP_USRDIR "/webftp_server_full.sprx", line);
					else if(lite)
						CopyFile(APP_USRDIR "/webftp_server_lite.sprx", line);
					else
						CopyFile(APP_USRDIR "/webftp_server_ps3mapi.sprx", line);
					goto exit;
				}
			}
			fclose(f);
		}

		// append line to mamba_plugins.txt (Mamba/PRX Loader - PS3MAPI)
		if(sysLv2FsStat(HDDROOT_DIR "/mamba_plugins.txt", &stat) == SUCCESS)
			f = fopen(HDDROOT_DIR "/mamba_plugins.txt", "a");
		else
			f = fopen(HDDROOT_DIR "/mamba_plugins.txt", "w");
		if((sysLv2FsStat(PLUGINS_DIR, &stat) == SUCCESS))
		{
			if(full || lite)
				fputs("\r\n" PLUGINS_DIR "/webftp_server.sprx", f);
			else
				fputs("\r\n" PLUGINS_DIR "/webftp_server_ps3mapi.sprx", f);
		}
		else if(full || lite)
			fputs("\r\n" HDDROOT_DIR "/webftp_server.sprx", f);
		else
			fputs("\r\n" HDDROOT_DIR "/webftp_server_ps3mapi.sprx", f);
		fclose(f);

		// delete old sprx
		sysLv2FsChmod(HDDROOT_DIR "/webftp_server_ps3mapi.sprx", 0777);
		sysLv2FsUnlink(HDDROOT_DIR "/webftp_server_ps3mapi.sprx");

		sysLv2FsChmod(PLUGINS_DIR "/webftp_server_ps3mapi.sprx", 0777);
		sysLv2FsUnlink(PLUGINS_DIR "/webftp_server_ps3mapi.sprx");

		// copy ps3mapi sprx
		if((sysLv2FsStat(PLUGINS_DIR, &stat) == SUCCESS))
		{
			if(full)
				CopyFile(APP_USRDIR "/webftp_server_full.sprx", PLUGINS_DIR "/webftp_server.sprx");
			else if(lite)
				CopyFile(APP_USRDIR "/webftp_server_lite.sprx", PLUGINS_DIR "/webftp_server.sprx");
			else if((sysLv2FsStat(REBUG_DIR, &stat) == SUCCESS))
				CopyFile(APP_USRDIR "/webftp_server_rebug_cobra_ps3mapi.sprx", PLUGINS_DIR "/webftp_server_ps3mapi.sprx");
			else
				CopyFile(APP_USRDIR "/webftp_server_ps3mapi.sprx", PLUGINS_DIR "/webftp_server_ps3mapi.sprx");
		}
		else
		{
			if(full)
				CopyFile(APP_USRDIR "/webftp_server_full.sprx", HDDROOT_DIR "/webftp_server.sprx");
			else if(lite)
				CopyFile(APP_USRDIR "/webftp_server_lite.sprx", HDDROOT_DIR "/webftp_server.sprx");
			else if((sysLv2FsStat(REBUG_DIR, &stat) == SUCCESS))
				CopyFile(APP_USRDIR "/webftp_server_rebug_cobra_ps3mapi.sprx", HDDROOT_DIR "/webftp_server_ps3mapi.sprx");
			else
				CopyFile(APP_USRDIR "/webftp_server_ps3mapi.sprx", HDDROOT_DIR "/webftp_server_ps3mapi.sprx");
		}
	}

	// update prx_plugins.txt (PRX LOADER)
	if(sysLv2FsStat(HDDROOT_DIR "/prx_plugins.txt", &stat) == SUCCESS)
	{
		// parse prx_plugins.txt (update existing path)
		f = fopen(HDDROOT_DIR "/prx_plugins.txt", "r");
		while(fgets(line, 255, f) != NULL)
		{
			if(strstr(line,"webftp_server") != NULL)
			{
				fclose(f);
				strtok(line, "\r\n");
				sysLv2FsUnlink(line);
				CopyFile(APP_USRDIR "/webftp_server_noncobra.sprx",line);
				goto exit;
			}
		}
		fclose(f);

		// append line to prx_plugins.txt
		f = fopen(HDDROOT_DIR "/prx_plugins.txt", "a");

		if(sysLv2FsStat(PLUGINS_DIR, &stat) == SUCCESS)
			fputs("\r\n" PLUGINS_DIR "/webftp_server_noncobra.sprx", f);
		else
			fputs("\r\n" HDDROOT_DIR "/webftp_server_noncobra.sprx", f);
		fclose(f);

		// delete old sprx
		sysLv2FsChmod(HDDROOT_DIR "/webftp_server_noncobra.sprx", 0777);
		sysLv2FsUnlink(HDDROOT_DIR "/webftp_server_noncobra.sprx");

		sysLv2FsChmod(PLUGINS_DIR "/webftp_server_noncobra.sprx", 0777);
		sysLv2FsUnlink(PLUGINS_DIR "/webftp_server_noncobra.sprx");

		// copy non cobra sprx
		if(sysLv2FsStat(PLUGINS_DIR, &stat) == SUCCESS)
			CopyFile(APP_USRDIR "/webftp_server_noncobra.sprx", PLUGINS_DIR "/webftp_server_noncobra.sprx");
		else
			CopyFile(APP_USRDIR "/webftp_server_noncobra.sprx", HDDROOT_DIR "/webftp_server_noncobra.sprx");
	}
	// exit
exit:

	// update category_game.xml (add fb.xml)
	if(add_mygame() != -2);


	// reboot
	sysLv2FsUnlink(TMP_DIR "/turnoff");
	//{lv2syscall4(379,0x200,0,0,0); return_to_user_prog(int);}
	//{lv2syscall4(379,0x1200,0,0,0); return_to_user_prog(int);}
	{lv2syscall3(SC_SYS_POWER, SYS_REBOOT, 0, 0); return_to_user_prog(int);}

	return 0;
}
