#include <stdio.h>
#include <stdlib.h>
#include "decompress.h"
#include "huffman.h"
#include "compress.h"
#include "trie.h"
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>

int isValid(unsigned char checkSum, FILE *infile)
{
	int i;
	unsigned char buffer[1];
	unsigned char tmpSum;
	fseek(infile, 3, SEEK_SET);
	if(fread(buffer, 1, 1, infile) == 1)
	{
		tmpSum = buffer[0];
	}else{
		fprintf(stderr, "A problem with reading header of compressed file occurred\n");
		return 5;
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
		fprintf(stderr,"The file is valid!\n");
		return 0;
	}else{
		printf("The file is corrupted\n");
		return 6;
	}
}
void decompressL1(FILE *infile, char *inName, FILE *outfile, int maxLengthOfCode, trieNode *root, int dictLength, int notCompressedBytes, int lastBits);

void decompressL2(FILE *infile, char *inName, FILE *outfile, int maxLengthOfCode, trieNode *root, int dictLength, int notCompressedBytes, int lastBits);

void decompressL3(FILE *infile, char *inName, FILE *outfile, int maxLengthOfCode, trieNode *root, int dictLength, int notCompressedBytes, int lastBits);


int decompressFile(FILE *infile, char *inName, char *outName, char checksum, bool info)
{
	FILE *outfile = fopen(outName, "wb");
	struct stat stats;					/*to get file size*/
	stat(inName, &stats);
	unsigned long long int sizeOfFile = stats.st_size;
	int isCorrupted = 0;
	unsigned char buffer[1];
	unsigned char *dictBuffer;
	int i, j;
	unsigned char compressLevelMask = 0b11000000;
	unsigned char cypherMask = 0b00100000;
	unsigned char lastBitsMask = 0b00011111;		/*think this through*/ 	/*of compressed file*/

	unsigned char notCompressedBytesMask = 0b11000000;
	unsigned char lastBitsOfDictMask = 0b00001111;

	unsigned char compressLevel = 0;
	unsigned char cypher = 0;
	unsigned char lastBits = 0;

	unsigned int dictLength = 0;
	unsigned char lastBitsOfDict = 0;
	unsigned char notCompressedBytes = 0;

	unsigned short currSymbol = 0;
	unsigned char currBitLength = 0;
	int tempSize = 0;	
	unsigned char status = 1; 				/* status defines whether we are looking for a symbol {1}, a bitLength {2} or a Code {3}
								   to make a dictionary*/
	fseek(infile, 0, SEEK_SET);
	if( fread(buffer, 1, 1, infile) == 1)
	{
		if(buffer[0] != 'S')
		{
			fprintf(stderr,"This file hasn't been compressed by this program or has already been decompressed\n");
			return 4;
		}
	}
	else
	{
		fprintf(stderr, "A problem with reading header of compressed file occurred\n");
		return 5;
	}
	if( fread(buffer, 1, 1, infile) == 1)
	{
		if(buffer[0] != 'K')
		{
			fprintf(stderr,"This file hasn't been compressed by this program or has already been decompressed\n");
			return 4; 
		}
	}
	else
	{
		fprintf(stderr, "A problem with reading header of compressed file occurred\n");
		return 5;
	}

	if( fread(buffer, 1, 1, infile) == 1 )
	{
		compressLevel = compressLevelMask & buffer[0];
		compressLevel = compressLevel >> 6;
		cypher = cypherMask & buffer[0];
		lastBits = lastBitsMask & buffer[0];
	}else{

		fprintf(stderr, "A problem with reading header of compressed file occurred\n");
		return 5;
	}
	if(info == true)
		fprintf(stderr,"Read Masks:\nCompressLevel: %d, isCyphered: %d, no. important bits of last compressed byte: %d\n", compressLevel, cypher, lastBits);
	if( (isCorrupted = isValid(checksum, infile) != 0))
		return isCorrupted;
	fseek(infile,4, SEEK_SET);
	for(i=0; i<3; i++)
	{
		if( fread(buffer, 1, 1, infile) == 1)
		{
			dictLength = dictLength<<8;
			dictLength += buffer[0];
		}
		else{
			fprintf(stderr, "A problem with reading header of compressed file occurred\n");
			return 5;
		}
	}
	if(info == true)
		fprintf(stderr, "Length of dictionary in bytes: %d\n", dictLength);
	if( fread(buffer, 1, 1, infile) == 1)
	{
		notCompressedBytes = notCompressedBytesMask & buffer[0];
		notCompressedBytes = notCompressedBytes >> 6;
		lastBitsOfDict = lastBitsOfDictMask & buffer[0];
	}
	else{
			fprintf(stderr, "A problem with reading header of compressed file occurred\n");
			return 5;
	}
	if(info == true)
		fprintf(stderr, "No. of  important bits of the lastbyte in dictionary: %d\n and no. of not compressed bytes: %d\n", lastBitsOfDict, notCompressedBytes);
	
	if( compressLevel != 0)
	{
		dictionary *readDict = NULL;
		data_t *unionRead = malloc(sizeof(*unionRead));
		dictBuffer = malloc( dictLength * sizeof(dictBuffer));
	
		fread(dictBuffer, 1, dictLength, infile);
	

		unsigned char currBuf = 0; 
		unsigned int neededToSymbol = 0;			/*while reading a symbol - how many more bits are needed to read the symbol*/
		switch(compressLevel)
		{
			case 1:
				neededToSymbol = 8;
				break;
			case 2:
				neededToSymbol = 12;
				break;
			case 3: 
				neededToSymbol = 16;
				break;
		}
		unsigned int neededToBitLength = 8;			/*while reading symbol's length - how many more bits are needed to read the bitLength*/
		unsigned int neededToCode = 0;				/*while reading a code - how many more bits are needed to read the code 
									  - determined by bitLength*/
		char *currCode;
		unsigned char oneOrZeroFlag = 0;			
		unsigned char currentCodeIndex = 0;		
       		int currZero = 0;	
		int maxLengthOfCode = 0;

		trieNode *root = addNode();

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
				int power = pow(2, j);				/*checking if current bit is 1 or 0*/
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
						if(compressLevel == 1)
						{
							if(neededToSymbol == 0)
							{
								currSymbol = unionRead->Val.A;
								status = 2;
								neededToSymbol = 8;
							}
						}
						else if(compressLevel == 2)
						{
							if(neededToSymbol == 0)
							{
								currSymbol = 0;
								currSymbol = unionRead->buf;
								currSymbol = (currSymbol & 0b0000111111111111);
								status = 2;
								neededToSymbol = 12;
							}
						}
						else if(compressLevel == 3)
						{
							if(neededToSymbol == 8)
							{
								currSymbol = unionRead->Val.A;
								currSymbol = currSymbol<<8;
							}else if(neededToSymbol == 0)
							{
								currSymbol += unionRead->Val.A;
								status = 2;
								neededToSymbol = 16;
							}
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
						if(neededToCode == 0)			/*add to dictionary*/
						{
							currCode[currentCodeIndex] = '\0';
							addWord(root, currCode, currSymbol);
							if(currBitLength >= maxLengthOfCode)
								maxLengthOfCode = currBitLength;
							status = 1;
							currentCodeIndex = 0;
							currSymbol = 0;				
							free(currCode);
						}
						break;
				}
			}
		}

		fseek(infile, 8+dictLength, SEEK_SET);								/*8 is size of header*/
	
		if(compressLevel == 1)
		{
			decompressL1(infile, inName, outfile, maxLengthOfCode, root, dictLength, notCompressedBytes, lastBits);
		}
		else if(compressLevel == 2)
		{
			decompressL2(infile, inName, outfile, maxLengthOfCode, root, dictLength, notCompressedBytes, lastBits);
		}
		else if(compressLevel == 3)
		{
			decompressL3(infile, inName, outfile, maxLengthOfCode, root, dictLength, notCompressedBytes, lastBits);
		}
		free(unionRead);
		free(dictBuffer);
		freeDict(readDict);
		freeTrie(root);
	}/*KONIEC IFA Z CL!=0*/
	else{
		fseek(infile, 8, SEEK_SET);
		while((fread(buffer,1,1,infile)==1))
		{
			fwrite(buffer,1,1,outfile);
		}
		
	}
	fclose(infile);
	fclose(outfile);
	return 0;
}

