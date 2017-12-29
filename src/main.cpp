#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include "main.h"
#include "sjis.h"

using namespace std;

// minimum length of characters to be considered a string
unsigned long minlength = 5;

// size of found string buffer
long strbufsize = 1024;
istream *instream = NULL;


int main(int argc, char **argv)
{
	int argres = process_args(argc, argv);
	if (argres != 0) return argres;
	
	if(instream == NULL) instream = &cin;
	vector<found_string> res = *find_sjis(instream, minlength, strbufsize);
	found_string thisstring;
	cout << showbase << internal << setfill('0') << hex;
	
	for(size_t siter = 0; siter < res.size(); siter++) {
		thisstring = res[siter];
		cout << thisstring.address << " " << &thisstring.data[0] << endl;
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
				cout << "Nothing specified for -l option" << endl;
				return 1;
			}
			minlength = strtol(argv[argloop], NULL, 10);
			if(minlength < 1) {
				cout << "Invalid value for minimum length option" << endl;
				return 1;
			}
		}
		
		// OPTION -b - string buffer size
		else if(!strcmp(argv[argloop], "-b")) {
			if(++argloop >= argc) {
				cout << "Nothing specified for -b option" << endl;
				return 3;
			}
			strbufsize = strtol(argv[argloop], NULL, 10);
			if(strbufsize < 1) {
				cout << "Invalid value for string buffer size" << endl;
				return 3;
			}
		}
		
		// OPTION filename
		else {
			ifstream* infile = new ifstream();
			infile->open(argv[argloop], ifstream::binary);
			if(!infile->good()) {
				cout << "File could not be opened" << endl;
				return 2;
			}
			instream = infile;
		}
	}
	
	return 0;
}