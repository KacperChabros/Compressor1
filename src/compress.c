#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"
#include "compress.h"
#include <stdbool.h>
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
	char flag=0;
	dictionary *tmp;
	data_t *union1=malloc(sizeof(*union1));
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
	fseek(outfile,2,SEEK_CUR);			/*leaving space for flag and checksum*/
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

		//OBSŁUGA POZOSTAŁYCH TEMPSIZE. POTEM OBSŁUGA PK I MASKI. ZABAWA Z STRUKTURĄ FILE.
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

	}
	flag+=tempSize;		/*how many oldest bits of last byte are important*/
	fseek(outfile,2,SEEK_SET);	/*write flag and checksum*/
	fprintf(outfile,"%c",flag);
	fprintf(outfile,"%c",checksum);
	printf("tempSize na koniec: %d\n",tempSize);
}
