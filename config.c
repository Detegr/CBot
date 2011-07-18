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
		unsigned int bytes_read = fread(buf, 1, filesize, fp);
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
	
		c->variables = (char**)calloc(lines, sizeof(char*));
		c->values = (char***)calloc(lines, sizeof(char**));

		if(c->variables && c->values)
		{
			int numof_values;
			char* eol;
			char* separator;

			while(bytes_handled<bytes_read)
			{
				numof_values=1;
				eol=buf;
				while(*eol!='\n') ++eol;
				separator=buf;
				while(*separator!='=' && separator!=eol) ++separator;
				if(separator==eol)
				{
					fprintf(stderr, "CONFIG: File invalid. Must separate variables and values with '='\n");
					for(unsigned int i=0; i<entrynum; ++i)
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

				c->variables[entrynum] = (char*)calloc(separator-buf+1, sizeof(char)); // +1 for null-byte.
				char* tmp = buf;
				for(int i=0; tmp!=separator; ++tmp,++i) c->variables[entrynum][i]=*tmp;
				tmp = separator+1;
				for(int i=0; tmp!=eol; ++tmp,++i)
				{
					if(*tmp==',')
					{
						while(*tmp==',') ++tmp;
						++numof_values;
					}
				}
				c->values[entrynum] = (char**)calloc(numof_values+1, sizeof(char*)); // +1 for null array. (For easy iteration)
				tmp=separator+1;

				int values_done=0;
				while(values_done != numof_values)
				{
					while(*tmp==' ') ++tmp;
					char* tmp2=tmp;
					while(*tmp2!=',' && *tmp2!='\n') ++tmp2;
					c->values[entrynum][values_done] = (char*)calloc(tmp2-tmp+1, sizeof(char)); // +1 for null-byte.
					for(int i=0; tmp!=tmp2; ++i, ++tmp) c->values[entrynum][values_done][i] = *tmp;
					++values_done;
					tmp=tmp2+1;
				}
				c->values[entrynum][values_done] = NULL;
				bytes_handled+=eol-buf+1;
				buf=eol+1;
				++entrynum;
			}
			free(buf_fallback);
			fclose(fp);
			return 0;
		}
		else
		{
			fprintf(stderr, "CONFIG: Allocation failed.\n");
			free(buf_fallback);
			fclose(fp);
			return -1;
		}
	}
	else
	{
		fprintf(stderr, "CONFIG: Fatal error\n");
		return -1;
	}
}

int config_createdefault()
{
	FILE* fp = fopen("config.conf", "w");
	if(!fp) return -1;

	fputs("nick=CBOT_DEFAULT\nrealname=CBOT_DEFAULT\nauthorized_users=DUMMYUSER1,DUMMYUSER2,DUMMYUSER3\n", fp);
	fclose(fp);
	return 0;
}

int config_write(struct config* c, const char* to)
{
	FILE* fp = fopen(to, "w");
	if(fp)
	{
		for(int i=0; i<c->entries; ++i)
		{
			fprintf(fp, "%s=", c->variables[i]);
			for(int j=0; c->values[i][j]; ++j) j>0 ? fprintf(fp, ", %s", c->values[i][j]) : fprintf(fp, "%s", c->values[i][j]);
			fprintf(fp, "\n");
		}
		return 0;
	}
	else
	{
		fprintf(stderr, "CONFIG: Cannot open %s to write.\n", to);
		return -1;
	}
}

const char** config_getvalues(struct config* c, const char* variable)
{
	// Assuming that c is initialized.
	for(int i=0; i<c->entries; ++i)
	{
		if(strcmp(variable, c->variables[i])==0)
		{
			return (const char**)c->values[i];
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

		int j=0;
		for(;c->values[i][j]; ++j)
		{
			free(c->values[i][j]);
			c->values[i][j]=NULL;
		}
		free(c->values[i][j]);
		c->values[i][j]=NULL;

		free(c->values[i]);
		c->values[i]=NULL;
	}
	free(c->variables);
	c->variables=NULL;
	free(c->values);
	c->values=NULL;
	c->entries=0;
	if(!c->variables && !c->values) return 0;
	else return -1;
}
