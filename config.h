#ifndef CONFIG_H
#define CONFIG_H

struct config
{
	int		entries;
	char**		variables;
	char***		values;
};

/*
 * config_create()
 *
 * Creates and allocates a new config struct.
 */
int		config_create(struct config* c);

/*
 * config_destroy()
 *
 * Unallocates the config struct.
 */
int		config_destroy(struct config* c);

/*
 * config_getvalues()
 *
 * @return char** array with values under 'variable' in config.
 *
 */
const char** 	config_getvalues(struct config* c, const char* variable);

/*
 * config_write()
 *
 * Writes config to a file.
 *
 */
int		config_write(struct config *c, const char* to);

/*
 * config_addvalues()
 *
 * Adds values from array to config. May modify the array.
 * @param values: Null-terminated array. E.g. {"first", "second", 0}
 *
 */
int		config_addvalues(struct config* c, const char* variable, char** values);

/*
 * config_add()
 *
 * For adding single value. Simpler to use than config_addvalues()
 *
 */
int		config_add(struct config* c, const char* variable, const char* value);

/*
 * config_delvar()
 *
 * Deletes a variable (and all its values) from config.
 *
 */
int		config_delvar(struct config* c, const char* variable);

/*
 * config_delval()
 * 
 * Deletes a value under variable from config.
 *
 */
int		config_delval(struct config* c, const char* variable, const char* value);

#endif
