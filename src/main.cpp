#include "app.hpp"
#include "enc_cp932.hpp"
#include "enc_eucjp.hpp"
#include "enc_shiftjis.hpp"
#include "usage.hpp"

#include <exception>
#include <fstream>
#include <getopt.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <vector>

#ifdef DEBUG
#include <chrono>
#endif

using namespace std;
using namespace encodings;

// 512k of buffer
static u32 constexpr DATABUFF_SIZE { (1024 * 512) };
static u8 constexpr DEFAULT_MATCH_LEN { 10 };
static char constexpr CUTOFF_INDICATOR[] { "..." };

enum enctypes
{
	shift_jis,
	cp932,
	eucjp
};

struct runtime_config_jstrings
{
	string infile;
	string encoding;
	uint match_length { DEFAULT_MATCH_LEN };
	uint cutoff { 0 };
	bool multiline { false };
};

static const map<const string, enctypes> enclist {
	{ "shift-jis", shift_jis }, { "shiftjis", shift_jis },
	{ "sjis", shift_jis },			{ "cp932", cp932 },
	{ "windows932", cp932 },		{ "windows31j", cp932 },
	{ "euc", eucjp },						{ "euc-jp", eucjp },
	{ "eucjp", eucjp }
};

// forward declarations
void process_args(int argc, char ** argv, runtime_config_jstrings & cfg);

typedef pair<off_t, vector<uint8_t>> found_string;

int main(int argc, char ** argv)
{
	runtime_config_jstrings cfg;

	encoding * encoding { nullptr };
	encoding_shiftjis enc_sjis;
	encoding_eucjp enc_eucjp;
	encoding_cp932 enc_cp932;

	vector<found_string> results;

	istream * indata { nullptr };

	try
	{

		ifstream infile;

		// SETUP
		process_args(argc, argv, cfg);

		// sanity checks
		if(cfg.match_length < 1)
			throw invalid_argument("Match length must be a positive value");

		if(cfg.infile.empty())
			indata = &cin;
		else
		{
			infile = ifstream(cfg.infile);
			if(!infile.good())
			{
				throw invalid_argument("File could not be opened");
			}

			indata = &infile;
			indata->seekg(0);
		}

		if(cfg.encoding.empty())
			encoding = &enc_sjis;
		else
		{
			if(enclist.find(cfg.encoding) == enclist.end())
			{
				throw invalid_argument("Invalid encoding specified");
			}

			switch(enclist.at(cfg.encoding))
			{
				case enctypes::shift_jis:
					encoding = &enc_sjis;
					break;
				case enctypes::eucjp:
					encoding = &enc_eucjp;
					break;
				case enctypes::cp932:
					encoding = &enc_cp932;
					break;
				default:
					cerr << "Encoding not yet supported" << endl;
					return 3;
			}
		}

#ifdef DEBUG
		chrono::high_resolution_clock::time_point t1 =
				chrono::high_resolution_clock::now();
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
		unsigned int work_iter;
		// where we are in reading the data chunk buffer
		size_t databuff_ptr { 0 };
		// number of valid bytes returns from the encoding
		u8 validcount;
		// work string; where we dump valid bytes
		found_string workstr;
		workstr.second.reserve(cfg.match_length);
		// the databuff_ptr value when we should read another chunk
		unsigned int buffborder;
		// cache this...
		u8 enc_max_seqlen = encoding->get_max_seq_len();
		// tracks where we are in the file
		size_t stream_ptr { 0 };
		unsigned int glyphcount { 0 };
		size_t this_buffsize = DATABUFF_SIZE;
		size_t this_buffoffset { 0 };
		bool is_cutoff { false };

		char const * multiline_0d = "\\0D";
		char const * multiline_0a = "\\0A";

		while(1)
		{
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

			for(databuff_ptr = 0; databuff_ptr < bytecount;)
			{
				// check the databuff pointer
				// is it within bytecount - (encoding max_seq_len) ?
				// if so, repoint the stream and read another chunk
				if(databuff_ptr >= buffborder)
				{
					// step the stream pointer back if needed
					// indata->seekg(0 - (bytecount - databuff_ptr), ios::cur);
					// well of course we can't seekg on stdin, so let's make things more
					// complicated
					// we'll copy the remaining bytes to the beginning of the buffer
					// then have chunk reader bring in that many less bytes
					// what a mess...
					copy(&databuff[DATABUFF_SIZE - (bytecount - databuff_ptr)],
							 &databuff[DATABUFF_SIZE], &databuff[0]);
					this_buffsize = DATABUFF_SIZE - (bytecount - databuff_ptr);
					this_buffoffset = bytecount - databuff_ptr;

					// break out and reread a chunk
					break;
				}

				// the multiline implementation is kind of hack-y for now, but it will
				// work until the state machine rewrite in the next major version
				if(cfg.multiline &&
					 (databuff[databuff_ptr] == 0x0d || databuff[databuff_ptr] == 0x0a))
				{
					validcount = 1;
				}
				else
				{
					validcount = encoding->is_valid(&databuff[databuff_ptr]);
				}

				if(validcount > 0)
				{
					// the data is a valid glyph
					// add to the work string
					if(glyphcount == 0)
					{
						// this is the first character, so store the address where the
						// beginning of the string was found
						workstr.first = stream_ptr;
					}
					++glyphcount;
					if(cfg.cutoff > 0 && glyphcount > cfg.cutoff)
					{
						databuff_ptr += validcount;
						stream_ptr += validcount;
						is_cutoff = true;
						continue;
					}
					if(cfg.multiline &&
						 (databuff[databuff_ptr] == 0x0d || databuff[databuff_ptr] == 0x0a))
					{
						if(databuff[databuff_ptr] == 0x0a)
							copy(multiline_0a, multiline_0a + 3,
									 back_inserter(workstr.second));
						else
							copy(multiline_0d, multiline_0d + 3,
									 back_inserter(workstr.second));
					}
					else
					{
						copy(&databuff[databuff_ptr], &databuff[databuff_ptr + validcount],
								 back_inserter(workstr.second));
					}
					databuff_ptr += validcount;
					stream_ptr += validcount;
				}
				else
				{
					// data is invalid
					// if there are enough characters in the work string,
					// add it to the list
					if(glyphcount >= cfg.match_length)
					{
						if(is_cutoff)
						{
							copy(CUTOFF_INDICATOR,
									 CUTOFF_INDICATOR + sizeof(CUTOFF_INDICATOR),
									 back_inserter(workstr.second));
							is_cutoff = false;
						}
						workstr.second.push_back('\0');
						results.push_back(workstr);
					}
					++databuff_ptr;
					++stream_ptr;
					if(glyphcount > 0)
					{
						glyphcount = 0;
						workstr.second.clear();
						workstr.second.reserve(cfg.match_length);
					}
				}
			}
		}

#ifdef DEBUG
		chrono::high_resolution_clock::time_point t2 =
				chrono::high_resolution_clock::now();
		auto duration =
				chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

		cerr << "Search duration: " << duration << endl;

		t1 = chrono::high_resolution_clock::now();
#endif

		// RESULTS
		found_string thisstring;

		cout << showbase << internal << setfill('0') << hex;

		for(found_string this_result : results)
		{
			cout << this_result.first << " " << &this_result.second[0] << endl;
		}

		return 0;
	}
	catch(const exception & e)
	{
		cerr << "Fatal error: " << e.what() << endl;
		return -1;
	}
}

