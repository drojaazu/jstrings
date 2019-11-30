/*!
 * \brief Find Shift-JIS encoded strings in a byte stream
 * \author Damian Rogers (damian@sudden-desu.net)
 * \version 1.0
 * \date 2017.12.30
 * \copyright GNU Public License
 */
#ifndef SHIFT_JIS_H
#define SHIFT_JIS_H
#include "jis_encoding.h"

namespace encodings
{

class shift_jis : public jis_encoding
{
public:
	shift_jis() : jis_encoding(2){};
	u8 is_valid(u8 *data);

protected:
	bool is_lobyte_valid(const u8 c);
};

} // namespace encodings
#endif // SHIFT_JIS_H