void decompressL1(FILE *infile, char *inName, FILE *outfile, int maxLengthOfCode, trieNode *root, int dictLength, int notCompressedBytes, int lastBits) 
{
	char *currentReadCode = malloc( maxLengthOfCode * sizeof(*currentReadCode));	
	int currentReadLength = 0;
	trieNode *currentEntry = NULL;
	int currZero = 0;
	int i = 8 + dictLength;
	int j;
	unsigned char buffer[1];
	unsigned char currBuf = 0;
	struct stat stats;					/*to get file size*/
	stat(inName, &stats);
	unsigned long long int sizeOfFile = stats.st_size;

	while( fread(buffer, 1, 1, infile) == 1)
	{
		currBuf = buffer[0];

		if(i==(sizeOfFile - notCompressedBytes - 1))				/*determines how many oldest bits of the last byte should be read*/
		{
			currZero += (8 - lastBits);
		}
		else
		{
			currZero = 0;
		}
		for(j=7; j>=currZero; j--)									
		{
			int power = pow(2, j);
			if(currBuf >= power)
			{
				currentReadCode[currentReadLength++] = '1';
				currBuf-=power;
			}else{
				currentReadCode[currentReadLength++] = '0';
			}
			currentReadCode[currentReadLength] = '\0';
			if( (currentEntry = lookForSymbol(root, currentReadCode)) != NULL ) 		/*find if the code exists in the dictionary*/
			{
				fprintf(outfile, "%c", currentEntry->symbol);
				currentReadLength = 0;
			}	
		}
		i++;
	}
	if(notCompressedBytes > 0 )
	{
		for(i=0; i<notCompressedBytes; i++)
		{
			fread(buffer, 1, 1, infile);
			fprintf(outfile, "%c", buffer[0]);
		}
	}

	free(currentReadCode);
}

