#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "readFile.h"
#include "huffman.h"
#include "compress.h"
#include "cypher.h"
void showHelp()
{
	printf("Prawidłowe wywołanie: ./prog <infile> <outfile> [parametry]\nInfile: Plik wejściowy\nOutfile: Plik wyjściowy\nParametry:\n"); 
	printf(" -x  Plik infile ma zostać zdekompresowany.Jeśli jest już zdekompresowany program wyświetli błąd \n -z Plik infile ma zostać skompresowany. \n -o<0,1,2,3> Poziom kompresji. Domyślny poziom kompresji to 1\n -c <haslo> Plik infile ma zostać zaszyfrowany podanym hasłem.Ten parametr musi zostać podany jako osobny parametr lub na koniec zbioru parametrów\n -v Program wypisze dodatkowe informacje\n"); 
	printf("\n");
	printf("Correct usage: ./prog <infile> <outfile> [parameters]\n"); 
	printf("Infile: Input file\n"); 
	printf("Outfile: Output file\n");
       	printf("Parameters:\n"); 
	printf("-x Decompress input file. If already decompressed display error.\n"); 
	printf("-z Compress input file.\n");
       	printf("-o<0,1,2,3> Level of compression. Default is 1.\n");
       	printf("-c <password> Cypher input file with the given password. This parameter must be given separately or at the end of other parameters.\n"); 
	printf("-v Display additional informations.\n");
}
void isValid(unsigned char checkSum, FILE *infile);

int main(int argc, char **argv)
{
	bool decompress=false,compress=false,cypher=false,info=false;/*Flagi dla parametrów: x-decompress z-compress c-cypher v-info*/
	int compresslevel=1;/*Flaga dla poziomu kompresji o1 o2 o3 o0*/
	FILE *infile,*outfile;
	char *password=NULL;
	int i,j;
	unsigned char checksum=0b10001001;
	fileInfo_t file1=malloc(sizeof(file1));
	file1->length=1000;
	file1->counter=0;
	file1->distinctChars=0;
	charInfo *charinfo1=NULL;
	/*file1->character=NULL;*/
	
	/*file1->bigbuffer=malloc(file1->length * sizeof(file1->bigbuffer));*/
	unsigned char *bigbuffer = malloc( file1->length * sizeof(*bigbuffer));

	dictionary *dict1 = NULL;
	if(argc<3)
	{
		showHelp();
		return 1;
	}
	for(i=3;i<argc;i++)
	{
		if(argv[i][0]=='-')
		{
			for(j=1; argv[i][j] != '\0'; j++)
			{
				switch(argv[i][j])
				{
					case 'x':
						decompress=true;
						break;
					case 'z':
						compress=true;
						break;
					case 'c':
						cypher=true;
						if(argv[i][j+1]!='\0'){
							showHelp();
							return 1;
						}else{
							if(i+1<argc && argv[i+1][0]!='-')
								password=argv[i+1];
							else{
								showHelp();
								return 1;
							}
						}
						break;
					case 'v':
						info=true;
						break;
					case 'h':
						showHelp();
						return 1;
						break;
					case 'o':
						switch(argv[i][j+1])
						{
							case '0':
								compresslevel=0;
								break;
							case '1':
								compresslevel=1;
								break;
							case '2':
								compresslevel=2;
								break;
							case '3':
								compresslevel=3;
								break;
							default:
								break;

						}
						break;
					default:
						break;

				}
			}
		}
	}
	printf("Decompress=%d | Compress=%d | Cypher=%d Password=%s| Info=%d | Level=%d \n", decompress,compress,cypher,password,info,compresslevel);
	infile=fopen(argv[1],"rb");
	outfile=fopen(argv[2],"wb");
	if(infile==NULL || outfile==NULL)
	{
		printf("A problem occurred with opening files\n");
		return 2;
	}
	bigbuffer=readfile(file1,infile, bigbuffer);
	for(i=0; i<file1->counter; i++)
	{
		printf("Code of symbol: %d\n", bigbuffer[i]);
	}
	printf("------------------------------------");
	charinfo1=frequency(file1,bigbuffer,charinfo1);
	printf("file1->length: %d | file1->counter: %d \n",file1->length,file1->counter);
	/*for(i=0; i<file1->counter; i++)
	{
		fwrite( (bigbuffer+i), 1, 1, outfile);
	}*/
	charInfo *iter;
	for(iter = charinfo1; iter != NULL; iter=iter->next)
	{
		printf("Symbol %d: %c and its frequency: %d\n", iter->value, iter->value, iter->freq);
	}

	dict1 = makeDictionary(file1, charinfo1);
	printf("distinct chars: %d\n", file1->distinctChars);
	
	dictionary *iterDictionary;
	for(iterDictionary = dict1; iterDictionary != NULL; iterDictionary=iterDictionary->next)
	{
		printf("Symbol no. %d: %c and its code %s and its size %d\n ", iterDictionary->symbol, iterDictionary->symbol, iterDictionary->code, iterDictionary->bitLength);
	}
	binWrite(dict1,bigbuffer,outfile,file1->counter,compresslevel,cypher,checksum);
	printf("NAPISALEM BINARY\n");
	fclose(outfile);
	if(cypher==true)
	{	
			
		outfile=fopen(argv[2],"rb");
		fileInfo_t file2=malloc(sizeof(file2));
        	file2->length=file1->length/2;
       		file2->counter=0;
        	file2->distinctChars=0;
        	unsigned char *bigbuffer2 = malloc( file2->length * sizeof(*bigbuffer2));
		bigbuffer2=readfile(file2,outfile,bigbuffer2);
		fclose(outfile);	
		outfile=fopen(argv[2],"wb");
		xorcode(file2->counter,outfile,bigbuffer2,password);
		/*fclose(outfile);
		outfile=fopen(argv[2],"wb");
		xorcode(file2->counter,outfile,bigbuffer2,password);*/
		free(bigbuffer2);
		free(file2);
		fclose(outfile);
	}
	fclose(infile);
	if(decompress==true)
	{
		infile=fopen(argv[1],"rb");
		isValid(checksum, infile);
		fclose(infile);
	}
	//fclose(outfile);
	free(bigbuffer);
	free(file1);
	freecharInfo(charinfo1);
	freeDict(dict1);
	return 0;
}

void isValid(unsigned char checkSum, FILE *infile)
{
	unsigned char buffer[1];
	unsigned char tmpSum;
	fread(buffer, 1, 1, infile);
	if(buffer[0] != 'S')
	{
		printf("This file hasn't been compressed by this program\n");
		return;
	}
	fread(buffer, 1, 1, infile);
	if(buffer[0] != 'K')
	{
		printf("This file hasn't been compressed by this program\n");
		return;
	}

	fread(buffer, 1, 1, infile);
	if(fread(buffer, 1, 1, infile) == 1)
	{
		tmpSum = buffer[0];
	}else{
		return;
	}
	while(fread(buffer, 1, 1, infile) == 1)
	{
		tmpSum = tmpSum ^ buffer[0];
	}
	if(tmpSum == checkSum)
	{
		printf("The file is valid!\n");
	}else{
		printf("The file is corrupted\n");
	}
}
