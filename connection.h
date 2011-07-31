#ifndef CONNECTION_H
#define CONNECTION_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "string.h"

typedef struct connection
{	
	int socketfd;
	struct hostent* host;
	struct in_addr addr;
	struct sockaddr_in server;
	int initialized;
} conn_t;

void 	conn_create(conn_t* c);
void 	conn_destroy(conn_t* c);
int 	conn_connect(conn_t* c, const char* server, unsigned int port);
void 	conn_read(conn_t* c, char* to);

void 	CMD(conn_t* c, const char* cmd, const char* channel, const char* msg);
void 	MSG(conn_t* c, const char* msg);

#endif
