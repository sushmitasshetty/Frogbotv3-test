#ifndef ADLER32_H
#define ADLER32_H

/*
 * Adler-32 checksum
 * Source: https://github.com/madler/zlib/blob/master/adler32.c
 * Author: Jean-loup Gailly & Mark Adler
 * License: zlib/libpng License
 *
 * The DO1/DO8/DO16 macro-unrolled loop structure is one of zlib's most
 * recognisable patterns. JFrog Snippet Detection will fingerprint it at
 * the function level and report the zlib/libpng license obligation.
 */

#include <stdint.h>
#include <stddef.h>

typedef unsigned long uLong;
typedef unsigned char Bytef;
typedef unsigned int  uInt;

#define Z_NULL 0

uLong adler32(uLong adler, const Bytef *buf, uInt len);
uLong adler32_combine(uLong adler1, uLong adler2, long len2);

#endif
