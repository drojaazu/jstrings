/**
 * @author Damian R (damian@motoi.pro)
 * @brief 
 * @version 1.5
 * 
 * @copyright ©2018 Motoi Productions / Released under MIT License
 *
 */

#ifndef __MOTOI__APP_HPP
#define __MOTOI__APP_HPP

/*
	These values should be set within CMakeLists.txt
*/
namespace APP
{
static unsigned int const VERSION_MAJOR {1};
static unsigned int const VERSION_MINOR {5};
static unsigned int const VERSION_PATCH {};
static char const * VERSION {"1.5"};

static wchar_t const * NAME {L"jstrings"};
static wchar_t const * CONTACT {L"Damian R (damian@motoi.pro)"};
static wchar_t const * WEBSITE {L"https://github.com/drojaazu"};
static wchar_t const * BRIEF {L""};
} // namespace APP
#endif
