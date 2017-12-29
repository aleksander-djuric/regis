/*
 * http.c
 *
 * Description: Simple SSL based registration system (RegiS).
 *              HTTP chat functions.
 *
 * Copyright (c) 2017 Aleksander Djuric. All rights reserved.
 * Distributed under the GNU Lesser General Public License (LGPL).
 * The complete text of the license can be found in the LICENSE
 * file included in the distribution.
 *
 */

#include "regcli.h"
#include "http.h"

#define BUFSIZE 8192

int http_connect(int sock, struct sockaddr *address, int addrlen)
{
	int noblock;
	int ret;

	if (sock < 0)
		return -1;

	noblock = 1;
	if (ioctl(sock, FIONBIO, &noblock) < 0)
		return -1;

	if (connect(sock, address, addrlen) < 0) {
		fd_set rfds, wfds;
		struct timeval tv;
		int optval;
		socklen_t optlen = sizeof(int);

		if (errno != EAGAIN && errno != EINPROGRESS)
			return -1;

		FD_ZERO(&rfds);
		FD_SET(sock, &rfds);
		wfds = rfds;

		tv.tv_sec = HTTP_TIMEOUT;
		tv.tv_usec = 0;

		do {
			ret = select(sock + 1, &rfds, &wfds, NULL, &tv);
		} while (ret < 0 && errno == EINTR);
		if (ret <= 0)
			return -1;

		if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
			return -1;
	}

	return 0;
}

void close_socket(int sock)
{
	if (sock != INVALID_SOCK) {
		int noblock = 0;
		ioctl(sock, FIONBIO, &noblock);

		shutdown(sock, SHUT_RDWR);

		while ((close(sock))
			&& (errno == EAGAIN || errno == EINPROGRESS
				|| errno == EALREADY))
			usleep(999999);
		sock = INVALID_SOCK;
	}
}

int http_send(int sock, char *buffer)
{
	struct timeval tv;
	fd_set wfds;
	int ret;
	int counter = ERR_COUNT;

	if (sock < 0)
		return -1;

	tv.tv_sec = SEND_TIMEOUT;
	tv.tv_usec = 0;

	FD_ZERO(&wfds);
	FD_SET(sock, &wfds);

	do {
		ret = select(sock + 1, NULL, &wfds, NULL, &tv);
	} while (ret < 0 && errno == EINTR);
	if (ret <= 0 || !FD_ISSET(sock, &wfds))
		return -1;

	do {
		ret = send(sock, buffer, strlen(buffer), MSG_NOSIGNAL);
		if (ret < 0)
			usleep(999999);
	} while (ret < 0 && errno == EAGAIN && --counter);

	if (ret <= 0)
		return -1;

	do {
		ret = select(sock + 1, NULL, &wfds, NULL, &tv);
	} while (ret < 0 && errno == EINTR);

	if (ret <= 0)
		return -1;

	return ret;
}

int http_recv(int sock, char *buffer, int size)
{
	fd_set rfds;
	struct timeval tv;
	int ret;
	int counter = ERR_COUNT;

	if (sock < 0)
		return -1;

	tv.tv_sec = RECV_TIMEOUT;
	tv.tv_usec = 0;

	FD_ZERO(&rfds);
	FD_SET(sock, &rfds);
	buffer[0] = '\0';

	do {
		ret = select(sock + 1, &rfds, NULL, NULL, &tv);
	} while (ret < 0 && errno == EINTR);
	if (ret <= 0 || !FD_ISSET(sock, &rfds))
		return -1;

	do {
		ret = recv(sock, buffer, size - 1, MSG_NOSIGNAL);
		if (ret < 0) usleep(999999);
	} while (ret < 0 && errno == EAGAIN && --counter);

	if (ret <= 0)
		return -1;
	else
		buffer[ret] = '\0';

	return ret;
}

int http_get(char *server, char *cmd, char **buffer)
{
	struct sockaddr_in address;
	struct hostent *h;
	int sock = INVALID_SOCK;
	int optval = 1;
	int ret;

	*buffer = (char *) malloc(BUFSIZE);
	if (!*buffer) {
		printf("Couldn't allocate memory: %s\n", strerror(errno));
		return -1;
	}

	h = gethostbyname(server);
	if (h == NULL) {
		printf("Unable to resolve host name: %s\n", strerror(errno));
		return -1;
	}

	memset(&address, 0, sizeof(struct sockaddr));
	address.sin_addr.s_addr =
		(in_addr_t) * ((unsigned long *) h->h_addr_list[0]);
	address.sin_family = AF_INET;
	address.sin_port = htons(HTTP_PORT);

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		printf("Unable to open socket: %s\n", strerror(errno));
		return -1;
	}
	setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE | SO_REUSEADDR,
		&optval, sizeof(int));

	if (http_connect(sock, (struct sockaddr *) &address,
		 sizeof(struct sockaddr_in)) < 0) {
		printf("Could not connect to %s\n", strerror(errno));
		close_socket(sock);
		return -1;
	}

	snprintf(*buffer, BUFSIZE - 1,
		"GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
		cmd, server);
	if (http_send(sock, *buffer) < 0)
		return -1;
	ret = http_recv(sock, *buffer, BUFSIZE - 1);

	if (ret) {
		char *body;

		if ((body = strstr(*buffer, "\r\n\r\n"))) {
			body += 4;
			ret -= (body - *buffer);
			memmove(*buffer, body, ret);
		}
	}

	close_socket(sock);

	return ret;
}

/* eof */
