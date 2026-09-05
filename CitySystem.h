// =============================================================================
// CitySystem.h - Main system controller: login, menus, event dispatch, reports
// =============================================================================
#pragma once
#include "Globals.h"
#include "Engineer.h"
#include "Event.h"
#include "SensorReading.h"
#include "CityLogEntry.h"
#include "LinkedList.h"
#include "CityComponent.h"
#include "FileManager.h"
#include <vector>
#include <queue>
#include <stack>
#include <map>
#include <string>
using namespace std;

// ============================================================================
//  CitySystem - orchestrates authentication, city data, events, and reports
// ============================================================================
class CitySystem {
private:
    // -- Section 1: Authentication ------------------------------------------------
    vector<Engineer> engineers;
    bool     engineerLoggedIn;
    Engineer currentEngineer;

    // -- Section 2: City data ---------------------------------------------------------
    vector<SensorReading>   sensorReadings;   // vector -- fast, random access by day
    LinkedListType<CityLogEntry> cityLogs;          // linked list -- unbounded, append-only

    // -- Section 3: Event processing -----------------------------------------------------
    queue<Event>            eventQueue;        // FIFO -- normal events, arrival order
    stack<EmergencyEvent>   emergencyStack;     // LIFO -- overrides, most recent first

    // -- Section 4: City components (OOP hierarchy) ----------------------------------------
    vector<CityComponent*> components;

    // -- Runtime configuration & bookkeeping --------------------------------------------------
    ConfigSettings config;
    int simClock;                              // simulated clock, ticks on every event action
    int totalEventsProcessed;
    int totalResponseTicks;                    // sum of (resolvedTick - creationTick)
    map<EventType, int> eventTypeTally; // tallies every resolved event, queue and stack alike

    // -- UI helpers ------------------------------------------------------------------------------
    void clearScreen();
    void printBanner();
    void printMainMenu();
    void pauseScreen();
    int  readIntChoice();   // reads a menu choice safely -- never hangs or spins on bad input
    int  readInt(const string& prompt);
    double readDouble(const string& prompt);

    // -- Section 1: Authentication ------------------------------------------------------------------
    bool loginMenu();
    void searchComplexityDemo();

    // -- Section 2: City data management --------------------------------------------------------------
    void addSensorReading();
    void removeOldestSensorReading();
    void displaySensorReadings();
    void addCityLogEntry(const string& source, const string& description);
    void removeOldestCityLog();
    void displayCityLogs();
    void cityDataMenu();

    // -- Section 3: Event processing ----------------------------------------------------------------------
    void generateRandomEvent();
    void processNextQueuedEvent();
    void resolveTopEmergency();
    void filterCriticalEvents();
    void dispatchToComponents(const Event& e);
    void eventMenu();

    // -- Section 4: OOP city architecture ---------------------------------------------------------------------
    void displayComponentStatuses();

    // -- Section 5: STL algorithms & Big-O justification -------------------------------------------------------------
    void algorithmDemoMenu();

    // -- Section 6: Reports & analytics ------------------------------------------------------------------------------
    void generateSystemReport();

public:
    CitySystem();
    ~CitySystem();
    void run();
};
