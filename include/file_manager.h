#define SC_FS_DISK_FREE		840

#define ICON_STYLE			" style=\"position:fixed;top:118px;right:10px;max-height:176px;z-index:-1;display:none\" onerror=\"this.style.display='none';\""

u32 _LINELEN = LINELEN;
u32 _MAX_PATH_LEN = MAX_PATH_LEN;
u32 _MAX_LINE_LEN = MAX_LINE_LEN;

#define _2MB_	0x200000ULL

static void add_list_entry(char *tempstr, bool is_dir, char *ename, char *templn, char *name, char *fsize, CellRtcDateTime rDate, u16 flen, unsigned long long sz, char *sf, u8 is_net, u8 show_icon0, u8 is_ps3_http)
{
	unsigned long long sbytes = sz; bool is_root = false;

	if(sz > 0x4000000000000ULL) sz = 0, is_dir = true; // fix host_root, app_home

	if(sz < 10240)			{sprintf(sf, "%s", STR_BYTE);} else
	if(sz < _2MB_)	{sprintf(sf, "%s", STR_KILOBYTE); sz >>= 10;} else
	if(sz < 0xC00000000ULL) {sprintf(sf, "%s", STR_MEGABYTE); sz >>= 20;} else
							{sprintf(sf, "%s", STR_GIGABYTE); sz >>= 30;}

	// encode file name for html
	htmlenc(tempstr, name, 0); strcpy(name, tempstr);

	flen = strlen(name); char *ext = name + MAX(flen - 4, 0); fsize[0] = NULL;

	if( !is_dir && !strcmp(ext, ".SFO") )
	{	//get title & app version from PARAM.SFO
		strcpy(tempstr, templn);
		getTitleID(tempstr, fsize, GET_VERSION);
		getTitleID(tempstr, ename, GET_TITLE_AND_ID); if(fsize[0]) {strcat(tempstr, " v"); strcat(tempstr, fsize);}
		sprintf(fsize, "<label title=\"%s\">%s</label><div style='position:absolute;top:300px;right:10px;font-size:14px'>%s</div>", tempstr, name, tempstr); strcpy(name, fsize);
	}

	// encode url for html
	if(urlenc(tempstr, templn)) strncpy(templn, tempstr, _MAX_LINE_LEN);

	// is image?
	u8 show_img = !is_ps3_http && (!is_dir && (!strcasecmp(ext, ".png") || !strcasecmp(ext, ".jpg") || !strcasecmp(ext, ".bmp")));

	// build size column
	if(is_dir)
	{
		bool show_play = ((flen == 8) && !strcmp(name, "dev_bdvd") && IS_ON_XMB);

		if(name[0] == '.')
			sprintf(fsize, HTML_URL, templn, HTML_DIR);
		else if(flen == 9 && !strcmp(name, "dev_blind"))
			sprintf(fsize, "<a href=\"%s?0\">%s</a>", templn, HTML_DIR);
		else if(show_play && (isDir("/dev_bdvd/PS3_GAME") || file_exists("/dev_bdvd/SYSTEM.CNF")))
			sprintf(fsize, HTML_URL, "/play.ps3", "&lt;Play>");
		else if(show_play && isDir("/dev_bdvd/BDMV"))
			sprintf(fsize, HTML_URL, "/play.ps3", "&lt;BDV>");
		else if(show_play && isDir("/dev_bdvd/VIDEO_TS"))
			sprintf(fsize, HTML_URL, "/play.ps3", "&lt;DVD>");
		else if(show_play && isDir("/dev_bdvd/AVCHD"))
			sprintf(fsize, HTML_URL, "/play.ps3", "&lt;AVCHD>");
#ifdef FIX_GAME
		else if(islike(templn, HDD0_GAME_DIR) || (strstr(templn + 10, "/PS3_GAME" ) != NULL))
			sprintf(fsize, "<a href=\"/fixgame.ps3%s\">%s</a>", templn, HTML_DIR);
#endif
#ifdef COPY_PS3
		else if(!is_net && ( (flen == 5 && (!strcasecmp(name, "VIDEO") || strcasestr(name, "music"))) || (flen == 6 && !strcasecmp(name, "covers")) || !strcmp(name, "savedata") || !strcmp(name, "exdata") || !strcmp(name, "home") ))
			sprintf(fsize, "<a href=\"/copy.ps3%s\" title=\"copy to %s\">%s</a>", templn, islike(templn, "/dev_hdd0") ? "/dev_usb000" : "/dev_hdd0", HTML_DIR);
#endif
		else
		if(strlen(templn) <= 11 && islike(templn, "/dev_"))
		{
			uint64_t freeSize = 0, devSize = 0; is_root = true;
			system_call_3(SC_FS_DISK_FREE, (uint64_t)(uint32_t)templn, (uint64_t)(uint32_t)&devSize, (uint64_t)(uint32_t)&freeSize);

			unsigned long long	free_mb    = (unsigned long long)(freeSize>>20),
								free_kb    = (unsigned long long)(freeSize>>10),
								devsize_mb = (unsigned long long)(devSize>>20);

			// show graphic of device size & free space
			sprintf(fsize,  "<div class='bf' style='height:18px;text-align:left;overflow:hidden;'><div class='bu' style='height:18px;width:%i%%'></div><div style='position:relative;top:-%ipx;text-align:right'>"
							"<a href=\"/mount.ps3%s\" title=\"%'llu %s (%'llu %s) / %'llu %s (%'llu %s)\">&nbsp; %'8llu %s &nbsp;</a>"
							"</div></div>", (int)(100.0f * (float)(devSize - freeSize) / (float)devSize), is_ps3_http ? 20 : 18, templn, free_mb, STR_MBFREE, freeSize, STR_BYTE, devsize_mb, STR_MEGABYTE, devSize, STR_BYTE, (freeSize < _2MB_) ? free_kb : free_mb, (freeSize < _2MB_) ? STR_KILOBYTE : STR_MEGABYTE);
		}
		else
#ifdef PS2_DISC
			sprintf(fsize, "<a href=\"/mount%s%s\">%s</a>", strstr(name, "[PS2")?".ps2":".ps3", templn, HTML_DIR);
#else
			sprintf(fsize, "<a href=\"/mount.ps3%s\">%s</a>", templn, HTML_DIR);
#endif
	}
#ifdef COBRA_ONLY
	else if( ((flen > 4) && strcasestr(ISO_EXTENSIONS, ext)!=NULL && !islike(templn, HDD0_GAME_DIR)) || (!is_net && ( strstr(name + MAX(flen - 13, 0), ".ntfs[") || !extcmp(name + MAX(flen - 8, 0), ".BIN.ENC", 8) )) )
	{
		if( (strcasestr(name, ".iso.") != NULL) && extcasecmp(name, ".iso.0", 6) )
			sprintf(fsize, "<label title=\"%'llu %s\"> %'llu %s</label>", sbytes, STR_BYTE, sz, sf);
		else
			sprintf(fsize, "<a href=\"/mount.ps3%s\" title=\"%'llu %s\">%'llu %s</a>", templn, sbytes, STR_BYTE, sz, sf);
	}
#endif

#ifdef PKG_HANDLER
	else if( !strcmp(ext, ".pkg") || !strcmp(ext, ".PKG") )
			sprintf(fsize, "<a href=\"/install.ps3%s\">%'llu %s</a>", templn, sz, sf);
#endif

#ifdef COPY_PS3
	else if(   !strcmp(ext, ".pkg") || !strcmp(ext, ".p3t") || !extcmp(name, ".edat", 5)
			|| !strcmp(ext, ".rco") || !strcmp(ext, ".qrc") || !memcmp(name, "coldboot", 8)
			|| !memcmp(name, "webftp_server", 13) || !memcmp(name, "boot_plugins_", 13)
			|| show_img
			|| !strcasecmp(ext, ".mp4") || !strcasecmp(ext, ".mkv") || !strcasecmp(ext, ".avi")
			|| !strcasecmp(ext, ".mp3")
 #ifdef SWAP_KERNEL
			|| !memcmp(name, "lv2_kernel", 10)
 #endif
			)
			sprintf(fsize, "<a href=\"/copy.ps3%s\" title=\"%'llu %s copy to %s\">%'llu %s</a>", templn, sbytes, STR_BYTE, islike(templn, "/dev_hdd0") ? "/dev_usb000" : "/dev_hdd0", sz, sf);
	else if( !extcmp(name, ".bak", 4) )
			sprintf(fsize, "<a href=\"/rename.ps3%s|\">%'llu %s</a>", templn, sz, sf);
#endif //#ifdef COPY_PS3

#ifdef LOAD_PRX
	else if(!is_net && ( !extcmp(name, ".sprx", 5)))
		sprintf(fsize, "<a href=\"/loadprx.ps3?slot=6&prx=%s\">%'llu %s</a>", templn, sz, sf);
#endif
	else if( (sz <= MAX_TEXT_LEN) && (strcasestr(".txt|.ini|.log|.sfx|.xml|.cfg|.his|.hip|.bup|.css|.html|conf|name", ext)!=NULL || strstr(templn, "wm_custom")!=NULL ) )
			sprintf(fsize, "<a href=\"/edit.ps3%s\">%'llu %s</a>", templn, sz, sf);
	else if(sbytes < 10240)
		sprintf(fsize, "%'llu %s", sz, sf);
	else
		sprintf(fsize, "<label title=\"%'llu %s\"> %'llu %s</label>", sbytes, STR_BYTE, sz, sf);

	snprintf(ename, 6, "%s    ", name);

	sprintf(tempstr, "%c%c%c%c%c%c<tr>"
					 "<td><a %s href=\"%s\"%s>%s</a></td>",
					is_dir ? '0' : '1', ename[0], ename[1], ename[2], ename[3], ename[4],
					is_dir ? "class=\"d\"" : "class=\"w\"", templn,
					show_img ? " onmouseover=\"s(this,0);\"" : (is_dir && show_icon0) ? " onmouseover=\"s(this,1);\"" : "", name);

	flen = strlen(tempstr);

	if(flen >= _LINELEN)
	{
		if(is_dir) sprintf(fsize, HTML_DIR); else sprintf(fsize, "%llu %s", sz, sf);

		sprintf(tempstr, "%c%c%c%c%c%c<tr>"
						 "<td><a %s href=\"%s\">%s</a></td>",
		is_dir ? '0' : '1', ename[0], ename[1], ename[2], ename[3], ename[4],
		is_dir ? "class=\"d\"" : "class=\"w\"", templn, name);

		flen = strlen(tempstr);

		if(flen >= _LINELEN)
		{
			if(is_dir) sprintf(fsize, HTML_DIR); else sprintf(fsize, "%llu %s", sz, sf);

			sprintf(tempstr, "%c%c%c%c%c%c<tr>"
							 "<td>%s</td>",
			is_dir ? '0' : '1', ename[0], ename[1], ename[2], ename[3], ename[4],
			name);
		}
	}

	sprintf(templn, "<td> %s%s</td>"
					"<td>%02i-%s-%04i %02i:%02i</td></tr>",
					fsize, is_root ? "" : " &nbsp; ",
					rDate.day, smonth[rDate.month-1], rDate.year, rDate.hour, rDate.minute);

	strcat(tempstr, templn);

	flen = strlen(tempstr);
	if(flen >= _LINELEN) {flen=0; tempstr[0] = NULL;} //ignore file if it is still too long
}

