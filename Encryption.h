// =============================================================================
// Encryption.h - Simple XOR + Caesar cipher for engineer passwords
// =============================================================================
#pragma once
#include <string>
using namespace std;

namespace Encryption {
    // Encrypt a plain-text string; returns an obfuscated string
    string encrypt(const string& plainText);

    // Decrypt an obfuscated string back to plain text
    string decrypt(const string& cipherText);

    // Convenience: encrypt and return a fixed-width char array (padded/truncated)
    void encryptToBuffer(const string& plainText, char* buffer, int bufLen);

    // Convenience: decrypt from a fixed-width char array
    string decryptFromBuffer(const char* buffer);
}
