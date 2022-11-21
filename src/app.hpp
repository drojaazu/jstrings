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

static char const * name {"jstrings"};
static char const * brief {"A tool for finding JIS-based Japanese text in binary data."};
static char const * copyright {"©2018 Motoi Productions / Released under MIT License"};
static char const * contact {"Damian R (damian@motoi.pro)"};
static char const * website {"https://github.com/drojaazu"};
} // namespace app

#endif