static void add_breadcrumb_trail(char *buffer, char *param)
{
	u32 tlen = 0;

	char swap[_MAX_PATH_LEN], templn[_MAX_PATH_LEN], url[_MAX_PATH_LEN], *slash;

	strcpy(templn, param);
	while((slash = strchr(templn+1, '/')))
	{
		slash[0] = NULL;
		tlen+=strlen(templn)+1;

		strcpy(swap, param);
		swap[tlen] = NULL;

		strcat(buffer, "<a class=\"f\" href=\"");
		urlenc(url, swap);
		strcat(buffer, url);

		htmlenc(url, templn, 1);
		sprintf(swap, "\">%s</a>/", templn);
		strcat(buffer, swap);

		strcpy(templn, param+tlen);
	}

	if(!param[1]) sprintf(swap, "/");
	else
	{
		char label[_MAX_PATH_LEN];

		urlenc(url, param); htmlenc(label, templn, 0);
		sprintf(swap, HTML_URL2,
#ifdef FIX_GAME
						islike(param, HDD0_GAME_DIR) ? "/fixgame.ps3" :
#endif
#ifdef PKG_HANDLER
						!extcmp(param, ".pkg", 4) ? "/install.ps3" :
#endif
						"/mount.ps3", url, label);
	}
	strcat(buffer, swap);
}

