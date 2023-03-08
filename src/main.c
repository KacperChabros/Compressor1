#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "readFile.h"
#include "huffman.h"
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
int main(int argc, char **argv)
{
	bool decompress=false,compress=false,cypher=false,info=false;/*Flagi dla parametrów: x-decompress z-compress c-cypher v-info*/
	int compresslevel=1;/*Flaga dla poziomu kompresji o1 o2 o3 o0*/
	FILE *infile,*outfile;
	char *password=NULL;
	int i,j;
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
		printf("Nie udało się otworzyć plików\n");
		return 2;
	}
	bigbuffer=readfile(file1,infile, bigbuffer);
	charinfo1=frequency(file1,bigbuffer,charinfo1);
	printf("file1->length: %d | file1->counter: %d \n",file1->length,file1->counter);
	for(i=0; i<file1->counter; i++)
	{
		fwrite( (bigbuffer+i), 1, 1, outfile);
	}
	
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
		printf("Symbol no. %d: %c and its code %s\n", iterDictionary->symbol, iterDictionary->symbol, iterDictionary->code);
	}

	fclose(infile);
	fclose(outfile);
	free(bigbuffer);
	free(file1);
	freecharInfo(charinfo1);
	return 0;
}
