/**
 * @file filesys.hpp
 * @author Motoi Productions (Damian Rogers damian@motoi.pro)
 * @brief File system/path utilities
 *
 * Updates:
 * 20211214 Initial
 * 20220420 Converted to basic_string<type>, added path parsing functions
 * 20220720 Change stringstream to ostringstream; added file_size & concat_paths
 */

#ifndef __MOTOI__FILESYS_HPP
#define __MOTOI__FILESYS_HPP

#include <cstring>
#include <dirent.h>
#include <fstream>
#include <sys/stat.h>

template <typename... StringT>
std::basic_string<StringT...> concat_paths(StringT... paths)
{
	size_t path_count = sizeof...(paths);
	size_t u = 0;
	ostringstream oss;
	for(auto const & s : { paths... })
	{
		++u;
		if(s.empty())
			continue;
		oss << s;
		if(s.back() != '/' && u < path_count)
			oss << '/';
	}

	return oss.str();
}

template <typename StringT>
struct stat stat(std::basic_string<StringT> const & path)
{
	static struct stat status;
	if(::stat(path, &status) != 0)
	{
		std::basic_ostringstream<StringT> ss;
		ss << "Could not open path " << path << ": " << strerror(errno);
		throw runtime_error(ss.str());
	}
	return status;
}

template <typename StringT>
bool exists(std::basic_string<StringT> const & path)
{
	static struct stat status;
	return (::stat(path.c_str(), &status) == 0);
}

template <typename StringT>
size_t file_size(std::basic_string<StringT> const & path)
{
	if(!exists(path))
	{
		std::basic_ostringstream<StringT> oss;
		oss << "Path " << path << " does not exist";
		throw runtime_error(oss.str());
	}

	static struct stat status;
	if(::stat(path.c_str(), &status) != 0)
	{
		std::basic_ostringstream<StringT> oss;
		oss << "Could not open path " << path << ": " << strerror(errno);
		throw runtime_error(oss.str());
	}
	return status.st_size;
}

template <typename StringT>
std::ifstream
ifstream_checked(std::basic_string<StringT> const & path,
								 std::basic_string<StringT> const & purpose = L"read")
{
	std::ifstream ifs(path);
	if(!ifs.good())
	{
		std::basic_ostringstream<StringT> oss;
		oss << "Could not open input path \"" << path << "\" for " << purpose
				<< ": " << strerror(errno);
		throw std::runtime_error(oss.str());
	}
	return ifs;
}

template <typename StringT>
std::ofstream
ofstream_checked(std::basic_string<StringT> const & path,
								 std::basic_string<StringT> const & purpose = L"write")
{
	std::ofstream ofs(path);
	if(!ofs.good())
	{
		std::basic_ostringstream<StringT> oss;
		oss << "Could not open output path \"" << path << "\" for " << purpose
				<< ": " << strerror(errno);
		throw runtime_error(oss.str());
	}
	return ofs;
}

template <typename StringT>
std::basic_string<StringT>
strip_extension(std::basic_string<StringT> const & path)
{
	auto i_at { path.find_last_of('.') };
	if(i_at == std::string::npos)
		return path;
	return path.substr(0, i_at);
}

template <typename StringT>
std::basic_string<StringT>
filename_from_path(std::basic_string<StringT> const & path)
{
	auto i_at { path.find_last_of('/') };
	if(i_at == std::string::npos)
		return path;
	return path.substr(0, i_at);
}

#endif
