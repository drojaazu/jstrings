#ifndef ENCODING_HPP
#define ENCODING_HPP
#include "types.hpp"

/*!
 * \brief Abstract for encoding classes
 */
class encoding_validator
{
protected:
	size_t m_max_seq_len;
	char const * m_iconv_code;
	bool m_include_crlf;

	encoding_validator (size_t max_seq_len, char const * iconv_code) :
			m_max_seq_len {max_seq_len},
			m_iconv_code {iconv_code},
			m_include_crlf {false}
	{
	}

public:
	encoding_validator() = delete;
	virtual ~encoding_validator() = default;

	/*!
	 * \brief Determines if the given bytes are a valid byte sequence for the encoding.
	 * Returns the number of valid bytes if true.
	 */
	virtual size_t is_valid (byte_t const * data) const = 0;

	size_t operator() (byte_t const * data) const
	{
		return is_valid (data);
	}

	[[nodiscard]] size_t max_seq_len() const
	{
		return m_max_seq_len;
	}

	[[nodiscard]] char const * iconv_code() const
	{
		return m_iconv_code;
	}

	void set_include_crlf (bool include_crlf)
	{
		m_include_crlf = include_crlf;
	}

	[[nodiscard]] bool include_crlf() const
	{
		return m_include_crlf;
	}
};

#endif // ENCODING_H