/********************************
* Module:       Socket Client   *
* Author:       Josh Chen       *
* Date:         2016/03/23      *
********************************/

#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <stdio.h>
#include "socket_client.h"

#define BLOCKING_TO	0

#if BLOCKING_TO
#include <sys/time.h>
#endif

int socket_client_close(struct socket_client *socket_client)
{
	if (socket_client->socket_fd)
	{
		close(socket_client->socket_fd);
		socket_client->socket_fd = 0;
	}
	return 1;
}

int socket_client_open(struct socket_client *socket_client)
{
	struct sockaddr_un addr;
	int socket_flags = 0;
#if BLOCKING_TO
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
#endif

	socket_client->socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (socket_client->socket_fd < 0)
	{
		printf("socket_client - socket error\n");
		return 0;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, socket_client->socket_path);
	if (connect(socket_client->socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		printf("socket_client - connect error\n");
		socket_client_close(socket_client);
		return 0;
	}

	if (socket_client->blocking_mode == 1)	//blocking mode
	{
#if BLOCKING_TO
		if (setsockopt(socket_client->socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv)) < 0)
		{
			printf("socket_client - setsockopt error\n");
			socket_client_close(socket_client);
			return 0;
		}
#endif
	}
	else	//non-blocking mode
	{
		socket_flags = fcntl(socket_client->socket_fd, F_GETFL, 0);
		if (socket_flags < 0)
		{
			printf("socket_client - fcntl get error\n");
			socket_client_close(socket_client);
			return 0;
		}

		if (fcntl(socket_client->socket_fd, F_SETFL, socket_flags | O_NONBLOCK) < 0)
		{
			printf("socket_client - fcntl set error\n");
			socket_client_close(socket_client);
			return 0;
		}
	}

	return 1;
}

int socket_client_send(struct socket_client *socket_client, char *data_buf, unsigned int data_len)
{
	int send_len = 0;
	if (socket_client->socket_fd == 0)
		return 0;

	send_len = send(socket_client->socket_fd, data_buf, data_len, 0);
	if (send_len != data_len)
	{
		printf("socket_client - send error\n");
		return send_len;
	}
	return send_len;
}

int socket_client_recv(struct socket_client *socket_client, char *data_buf, unsigned int data_len)
{
	int read_len = 0;
	if (socket_client->socket_fd == 0)
		return 0;

	if (socket_client->blocking_mode == 1)	//blocking mode
	{
		if (recv(socket_client->socket_fd, data_buf+read_len, 1, 0) > 0)
		{
			if (data_buf[read_len] != 0)
				read_len++;
		}
	}
	else	//non-blocking mode
	{
		while (recv(socket_client->socket_fd, data_buf+read_len, 1, 0) > 0)
		{
			if (data_buf[read_len] != 0)
				read_len++;

			if (read_len >= (*data_len - 1))
				break;

			usleep(1);
		}
	}

	if (read_len <= 0)
		return 0;

	return read_len;
}
