#ifndef BOT_H
#define BOT_H

#include "config.h"
#include "connection.h"

struct bot
{
	conf_t*	conf;
	conn_t*	conn;
};

int	bot_create(struct bot* b);
int	bot_connect(struct bot* b, const char* server, unsigned int port);
int	bot_disconnect(struct bot* b);
int	bot_destroy(struct bot* b);

int 	bot_pingpong(struct bot* b, char* msg);
int 	bot_execcmd(struct bot* b, char* msg);
int 	bot_parsecmd(char* in, char* host, char* nick, char* cmd, char* channel, char* msg);

void	bot_parsemsg(struct bot* b, char* msg);
int	is_cbotcommand(const char* msg);
int	is_publiccommand(const char* msg);

int	bot_work(struct bot* b);

#endif
