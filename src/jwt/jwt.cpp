#include <string>
#include <vector>
#include "../hmac_sha256/hmac_sha256.h"
#include "jwt.hpp"
#include "base64.hpp"

// The standard JWT header already base64 encoded. Equates to {"alg": "HS256", "typ": "JWT"}
const char *jwtHeader = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9";

jwt::jwt(string &psk) {
    _psk = psk;
}

jwt::jwt(const char *psk) {
    _psk = string(psk);
}

void jwt::setPSK(string &psk) {
    _psk = psk;
}

void jwt::setPSK(const char *psk) {
    _psk = string(psk);
}

size_t jwt::getJWTLength(string &payload) {
    return getJWTLength(payload.data());
}

size_t jwt::getJWTLength(const char *payload) {
    return strlen(jwtHeader) + encode_base64_length(strlen(payload)) + encode_base64_length(32) + 2;
}

string jwt::encodeJWT(string &payload) {
    vector<char> jwt(getJWTLength(payload));
    encodeJWT(payload.data(), jwt.data());
    return {jwt.data()};
}

void jwt::encodeJWT(char *payload, char *jwt) {
    auto *ptr = reinterpret_cast<unsigned char *>(jwt);
    // Build the initial part of the jwt (header.payload)
    memcpy(ptr, jwtHeader, strlen(jwtHeader));
    ptr += strlen(jwtHeader);
    *ptr++ = '.';
    encode_base64(reinterpret_cast<unsigned char *>(payload), strlen(payload), ptr);
    ptr += encode_base64_length(strlen(payload));
    // Get rid of any padding (trailing '=' added when base64 encoding)
    while (*(ptr - 1) == '=') {
        ptr--;
    }
    *(ptr) = 0;

    std::vector<uint8_t> out(32);

    // Build the signature
    hmac_sha256(_psk.data(), _psk.length(), jwt, strlen(jwt), out.data(), out.size());
    // Add the signature to the jwt
    *ptr++ = '.';
    encode_base64(out.data(), 32, ptr);
    ptr += encode_base64_length(32);
    // Get rid of any padding and replace / and +
    while (*(ptr - 1) == '=') {
        ptr--;
    }
    *(ptr) = 0;
}
