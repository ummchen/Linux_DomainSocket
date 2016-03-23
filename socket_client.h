#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

struct socket_client {
	int	socket_fd;
	char	*socket_path;
	unsigned char	blocking_mode;	//blocking = 1, Non-blocking = 0
};

int socket_client_close(struct socket_client *socket_client);
int socket_client_open(struct socket_client *socket_client);
int socket_client_send(struct socket_client *socket_client, char *data_buf, unsigned int data_len);	//return send data number
int socket_client_recv(struct socket_client *socket_client, char *data_buf, unsigned int data_len);	//return recv data number

#ifdef __cplusplus
}
#endif

#endif
