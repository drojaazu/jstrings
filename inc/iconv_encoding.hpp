#ifndef __MOTOI__ICONV_ENCODING_HPP
#define __MOTOI__ICONV_ENCODING_HPP

namespace motoi
{

/**
 * @brief Abstract class for specifying an iconv encoding code
 *
 */
class iconv_encoding
{
protected:
	char const * m_iconv_code;

	iconv_encoding(char const * iconv_code) :
			m_iconv_code {iconv_code}
	{
	}

public:
	iconv_encoding() = delete;
	virtual ~iconv_encoding() = default;

	[[nodiscard]] char const * iconv_code() const
	{
		return m_iconv_code;
	}
};

} // namespace motoi
#endif
