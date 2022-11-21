/**
 * @file uniconv.hpp
 * @author Damian Rogers (damian@motoi.pro)
 * @copyright Motoi Productions / Released under MIT License
 * @brief Wrapper for iconv to convert to utf8
 *
 * Updates:
 * 20221120 Initial
 */

#ifndef __MOTOI__UNICONV_HPP
#define __MOTOI__UNICONV_HPP

#include <iconv.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

namespace motoi
{
class uniconv
{
private:
	constexpr static char const * m_convcode = "UTF-8";
	iconv_t m_conv;

public:
	uniconv (uniconv const &) = delete;
	uniconv & operator= (uniconv const &) = delete;
	uniconv (uniconv &&) = default;
	uniconv & operator= (uniconv &&) = default;

	uniconv (const char * fromcode)
	{
		m_conv = iconv_open (uniconv::m_convcode, fromcode);

		if (m_conv == (iconv_t) -1)
		{
			std::ostringstream oss;
			oss << "Could not open converter using encoding " << fromcode;
			throw std::system_error (errno, std::generic_category(), oss.str());
		}
	}

	~uniconv()
	{
		iconv_close (m_conv);
	}

	std::string convert (char * in, size_t in_size)
	{
		std::string out (in_size * 2, '\0');

		char * p_in = in;
		char * p_out = (char *) out.data();
		size_t out_size = out.size();
		size_t result;

		// clang-format off
		while ((result = iconv
			(m_conv, &p_in, &in_size, &p_out, &out_size)
			== -1))
		// clang-format on
		{
			// received some kind of error status
			if (errno == EILSEQ || errno == EINVAL)
			{
				ptrdiff_t pos = p_in - in;
				std::ostringstream oss;
				oss << "Encountered invalid byte sequence at position " << pos;
				throw std::system_error (errno, std::generic_category(), oss.str());
			}

			// output buffer out of space, resize and retry
			if (errno == E2BIG)
			{
				ptrdiff_t pos = p_out - out.data();
				out.resize (out.size() * 2);
				p_out = out.data() + pos;
				out_size = out.size() - pos;
			}
		}

		out.resize (p_out - out.data());

		return out;
	}

	std::string convert (std::vector<char> & in)
	{
		return convert (in.data(), in.size());
	}

	std::string convert (std::vector<unsigned char> & in)
	{
		return convert ((char *) in.data(), in.size());
	}

	std::string convert (std::string & in)
	{
		return convert (in.data(), in.size());
	}
};
} // namespace motoi

#endif
