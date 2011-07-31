#include "connection.h"
#include "utils.h"
#include "config.h"
#include "bot.h"

#include <stdlib.h>
#include <stdio.h>

int globalkill=0;

int main()
{
	struct bot b;

	if(bot_create(&b)==0)
	{
		if(bot_connect(&b, "irc.quakenet.org", 6667)==0)
		{
			while(bot_work(&b)==0);	
		}
	}
	
	if(bot_destroy(&b)==0)
	{
		printf("Exited successfully.\n");
		exit(EXIT_SUCCESS);
	}
	else
	{
		printf("Failed to exit CBOT.");
		exit(EXIT_FAILURE);
	}
}
