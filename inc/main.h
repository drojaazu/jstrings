/*!
 * \author Damian Rogers (damian@sudden-desu.net)
 * \version 1.1
 * \date 2019.12.01
 * \copyright MIT License
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fstream>
#include <iomanip>
#include <iostream>

#include "enc_cp932.h"
#include "enc_eucjp.h"
#include "enc_shiftjis.h"
#include "encoding.h"
#include "types.h"

/*!
 * \enum encodings
 * \brief List of supported encodings
 */
enum enctypes { shift_jis, cp932, eucjp };

void process_args(int argc, char **argv);

void print_help();
