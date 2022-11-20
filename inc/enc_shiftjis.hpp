#ifndef ENC_SHIFTJIS_HPP
#define ENC_SHIFTJIS_HPP
#include "encoding.hpp"

namespace encodings
{

class shiftjis_validator : public encoding_validator
{
public:
	shiftjis_validator() :
			encoding_validator (2, "SHIFT-JIS") {};
	size_t is_valid (byte_t const * data) const override;
	~shiftjis_validator() override = default;
	;
};

} // namespace encodings

#endif