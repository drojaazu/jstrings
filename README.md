# jstrings

A tool for finding JIS encoded Japanese text in binary data.

## Usage

	jstrings [options] [input_file]

Input can be a filename or data from stdin. Output is sent to stdout.

### Options

	-e encoding
	--encoding encoding

Specify the encoding to use. Use one of the strings listed in parantheses below for that encoding:

* Shift-JIS (shift-jis, shiftjis, sjis)
* EUC-JP (euc, euc-jp, eucjp)
* Microsoft CP932 (cp932, windows932, windows31j)

Optional; default is Shift-JIS.

	-l value
	--match-length value

Set number of consecutive characters required to be considered a valid string.

Optional; default is 5.

	-c value
	--cutoff value

Limit the output to the specified number of characters for a string. This is useful for "previewing" a file which may have large blocks of junk data that happen to fall within the range of valid code points. Strings that are cut off will be appended with an ellipsis.

Note that the length is in bytes, not characters. As such, due to the variable width nature of UTF-8, there is a chance the final character displayed may be incorrect. STL string functions do not work natively with encodings and the author feels that the work needed to implement this for an optional feature that should only be used for quickly previewing data would be overly complex.

Optional; default is no cutoff.

	-m
	--multiline

Include newline characters (0x0D or 0x0D0A) as valid. Otherwise, these will count as end of string markers.

Optional; default is disabled.

	-r
	--raw

Output the data in its original encoding without converting to Unicode.

Optional; default is disabled (will convert output strings to UTF-8 using libiconv).

	-s
	--skip-jis0201

Skip checking for JIS X 0201 characters. These is an 8 bit katakana-only code space that acts as a supplement to ASCII and was generally only used in older (early to mid 1980s) home computers. Disabling this can reduce false positives if you are working with newer data.

Optional; default is disabled (will include JIS X 0201 code points as valid matches).


	-l
	--multiline

Do not break the string on CR/LF characters. Such characters will instead appear as /0D and /0A respectively in the output string.

## Output

Found strings are prepended with the offset in which they were found in the original data and sent to stdout. Strings are converted to UTF-8 using libiconv. The original encoding can be preserved by using the `--raw` option.

## Building

CMake is used for the build system. From the root directory:

	mkdir build && cd build
	cmake ..
	make
	sudo make install
