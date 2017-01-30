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
		return FAILED;
	}

	if(connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		return FAILED;
	}

	return s;
}
