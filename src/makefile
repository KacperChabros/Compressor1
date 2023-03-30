skcomp: main.o readFile.o huffman.o compress.o cypher.o decompress.o trie.o
	 $(CC) $^ -lm -o skcomp
main.o: readFile.h huffman.h compress.h
decompress.o: huffman.h compress.h decompress.h trie.h
trie.o: trie.h huffman.h
cypher.o: cypher.h
compress.o: huffman.h compress.h
huffman.o: readFile.h huffman.h
readFile.o: readFile.h
