/*!
 * \author Damian Rogers (damian@sudden-desu.net)
 * \version 1.1
 * \date 2019.12.01
 * \copyright MIT License
 */

#ifndef ENC_CP932_H
#define ENC_CP932_H

#include "enc_shiftjis.h"

namespace encodings
{

class cp932 : public shift_jis
{
public:
	u8 is_valid(u8 *data);
};

} // namespace encodings

#endif