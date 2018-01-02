/*!
 * \brief A tool for finding JIS-based character strings in a binary stream
 * \author Damian Rogers (damian@sudden-desu.net)
 * \version 1.0
 * \date 2017.12.30
 * \copyright GNU Public License
*/

/*!
 * \enum encodings
 * \brief List of supported JIS encodings
*/
enum encodings { shift_jis_enc, cp932_enc, euc_enc };

int process_args(int argc, char **argv);
