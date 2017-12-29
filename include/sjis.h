#ifndef JSTRINGS_H
#define JSTRINGS_H
#include <vector>
#include <iostream>
#include <stdint.h>

struct found_string {
	off_t address;
	std::vector<unsigned char> data;
};

std::vector<found_string>* find_sjis(std::istream* instream, unsigned long minlength, long strbufsize);
bool is_ascii(const uint8_t* c);
bool is_sjis_half_katakana(const uint8_t* c);
bool is_sjis_firstbyte(const uint8_t* c);
bool is_sjis_secondbyte(const uint8_t* c1, const uint8_t* c2);

#endif // JSTRINGS_H
