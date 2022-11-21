#include "enc_cp932.hpp"
#include "enc_eucjp.hpp"
#include "enc_shiftjis.hpp"
#include "fstreams.hpp"
#include "jstrings.hpp"
#include "uniconv.hpp"
#include "usage.hpp"
#include <getopt.h>
#include <iconv.h>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef DEBUG
#include <chrono>
#endif

using namespace std;
using namespace motoi;

static char constexpr CUTOFF_INDICATOR[] {"..."};

struct runtime_config_jstrings
{
public:
	string input_path;
	string encoding {"shiftjis"};
	size_t match_length {default_match_length};
	size_t cutoff {0};
	bool multiline {false};
	bool raw {false};
	bool skip_jis0201 {false};
} cfg;

void process_args(int argc, char ** argv);

int main(int argc, char ** argv)
{
#ifdef DEBUG
	chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
#endif
	istream * indata {nullptr};
	ifstream infile;

	shiftjis_validator shiftjis_valid;
	cp932_validator cp932_valid;
	eucjp_validator eucjp_valid;

	// clang-format off
	unordered_map<string, jis_validator *> validators
	{
		{"shiftjis", &shiftjis_valid}, {"shift-jis", &shiftjis_valid}, {"sjis", &shiftjis_valid},
		{"cp932", &cp932_valid}, {"windows932", &cp932_valid}, {"windows31j", &cp932_valid},
		{"eucjp", &cp932_valid}, {"euc-jp", &cp932_valid}
	};
	// clang-format on

	if (validators.count(cfg.encoding) == 0)
	{
		throw invalid_argument("Invalid encoding specified");
	}

	try
	{
		process_args(argc, argv);

		if (cfg.input_path.empty())
			indata = &cin;
		else
		{
			try
			{
				infile = ifstream_checked(cfg.input_path);
			}
			catch (system_error const & sys_ex)
			{
				cerr << sys_ex.what() << ": " << sys_ex.code().message() << endl;
				return -1;
			}
			indata = &infile;
		}

#ifdef DEBUG
		chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
		auto duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

		cerr << "SETUP: " << duration << "ms" << endl;
#endif

#ifdef DEBUG
		t1 = chrono::high_resolution_clock::now();
#endif

		jis_validator * validator {validators[cfg.encoding]};
		validator->include_crlf(cfg.multiline);
		validator->skip_jis0201(cfg.skip_jis0201);
		uniconv conv(validator->iconv_code());

		auto found_strings = find(*indata, *validator, cfg.match_length);
#ifdef DEBUG
		t2 = chrono::high_resolution_clock::now();
		duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

		cerr << "VALIDATION & COPY: " << duration << "ms" << endl;
#endif

#ifdef DEBUG
		t1 = chrono::high_resolution_clock::now();
#endif
		cout << showbase << internal << setfill('0') << hex;

		size_t counter = 0;
		string as_utf8;
		for (auto & this_string : found_strings)
		{
			cout << setw(10) << this_string.first << ' ';
			if (cfg.raw)
			{
				copy(this_string.second.data(),
					this_string.second.data() + this_string.second.size(),
					ostream_iterator<byte_t>(cout));
			}
			else
			{
				as_utf8 = conv.convert(this_string.second);
				if (cfg.cutoff != 0 && as_utf8.size() > cfg.cutoff)
					cout << string({as_utf8.begin(), as_utf8.begin() + cfg.cutoff}) << CUTOFF_INDICATOR;
				else
					cout << as_utf8;
			}
			cout << endl;
			++counter;
		}
#ifdef DEBUG
		t2 = chrono::high_resolution_clock::now();
		duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

		cerr << "OUTPUT: " << duration << "ms" << endl;
#endif
	}
	catch (exception const & e)
	{
		cout << "Uncaught Exception: " << e.what() << endl;
	}
}

void process_args(int argc, char ** argv)
{
	// clang-format off
	string const short_opts {":l:c:e:mrsh"};
	vector<option> const long_opts {
		{"match-length", required_argument, nullptr, 'l'},
		{"cutoff", required_argument, nullptr, 'c'},
		{"encoding", required_argument, nullptr, 'e'},
		{"multiline", no_argument, nullptr, 'm'},
		{"raw", no_argument, nullptr, 'r'},
		{"skip-jis0201", no_argument, nullptr, 's'},
		{"help", no_argument, nullptr, 'h'},
		{nullptr, 0, nullptr, 0}
	};

	vector<option_details> const opt_details
	{
		{false, "Specify number of sequential characters required to qualify as a string ", nullptr},
		{false, "Specify maximum number of characters to display in a single string", nullptr},
		{false, "Specify text encoding to use", "shiftjis|cp932|eucjp"},
		{false, "Do not split multiline strings", nullptr},
		{false, "Output the data in its original encoding without converting to unicode", nullptr},
		{false, "Skip JIS 0201 (8-bit) bytes", nullptr},
		{false, "Display usage", nullptr}
	};
	// clang-format on

	while (true)
	{
		const auto this_opt = getopt_long(argc, argv, short_opts.data(), long_opts.data(), nullptr);

		if (this_opt == -1)
			break;

		switch (this_opt)
		{
			case 'l':
				cfg.match_length = strtoul(optarg, nullptr, 10);
				break;
			case 'c':
				cfg.cutoff = strtoul(optarg, nullptr, 10);
				break;
			case 'e':
				cfg.encoding = optarg;
				break;
			case 'm':
				cfg.multiline = true;
				break;
			case 'r':
				cfg.raw = true;
				break;
			case 's':
				cfg.skip_jis0201 = true;
				break;
			case 'h':
				show_usage(long_opts.data(), opt_details.data(), cout);
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
				show_usage(long_opts.data(), opt_details.data(), cout);
				exit(1);
				break;
		}
	}

	if (optind < argc)
	{
		// only read the first non-option argument, assuming it is input filename
		cfg.input_path = argv[optind];
	}

	// sanity checking
	if (cfg.match_length < 1)
		throw invalid_argument("Match length must be a positive value");
}
