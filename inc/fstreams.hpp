/**
 * @file fstreams.hpp
 * @author Damian Rogers (damian@motoi.pro)
 * @copyright Motoi Productions / Released under MIT License
 * @brief Checked file streams
 *
 * Updates:
 * 20220916 Initial
 * 20221118 Changed exception to std::system_error
 */

#ifndef __MOTOI__FSTREAMS_HPP
#define __MOTOI__FSTREAMS_HPP

#include <cstring>
#include <fstream>
#include <sstream>
#include <system_error>

namespace motoi
{

template <typename StringT>
std::ifstream ifstream_checked (
	std::basic_string<StringT> const & path, std::basic_string<StringT> const & purpose = "read")
{
	std::ifstream ifs (path);
	if (! ifs.good())
	{
		std::basic_ostringstream<StringT> oss;
		oss << "Could not open input path \"" << path << "\" for " << purpose;
		throw std::system_error (errno, std::system_category(), oss.str());
	}
	return ifs;
}

template <typename StringT>
std::ofstream ofstream_checked (
	std::basic_string<StringT> const & path, std::basic_string<StringT> const & purpose = "write")
{
	std::ofstream ofs (path);
	if (! ofs.good())
	{
		std::basic_ostringstream<StringT> oss;
		oss << "Could not open output path \"" << path << "\" for " << purpose << ": " << strerror (errno);
		throw std::system_error (errno, std::system_category(), oss.str());
	}
	return ofs;
}
} // namespace motoi

#endif
