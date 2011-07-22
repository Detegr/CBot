#include "bot.h"
#include "utils.h"
#include <stdio.h>

int bot_create(struct bot* b, struct config* cfg, struct connection* c)
{
	b->conf = cfg;
	b->conn = c;

	// TODO: Check config?

	return 0;
}

int bot_connect(struct bot* b, const char* server, unsigned int port)
{
	if(conn_connect(b->conn, server, port)==0)
	{
		const char** nick = config_getvals(b->conf, "nick");
		const char** realname = config_getvals(b->conf, "realname");

		CMD(b->conn, "NICK", *nick);
		char name[64]; *name=0;
		concat(name, 5, *nick, " ", *nick, " * :", *realname);
		CMD(b->conn, "USER", name);

		return 0;
	}
	else
	{
		fprintf(stderr, "Couldn't connect to %s:%d\n", server, port);
		return -1;
	}
}

int bot_disconnect(struct bot* b)
{
	return -1;
}

int bot_destroy(struct bot* b)
{
	config_write(b->conf, "config.conf");
	conn_destroy(b->conn);
	config_destroy(b->conf);
	b->conn=NULL;
	b->conf=NULL;
	if(!b->conn && !b->conf) return 0;
	else return -1;
}

void bot_parsemsgs(struct bot* b, char* msg, void (*func)(struct bot*, char*))
{
	char* cmd = strtok(msg, "\n");
	while(cmd)
	{
		func(b, cmd);
		cmd=strtok(NULL, "\n");
	}
}

int bot_execcmd(struct bot* b, char* msg)
{
	char* p = strstr(msg, " ");
	if(p)
	{
		*p=0;
		CMD(b->conn, msg, ++p);
		//printf("CMD: %s, TOKEN: %s\n", msg, p);
		return 0;	
	}
	return -1;
}

int bot_parsecmd(char* in, char* user, char* cmd, char* msg)
{
	/* TODO: Maybe get rid of strstr:s? */
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

int bot_pingpong(struct bot* b, char* msg)
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
			fprintf(stderr, "BOT: Invalid pingpong message.\n");
			return -1;
		}
		printf("%s\n", pos);
		MSG(b->conn, pos);
		return 0;
	}
	return -1;
}

void bot_parsemsg(struct bot* b, char* msg)
{
	printf("S> %s <E\n", msg);
	if(bot_pingpong(b, msg)<0)
	{
		/*
		 * TODO: Adjust sizes to something less stupid.
		 */
		char user[256];
		char cmd[256];
		char message[256];

		if(bot_parsecmd(msg, user, cmd, message)==0)
		{
			printf("USER: %s\nCMD: %s\nMSG: %s\n", user, cmd, message);
			if(strcmp(cmd, "PRIVMSG")==0) bot_execcmd(b, message);
		}
	}
}

int bot_work(struct bot* b)
{
		char buf[4096];
		conn_read(b->conn, buf);
		char* msg = strtok(buf, "\n");
		while(msg)
		{
			bot_parsemsg(b, msg);
			msg=strtok(NULL, "\n");
		}
		return 0;
}
