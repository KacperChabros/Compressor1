#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cypher.h"
void xorcode(FILE *outfile,char *key,int length, char *file) /*Overwrites FILE using XOR with the given key*/
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
	for(i=0;i<counter;i++)
	{
			/*printf("Przed:Bigbuffer[%d](%d)=%c\t",i,(int)bigbuffer[i],bigbuffer[i]);*/
			if(i>=8 && i<dictLength+8){
				text=bigbuffer[i]-'A';
				charkey=key[i%keylength]-'A';
				bigbuffer[i]=(unsigned char)((text^charkey)+'A');
			}
			/*printf("Po:Bigbuffer[%d](%d)=%c\n",i,(int)bigbuffer[i],bigbuffer[i]);*/
			fwrite((bigbuffer+i),1,1,outfile);
	}
	free(bigbuffer);	
}
