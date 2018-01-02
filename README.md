# jstrings
A tool for finding JIS-based Japanese characters in binary data.

## Usage
	jstrings [options] [input]

Input can be a filename or data from stdin.

### Options
	-m number

Set minimum number of characters to match as a valid string. Default: 10.

	-e encoding

Specify the encoding to use. Currently, the only valid value is "shift-jis". Default: shift-jis

	-l

Use little-endian order for multibyte characters

	-jisx0213

Use JIS X 0213 character set instead of JIS X 0208 for double byte characters

## Notes
Data is output in its original encoding without any conversion. Other tools, such as iconv, can do conversion to something more useful (such as UTF8). For example:

	jstrings file.bin | iconv -f SHIFT-JIS -t UTF-8 -c

### To Do
- Add support for other JIS encodings: CP932, EUC
- Add support for JIS X 0212 for non-SJIS encodings (only EUC?)
- Add option to only return strings with double-byte characters present

