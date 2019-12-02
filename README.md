# jstrings
A tool for finding JIS-based Japanese text in binary data.

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

	-m number
	--match-length number

Set minimum number of characters to match as a valid string. Optional; default is 10.

	-c number
	--cutoff number

Limit the output to the specified number of characters for a string. This is useful for "previewing" a file which may have large blocks of junk data that happen to fall within the range of valid encoding values. Optional; default is no cutoff.

## Output
Data is output in its original encoding without any conversion. Other tools, such as iconv, can do conversion to something more useful (such as UTF8). For example:

	# for Shift-JIS
	jstrings file.bin | iconv -f SHIFT-JIS -t UTF-8 -c | less
	# for CP932
	jstrings file.bin | iconv -f CP932 -t UTF-8 -c | less
	# for EUC-JP
	jstrings file.bin | iconv -f EUC-JP -t UTF-8 -c | less

## Building
CMake is used for the build system. From the root directory:

	mkdir build && cd build
	cmake ..
	make
	sudo make install
