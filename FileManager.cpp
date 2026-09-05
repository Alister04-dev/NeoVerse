// =============================================================================
// FileManager.cpp - File I/O implementation
// =============================================================================
#define _CRT_SECURE_NO_WARNINGS
#include "FileManager.h"
#include "Encryption.h"
#include <fstream>
#include <sstream>
#include <cstring>
#include <ctime>
#include <iostream>
using namespace std;

namespace FileManager {

    // -- Helpers ------------------------------------------------------------------
    string getCurrentTimestamp() {
        time_t t = time(nullptr);
        tm* tm_info = localtime(&t);
        char buf[20];
        strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M:%S", tm_info);
        return string(buf);
    }

    // ============================================================================
    //  INITIALISATION - seed engineers.dat with two default logins on first run
    // ============================================================================
    void initialiseDataFiles() {
        // -- engineers.dat --------------------------------------------------------
        {
            ifstream test(F_ENGINEERS, ios::binary);
            if (!test.good()) {
                test.close();
                Engineer eng1("ENG001", "jsmith", Encryption::encrypt("password1"), ClearanceLevel::HIGH);
                Engineer eng2("ENG002", "tmokoena", Encryption::encrypt("password2"), ClearanceLevel::MEDIUM);
                saveEngineer(eng1);
                saveEngineer(eng2);
                cout << "  [INIT] engineers.dat created with 2 default logins.\n";
            }
        }

        // -- events.dat / city_logs.dat -- just need to exist ----------------------
        {
            ifstream test(F_EVENTS, ios::binary);
            if (!test.good()) {
                test.close();
                ofstream f(F_EVENTS, ios::binary | ios::trunc);
                cout << "  [INIT] events.dat created.\n";
            }
        }
        {
            ifstream test(F_CITYLOGS, ios::binary);
            if (!test.good()) {
                test.close();
                ofstream f(F_CITYLOGS, ios::binary | ios::trunc);
                cout << "  [INIT] city_logs.dat created.\n";
            }
        }

        // -- config.txt -------------------------------------------------------------
        {
            ifstream test(F_CONFIG);
            if (!test.good()) {
                test.close();
                ConfigSettings defaults{ MAX_LOGIN_ATTEMPTS, CRITICAL_SEVERITY_THRESHOLD, 30 };
                saveConfig(defaults);
                cout << "  [INIT] config.txt created with default settings.\n";
            }
        }
    }

    // ============================================================================
    //  ENGINEER I/O
    // ============================================================================
    vector<Engineer> loadAllEngineers() {
        vector<Engineer> result;
        ifstream f(F_ENGINEERS, ios::binary);
        if (!f.good()) return result;

        EngineerRecord rec{};
        while (f.read(reinterpret_cast<char*>(&rec), sizeof(EngineerRecord))) {
            Engineer e;
            e.fromRecord(rec);
            result.push_back(e);
        }
        return result;
    }

    bool saveEngineer(const Engineer& e) {
        ofstream f(F_ENGINEERS, ios::binary | ios::app);
        if (!f.good()) return false;
        EngineerRecord rec = e.toRecord();
        f.write(reinterpret_cast<char*>(&rec), sizeof(EngineerRecord));
        return true;
    }

    bool engineerExists(const string& engineerID) {
        vector<Engineer> all = loadAllEngineers();
        for (size_t i = 0; i < all.size(); ++i) {
            if (all[i].getEngineerID() == engineerID) return true;
        }
        return false;
    }

    // ============================================================================
    //  EVENT I/O
    // ============================================================================
    bool saveEvent(const Event& e, bool wasEmergency) {
        ofstream f(F_EVENTS, ios::binary | ios::app);
        if (!f.good()) return false;
        EventRecord rec = e.toRecord(wasEmergency);
        f.write(reinterpret_cast<char*>(&rec), sizeof(EventRecord));
        return true;
    }

    vector<Event> loadAllEvents() {
        vector<Event> result;
        ifstream f(F_EVENTS, ios::binary);
        if (!f.good()) return result;

        EventRecord rec{};
        while (f.read(reinterpret_cast<char*>(&rec), sizeof(EventRecord))) {
            result.push_back(Event::fromRecord(rec));
        }
        return result;
    }

