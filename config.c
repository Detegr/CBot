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
			if(c->variables[i] && c->values[i])
			{
				fprintf(fp, "%s=", c->variables[i]);
				for(int j=0; c->values[i][j]; ++j)
				{
					#ifdef DEBUG
						printf("DEBUG: Wrote %s under %s.\n", c->values[i][j], c->variables[i]);
					#endif
					j>0 ? fprintf(fp, ", %s", c->values[i][j]) : fprintf(fp, "%s", c->values[i][j]);
				}
				fprintf(fp, "\n");
			}
		}
		fclose(fp);
		return 0;
	}
	else
	{
		fprintf(stderr, "CONFIG: Cannot open %s to write.\n", to);
		return -1;
	}
}

int config_add(struct config* c, const char* variable, const char* value)
{
	const char* values[2]={value, 0};
	return config_addvalues(c, variable, (char**)values);
}

int config_addvalues(struct config* c, const char* variable, char** values)
{
	// Check if variable exists.
	int index=-1;
	for(int i=0; i<c->entries; ++i)
	{
		if(c->variables[i] && strcmp(variable, c->variables[i])==0)
		{
			index = i;
		}
	}

	if(index!=-1)
	{
		int values_size=0;
		for(int j=0; c->values[index][j]; ++j)
		{
			++values_size;
			for(int z=0; values[z]; ++z)
			{
				if(strcmp(c->values[index][j], values[z])==0)
				{
					#ifdef DEBUG
						fprintf(stderr, "DEBUG: Value %s already exists under %s\n", values[z], variable);
					#endif
					for(int i=z; values[i]; ++i) values[i] = values[i+1];
				}
			}
		}
		// Conflicts resolved, allocate more space and copy the previous data.
		char** new_val = (char**)calloc((values_size*2)+1, sizeof(char*)); // +1 because of the null-array.
		int i=0;
		for(;c->values[index][i]; ++i)
		{
			#ifdef DEBUG
				printf("DEBUG: Copied %s from slot %d to new config, slot %d\n", c->values[index][i], i, i);
			#endif
			new_val[i] = c->values[index][i];
		}
		for(int j=0;values[j]; ++i, ++j)
		{
			#ifdef DEBUG
				printf("DEBUG: Copied %s from slot %d to new config, slot %d\n", values[j], j, i);
			#endif
			new_val[i] = (char*)calloc(strlen(values[j])+1, sizeof(char));
			strcpy(new_val[i], values[j]);
		}
		new_val[i] = NULL;
		
		free(c->values[index]);
		c->values[index]=new_val;

		return 0;
	}

	// No existing values, add new to the config.
	for(int i=0; i<c->entries; ++i)
	{
		if(!c->variables[i])
		{
			c->variables[i] = (char*)calloc(strlen(variable)+1, sizeof(char));
			for(int j=0; variable[j]; ++j) c->variables[i][j]=variable[j];

			// Assuming that no hand made modifications has been done to c->values.
			int values_size=0;
			while(values[values_size]) ++values_size;

			c->values[i] = (char**)calloc(values_size+1, sizeof(char*));
			for(int j=0; j<values_size; ++j)
			{
				c->values[i][j] = calloc(strlen(values[j]), sizeof(char));
				for(int z=0; values[j][z]; ++z) c->values[i][j][z] = values[j][z];
			}
			c->values[i][values_size]=NULL;
			return 0;
		}
	}
	
	// No space for new pair. Must reallocate the whole thing.
	char** new_var = calloc(c->entries*2, sizeof(char*));
	for(int i=0; i<c->entries; ++i) new_var[i] = c->variables[i];
	free(c->variables);
	c->variables = new_var;

	char*** new_val = calloc(c->entries*2, sizeof(char**));
	for(int i=0; i<c->entries; ++i) new_val[i] = c->values[i];
	free(c->values);
	c->values = new_val;

	c->entries*=2;
	if(config_addvalues(c, variable, values)==0) return 0;
	else return -1;
}

const char** config_getvalues(struct config* c, const char* variable)
{
	for(int i=0; i<c->entries; ++i)
	{
		if(c->variables[i] && strcmp(variable, c->variables[i])==0)
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
		if(c->values[i])
		{
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
	}
	free(c->variables);
	c->variables=NULL;

	free(c->values);
	c->values=NULL;

	c->entries=0;
	if(!c->variables && !c->values) return 0;
	else return -1;
}
