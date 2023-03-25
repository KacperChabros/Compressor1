#include <stdio.h>
#include <stdlib.h>
#include "readFile.h"
unsigned short *readfile(fileInfo_t file,FILE *infile, unsigned short *bigbuffer, int compresslevel, int *lastBytesNotCompressed, unsigned char *notCompressedBytes)
{
	unsigned char buffer[2];
	
	if( compresslevel == 1||compresslevel==0)
	{
		while(fread(buffer,1,1,infile)==1)
		{
			if(file->counter==file->length)
			{
				file->length *= 2;
				bigbuffer = realloc(bigbuffer, file->length * sizeof(unsigned short)); /*ZMIANY*/
			}
			bigbuffer[file->counter] = buffer[0];
			file->counter++;
		}
	}
	else if( compresslevel == 2 )
	{
		int i = 0;
		int readBytes = 0;
		unsigned char buff[3];
		while( (readBytes = fread(buff, 1, 3, infile)) == 3 || readBytes == 2 || readBytes == 1)
		{
			if((file->counter+2)==file->length)
			{
				file->length *= 2;
				bigbuffer = realloc(bigbuffer, file->length * sizeof(unsigned short)); 
			}

			if(readBytes == 3)
			{
				unsigned char tmp1 = buff[1];
				tmp1 = tmp1>>4;
				bigbuffer[file->counter] = 0;
				bigbuffer[file->counter] += buff[0];
				bigbuffer[file->counter] = bigbuffer[file->counter] << 4;
				bigbuffer[file->counter] += tmp1;
				//bigbuffer[file->counter] += (buff[1] & 0b11110000);
				file->counter++;
				bigbuffer[file->counter] = 0;
				bigbuffer[file->counter] += (buff[1] & 0b00001111);
				bigbuffer[file->counter] = bigbuffer[file->counter] << 8;
				bigbuffer[file->counter] += buff[2];
				file->counter++;
				//fprintf(stderr, "to %d + pierwsze cztery tego %d dało to: %d\n", buff[0], buff[1], bigbuffer[file->counter-1]);
				//fprintf(stderr, "drugie 4 tego %d + to %d dało to: %d\n\n", buff[1], buff[2], bigbuffer[file->counter]);

			}
			else if(readBytes == 2)
			{
				*lastBytesNotCompressed = 2;
				notCompressedBytes[0] = buff[0];
				notCompressedBytes[1] = buff[1];
			}
			else if(readBytes == 1)
			{
				*lastBytesNotCompressed = 1;
				notCompressedBytes[0] = buff[0];
			}

		}	
	}
	else if( compresslevel == 3)
	{
		int i = 0;
		int readBytes=0;
		while( (readBytes = fread(buffer, 1, 2, infile)) == 2 || readBytes == 1)
		{
			if(file->counter==file->length)
			{
				file->length *= 2;
				bigbuffer = realloc(bigbuffer, file->length * sizeof(unsigned short)); 
			}

			if(readBytes == 2)
			{
				bigbuffer[file->counter] = 0;
				bigbuffer[file->counter] += buffer[0];
			       	bigbuffer[file->counter] = bigbuffer[file->counter] << 8;
				bigbuffer[file->counter] += buffer[1];
				/*fprintf(stderr, "To: %d + to %d daje to: cyfra: %d\n", buffer[0], buffer[1], bigbuffer[file->counter]);*/
				file->counter++;
			}
			else
			{
				*lastBytesNotCompressed = 1;
				notCompressedBytes[0] = buffer[0];	
			}
		}
	}
	return bigbuffer;
}
charInfo *frequency(fileInfo_t file, unsigned short *bigbuffer, charInfo *charinfo1)	/*ZMIANY*/
{
	int i;
	for(i=0; i<file->counter; i++)
	{
		charinfo1=addcharInfo(charinfo1,bigbuffer[i], &(file->distinctChars));
	}
	return charinfo1;
}
charInfo *addcharInfo(charInfo *info,unsigned short buffer, int *distinctChars) 	/*ZMIANY*/
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
