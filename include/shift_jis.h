/*!
 * \brief Find Shift-JIS encoded strings in a byte stream
 * \author Damian Rogers (damian@sudden-desu.net)
 * \version 1.0
 * \date 2017.12.30
 * \copyright GNU Public License
 */
#ifndef SHIFT_JIS_H
#define SHIFT_JIS_H
#include <stdint.h>
#include <iostream>
#include <vector>
#include "jis_encoding.h"

class shift_jis : public jis_encoding {
  static bool is_byte1_valid(const uint8_t c);
  static bool is_byte2_valid(const uint8_t c);

 public:
  std::vector<found_string>* find(std::istream* instream);
  /*!
   * \brief Determines if a given byte is valid for JIS X 0201 and is printable
   * \return True if the byte is valid
   *
   * In this case, valid means not within the reserved range (0x80 to 0xa0, 0xe0
   * to 0xff) and not an ASCII control character (0x00 to 0x1f, 0x7f)
   */
  static bool is_jisx0201(const uint8_t c);
  /*!
   * \brief Determines if the given bytes make up a valid JIS X 0208 character
   * \return True if the bytes are valid
   */
  static bool is_jisx0208(const uint8_t c_h, const uint8_t c_l,
                          bool accurate_mode);
  /*!
   * \brief Determines if the given bytes make up a valid JIS X 0213 character
   * \return True if the bytes are valid
   */
  static bool is_jisx0213(const uint8_t c_h, const uint8_t c_l,
                          bool accurate_mode);
};

#endif  // SHIFT_JIS_H
