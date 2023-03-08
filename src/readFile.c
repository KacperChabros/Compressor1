#include <stdio.h>
#include <stdlib.h>
#include "readFile.h"
unsigned char *readfile(fileInfo_t file,FILE *infile, unsigned char *bigbuffer)
{
	unsigned char buffer[1];
	while(fread(buffer,1,1,infile)==1)
	{
		if(file->counter==file->length)
		{
			file->length *= 2;
			bigbuffer = realloc(bigbuffer, file->length * sizeof(unsigned char));
		}
		bigbuffer[file->counter] = buffer[0];
		file->counter++;
		/*file->character=addcharInfo(file->character,buffer[0],&(file->distinctChars));*/
	}
	return bigbuffer;
}
charInfo *frequency(fileInfo_t file, unsigned char *bigbuffer, charInfo *charinfo1)
{
	int i;
	for(i=0; i<file->counter ;i++)
	{
		charinfo1=addcharInfo(charinfo1,bigbuffer[i], &(file->distinctChars));
	}
	return charinfo1;
}
/*unsigned char *doubleSize(unsigned char *bigbuffer,int *length)
{
	int i;
	*length*=2;
	char *tmp = malloc( *length * sizeof(*tmp));
	for(i=0; i<(*length / 2); i++)
	{
		*(tmp+i) = *(bigbuffer+i);
	}
	bigbuffer=realloc(bigbuffer,*length*sizeof(bigbuffer));
	return bigbuffer;
}*/
charInfo *addcharInfo(charInfo *info,unsigned char buffer, int *distinctChars)
{
	if(info==NULL)
	{
		info=malloc(sizeof(info));
		info->value=buffer;
		info->freq=1;
		info->next=NULL;
		*distinctChars=1;
	}else{
		charInfo *i;
		for(i=info; i->next!=NULL;i=i->next)
		{
			if(i->value==buffer)
			{
				i->freq++;
				return info;
			}
		}
		if(i->value == buffer)
		{
			i->freq++;
			return info;
		}
		charInfo *tmp=malloc(sizeof(*tmp));
		tmp->value=buffer;
		tmp->freq=1;
		tmp->next=NULL;
		i->next=tmp;
		(*distinctChars)++;
	}
	return info;
}
void freecharInfo(charInfo *charinfo1)
{
	charInfo *i;
	charInfo *tmp;
	if(charinfo1 == NULL)
	{
		return;
	}else if(charinfo1->next == NULL)
	{
		free(charinfo1);
	}else{
		tmp=charinfo1;
		for(i=charinfo1->next; i->next != NULL; i=i->next)
		{
			free(tmp);
			tmp=i;
		}
		free(i);
	}
}
