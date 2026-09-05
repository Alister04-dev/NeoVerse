// =============================================================================
// Engineer.h - AI Engineer account (login & access control)
// =============================================================================
#pragma once
#include "Globals.h"
#include <string>
using namespace std;

// ============================================================================
//  Engineer - one authorised operator of the city system
// ============================================================================
class Engineer {
private:
    string    engineerID;         // e.g. ENG001
    string    username;
    string    encryptedPassword;  // stored encrypted, never in plain text
    ClearanceLevel clearance;

public:
    // -- Constructor ------------------------------------------------------------
    Engineer(const string& id = "",
        const string& user = "",
        const string& encryptedPass = "",
        ClearanceLevel     level = ClearanceLevel::LOW);

    // -- Getters ------------------------------------------------------------------
    string    getEngineerID()        const { return engineerID; }
    string    getUsername()          const { return username; }
    string    getEncryptedPassword() const { return encryptedPassword; }
    ClearanceLevel getClearance()         const { return clearance; }
    string    getClearanceName()     const;

    // -- Setters ------------------------------------------------------------------
    void setUsername(const string& user) { username = user; }
    void setEncryptedPassword(const string& encryptedPass) { encryptedPassword = encryptedPass; }
    void setClearance(ClearanceLevel level) { clearance = level; }

    // -- Authentication -------------------------------------------------------------
    // Encrypts the attempt and compares it against the stored encrypted password.
    bool verifyPassword(const string& plainAttempt) const;

    // -- Conversion helpers (for binary file I/O) -----------------------------------
    EngineerRecord toRecord() const;
    void           fromRecord(const EngineerRecord& rec);

    // -- Display --------------------------------------------------------------------
    void display() const;

    // -- Free helper: ClearanceLevel <-> string --------------------------------------
    static string clearanceToString(ClearanceLevel level);
    static ClearanceLevel stringToClearance(const string& text);
};
