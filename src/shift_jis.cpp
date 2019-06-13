#include "shift_jis.h"
#include <stdint.h>
#include <string.h>
#include "jis_encoding.h"

using namespace std;

// sjis information comes from:
// JWPce tables
// the Yamaha RT series page -
// http://www.rtpro.yamaha.co.jp/RT/docs/misc/kanji-sjis.html the Unicode
// consortium conversion sheet -
// ftp://ftp.unicode.org/Public/MAPPINGS/OBSOLETE/EASTASIA/JIS/SHIFTJIS.TXT
// http://www.asahi-net.or.jp/~ax2s-kmtn/ref/index.html

vector<found_string>* shift_jis::find(istream* instream) {
  // list of found strings
  vector<found_string>* found_strings = new vector<found_string>;

  // work bytes
  char tempc;
  uint8_t thisc, nextc;

  // work string
  found_string this_str;
  this_str.address = -1;
  this_str.data.reserve(this->match_len);

  // choose which double byte filter to use
  bool (*jisx_version)(const uint8_t, const uint8_t, bool accurate_mode);
  if (this->use_jisx0213)
    jisx_version = shift_jis::is_jisx0213;
  else
    jisx_version = shift_jis::is_jisx0208;

  // stream read loop
  while (instream->get(tempc)) {
    thisc = (uint8_t)tempc;

    // ---- CHECK #1 - JIS X 0201 (single byte)
    if (this->is_jisx0201(thisc)) {
      // subtract two from tellg address to account for the posistion of the
      // stream pointer (which is after the 'current' byte)
      if (this_str.address < 0)
        this_str.address = (instream->tellg() - (streampos)1);
      this_str.data.push_back(thisc);
      continue;
    }

    // ---- CHECK #2 - JIS X 0208 or 0213 (double byte)
    // first byte was valid, let's check the next one
    // get next byte
    if (!instream->get(tempc)) continue;
    nextc = (uint8_t)tempc;

    if ((this->is_big_endian && jisx_version(thisc, nextc, this->accurate)) ||
        jisx_version(nextc, thisc, this->accurate)) {
      // subtract two from tellg address to account for the posistion of the
      // stream pointer (which is after the 'current' byte) and for nextc that
      // was read from the stream
      if (this_str.address < 0)
        this_str.address = (instream->tellg() - (streampos)2);
      this_str.data.push_back(thisc);
      this_str.data.push_back(nextc);
      continue;
    } else {
      // push the read pointer back a byte
      instream->unget();
    }

    // hit an invalid byte
    // are there enough character matches to make a string?
    if (this_str.data.size() >= this->match_len) {
      // add terminator to string
      this_str.data.push_back('\0');
      found_strings->push_back(this_str);
    }
    this_str.data.clear();
    this_str.data.reserve(this->match_len);
    this_str.address = -1;
  }
  // do a final check if we were in the middle of a group of valid bytes
  // todo - make this DRY
  if (this_str.data.size() >= this->match_len) {
    // add terminator to string
    this_str.data.push_back('\0');
    found_strings->push_back(this_str);
  }
  this_str.data.clear();
  this_str.data.reserve(this->match_len);
  this_str.address = -1;

  return found_strings;
}

inline bool shift_jis::is_byte1_valid(const uint8_t c) {
  // sjis byte 1 can never be these values
  if ((c >= 0) & (c <= 0x80) || (c >= 0x85) & (c <= 0x87) ||
      (c >= 0xa0) & (c <= 0xdf))
    return false;
  return true;
}

inline bool shift_jis::is_byte2_valid(const uint8_t c) {
  // sjis byte 2 can never be these values
  if ((c >= 0x0) & (c <= 0x3f) || (c == 0x7f) || (c >= 0xfd) & (c <= 0xff))
    return false;
  return true;
}

// JIS X 0201 - 8-bit characters (including 7-bit ASCII)
// excludes non-printable (control code) and reserved bytes
inline bool shift_jis::is_jisx0201(const uint8_t c) {
  if ((c >= 0x20) & (c <= 0x7e) || (c >= 0xa1) & (c <= 0xdf)) return true;
  return false;
}

