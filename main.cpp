#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include "main.h"
#include "jstrings.h"

using namespace std;

// minimum length of characters to be considered a string
unsigned long minlength = 5;
// size of found string buffer
long strbufsize = 4096;
istream *instream = NULL;


int main(int argc, char **argv)
{
	int argres = process_args(argc, argv);
	if (argres != 0) return argres;
	
	if(instream == NULL) instream = &cin;
	vector<vector<char> > res = *find_sjis(instream, minlength, strbufsize);
	vector<char> thisstring;
	for(size_t siter = 0; siter < res.size(); siter++) {
		thisstring = res[siter];
		for (size_t citer = 0; citer < thisstring.size(); citer++) {
			cout << thisstring[citer];
		}
		cout << '\n';
	}
	
	delete instream;
	
	return 0;
}

int process_args(int argc, char** argv) {
	if(argc < 2) return 0;
	
	for(int argloop = 1; argloop < argc; argloop++) {
		
		// OPTION -l - set minimum length of string
		if(!strcmp(argv[argloop], "-l")) {
			if(++argloop >= argc) {
				printf("Nothing specified for -l option\n");
				return 1;
			}
			minlength = strtol(argv[argloop], NULL, 10);
			if(minlength < 1) {
				printf("Invalid value for minimum length option\n");
				return 1;
			}
		}
		
		// OPTION -b - string buffer size
		else if(!strcmp(argv[argloop], "-b")) {
			if(++argloop >= argc) {
				printf("Nothing specified for -b option\n");
				return 3;
			}
			strbufsize = strtol(argv[argloop], NULL, 10);
			if(strbufsize < 1) {
				printf("Invalid value for string buffer size\n");
				return 3;
			}
		}
		
		// OPTION filename
		else {
			ifstream* infile = new ifstream();
			infile->open(argv[argloop], ifstream::in);
			if(!infile->good()) {
				printf("File could not be opened\n");
				return 2;
			}
			instream = infile;
		}
	}
	
	return 0;
}