    // ============================================================================
    //  CITY LOG I/O
    // ============================================================================
    bool saveCityLog(const CityLogEntry& entry) {
        ofstream f(F_CITYLOGS, ios::binary | ios::app);
        if (!f.good()) return false;

        CityLogRecord rec{};
        memset(&rec, 0, sizeof(rec));
        strncpy(rec.timestamp, entry.timestamp.c_str(), TIMESTAMP_LEN - 1);
        strncpy(rec.sourceComponent, entry.sourceComponent.c_str(), SOURCE_LEN - 1);
        strncpy(rec.description, entry.description.c_str(), LOG_DESC_LEN - 1);

        f.write(reinterpret_cast<char*>(&rec), sizeof(CityLogRecord));
        return true;
    }

    vector<CityLogEntry> loadAllCityLogs() {
        vector<CityLogEntry> result;
        ifstream f(F_CITYLOGS, ios::binary);
        if (!f.good()) return result;

        CityLogRecord rec{};
        while (f.read(reinterpret_cast<char*>(&rec), sizeof(CityLogRecord))) {
            CityLogEntry entry;
            entry.timestamp = string(rec.timestamp);
            entry.sourceComponent = string(rec.sourceComponent);
            entry.description = string(rec.description);
            result.push_back(entry);
        }
        return result;
    }

    // ============================================================================
    //  CONFIG I/O  (plain text, key=value)
    // ============================================================================
    ConfigSettings loadConfig() {
        ConfigSettings cfg{ MAX_LOGIN_ATTEMPTS, CRITICAL_SEVERITY_THRESHOLD, 30 };
        ifstream f(F_CONFIG);
        if (!f.good()) return cfg;

        string line;
        while (getline(f, line)) {
            size_t eq = line.find('=');
            if (eq == string::npos) continue;
            string key = line.substr(0, eq);
            string value = line.substr(eq + 1);

            if (key == "MaxLoginAttempts")            cfg.maxLoginAttempts = stoi(value);
            else if (key == "CriticalSeverityThreshold") cfg.criticalSeverityThreshold = stoi(value);
            else if (key == "MaxSensorReadingsKept")     cfg.maxSensorReadingsKept = stoi(value);
        }
        return cfg;
    }

    void saveConfig(const ConfigSettings& cfg) {
        ofstream f(F_CONFIG, ios::trunc);
        if (!f.good()) return;
        f << "MaxLoginAttempts=" << cfg.maxLoginAttempts << "\n";
        f << "CriticalSeverityThreshold=" << cfg.criticalSeverityThreshold << "\n";
        f << "MaxSensorReadingsKept=" << cfg.maxSensorReadingsKept << "\n";
    }

    // ============================================================================
    //  EXPORT
    // ============================================================================
    void exportCityLogsToCSV() {
        vector<CityLogEntry> logs = loadAllCityLogs();
        ofstream f(F_CITYLOGS_CSV, ios::trunc);
        if (!f.good()) return;

        f << "Timestamp,SourceComponent,Description\n";
        for (size_t i = 0; i < logs.size(); ++i) {
            f << logs[i].timestamp << "," << logs[i].sourceComponent << ","
                << logs[i].description << "\n";
        }
        cout << "  Exported " << logs.size() << " log entries to " << F_CITYLOGS_CSV << endl;
    }

    void exportEventsToCSV() {
        vector<Event> events = loadAllEvents();
        ofstream f(F_EVENTS_CSV, ios::trunc);
        if (!f.good()) return;

        f << "EventID,Type,Description,Timestamp,Severity,Resolved\n";
        for (size_t i = 0; i < events.size(); ++i) {
            f << events[i].getEventID() << "," << Event::typeToString(events[i].getType()) << ","
                << events[i].getDescription() << "," << events[i].getTimestamp() << ","
                << events[i].getSeverity() << "," << (events[i].isResolved() ? "Yes" : "No") << "\n";
        }
        cout << "  Exported " << events.size() << " events to " << F_EVENTS_CSV << endl;
    }

} // namespace FileManager
