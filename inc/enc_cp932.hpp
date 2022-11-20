#ifndef ENC_CP932_HPP
#define ENC_CP932_HPP

#include "enc_shiftjis.hpp"

namespace encodings
{

class cp932_validator : public shiftjis_validator
{
public:
	cp932_validator()
	{
		m_iconv_code = "CP932";
	}

	size_t is_valid (byte_t const * data) const override;
	~cp932_validator() override = default;
};

} // namespace encodings

#endif