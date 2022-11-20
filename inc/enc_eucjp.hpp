#ifndef ENC_EUCJP_HPP
#define ENC_EUCJP_HPP
#include "encoding.hpp"

namespace encodings
{

class eucjp_validator : public encoding_validator
{
public:
	eucjp_validator() :
			encoding_validator (3, "EUC-JP") {};
	size_t is_valid (byte_t const * data) const override;
	~eucjp_validator() override = default;
	;
};

} // namespace encodings
#endif // ENC_EUC_H
