#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "connection.h"
#include "utils.h"

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

void conn_connect(struct connection* c, const char* server, unsigned int port)
{
   c->socketfd = socket(AF_INET, SOCK_STREAM, 0);	
   c->host = gethostbyname(server);
   c->addr.s_addr=(*((unsigned long*)c->host->h_addr_list[0]));
   c->server.sin_family = AF_INET;
   c->server.sin_port = htons(port);

   int res = inet_pton(AF_INET, inet_ntoa(c->addr), &c->server.sin_addr);
   if(res<0)
   {
      perror("First parameter is not a valid address");
      close(c->socketfd);
      exit(EXIT_FAILURE);
   }
   else if(res==0)
   {
      perror("Second parameter is not an IP-address");
      close(c->socketfd);
      exit(EXIT_FAILURE);
   }
   if(connect(c->socketfd, (struct sockaddr*)&c->server, sizeof(c->server)))
   {
      perror("Couldn't connect to server");
      close(c->socketfd);
      exit(EXIT_FAILURE);
   }
   c->initialized=1;
   printf("Got connection!\n");
   CMD(c, "NICK", "qwermies");
   CMD(c, "USER", "qwermies qwermies * :Name goes here");
}

void conn_read(struct connection* c)
{
   char buf[4096];
   int n = recv(c->socketfd, buf, 4095, 0);
   buf[4096]=0;
   if(n<0) perror("Read");
   printf("%s\n",buf);
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

void PRIVMSG(struct connection* c, const char* to, const char* msg)
{
   int slen=strlen(msg);
   int tolen=strlen(to);
   
   char tmp[slen+tolen+10];
   tmp[0]=0;
   concat(tmp, 4, "PRIVMSG ", to, ": ", msg);

   MSG(c, tmp);
}

