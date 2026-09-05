// =============================================================================
// Engineer.cpp - Engineer class implementation
// =============================================================================
#define _CRT_SECURE_NO_WARNINGS
#include "Engineer.h"
#include "Encryption.h"
#include <cstring>
#include <iostream>
using namespace std;

Engineer::Engineer(const string& id, const string& user,
    const string& encryptedPass, ClearanceLevel level)
    : engineerID(id), username(user), encryptedPassword(encryptedPass), clearance(level) {
}

string Engineer::getClearanceName() const {
    return clearanceToString(clearance);
}

bool Engineer::verifyPassword(const string& plainAttempt) const {
    // Encrypt the attempt with the same key and compare cipher-to-cipher --
    // the plain-text password is never reconstructed or compared directly.
    return Encryption::encrypt(plainAttempt) == encryptedPassword;
}

EngineerRecord Engineer::toRecord() const {
    EngineerRecord rec{};
    memset(&rec, 0, sizeof(rec));
    strncpy(rec.engineerID, engineerID.c_str(), ID_LEN - 1);
    strncpy(rec.username, username.c_str(), USERNAME_LEN - 1);
    strncpy(rec.encryptedPassword, encryptedPassword.c_str(), PASSWORD_LEN - 1);
    rec.clearance = static_cast<int>(clearance);
    return rec;
}

void Engineer::fromRecord(const EngineerRecord& rec) {
    engineerID = string(rec.engineerID);
    username = string(rec.username);
    encryptedPassword = string(rec.encryptedPassword);
    clearance = static_cast<ClearanceLevel>(rec.clearance);
}

void Engineer::display() const {
    cout << "  " << engineerID << "  |  " << username
        << "  |  Clearance: " << getClearanceName() << endl;
}

string Engineer::clearanceToString(ClearanceLevel level) {
    switch (level) {
    case ClearanceLevel::LOW:    return "Low";
    case ClearanceLevel::MEDIUM: return "Medium";
    case ClearanceLevel::HIGH:   return "High";
    default:                     return "Unknown";
    }
}

ClearanceLevel Engineer::stringToClearance(const string& text) {
    if (text == "Medium") return ClearanceLevel::MEDIUM;
    if (text == "High")   return ClearanceLevel::HIGH;
    return ClearanceLevel::LOW;
}
