#ifndef __MOTOI__JSTRINGS_HPP
#define __MOTOI__JSTRINGS_HPP

#include "enc_validator.hpp"
#include "types.hpp"
#include <istream>
#include <utility>
#include <vector>

namespace motoi
{

constexpr static size_t const default_match_length {5};

// buffer size is 4mb
constexpr static size_t const buffer_size {0x100000 * 4};

/**
 * @brief Contains an array of data identified as a valid
 * string of text for a certain encoding. First value is
 * the offset in the source data in which the string was
 * found; second value is the vector of data.
 */
using found_string = std::pair<size_t, std::vector<byte_t>>;

/**
 * @brief Finds strings matching a given encoding within binary data
 */
std::vector<found_string> find (
	std::istream & input, encoding_validator const & validator, size_t const minimum_match_length = default_match_length);

} // namespace motoi

#endif
