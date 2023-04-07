#ifndef _CYPHER_H_
#define _CYPHER_H_
int xorcode(FILE *outfile,char *key,int length,char *file,int compresslevel);
int xorfile(FILE *outfile,char *key,int length, char *file,unsigned char checksum,char *tmpname);
#endif
