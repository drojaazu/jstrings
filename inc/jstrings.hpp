/**
 * @file jstrings.hpp
 * @author Damian Rogers (damian@motoi.pro)
 * @copyright Motoi Productions / Released under MIT License
 * @brief Tool for finding strings for a given encoding within binary data
 *
 * Updates:
 * 20221103 Initial
 */

#ifndef __MOTOI__JSTRINGS_HPP
#define __MOTOI__JSTRINGS_HPP

#include "encoding.hpp"
#include "types.hpp"
#include <fstream>
#include <iconv.h>
#include <string>
#include <vector>

constexpr static size_t const default_match_length {5};

// buffer size is 4mb
constexpr static size_t const buffer_size {0x100000 * 4};

/**
 * @brief Contains a valid string found within data including
 * its start address and length within the input
 * the length of the data can be obtained from the vector
 *
 */
using found_string = std::pair<size_t, std::vector<byte_t>>;

std::vector<found_string> find (
	std::istream & is, encoding_validator const & enc, size_t const minimum_match_length = default_match_length);

#endif
