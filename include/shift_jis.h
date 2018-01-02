/*!
 * \brief Find Shift-JIS encoded strings in a byte stream
 * \author Damian Rogers (damian@sudden-desu.net)
 * \version 1.0
 * \date 2017.12.30
 * \copyright GNU Public License
*/
#ifndef SHIFT_JIS_H
#define SHIFT_JIS_H
#include <vector>
#include <iostream>
#include <stdint.h>
#include "jis_enc.h"

class shift_jis : public jis_enc {
	static bool is_byte2_valid(const uint8_t* c);

public:
	shift_jis(std::istream* instream) : jis_enc(instream) {};
	~shift_jis();
	std::vector<found_string>* find();
	bool get_use_jisx0213();
	void set_use_jisx0213(bool use_jisx0213);
	static bool is_jisx0201_printable(const uint8_t* c);
	static bool is_jisx0208(const uint8_t* c_h, const uint8_t* c_l);
	static bool is_jisx0213(const uint8_t* c_h, const uint8_t* c_l);
};

#endif // SHIFT_JIS_H
