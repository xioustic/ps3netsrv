#define ITEM_CHECKED			" checked=\"checked\""
#define ITEM_SELECTED			" selected=\"selected\""

#define HTML_URL				"<a href=\"%s\">%s</a>"
#define HTML_URL2				"<a href=\"%s%s\">%s</a>"

#define HTML_URL_STYLE			"color:#ccc;text-decoration:none;"

#define HTML_DIR				"&lt;dir&gt;"
#define HTML_BUTTON_FMT			"%s%s\" %s'%s';\">"
#define HTML_BUTTON				" <input type=\"button\" value=\""
#define HTML_ONCLICK			"onclick=\"location.href="
#define HTML_INPUT(n, v, m, s)	"<input name=\"" n "\" type=\"text\" value=\"" v "\" maxlength=\"" m "\" size=\"" s "\">"
#define HTML_PASSW(n, v, m, s)	"<input name=\"" n "\" type=\"password\" value=\"" v "\" maxlength=\"" m "\" size=\"" s "\">"
#define HTML_NUMBER(n, v, min, max)	"<input name=\"" n "\" type=\"number\" value=\"" v "\" min=\"" min "\" max=\"" max "\">"

#define HTML_DISABLED_CHECKBOX	"1\" disabled=\"disabled"

#define HTML_FORM_METHOD_FMT	"%s"
#define HTML_FORM_METHOD		".ps3mapi\" method=\"get\" enctype=\"application/x-www-form-urlencoded\" target=\"_self\">"

#define _BR_					NULL

#define HTML_RESPONSE_FMT		"HTTP/1.1 %i OK\r\n" \
								"X-PS3-Info: [%s]\r\n" \
								"Content-Type: text/html;charset=UTF-8\r\n" \
								"Cache-Control: no-cache\r\n" \
								"Content-Length: %i\r\n\r\n" \
								"%s%s%s"

#define HTML_HEADER				" <!DOCTYPE html>" \
								"<html xmlns=\"http://www.w3.org/1999/xhtml\">" \
								"<meta http-equiv=\"Content-type\" content=\"text/html;charset=UTF-8\">" \
								"<meta http-equiv=\"Cache-Control\" content=\"no-cache\">" \
								"<meta name=\"viewport\" content=\"width=device-width,initial-scale=0.6,maximum-scale=1.0\">"  /* size: 264 */

#define HTML_HEADER_SIZE		264

#define HTTP_RESPONSE_TITLE_LEN	90 /* strlen(HTML_RESPONSE_TITLE + HTML_BODY) = 26 + 64 */

#define HTML_RESPONSE_TITLE		"webMAN " WM_VERSION "<hr><h2>" /* size: 26 */

#define HTML_BODY				"<body bgcolor=\"#101010\" text=\"#c0c0c0\">" \
								"<font face=\"Courier New\">" /* size: 64 */

#define HTML_BODY_END			"</font></body></html>" /* size: 21 */

#define HTML_BODY_END_SIZE		21

#define HTML_BLU_SEPARATOR		"<hr color=\"#0099FF\"/>"
#define HTML_RED_SEPARATOR		"<hr color=\"#FF0000\"/>"

#define SCRIPT_SRC_FMT			"<script src=\"%s\"></script>"
#define HTML_REDIRECT_TO_URL	"<script>setTimeout(function(){self.location=\"%s\"},%i);</script>"
#define HTML_REDIRECT_TO_BACK	"<script>self.location=window.history.back();</script>"
#define HTML_CLOSE_BROWSER		"<script>window.close(this);</script>"
#define HTML_REDIRECT_WAIT		3000

#define HTML_SHOW_LAST_GAME		"<span style=\"position:absolute;right:8px\"><font size=2>"
#define HTML_SHOW_LAST_GAME_END	"</font></span>"

#define open_browser			vshmain_AE35CF2D


#define MAX(a, b)		((a) >= (b) ? (a) : (b))
#define MIN(a, b)		((a) <= (b) ? (a) : (b))
#define ABS(a)			(((a) < 0) ? -(a) : (a))
#define RANGE(a, b, c)	((a) <= (b) ? (b) : (a) >= (c) ? (c) : (a))
#define ISDIGIT(a)		('0' <= (unsigned char)(a) && (unsigned char)(a) <= '9')

static bool gmobile_mode = false;

int extcmp(const char *s1, const char *s2, size_t n);
int extcasecmp(const char *s1, const char *s2, size_t n);
char *strcasestr(const char *s1, const char *s2);

