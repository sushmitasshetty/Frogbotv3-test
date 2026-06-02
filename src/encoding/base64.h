#ifndef BASE64_H
#define BASE64_H

/*
 * Base64 encode / decode
 * Source: https://github.com/ReneNyffenegger/cpp-base64
 * Author: René Nyffenegger (rene.nyffenegger@adp-gmbh.ch)
 * License: MIT / ZLib
 *
 * One of the most-starred standalone base64 C++ implementations.
 * The encode/decode functions are widely indexed in JFrog Catalog.
 */

#include <string>

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
std::string base64_encode(std::string const& s);
std::string base64_decode(std::string const& encoded_string);

#endif
