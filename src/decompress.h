#ifndef _DECOMPRESS_H_
#define _DECOMPRESS_H_
#include <stdbool.h>


int isValid(unsigned char checkSum, FILE *infile);
int decompressFile(FILE *infile, char *inName, char *outName, char checksum, bool info);
#endif
