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

void conn_create(struct connection* c);
void conn_destroy(struct connection* c);
void conn_connect(struct connection* c, const char* server, unsigned int port);
void conn_read(struct connection* c, char* to);

int conn_pingpong(struct connection* c, char* msg);
void conn_parsemsgs(struct connection* c, char* msg, void (*func)(struct connection*, char*));
void conn_execcmd(struct connection* c, char* msg);
int conn_parsecmd(char* in, char* user, char* cmd, char* msg);

void CMD(struct connection* c, const char* cmd, const char* msg);
void MSG(struct connection* c, const char* msg);
void PRIVMSG(struct connection* c, const char* to, const char* msg);

#endif
