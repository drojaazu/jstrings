/*!
 * \author Damian Rogers (damian@sudden-desu.net)
 * \version 1.1
 * \date 2019.12.01
 * \copyright MIT License
 */

#ifndef ENCODING_H
#define ENCODING_H
#include "types.hpp"

/*!
 * \brief Abstract for encoding classes
 */
class encoding
{
public:
	encoding(u8 max_seq_len)
	{
		this->max_seq_len = max_seq_len;
	}
	encoding() = delete;
	virtual ~encoding() = default;

	/*!
	 * \brief Determines if the given bytes are a valid byte sequence for the encoding.
	 * Returns the number of valid bytes if true.
	 */
	virtual u8 is_valid(u8 const * data) = 0;

	const u8 get_max_seq_len()
	{
		return this->max_seq_len;
	}

protected:
	u8 max_seq_len;
};

#endif // ENCODING_H