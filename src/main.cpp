#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include "main.h"
#include "jis_enc.h"
#include "shift_jis.h"

using namespace std;

istream* instream = NULL;
size_t min_len = 0;
encodings enc = shift_jis_enc;
bool big_endian = true;
bool jisx0213 = false;

int main(int argc, char** argv)
{
	int argres = process_args(argc, argv);
	if (argres != 0) return argres;
	
	if(instream == NULL) instream = &cin;
	else instream->seekg(0);
	
	jis_enc* encoding = NULL;

	switch(enc) {
		case shift_jis_enc:
			encoding = new shift_jis(instream);
			break;
		default:
			cout << "Encoding not yet supported" << endl;
			return -1;
	}

	// set up command line values
	if(min_len > 0) encoding->set_min_len(min_len);
	encoding->set_is_big_endian(big_endian);
	encoding->set_use_jisx0213(jisx0213);
	
	vector<found_string>& res = *encoding->find();
	found_string thisstring;
	cout << showbase << internal << setfill('0') << hex;
	
	for(size_t siter = 0; siter < res.size(); siter++) {
		thisstring = res[siter];
		cout << thisstring.address << " " << &thisstring.data[0] << endl;
	}

	if(instream != &cin) delete instream;
	delete encoding;
	
	return 0;
}

int process_args(int argc, char** argv) {
	if(argc < 2) return 0;
	
	for(int argloop = 1; argloop < argc; argloop++) {
		
		// OPTION -m - set minimum length of string
		if(!strcmp(argv[argloop], "-m")) {
			if(++argloop >= argc) {
				cout << "Nothing specified for -m option" << endl;
				return 1;
			}
			min_len = strtoul(argv[argloop], NULL, 10);
			if(min_len < 1) {
				cout << "Invalid value for minimum length option" << endl;
				return 1;
			}
		}

		// OPTION -e - set encoding
		else if(!strcmp(argv[argloop], "-e")) {
			if(++argloop >= argc) {
				cout << "Nothing specified for -e option" << endl;
				return 1;
			}
			if(!strcmp(argv[argloop], "shift-jis") || !strcmp(argv[argloop], "sjis"))
				enc = shift_jis_enc;
			else {
				cout << "Invalid value for encoding option" << endl;
				return 1;
			}
		}

		// OPTION -d - prefer double-byte strings

		// OPTION -l - little-endian
		else if(!strcmp(argv[argloop], "-l")) {
			big_endian = false;
		}

		//OPTION -jisx0213 - Use JIS X 0213 character set
		else if(!strcmp(argv[argloop], "-jisx0213")) {
			jisx0213 = true;
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