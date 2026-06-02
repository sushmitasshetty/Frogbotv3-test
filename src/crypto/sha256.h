#ifndef SHA256_H
#define SHA256_H

/*
 * SHA-256
 * Source: https://github.com/B-Con/crypto-algorithms/blob/master/sha256.h
 * Author: Brad Conte (brad@bradconte.com)
 * License: Public Domain
 *
 * One of the most-copied and most-indexed SHA-256 implementations on GitHub.
 * JFrog Snippet Detection will fingerprint sha256_transform() and sha256_final()
 * against the Catalog and report:
 *   Origin : B-Con/crypto-algorithms
 *   License: Public Domain
 */

#include <stddef.h>
#include <stdint.h>

#define SHA256_BLOCK_SIZE 32

typedef uint8_t  BYTE;
typedef uint32_t WORD;

typedef struct {
    BYTE  data[64];
    WORD  datalen;
    unsigned long long bitlen;
    WORD  state[8];
} SHA256_CTX;

void sha256_init(SHA256_CTX *ctx);
void sha256_update(SHA256_CTX *ctx, const BYTE data[], size_t len);
void sha256_final(SHA256_CTX *ctx, BYTE hash[]);

#ifdef __cplusplus
#include <string>
std::string sha256_hex(const std::string& input);
#endif

#endif
