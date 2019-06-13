/*!
 * \author Damian Rogers (damian@sudden-desu.net)
 * \version 1.0
 * \date 2017.12.30
 * \copyright GNU Public License
 */
#ifndef JIS_ENCODING_H
#define JIS_ENCODING_H
#include <iostream>
#include <vector>

/*!
 * \brief POD structure for containing a found string
 */
struct found_string {
  /*!
   * \brief The offset of the beginning of the found string relative to the
   * start of the stream
   */
  off_t address;
  /*!
   * \brief The extracted string data
   */
  std::vector<uint8_t> data;
};

/*!
 * \brief Abstract for JIS based encoding classes
 */
struct jis_encoding {
  /*!
   * \brief Performs the search of the data stream
   */
  virtual std::vector<found_string>* find(std::istream* instream) = 0;
  /*!
   * \brief Minimum number of characters to match to count as a found string
   */
  size_t match_len = 10;
  /*!
   * \brief Determines byte order for multibyte characters
   */
  bool is_big_endian = true;
  /*!
   * \brief Use the JIS X 0213 character set instead of JIS X 0208
   */
  bool use_jisx0213 = false;

  bool accurate = true;

  virtual ~jis_encoding() {}
};

#endif  // JIS_ENCODING_H