/*!
 * \author Damian Rogers (damian@sudden-desu.net)
 * \version 1.1
 * \date 2019.12.01
 * \copyright MIT License
 */

#ifndef TYPES_H
#define TYPES_H

#include <map>
#include <memory>
#include <stdint.h>
#include <vector>
#include <sys/types.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

template <typename T> using uptr = std::unique_ptr<T>;
template <typename T> using sptr = std::shared_ptr<T>;

typedef std::map<std::string const, std::string> kvmap;

#endif