static bool folder_listing(char *buffer, u32 BUFFER_SIZE_HTML, char *templn, char *param, int conn_s, char *tempstr, char *header, u8 is_ps3_http)
{
	struct CellFsStat buf;
	int fd;

	CellRtcDateTime rDate;

	if(islike(param, "/dev_blind?"))
	{
		if(strstr(param, "?1")) enable_dev_blind(NO_MSG);
		if(strstr(param, "?0")) disable_dev_blind();

		sprintf(templn, "/dev_blind: %s", isDir("/dev_blind")?STR_ENABLED:STR_DISABLED); strcat(buffer, templn); return true; //goto send_response;
	}

	absPath(templn, param, "/"); // auto mount /dev_blind

	u8 is_net = (param[1] == 'n');

	if(copy_aborted) strcat(buffer, STR_CPYABORT);    //  /copy.ps3$abort
	else
	if(fix_aborted)  strcat(buffer, "Fix aborted!");  //  /fixgame.ps3$abort

	if(copy_aborted | fix_aborted) {strcat(buffer, "<p>"); sys_timer_usleep(100000); copy_aborted = fix_aborted = false;}

	_LINELEN = LINELEN;
	_MAX_PATH_LEN = MAX_PATH_LEN;
	_MAX_LINE_LEN = MAX_LINE_LEN;

	if(strstr(param, "/exdata")) _LINELEN = _MAX_LINE_LEN = _MAX_PATH_LEN = 320;

	if(is_net || cellFsOpendir(param, &fd) == CELL_FS_SUCCEEDED)
	{
		CellFsDirent entry;
		u64 read_e;
		unsigned long long sz=0, dir_size=0;
		char sf[8];
		char fsize[_LINELEN];
		char ename[16];
		char swap[_MAX_PATH_LEN];
		u16 idx = 0, dirs = 0, flen; bool is_dir;
		u32 tlen = strlen(buffer); buffer[tlen] = NULL;
		char *sysmem_html = buffer + _6KB_;

		typedef struct
		{
			char path[_LINELEN];
		}
		t_line_entries;
		t_line_entries *line_entry = (t_line_entries *)sysmem_html;
		u16 max_entries = ((BUFFER_SIZE_HTML - _6KB_) / _MAX_LINE_LEN)-1; tlen = 0;

		BUFFER_SIZE_HTML -= _2KB_;

		u8 jb_games = (strstr(param, "/GAMES") || strstr(param, "/GAMEZ"));
		u8 show_icon0 = jb_games || (islike(param, "/dev_hdd0/game") || islike(param, "/dev_hdd0/home/"));

		sprintf(templn, "<img id=\"icon\"%s>"
						"<script>"
						// show icon of item pointed with mouse
						"function s(o,d){u=o.href;p=u.indexOf('.ps3');if(p>0)u=u.substring(p+4);if(d){p=u.indexOf('/PS3_');if(p<0)p=u.indexOf('/USRDIR');if(p>0)u=u.substring(0,p);u+='%s/ICON0.PNG';}icon.src=u;icon.style.display='block';}"
						"</script>", ICON_STYLE, (jb_games ? "/PS3_GAME" : "")); strcat(buffer, templn);

		// breadcrumb trail //
		add_breadcrumb_trail(buffer, param);

		if((param[7] == 'v' || param[7] == 'm') && IS_ON_XMB && (isDir("/dev_bdvd/PS3_GAME") || file_exists("/dev_bdvd/SYSTEM.CNF") || isDir("/dev_bdvd/BDMV") || isDir("/dev_bdvd/VIDEO_TS") || isDir("/dev_bdvd/AVCHD")))
			strcat(buffer, ": <a href=\"/play.ps3\">&lt;Play>&nbsp;</a><br>");
		else
			strcat(buffer, ":<br>");

		strcat(buffer, "<table class=\"propfont\"><tr><td colspan=3><col width=\"220\"><col width=\"98\">");

		tlen = 0;

 #ifdef COBRA_ONLY
  #ifndef LITE_EDITION
		if(is_net)
		{
			int ns = FAILED, abort_connection = 0;

			if(param[4] >= '0' && param[4] <= '4') ns = connect_to_remote_server((param[4]  & 0xFF) - '0');

			if(ns >= 0)
			{
				strcat(param, "/");
				if(open_remote_dir(ns, param+5, &abort_connection) >= 0)
				{
					strcpy(templn, param); if(templn[strlen(templn)-1] == '/') templn[strlen(templn)-1] = NULL;
					if(strrchr(templn, '/')) templn[strrchr(templn, '/')-templn] = NULL; if(strlen(templn)<6 && strlen(param)<8) {templn[0]='/'; templn[1] = NULL;}

					urlenc(swap, templn);
					sprintf(line_entry[idx].path, "!00000<tr>"
												  "<td><a class=\"f\" href=\"%s\">..</a></td>"
												  "<td> " HTML_URL " &nbsp; </td>"
												  "<td>11-Nov-2006 11:11</td>"
												  "</tr>", swap, swap, HTML_DIR);

					flen = strlen(line_entry[idx].path);
					if(flen >= _MAX_LINE_LEN) return false; //ignore lines too long
					idx++; dirs++;
					tlen += flen;

					sys_addr_t data = 0;
					netiso_read_dir_result_data *dir_items = NULL;
					int v3_entries = 0;
					v3_entries = read_remote_dir(ns, &data, &abort_connection);
					if(data != NULL)
					{
						dir_items = (netiso_read_dir_result_data*)data;

						for(int n = 0; n < v3_entries; n++)
						{
							if(dir_items[n].name[0] == '.' && dir_items[n].name[1] == 0) continue;
							if(tlen > BUFFER_SIZE_HTML) break;
							if(idx >= (max_entries-3)) break;

							if(param[1] == 0)
								sprintf(templn, "/%s", dir_items[n].name);
							else
							{
								sprintf(templn, "%s%s", param, dir_items[n].name);
							}
							flen = strlen(templn) - 1; if(templn[flen] == '/') templn[flen] = NULL;

							cellRtcSetTime_t(&rDate, dir_items[n].mtime);

							sz=(unsigned long long)dir_items[n].file_size; dir_size+=sz;

							is_dir=dir_items[n].is_directory; if(is_dir) dirs++;

							add_list_entry(tempstr, is_dir, ename, templn, dir_items[n].name, fsize, rDate, flen, sz, sf, true, show_icon0, is_ps3_http);

							flen = strlen(tempstr);
							if((flen == 0) || (flen > _MAX_LINE_LEN)) continue; //ignore lines too long
							strcpy(line_entry[idx].path, tempstr); idx++;
							tlen += flen;

							if(!working) break;
						}
						sys_memory_free(data);
					}
				}
				else //may be a file
				{
					flen = strlen(param) - 1; if(param[flen] == '/') param[flen] = NULL;

					int is_directory = 0;
					int64_t file_size;
					u64 mtime, ctime, atime;
					if(remote_stat(ns, param+5, &is_directory, &file_size, &mtime, &ctime, &atime, &abort_connection) == 0)
					{
						if(file_size && !is_directory)
						{
							if(open_remote_file(ns, param+5, &abort_connection) > 0)
							{
								prepare_header(header, param, 1);
								sprintf(templn, "Content-Length: %llu\r\n\r\n", (unsigned long long)file_size); strcat(header, templn);

								ssend(conn_s, header);
								int bytes_read, boff=0;
								while(boff < file_size)
								{
									bytes_read = read_remote_file(ns, (char*)buffer, boff, _64KB_, &abort_connection);
									if(bytes_read)
									{
										if(send(conn_s, buffer, bytes_read, 0) < 0) break;
									}
									boff+=bytes_read;
									if((uint32_t)bytes_read < _64KB_ || boff >= file_size) break;
								}
								open_remote_file(ns, (char*)"/CLOSEFILE", &abort_connection);
								shutdown(ns, SHUT_RDWR); socketclose(ns);
								sclose(&conn_s);
								return false;
							}
						}
					}
				}
				shutdown(ns, SHUT_RDWR); socketclose(ns);
			}
		}
		else
  #endif
 #endif
		{
			while(cellFsReaddir(fd, &entry, &read_e) == 0 && read_e > 0)
			{
				if(entry.d_name[0] == '.' && entry.d_name[1] == 0) continue;
				if(tlen > BUFFER_SIZE_HTML) break;
				if(idx >= (max_entries-3)) break;

				if(param[1] == 0)
					sprintf(templn, "/%s", entry.d_name);
				else
				{
					sprintf(templn, "%s/%s", param, entry.d_name);
				}
				flen = strlen(templn) - 1; if(templn[flen] == '/') templn[flen] = NULL;

				cellFsStat(templn, &buf);
				cellRtcSetTime_t(&rDate, buf.st_mtime);

				sz = (unsigned long long)buf.st_size; dir_size += sz;

				is_dir = (buf.st_mode & S_IFDIR); if(is_dir) dirs++;

				add_list_entry(tempstr, is_dir, ename, templn, entry.d_name, fsize, rDate, flen, sz, sf, false, show_icon0, is_ps3_http);

				flen = strlen(tempstr);
				if((flen == 0) || (flen > _MAX_LINE_LEN)) continue; //ignore lines too long
				strcpy(line_entry[idx].path, tempstr); idx++;
				tlen += flen;

				if(!working) break;
			}
			cellFsClosedir(fd);
		}

#ifdef COBRA_ONLY
 #ifndef LITE_EDITION
		if(strlen(param)<4)
		{
			for(u8 n=0; n<5; n++)
			if( (n == 0 && (webman_config->netd0 && webman_config->neth0[0] && webman_config->netp0))
			||	(n == 1 && (webman_config->netd1 && webman_config->neth1[0] && webman_config->netp1))
			||	(n == 2 && (webman_config->netd2 && webman_config->neth2[0] && webman_config->netp2))
#ifdef NET3NET4
			||	(n == 3 && (webman_config->netd3 && webman_config->neth3[0] && webman_config->netp3))
			||	(n == 4 && (webman_config->netd4 && webman_config->neth4[0] && webman_config->netp4))
#endif
			  )
			{
				sprintf(line_entry[idx].path, "0net%i <tr>"
											  "<td><a class=\"d\" href=\"/net%i\">net%i (%s:%i)</a></td>"
											  "<td> <a href=\"/mount.ps3/net%i\">%s</a> &nbsp; </td><td>11-Nov-2006 11:11</td>"
											  "</tr>",  n, n, n,	n == 1 ? webman_config->neth1 :
														n == 2 ? webman_config->neth2 :
#ifdef NET3NET4
														n == 3 ? webman_config->neth3 :
														n == 4 ? webman_config->neth4 :
#endif
														webman_config->neth0,
														n == 1 ? webman_config->netp1 :
														n == 2 ? webman_config->netp2 :
#ifdef NET3NET4
														n == 3 ? webman_config->netp3 :
														n == 4 ? webman_config->netp4 :
#endif
														webman_config->netp0, n, HTML_DIR); idx++;
			}
		}
 #endif
#endif
		if(idx)
		{   // sort html file entries
			u16 n, m;
			for(n=0; n<(idx-1); n++)
				for(m=(n+1); m<idx; m++)
					if(strcasecmp(line_entry[n].path, line_entry[m].path)>0)
					{
						strcpy(swap, line_entry[n].path);
						strcpy(line_entry[n].path, line_entry[m].path);
						strcpy(line_entry[m].path, swap);
					}
		}


		tlen = strlen(buffer);

		for(u16 m = 0; m < idx; m++)
		{
			strcat(buffer + tlen, (line_entry[m].path)+6); tlen += strlen(buffer + tlen);
			if(tlen > BUFFER_SIZE_HTML) break;
		}

		buffer += tlen;

		strcat(buffer, "</table>");

		if(strlen(param) > 4)
		{
			///////////
			unsigned int effective_disctype = 1;

			if(!is_ps3_http)
			{
				bool show_icon = false;
				if(is_net && jb_games)
				{
					char *p = strchr(param + 12, '/'); if(p) p[0] = NULL; sprintf(templn, "%s/PS3_GAME/ICON0.PNG", param); show_icon = true;
				}

				if(!show_icon)
				{
					sprintf(templn, "%s/ICON0.PNG", param); show_icon = file_exists(templn);                    // current folder
					if(!show_icon) sprintf(templn, "%s/ICON2.PNG", param); show_icon = file_exists(templn);     // ps3_extra folder
					if(!show_icon)
					{
						char *p = strchr(param + 18, '/'); if(p) p[0] = NULL;
						sprintf(templn, "%s/PS3_GAME/ICON0.PNG", param); show_icon = file_exists(templn);       // dev_bdvd or jb folder
						if(!show_icon) sprintf(templn, "%s/ICON0.PNG", param); show_icon = file_exists(templn); // game dir
					}
				}

#ifdef COBRA_ONLY
				unsigned int real_disctype, iso_disctype;
#endif

				if(!show_icon && islike(param, "/dev_bdvd"))
				{
#ifdef COBRA_ONLY
					cobra_get_disc_type(&real_disctype, &effective_disctype, &iso_disctype);
					if(iso_disctype == DISC_TYPE_PSX_CD) sprintf(templn, "%s", wm_icons[6]); else
					if(iso_disctype == DISC_TYPE_PS2_DVD || iso_disctype == DISC_TYPE_PS2_CD) sprintf(templn, "%s", wm_icons[7]); else
					if(iso_disctype == DISC_TYPE_DVD) sprintf(templn, "%s", wm_icons[9]); else
#endif
					sprintf(templn, "%s", wm_icons[5]); show_icon = true;
				}

				for(u16 m = idx; m < 8; m++) strcat(buffer, "<BR>");

				if(show_icon || show_icon0)
					{urlenc(swap, templn); sprintf(templn, "<script>icon.src=\"%s\";icon.style.display='block';</script>", swap); strcat(buffer, templn);}
			}
			///////////

#ifdef EMBED_JS
			sprintf(tempstr, // popup menu
							"<div id='mnu' style='position:fixed;width:140px;background:#333;display:none;padding:5px;'>"
							"<a id='m0'>%s</a><a id='m1'>%s</a><a id='m2'>%s</a><hr><a id='m3'>%s<br></a><a href=\"javascript:t=prompt('%s',self.location.pathname);if(t.indexOf('/dev_')==0)self.location='/mkdir.ps3'+t\">%s</a><hr><a id='m4'>%s<br></a><a id='m5'>%s<br></a><a id='m6'>%s</a><hr><a id='m7'>%s<br></a><a id='m8'>%s</a></div>"
							"<script>var s,m;window.addEventListener('contextmenu',function(e){if(s)s.color='#ccc';t=e.target,s=t.style,c=t.className,m=mnu.style,p=t.pathname;if(c=='w'||c=='d'){e.preventDefault();s.color='#fff',b='block',n='none';m.display=b;m.left=(e.clientX+12)+'px';y=e.clientY;w=window.innerHeight;m.top=(((y+220)<w)?(y+12):(w-220))+'px';m0.href='/install.ps3'+p;m0.style.display=(p.indexOf('.pkg')>0)?b:n;m1.href='/mount.ps3'+p;m1.style.display=(p.toLowerCase().indexOf('.iso')>0||c=='d'||p.indexOf('/GAME')>0)?b:n;m2.href=p;m2.text=(c=='w')?'Download':'Open';m3.href='/delete.ps3'+p;m4.href='/cut.ps3'+p;m5.href='/cpy.ps3'+p;m6.href='/paste.ps3'+self.location.pathname;m7.href='javascript:rn(\"'+p+'\")';m7.style.display=(p.substring(0,5)=='/dev_')?b:n;m8.href='/copy.ps3'+p}},false);window.onclick=function(e){if(m)m.display=n;};"

							// F2 = rename/move item pointed with mouse
							"document.addEventListener('keyup',ku,false);"
							"function rn(f){if(f.substring(0,5)=='/dev_'){f=unescape(f);t=prompt('Rename to:',f);if(t&&t!=f)self.location='/rename.ps3'+f+'|'+escape(t)}}"
							"function ku(e){e=e||window.event;if(e.keyCode==113){var a=document.querySelectorAll('a:hover')[0].pathname;rn(a);}}"

						 	"</script>",
							"Install PKG", "Mount", "Open", "Delete", "New Folder", "New Folder", "Cut", "Copy", "Paste", "Rename", "Copy To"); strcat(buffer, tempstr);
#else
			// add fm.js script
			if(file_exists(FM_SCRIPT_JS))
			{
				sprintf(templn, SCRIPT_SRC_FMT, FM_SCRIPT_JS); strcat(buffer, templn);
			}
#endif

			// show last mounted game
			memset(tempstr, 0, _4KB_); memset(templn, 0, _MAX_PATH_LEN);
			if(effective_disctype != DISC_TYPE_NONE && !strcmp(param, "/dev_bdvd") && file_exists(WMTMP "/last_game.txt"))
			{
				int fd = 0;

				if(cellFsOpen(WMTMP "/last_game.txt", CELL_FS_O_RDONLY, &fd, NULL, 0) == CELL_FS_SUCCEEDED)
				{
					cellFsRead(fd, (void *)templn, _MAX_PATH_LEN, NULL);
					cellFsClose(fd);

					if(strlen(templn) > 10) {sprintf(tempstr, "<span style=\"position:absolute;right:8px\"><font size=2>"); add_breadcrumb_trail(tempstr, templn); strcat(tempstr, "</font></span>");}
				}
			}

			///////////
			char *slash = strchr(param + 1, '/');
			if(slash) slash[0] = NULL;

			if(param[1] == 'n')
				sprintf(templn, "<hr>"
								"<b>" HTML_URL ":", param, param);
			else
			{
				uint32_t blockSize;
				uint64_t freeSize;
				cellFsGetFreeSize(param, &blockSize, &freeSize);
				sprintf(templn, "<hr>"
								"<b>" HTML_URL ": %'d %s",
								param, param, (int)((blockSize*freeSize)>>20), STR_MBFREE);
			}

			strcat(buffer, templn);

			// summary
			sprintf(templn, "</b> &nbsp; <font color=\"#707070\">%'i Dir(s) %'d %s %'d %s</font>%s",
							MAX(dirs - 1, 0), (idx-dirs), STR_FILES,
							dir_size<(_1MB_) ? (int)(dir_size>>10):(int)(dir_size>>20),
							dir_size<(_1MB_) ? STR_KILOBYTE:STR_MEGABYTE, tempstr);

			strcat(buffer, templn);
			///////////
		}
		else
			strcat(buffer,  HTML_BLU_SEPARATOR
							"webMAN - Simple Web Server" EDITION "<br>");
	}
	return true;
}
