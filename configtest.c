#include <stdio.h>
#include "config.h"

int main()
{
	struct config c;
	if(config_create(&c)==0)
	{
		const char** users = config_getvalues(&c, "authorized_users");
		for(int i=0; users[i]; ++i) printf("Authorized user #%d: %s\n", i+1, users[i]);
		const char* values[3] = {"first", "second", 0};
		config_add(&c, "testval", values);
		config_write(&c, "config_writetest.conf");
		if(config_destroy(&c)==0) printf("Config destroyed successfully.\n");
	}
}
