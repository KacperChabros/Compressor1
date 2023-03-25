#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "readFile.h"
#include "huffman.h"
#include "compress.h"
#include "cypher.h"
#include "decompress.h"
void showHelp()
{
	printf("Prawidłowe wywołanie: ./prog <infile> <outfile> [parametry]\nInfile: Plik wejściowy\nOutfile: Plik wyjściowy\nParametry:\n"); 
	printf(" -x  Plik infile ma zostać zdekompresowany.Jeśli jest już zdekompresowany program wyświetli błąd \n -z Plik infile ma zostać skompresowany. \n -o<0,1,2,3> Poziom kompresji. Domyślny poziom kompresji to 1\n -c <haslo> Plik infile ma zostać zaszyfrowany podanym hasłem.Ten parametr musi zostać podany jako osobny parametr lub na koniec zbioru parametrów\n -v Program wypisze dodatkowe informacje\n");
       printf("Uwaga: -x i -z nie mogą być użyte jednocześnie\n");	
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
	printf("Note: -x and -z cannot be used together.\n");
}

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
	unsigned short *bigbuffer = malloc( file1->length * sizeof(*bigbuffer)); /*zmiany*/
	
	int lastBytesNotCompressed = 0;						/*defines how many last bytes of infile are not compressed
										  * 1st compression level - 0
										  * 2nd compression level - 0, 1 or 2 bytes
										  * 3rd compression level - 0 or 1 byte
										  */

	unsigned char *notCompressedBytes = malloc( 2 * sizeof(*notCompressedBytes)); 	/*buffer to store those not compressed bytes*/


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
	if(info==true){
		printf("Decompress=%d | Compress=%d | Cypher=%d Password=%s| Info=%d | Level=%d \n", decompress,compress,cypher,password,info,compresslevel);
	}
	if((decompress==false&&compress==false)||(decompress==true&&compress==true))
	{
		showHelp();
		return 1;
	}
	infile=fopen(argv[1],"rb");
	outfile=fopen(argv[2],"wb");
	if(infile==NULL || outfile==NULL)
	{
		fprintf(stderr,"A problem occurred with opening files\n");
		free(file1);
		free(bigbuffer);
		return 2;
	}
	if(compress==true)
	{
		bigbuffer=readfile(file1,infile, bigbuffer, compresslevel, &lastBytesNotCompressed, notCompressedBytes);
		if(file1->counter==0)
		{
			fprintf(stderr,"File is empty\n");
			return 3;
		}
		if(info==true){
			/*for(i=0; i<file1->counter; i++)
			{
				printf("Code of symbol: %d\n", bigbuffer[i]);
			}*/
			if( lastBytesNotCompressed > 0 )
			{
				int k;
				for( k=0; k<lastBytesNotCompressed; k++)
				{
					fprintf(stderr, "Not Compressed Byte: %c, %d\n", notCompressedBytes[k], notCompressedBytes[k]);
				}
			}
		}
		if(compresslevel!=0)
		{
		charinfo1=frequency(file1,bigbuffer,charinfo1);
		if(info==true){
			fprintf(stderr,"file1->length: %d | file1->counter: %d \n",file1->length,file1->counter);
	/*for(i=0; i<file1->counter; i++)
	{
		fwrite( (bigbuffer+i), 1, 1, outfile);
	}*/
		
			charInfo *iter;
			for(iter = charinfo1; iter != NULL; iter=iter->next)
			{
				fprintf(stderr,"Symbol no.%d and its frequency: %d\n", iter->value, iter->freq);
			}
			fprintf(stderr,"Distinct chars in file: %d\n", file1->distinctChars);
		}
		dict1 = makeDictionary(file1, charinfo1);
	
		if(info==true){
			dictionary *iterDictionary;
			for(iterDictionary = dict1; iterDictionary != NULL; iterDictionary=iterDictionary->next)
			{
				fprintf(stderr,"Symbol no. %d and its code %s and its size %d\n ", iterDictionary->symbol, iterDictionary->code, iterDictionary->bitLength);
			}
		}
		}
		binWrite(dict1,bigbuffer,outfile,file1->counter,compresslevel,cypher,checksum,lastBytesNotCompressed,notCompressedBytes,info);
	}
	fclose(outfile);
	if(cypher==true && compress==true)			/*TRZEBA BĘDZIE POPRAWIĆ*/
	{	
			
		xorcode(outfile,password,file1->length/2,argv[2]);
		fclose(outfile);
		
		/*THIS IS A TEST FOR IMMEDIATE DECODE*/

		/*xorcode(outfile,password,file1->length/2,argv[2]);
		fclose(outfile);*/
	}
	fclose(infile);
	if(decompress==true)
	{	
		if(cypher==true){
			xorcode(infile,password,file1->length,argv[1]);
			fclose(infile);

		}
		int returnCode = 0;
		infile=fopen(argv[1],"rb");
		/*isValid(checksum, infile);*/

		returnCode = decompressFile(infile, argv[1], argv[2], checksum, info);
		if(returnCode != 0)
		{
			/*freejowanie*/
			return returnCode;	
		}
	}
	//fclose(outfile);
	free(bigbuffer);
	free(file1);
	freecharInfo(charinfo1);
	freeDict(dict1);
	return 0;
}
