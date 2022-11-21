#include "enc_shiftjis.hpp"

namespace motoi
{

uint shiftjis_validator::is_valid (byte_t const * data) const
{
	/*
		JIS X 0201 - 8-bit characters (including 7-bit ASCII)
		excludes non-printable (control code) and reserved bytes
		(but include tab (0x09))
	*/
	u8 c_hi {*data};
	// clang-format off
	if ((c_hi == 0x09) || (c_hi >= 0x20) & (c_hi <= 0x7e)
		|| ((! m_skip_jis0201) && ((c_hi >= 0xa1) & (c_hi <= 0xdf)))
	)
		return 1;
	// clang-format on

	// JIS X 0208 - 16 bit characters
	u8 c_lo {*(data + 1)};

	if (m_include_crlf && (c_hi == 0x0a || (c_hi == 0x0d && c_lo == 0x0a)))
		return 1;

	// sjis lower byte can never be these values
	if ((c_lo >= 0x0) & (c_lo <= 0x3f) || (c_lo == 0x7f) || (c_lo >= 0xfd) & (c_lo <= 0xff))
		return 0;

	/*
		Partial fields (always excluding 0x7f)
		0x81 - 0x40 to 0xac, 0xb8 to 0xbf, 0xc8 to 0xce, 0xda to 0xe8, 0xf0 to
		0xf7, 0xfc 0x82 - 0x4f to 0x58, 0x60 to 0x79, 0x81 to 0x9a, 0x9f to 0xf1
		0x83 - 0x40 to 0x96, 0x9f to 0xb6, 0xbf to 0xd6
		0x84 - 0x40 to 0x60, 0x70 to 0x91, 0x9f to 0xbe
		0x88 - 0x9f to 0xfc
		0x98 - 0x40 to 0x72, 0x9f to 0xfc
		0xea - 0x40 to 0xa4
	*/
	switch (c_hi)
	{
		case 0x81:
			if ((c_lo >= 0x40) & (c_lo <= 0xac) || (c_lo >= 0xb8) & (c_lo <= 0xbf) || (c_lo >= 0xc8) & (c_lo <= 0xce) ||
					(c_lo >= 0xda) & (c_lo <= 0xe8) || (c_lo >= 0xf0) & (c_lo <= 0xf7) || (c_lo == 0xfc))
				return 2;
			return 0;
		case 0x82:
			if ((c_lo >= 0x4f) & (c_lo <= 0x58) || (c_lo >= 0x60) & (c_lo <= 0x79) || (c_lo >= 0x81) & (c_lo <= 0x9a) ||
					(c_lo >= 0x9f) & (c_lo <= 0xf1))
				return 2;
			return 0;
		case 0x83:
			if ((c_lo >= 0x40) & (c_lo <= 0x96) || (c_lo >= 0x9f) & (c_lo <= 0xb6) || (c_lo >= 0xbf) & (c_lo <= 0xd6))
				return 2;
			return 0;
		case 0x84:
			if ((c_lo >= 0x40) & (c_lo <= 0x60) || (c_lo >= 0x70) & (c_lo <= 0x91) || (c_lo >= 0x9f) & (c_lo <= 0xbe))
				return 2;
			return 0;
		case 0x88:
			if ((c_lo >= 0x9f) & (c_lo <= 0xfc))
				return 2;
			return 0;
		case 0x98:
			if ((c_lo >= 0x40) & (c_lo <= 0x72) || (c_lo >= 0x9f) & (c_lo <= 0xfc))
				return 2;
			return 0;
		case 0xea:
			if ((c_lo >= 0x40) & (c_lo <= 0xa4))
				return 2;
			return 0;
	}

	/*
		Full fields (0x40 to 0xfc, excluding 0x7f)
		0x89 to 0x97, 0x99 to 0x9f, 0xe0 to 0xe9
	*/
	// clang-format off
	if (
		(((c_hi >= 0x89) & (c_hi <= 0x97))
			|| ((c_hi >= 0x99) & (c_hi <= 0x9f))
			|| ((c_hi >= 0xe0) & (c_hi <= 0xe9))
		)
		&& ((c_lo >= 0x40) & (c_lo <= 0xfc))
	)
		return 2;
	// clang-format on
	return 0;
}

} // namespace motoi
