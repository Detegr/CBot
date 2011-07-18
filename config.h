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
int		config_write(struct config *c, const char* to);

/*
 * config_add()
 *
 * @param values: Values-array needs to be null-terminated. {"first", "second", 0}
 */
int		config_add(struct config* c, const char* variable, const char** values);

#endif
