#ifndef BOT_H
#define BOT_H

#include "config.h"
#include "connection.h"

struct bot
{
	struct config*		conf;
	struct connection* 	conn;
};

int	bot_create(struct bot* b, struct config* cfg, struct connection* c);
int	bot_connect(struct bot* b, const char* server, unsigned int port);
int	bot_disconnect(struct bot* b);
int	bot_destroy(struct bot* b);

int 	bot_pingpong(struct bot* b, char* msg);
int 	bot_execcmd(struct bot* b, char* msg);
int 	bot_parsecmd(char* in, char* user, char* cmd, char* msg);

void	bot_parsemsg(struct bot* b, char* msg);

int	bot_work(struct bot* b);

#endif