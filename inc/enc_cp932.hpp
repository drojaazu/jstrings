#ifndef ENC_CP932_HPP
#define ENC_CP932_HPP

#include "enc_shiftjis.hpp"

namespace motoi
{

/**
 * @brief Determines if input data points to a valid MS Code Page 932 code point
 *
 * @return size_t Returns 0 if not a valid sequence; otherwise returns the
 * number of bytes in the code point sequence
 *
 */
class cp932_validator : public shiftjis_validator
{
public:
	cp932_validator()
	{
		m_iconv_code = "CP932";
	}

	uint is_valid (byte_t const * data) const override;
	~cp932_validator() override = default;
};

} // namespace motoi

#endif