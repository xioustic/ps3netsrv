#define SC_FS_DISK_FREE		840

#define ICON_STYLE			" style=\"position:fixed;top:118px;right:10px;max-height:176px;z-index:-1\" onerror=\"this.style.display='none';\""

u32 _LINELEN = LINELEN;
u32 _MAX_PATH_LEN = MAX_PATH_LEN;
u32 _MAX_LINE_LEN = MAX_LINE_LEN;

static void add_list_entry(char *tempstr, bool is_dir, char *ename, char *templn, char *name, char *fsize, CellRtcDateTime rDate, u16 flen, unsigned long long sz, char *sf, u8 is_net, u8 show_icon0, u8 is_ps3_http)
{
	unsigned long long sbytes = sz;

	if(sz < 10240)			{sprintf(sf, "%s", STR_BYTE);} else
	if(sz < 0x200000ULL)	{sprintf(sf, "%s", STR_KILOBYTE); sz >>= 10;} else
	if(sz < 0xC00000000ULL) {sprintf(sf, "%s", STR_MEGABYTE); sz >>= 20;} else
							{sprintf(sf, "%s", STR_GIGABYTE); sz >>= 30;}

	urlenc(tempstr, templn, 1); strncpy(templn, tempstr, _MAX_LINE_LEN);

	{strcpy(tempstr, name); htmlenc(name, tempstr, 0);}

	flen = strlen(name); char *ext = name + MAX(flen - 4, 0);
	u8 show_img = !is_ps3_http && (!is_dir && (!strcasecmp(ext, ".png") || !strcasecmp(ext, ".jpg") || !strcasecmp(ext, ".bmp")));

	if(is_dir)
	{
		bool show_play = ((flen == 8) && !strcmp(name, "dev_bdvd") && View_Find("game_plugin") == 0);

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
		else if(flen == 9 && islike(templn, HDD0_GAME_DIR))
			sprintf(fsize, "<a href=\"/fixgame.ps3%s\">%s</a>", templn, HTML_DIR);
#endif
#ifdef COPY_PS3
		else if(!is_net && ( (flen == 5 && (!strcasecmp(name, "VIDEO") || strcasestr(name, "music"))) || (flen == 6 && !strcasecmp(name, "covers")) || !strcmp(name, "savedata") || !strcmp(name, "exdata") || !strcmp(name, "home") ))
			sprintf(fsize, "<a href=\"/copy.ps3%s\" title=\"copy to %s\">%s</a>", templn, islike(templn, "/dev_hdd0") ? "/dev_usb000" : "/dev_hdd0", HTML_DIR);
#endif
		else
		if(strlen(templn) <= 11 && islike(templn, "/dev_"))
		{
			uint64_t freeSize, devSize;
			system_call_3(SC_FS_DISK_FREE, (uint64_t)templn, (uint64_t)&devSize, (uint64_t)&freeSize);

			sprintf(fsize, "<a href=\"/mount.ps3%s\" title=\"%'llu %s (%'llu %s)\">%'8llu %s</a>", templn, (unsigned long long)((devSize)>>20), STR_MEGABYTE, devSize, STR_BYTE, (unsigned long long)((freeSize)>>20), STR_MEGABYTE);
		}
		else
#ifdef PS2_DISC
			sprintf(fsize, "<a href=\"/mount%s%s\">%s</a>", strstr(name, "[PS2")?".ps2":".ps3", templn, HTML_DIR);
#else
			sprintf(fsize, "<a href=\"/mount.ps3%s\">%s</a>", templn, HTML_DIR);
#endif
	}


#ifdef COBRA_ONLY
	else if( (flen>4 && strcasestr(ISO_EXTENSIONS, ext)!=NULL && !islike(templn, HDD0_GAME_DIR)) || (!is_net && ( strstr(name + MAX(flen - 13, 0), ".ntfs[") || !extcmp(name + MAX(flen - 8, 0), ".BIN.ENC", 8) )) )
	{
		if( strcasestr(name, ".iso.") && extcasecmp(name, ".iso.0", 6) )
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
	else if( (sz <= MAX_TEXT_LEN) && (strcasestr(".txt|.ini|.log|.sfx|.xml|.cfg|.css|.html|.conf|name", ext)!=NULL || strstr(templn, "wm_custom")!=NULL ) )
			sprintf(fsize, "<a href=\"/edit.ps3%s\">%'llu %s</a>", templn, sz, sf);
	else
		sprintf(fsize, "<label title=\"%'llu %s\"> %'llu %s</label>", sbytes, STR_BYTE, sz, sf);

	snprintf(ename, 6, "%s    ", name); if(!strstr(templn, ":")) urlenc(templn, tempstr, 1);

	sprintf(tempstr, "%c%c%c%c%c%c<tr>"
                     "<td><a %s href=\"%s\"%s>%s</a></td>",
	is_dir ? '0' : '1', ename[0], ename[1], ename[2], ename[3], ename[4],
	is_dir ? "class=\"d\"" : "class=\"w\"", templn, show_img ? " onmouseover=\"s(this,0);\"" : (is_dir && show_icon0) ? " onmouseover=\"s(this,1);\"":"", name);

	flen=strlen(tempstr);
	if(flen >= _LINELEN)
	{
		if(is_dir) sprintf(fsize, HTML_DIR); else sprintf(fsize, "%llu %s", sz, sf);

		sprintf(tempstr, "%c%c%c%c%c%c<tr>"
                         "<td><a %s href=\"%s\">%s</a></td>",
		is_dir ? '0' : '1', ename[0], ename[1], ename[2], ename[3], ename[4],
		is_dir ? "class=\"d\"" : "class=\"w\"", templn, name);

		flen=strlen(tempstr);
		if(flen >= _LINELEN)
		{
			if(is_dir) sprintf(fsize, HTML_DIR); else sprintf(fsize, "%llu %s", sz, sf);

			sprintf(tempstr, "%c%c%c%c%c%c<tr>"
                             "<td>%s</td>",
			is_dir ? '0' : '1', ename[0], ename[1], ename[2], ename[3], ename[4],
			name);
		}
	}

	sprintf(templn, "<td> %s &nbsp; </td>"
					"<td>%02i-%s-%04i %02i:%02i</td></tr>",
					fsize,
					rDate.day, smonth[rDate.month-1], rDate.year, rDate.hour, rDate.minute);
	strcat(tempstr, templn);

	flen=strlen(tempstr);
	if(flen >= _LINELEN) {flen=0; tempstr[0] = NULL;} //ignore file if it is still too long
}

static void add_breadcrumb_trail(char *buffer, char *param)
{
	u32 tlen = 0;

	char swap[_MAX_PATH_LEN], templn[_MAX_PATH_LEN], url[_MAX_PATH_LEN];

	strcpy(templn, param);
	while(strchr(templn+1, '/'))
    {
		templn[strchr(templn+1, '/')-templn] = NULL;
		tlen+=strlen(templn)+1;

		strcpy(swap, param);
		swap[tlen] = NULL;

		strcat(buffer, "<a class=\"f\" href=\"");
		urlenc(url, swap, 0);
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

		urlenc(url, param, 0); htmlenc(label, templn, 0);
		sprintf(swap, "<a href=\"%s%s\">%s</a>",
#ifdef FIX_GAME
						islike(param, HDD0_GAME_DIR) ? "/fixgame.ps3" :
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
		if(strstr(param, "?1")) enable_dev_blind(NULL);
		if(strstr(param, "?0")) {system_call_3(SC_FS_UMOUNT, (u64)(char*)"/dev_blind", 0, 1);}

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
		char ename[8];
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

		u8 jb_games = (!extcmp(param, "/GAMES", 6) || !extcmp(param, "/GAMEZ", 6));
		u8 show_icon0 = false;

		if(!is_ps3_http)
		{
			show_icon0 = jb_games || ((strlen(param) >= 14) && (islike(param, "/dev_hdd0/game") || islike(param, "/dev_hdd0/home/")));
			sprintf(templn, "<img id=\"icon\"%s>"
							"<script>"
							"function s(o,d){icon.style.display='block';icon.src=o.href.replace('/delete.ps3','').replace('/cut.ps3','').replace('/cpy.ps3','')+((d)?'%s/ICON0.PNG':'');}"
							"</script>", ICON_STYLE, jb_games ? "/PS3_GAME" : ""); strcat(buffer, templn);
		}

		strcat(buffer, "<table class=\"propfont\"><tr><td>");

		// breadcrumb trail //
		add_breadcrumb_trail(buffer, param);

		if((param[7] == 'v' || param[7] == 'm') && View_Find("game_plugin") == 0 && (isDir("/dev_bdvd/PS3_GAME") || file_exists("/dev_bdvd/SYSTEM.CNF") || isDir("/dev_bdvd/BDMV") || isDir("/dev_bdvd/VIDEO_TS") || isDir("/dev_bdvd/AVCHD")))
			strcat(buffer, ":</td><td width=90><a href=\"/play.ps3\">&lt;Play>&nbsp;</a>");
		else
			strcat(buffer, ":</td><td width=90>&nbsp;");

		strcat(buffer, "</td><td></td></tr>");

		tlen = 0;

 #ifdef COBRA_ONLY
  #ifndef LITE_EDITION
		if(is_net)
		{
			int ns = FAILED, abort_connection = 0;

			if(param[4]  >=  '0' && param[4] <= '4') ns = connect_to_remote_server((param[4]  & 0xFF) - '0');

			if(ns  >=  0)
			{
				strcat(param, "/");
				if(open_remote_dir(ns, param+5, &abort_connection)  >=  0)
				{
					strcpy(templn, param); if(templn[strlen(templn)-1] == '/') templn[strlen(templn)-1] = NULL;
					if(strrchr(templn, '/')) templn[strrchr(templn, '/')-templn] = NULL; if(strlen(templn)<6 && strlen(param)<8) {templn[0]='/'; templn[1] = NULL;}

					urlenc(swap, templn, 0);
					sprintf(tempstr, "!00000<tr>"
									 "<td><a class=\"f\" href=\"%s\">..</a></td>"
									 "<td> " HTML_URL " &nbsp; </td>"
									 "<td>11-Nov-2006 11:11</td>"
									 "</tr>", swap, swap, HTML_DIR);

					if(strlen(tempstr) > _MAX_LINE_LEN) return false; //ignore lines too long
					strncpy(line_entry[idx].path, tempstr, _LINELEN); idx++; dirs++;
					tlen+=strlen(tempstr);

					sys_addr_t data2 = 0;
					netiso_read_dir_result_data *data = NULL;
					int v3_entries = 0;
					v3_entries = read_remote_dir(ns, &data2, &abort_connection);
					if(data2 != NULL)
					{
						data = (netiso_read_dir_result_data*)data2;

						for(int n = 0; n < v3_entries; n++)
						{
							if(data[n].name[0] == '.' && data[n].name[1] == 0) continue;
							if(tlen > BUFFER_SIZE_HTML) break;
							if(idx >= (max_entries-3)) break;

							if(param[1] == 0)
								sprintf(templn, "/%s", data[n].name);
							else
							{
								sprintf(templn, "%s%s", param, data[n].name);
							}
							flen=strlen(templn)-1; if(templn[flen] == '/') templn[flen] = NULL;

							cellRtcSetTime_t(&rDate, data[n].mtime);

							sz=(unsigned long long)data[n].file_size; dir_size+=sz;

							is_dir=data[n].is_directory; if(is_dir) dirs++;

							add_list_entry(tempstr, is_dir, ename, templn, data[n].name, fsize, rDate, flen, sz, sf, true, show_icon0, is_ps3_http);

							if(strlen(tempstr) > _MAX_LINE_LEN) continue; //ignore lines too long
							strncpy(line_entry[idx].path, tempstr, _LINELEN); idx++;
							tlen+=strlen(tempstr);

							if(!working) break;
						}
						sys_memory_free(data2);
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
									if((uint32_t)bytes_read < _64KB_ || boff  >=  file_size) break;
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

				if(strlen(tempstr) > _MAX_LINE_LEN) continue; //ignore lines too long
				strncpy(line_entry[idx].path, tempstr, _LINELEN); idx++;
				tlen+=flen;

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
				sprintf(tempstr, "0net%i <tr>"
										"<td><a class=\"d\" href=\"/net%i\">net%i (%s:%i)</a></td>"
										"<td> <a href=\"/mount.ps3/net%i\">%s</a> &nbsp; </td><td>11-Nov-2006 11:11</td>"
										"</tr>", n, n, n,	n == 1 ? webman_config->neth1 :
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
															webman_config->netp0, n, HTML_DIR);
				strncpy(line_entry[idx].path, tempstr, _LINELEN); idx++;
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

		for(u16 m=0;m<idx;m++)
		{
			strcat(buffer, (line_entry[m].path)+6); tlen += strlen(buffer + tlen);
			if(tlen > BUFFER_SIZE_HTML) break;
		}

		//if(sysmem_html) sys_memory_free(sysmem_html);
		strcat(buffer + tlen, "</table>");

		if(strlen(param) > 4)
		{
			///////////
			unsigned int effective_disctype = 1;

			if(!is_ps3_http)
			{
				bool show_icon = false;
				if(is_net && (strstr(param, "/GAMES/") || strstr(param, "/GAMEZ/")))
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

				for(u16 m=idx;m<7;m++) strcat(buffer + tlen, "<BR>");

				if(show_icon || show_icon0)
					{urlenc(swap, templn, 0); sprintf(templn, "<script>icon.src=\"%s\"</script>", swap); strcat(buffer + tlen, templn);}
			}
			///////////

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
			if(strchr(param+1, '/'))
				param[strchr(param+1, '/')-param] = NULL;

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
			strcat(buffer + tlen, templn);

			sprintf(templn, "</b> &nbsp; <font color=\"#707070\">%'i Dir(s) %'d %s %'d %s</font>%s",
							MAX(dirs - 1, 0), (idx-dirs), STR_FILES,
							dir_size<(_1MB_)?(int)(dir_size>>10):(int)(dir_size>>20),
							dir_size<(_1MB_)?STR_KILOBYTE:STR_MEGABYTE, tempstr);
			strcat(buffer + tlen, templn);
			///////////
		}
		else
			strcat(buffer + tlen,  HTML_BLU_SEPARATOR
							"webMAN - Simple Web Server" EDITION "<br>");
	}
	return true;
}
