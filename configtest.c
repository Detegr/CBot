/*
 * configtest.c
 *
 * Test file and example for config system usage.
 *
 */

#include <stdio.h>
#include "config.h"

int main()
{
	struct config c;
	if(config_create(&c)==0)
	{
		const char** users = config_getvals(&c, "authorized_users");
		for(int i=0; users[i]; ++i) printf("Authorized user #%d: %s\n", i+1, users[i]);
		char* values[4] = {"first", "second", "third", 0};
		config_addmultiple(&c, "testval", values);
		config_add(&c, "config_add_single", "thisvalue");
		config_delvar(&c, "authorized_users");
		config_delval(&c, "testval", "second");
		const char* del[3]={"first", "third", 0};
		config_delvals(&c, "testval", del);
		config_add(&c, "authorized_users", "newval");
		config_write(&c, "config_writetest.conf");
		if(config_destroy(&c)==0) printf("Config destroyed successfully.\n");
	}

	return 0;
}
