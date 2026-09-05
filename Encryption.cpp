// =============================================================================
// Encryption.cpp - XOR + Caesar cipher implementation
// =============================================================================
#define _CRT_SECURE_NO_WARNINGS
#include "Encryption.h"
#include "Globals.h"
#include <cstring>
using namespace std;

namespace Encryption {

    // -- encrypt --------------------------------------------------------------
    // XOR each character with the rolling key, then add a Caesar shift of 7.
    string encrypt(const string& plainText) {
        string result = plainText;
        size_t keyLen = ENCRYPTION_KEY.size();
        for (size_t i = 0; i < result.size(); ++i) {
            result[i] = static_cast<char>(
                (static_cast<unsigned char>(result[i]) ^ ENCRYPTION_KEY[i % keyLen]) + 7
                );
        }
        return result;
    }

    // -- decrypt --------------------------------------------------------------
    // Reverse: subtract the Caesar shift, then XOR with the same key.
    string decrypt(const string& cipherText) {
        string result = cipherText;
        size_t keyLen = ENCRYPTION_KEY.size();
        for (size_t i = 0; i < result.size(); ++i) {
            result[i] = static_cast<char>(
                (static_cast<unsigned char>(result[i]) - 7) ^ ENCRYPTION_KEY[i % keyLen]
                );
        }
        return result;
    }

    // -- encryptToBuffer --------------------------------------------------------
    void encryptToBuffer(const string& plainText, char* buffer, int bufLen) {
        string enc = encrypt(plainText);
        memset(buffer, 0, bufLen);
        strncpy(buffer, enc.c_str(), bufLen - 1);
    }

    // -- decryptFromBuffer --------------------------------------------------------
    string decryptFromBuffer(const char* buffer) {
        return decrypt(string(buffer));
    }

} // namespace Encryption