void process_args(int argc, char ** argv, runtime_config_jstrings & cfg)
{
	string const short_opts { ":hm:c:e:l" };
	vector<option> const long_opts {
		{ "help", no_argument, nullptr, 'h' },
		{ "match-length", required_argument, nullptr, 'm' },
		{ "cutoff", required_argument, nullptr, 'c' },
		{ "encoding", required_argument, nullptr, 'e' },
		{ "multiline", no_argument, nullptr, 'l' },
		{ 0, 0, 0, 0 }
	};

	vector<option_details> const opt_details {
		{ false, L"Display usage", nullptr },
		{ false,
			L"Specify number of sequential characters required to qualify as a "
			L"string ",
			nullptr },
		{ false,
			L"Specify maximum number of characters to display in a single string",
			nullptr },
		{ false, L"Specify text encoding to use", L"shiftjis|cp932|eucjp" },
		{ false, L"Do not split multiline strings", nullptr }
	};

	while(true)
	{
		const auto this_opt =
				getopt_long(argc, argv, short_opts.data(), long_opts.data(), nullptr);

		if(this_opt == -1)
			break;

		switch(this_opt)
		{
			case 'm':
				cfg.match_length = strtoul(optarg, nullptr, 10);
				break;
			case 'c':
				cfg.cutoff = strtoul(optarg, nullptr, 10);
				break;
			case 'e':
				cfg.encoding = optarg;
				break;
			case 'l':
				cfg.multiline = true;
				break;
			case 'h':
				show_usage(long_opts.data(), opt_details.data(), wcout);
				exit(0);
				break;
			case ':':
				cerr << "Missing argument" << endl;
				exit(1);
				break;
			case '?':
				cerr << "Invalid option" << endl;
				exit(1);
				break;
			default:
				show_usage(long_opts.data(), opt_details.data(), wcout);
				exit(1);
				break;
		}
	}

	if(optind < argc)
	{
		// only read the first non-option argument, assuming it is input filename
		cfg.infile = argv[optind];
	}
}
