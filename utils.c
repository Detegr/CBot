#include "utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void config_create()
{
   FILE* fp = fopen("config.conf", "w");
   fputs("[authorized_users]\n[nick]\ndefault\n[realname]\ndefault\n", fp);
   fclose(fp);
}

void config_get(struct config* conf)
{
   FILE* fp = fopen("config.conf", "r");
   if(!fp)
   {
      printf("Config file not found, creating one.\n");
      config_create();
      fp = fopen("config.conf", "r");
   }
   if(fp)
   {
      char* buf;
      long filesize;
      fseek(fp, 0, SEEK_END);
      filesize = ftell(fp);
      rewind(fp);

      unsigned int usercount=0;
      
      buf=(char*)malloc(filesize*sizeof(char));
      if(!buf) exit(EXIT_FAILURE);
      int readbytes = fread(buf, 1, filesize, fp);
      if(readbytes!=filesize) exit(EXIT_FAILURE);

      char* token = strtok(buf, "\n");
      while(token)
      {
	 if(strcmp(token, "[authorized_users]")==0)
	 {
	    token=strtok(NULL, "\n");
	    while(strcmp(token, "[nick]")!=0) // Will cause crash if config file is invalid.
	    {
	       printf("AUTHORIZED USER: %s\n", token);
	       token = strtok(NULL, "\n");
	    }
	 }
	 if(strcmp(token, "[nick]")==0)
	 {
	    token=strtok(NULL, "\n");
	    printf("Bot nick: %s\n", token);
	 }
	 if(strcmp(token, "[realname]")==0)
	 {
	    token=strtok(NULL, "\n");
	    printf("Bot realname: %s\n", token);
	 }
	 token=strtok(NULL, "\n");
      }

      fclose(fp);
      free(buf);
   }
   else exit(EXIT_FAILURE);
}

void config_adduser(struct config* conf, const char* user)
{
   
}

int config_writeuser(const char* user)
{
   FILE* fp = fopen("config.conf", "r+b");
   if(fp)
   {
      char* buf;
      long filesize;
      fseek(fp, 0, SEEK_END);
      filesize = ftell(fp);
      rewind(fp);

      buf=(char*)malloc(filesize*sizeof(char));
      if(!buf) exit(EXIT_FAILURE);
      int readbytes = fread(buf, 1, filesize, fp);
      if(readbytes!=filesize) exit(EXIT_FAILURE);

      char* token = strtok(buf, "\n");
      long pos=0;
      
      if(strcmp(token, "[authorized_users]")==0)
      {
	 fseek(fp, strlen(token), SEEK_SET);
	 token=strtok(NULL, "\n");
	 while(strcmp(token, "[nick]")!=0) // Will cause crash if config file is invalid.
	 {
	    if(strcmp(token, user)==0)
	    {
		 printf("User already in config file.\n");
		 break;
	    }
	    token = strtok(NULL, "\n");
	 }
      }
      else
      {
	 fprintf(stderr, "Invalid config file.\n");
	 fclose(fp);
	 free(buf);
      }
      fseek(fp, strlen("[authorized_users]")+1, SEEK_SET);
      if(fwrite(user, strlen(user), 1, fp) && fwrite("\n", 2, 1, fp)) printf("Wrote %s to config.\n", user);
      else perror("Error writing to config:");
      fclose(fp);
      free(buf);

      return 0;
   }
   else return -1;
}

void concat(char* to, int n, ...)
{
   va_list vl;
   va_start(vl, n);
   for(int i=0; i<n; ++i) strcat(to, va_arg(vl, const char*));
   va_end(vl);
}
