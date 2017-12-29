/*
 * http.h
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

#ifndef _HTTP_H
#define _HTTP_H

#define HTTP_PORT 80
#define HTTP_TIMEOUT 60
#define SEND_TIMEOUT 60
#define RECV_TIMEOUT 60

#define INVALID_SOCK -1
#define ERR_COUNT 10

int http_connect(int sock, struct sockaddr *address, int addrlen);
void close_socket (int sock);
int http_send (int sock, char *buffer);
int http_recv (int sock, char *buffer, int size);
int http_get(char *server, char *cmd, char **result);

#endif /* HTTP */
