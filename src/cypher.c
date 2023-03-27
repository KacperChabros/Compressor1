#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cypher.h"
void xorcode(FILE *outfile,char *key,int length, char *file,int compresslevel) /*Overwrites FILE using XOR with the given key*/
{	
	unsigned int dictLength=0;
	unsigned char *bigbuffer = malloc(length * sizeof(*bigbuffer));
	int counter=0;
	int i;
	int keylength=strlen(key); 
	int text=0;
	int charkey=0;
	unsigned char buffer[1];
	outfile=fopen(file,"rb");/*First we need to open outfile to read it as binary*/
	fseek(outfile,4,SEEK_SET);
	for(i=0; i<3; i++)/*Method for reading dictionary length*/
	{
		if( fread(buffer, 1, 1, outfile) == 1)
		{
			dictLength = dictLength<<8;
			dictLength += buffer[0];
		}
	}
	fseek(outfile,0,SEEK_SET);
	while(fread(buffer,1,1,outfile)==1) /*Reading the file and giving bigbuffer all the symbols*/
	{
		if(counter==length)
		{
			length*=2;
			bigbuffer=realloc(bigbuffer,length*sizeof(unsigned char));
		}
		bigbuffer[counter]=buffer[0];
		counter++;
	}
	fclose(outfile);
	outfile=fopen(file,"wb"); /*Overwriting with coded/decoded symbols excluding the first two symbols, flags, control sum and dictionary length*/
	if(compresslevel!=0){
		for(i=0;i<counter;i++)
		{
				//printf("Przed:Bigbuffer[%d](%d)=%c\t",i,(int)bigbuffer[i],bigbuffer[i]);
				if(i>=8 && i<dictLength+8){
					text=bigbuffer[i]-'A';
					charkey=key[i%keylength]-'A';
					bigbuffer[i]=(unsigned char)((text^charkey)+'A');
				}
				//printf("Po:Bigbuffer[%d](%d)=%c\n",i,(int)bigbuffer[i],bigbuffer[i]);
				fwrite((bigbuffer+i),1,1,outfile);
		}
	}else{
		for(i=0;i<counter;i++)
		{
				//printf("Jestem!Przed:Bigbuffer[%d](%d)=%c\t",i,(int)bigbuffer[i],bigbuffer[i]);
				if(i>=8){
				text=bigbuffer[i]-'A';
				charkey=key[i%keylength]-'A';
				bigbuffer[i]=(unsigned char)((text^charkey)+'A');
				}
				//printf("Przed:Bigbuffer[%d](%d)=%c\n",i,(int)bigbuffer[i],bigbuffer[i]);

			fwrite((bigbuffer+i),1,1,outfile);
		}
	}
	free(bigbuffer);	
}
int xorfile(FILE *outfile,char *key,int length, char *file,unsigned char checksum,char *tmpname) /*Overwrites FILE using XOR with the given key*/
{	
	unsigned int dictLength=0;
	unsigned char *bigbuffer = malloc(length * sizeof(*bigbuffer));
	unsigned char tmpSum;
	unsigned char compressLevelMask=0b11000000;
	unsigned char compressLevel=0;
	int counter=0;
	int i;
	int keylength=strlen(key); 
	int text=0;
	int charkey=0;
	unsigned char buffer[1];
	outfile=fopen(file,"rb");/*First we need to open outfile to read it as binary*/
	fseek(outfile,2,SEEK_SET);
	if(fread(buffer, 1, 1, outfile) == 1 )
        {
                compressLevel = compressLevelMask & buffer[0];
                compressLevel = compressLevel >> 6;
	}
	fseek(outfile,4,SEEK_SET);
	for(i=0; i<3; i++)/*Method for reading dictionary length*/
	{
		if( fread(buffer, 1, 1, outfile) == 1)
		{
			dictLength = dictLength<<8;
			dictLength += buffer[0];
		}
	}
	fseek(outfile,0,SEEK_SET);
	while(fread(buffer,1,1,outfile)==1) /*Reading the file and giving bigbuffer all the symbols*/
	{
		if(counter==length)
		{
			length*=2;
			bigbuffer=realloc(bigbuffer,length*sizeof(unsigned char));
		}
		bigbuffer[counter]=buffer[0];
		counter++;
	}
	/*Overwriting with coded/decoded symbols excluding the first two symbols, flags, control sum and dictionary length*/
	if(compressLevel!=0){
		for(i=0;i<counter;i++)
		{
				//printf("Przed:Bigbuffer[%d](%d)=%c\t",i,(int)bigbuffer[i],bigbuffer[i]);
				if(i>=8 && i<dictLength+8){
					text=bigbuffer[i]-'A';
					charkey=key[i%keylength]-'A';
					bigbuffer[i]=(unsigned char)((text^charkey)+'A');
				}
				//printf("Po:Bigbuffer[%d](%d)=%c\n",i,(int)bigbuffer[i],bigbuffer[i]);
		}
	}else{
		for(i=8;i<counter;i++)
		{	
			//printf("Przed:Bigbuffer[%d](%d)=%c\t",i,(int)bigbuffer[i],bigbuffer[i]);
			text=bigbuffer[i]-'A';
			charkey=key[i%keylength]-'A';
			bigbuffer[i]=(unsigned char)((text^charkey)+'A');
			//printf("Po:Bigbuffer[%d](%d)=%c\n",i,(int)bigbuffer[i],bigbuffer[i]);
		}
	}
	tmpSum=bigbuffer[3];
	for(i=8;i<counter;i++)
	{
		tmpSum=tmpSum^(bigbuffer[i]);
	}
	for(i=4;i<=7;i++)
	{
		tmpSum=tmpSum^(bigbuffer[i]);
	}
	//fprintf(stderr,"tmpSum:%d | checksum:%d\n",tmpSum,checksum);
	if(tmpSum==checksum)
	{	
		fclose(outfile);
		outfile=fopen(tmpname,"wb"); 
		for(i=0;i<counter;i++)
		{
			fwrite((bigbuffer+i),1,1,outfile);
		}
		fclose(outfile);
		free(bigbuffer);
		return 0;
	}else{
		fprintf(stderr,"The file is corrupted or the password is wrong\n");
		free(bigbuffer);
		return 6;
	}
	
	
	
	}

