#include "main.h"
#include "project.hpp"

#ifdef DEBUG
#include <chrono>
#endif

using namespace std;

// 512k of buffer
static u32 const DATABUFF_SIZE = 524288;
static u8 const DEFAULT_MATCH_LEN = 10;

istream *indata = nullptr;
size_t match_len = DEFAULT_MATCH_LEN;
size_t str_cutoff{0};
string encoding_str = "";

static const map<const string, enctypes> enclist{
		{"shift-jis", shift_jis}, {"shiftjis", shift_jis}, {"sjis", shift_jis},
		{"cp932", cp932},					{"windows932", cp932},	 {"windows31j", cp932},
		{"euc", eucjp},						{"euc-jp", eucjp},			 {"eucjp", eucjp}};

int main(int argc, char **argv)
{
	encoding *encoding = nullptr;
	vector<found_string> results;

	try {
		// SETUP
		process_args(argc, argv);

		if(indata == nullptr)
			indata = &cin;
		else {
			if(!indata->good()) {
				throw invalid_argument("File could not be opened");
			}
			indata->seekg(0);
		}

		if(encoding_str.empty())
			encoding = new encodings::shift_jis();
		else {
			if(enclist.find(encoding_str) == enclist.end()) {
				throw invalid_argument("Invlaid encoding specified");
			}

			switch(enclist.at(encoding_str)) {
				case shift_jis:
					encoding = new encodings::shift_jis();
					break;
				case eucjp:
					encoding = new encodings::euc();
					break;
				case cp932:
					encoding = new encodings::cp932();
					break;
				default:
					cerr << "Encoding not yet supported" << endl;
					return 3;
			}
		}

#ifdef DEBUG
		std::chrono::high_resolution_clock::time_point t1 =
				std::chrono::high_resolution_clock::now();
#endif

		// SEARCH
		// - read buffer chunk
		// - pass pointer to chunk offset to is_valid
		// - if return value > 0
		// -- add (return value) chars to current string
		// -- move buffer point +(return value)
		// - if return value <= 0
		// -- are there enough chars in our temp string to count as a found string?
		// --- if yes, add string to list
		// -- clear temp string
		// start over
		vector<found_string> found_strings;
		u8 databuff[DATABUFF_SIZE];
		streamsize bytecount;
		u32 work_iter;
		// where we are in reading the data chunk buffer
		u32 databuff_ptr{0};
		// number of valid bytes returns from the encoding
		u8 validcount;
		// work string; where we dump valid bytes
		found_string workstr;
		workstr.data.reserve(match_len);
		// the databuff_ptr value when we should read another chunk
		u32 buffborder;
		// cache this...
		u8 enc_max_seqlen = encoding->get_max_seq_len();
		// tracks where we are in the file
		u64 stream_ptr{0};
		s16 glyphcount{0};
		u32 this_buffsize = DATABUFF_SIZE;
		u32 this_buffoffset{0};

		while(1) {
			if(indata->eof())
				break;
			// read a chunk and count how many bytes were actually captured
			bytecount =
					indata->read((char *)(databuff + this_buffoffset), this_buffsize)
							.gcount();
			if(bytecount < 1)
				break;

			// cache this too...
			buffborder = bytecount - enc_max_seqlen;

			for(databuff_ptr = 0; databuff_ptr < bytecount;) {
				// check the databuff pointer
				// is it within bytecount - (encoding max_seq_len) ?
				// if so, repoint the stream and read another chunk
				if(databuff_ptr >= buffborder) {
					// step the stream pointer back if needed
					// indata->seekg(0 - (bytecount - databuff_ptr), ios::cur);
					// well of course we can't seekg on stdin, so let's make things more
					// complicated
					// we'll copy the remaining bytes to the beginning of the buffer
					// then have chunk reader bring in that many less bytes
					// what a mess...
					std::copy(&databuff[DATABUFF_SIZE - (bytecount - databuff_ptr)],
										&databuff[DATABUFF_SIZE], &databuff[0]);
					this_buffsize = DATABUFF_SIZE - (bytecount - databuff_ptr);
					this_buffoffset = bytecount - databuff_ptr;

					// break out and reread a chunk
					break;
				}

				validcount = encoding->is_valid(&databuff[databuff_ptr]);
				if(validcount > 0) {
					// the data is a valid glyph
					// add to the work string
					if(glyphcount == 0) {
						// this is the first character, so store the address where the
						// beginning of the string was found
						workstr.address = stream_ptr;
					}
					glyphcount++;
					if(str_cutoff > 0 && glyphcount >= str_cutoff) {
						databuff_ptr += validcount;
						stream_ptr += validcount;
						continue;
					}
					std::copy(&databuff[databuff_ptr],
										&databuff[databuff_ptr + validcount],
										std::back_inserter(workstr.data));
					databuff_ptr += validcount;
					stream_ptr += validcount;
				} else {
					// data is invalid
					// if there are enough characters in the work string, add it to the
					// list
					if(glyphcount >= match_len) {
						workstr.data.push_back('\0');
						results.push_back(workstr);
					}
					++databuff_ptr;
					++stream_ptr;
					if(glyphcount > 0) {
						glyphcount = 0;
						workstr.data.clear();
						workstr.data.reserve(match_len);
					}
				}
			}
		}

#ifdef DEBUG
		std::chrono::high_resolution_clock::time_point t2 =
				std::chrono::high_resolution_clock::now();
		auto duration =
				std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

		cerr << "Search duration: " << duration << endl;

		t1 = std::chrono::high_resolution_clock::now();
#endif

		// RESULTS
		found_string thisstring;

		std::cout << showbase << internal << setfill('0') << hex;

		for(size_t siter = 0; siter < results.size(); siter++) {
			thisstring = results.at(siter);
			std::cout << thisstring.address << " " << &thisstring.data[0] << endl;
		}

		if(indata != &cin)
			delete indata;
		delete encoding;
		// delete results;

		return 0;
	} catch(const exception &e) {
		cerr << "Fatal error: " << e.what() << endl;

		if(indata != &cin)
			delete indata;
		delete encoding;
		// delete results;

		return -1;
	}
}

