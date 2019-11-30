#include "main.h"

#ifdef DEBUG
#include <chrono>
#endif

using namespace std;

static const string version = string("1.1");

istream *indata = nullptr;
size_t match_len = 0;
// encoding enc = shift_jis_enc;
encoding enc = euc_enc;
string encoding_str = "";

// 512k of buffer
static const u32 DATABUFF_SIZE = 524288;
static const u8 DEFAULT_MATCH_LEN = 10;

int main(int argc, char **argv)
{
	jis_encoding *encoding = nullptr;
	vector<found_string> results;

	try {
		// SETUP
		process_args(argc, argv);

		if(indata == nullptr)
			indata = &cin;
		else {
			if(!indata->good()) {
				cerr << "File could not be opened" << endl;
				return 2;
			}
			indata->seekg(0);
		}

		switch(enc) {
			case shift_jis_enc:
				encoding = new encodings::shift_jis();
				break;
			case euc_enc:
				encoding = new encodings::euc();
				break;
			default:
				cerr << "Encoding not yet supported" << endl;
				return 3;
		}

		if(match_len < 1)
			match_len = DEFAULT_MATCH_LEN;

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
		vector<found_string> found_strings();
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

			// and cache this too...
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

/*
psudo code planning time

byte buffer - say 512k in size
read 512k from file into buffer
check for the number of bytes actually read
(http://www.cplusplus.com/reference/istream/istream/read/ - gcount member)
this_buf_size = num bytes read

each encoding has a max byte size - sjis 2 bytes, euc 3 bytes, etc

maintain offset counter inside buffer array
pass pointer to next data inside array to encoding->is_valid

if this_buf_size - current buffer offset is less than max byte size for
encoding, re-read  the buffer from that point


*/
/*
vector<found_string> *find_strings(istream &instream, jis_encoding &encoding,
																	 jis_charset charset)
{
	// list of found strings
	vector<found_string> *found_strings = new vector<found_string>;

	u8 buffer[DATABUFF];

	// work bytes
	char tempc;
	u8 this_c, next_c;

	// work string
	found_string this_str;
	this_str.address = -1;
	// this_str.data.reserve(this->match_len);

	streamsize buff_len;
	off_t buff_ptr{0};
	int remaining{0};
	u8 max_seq_len = encoding.get_max_seq_len();
	u8 valid_count;
	u8 iter;
	while(1) {
		buff_len = instream.read((char *)buffer, DATABUFF).gcount();
		buff_ptr = 0;
		remaining = buff_len;
		// process work here
		while(remaining >= max_seq_len) {
			valid_count = encoding.is_valid(buffer + buff_ptr);
			if(valid_count) {
				buff_ptr += valid_count;
				for(iter = 0; iter < valid_count; ++iter) {
					this_str.data.push_back(buffer[buff_ptr + iter]);
				}

			} else {
				++buff_ptr;
				// hit an invalid byte
				// are there enough character matches to make a string?
				if(this_str.data.size() >= this->match_len) {
					// add terminator to string
					this_str.data.push_back('\0');
					found_strings->push_back(this_str);
				}
			}
			remaining = buff_len - buff_ptr;
		}

		// if we're already past the end of the file, we're done here
		if(instream.eof()) {
			break;
		}

		// re-read the buffer from the point where we stopped
		instream.seekg(-(buff_ptr - (buff_len - buff_ptr)), std::ios_base::cur);
	}

	// stream read loop
	while(instream.get(tempc)) {
		this_c = (u8)tempc;

		// ---- CHECK #1 - 8-BIT VALIDITY (JIS X 0201)
		if(encoding.is_jisx0201(&this_c)) {
			// subtract two from tellg address to account for the posistion of the
			// stream pointer (which is after the 'current' byte)
			if(this_str.address < 0)
				this_str.address = (instream.tellg() - (streampos)1);
			this_str.data.push_back(this_c);
			continue;
		}

		// ---- CHECK #2 - 16-BIT VALIDITY (JIS X 0208, 0212, 0213)
		// first byte was valid, let's check the next one
		// get next byte
		if(!instream->get(tempc))
			continue;
		next_c = (uint8_t)tempc;

		if((this->is_big_endian && jisx_version(this_c, next_c, this->accurate)) ||
			 jisx_version(next_c, this_c, this->accurate)) {
			// subtract two from tellg address to account for the posistion of the
			// stream pointer (which is after the 'current' byte) and for next_c that
			// was read from the stream
			if(this_str.address < 0)
				this_str.address = (instream->tellg() - (streampos)2);
			this_str.data.push_back(this_c);
			this_str.data.push_back(next_c);
			continue;
		} else {
			// push the read pointer back a byte
			instream->unget();
		}

		// hit an invalid byte
		// are there enough character matches to make a string?
		if(this_str.data.size() >= this->match_len) {
			// add terminator to string
			this_str.data.push_back('\0');
			found_strings->push_back(this_str);
		}
		this_str.data.clear();
		this_str.data.reserve(this->match_len);
		this_str.address = -1;
	}
	// do a final check if we were in the middle of a group of valid bytes
	// todo - make this DRY
	if(this_str.data.size() >= this->match_len) {
		// add terminator to string
		this_str.data.push_back('\0');
		found_strings->push_back(this_str);
	}
	this_str.data.clear();
	this_str.data.reserve(this->match_len);
	this_str.address = -1;

	return found_strings;
}
*/

void process_args(int argc, char **argv)
{
	const char *const short_opts = ":hm:e:lxf";
	const option long_opts[] = {{"help", no_argument, nullptr, 'h'},
															{"min-length", required_argument, nullptr, 'm'},
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
	cerr << "jstrings version " << version << endl << endl;
	cerr << "Valid options:" << endl;
	cerr << "  --encoding, -e     Specify encoding to use" << endl;
}
