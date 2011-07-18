#include "config.h"
#include <stdio.h>
#include <stdlib.h>

int config_create(struct config* c, unsigned int entries)
{
}

int config_createdefault()
{
	FILE* fp = fopen("config.conf", "w");
	if(!fp) return -1;

	fputs("nick=CBOT_DEFAULT\nrealname=CBOT_DEFAULT\nauthorized_users=\n", fp);
	fclose(fp);
	return 0;
}

int config_load(struct config* c, const char* file)
{
	FILE* fp = fopen("config.conf", "r");
	if(!fp)
	{
		fprintf("CONFIG: File not found, creating...");
		config_createdefault();
		printf("DONE!\n");
		fp = fopen("config.conf", "r");
	}

	unsigned int entrynum=0;
	unsigned int bytes_handled=0;
	if(fp)
	{
		char* buf;
		long filesize;
		fseek(fp, 0, SEEK_END);
		filesize = ftell(fp);
		rewind(fp);

		buf=(char*)malloc(filesize*sizeof(char));
		if(!buf)
		{
			fprintf(stderr,"CONFIG: Allocation failed.");
			fclose(fp);
			return -1;
		}
		int readbytes = fread(buf, 1, filesize, fp);
		if(readbytes!=filesize)
		{
			fprintf(stderr,"CONFIG: Reading failed.");
			fclose(fp);
			free(buf);
			return -1;
		}
		int equs=0;
		int lines=0;
		for(int i=0; buf[i]; ++i)
		{
			if(buf[i]=="=") ++equs;
			else if(buf[i]=="\n") ++lines;
		}
		if(lines!=equs)
		{
			fprintf(stderr, "CONFIG: Different number of lines and assigns.");
			free(buf);
			fclose(fp);
			return -1;
		}
		c->entries = lines;
	
		c->variables = (char**)malloc(lines*sizeof(char*));
		c->values = (char**)malloc(lines*sizeof(char*));

		if(c->variables && c->values)
		{
			char* eol;
			char* separator;

			while(bytes_handled<bytesread)
			{
				eol=buf;
				while(*eol!="\n") ++eol;
				separator=buf;
				while(*separator!="=" && separator!=eol) ++variable;
				if(separator==eol)
				{
					fprintf(stderr, "CONFIG: File invalid. Must separate variables and values with '='");
					free(buf);
					fclose(fp);
					exit(EXIT_FAILURE);
				}

				c->variables[entrynum] = (char*)malloc(separator-buf);
				c->values[entrynum] = (char*)malloc(eol-separator);
				for(int i=0,char* tmp=buf; tmp!=separator; ++tmp,++i) tmp[i]=c->variables[entrynum][i];
				for(int i=0,char* tmp=separator; tmp!=eol; ++tmp,++i) tmp[i]=c->values[entrynum][i];
				bytes_handled+=(eol-buf);
				buf=eol+1;
				++entrynum;
			}
		}
		else
		{
			fprintf(stderr, "CONFIG: Allocation failed.");
			free(buf);
			fclose(fp);
			return -1;
		}
	}
}

int config_destroy(struct config* c)
{
	if(c->loaded)
	{
		for(int i=0;i<entries;++i)
		{
			free(variables[i]);
			free(values[i]);
		}
	}
	free(variables);
	free(values);
	if(!variables && !values) return 0;
	else return -1;
}
