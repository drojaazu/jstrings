
#include "main.h"
#ifdef DEBUG
#include <chrono>
#endif

using namespace std;

const string version = string("1.0");

istream *indata = nullptr;
size_t match_len = 0;
encodings enc = shift_jis_enc;
bool big_endian = true;
bool jisx0213 = false;
bool accurate_mode = true;

int main(int argc, char **argv) {
  jis_encoding *encoding = nullptr;
  vector<found_string> *results = nullptr;

  try {
    process_args(argc, argv);

    if (indata == nullptr)
      indata = &cin;
    else {
      if (!indata->good()) {
        cerr << "File could not be opened" << endl;
        return 2;
      }
      indata->seekg(0);
    }

    switch (enc) {
      case shift_jis_enc:
        encoding = new shift_jis();
        break;
      default:
        cerr << "Encoding not yet supported" << endl;
        return 3;
    }

    // set up command line values
    if (match_len > 0) encoding->match_len = match_len;
    encoding->is_big_endian = big_endian;
    encoding->use_jisx0213 = jisx0213;
    encoding->accurate = accurate_mode;
#ifdef DEBUG
    std::chrono::high_resolution_clock::time_point t1 =
        std::chrono::high_resolution_clock::now();
#endif
    auto results = encoding->find(indata);
#ifdef DEBUG
    std::chrono::high_resolution_clock::time_point t2 =
        std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    cerr << "Search duration: " << duration << endl;

    t1 = std::chrono::high_resolution_clock::now();
#endif
    found_string thisstring;

    cout << showbase << internal << setfill('0') << hex;

    for (size_t siter = 0; siter < results->size(); siter++) {
      thisstring = results->at(siter);
      cout << thisstring.address << " " << &thisstring.data[0] << endl;
    }

    if (indata != &cin) delete indata;
    delete encoding;
    delete results;

    return 0;
  } catch (const exception &e) {
    cerr << "Fatal error: " << e.what() << endl;

    if (indata != &cin) delete indata;
    delete encoding;
    delete results;

    return -1;
  }
}

void process_args(int argc, char **argv) {
  // TODO: add option for double-byte strings bias
  // OPTION -d - prefer double-byte strings

  const char *const short_opts = ":hm:e:lxf";
  const option long_opts[] = {{"help", no_argument, nullptr, 'h'},
                              {"min-length", required_argument, nullptr, 'm'},
                              {"encoding", required_argument, nullptr, 'e'},
                              {"little-endian", no_argument, nullptr, 'l'},
                              {"jisx0213", no_argument, nullptr, 'x'},
                              {"fast", no_argument, nullptr, 'f'},
                              {nullptr, 0, nullptr, 0}};

  while (true) {
    const auto this_opt =
        getopt_long(argc, argv, short_opts, long_opts, nullptr);

    if (this_opt == -1) break;

    switch (this_opt) {
      case 'm':
        // OPTION -m - set minimum length of string
        match_len = strtoul(optarg, nullptr, 10);
        break;
      case 'e':
        // OPTION -e - set encoding
        // TODO: move encodings into a map
        if (!strcmp(optarg, "shift-jis") || !strcmp(optarg, "sjis"))
          enc = shift_jis_enc;
        else {
          cerr << "Unsupported encoding, defaulting to Shift-JIS" << endl;
        }
        break;
      case 'l':
        // OPTION -l - little-endian
        big_endian = false;
        break;
      case 'x':
        // OPTION -jisx0213 - Use JIS X 0213 character set
        jisx0213 = true;
        break;
      case 'f':
        // OPTION -fast - Use fast mode
        accurate_mode = false;
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

  if (optind < argc) {
    // only read the first non-option argument, assuming it is input filename
    indata = new ifstream(argv[optind]);
  }
}

void print_help() {
  cerr << "jstrings version " << version << endl << endl;
  cerr << "Valid options:" << endl;
  cerr << "  --encoding, -e     Specify encoding to use" << endl;
  cerr << "  --jisx0213         Include JIS X 0213 definitions" << endl;
  cerr << "  --help, -h         Display this text" << endl;
}
