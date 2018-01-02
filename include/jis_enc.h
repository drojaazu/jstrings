/*!
 * \author Damian Rogers (damian@sudden-desu.net)
 * \version 1.0
 * \date 2017.12.30
 * \copyright GNU Public License
*/
#ifndef JIS_ENC_H
#define JIS_ENC_H
#include <iostream>
#include <vector>

/*!
 * \brief POD structure for containing a found string
*/
struct found_string {
	/*!
	 * \brief The offset of the beginning of the found string relative to the start of the stream
	*/
	off_t address;
	/*!
	 * \brief The extracted string data
	*/
	std::vector<uint8_t> data;
};

/*!
 * \brief Abstract class for JIS based encoding classes
*/
class jis_enc {
protected:
	std::istream* instream;
	/*!
	 * \brief Minimum number of characters to match to count as a found string
	*/
	size_t min_len = 10;
	/*!
	 * \brief Determines byte order for multibyte characters
	*/
	bool is_big_endian = true;
	/*!
	 * \brief Use the JIS X 0213 character set instead of JIS X 0208
	*/
	bool use_jisx0213 = false;

public:
	jis_enc(std::istream* instream);
	virtual ~jis_enc();
	/*!
	 * \brief Performs the search of the data stream
	*/
	virtual std::vector<found_string>* find() = 0;
	/*!
	 * \brief Setter for min_len
	*/
	void set_min_len(size_t min_len);
	/*!
	 * \brief Getter for min_len
	*/
	size_t get_min_len();
	/*!
	 * \brief Setter for is_big_endian
	*/
	void set_is_big_endian(bool is_big_endian);
	/*!
	 * \brief Getter for is_big_endian
	*/
	bool get_is_big_endian();
	/*!
	 * \brief Setter for use_jisx0213
	*/
	void set_use_jisx0213(bool use_jisx0213);
	/*!
	 * \brief Getter for use_jisx0213
	*/
	bool get_use_jisx0213();
};

#endif // JIS_ENC_H