#include <stdio.h>
#include <string.h>
#include <lv2/sysfs.h>
#include <sys/file.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/process.h>

#include <arpa/inet.h>

#define ssend(socket, str) send(socket, str, strlen(str), 0)

static int connect_to_webman()
{
	struct sockaddr_in sin;
	int s;

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0x7F000001;	//127.0.0.1 (localhost)
	sin.sin_port = htons(80);			//http port (80)
	s = socket(AF_INET, SOCK_STREAM, 0);
	if(s < 0)
	{
		return -1;
	}

	if(connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		return -1;
	}

	return s;
}

static char h2a(char hex)
{
	char c = hex;
	if(c>=0 && c<=9)
		c += '0';
	else if(c>=10 && c<=15)
		c += 0x57; //a-f
	return c;
}

static void urlenc(char *dst, char *src)
{
	size_t j=0;
	size_t n=strlen(src);
	for(size_t i=0; i<n; i++,j++)
	{
		     if(src[i]==' ') {dst[j++] = '%'; dst[j++] = '2'; dst[j] = '0';}
		else if(src[i] & 0x80)
		{
			dst[j++] = '%';
			dst[j++] = h2a((unsigned char)src[i]>>4);
			dst[j] = h2a(src[i] & 0xf);
		}
		else if(src[i]==34) {dst[j++] = '%'; dst[j++] = '2'; dst[j] = '2';}
		else if(src[i]=='"') {dst[j++] = '%'; dst[j++] = '2'; dst[j] = '2';}
		else if(src[i]=='%') {dst[j++] = '%'; dst[j++] = '2'; dst[j] = '5';}
		else if(src[i]=='&') {dst[j++] = '%'; dst[j++] = '2'; dst[j] = '6';}
		else if(src[i]=='+') {dst[j++] = '%'; dst[j++] = '2'; dst[j] = 'B';}
		else if(src[i]=='?') {dst[j++] = '%'; dst[j++] = '3'; dst[j] = 'F';}
		else dst[j] = src[i];
	}
	dst[j] = '\0';
}

int main(int argc, const char* argv[])
{
	int n;

	char path[2048] = "";
	char url[2048] = "GET /refresh.ps3 HTTP/1.0\r\n";

	if(argc > 0 && argv)
	{
		if(strncmp(argv[0], "/dev_hdd0/game/", 15)) return 0;

		strcpy(path, argv[0]);

		n = 15; while(path[n] != '/' && path[n] != 0) n++;

		if(path[n] == '/') path[n] = 0;

		strcat(path, "/USRDIR/wm_request\0");
	}

	FILE *fp;

	fp = fopen(path, "rb");

	if (fp)
	{
		memset(path, 2048, 0);

		fread((void *) path, 1, 2048, fp);
		fclose(fp);

		if(path[0])
		{
			if(!strstr(path, "%")) {urlenc(url, path); strcpy(path, url);}

			char *p;
			p = strstr(path, "\r"); if(p) p[0] = 0;
			p = strstr(path, "\n"); if(p) p[0] = 0;

			if(!strncmp(path, "/dev_", 5) || !strncmp(path, "/net", 4))
				sprintf(url, "GET /mount_ps3%s\r\n", path);
			else
			if(path[0]=='/')
				sprintf(url, "GET %s HTTP/1.0\r\n", path);
			else
			if(!strncmp(path, "GET ", 4))
				sprintf(url, "%s HTTP/1.0\r\n", path);
		}
	}

	if(!strncmp(url, "GET /mount", 10))
	{
		fp = fopen("/dev_hdd0/tmp/wm_request", "wb");
		if (fp)
		{
			fwrite((void *) url, 1, strlen(url), fp);
			fclose(fp);
			return 0;
		}
	}

	int s=-1;
	s=connect_to_webman();
	if(s>=0) ssend(s, url);

	return 0;
}
