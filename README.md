# Compressor
"Skcomp" is a compressor and a decompressor for any kind of file. It implements Huffman's algorithm. Skcomp was created with my friend as a university project. It allows user to choose from 4 levels of compression and cypher the file with desired password. This program is aimed at Linux users.
## Technologies
* C
* makefile
## Features
* compress file with one out of 4 compression levels
    * Level 0 - no compression
    * Level 1 - 8-bit compression
    * Level 2 - 12-bit compression
    * Level 3 - 16-bit compression (unfortunately some issues appeared - might not work properly)
* cypher file with desired password using XOR algorithm
* decompress and decypher compressed file
* check if the file is valid using checksum
* display additional information during the process
## Setup and usage
To run this project one should clone the repository, go to the folder with the repository using terminal and type "make". "Help" of skcomp is written below:
Correct usage:  
**_./skcomp <infile> <outfile> [parameters]_**  
	Infile: Input file  
	Outfile: Output file  
    **Parameters:**  
	-x Decompress input file. If already decompressed display error.  
	-z Compress input file.  
    -o<0,1,2,3> Level of compression.  
    o0 - no compression  
    o1 - 8-bit compresssion  
    o2 - 12-bit compression  
    o3 - 16-bit compression  
    Default level is 1  
    -c <password> Cypher input file with the given password. This parameter must be given separately or at the end of other parameters.  
    -v Display additional information.  
	Note: -x and -z cannot be used together.  
## Credits
I created this project with:
* [Grazzly](https://github.com/Grazzly)
