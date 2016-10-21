#define getPort(p1x, p2x) ((p1x * 256) + p2x)

static int ssend(int socket, const char *str)
{
	return send(socket, str, strlen(str), 0);
}

static int connect_to_server_ex(const char *server, uint16_t port, bool rcv_timeout)
{
	struct sockaddr_in sin;
	unsigned int temp;
	int s;

	if((temp = inet_addr(server)) != (unsigned int)-1)
	{
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = temp;
	}
	else
	{
		struct hostent *hp;

		if((hp = gethostbyname(server)) == NULL)
		{
			return FAILED;
		}

		sin.sin_family = hp->h_addrtype;
		memcpy(&sin.sin_addr, hp->h_addr, hp->h_length);
	}

	sin.sin_port = htons(port);
	s = socket(AF_INET, SOCK_STREAM, 0);
	if(s < 0)
	{
		return FAILED;
	}

	struct timeval tv;
	tv.tv_usec = 0;
	tv.tv_sec = 3;
	setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

	if(rcv_timeout)
		setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	if(connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		return FAILED;
	}

	tv.tv_sec = 60;
	//setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

	return s;
}

static int connect_to_server(const char *server, uint16_t port)
{
	return connect_to_server_ex(server, port, false);
}

static int slisten(int port, int backlog)
{
	int list_s = socket(AF_INET, SOCK_STREAM, 0);
	if(list_s < 0) return list_s;

	//int optval = 1;
	//setsockopt(list_s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	struct sockaddr_in sa;
	socklen_t sin_len = sizeof(sa);
	memset(&sa, 0, sin_len);

	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(list_s, (struct sockaddr *)&sa, sin_len);
	listen(list_s, backlog);

	return list_s;
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
