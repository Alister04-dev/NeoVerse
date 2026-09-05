// =============================================================================
// FileManager.h - Declarations for all file I/O operations (persistence)
// =============================================================================
#pragma once
#include "Globals.h"
#include "Engineer.h"
#include "Event.h"
#include "CityLogEntry.h"
#include <vector>
#include <string>
using namespace std;

// -- Plain-text runtime configuration, backed by config.txt -------------------
struct ConfigSettings {
    int maxLoginAttempts;
    int criticalSeverityThreshold;
    int maxSensorReadingsKept;
};

namespace FileManager {

    // -- Initialisation -----------------------------------------------------------
    // Creates all data files with sane defaults if they don't exist yet.
    void initialiseDataFiles();

    // -- Engineer I/O (engineers.dat, binary) --------------------------------------
    vector<Engineer> loadAllEngineers();
    bool saveEngineer(const Engineer& e);      // append one new engineer record
    bool engineerExists(const string& engineerID);

    // -- Event I/O (events.dat, binary) --------------------------------------------
    bool saveEvent(const Event& e, bool wasEmergency = false); // append
    vector<Event> loadAllEvents();

    // -- City log I/O (city_logs.dat, binary) ---------------------------------------
    bool saveCityLog(const CityLogEntry& entry); // append
    vector<CityLogEntry> loadAllCityLogs();

    // -- Config I/O (config.txt, plain text key=value) --------------------------------
    ConfigSettings loadConfig();
    void saveConfig(const ConfigSettings& cfg);

    // -- Export -------------------------------------------------------------------------
    void exportCityLogsToCSV();
    void exportEventsToCSV();

    // -- Helpers --------------------------------------------------------------------------
    string getCurrentTimestamp(); // DD/MM/YYYY HH:MM:SS
}
