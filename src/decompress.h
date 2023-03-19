#ifndef _DECOMPRESS_H_
#define _DECOMPRESS_H_

int isValid(unsigned char checkSum, FILE *infile);


void decompressFile(FILE *infile, char *outName, char checksum);

#endif
