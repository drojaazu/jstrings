/*!
 * \author Damian Rogers (damian@sudden-desu.net)
 * \version 1.1
 * \date 2019.12.01
 * \copyright MIT License
 */
#ifndef ENC_EUCJP_H
#define ENC_EUCJP_H
#include "encoding.h"

namespace encodings
{

class euc : public encoding
{
public:
	euc() : encoding(3){};
	u8 is_valid(u8 const *data);
	~euc(){};
};

} // namespace encodings
#endif // ENC_EUC_H
