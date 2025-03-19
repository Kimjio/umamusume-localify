#ifndef JWT_H
#define JWT_H

#include <string>

using namespace std;

class jwt {
private:
    string _psk;

public:
    jwt(string &psk);

    jwt(const char *psk);

    // Set a new psk for encoding and decoding JWTs
    void setPSK(string &psk);

    void setPSK(const char *psk);

    // Get the calculated length of a JWT
    static size_t getJWTLength(string &payload);

    static size_t getJWTLength(const char *payload);

    // Create a JSON Web Token
    string encodeJWT(string &payload);

    void encodeJWT(char *payload, char *jwt);
};

#endif
