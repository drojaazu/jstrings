/*!
 * \brief Find EUC encoded strings in a byte stream
 * \author Damian Rogers (damian@sudden-desu.net)
 * \version 1.0
 * \date 2017.12.30
 * \copyright GNU Public License
 */
#ifndef EUC_H
#define EUC_H
#include "jis_encoding.h"

namespace encodings
{

class euc : public jis_encoding
{
public:
	euc() : jis_encoding(3){};
	u8 is_valid(u8 *data);
};

} // namespace encodings
#endif // EUC_H
