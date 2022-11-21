#ifndef ENC_EUCJP_HPP
#define ENC_EUCJP_HPP

#include "jis_validator.hpp"

namespace motoi
{

/**
 * @brief Determines if input data points to a valid EUC JP code point
 *
 * @return size_t Returns 0 if not a valid sequence; otherwise returns the
 * number of bytes in the code point sequence
 *
 */
class eucjp_validator : public jis_validator
{
private:
	constexpr static char const * m_iconvcode = "EUC-JP";

public:
	eucjp_validator() :
			jis_validator(3, m_iconvcode) {};
	uint is_valid(byte_t const * data) const override;
	~eucjp_validator() override = default;
};

} // namespace motoi
#endif
