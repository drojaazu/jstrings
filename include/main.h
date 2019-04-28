/*!
 * \brief A tool for finding JIS-based character strings in a binary stream
 * \author Damian Rogers (damian@sudden-desu.net)
 * \version 1.0
 * \date 2017.12.30
 * \copyright GNU Public License
 */
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <getopt.h>

#include "jis_enc.h"
#include "shift_jis.h"

/*!
 * \enum encodings
 * \brief List of supported JIS encodings
 */

enum encodings { shift_jis_enc, cp932_enc, euc_enc };

void process_args(int argc, char **argv);

void print_help();
