#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"
#include "compress.h"
#include <stdbool.h>
#include <math.h>
dictionary *findCode(dictionary *dict,unsigned char symbol)	/* returns an instance of a dictionary with desired symbol*/
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
void binWrite(dictionary *dict, unsigned char *bigbuffer ,FILE *outfile,int counter, int compressLevel, bool cypher, unsigned char checksum)
{
	int i, j;
	int tempSize=0;
	int dictLength=0;		/*in bytes*/
	int lastBitsOfDict=0;		/*how many oldest bits of the last byte of dictionary are important*/ 
	dictionary *iter;
	char flag=0;
	dictionary *tmp;
	data_t *union1=malloc(sizeof(*union1));
	data_t *unionDict=malloc(sizeof(*unionDict));
	fprintf(outfile,"SK");				/*distinctive beginning of our compressed file*/
	/*TO DO: ZAPISAĆ SŁOWNIK. PRZEJECHAĆ SŁOWNIK PRZEZ CHECKSUM.*/
	if(compressLevel==1)				/*adding level of compression to flag - 2 oldest bits*/
		flag+=64;
	else if(compressLevel==2)
		flag+=128;
	else if(compressLevel==4)
		flag+=192;
	if(cypher==true)				/*adding cypher to flag 3rd oldest bit*/
		flag+=32;
	fseek(outfile,6,SEEK_CUR);			/*leaving space for flag and checksum and dictionary length and how many bits left in
							  the last byte of dictionary are important. Dictionary length is 3 bytes*/
	for(iter=dict;iter!=NULL;iter=iter->next)
	{	

		int currSymbol = iter->symbol;
		int currBitLength = iter->bitLength;
		//if(tempSize > 8)		* if its longer than a byte -> write to file and XOR written byte*
		/*{
			fwrite(&unionDict->Val.A,1,1,outfile);	
			checksum=checksum^unionDict->Val.A;
			tempSize-=8;
			dictLength++;
		}*/
		/*unionDict->buf=unionDict->buf<<8;
		unionDict->buf+=(short)iter->symbol;
		tempSize+=8;
		if(tempSize > 8)		* if its longer than a byte -> write to file and XOR written byte*
		{
			fwrite(&unionDict->Val.A,1,1,outfile);
			checksum=checksum^unionDict->Val.A;
			tempSize-=8;
			dictLength++;
		}*/
		for(i=7; i>=0; i--)
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
		/*unionDict->buf=unionDict->buf<<8;
		unionDict->buf+=(short)iter->bitLength;
		tempSize+=8;
		*///if(tempSize > 8)		/* if its longer than a byte -> write to file and XOR written byte*
		/*{
			fwrite(&unionDict->Val.A,1,1,outfile);	
			checksum=checksum^unionDict->Val.A;
			tempSize-=8;
			dictLength++;
		}*/
		for(i=7; i>=0; i--)
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

		for(i=0;i<iter->bitLength;i++)
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
		/*if(tempSize >= 8)
		{
			fwrite(&union1->Val.A,1,1,outfile);
			tempSize-=8;
		}*/

		/*tempSize+=8;
		if(tempSize > 8)
		{
			fwrite(&union1->Val.A,1,1,outfile);
			tempSize-=8;
		}
		union1->buf=union1->buf<<8;
		union1->buf+=bigbuffer[i];*/
		/*if(tempSize>=8)
		{
			fwrite(&union1->Val.A,1,1,outfile);
			tempSize-=8;
		}*/

		/*printf("tempSize po if i petli: %d\n",tempSize);*/
	}
	if(tempSize>0)		/*add zeroes to fill the byte and write to file*/
	{
		union1->buf=union1->buf<<(8-tempSize);
		fwrite(&union1->Val.A,1,1,outfile);
		checksum=checksum^union1->Val.A;
		flag+=tempSize;		/*how many oldest bits of last byte are important*/
	}else{				/* tempSize == 0 -> all bits of last byte of file are important*/
		flag+=8;
	}
	//fseek(outfile,2,SEEK_SET);	/*write flag and checksum*
	//fprintf(outfile,"%c",flag);
	//fprintf(outfile,"%c",checksum);
	fprintf(stderr, "thats a dictLength: %d\n", dictLength);

	unsigned int secondOldest = dictLength & 0x00FF0000;		/*masks to extract 2nd, 3rd and 4th oldest bytes*/
	unsigned int thirdOldest = dictLength & 0x0000FF00;
	unsigned char fourthOldest = dictLength & 0x000000FF;
	
	unsigned char secondOldestChar = 0;				/*variables to put those extracted bytes in*/
	unsigned char thirdOldestChar = 0;

	secondOldestChar = secondOldest>>16;				/*shifting bits to the right to make them fit into char*/
	thirdOldestChar = thirdOldest>>8;
	//fprintf(outfile, "%c", secondOldestChar);			/*writing them to file*
	//fprintf(outfile, "%c", thirdOldestChar);
	//fprintf(outfile, "%c", fourthOldest);
									
	//fprintf(outfile, "%c", lastBitsOfDict);				/*writing the number of the oldest important bits in the last byte of dictionary*
	
	checksum = checksum ^ secondOldestChar;
	checksum = checksum ^ thirdOldestChar;
	checksum = checksum ^ fourthOldest;
	checksum = checksum ^ lastBitsOfDict;

	fseek(outfile,2,SEEK_SET);	/*write flag and checksum*/
	fprintf(outfile,"%c",flag);
	fprintf(outfile,"%c",checksum);

	fprintf(outfile, "%c", secondOldestChar);			/*writing the length of dict to file*/
	fprintf(outfile, "%c", thirdOldestChar);
	fprintf(outfile, "%c", fourthOldest);

	fprintf(outfile, "%c", lastBitsOfDict);				/*writing the number of the oldest important bits in the last byte of
									  dictionary*/

	printf("tempSize na koniec: %d\n",tempSize);
}
