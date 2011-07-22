#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "connection.h"
#include "utils.h"
#include "config.h"

extern struct config conf;

void conn_create(struct connection* c)
{
	c->socketfd=0;
	c->host=NULL;
	c->initialized=0;
}

void conn_destroy(struct connection* c)
{
	if(c->initialized)
	{
		shutdown(c->socketfd, SHUT_RDWR);
		close(c->socketfd);
	}
}

int conn_connect(struct connection* c, const char* server, unsigned int port)
{
	c->socketfd = socket(AF_INET, SOCK_STREAM, 0);	
	c->host = gethostbyname(server);
	c->addr.s_addr=(*((unsigned long*)c->host->h_addr_list[0]));
	c->server.sin_family = AF_INET;
	c->server.sin_port = htons(port);

	int res = inet_pton(AF_INET, inet_ntoa(c->addr), &c->server.sin_addr);
	if(res<0)
	{
		fprintf(stderr, "CONNECTION: %s", strerror(errno));
		close(c->socketfd);
		return -1;
	}
	else if(res==0)
	{
		fprintf(stderr, "CONNECTION: %s", strerror(errno));
		close(c->socketfd);
		return -1;
	}
	if(connect(c->socketfd, (struct sockaddr*)&c->server, sizeof(c->server)))
	{
		fprintf(stderr, "CONNECTION: %s", strerror(errno));
		close(c->socketfd);
		return -1;
	}
	c->initialized=1;
	printf("Got connection!\n");
	
	return 0;
}

void conn_read(struct connection* c, char* to)
{
	char buf[4096];
	memset(buf, 0, sizeof(buf)); // Clear buffer just in case.
	int n = recv(c->socketfd, buf, sizeof(buf), 0);
	if(n<0) perror("Read");

	// Get rid of \r\n line endings.
	for(unsigned int i=0; i<sizeof(buf); ++i) if(buf[i]=='\r' && buf[i+1]=='\n') buf[i]='\n';
	strcpy(to, buf);
}

void CMD(struct connection* c, const char* cmd, const char* msg)
{
	int slen=strlen(msg);
	int cmdlen=strlen(cmd);

	char tmp[cmdlen+slen+1];
	tmp[0]=0;
	concat(tmp, 3, cmd, " ", msg);

	MSG(c, tmp);
}

void MSG(struct connection* c, const char* msg)
{
	int len = strlen(msg);
	char tmp[len+2];
	tmp[0]=0;
	concat(tmp, 2, msg, "\r\n");
	send(c->socketfd, tmp, strlen(tmp), 0);
}
