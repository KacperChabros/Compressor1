#include <stdio.h>
#include <stdlib.h>
#include "decompress.h"
#include "huffman.h"
#include "compress.h"
#include <math.h>
#include <string.h>

int isValid(unsigned char checkSum, FILE *infile)
{
	int i;
	unsigned char buffer[1];
	unsigned char tmpSum;
	fread(buffer, 1, 1, infile);
	if(buffer[0] != 'S')
	{
		printf("This file hasn't been compressed by this program\n");
		return 5;
	}
	fread(buffer, 1, 1, infile);
	if(buffer[0] != 'K')
	{
		printf("This file hasn't been compressed by this program\n");
		return 5;
	}

	fread(buffer, 1, 1, infile);
	if(fread(buffer, 1, 1, infile) == 1)
	{
		tmpSum = buffer[0];
	}else{
		return 4;
	}
	fseek(infile,4,SEEK_CUR);
	while(fread(buffer, 1, 1, infile) == 1)
	{
		tmpSum = tmpSum ^ buffer[0];
	}
	fseek(infile, 4 ,SEEK_SET);
	for(i=0; i<4; i++)
	{
		if(fread(buffer, 1, 1, infile) == 1)
		{
			tmpSum = tmpSum ^ buffer[0];
		}
	}
	if(tmpSum == checkSum)
	{
		/*printf("The file is valid!\n");*/
		return 0;
	}else{
		return 4;
		/*printf("The file is corrupted\n");*/
	}
}

void decompressFile(FILE *infile, char *outName, char checksum)
{
	FILE *outfile = fopen(outName, "wb");
	unsigned char buffer[1];
	unsigned char *dictBuffer;
	int i, j;
	unsigned char compressLevelMask = 0b11000000;
	unsigned char cypherMask = 0b00100000;
	unsigned char lastBitsMask = 0b00011111;		/*think this through*/
	
	unsigned char compressLevel = 0;
	unsigned char cypher = 0;
	unsigned char lastBits = 0;

	unsigned int dictLength = 0;
	unsigned char lastBitsOfDict = 0;

	unsigned char currSymbol = 0;
	unsigned char currBitLength = 0;
	int tempSize = 0;	
	unsigned char status = 1; 				/* status defines whether we are looking for a symbol {1}, a bitLength {2} or a Code {3}
								   to make a dictionary*/
	/*call isValid*/
	fseek(infile, 2, SEEK_SET);
	if( fread(buffer, 1, 1, infile) == 1 )
	{
		compressLevel = compressLevelMask & buffer[0];
		cypher = cypherMask & buffer[0];
		lastBits = lastBitsMask & buffer[0];
	}else{
		return;
	}
	//fprintf(stderr, "Maskiii: CL: %d, Cyp: %d, lastBits: %d\n", compressLevel, cypher, lastBits);
	fseek(infile, 1, SEEK_CUR);
	for(i=0; i<3; i++)
	{
		if( fread(buffer, 1, 1, infile) == 1)
		{
			dictLength = dictLength<<8;
			dictLength += buffer[0];
		}
	}
	//fprintf(stderr, "Długość słownika: %d\n", dictLength);
	if( fread(buffer, 1, 1, infile) == 1)
	{
		lastBitsOfDict = buffer[0];
	}
	//fprintf(stderr, "Ostatnie bity w słowniku: %d\n", lastBitsOfDict);
	
	dictionary *readDict = NULL;
	data_t *unionRead = malloc(sizeof(*unionRead));
	dictBuffer = malloc( dictLength * sizeof(dictBuffer));
	
	fread(dictBuffer, 1, dictLength, infile);

	unsigned char currBuf = 0;
	unsigned int neededToSymbol = 8;
	unsigned int neededToBitLength = 8;
	unsigned int neededToCode = 0;
	unsigned char *currCode;
	unsigned char oneOrZeroFlag = 0;
	unsigned char currentCodeIndex = 0;
       	int currZero = 0;	
	for(i=0; i<dictLength; i++)
	{
		currBuf = dictBuffer[i];
		if(i==(dictLength-1))				/*determines how many oldest bits of the last byte should be read*/
		{
			currZero += (8 - lastBitsOfDict);
		}
		else
		{
			currZero = 0;
		}
		for(j=7; j>=currZero; j--)
		{
			int power = pow(2, j);
			unionRead->buf = unionRead->buf<<1;
			if(currBuf >= power)
			{
				unionRead->buf += 1;
				currBuf-=power;
				oneOrZeroFlag = '1';
			}else{
				oneOrZeroFlag = '0';
			}
			switch(status)
			{
				case 1:
					neededToSymbol--;
					if(neededToSymbol == 0)
					{
						currSymbol = unionRead->Val.A;
						status = 2;
						neededToSymbol = 8;
					}	
					break;
				case 2:
					neededToBitLength--;
					if(neededToBitLength == 0)
					{
						currBitLength = unionRead->Val.A;
						status = 3;
						neededToBitLength = 8;
						neededToCode = currBitLength;
						currCode = malloc( (neededToCode + 1) * sizeof(currCode));
					}
					break;
				case 3: 
					neededToCode--;
					currCode[currentCodeIndex] = oneOrZeroFlag;
					currentCodeIndex++;
					if(neededToCode == 0)
					{
						currCode[currentCodeIndex] = '\0';
						readDict = addToReadDict(readDict, currSymbol, currBitLength, currCode);
						status = 1;
						currentCodeIndex = 0;						
						free(currCode);
					}
					break;
			}
		}
	}

	dictionary *iter;
	for(iter = readDict; iter != NULL; iter=iter->next)
	{
		fprintf(stderr, "Jestem symbolem %c o wartości %d. To długość mojego kodu: %d i on sam: %s\n", iter->symbol, iter->symbol, iter->bitLength, iter->code);
	}
	free(unionRead);
	free(dictBuffer);
	freeDict(readDict);
}
