#include "connection.h"
#include "utils.h"
#include "config.h"
#include <stdio.h>

void parsefunc(struct connection* c, char* msg)
{
	printf("S> %s <E\n", msg);
	if(conn_pingpong(c, msg)<0)
	{
		/*
		 * TODO: Adjust sizes to something less stupid.
		 */
		char user[256];
		char cmd[256];
		char message[256];

		if(conn_parsecmd(msg, user, cmd, message)==0)
		{
			printf("USER: %s\nCMD: %s\nMSG: %s\n", user, cmd, message);
			if(strcmp(cmd, "PRIVMSG")==0) conn_execcmd(c, message);
		}
	}
}


struct config conf;
int main()
{
	struct connection c;
	config_create(&conf);

	conn_create(&c);
	conn_connect(&c, "irc.fi.quakenet.org", 6667);

	while(1)
	{
		char buf[4096];
		conn_read(&c, buf);
		conn_parsemsgs(&c, buf, parsefunc);
	} 
	conn_destroy(&c);	

	config_write(&conf, "config.conf");
	config_destroy(&conf);
}
