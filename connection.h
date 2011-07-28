#ifndef CONNECTION_H
#define CONNECTION_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "string.h"

struct connection
{	
	int socketfd;
	struct hostent* host;
	struct in_addr addr;
	struct sockaddr_in server;
	int initialized;
};

void 	conn_create(struct connection* c);
void 	conn_destroy(struct connection* c);
int 	conn_connect(struct connection* c, const char* server, unsigned int port);
void 	conn_read(struct connection* c, char* to);

void 	CMD(struct connection* c, const char* cmd, const char* channel, const char* msg);
void 	MSG(struct connection* c, const char* msg);

#endif
