/*!
 * \author Damian Rogers (damian@sudden-desu.net)
 * \version 1.1
 * \date 2019.12.01
 * \copyright MIT License
 */
#ifndef ENC_EUCJP_H
#define ENC_EUCJP_H
#include "encoding.hpp"

namespace encodings
{

class encoding_eucjp : public encoding
{
public:
	encoding_eucjp() : encoding(3) {};
	u8 is_valid(u8 const * data);
	~encoding_eucjp() {};
};

} // namespace encodings
#endif // ENC_EUC_H
