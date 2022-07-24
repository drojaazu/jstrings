#ifndef __MOTOI__APP_HPP
#define __MOTOI__APP_HPP

/*
	These values should be set within CMakeLists.txt
*/
namespace APP {
	static unsigned int const VERSION_MAJOR{1};
	static unsigned int const VERSION_MINOR{2};
	static unsigned int const VERSION_PATCH{};
	static char const * VERSION{"1.2"};

	static wchar_t const * NAME{L"jstrings"};
	static wchar_t const * CONTACT{L"Damian R (damian@sudden-desu.net)"};
	static wchar_t const * WEBSITE{L"https://github.com/drojaazu"};
}
#endif