// JIS X 0208 - 16-bit characters
inline bool shift_jis::is_jisx0208(const uint8_t c_hi, const uint8_t c_lo,
                                   bool accurate_mode) {
  if (!shift_jis::is_byte2_valid(c_lo)) return false;

  // we've determined the second byte is valid as part of an SJIS encoded pair
  // if we're in fast mode, that's good enough; return
  if (!accurate_mode) return true;

  // Partial ku (excluding 0x7f)
  // 0x81 - 0x40 to 0xac, 0xb8 to 0xbf, 0xc8 to 0xce, 0xda to 0xe8, 0xf0 to
  // 0xf7, 0xfc 0x82 - 0x4f to 0x58, 0x60 to 0x79, 0x81 to 0x9a, 0x9f to 0xf1
  // 0x83 - 0x40 to 0x96, 0x9f to 0xb6, 0xbf to 0xd6
  // 0x84 - 0x40 to 0x60, 0x70 to 0x91, 0x9f to 0xbe
  // 0x88 - 0x9f to 0xfc
  // 0x98 - 0x40 to 0x72, 0x9f to 0xfc
  // 0xea - 0x40 to 0xa4
  switch (c_hi) {
    case 0x81:
      if ((c_lo >= 0x40) & (c_lo <= 0xac) || (c_lo >= 0xb8) & (c_lo <= 0xbf) ||
          (c_lo >= 0xc8) & (c_lo <= 0xce) || (c_lo >= 0xda) & (c_lo <= 0xe8) ||
          (c_lo >= 0xf0) & (c_lo <= 0xf7) || (c_lo == 0xfc))
        return true;
      return false;
    case 0x82:
      if ((c_lo >= 0x4f) & (c_lo <= 0x58) || (c_lo >= 0x60) & (c_lo <= 0x79) ||
          (c_lo >= 0x81) & (c_lo <= 0x9a) || (c_lo >= 0x9f) & (c_lo <= 0xf1))
        return true;
      return false;
    case 0x83:
      if ((c_lo >= 0x40) & (c_lo <= 0x96) || (c_lo >= 0x9f) & (c_lo <= 0xb6) ||
          (c_lo >= 0xbf) & (c_lo <= 0xd6))
        return true;
      return false;
    case 0x84:
      if ((c_lo >= 0x40) & (c_lo <= 0x60) || (c_lo >= 0x70) & (c_lo <= 0x91) ||
          (c_lo >= 0x9f) & (c_lo <= 0xbe))
        return true;
      return false;
    case 0x88:
      if ((c_lo >= 0x9f) & (c_lo <= 0xfc)) return true;
      return false;
    case 0x98:
      if ((c_lo >= 0x40) & (c_lo <= 0x72) || (c_lo >= 0x9f) & (c_lo <= 0xfc))
        return true;
      return false;
    case 0xea:
      if ((c_lo >= 0x40) & (c_lo <= 0xa4)) return true;
      return false;
  }
  // Full ku (0x40 to 0xfc, excluding 0x7f)
  // 0x89 to 0x97, 0x99 to 0xe9
  if ((((c_hi >= 0x89) & (c_hi <= 0x97)) ||
       ((c_hi >= 0x99) & (c_hi <= 0xe9))) &&
      ((c_lo >= 0x40) & (c_lo <= 0xfc)))
    return true;
  return false;
}

// JIS X 0213 - 16-bit characters
inline bool shift_jis::is_jisx0213(const uint8_t c_hi, const uint8_t c_lo,
                                   bool accurate_mode) {
  if (!shift_jis::is_byte2_valid(c_lo)) return false;

  // we've determined the second byte is valid as part of an SJIS encoded pair
  // if we're in fast mode, that's good enough; return
  if (!accurate_mode) return true;

  // Partial ku (excluding 0x7f)
  // 0x84 - 0x40 to 0xdc, 0xe5 to 0xfa
  // 0x86 - 0x40 to 0xf1, 0xfb to 0xfc
  // 0x87 - 0x40 to 0x76, 0x7e to 0x8f, 0x93, 0x98 to 0x99, 0x9d to 0xfc
  // 0xfc - 0x40 to 0xf4
  switch (c_hi) {
    case 0x84:
      if ((c_lo >= 0x40) & (c_lo <= 0xdc) || (c_lo >= 0xe5) & (c_lo <= 0xfa))
        return true;
      return false;
    case 0x86:
      if ((c_lo >= 0x40) & (c_lo <= 0xf1) || (c_lo >= 0xfb) & (c_lo <= 0xfc))
        return true;
      return false;
    case 0x87:
      if ((c_lo >= 0x40) & (c_lo <= 0x76) || (c_lo >= 0x7e) & (c_lo <= 0x8f) ||
          (c_lo == 0x93) || (c_lo >= 0x98) & (c_lo <= 0x99) ||
          (c_lo >= 0x9d) & (c_lo <= 0xfc))
        return true;
      return false;
    case 0xfc:
      if ((c_lo >= 0x40) & (c_lo <= 0xf4)) return true;
      return false;
  }

  // Full ku (0x40 to 0xfc, excluding 0x7f)
  // 0x81 to 0x83, 0x85, 0x88 to 0xfb
  if ((((c_hi >= 0x81) & (c_hi <= 0x83)) || (c_hi == 0x85) ||
       ((c_hi >= 0x88) & (c_hi <= 0xfb))) &&
      ((c_lo >= 0x40) & (c_lo <= 0xfc)))
    return true;
  return false;
}
