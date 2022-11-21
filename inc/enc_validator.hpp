#ifndef ENCODING_HPP
#define ENCODING_HPP

#include "types.hpp"

namespace motoi
{
/*!
 * \brief Abstract for encoding validator classes
 */
class encoding_validator
{
protected:
	uint m_max_seq_len;

	encoding_validator (uint max_seq_len) :
			m_max_seq_len {max_seq_len}
	{
	}

public:
	encoding_validator() = delete;
	virtual ~encoding_validator() = default;

	/*!
	 * \brief Determines if the given bytes are a valid byte sequence for the encoding.
	 * Returns the number of valid bytes if true.
	 */
	virtual uint is_valid (byte_t const * data) const = 0;

	uint operator() (byte_t const * data) const
	{
		return is_valid (data);
	}

	[[nodiscard]] uint max_seq_len() const
	{
		return m_max_seq_len;
	}
};
} // namespace motoi

#endif
