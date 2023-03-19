a.out: main.o readFile.o huffman.o compress.o cypher.o decompress.o
	 $(CC) $^ -lm
main.o: readFile.h huffman.h compress.h
decompress.o: huffman.h compress.h decompress.h
cypher.o: cypher.h
compress.o: huffman.h compress.h
huffman.o: readFile.h huffman.h
readFile.o: readFile.h