void process_args(int argc, char **argv)
{
	const char *const short_opts = ":hm:e:lxf";
	const option long_opts[] = {{"help", no_argument, nullptr, 'h'},
															{"match-length", required_argument, nullptr, 'm'},
															{"cutoff", required_argument, nullptr, 'c'},
															{"encoding", required_argument, nullptr, 'e'},
															{nullptr, 0, nullptr, 0}};

	while(true) {
		const auto this_opt =
				getopt_long(argc, argv, short_opts, long_opts, nullptr);

		if(this_opt == -1)
			break;

		switch(this_opt) {
			case 'm':
				match_len = strtoul(optarg, nullptr, 10);
				if(match_len < 1)
					throw invalid_argument("Match length must be a positive value");
				break;
			case 'c':
				str_cutoff = strtoul(optarg, nullptr, 10);
				if(str_cutoff < 1)
					throw invalid_argument("Max length must be a positive value");
				break;
			case 'e':
				encoding_str = argv[optind];
				break;
			case 'h':
				print_help();
				exit(0);
				break;
			case ':':
				cerr << "Missing argument" << endl;
				print_help();
				exit(1);
				break;
			case '?':
				cerr << "Invalid option" << endl;
				print_help();
				exit(1);
				break;
			default:
				print_help();
				exit(1);
				break;
		}
	}

	if(optind < argc) {
		// only read the first non-option argument, assuming it is input filename
		indata = new ifstream(argv[optind]);
	}
}

void print_help()
{
	cerr << PROJECT::PROJECT_NAME << " - ver. " << PROJECT::VERSION << endl << endl;
	cerr << "Valid options:" << endl;
	cerr << "  --encoding, -e         Specify encoding to use" << endl;
	cerr << "         (Valid options: shiftjis, cp932, eucjp)" << endl;
	cerr << "  --match-length, -m     Specify number of sequential characters "
					"required to qualify as a string"
			 << endl;
	cerr << "  --cutoff, -c       Specify maximum number of characters to "
					"display in a single string"
			 << endl;
}
