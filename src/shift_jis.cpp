#include <string.h>
#include <stdint.h>
#include "jis_enc.h"
#include "shift_jis.h"

using namespace std;

shift_jis::~shift_jis() {}

bool shift_jis::is_byte2_valid(const uint8_t* c) {
	// sjis byte 2 can never be these values
		if((*c >= 0x0) & (*c <= 0x3f) ||
			(*c == 0x7f) ||
			(*c >= 0xfd) & (*c <= 0xff))
			return false;
		return true;
}

vector<found_string>* shift_jis::find() {
	
	// matching strings
	vector<found_string>* found_strings = new vector<found_string>;
	
	// character read from stream
	char tempc;
	uint8_t thisc, nextc;

	found_string this_str;
	this_str.address = -1;
	this_str.data.reserve(this->min_len);

	if(this->instream != &cin) this->instream->seekg(0);

	bool (*jisx_version)(const uint8_t*, const uint8_t*);
	if(this->use_jisx0213) jisx_version = shift_jis::is_jisx0213;
	else jisx_version = shift_jis::is_jisx0208;

	//while(!this->instream->eof()) {

	while(this->instream->get(tempc)) {
		// get first byte
		thisc = (uint8_t)tempc;
	
		// check for JIS X 0201 (single byte)
		if(this->is_jisx0201_printable(&thisc)) {
			//if(this_str.address < 0) this_str.address = (off_t)(this->instream->tellg()) - 1;
			if(this_str.address < 0) this_str.address = this->instream->tellg();
			this_str.data.push_back(thisc);
			continue;
		}
	
		// check for JIS X 0208 or 0213 (double byte)
		// get next byte
		if(!this->instream->get(tempc)) continue;
		nextc = (uint8_t)tempc;

		if((this->is_big_endian && jisx_version(&thisc, &nextc)) || jisx_version(&nextc, &thisc)) {
			this_str.data.push_back(thisc);
			this_str.data.push_back(nextc);
			continue;
		} 
		else {
			// push the read pointer back a byte
			this->instream->unget();
		}
		
		// hit an invalid byte
		if(this_str.data.size() >= this->min_len) {
			// add terminator to string
			this_str.data.push_back('\0');
			found_strings->push_back(this_str);
		}
		this_str.data.clear();
		this_str.data.reserve(this->min_len);
		this_str.address = -1;
		
	}
	//do a final check if we were in the middle of a group of valid bytes
	// todo - make this DRY
	if(this_str.data.size() >= this->min_len) {
		// add terminator to string
		this_str.data.push_back('\0');
		found_strings->push_back(this_str);
	}
	this_str.data.clear();
	this_str.data.reserve(this->min_len);
	this_str.address = -1;

	return found_strings;
}

	// JIS X 0201 - 8-bit characters (including 7-bit ASCII)
	// excludes non-printable (control code) and reserved bytes
	bool shift_jis::is_jisx0201_printable(const uint8_t* c) {
		if((*c >= 0x20) & (*c <= 0x7e) ||
			(*c >= 0xa1) & (*c <= 0xdf))
			return true;
		return false;
	}

	// JIS X 0208 - 16-bit characters
	bool shift_jis::is_jisx0208(const uint8_t* c_h, const uint8_t* c_l) {
		
		if(!shift_jis::is_byte2_valid(c_l))
			return false;

		// Partial ku (excluding 0x7f)
		// 0x81 - 0x40 to 0xac, 0xb8 to 0xbf, 0xc8 to 0xce, 0xda to 0xe8, 0xf0 to 0xf7, 0xfc
		// 0x82 - 0x4f to 0x58, 0x60 to 0x79, 0x81 to 0x9a, 0x9f to 0xf1
		// 0x83 - 0x40 to 0x96, 0x9f to 0xb6, 0xbf to 0xd6
		// 0x84 - 0x40 to 0x60, 0x70 to 0x91, 0x9f to 0xbe
		// 0x88 - 0x9f to 0xfc
		// 0x98 - 0x40 to 0x72, 0x9f to 0xfc
		// 0xea - 0x40 to 0xa4
		switch(*c_h) {
			case 0x81:
				if((*c_l >= 0x40) & (*c_l <= 0xac) ||
					(*c_l >= 0xb8) & (*c_l <= 0xbf) ||
					(*c_l >= 0xc8) & (*c_l <= 0xce) ||
					(*c_l >= 0xda) & (*c_l <= 0xe8) ||
					(*c_l >= 0xf0) & (*c_l <= 0xf7) ||
					(*c_l == 0xfc))
					return true;
				return false;
			case 0x82:
				if((*c_l >= 0x4f) & (*c_l <= 0x58) ||
					(*c_l >= 0x60) & (*c_l <= 0x79) ||
					(*c_l >= 0x81) & (*c_l <= 0x9a) ||
					(*c_l >= 0x9f) & (*c_l <= 0xf1))
					return true;
				return false;
			case 0x83:
				if((*c_l >= 0x40) & (*c_l <= 0x96) ||
					(*c_l >= 0x9f) & (*c_l <= 0xb6) ||
					(*c_l >= 0xbf) & (*c_l <= 0xd6))
					return true;
				return false;
			case 0x84:
				if((*c_l >= 0x40) & (*c_l <= 0x60) ||
					(*c_l >= 0x70) & (*c_l <= 0x91) ||
					(*c_l >= 0x9f) & (*c_l <= 0xbe))
					return true;
				return false;
			case 0x88:
				if((*c_l >= 0x9f) & (*c_l <= 0xfc))
					return true;
				return false;
			case 0x98:
				if((*c_l >= 0x40) & (*c_l <= 0x72) ||
					(*c_l >= 0x9f) & (*c_l <= 0xfc))
					return true;
				return false;
			case 0xea:
				if((*c_l >= 0x40) & (*c_l <= 0xa4))
					return true;
				return false;
		}

		// Full ku (0x40 to 0xfc, excluding 0x7f)
		// 0x89 to 0x97, 0x99 to 0xe9
		if((((*c_h >= 0x89) & (*c_h <= 0x97)) ||
			((*c_h >= 0x99) & (*c_h <= 0xe9))) &&
			((*c_l >= 0x40) & (*c_l <= 0xfc)))
			return true;

		return false;
	}

	// JIS X 0213 - 16-bit characters
	bool shift_jis::is_jisx0213(const uint8_t* c_h, const uint8_t* c_l) {

		if(!shift_jis::is_byte2_valid(c_l))
			return false;
		
		// Partial ku (excluding 0x7f)
		// 0x84 - 0x40 to 0xdc, 0xe5 to 0xfa
		// 0x86 - 0x40 to 0xf1, 0xfb to 0xfc
		// 0x87 - 0x40 to 0x76, 0x7e to 0x8f, 0x93, 0x98 to 0x99, 0x9d to 0xfc
		// 0xfc - 0x40 to 0xf4
		switch(*c_h) {
			case 0x84:
				if((*c_l >= 0x40) & (*c_l <= 0xdc) ||
					(*c_l >= 0xe5) & (*c_l <= 0xfa))
					return true;
				return false;
			case 0x86:
				if((*c_l >= 0x40) & (*c_l <= 0xf1) ||
					(*c_l >= 0xfb) & (*c_l <= 0xfc))
					return true;
				return false;
			case 0x87:
				if((*c_l >= 0x40) & (*c_l <= 0x76) ||
					(*c_l >= 0x7e) & (*c_l <= 0x8f) ||
					(*c_l == 0x93) ||
					(*c_l >= 0x98) & (*c_l <= 0x99) ||
					(*c_l >= 0x9d) & (*c_l <= 0xfc))
					return true;
				return false;
			case 0xfc:
				if((*c_l >= 0x40) & (*c_l <= 0xf4))
					return true;
				return false;
		}
		
		// Full ku (0x40 to 0xfc, excluding 0x7f)
		// 0x81 to 0x83, 0x85, 0x88 to 0xfb
		if((((*c_h >= 0x81) & (*c_h <= 0x83)) ||
			(*c_h == 0x85) ||
			((*c_h >= 0x88) & (*c_h <= 0xfb))) &&
			((*c_l >= 0x40) & (*c_l <= 0xfc)))
			return true;

		return false;
	}


// sjis information comes from JWPce tables,
// the Yamaha RT series page - http://www.rtpro.yamaha.co.jp/RT/docs/misc/kanji-sjis.html
// and the Unicode consortium conversion sheet - ftp://ftp.unicode.org/Public/MAPPINGS/OBSOLETE/EASTASIA/JIS/SHIFTJIS.TXT
// and http://www.asahi-net.or.jp/~ax2s-kmtn/ref/index.html

// Shift-JIS is an ENCODING of the JIS CHARACTER SET
// SJIS supports JIS 0208 and 0213 (not 0212)

