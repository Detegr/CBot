/*
 * config.h
 *
 * Header for config system.
 *
 */

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
 * @return 0 if succeeded.
 * 	  -1 if failed.
 */
int		config_create(struct config* c);

/*
 * config_destroy()
 *
 * Unallocates the config struct.
 * @return 0 if succeeded.
 * 	  -1 if failed.
 */
int		config_destroy(struct config* c);

/*
 * config_getvals()
 *
 * @return char** array with values under 'variable' in config.
 *         NULL if no 'variable' found.
 */
const char** 	config_getvals(struct config* c, const char* variable);

/*
 * config_write()
 *
 * Writes config to a file.
 * @return 0 if succeeded.
 * 	  -1 if failed.
 */
int		config_write(struct config *c, const char* to);

/*
 * config_addmultiple()
 *
 * Adds values from array to config. May modify the array.
 * @param values: Null-terminated array. E.g. {"first", "second", 0}
 *
 * @return 0 if succeeded.
 * 	  -1 if failed.
 */
int		config_addmultiple(struct config* c, const char* variable, char** values);

/*
 * config_add()
 *
 * For adding single value. Simpler to use than config_addvalues()
 * @return 0 if succeeded.
 * 	  -1 if failed.
 */
int		config_add(struct config* c, const char* variable, const char* value);

/*
 * config_delvar()
 *
 * Deletes a variable (and all its values) from config.
 * @return 0 if succeeded
 * 	   1 if no 'variable' found.
 */
int		config_delvar(struct config* c, const char* variable);

/*
 * config_delval()
 * 
 * Deletes a value under variable from config.
 * @return 0 if succeeded
 * 	   1 if no 'value' or 'variable' found.
 */
int		config_delval(struct config* c, const char* variable, const char* value);

/*
 * config_delvals()
 * 
 * Deletes a array of values under variable from config.
 * @see config_addmultiple for the style of array 'values'.
 * @return 0 if succeeded
 * 	   1 if not a single 'value' or 'variable' found.
 */
int		config_delvals(struct config* c, const char* variable, const char** values);
#endif
