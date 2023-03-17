#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cypher.h"
void xorcode(int counter,FILE *outfile,unsigned char *bigbuffer,char *key) /*Overwrites FILE using XOR with the given key*/
{	
	int i;
	int keylength=strlen(key); 
	int text;
	int charkey;
	for(i=3;i<counter;i++)
	{
		text=bigbuffer[i]-'A'; 
		charkey=key[i%keylength]-'A';
		bigbuffer[i]=(unsigned char)((text^charkey)+'A');
		
	}
	for(i=0; i<counter; i++)
	{
		fwrite( (bigbuffer+i), 1, 1, outfile);
	}
}
