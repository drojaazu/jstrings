#ifndef __MAIN_HPP
#define __MAIN_HPP

#include <string>

/*
	These values should be set within CMakeLists.txt
*/
namespace PROJECT {
	static unsigned int const VERSION_MAJOR{1};
	static unsigned int const VERSION_MINOR{2};
	static unsigned int const VERSION_PATCH{};
	static std::string const VERSION{"1.2"};

	static std::string const PROJECT_NAME{"jstrings"};
	static std::string const PROJECT_CONTACT{"Damian R (damian@sudden-desu.net)"};
	static std::string const PROJECT_WEBSITE{"https://github.com/drojaazu"};
}
#endif
