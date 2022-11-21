/**
 * @author Damian R (damian@motoi.pro)
 * @brief A tool for finding JIS-based Japanese text in binary data.
 * @version 1.6
 * 
 * @copyright ©2018 Motoi Productions / Released under MIT License
 *
 */

#ifndef __MOTOI__APP_HPP
#define __MOTOI__APP_HPP

/*
	These values should be set within CMakeLists.txt
*/
namespace app
{
static unsigned int const version_major {1};
static unsigned int const version_minor {6};
static unsigned int const version_patch {};
static char const * version {"1.6"};

static wchar_t const * name {L"jstrings"};
static wchar_t const * brief {L"A tool for finding JIS-based Japanese text in binary data."};
static wchar_t const * copyright {L"©2018 Motoi Productions / Released under MIT License"};
static wchar_t const * contact {L"Damian R (damian@motoi.pro)"};
static wchar_t const * website {L"https://github.com/drojaazu"};
} // namespace app

#endif
