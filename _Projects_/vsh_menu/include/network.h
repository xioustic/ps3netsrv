#include <arpa/inet.h>
#include <sys/socket.h>

struct timeval {
	int64_t tv_sec;			/* seconds */
	int64_t tv_usec;		/* and microseconds */
};

static int connect_to_webman(void)
{
	struct sockaddr_in sin;
	int s;

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0x7F000001; //127.0.0.1 (localhost)
	sin.sin_port = htons(80);         //http port (80)
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0)
	{
		return -1;
	}

	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		return -1;
	}

	return s;
}

static void sclose(int *socket_e)
{
	if(*socket_e != -1)
	{
		shutdown(*socket_e, SHUT_RDWR);
		socketclose(*socket_e);
		*socket_e = -1;
	}
}

static void send_wm_request(const char *cmd)
{
	// send command
	int conn_s = -1;
	conn_s = connect_to_webman();

	struct timeval tv;
	tv.tv_usec = 0;
	tv.tv_sec = 3;
	setsockopt(conn_s, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	setsockopt(conn_s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	if(conn_s >= 0)
	{
		char wm_cmd[1048];
		int cmd_len = sprintf(wm_cmd, "GET %s HTTP/1.0\r\n", cmd);
		send(conn_s, wm_cmd, cmd_len, 0);
		sclose(&conn_s);
	}
}

