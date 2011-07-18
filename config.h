#ifndef CONFIG_H
#define CONFIG_H

struct config
{
	int		entries;
	char**		variables;
	char**		values;
};

int config_create(struct config* c);
int config_load(struct config* c, const char* file);
int config_destroy(struct config* c);

#endif