void decompressL2(FILE *infile, char *inName, FILE *outfile, int maxLengthOfCode, trieNode *root, int dictLength, int notCompressedBytes, int lastBits)
{
	char *currentReadCode = malloc( maxLengthOfCode * sizeof(*currentReadCode));	
	int currentReadLength = 0;
	trieNode *currentEntry = NULL;
	int currZero = 0;
	int i = 8 + dictLength;
	int j;
	unsigned char buffer[1];
	unsigned char currBuf = 0;
	struct stat stats;					/*to get file size*/
	stat(inName, &stats);
	unsigned long long int sizeOfFile = stats.st_size;
	unsigned char halfChar = 0;
	int written = 0;
	int breakFlag = 0;
	while( fread(buffer, 1, 1, infile) == 1)
	{
		currBuf = buffer[0];

		if( i==(sizeOfFile - notCompressedBytes - 1))				/*determines how many oldest bits of the last byte should be read*/
		{
			currZero += (8 - lastBits);
			if(lastBits == 8)
				breakFlag = 1;
		}
		else
		{
			currZero = 0;
		}
		for(j=7; j>=currZero; j--)
		{
			int power = pow(2, j);
			if(currBuf >= power)
			{
				currentReadCode[currentReadLength++] = '1';
				currBuf-=power;
			}else{
				currentReadCode[currentReadLength++] = '0';
			}
			currentReadCode[currentReadLength] = '\0';
			
			if( (currentEntry = lookForSymbol(root, currentReadCode)) != NULL ) 
			{
				
				unsigned short symbol = currentEntry->symbol;
				unsigned char fullChar = 0;
				written++;
				if(written % 2 != 0)
				{
					halfChar += (symbol & 0b0000000000001111);
					halfChar = halfChar<<4;
					symbol = symbol >> 4;
					fullChar += symbol;
					fprintf(outfile, "%c", fullChar);
				}
				else
				{
					fullChar += (symbol & 0b0000000011111111);
					symbol = symbol >> 8;
					halfChar += symbol;
					fprintf(outfile, "%c", halfChar);
					fprintf(outfile, "%c", fullChar);
					halfChar = 0;
				}
				currentReadLength = 0;
			}
		}
		i++;
		if(currZero != 0 || breakFlag == 1)
		{
			break;
		}
	}

	if(notCompressedBytes > 0 )
	{
		for(i=0; i<notCompressedBytes; i++)
		{
			fread(buffer, 1, 1, infile);
			fprintf(outfile, "%c", buffer[0]);
		}
	}

	free(currentReadCode);
}

void decompressL3(FILE *infile, char *inName, FILE *outfile, int maxLengthOfCode, trieNode *root, int dictLength, int notCompressedBytes, int lastBits)
{
	char *currentReadCode = malloc( maxLengthOfCode * sizeof(*currentReadCode));	
	int currentReadLength = 0;
	trieNode *currentEntry = NULL;
	int currZero = 0;
	int i = 8 + dictLength;
	int j;
	unsigned char buffer[1];
	unsigned char currBuf = 0;
	struct stat stats;					/*to get file size*/
	stat(inName, &stats);
	unsigned long long int sizeOfFile = stats.st_size;
	int breakFlag = 0;
	
	while( fread(buffer, 1, 1, infile) == 1)
	{
		currBuf = buffer[0];

		if(i==(sizeOfFile - notCompressedBytes - 1))				/*determines how many oldest bits of the last byte should be read*/
		{
			currZero += (8 - lastBits);
			if(lastBits == 8)
				breakFlag = 1;
		}
		else
		{
			currZero = 0;
		}
		for(j=7; j>=currZero; j--)
		{
			int power = pow(2, j);
			if(currBuf >= power)
			{
				currentReadCode[currentReadLength++] = '1';
				currBuf-=power;
			}else{
				currentReadCode[currentReadLength++] = '0';
			}
			currentReadCode[currentReadLength] = '\0';
			
			if( (currentEntry = lookForSymbol(root, currentReadCode)) != NULL ) 
			{
				unsigned short symbol = 0;
				unsigned char secondHalf = 0;
				unsigned char firstHalf = 0;
				symbol = currentEntry->symbol;
				secondHalf = (symbol & 0b0000000011111111);
				symbol = symbol >> 8;
				firstHalf = (symbol & 0b0000000011111111);
				fprintf(outfile, "%c", firstHalf);
				fprintf(outfile, "%c", secondHalf);
				currentReadLength = 0;
			}
		}
		i++;
		if(currZero != 0 || breakFlag == 1)
		{
			break;
		}
	}	

	if(notCompressedBytes > 0 )
	{
		for(i=0; i<notCompressedBytes; i++)
		{
			fread(buffer, 1, 1, infile);
			fprintf(outfile, "%c", buffer[0]);
		}
	}
	free(currentReadCode);
}
