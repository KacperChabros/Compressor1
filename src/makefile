a.out: main.o readFile.o huffman.o compress.o cypher.o
	 $(CC) $^ -lm
main.o: readFile.h huffman.h compress.h
cypher.o: cypher.h
compress.o: huffman.h compress.h
huffman.o: readFile.h huffman.h
readFile.o: readFile.h
