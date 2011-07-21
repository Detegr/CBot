#include <stdio.h>
#include "config.h"

int main()
{
	struct config c;
	if(config_create(&c)==0)
	{
		const char** users = config_getvalues(&c, "authorized_users");
		for(int i=0; users[i]; ++i) printf("Authorized user #%d: %s\n", i+1, users[i]);
		char* values[3] = {"second", "third", 0};
		config_addvalues(&c, "testval", values);
		config_add(&c, "config_add_single", "thisvalue");
		config_delvar(&c, "authorized_users");
		config_write(&c, "config_writetest.conf");
		if(config_destroy(&c)==0) printf("Config destroyed successfully.\n");
	}
}
