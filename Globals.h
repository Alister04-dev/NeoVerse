// =============================================================================
// Globals.h - Shared constants, enumerations, and file-record structs
// NeoVerse: AI City Survival System
// =============================================================================
#pragma once

#include <string>
using namespace std;

#define _CRT_SECURE_NO_WARNINGS

// -- Fixed buffer sizes for binary file I/O ----------------------------------
#define ID_LEN          10
#define USERNAME_LEN    30
#define PASSWORD_LEN    100
#define EVENT_ID_LEN     10
#define EVENT_DESC_LEN  150
#define TIMESTAMP_LEN    20
#define SOURCE_LEN       30
#define LOG_DESC_LEN    150

// -- Security settings --------------------------------------------------------
const int MAX_LOGIN_ATTEMPTS = 3;

// -- XOR encryption key --------------------------------------------------------
const string ENCRYPTION_KEY = "NEOVERSE2035";

// -- Critical alert threshold (severity >= this counts as "critical") --------
const int CRITICAL_SEVERITY_THRESHOLD = 4;

// -- File names -----------------------------------------------------------------
const string F_ENGINEERS = "engineers.dat";
const string F_EVENTS = "events.dat";
const string F_CITYLOGS = "city_logs.dat";
const string F_CONFIG = "config.txt";
const string F_EVENTS_CSV = "events_export.csv";
const string F_CITYLOGS_CSV = "city_logs_export.csv";

// -- Clearance level enumeration ------------------------------------------------
enum class ClearanceLevel {
    LOW = 0,
    MEDIUM = 1,
    HIGH = 2
};

// -- Event type enumeration ------------------------------------------------------
enum class EventType {
    TRAFFIC_ACCIDENT = 0,
    POWER_FAILURE = 1,
    NETWORK_OVERLOAD = 2,
    WEATHER_ALERT = 3
};

// ============================================================================
//  FILE-STORAGE RECORDS  (fixed-size structs for binary I/O)
// ============================================================================

struct EngineerRecord {
    char engineerID[ID_LEN];
    char username[USERNAME_LEN];
    char encryptedPassword[PASSWORD_LEN];
    int  clearance;          // cast to/from ClearanceLevel
};

struct EventRecord {
    char eventID[EVENT_ID_LEN];
    int  type;                // cast to/from EventType
    char description[EVENT_DESC_LEN];
    char timestamp[TIMESTAMP_LEN];
    int  severity;             // 1 (minor) - 5 (critical)
    bool resolved;
    int  creationTick;
    int  resolvedTick;         // -1 if not yet resolved
    bool wasEmergencyOverride; // came from the stack rather than the queue
};

struct CityLogRecord {
    char timestamp[TIMESTAMP_LEN];
    char sourceComponent[SOURCE_LEN];
    char description[LOG_DESC_LEN];
};
