/*
 * config.h
 *
 * Header for config system.
 *
 */

#ifndef CONFIG_H
#define CONFIG_H

typedef struct config
{
	int		entries;
	char**		variables;
	char***		values;
} conf_t;

/*
 * config_create()
 *
 * Creates and allocates a new config struct.
 * @return 0 if succeeded.
 * 	  -1 if failed.
 */
int		config_create(conf_t* c);

/*
 * config_destroy()
 *
 * Unallocates the config struct.
 * @return 0 if succeeded.
 * 	  -1 if failed.
 */
int		config_destroy(conf_t* c);

/*
 * config_getvals()
 *
 * @return char** array with values under 'variable' in config.
 *         NULL if no 'variable' found.
 */
const char** 	config_getvals(conf_t* c, const char* variable);

/*
 * config_write()
 *
 * Writes config to a file.
 * @return 0 if succeeded.
 * 	  -1 if failed.
 */
int		config_write(conf_t *c, const char* to);

/*
 * config_addmultiple()
 *
 * Adds values from array to config. May modify the array.
 * @param values: Null-terminated array. E.g. {"first", "second", 0}
 *
 * @return 0 if succeeded.
 * 	  -1 if failed.
 */
int		config_addmultiple(conf_t* c, const char* variable, char** values);

/*
 * config_add()
 *
 * For adding single value. Simpler to use than config_addvalues()
 * @return 0 if succeeded.
 * 	  -1 if failed.
 */
int		config_add(conf_t* c, const char* variable, const char* value);

/*
 * config_delvar()
 *
 * Deletes a variable (and all its values) from config.
 * @return 0 if succeeded
 * 	   1 if no 'variable' found.
 */
int		config_delvar(conf_t* c, const char* variable);

/*
 * config_delval()
 * 
 * Deletes a value under variable from config.
 * @return 0 if succeeded
 * 	   1 if no 'value' or 'variable' found.
 */
int		config_delval(conf_t* c, const char* variable, const char* value);

/*
 * config_delvals()
 * 
 * Deletes a array of values under variable from config.
 * @see config_addmultiple for the style of array 'values'.
 * @return 0 if succeeded
 * 	   1 if not a single 'value' or 'variable' found.
 */
int		config_delvals(conf_t* c, const char* variable, const char** values);
#endif