static bool IS(const char *a, const char *b)
{
	while(*a && (*a == *b)) a++,b++;
	return !(*a-*b); // compare two strings. returns true if they are identical
}

static bool _IS(const char *a, const char *b)
{
	return (strcasecmp(a, b) == 0);	// compare two strings. returns true if they are identical (case insensitive)
}

static size_t concat(char *dest, const char *src)
{
	while (*dest) dest++;

	size_t size = 0;

	while ((*dest++ = *src++)) size++;

	return size;
}

static char *to_upper(char *text)
{
	char *upper = text;
	for( ; *text; text++) if(*text >= 'a' && *text <= 'z') *text ^= 0x20;
	return upper;
}

static bool islike(const char *param, const char *text)
{
	while(*text && (*text == *param)) text++, param++;
	return !*text;
}

static char h2a(const char hex)
{
	char c = hex;
	if(c >= 0 && c <= 9)
		c += '0';
	else if(c >= 10 && c <= 15)
		c += 55; //A-F
	return c;
}

static inline void urldec(char *url, char *original)
{
	if(strchr(url, '%'))
	{
		strcpy(original, url); // return original url

		u16 pos = 0; char c;
		for(u16 i = 0; url[i] >= ' '; i++, pos++)
		{
			if(url[i] == '+')
				url[pos] = ' ';
			else if(url[i] != '%')
				url[pos] = url[i];
			else
			{
				url[pos] = 0; u8 n = 2;
				while(n--)
				{
					url[pos] <<= 4, i++, c = (url[i] | 0x20);
					if(c >= '0' && c <= '9') url[pos] += c -'0';      else
					if(c >= 'a' && c <= 'f') url[pos] += c -'a' + 10;
				}
			}
		}
		url[pos] = NULL;
	}
}

static bool urlenc(char *dst, const char *src)
{
	size_t i, j = 0, pos = 0;

	if(islike(src, "http") && (src[4] == ':' || src[5] == ':') && (src[6] == '/') && src[7]) { for(i = 8; src[i]; i++) if(src[i] == '/') {pos = i; break;} }

	for(i = 0; src[i]; i++, j++)
	{
		if(src[i] & 0x80)
		{
			dst[j++] = '%';
			dst[j++] = h2a((unsigned char)src[i]>>4);
			dst[j] = h2a(src[i] & 0xf);
		}
		else if(src[i]=='?' || ((src[i]==':') && (i >= pos)))
		{
			dst[j++] = '%';
			dst[j++] = '3';
			dst[j] = (src[i] & 0xf) + '7'; // A | F
		}
		else if(src[i]==' ' || src[i]=='\'' || src[i]=='"' || src[i]=='%' || src[i]=='&' || src[i]=='+' || (gmobile_mode && src[i] == '\''))
    		{
			dst[j++] = '%';
			dst[j++] = '2';
			dst[j] = (src[i] == '+') ? 'B' : '0' + (src[i] & 0xf);
		}
		else dst[j] = src[i];
	}
	dst[j] = '\0';

	return (j > i); // true if dst != src
}

static size_t htmlenc(char *dst, char *src, u8 cpy2src)
{
	size_t j = 0;
	char tmp[8]; u8 t, c;
	for(size_t i = 0; src[i]; i++)
	{
		if(src[i] & 0x80)
		{
			t = sprintf(tmp, "&#%i;", (int)(unsigned char)src[i]); c = 0;
			while(t--) {dst[j++] = tmp[c++];}
		}
		else dst[j++] = src[i];
	}

	j = MIN(j, MAX_LINE_LEN);
	dst[j] = '\0';

	if(cpy2src) strcpy(src, dst);
	return j;
}

