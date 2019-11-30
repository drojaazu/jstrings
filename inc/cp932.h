#ifndef CP932_H
#define CP932_H

#include "shift_jis.h"

namespace encodings
{

class cp932 : public shift_jis
{
public:
	u8 is_valid(u8 *data);
};

} // namespace encodings

#endif