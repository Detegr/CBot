#ifndef CONFIG_H
#define CONFIG_H

struct config
{
	int		entries;
	char**		variables;
	char***		values;
};

int		config_create(struct config* c);
int		config_createdefault();
int		config_destroy(struct config* c);
const char** 	config_getvalues(struct config* c, const char* variable);

#endif