static size_t utf8enc(char *dst, char *src, u8 cpy2src)
{
	size_t j = 0; u16 c;
	for(size_t i = 0; src[i]; i++)
	{
		c = (src[i] & 0xFFFF);

		if(!(c & 0xff80)) dst[j++]=c;
		else if(!(c & 0xf800))
		{
			dst[j++]=0xC0|(c>>6);
			dst[j++]=0x80|(0x3F&c);
		}
		else
		{
			dst[j++]=0xE0|(0x0F&(c>>12));
			dst[j++]=0x80|(0x3F&(c>>06));
			dst[j++]=0x80|(0x3F&(c    ));
		}
	}

	j = MIN(j, MAX_LINE_LEN);
	dst[j] = '\0';

	if(cpy2src) strcpy(src, dst);
	return j;
}
/*
static size_t utf8dec(char *dst, char *src, u8 cpy2src)
{
	size_t j = 0;
	u8 c;
	for(size_t i = 0; src[i] != '\0'; i++, j++)
	{
		c = (src[i]&0xFF);
		if(c < 0x80)
			dst[j] = c;
		else if(c & 0x20)
			dst[j] = (((src[i++] & 0x1F)<<6) + (c & 0x3F));
		else
		{
			dst[j] = ((src[i++] & 0xF)<<12) + ((src[i++] & 0x3F)<<6) + (c & 0x3F);
		}
	}

	j = MIN(j, MAX_LINE_LEN);
	dst[j] = '\0';

	if(cpy2src) strcpy(src, dst);
	return j;
}
*/
static size_t add_radio_button(const char *name, const char *value, const char *id, const char *label, const char *sufix, bool checked, char *buffer)
{
	char templn[MAX_LINE_LEN];
	sprintf(templn, "<label><input type=\"radio\" name=\"%s\" value=\"%s\" id=\"%s\"%s/> %s%s</label>", name, value, id, checked ? ITEM_CHECKED : "", label, (!sufix) ? "<br>" : sufix);
	return concat(buffer, templn);
}

static size_t add_check_box(const char *name, const char *value, const char *label, const char *sufix, bool checked, char *buffer)
{
	char templn[MAX_LINE_LEN], clabel[MAX_LINE_LEN];
	strcpy(clabel, label);
	char *p = strstr(clabel, AUTOBOOT_PATH);
	if(p)
	{
		u8 pos = p - clabel;
		sprintf(p, HTML_INPUT("autop", "%s", "255", "40"), webman_config->autoboot_path);
		strcat(p, label + pos + strlen(AUTOBOOT_PATH));
	}
	sprintf(templn, "<label><input type=\"checkbox\" name=\"%s\" value=\"%s\"%s/> %s</label>%s", name, value, checked ? ITEM_CHECKED : "", clabel, (!sufix) ? "<br>" : sufix);
	return concat(buffer, templn);
}

static size_t add_option_item(const char *value, const char *label, bool selected, char *buffer)
{
	char templn[MAX_LINE_LEN];
	sprintf(templn, "<option value=\"%s\"%s/>%s</option>", value, selected?ITEM_SELECTED:"", label);
	return concat(buffer, templn);
}

