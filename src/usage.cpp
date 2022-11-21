/**
 * @file usage.cpp
 * @author Motoi Productions (Damian Rogers damian@motoi.pro)
 * @brief Display program options/usage
 *
 * Updates:
 * 20220415 Initial
 * 20220722 Using const in show_usage parameters
 * 20220914 Added show_version
 * 20221118 Added copyright field; change to non-wide characters to properly
 * support UTF-8; updates to match changes to match app.hpp.cfg
 */

#include "usage.hpp"
#include "app.hpp"
#include <getopt.h>
#include <ostream>
#include <sstream>

namespace motoi {

using namespace std;

void show_version(ostream &output) {
  ostringstream ss;
  ss << app::name << " - version " << app::version << endl;
  ss << app::copyright << endl;
  ss << app::contact << " / " << app::website << endl;

  output << ss.str();
}

void show_usage(option const *opts, option_details const *details,
                ostream &output) {
  setlocale(LC_ALL, "");

  show_version(output);

  ostringstream ss;
  ss << endl << "Usage:" << endl;

  while (true) {
    if (opts->name == nullptr)
      break;

    ss << " --" << opts->name << ", -" << (char)opts->val;
    if (opts->has_arg == required_argument) {
      if (details->arg_type != nullptr)
        ss << " <" << details->arg_type << ">";
      else
        ss << " <value>";
    }
    if (opts->has_arg == optional_argument) {
      if (details->arg_type != nullptr)
        ss << " <optional " << details->arg_type << ">";
      else
        ss << " <optional value>";
    }
    ss << endl;
    ss << "    ";
    if (details->required)
      ss << "[Required] ";

    ss << details->desc << endl;

    ++opts;
    ++details;
  }

  output << ss.str();
}
} // namespace motoi
