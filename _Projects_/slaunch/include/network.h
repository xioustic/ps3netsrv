#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

struct timeval {
	int64_t tv_sec;			/* seconds */
	int64_t tv_usec;		/* and microseconds */
};


#define NONE   -1

static inline int connect_to_webman(void)
{
	struct sockaddr_in sin;
	int s;

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0x7F000001; //127.0.0.1 (localhost)
	sin.sin_port = htons(80);         //http port (80)
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0)
	{
		return NONE;
	}

	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		return NONE;
	}

	return s;
}

static inline void sclose(int32_t *s)
{
	if(*s != NONE)
	{
		shutdown(*s, SHUT_RDWR);
		socketclose(*s);
		*s = NONE;
	}
}

static int send_wm_request(const char *cmd)
{
	// send command
	int conn_s = NONE;
	conn_s = connect_to_webman();

	struct timeval tv;
	tv.tv_usec = 0;
	tv.tv_sec = 10;
	setsockopt(conn_s, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
//	setsockopt(conn_s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	if(conn_s >= 0)
	{
		int pa=0;
		char proxy_action[512];
		proxy_action[pa++] = 'G';
		proxy_action[pa++] = 'E';
		proxy_action[pa++] = 'T';
		proxy_action[pa++] = ' ';

		for(uint16_t i=0;(i < strlen(cmd)) && (pa < 500); i++)
		{
			if(cmd[i] != 0x20)
				proxy_action[pa++] = cmd[i];
			else
			{
				proxy_action[pa++] = '%';
				proxy_action[pa++] = '2';
				proxy_action[pa++] = '0';
			}
		}

		proxy_action[pa++] = '\r';
		proxy_action[pa++] = '\n';
		proxy_action[pa] = 0;
		send(conn_s, proxy_action, pa, 0);
		sclose(&conn_s);
	}
	return conn_s;
}