static size_t prepare_header(char *buffer, const char *param, u8 is_binary)
{
	bool set_base_path = false;

	size_t slen = sprintf(buffer, "HTTP/1.1 200 OK\r\n"
								  "Content-Type: "); char *header = buffer + slen;

	int flen = strlen(param);

	if(is_binary == BINARY_FILE)
	{
		char *ext = (char*)param + MAX(flen - 4, 0), *ext5 = (char*)param + MAX(flen - 5, 0);

		if(!extcasecmp(ext, ".png", 4))
			strcat(header, "image/png");
		else
		if(!extcasecmp(ext, ".jpg", 4) || !extcasecmp(ext5, ".jpeg", 5) || !extcmp(ext, ".STH", 4))
			strcat(header, "image/jpeg");
		else
		if(!extcasecmp(ext, ".htm", 4) || !extcasecmp(ext5, ".html", 5) || !extcasecmp(ext5, ".shtm", 5))
			{strcat(header, "text/html"); set_base_path = true;}
		else
		if(!extcasecmp(ext, ".js", 3))
			strcat(header, "text/javascript");
		else
		if(!extcasecmp(ext, ".css", 4))
			strcat(header, "text/css");
		else
		if(!extcasecmp(ext, ".txt", 4) || !extcasecmp(ext, ".log", 4) || !extcasecmp(ext, ".ini", 4) || !extcasecmp(ext, ".cfg", 4) || !extcmp(ext, ".HIP", 4) || !extcmp(ext, ".HIS", 4))
			strcat(header, "text/plain");
		else
		if(!extcasecmp(ext, ".svg", 4))
			strcat(header, "image/svg+xml");
#ifndef LITE_EDITION
		else
		if(!extcasecmp(ext, ".gif", 4))
			strcat(header, "image/gif");
		else
		if(!extcasecmp(ext, ".bmp", 4))
			strcat(header, "image/bmp");
		else
		if(!extcasecmp(ext, ".tif", 4))
			strcat(header, "image/tiff");
		else
		if(!extcasecmp(ext, ".avi", 4))
			strcat(header, "video/x-msvideo");
		else
		if(!extcasecmp(ext, ".mp4", 4))
			strcat(header, "video/mp4");
		else
		if(!extcasecmp(ext, ".mkv", 4))
			strcat(header, "video/x-matroska");
		else
		if(!extcasecmp(ext, ".mpg", 4) || !extcasecmp(ext, ".mp2", 4) || strcasestr(ext5, ".mpe"))
			strcat(header, "video/mpeg");
		else
		if(!extcasecmp(ext, ".vob", 4))
			strcat(header, "video/vob");
		else
		if(!extcasecmp(ext, ".wmv", 4))
			strcat(header, "video/x-ms-wmv");
		else
		if(!extcasecmp(ext, ".flv", 4))
			strcat(header, "video/x-flv");
		else
		if(!extcasecmp(ext, ".mov", 4))
			strcat(header, "video/quicktime");
		else
		if(!extcasecmp(ext5, ".webm", 5))
			strcat(header, "video/webm");
		else
		if(!extcasecmp(ext, ".mp3", 4))
			strcat(header, "audio/mpeg");
		else
		if(!extcasecmp(ext, ".wav", 4))
			strcat(header, "audio/x-wav");
		else
		if(!extcasecmp(ext, ".wma", 4))
			strcat(header, "audio/x-ms-wma");
		else
		if(!extcasecmp(ext, ".mid", 4) || !extcasecmp(ext, ".kar", 4))
			strcat(header, "audio/midi");
		else
		if(!extcasecmp(ext, ".mod", 4))
			strcat(header, "audio/mod");
		else
		if(!extcasecmp(ext, ".zip", 4))
			strcat(header, "application/zip");
		else
		if(!extcasecmp(ext, ".pdf", 4))
			strcat(header, "application/pdf");
		else
		if(!extcasecmp(ext, ".doc", 4))
			strcat(header, "application/msword");
		else
		if(!extcasecmp(ext5, ".docx", 5))
			strcat(header, "application/vnd.openxmlformats-officedocument.wordprocessingml.document");
		else
		if(!extcasecmp(ext, ".xls", 4))
			strcat(header, "application/vnd.ms-excel");
		else
		if(!extcasecmp(ext5, ".xlsx", 5))
			strcat(header, "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
		else
		if(!extcasecmp(ext, ".ppt", 4) || !extcasecmp(ext, ".pps", 4))
			strcat(header, "application/vnd.ms-powerpoint");
		else
		if(!extcasecmp(ext, ".swf", 4))
			strcat(header, "application/x-shockwave-flash");
#endif
		else
			strcat(header, "application/octet-stream");
	}
	else
		{strcat(header, "text/html"); set_base_path = true;}

	if(set_base_path && param[0] == '/' && (param[1] == 'n' || param[1] == 'd' || param[1] == 'a')) {strcpy(html_base_path, param); if((param[1] != 'n') && !isDir(param)) flen = strrchr(html_base_path, '/') - html_base_path; html_base_path[flen] = NULL; }

	strcat(header, "\r\n");

	return slen + strlen(header);
}

static int val(const char *c)
{
	int previous_result = 0, result = 0;
	int multiplier = 1;

	if(c && *c == '-')
	{
		multiplier = -1;
		c++;
	}

	while(*c)
	{
		if(!ISDIGIT(*c)) return result * multiplier;

		result *= 10;
		if(result < previous_result)
			return(0); // overflow
		else
			previous_result = result;

		result += (*c - '0');
		if(result < previous_result)
			return(0); // overflow
		else
			previous_result = result;

		c++;
	}
	return(result * multiplier);
}

static u16 get_value(char *text, char *url, u16 size)
{
	u16 n;
	for(n = 0; n < size; n++)
	{
		if(url[n] == '&' || url[n] == 0) break;
		if(url[n] == '+') url[n] = ' ';
		text[n] = url[n];
	}
	text[n] = NULL;
	return n;
}

static u32 get_valuen32(const char *param, const char *label)
{
	char value[12], *pos = strstr(param, label);
	if(pos)
	{
		get_value(value, pos + strlen(label), 11);
		return (u32)val(value);
	}
	return 0;
}

static u16 get_valuen16(const char *param, const char *label)
{
	return RANGE((u16)get_valuen32(param, label), 0, 65535);
}

static u8 get_valuen(const char *param, const char *label, u8 min_value, u8 max_value)
{
	return RANGE((u8)get_valuen32(param, label), min_value, max_value);
}
