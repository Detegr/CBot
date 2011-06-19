#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "connection.h"
#include "utils.h"

#define BOT_NAME       "qwermies"
#define BOT_REALNAME   "Name goes here"

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
   CMD(c, "NICK", BOT_NAME);
   char name[64]; *name=0;
   concat(name, 5, BOT_NAME, " ", BOT_NAME, " * :", BOT_REALNAME);
   CMD(c, "USER", name);
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

void conn_parsemsgs(struct connection* c, char* msg, void (*func)(struct connection*, char*))
{
   char* cmd = strtok(msg, "\n");
   while(cmd)
   {
      func(c, cmd);
      cmd=strtok(NULL, "\n");
   }
}

void conn_execcmd(struct connection* c, char* msg)
{
   char* p = strstr(msg, " ");
   if(p)
   {
      *p=0;
      CMD(c, msg, ++p);
      //printf("CMD: %s, TOKEN: %s\n", msg, p);
   }
}

int conn_parsecmd(char* in, char* user, char* cmd, char* msg)
{
   if(in[0]!=':') return -1;

   char* start = strstr(in, ":");
   char* end = strstr(++start, ":");
   if(start && end)
   {
      end++[-1]=0; // Null-terminate prefix.
   
      char* token = strtok(start, " ");
      strcpy(user, token);
   
      token = strtok(NULL, " ");
      strcpy(cmd, token);
   
      strcpy(msg, end);

      *in=0;
      return 0;
   }
   else return -1;
}

int conn_pingpong(struct connection* c, char* msg)
{
   char* pos = strstr(msg, "PING");
   if(pos)
   {
      /*
       * TODO: Line that includes PING anywhere will trigger this.
       */
      if(pos[1]=='I') pos[1]='O';
      else
      {
	 printf("Invalid pingpong message.\n");
	 exit(EXIT_FAILURE);
      }
      printf("%s\n", pos);
      MSG(c, pos);
      return 0;
   }
   return -1;
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

