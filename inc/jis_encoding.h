/*!
 * \author Damian Rogers (damian@sudden-desu.net)
 * \version 1.0
 * \date 2017.12.30
 * \copyright GNU Public License
 */
#ifndef JIS_ENCODING_H
#define JIS_ENCODING_H
#include "types.h"

/*!
 * \brief Abstract for JIS based encoding classes
 */
class jis_encoding
{
public:
	jis_encoding(u8 max_seq_len) { this->max_seq_len = max_seq_len; }

	/*!
	 * \brief Determines if the given bytes are a valid byte sequence for the
	 * encoding. Returns the number of valid bytes if true.
	 */
	virtual u8 is_valid(u8 *data) = 0;

	const u8 get_max_seq_len() { return this->max_seq_len; }

protected:
	u8 max_seq_len;
};

#endif // JIS_ENCODING_H