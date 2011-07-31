#include "bot.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

extern int globalkill;

int bot_create(struct bot* b)
{
	b->conn = (conn_t*)malloc(sizeof(conn_t));
	b->conf = (conf_t*)malloc(sizeof(conf_t));
	conn_create(b->conn);
	config_create(b->conf);
	return 0;
}

int bot_connect(struct bot* b, const char* server, unsigned int port)
{
	if(conn_connect(b->conn, server, port)==0)
	{
		const char** nick = config_getvals(b->conf, "nick");
		const char** realname = config_getvals(b->conf, "realname");

		CMD(b->conn, "NICK", NULL, *nick);
		char name[64]; *name=0;
		concat(name, 5, *nick, " ", *nick, " * :", *realname);
		CMD(b->conn, "USER", NULL, name);

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
	printf("Dying...\n");
	CMD(b->conn, "PRIVMSG", NULL, "EXIT CBOT is dying.");
	globalkill=1;
	return 0;
}

int bot_destroy(struct bot* b)
{
	config_write(b->conf, "config.conf");
	conn_destroy(b->conn);
	config_destroy(b->conf);
	free(b->conn);
	free(b->conf);
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
		CMD(b->conn, msg, NULL, ++p);
		//printf("CMD: %s, TOKEN: %s\n", msg, p);
		return 0;	
	}
	return -1;
}

int bot_parsecmd(char* in, char* host, char* nick, char* cmd, char* channel, char* msg)
{
	if(in[0]!=':') return -1;

	// Input strings are in following form:
	// :user!~indent@host COMMAND NICK/CHANNEL :message
	char* data_end=in+1; // Skip the first ':'
	while(*data_end!=':') ++data_end;
	strcpy(msg, data_end+1); // The message data.
	
	// Parse other data with pointers limit and in.
	char* limit=in+1;
	
	while(*limit!=' ' && limit!=data_end) ++limit; *limit=0; ++limit;
	strcpy(host, in+1);
	in=limit;

	char* tmp = host;
	char* hostlen=host+strlen(host);
	while(*tmp!='!' && tmp<hostlen) ++tmp;
	if(tmp<hostlen) { strncpy(nick, host, tmp-host); nick[tmp-host]=0; } // The nick part
	
	while(*limit!=' ' && limit!=data_end) ++limit; *limit=0; ++limit;
	strcpy(cmd, in);
	in=limit;

	while(*limit!=' ' && limit!=data_end) ++limit; *limit=0; ++limit;
	strcpy(channel, in);
	in=limit;

	return 0;
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
		char host[256];
		char nick[64];
		char cmd[256];
		char channel[64];
		char message[512];

		if(bot_parsecmd(msg, host, nick, cmd, channel, message)==0)
		{
			printf("HOST: %s\nNICK: %s\nCMD: %s\nCHANNEL: %s\nMSG: %s\n", host, nick, cmd, channel, message);
			if(strcmp(cmd, "PRIVMSG")==0)
			{
				int allowedtoexec=0;

				const char** authedusers = config_getvals(b->conf, "authorized_users");
				if(!*authedusers[0] && strcmp(message, "iamyourfather")==0)
				{
					#ifdef DEBUG
						printf("Checking authed users.\n");
					#endif
					config_add(b->conf, "authorized_users", host);
					CMD(b->conn, "PRIVMSG", nick, "Admin user added.");
					allowedtoexec=1;
				}

				for(int i=0; authedusers[i]; ++i)
				{
					if(strcmp(host, authedusers[i])==0)
					{
						allowedtoexec=1;
						break;
					}
				}

				#ifdef DEBUG
					printf("Allowed to exec: %d\n", allowedtoexec);
				#endif
				if(allowedtoexec)
				{
					if(is_cbotcommand(message)==0)
					{
						char* ccmd = message+6;

						if(strcmp(ccmd, "DIE")==0)
						{
							bot_disconnect(b);
							return;
						}
						else if(strncmp(ccmd, "join", 4)==0)
						{
							CMD(b->conn, "JOIN", NULL, ccmd+4);
						}
						else CMD(b->conn, cmd, NULL, ccmd);
					}
				}
				else CMD(b->conn, "PRIVMSG", nick, "Access denied!");
				//bot_execcmd(b, message);
			}
		}
	}
}

int is_cbotcommand(const char* msg)
{
	return strncmp(msg, "!CBOT", 5);
}

int bot_work(struct bot* b)
{
		char buf[4096];
		conn_read(b->conn, buf);
		char* tmp = buf;
		char* msg = buf;
		while(1)
		{
			while(*tmp!='\n')
			{
				++tmp;
				if(!(*tmp)) break;
			}
			if(*tmp)
			{
				*tmp=0;
				bot_parsemsg(b, msg);
				msg=++tmp;
			}
			else
			{
				bot_parsemsg(b, msg);
				break;
			}
		}
		if(globalkill) return 1;
		return 0;
}
