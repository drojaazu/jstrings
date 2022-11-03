#include "enc_cp932.hpp"

namespace encodings {

u8 encoding_cp932::is_valid(u8 const *data) {
  u8 valid_count = encoding_shiftjis::is_valid(data);
  if (valid_count > 0)
    return valid_count;
  else {
    u8 c_hi{*data};
    u8 c_lo{*(data + 1)};

    /*
            ku 13 - lead byte 0x87 - NEC tokushu moji
                    0x40 to 0x5d, 0x5f to 0x75, 0x7e, 0x80 to 0x8f, 0x93, 0x94,
       0x98, 0x99 ku 89-92 - lead byte 0xed, 0xee - NEC sentei IBM kakuchou moji
                    0x40 to 0x7e, 0x80 to 0xfc
            ku 115-119 - lead byte 0xfa to 0xfc - IBM kakuchou moji
                    0x40 to 0x7e, 0x80 to 0xfc (except 0xfc: 0x40 to 0x4b)
    */
    switch (c_hi) {
    case 0x87:
      if (((c_lo >= 0x40) & (c_lo <= 0x5d)) ||
          ((c_lo >= 0x5f) & (c_lo <= 0x75)) || (c_lo == 0x7e) ||
          ((c_lo >= 0x80) & (c_lo <= 0x8f)) || (c_lo == 0x93) ||
          (c_lo == 0x94) || (c_lo == 0x98) || (c_lo == 0x99))
        return 2;
      break;
    case 0xed:
    case 0xee:
    case 0xfa:
    case 0xfb:
      // 0x7f should already be excluded from the base shiftjis check
      if ((c_lo >= 0x40) & (c_lo <= 0xfc))
        return 2;
    case 0xfc:
      if ((c_lo >= 0x40) & (c_lo <= 0x4b))
        return 2;
    }
  }
  return 0;
}
} // namespace encodings