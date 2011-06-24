#ifndef UTILS_H
#define UTILS_H

struct config
{
   char** authorized_users;
   char* bot_name;
   char* bot_realname;
};

void config_create();
void config_get(struct config* config);
void config_adduser(struct config* config, const char* user);
int config_writeuser(const char* user);
void config_destroy(struct config* config);
void concat(char* to, int n, ...);

#endif
