#include <iostream>
#include "jis_enc.h"

jis_enc::jis_enc(std::istream* instream) {
	this->instream = instream;
}

jis_enc::~jis_enc() {
	
}

void jis_enc::set_min_len(size_t min_len) {
	this->min_len = min_len;
}

size_t jis_enc::get_min_len() {
	return this->min_len;
}

void jis_enc::set_is_big_endian(bool is_big_endian) {
	this->is_big_endian = is_big_endian;
}

bool jis_enc::get_is_big_endian() {
	return this->is_big_endian;
}

void jis_enc::set_use_jisx0213(bool use_jisx0213) {
	this->use_jisx0213 = use_jisx0213;
}

bool jis_enc::get_use_jisx0213() {
	return this->use_jisx0213;
}
