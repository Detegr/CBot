#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int config_create(struct config* c)
{
	FILE* fp = fopen("config.conf", "r");
	if(!fp)
	{
		printf("CONFIG: File not found, creating...");
		config_createdefault();
		printf("DONE!\n");
		fp = fopen("config.conf", "r");
	}

	unsigned int entrynum=0;
	unsigned int bytes_handled=0;
	if(fp)
	{
		char* buf;
		char* buf_fallback;
		long filesize;
		fseek(fp, 0, SEEK_END);
		filesize = ftell(fp);
		rewind(fp);

		buf=(char*)malloc(filesize*sizeof(char));
		buf_fallback=buf;
		if(!buf)
		{
			fprintf(stderr,"CONFIG: Allocation failed.\n");
			fclose(fp);
			return -1;
		}
		int bytes_read = fread(buf, 1, filesize, fp);
		if(bytes_read!=filesize)
		{
			fprintf(stderr,"CONFIG: Reading failed.\n");
			fclose(fp);
			free(buf_fallback);
			return -1;
		}
		int equs=0;
		int lines=0;
		for(int i=0; buf[i]; ++i)
		{
			if(buf[i]=='=') ++equs;
			else if(buf[i]=='\n') ++lines;
		}
		if(lines!=equs)
		{
			fprintf(stderr, "CONFIG: Different number of lines and assigns.\n");
			fclose(fp);
			free(buf_fallback);
			return -1;
		}
		c->entries = lines;
	
		c->variables = (char**)malloc(lines*sizeof(char*));
		c->values = (char**)malloc(lines*sizeof(char*));

		if(c->variables && c->values)
		{
			char* eol;
			char* separator;

			while(bytes_handled<bytes_read)
			{
				eol=buf;
				while(*eol!='\n') ++eol;
				separator=buf;
				while(*separator!='=' && separator!=eol) ++separator;
				if(separator==eol)
				{
					fprintf(stderr, "CONFIG: File invalid. Must separate variables and values with '='\n");
					for(int i=0; i<entrynum; ++i)
					{
						free(c->variables[i]);
						free(c->values[i]);
					}
					free(c->variables);
					free(c->values);
					fclose(fp);
					free(buf_fallback);
					return -1;
				}

				c->variables[entrynum] = (char*)malloc(separator-buf);
				c->values[entrynum] = (char*)malloc(eol-separator);
				char* tmp = buf;
				for(int i=0; tmp!=separator; ++tmp,++i) c->variables[entrynum][i]=*tmp;
				tmp = separator+1;
				for(int i=0; tmp!=eol; ++tmp,++i) c->values[entrynum][i]=*tmp;
				bytes_handled+=eol-buf+1;
				buf=eol+1;
				++entrynum;
			}
		}
		else
		{
			fprintf(stderr, "CONFIG: Allocation failed.\n");
			free(buf_fallback);
			fclose(fp);
			return -1;
		}
	}
}

int config_createdefault()
{
	FILE* fp = fopen("config.conf", "w");
	if(!fp) return -1;

	fputs("nick=CBOT_DEFAULT\nrealname=CBOT_DEFAULT\nauthorized_users=\n", fp);
	fclose(fp);
	return 0;
}

const char* config_getvalue(struct config* c, const char* variable)
{
	// Assuming that c is initialized.
	for(int i=0; i<c->entries; ++i)
	{
		if(strcmp(variable, c->variables[i])==0)
		{
			return c->values[i];
		}
	}
	return NULL;
}

int config_destroy(struct config* c)
{
	for(int i=0;i<c->entries;++i)
	{
		free(c->variables[i]);
		c->variables[i]=NULL;
		free(c->values[i]);
		c->values[i]=NULL;
	}
	free(c->variables);
	c->variables=NULL;
	free(c->values);
	c->values=NULL;
	if(!c->variables && !c->values) return 0;
	else return -1;
}
