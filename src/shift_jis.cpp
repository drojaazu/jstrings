#include "shift_jis.h"

#include <stdint.h>
#include <string.h>

namespace encodings
{
/*
	This supports traditional Shift-JIS, which encompasses JIS X 0201 and JIS X
	0208 There is extended support for 0213, though we're not going to fiddle with
	it Maybe we'll make an extended class
*/
bool shift_jis::is_lobyte_valid(const u8 c)
{
	// sjis lower byte can never be these values
	if((c >= 0x0) & (c <= 0x3f) || (c == 0x7f) || (c >= 0xfd) & (c <= 0xff))
		return false;
	return true;
}

u8 shift_jis::is_valid(u8 *data)
{
	// JIS X 0201 - 8-bit characters (including 7-bit ASCII)
	// excludes non-printable (control code) and reserved bytes
	// (but include tab (0x09))
	u8 c_hi{*data};
	if((c_hi == 0x09) || (c_hi >= 0x20) & (c_hi <= 0x7e) || (c_hi >= 0xa1) & (c_hi <= 0xdf))
		return 1;

	// JIS X 0208 - 16 bit characters
	u8 c_lo{*(data + 1)};

	// if accurate mode, check for hi byte here
	// always check for lo byte tho:
	if(!shift_jis::is_lobyte_valid(c_lo))
		return false;

	// we've determined the second byte is valid as part of an SJIS encoded pair
	// if we're in fast mode, that's good enough; return
	// if(!accurate_mode)
	//	return true;

	// Partial ku (excluding 0x7f)
	// 0x81 - 0x40 to 0xac, 0xb8 to 0xbf, 0xc8 to 0xce, 0xda to 0xe8, 0xf0 to
	// 0xf7, 0xfc 0x82 - 0x4f to 0x58, 0x60 to 0x79, 0x81 to 0x9a, 0x9f to 0xf1
	// 0x83 - 0x40 to 0x96, 0x9f to 0xb6, 0xbf to 0xd6
	// 0x84 - 0x40 to 0x60, 0x70 to 0x91, 0x9f to 0xbe
	// 0x88 - 0x9f to 0xfc
	// 0x98 - 0x40 to 0x72, 0x9f to 0xfc
	// 0xea - 0x40 to 0xa4
	switch(c_hi) {
		case 0x81:
			if((c_lo >= 0x40) & (c_lo <= 0xac) || (c_lo >= 0xb8) & (c_lo <= 0xbf) ||
				 (c_lo >= 0xc8) & (c_lo <= 0xce) || (c_lo >= 0xda) & (c_lo <= 0xe8) ||
				 (c_lo >= 0xf0) & (c_lo <= 0xf7) || (c_lo == 0xfc))
				return 2;
			return false;
		case 0x82:
			if((c_lo >= 0x4f) & (c_lo <= 0x58) || (c_lo >= 0x60) & (c_lo <= 0x79) ||
				 (c_lo >= 0x81) & (c_lo <= 0x9a) || (c_lo >= 0x9f) & (c_lo <= 0xf1))
				return 2;
			return false;
		case 0x83:
			if((c_lo >= 0x40) & (c_lo <= 0x96) || (c_lo >= 0x9f) & (c_lo <= 0xb6) ||
				 (c_lo >= 0xbf) & (c_lo <= 0xd6))
				return 2;
			return false;
		case 0x84:
			if((c_lo >= 0x40) & (c_lo <= 0x60) || (c_lo >= 0x70) & (c_lo <= 0x91) ||
				 (c_lo >= 0x9f) & (c_lo <= 0xbe))
				return 2;
			return false;
		case 0x88:
			if((c_lo >= 0x9f) & (c_lo <= 0xfc))
				return 2;
			return false;
		case 0x98:
			if((c_lo >= 0x40) & (c_lo <= 0x72) || (c_lo >= 0x9f) & (c_lo <= 0xfc))
				return 2;
			return false;
		case 0xea:
			if((c_lo >= 0x40) & (c_lo <= 0xa4))
				return 2;
			return false;
	}
	// Full ku (0x40 to 0xfc, excluding 0x7f)
	// 0x89 to 0x97, 0x99 to 0xe9
	if((((c_hi >= 0x89) & (c_hi <= 0x97)) || ((c_hi >= 0x99) & (c_hi <= 0xe9))) &&
		 ((c_lo >= 0x40) & (c_lo <= 0xfc)))
		return 2;
	return false;
}

} // namespace encodings
