#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"
#include "compress.h"
#include <stdbool.h>
#include <math.h>
dictionary *findCode(dictionary *dict,unsigned short symbol)	/* returns an instance of a dictionary with desired symbol*/
{
	if(dict==NULL)
		return NULL;
	else{
		dictionary *iter;
		for(iter=dict;iter!=NULL;iter=iter->next)
		{
			if(iter->symbol==symbol)
				return iter;
		}
		fprintf(stderr,"Cannot find symbol\n");
	}
	return NULL;
}
void binWrite(dictionary *dict, unsigned short *bigbuffer ,FILE *outfile,int counter, int compressLevel, bool cypher, unsigned char checksum, int lastBytesNotCompressed, unsigned char *notCompressedBytes,bool info)
{
	int i, j;
	int iterator=0;
	int tempSize=0;
	int dictLength=0;		/*in bytes*/
	int lastBitsOfDict=0;		/*how many oldest bits of the last byte of dictionary are important*/ 
	dictionary *iter;
	char flag=0;
	char notCompressedAndDictLengthFlag = 0;		/*flag to store: 
								  -how many not compressed bytes are there:
								  *2 oldest bits:
								  00 - 0 bytes
								  01 - 1 byte
								  10 - 2 bytes

								  -how many oldest bits of the last byte of dictionary are important - 4 youngest bits*/

	dictionary *tmp;
	data_t *union1=malloc(sizeof(*union1));
	data_t *unionDict=malloc(sizeof(*unionDict));
	
	switch(compressLevel)				/*iterator defines how many bits-1 are important in the value of symbol*/
	{
		case 1:
			iterator = 7;
			break;
		case 2:
			iterator = 11;
			break;
		case 3:
			iterator = 15;
			break;
	}

	fprintf(outfile,"SK");				/*distinctive beginning of our compressed file*/
	if(compressLevel==1)				/*adding level of compression to flag - 2 oldest bits*/
		flag+=64;
	else if(compressLevel==2)
		flag+=128;
	else if(compressLevel==3)
		flag+=192;
	if(cypher==true)				/*adding cypher to flag 3rd oldest bit*/
		flag+=32;

	if(lastBytesNotCompressed == 1)
		notCompressedAndDictLengthFlag += 64;
	else if(lastBytesNotCompressed == 2)
		notCompressedAndDictLengthFlag += 128;

	fseek(outfile,6,SEEK_CUR);			/*leaving space for flag and checksum and dictionary length and how many bits left in
							  the last byte of dictionary are important. Dictionary length is 3 bytes*/
	
	if(compressLevel == 0)
	{	for(i=0;i<counter;i++)
		{
			checksum=checksum^bigbuffer[i];
			fwrite((bigbuffer+i),1,1,outfile);
		}
	}
	else
	{
		for(iter=dict;iter!=NULL;iter=iter->next)
		{	

			int currSymbol = iter->symbol;
			int currBitLength = iter->bitLength;
			for(i=iterator; i>=0; i--)		/*writing symbol*/
			{
				int power = pow(2, i);
				tempSize+=1;
				if(tempSize > 8)		/* if its longer than a byte -> write to file and XOR written byte*/
				{
					fwrite(&unionDict->Val.A,1,1,outfile);	
					checksum=checksum^unionDict->Val.A;
					tempSize-=8;
					dictLength++;
				}
				unionDict->buf = unionDict->buf<<1;
				if(currSymbol >= power)
				{
					unionDict->buf+=1;
					currSymbol-=power;
				}	
			}
			for(i=7; i>=0; i--)			/*writing length of code in bits*/
			{	
				int power = pow(2, i);
				tempSize+=1;
				if(tempSize > 8)		/* if its longer than a byte -> write to file and XOR written byte*/
				{
					fwrite(&unionDict->Val.A,1,1,outfile);	
					checksum=checksum^unionDict->Val.A;
					tempSize-=8;
					dictLength++;
				}
				unionDict->buf = unionDict->buf<<1;
				if(currBitLength >= power)
				{
					unionDict->buf+=1;
					currBitLength-=power;
				}	
			}

			for(i=0;i<iter->bitLength;i++)		/*writing code to file*/
			{
				tempSize+=1;
				if(tempSize > 8)		/* if its longer than a byte -> write to file and XOR written byte*/
				{
					fwrite(&unionDict->Val.A,1,1,outfile);	
					checksum=checksum^unionDict->Val.A;
					tempSize-=8;
					dictLength++;
				}

				if(iter->code[i] == '1')		/*push bits to the left and place 1 as youngest bit*/
				{
					unionDict->buf = unionDict->buf<<1;
					unionDict->buf+=1;
				}else if(iter->code[i] == '0'){		/*push bits to the left*/
					unionDict->buf = unionDict->buf<<1;
				}
			}
		}
		if(tempSize>0)		/*add zeroes to fill the byte and write to file*/
		{
			unionDict->buf=unionDict->buf<<(8-tempSize);
			fwrite(&unionDict->Val.A,1,1,outfile);
			checksum=checksum^unionDict->Val.A;
			dictLength++;
			lastBitsOfDict = tempSize;
		}else{							/*tempSize == 0 then all bits of last bytes are important*/
			lastBitsOfDict = 8;
		}
		if(info==true)
			fprintf(stderr, "Number of extra bits in the last byte of dictionary: %d\n", 8-lastBitsOfDict);
		tempSize=0;
		for(i=0;i<counter;i++)
		{
			tmp=findCode(dict,bigbuffer[i]);	/*find code for current symbol*/
			for(j=0; j<tmp->bitLength; j++)
			{
				tempSize+=1;
				if(tempSize > 8)		/* if its longer than a byte -> write to file and XOR written byte*/
				{
					fwrite(&union1->Val.A,1,1,outfile);	
					checksum=checksum^union1->Val.A;
					tempSize-=8;
				}

				if(tmp->code[j] == '1')		/*push bits to the left and place 1 as youngest bit*/
				{
					union1->buf = union1->buf<<1;
					union1->buf+=1;
				}else if(tmp->code[j] == '0'){	/*push bits to the left*/
					union1->buf = union1->buf<<1;
				}
			}
		}
	}/*END OF COMPRESSIONLEVEL!=0*/
	if(tempSize>0)		/*add zeroes to fill the byte and write to file*/
	{
		union1->buf=union1->buf<<(8-tempSize);
		fwrite(&union1->Val.A,1,1,outfile);
		checksum=checksum^union1->Val.A;
		flag+=tempSize;		/*how many oldest bits of last byte (before not compressed ones) are important*/
	}else{				/* tempSize == 0 -> all bits of last byte of file are important*/
		flag+=8;
	}
	if(info==true)
		fprintf(stderr, "Extra bits of last compressed byte: %d\n", 8-tempSize);
	
	for(i=0; i<lastBytesNotCompressed; i++)
	{
		fprintf(outfile, "%c", notCompressedBytes[i]);
		checksum = checksum ^ notCompressedBytes[i];
	}
	if(info==true){
		fprintf(stderr, "Dictionary length in bytes is: %d\n", dictLength);
	}

	unsigned int secondOldest = dictLength & 0x00FF0000;		/*masks to extract 2nd, 3rd and 4th oldest bytes*/
	unsigned int thirdOldest = dictLength & 0x0000FF00;
	unsigned char fourthOldest = dictLength & 0x000000FF;
	
	unsigned char secondOldestChar = 0;				/*variables to put those extracted bytes in*/
	unsigned char thirdOldestChar = 0;

	secondOldestChar = secondOldest>>16;				/*shifting bits to the right to make them fit into char*/
	thirdOldestChar = thirdOldest>>8;
									
	
	checksum = checksum ^ secondOldestChar;
	checksum = checksum ^ thirdOldestChar;
	checksum = checksum ^ fourthOldest;

	notCompressedAndDictLengthFlag += lastBitsOfDict;
	checksum = checksum ^ notCompressedAndDictLengthFlag;

	fseek(outfile,2,SEEK_SET);	/*write flag and checksum*/
	fprintf(outfile,"%c",flag);
	fprintf(outfile,"%c",checksum);

	fprintf(outfile, "%c", secondOldestChar);			/*writing the length of dict to file*/
	fprintf(outfile, "%c", thirdOldestChar);
	fprintf(outfile, "%c", fourthOldest);

	fprintf(outfile, "%c", notCompressedAndDictLengthFlag);		/*writing number of not compressed bytes and number of important bits in the last
									  byte of dictionary*/
	
	free(union1);
	free(unionDict);
}
