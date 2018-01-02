#ifndef JIS_ENC_H
#define JIS_ENC_H
#include <iostream>
#include <vector>

struct found_string {
	off_t address;
	std::vector<unsigned char> data;
};

class jis_enc {
protected:
	std::istream* instream;
	size_t min_len = 10;
	bool is_big_endian = true;
	bool use_jisx0213 = false;

public:
	jis_enc(std::istream* instream);
	virtual ~jis_enc();
	virtual std::vector<found_string>* find() = 0;
	void set_min_len(size_t min_len);
	size_t get_min_len();
	void set_is_big_endian(bool is_big_endian);
	bool get_is_big_endian();
	void set_use_jisx0213(bool use_jisx0213);
	bool get_use_jisx0213();
};

#endif // JIS_ENC_H