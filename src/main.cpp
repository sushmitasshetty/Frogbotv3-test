/*
 * JFrog Snippet Detection Demo — C++ Secure Config Processor
 *
 * This project uses VERBATIM code copied from four of the most heavily
 * indexed open-source repositories in JFrog Catalog:
 *
 *  ┌───────────────────────────────────────────────────────────────────┐
 *  │ File              Origin Repo              License                │
 *  ├───────────────────────────────────────────────────────────────────┤
 *  │ json/cjson.cpp    DaveGamble/cJSON         MIT                    │
 *  │ crypto/sha256.cpp B-Con/crypto-algorithms  Public Domain          │
 *  │ checksum/adler32  madler/zlib              zlib/libpng License    │
 *  │ encoding/base64   ReneNyffenegger/cpp-base64  MIT                 │
 *  └───────────────────────────────────────────────────────────────────┘
 *
 * Run snippet detection:
 *   jf audit --sca --static-sca --snippet --watches=<watch-name>
 */

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

#include "json/cjson.h"
#include "crypto/sha256.h"
#include "checksum/adler32.h"
#include "encoding/base64.h"

#define RESET  "\033[0m"
#define GREEN  "\033[32m"
#define YELLOW "\033[33m"
#define CYAN   "\033[36m"
#define BOLD   "\033[1m"

static void print_banner() {
    std::cout << BOLD << CYAN;
    std::cout << "\n╔═══════════════════════════════════════════════════════╗\n";
    std::cout <<   "║   JFrog Snippet Detection Demo — SecureConfig Tool   ║\n";
    std::cout <<   "╚═══════════════════════════════════════════════════════╝\n";
    std::cout << RESET << "\n";
}

static void section(const std::string& t) {
    std::cout << BOLD << YELLOW << "\n▶ " << t << RESET << "\n";
    std::cout << std::string(55, '-') << "\n";
}

/* ── 1. cJSON parsing demo ─────────────────────────────────────── */
static void demo_json() {
    section("cJSON Parser  (DaveGamble/cJSON — MIT)");

    const char *config_json = R"({
        "service": "snippet-demo",
        "version": "1.0.0",
        "debug": false,
        "timeout": 30,
        "tags": ["security", "jfrog", "snippet-detection"]
    })";

    cJSON *root = cJSON_Parse(config_json);
    if (!root) { std::cout << "  Parse failed\n"; return; }

    cJSON *svc     = cJSON_GetObjectItem(root, "service");
    cJSON *ver     = cJSON_GetObjectItem(root, "version");
    cJSON *timeout = cJSON_GetObjectItem(root, "timeout");
    cJSON *tags    = cJSON_GetObjectItem(root, "tags");

    std::cout << "  service : " << GREEN << (svc     ? svc->valuestring     : "?") << RESET << "\n";
    std::cout << "  version : " << GREEN << (ver     ? ver->valuestring     : "?") << RESET << "\n";
    std::cout << "  timeout : " << GREEN << (timeout ? timeout->valueint    : 0)   << RESET << "s\n";
    std::cout << "  tags    : ";
    if (tags && cJSON_IsArray(tags)) {
        int sz = cJSON_GetArraySize(tags);
        for (int i = 0; i < sz; i++) {
            cJSON *t = cJSON_GetArrayItem(tags, i);
            std::cout << GREEN << (t ? t->valuestring : "?") << RESET;
            if (i < sz - 1) std::cout << ", ";
        }
    }
    std::cout << "\n";
    std::cout << "  ⚑ Origin: DaveGamble/cJSON  License: MIT\n";

    cJSON_Delete(root);
}

/* ── 2. SHA-256 hashing demo ───────────────────────────────────── */
static void demo_sha256() {
    section("SHA-256  (B-Con/crypto-algorithms — Public Domain)");

    std::string inputs[] = {
        "JFrog Snippet Detection",
        "hello world",
        "The quick brown fox jumps over the lazy dog"
    };

    for (const auto& s : inputs) {
        std::string h = sha256_hex(s);
        std::cout << "  \"" << s << "\"\n";
        std::cout << "   → " << GREEN << h << RESET << "\n";
    }
    std::cout << "  ⚑ Origin: B-Con/crypto-algorithms  License: Public Domain\n";
}

/* ── 3. Adler-32 checksum demo ─────────────────────────────────── */
static void demo_adler32() {
    section("Adler-32  (madler/zlib — zlib/libpng License)");

    const std::string payload = "snippet-detection-demo-payload";
    uLong checksum = adler32(1L, (const Bytef*)payload.c_str(), (uInt)payload.size());

    std::cout << "  Input    : \"" << payload << "\"\n";
    std::cout << "  Adler-32 : " << GREEN << "0x" << std::hex << std::uppercase
              << checksum << std::dec << RESET << "\n";
    std::cout << "  ⚑ Origin: madler/zlib  License: zlib/libpng (attribution required)\n";
}

/* ── 4. Base64 encoding demo ───────────────────────────────────── */
static void demo_base64() {
    section("Base64  (ReneNyffenegger/cpp-base64 — MIT)");

    const std::string original = "JFrog Snippet Detection finds copied code at function level";
    std::string encoded = base64_encode(original);
    std::string decoded = base64_decode(encoded);

    std::cout << "  Original : \"" << original << "\"\n";
    std::cout << "  Encoded  : " << GREEN << encoded << RESET << "\n";
    std::cout << "  Decoded  : " << GREEN << decoded << RESET << "\n";
    std::cout << "  Match    : " << (original == decoded ? "✓ YES" : "✗ NO") << "\n";
    std::cout << "  ⚑ Origin: ReneNyffenegger/cpp-base64  License: MIT\n";
}

/* ── CLI guide ─────────────────────────────────────────────────── */
static void print_cli_guide() {
    std::cout << BOLD << CYAN;
    std::cout << "\n╔═══════════════════════════════════════════════════════╗\n";
    std::cout <<   "║          Run JFrog Snippet Detection                  ║\n";
    std::cout <<   "╚═══════════════════════════════════════════════════════╝\n";
    std::cout << RESET;
    std::cout << R"(
  # Configure JFrog CLI
  jf c add my-server --url=https://<jpd>.jfrog.io \
                     --access-token=<token>
  jf c use my-server

  # Run — snippet detection + SCA from conanfile.txt
  jf audit --sca --static-sca --snippet --watches=<watch-name>

  Expected snippet findings:
  ┌──────────────────────────────────────────────────────┐
  │ json/cjson.cpp    → DaveGamble/cJSON  (MIT)          │
  │ crypto/sha256.cpp → B-Con/crypto-algorithms (PD)     │
  │ checksum/adler32  → madler/zlib  (zlib/libpng)       │
  │ encoding/base64   → ReneNyffenegger/cpp-base64 (MIT) │
  └──────────────────────────────────────────────────────┘
)";
}

int main() {
    print_banner();
    demo_json();
    demo_sha256();
    demo_adler32();
    demo_base64();
    print_cli_guide();
    return 0;
}
