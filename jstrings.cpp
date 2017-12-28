#include <string.h>
#include <stdint.h>
#include "jstrings.h"

using namespace std;


vector<vector<char> >* find_sjis(istream* instream, unsigned long minlength, long strbufsize) {
	vector<vector<char> >* found_strings = new vector<vector<char> >(0);
	
	//found string vector
	//	read first byte from stream
	//	isAscii or isHalfKatakana? if yes, add to vector, increase char found count, move to next byte
	//	isSjisFirstByte?
	//		if yes, check for eof, read next byte
	//			isSjisSecondByte? if yes, add both bytes to string vector, increase char found count, move to next byte
	//		if no, move stream pointer back
	//		if eof, do nothing
	//	if we're here, no valid bytes found or eof; have we found enough characters?
	//		if yes, add found chars vector to string list vector, reset char found count
	//		if no, reset char found count, jump back up
	
	instream->seekg(0);
	
	// matching characters
	vector<char> found_chars (0); // set default to minlength
	// character read from stream
	char t_c;
	uint8_t thisc, nextc;
	
	// count for number of found characters (not bytes!)
	unsigned long found_count = 0;
	
	// current position in stream
	int currpos = -1;
	
	while(!instream->eof()) {
		// get first byte
		instream->get(t_c); thisc = (uint8_t)t_c;
		
		// check for single byte validity
		if(is_ascii(&thisc) || is_sjis_half_katakana(&thisc)) {
			if(currpos < 0) currpos = instream->tellg();
			found_count++;
			found_chars.push_back(thisc);
			continue;
		}
		
		// check for first byte of sjis sequence
		if(is_sjis_firstbyte(&thisc)) {
			// todo - check for eof here
			if(currpos < 0) currpos = instream->tellg();
			instream->get(t_c); nextc = (uint8_t)t_c;
			if(is_sjis_secondbyte(&thisc, &nextc)) {
				found_chars.push_back(thisc);
				found_chars.push_back(nextc);
				found_count++;
				continue;
			}
			else {
				// push the read pointer back a byte
				instream->unget();
			}
		}
		
		// hit an invalid byte
		if(found_count >= minlength) {
			char temp_addr1[20];
			size_t numchars = sprintf(temp_addr1, "%#010x ", (unsigned int)instream->tellg());
			char temp_addr2[numchars+1];
			strncpy(temp_addr2, temp_addr1, numchars);
			//found_chars.insert(0, temp_addr2);
			
			//size_t tempsz = found_chars.size();
			//char final_str [tempsz + numchars];
			//strcpy(final_str, temp_addr2);
			//strcat(final_str, &found_chars[0]);
			found_strings->push_back(found_chars);\
		}
		found_chars.clear();
		currpos = -1;
		found_count = 0;
	}
	
	// do a final check if we were in the middle of a group of valid bytes
	// todo - make this DRY
	if(found_count >= minlength) {
		char temp_addr1[20];
		size_t numchars = sprintf(temp_addr1, "%#010x ", (unsigned int)instream->tellg());
		char temp_addr2[numchars+1];
		strncpy(temp_addr2, temp_addr1, numchars);
		//found_chars.insert(0, temp_addr2);
		
		//size_t tempsz = found_chars.size();
		//char final_str [tempsz + numchars];
		//strcpy(final_str, temp_addr2);
		//strcat(final_str, &found_chars[0]);
		found_strings->push_back(found_chars);
	}
	found_chars.clear();
	currpos = -1;
	found_count = 0;
	
	return found_strings;
}

bool is_ascii(const uint8_t* c) {
	if((*c >= 0x20) & (*c <= 0x7d))
		return true;
	return false;
}

bool is_sjis_half_katakana(const uint8_t* c) {
	if((*c >= 0xa1) & (*c <= 0xdf))
		return true;
	return false;
}

// JIS X 0201 - 8-bit characters
// compatible with 7-bit ASCII
// returns true if the character is printable
// (i.e. not a control character and not in the 'reserved' ranges)
bool is_jis0201_printable(const uint8_t* c) {
	if((*c >= 0x20) & (*c <= 0x7e) ||
		(*c >= 0xa1) & (*c <= 0xdf))
		return true;
	return false;
}

