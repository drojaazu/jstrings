/*!
 * \author Damian Rogers (damian@sudden-desu.net)
 * \version 1.1
 * \date 2019.12.01
 * \copyright MIT License
 */

#ifndef ENC_SHIFTJIS_H
#define ENC_SHIFTJIS_H
#include "encoding.h"

namespace encodings
{

class shift_jis : public encoding
{
public:
	shift_jis() : encoding(2){};
	u8 is_valid(u8 *data);

protected:
	bool is_lobyte_valid(const u8 c);
};

} // namespace encodings
#endif // ENC_SHIFTJIS_H
