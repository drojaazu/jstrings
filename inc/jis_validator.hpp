#ifndef __MOTOI__JIS_VALIDATOR_HPP
#define __MOTOI__JIS_VALIDATOR_HPP

#include "enc_validator.hpp"
#include "iconv_encoding.hpp"

namespace motoi
{
/**
 * @brief Abstract class for JIS character set encoding validators
 */
class jis_validator : public encoding_validator, public iconv_encoding
{
protected:
	bool m_include_crlf {false};
	bool m_skip_jis0201 {false};

	jis_validator(size_t max_seq_len, char const * iconv_code) :
			encoding_validator(max_seq_len),
			iconv_encoding(iconv_code)
	{
	}

public:
	jis_validator() = delete;

	void include_crlf(bool include_crlf)
	{
		m_include_crlf = include_crlf;
	}

	[[nodiscard]] bool include_crlf() const
	{
		return m_include_crlf;
	}

	void skip_jis0201(bool skip_jis0201)
	{
		m_skip_jis0201 = skip_jis0201;
	}

	[[nodiscard]] bool skip_jis0201() const
	{
		return m_skip_jis0201;
	}
};

} // namespace motoi

#endif
