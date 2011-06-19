#include "connection.h"
#include <stdio.h>

void parsefunc(struct connection* c, char* msg)
{
   printf("S> %s <E\n", msg);
   conn_pingpong(c, msg);
}

int main()
{
   struct connection c;

   conn_create(&c);
   conn_connect(&c, "irc.fi.quakenet.org", 6667);
   while(1)
   {
      char buf[4096];
      conn_read(&c, buf);
      conn_parsemsgs(&c, buf, parsefunc);
   } 
   conn_destroy(&c);
}
