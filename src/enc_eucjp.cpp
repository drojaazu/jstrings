#include "enc_eucjp.hpp"

namespace motoi
{

uint eucjp_validator::is_valid(byte_t const * data) const
{

	u8 c_hi {*data};

	// ASCII except control characters (including tab and conditionally newlines)
	if ((c_hi == 0x09) || ((c_hi >= 0x20) & (c_hi <= 0x7e)) || (m_include_crlf && (c_hi == 0x0a)))
		return 1;

	u8 c_lo {*(data + 1)};

	if (m_include_crlf && ((c_hi == 0x0d) && (c_lo == 0x0a)))
		return 2;

	/*
		JIS X 0201
		the raw 0201 code prefixed with 0x8E
	*/
	if (! m_skip_jis0201 && (c_hi == 0x8e))
	{
		if ((c_lo >= 0xa1) & (c_lo <= 0xdf))
			return 2;
	}
	/*
		JIS X 0208
		standard 0208 code with top bit set
		Partial ku
		0xa2 - 0xa1 to 0xae, 0xba to 0xc1, 0xca to 0xd0, 0xdc to 0xea, 0xf2 to 0xf9, 0xfe
		0xa3 - 0xb0 to 0xb9, 0xc1 to 0xda, 0xe1 to 0xfa
		0xa4 - 0xa1 to 0xf3
		0xa5 - 0xa1 to 0xf6
		0xa6 - 0xa1 to 0xb8, 0xc1 to 0xd8
		0xa7 - 0xa1 to 0xc1, 0xd1 to 0xf1
		0xa8 - 0xa1 to 0xc0
		0xcf - 0xa1 to 0xd3
		0xf4 - 0xa1 to 0xa6
	*/
	switch (c_hi)
	{
		case 0xa2:
			if ((c_lo >= 0xa1) & (c_lo <= 0xae) || (c_lo >= 0xba) & (c_lo <= 0xc1) || (c_lo >= 0xca) & (c_lo <= 0xd0) ||
					(c_lo >= 0xdc) & (c_lo <= 0xea) || (c_lo >= 0xf2) & (c_lo <= 0xf9) || (c_lo == 0xfe))
				return 2;
			return false;
		case 0xa3:
			if ((c_lo >= 0xb0) & (c_lo <= 0xb9) || (c_lo >= 0xc1) & (c_lo <= 0xda) || (c_lo >= 0xe1) & (c_lo <= 0xfa))
				return 2;
			return false;
		case 0xa4:
			if ((c_lo >= 0xa1) & (c_lo <= 0xf3))
				return 2;
			return false;
		case 0xa5:
			if ((c_lo >= 0xa1) & (c_lo <= 0xf6))
				return 2;
			return false;
		case 0xa6:
			if ((c_lo >= 0xa1) & (c_lo <= 0xb8) || (c_lo >= 0xc1) & (c_lo <= 0xd8))
				return 2;
			return false;
		case 0xa7:
			if ((c_lo >= 0xa1) & (c_lo <= 0xc1) || (c_lo >= 0xd1) & (c_lo <= 0xf1))
				return 2;
			return false;
		case 0xa8:
			if ((c_lo >= 0xa1) & (c_lo <= 0xc0))
				return 2;
			return false;
		case 0xcf:
			if ((c_lo >= 0xa1) & (c_lo <= 0xd3))
				return 2;
			return false;
		case 0xf4:
			if ((c_lo >= 0xa1) & (c_lo <= 0xa6))
				return 2;
			return false;
	}

	/*
		Full ku (lo bytes 0xa1 to 0xfe)
		0xa1, 0xb0 to 0xce, 0xd0 to 0xf3
	*/
	if (((c_hi == 0xa1) || ((c_hi >= 0xb0) & (c_hi <= 0xce)) || ((c_hi >= 0xd0) & (c_hi <= 0xf3))) &&
			((c_lo >= 0xa1) & (c_lo <= 0xfe)))
		return 2;

	/*
		JIS X 0212
		0208 extension, so only a few ku are present
		0208 code prefixed by 0x8f
	*/
	if (c_hi == 0x8f)
	{
		u8 c_md = c_lo;
		c_lo = *(data + 2);

		/*
			Partial ku:
			0xa2 - 0xaf to 0xb9, 0xc2 to 0xc4, 0xeb to 0xf1
			0xa6 - 0xe1 to 0xe5, 0xe7, 0xe9, 0xea, 0xec, 0xf1 to 0xfc
			0xa7 - 0xc2 to 0xce, 0xf2 to 0xfe
			0xa9 - 0xa1, 0xa2, 0xa4, 0xa6, 0xa8, 0xa9, 0xab to 0xad, 0xaf, 0xb0, 0xc1 to 0xd0
			0xaa - 0xa1 to 0xb8, 0xba to 0xf7
			0xab - 0xa1 to 0xbb, 0xbd to 0xc3, 0xc5 to 0xf7
			0xed - 0xa1 to 0xe3
		*/
		switch (c_md)
		{
			case 0xa2:
				if (((c_lo >= 0xaf) & (c_lo <= 0xb9)) || ((c_lo >= 0xc2) & (c_lo <= 0xc4)) || ((c_lo >= 0xeb) & (c_lo <= 0xf1)))
					return 3;
				return false;
			case 0xa6:
				if (((c_lo >= 0xe1) & (c_lo <= 0xe5)) || (c_lo == 0xe7) || (c_lo == 0xe9) || (c_lo == 0xea) || (c_lo == 0xec) ||
						((c_lo >= 0xf1) & (c_lo <= 0xfc)))
					return 3;
				return false;
			case 0xa7:
				if (((c_lo >= 0xc2) & (c_lo <= 0xce)) || ((c_lo >= 0xf2) & (c_lo <= 0xfe)))
					return 3;
				return false;
			case 0xa9:
				if ((c_lo == 0xa1) || (c_lo == 0xa2) || (c_lo == 0xa4) || (c_lo == 0xa6) || (c_lo == 0xa8) || (c_lo == 0xa9) ||
						((c_lo >= 0xab) & (c_lo <= 0xad)) || (c_lo == 0xaf) || (c_lo == 0xb0) || ((c_lo >= 0xc1) & (c_lo <= 0xd0)))
					return 3;
				return false;
			case 0xaa:
				if (((c_lo >= 0xa1) & (c_lo <= 0xb8)) || ((c_lo >= 0xba) & (c_lo <= 0xf7)))
					return 3;
				return false;
			case 0xab:
				if (((c_lo >= 0xa1) & (c_lo <= 0xbb)) || ((c_lo >= 0xbd) & (c_lo <= 0xc3)) || ((c_lo >= 0xc5) & (c_lo <= 0xf7)))
					return 3;
				return false;
			case 0xed:
				if ((c_lo >= 0xa1) & (c_lo <= 0xe3))
					return 3;
				return false;
		}

		// Full ku (lo bytes 0xa1 to 0xfe)
		// 0xb0 to 0xec
		if (((c_md >= 0xb0) & (c_md <= 0xec)) && ((c_lo >= 0xa1) & (c_lo <= 0xfe)))
			return 3;
	}
	return false;
}

} // namespace motoi