// sjis information comes from JWPce tables,
// the Yamaha RT series page - http://www.rtpro.yamaha.co.jp/RT/docs/misc/kanji-sjis.html
// and the Unicode consortium conversion sheet - ftp://ftp.unicode.org/Public/MAPPINGS/OBSOLETE/EASTASIA/JIS/SHIFTJIS.TXT
// and http://www.asahi-net.or.jp/~ax2s-kmtn/ref/index.html

// Shift-JIS is an ENCODING of the JIS CHARACTER SET
// SJIS supports JIS 0208 and 0213 (not 0212)

bool is_sjis_firstbyte(const uint8_t* c) {
	// with JIS 0213/vendor extensions, include 0x85 to 0x87, and 0xeb to 0xef
	
	// first byte will always have highest bit set
	if(*c >> 8) return false;
	
	if(((*c >= 0x81) & (*c <= 0x84)) ||
		((*c >= 0x88) & (*c <= 0x9f)) ||
		((*c >= 0xe0) & (*c <= 0xea)))
			return true;
	return false;
}

bool is_sjis_secondbyte(const uint8_t* c1, const uint8_t* c2) {
	// 0x81 series - 0x40 to 0xac, 0xb8 to 0xbf, 0xc8 to 0xce, 0xda to 0xe8, 0xf0 to 0xf7, 0xfc
	// 0x82 series - 0x4f to 0x58, 0x60 to 0x79, 0x81 to 0x9a, 0x9f to 0xf1
	// 0x83 series - 0x40 to 0x96, 0x9f to 0xb6, 0xbf to 0xd6
	// 0x84 series - 0x40 to 0x60, 0x70 to 0x91, 0x9f to 0xbe
	// 0x88 series - 0x9f to 0xfc
	// 0x89 to 0x97 series - 0x40 to 0xfc
	// 0x98 series - 0x40 to 0x72, 0x9f to 0xfc
	// 0x99 to 0xe9 series - 0x40 to 0xfc
	// 0xea series - 0x40 to 0xa4
	
	// with JIS 0213/vendor extensions, include 0x85 to 0x87, and 0xeb to 0xef in FIRST BYTE validity
	// unsure about second byte of extensions, assume 0x40 to 0xfc ?
	
	// assume first byte has already been checked for validity
	switch(*c1) {
		case 0x81:
			if((*c2 >= 0x40) & (*c2 <= 0xac) ||
				(*c2 >= 0xb8) & (*c2 <= 0xbf) ||
				(*c2 >= 0xc8) & (*c2 <= 0xce) ||
				(*c2 >= 0xda) & (*c2 <= 0xe8) ||
				(*c2 >= 0xf0) & (*c2 <= 0xf7) ||
				(*c2 == 0xfc))
				return true;
			return false;
		case 0x82:
			if((*c2 >= 0x4f) & (*c2 <= 0x58) ||
				(*c2 >= 0x60) & (*c2 <= 0x79) ||
				(*c2 >= 0x81) & (*c2 <= 0x9a) ||
				(*c2 >= 0x9f) & (*c2 <= 0xf1))
				return true;
			return false;
		case 0x83:
			if((*c2 >= 0x40) & (*c2 <= 0x96) ||
				(*c2 >= 0x9f) & (*c2 <= 0xb6) ||
				(*c2 >= 0xbf) & (*c2 <= 0xd6))
				return true;
			return false;
		case 0x84:
			if((*c2 >= 0x40) & (*c2 <= 0x60) ||
				(*c2 >= 0x70) & (*c2 <= 0x91) ||
				(*c2 >= 0x9f) & (*c2 <= 0xbe))
				return true;
			return false;
		case 0x88:
			if((*c2 >= 0x9f) & (*c2 <= 0xfc))
				return true;
			return false;
		case 0x98:
			if((*c2 >= 0x40) & (*c2 <= 0x72) ||
				(*c2 >= 0x9f) & (*c2 <= 0xfc))
				return true;
			return false;
		case 0xea:
			if((*c2 >= 0x40) & (*c2 <= 0xa4))
				return true;
			return false;
	}
	
	// 0x89 to 0x97, 0x99 to 0xe9 series - 0x40 to 0xfc
	if((*c1 >= 0x89) & (*c1 <= 0x97) ||
		(*c1 >= 0x99) & (*c1 <= 0xe9) &
		((*c2 >= 0x40) & (*c2 <= 0xfc)))
		return true;
	
	return false;
}