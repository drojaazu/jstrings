#ifndef ENC_SHIFTJIS_HPP
#define ENC_SHIFTJIS_HPP
#include "jis_validator.hpp"

namespace motoi
{

/**
 * @brief Determines if input data points to a valid Shift-JIS code point
 *
 * @return size_t Returns 0 if not a valid sequence; otherwise returns the
 * number of bytes in the code point sequence
 *
 * @details This supports traditional Shift-JIS, which encompasses JIS X 0201 and
 * JIS X 0208 character sets. It does not currently support the JIS X 0213
 * extension set.
 */
class shiftjis_validator : public jis_validator
{
private:
	constexpr static char const * m_iconvcode = "SHIFT-JIS";

public:
	shiftjis_validator() :
			jis_validator(2, m_iconvcode) {};
	uint is_valid(byte_t const * data) const override;
	~shiftjis_validator() override = default;
	;
};

} // namespace motoi
#endif