// =============================================================================
// CitySystem.cpp - CitySystem implementation
// =============================================================================
#define _CRT_SECURE_NO_WARNINGS
#include "CitySystem.h"
#include "Encryption.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <limits>
using namespace std;

// ============================================================================
//  Construction / Destruction
// ============================================================================
CitySystem::CitySystem()
    : engineerLoggedIn(false), simClock(0), totalEventsProcessed(0), totalResponseTicks(0) {

    FileManager::initialiseDataFiles();
    config = FileManager::loadConfig();
    engineers = FileManager::loadAllEngineers();

    // Rebuild the historical log linked list from city_logs.dat
    vector<CityLogEntry> savedLogs = FileManager::loadAllCityLogs();
    for (size_t i = 0; i < savedLogs.size(); ++i) {
        cityLogs.insertLast(savedLogs[i]);
    }

    // Seed a handful of sensor readings so the STL algorithm demos have data
    // to work with even before an engineer adds any manually. Sensor readings
    // are session-only (not one of the four required persisted files).
    const char* sampleDates[] = { "01/09/2035", "02/09/2035", "03/09/2035", "04/09/2035", "05/09/2035" };
    double sampleEnergy[] = { 812.4, 940.1, 705.8, 1102.6, 875.3 };
    int sampleTraffic[] = { 4200, 5100, 3300, 6400, 4800 };
    int sampleAlerts[] = { 2, 5, 1, 6, 3 };
    for (int i = 0; i < 5; ++i) {
        SensorReading r;
        r.date = sampleDates[i];
        r.populationStat = 2450000 + i * 1200;
        r.energyUsage = sampleEnergy[i];
        r.trafficDensity = sampleTraffic[i];
        r.alertCount = sampleAlerts[i];
        sensorReadings.push_back(r);
    }

    // -- Build the four city components (OOP hierarchy, Section 4) --------------
    components.push_back(new PowerSystem(1, "Power Grid"));
    components.push_back(new TransportSystem(2, "Transport Network"));
    components.push_back(new HealthSystem(3, "Health Response"));
    components.push_back(new SecuritySystem(4, "Security Grid"));
    for (size_t i = 0; i < components.size(); ++i) {
        components[i]->activate();
    }
}

CitySystem::~CitySystem() {
    for (size_t i = 0; i < components.size(); ++i) {
        delete components[i];
    }
    components.clear();
}

// ============================================================================
//  UI helpers
// ============================================================================
void CitySystem::clearScreen() {
    for (int i = 0; i < 3; ++i) cout << endl;
}

void CitySystem::printBanner() {
    cout << "==============================================================\n";
    cout << "   NeoVerse: AI City Survival System  --  NeoVerse Labs, 2035\n";
    cout << "==============================================================\n";
}

void CitySystem::printMainMenu() {
    cout << "\nLogged in as: " << currentEngineer.getUsername()
        << " (" << currentEngineer.getClearanceName() << ")\n";
    cout << "--------------------------------------------------------------\n";
    cout << " 1. City Data Management        (vector + linked list)\n";
    cout << " 2. Event Processing             (queue + stack)\n";
    cout << " 3. City Component Status        (OOP / polymorphism)\n";
    cout << " 4. STL Algorithm Demonstrations (sort/find/min/max/count_if)\n";
    cout << " 5. Login Search Complexity Demo (linear vs binary)\n";
    cout << " 6. System Reports & Analytics\n";
    cout << " 7. Export Logs & Events to CSV\n";
    cout << " 0. Logout & Exit\n";
    cout << "--------------------------------------------------------------\n";
    cout << "Choice: ";
}

void CitySystem::pauseScreen() {
    // The caller (run()'s main loop) has already consumed the trailing
    // newline after reading the menu choice, so the buffer is clean here --
    // a single get() is enough to wait for the user's Enter key.
    cout << "\nPress Enter to continue...";
    cin.get();
}

// Reads one menu choice safely. A plain "cin >> choice" leaves the stream
// stuck in a failed state forever if the input isn't numeric (or the input
// source runs out), which turns "while (choice != 0)" into an infinite,
// non-blocking spin instead of a clean prompt or exit. Clearing the fail
// state -- and treating EOF as a request to back out -- avoids that.
int CitySystem::readIntChoice() {
    int value;
    cin >> value;
    if (cin.fail()) {
        bool hitEOF = cin.eof();
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return hitEOF ? 0 : -1; // EOF backs all the way out; garbage input is just "invalid"
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

int CitySystem::readInt(const string& prompt) {
    cout << prompt;
    int value = 0;
    cin >> value;
    if (cin.fail()) { cin.clear(); value = 0; }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

double CitySystem::readDouble(const string& prompt) {
    cout << prompt;
    double value = 0.0;
    cin >> value;
    if (cin.fail()) { cin.clear(); value = 0.0; }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

// ============================================================================
//  SECTION 1 -- AI Agent Authentication & Access Control
// ============================================================================
bool CitySystem::loginMenu() {
    for (int attempt = 1; attempt <= config.maxLoginAttempts; ++attempt) {
        string username, password;
        cout << "\nUsername: ";
        getline(cin, username);
        cout << "Password: ";
        getline(cin, password);

        // -- Linear search with <algorithm>'s find_if, O(n) worst case --------------
        vector<Engineer>::iterator it = find_if(
            engineers.begin(), engineers.end(),
            [&username](const Engineer& e) { return e.getUsername() == username; }
        );

        if (it != engineers.end() && it->verifyPassword(password)) {
            currentEngineer = *it;
            engineerLoggedIn = true;
            cout << "\nLogin successful. Welcome, " << currentEngineer.getUsername() << ".\n";
            return true;
        }

        cout << "Invalid credentials. Attempt " << attempt << " of "
            << config.maxLoginAttempts << ".\n";
    }
    cout << "\nToo many failed attempts. Access denied.\n";
    return false;
}

// Demonstrates linear vs binary search on the engineer list, tracing each
// comparison so the Big-O difference is visible, not just stated.
void CitySystem::searchComplexityDemo() {
    if (engineers.empty()) {
        cout << "No engineers loaded.\n";
        return;
    }

    cout << "\nSearch which Engineer ID (e.g. ENG002)? ";
    string target;
    getline(cin, target);

    // -- Linear search, O(n) --------------------------------------------------------
    int linearComparisons = 0;
    int linearIndex = -1;
    for (size_t i = 0; i < engineers.size(); ++i) {
        linearComparisons++;
        if (engineers[i].getEngineerID() == target) { linearIndex = static_cast<int>(i); break; }
    }
    cout << "\n[Linear Search]  O(n)\n";
    cout << "  Comparisons made: " << linearComparisons << "\n";
    cout << "  Result: " << (linearIndex >= 0 ? "found" : "not found") << "\n";

    // -- Binary search, O(log n) -- requires a sorted copy first --------------------
    vector<Engineer> sorted = engineers;
    sort(sorted.begin(), sorted.end(),
        [](const Engineer& a, const Engineer& b) { return a.getEngineerID() < b.getEngineerID(); });

    int low = 0, high = static_cast<int>(sorted.size()) - 1;
    int binaryComparisons = 0;
    int binaryIndex = -1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        binaryComparisons++;
        if (sorted[mid].getEngineerID() == target) { binaryIndex = mid; break; }
        else if (sorted[mid].getEngineerID() < target) low = mid + 1;
        else high = mid - 1;
    }
    cout << "\n[Binary Search]  O(log n) -- requires the list to be sorted first\n";
    cout << "  Comparisons made: " << binaryComparisons << "\n";
    cout << "  Result: " << (binaryIndex >= 0 ? "found" : "not found") << "\n";

    cout << "\nWith only " << engineers.size() << " engineers the difference is small, "
        << "but linear search's comparison count grows directly with the roster size while "
        << "binary search's grows with log2(n) -- at real city scale (thousands of engineers) "
        << "binary search stays fast as long as the list is kept sorted.\n";
}

// ============================================================================
//  SECTION 2 -- City Data Management
// ============================================================================
void CitySystem::addSensorReading() {
    SensorReading r;
    cout << "Date (DD/MM/YYYY): ";
    getline(cin, r.date);
    r.populationStat = readInt("Population stat: ");
    r.energyUsage = readDouble("Energy usage (MWh): ");
    r.trafficDensity = readInt("Traffic density: ");
    r.alertCount = readInt("Alert count: ");

    // O(1) amortised -- vector appends at the back without shifting anything.
    sensorReadings.push_back(r);
    cout << "Sensor reading added.\n";

    // Respect the config.txt cap -- once exceeded, the oldest reading is
    // dropped automatically so the vector doesn't grow without bound.
    if (static_cast<int>(sensorReadings.size()) > config.maxSensorReadingsKept) {
        sensorReadings.erase(sensorReadings.begin());
        cout << "(Oldest reading auto-trimmed -- MaxSensorReadingsKept="
            << config.maxSensorReadingsKept << " in config.txt)\n";
    }
}

void CitySystem::removeOldestSensorReading() {
    if (sensorReadings.empty()) {
        cout << "No sensor readings to remove.\n";
        return;
    }
    // O(n) -- removing from the front of a vector shifts every remaining element.
    sensorReadings.erase(sensorReadings.begin());
    cout << "Oldest sensor reading removed.\n";
}

void CitySystem::displaySensorReadings() {
    cout << "\n-- Sensor Readings (vector, " << sensorReadings.size() << " entries) --\n";
    for (size_t i = 0; i < sensorReadings.size(); ++i) {
        sensorReadings[i].display();
    }
}

void CitySystem::addCityLogEntry(const string& source, const string& description) {
    CityLogEntry entry;
    entry.timestamp = FileManager::getCurrentTimestamp();
    entry.sourceComponent = source;
    entry.description = description;

    // O(1) -- the linked list keeps a tail pointer, so append never walks the chain.
    cityLogs.insertLast(entry);
    FileManager::saveCityLog(entry);
}

void CitySystem::removeOldestCityLog() {
    // O(1) -- removing the front node needs no shifting, unlike a vector.
    if (cityLogs.removeOldest()) {
        cout << "Oldest city log entry removed.\n";
    }
    else {
        cout << "City log is empty.\n";
    }
}

void CitySystem::displayCityLogs() {
    cout << "\n-- City Logs (linked list, " << cityLogs.getCount() << " entries) --\n";
    // O(n) -- a linked list must be walked node by node; it has no random access.
    vector<CityLogEntry> snapshot = cityLogs.toVector();
    for (size_t i = 0; i < snapshot.size(); ++i) {
        snapshot[i].display();
    }
}

void CitySystem::cityDataMenu() {
    int choice = -1;
    while (choice != 0) {
        cout << "\n-- City Data Management --\n";
        cout << " 1. Add sensor reading (vector)\n";
        cout << " 2. Remove oldest sensor reading\n";
        cout << " 3. Display sensor readings\n";
        cout << " 4. Add city log entry (linked list)\n";
        cout << " 5. Remove oldest city log entry\n";
        cout << " 6. Display city logs\n";
        cout << " 0. Back\n";
        cout << "Choice: ";
        choice = readIntChoice();

        switch (choice) {
        case 1: addSensorReading(); break;
        case 2: removeOldestSensorReading(); break;
        case 3: displaySensorReadings(); break;
        case 4: addCityLogEntry("Engineer", "Manual log entry by " + currentEngineer.getUsername()); break;
        case 5: removeOldestCityLog(); break;
        case 6: displayCityLogs(); break;
        case 0: break;
        default: cout << "Invalid choice.\n";
        }
    }
}

// ============================================================================
//  SECTION 3 -- Event Processing System
// ============================================================================
void CitySystem::generateRandomEvent() {
    EventType type = static_cast<EventType>(rand() % 4);
    int severity = 1 + (rand() % 5);
    simClock++;

    string desc;
    switch (type) {
    case EventType::TRAFFIC_ACCIDENT: desc = "Multi-vehicle collision reported"; break;
    case EventType::POWER_FAILURE:    desc = "Substation output dropping"; break;
    case EventType::NETWORK_OVERLOAD: desc = "Traffic spike on city network"; break;
    case EventType::WEATHER_ALERT:    desc = "Severe weather system approaching"; break;
    }

    string id = Event::generateEventID(simClock);
    string timestamp = FileManager::getCurrentTimestamp();

    if (severity >= config.criticalSeverityThreshold) {
        // Critical events skip the queue and go straight onto the emergency
        // override stack, so the newest critical event is resolved first.
        EmergencyEvent e(id, type, desc, timestamp, severity, simClock, severity);
        emergencyStack.push(e);
        FileManager::saveEvent(e, true);
        cout << "\n[EMERGENCY OVERRIDE] ";
        e.display();
    }
    else {
        Event e(id, type, desc, timestamp, severity, simClock);
        eventQueue.push(e);
        FileManager::saveEvent(e, false);
        cout << "\n[NEW EVENT] ";
        e.display();
    }

    addCityLogEntry("EventGenerator", "Generated " + Event::typeToString(type) + " (" + id + ")");
}

// Runs the same virtual call against every component -- each override decides
// for itself whether the event falls in its domain. This single call site
// resolving to four different behaviours at runtime is the polymorphism
// required by Section 4.
void CitySystem::dispatchToComponents(const Event& e) {
    for (size_t i = 0; i < components.size(); ++i) {
        components[i]->processEvent(e);
    }
}

void CitySystem::processNextQueuedEvent() {
    if (eventQueue.empty()) {
        cout << "Event queue is empty.\n";
        return;
    }
    Event e = eventQueue.front();
    eventQueue.pop(); // O(1) -- FIFO removal from the front
    simClock++;
    e.resolve(simClock);

    dispatchToComponents(e);

    totalEventsProcessed++;
    totalResponseTicks += (e.getResolvedTick() - e.getCreationTick());
    eventTypeTally[e.getType()]++;

    FileManager::saveEvent(e, false);
    addCityLogEntry("EventQueue", "Resolved " + Event::typeToString(e.getType()) + " (" + e.getEventID() + ")");
    cout << "Event resolved.\n";
}

void CitySystem::resolveTopEmergency() {
    if (emergencyStack.empty()) {
        cout << "No emergency overrides pending.\n";
        return;
    }
    EmergencyEvent e = emergencyStack.top();
    emergencyStack.pop(); // O(1) -- LIFO removal from the top
    simClock++;
    e.resolve(simClock);

    dispatchToComponents(e);

    totalEventsProcessed++;
    totalResponseTicks += (e.getResolvedTick() - e.getCreationTick());
    eventTypeTally[e.getType()]++;

    FileManager::saveEvent(e, true);
    addCityLogEntry("EmergencyStack", "Resolved override " + Event::typeToString(e.getType()) + " (" + e.getEventID() + ")");
    cout << "Emergency override resolved.\n";
}

// Filters the persisted event log down to critical-severity events using
// <algorithm>'s copy_if -- O(n), one pass over the log.
void CitySystem::filterCriticalEvents() {
    vector<Event> allEvents = FileManager::loadAllEvents();
    vector<Event> critical;
    copy_if(allEvents.begin(), allEvents.end(), back_inserter(critical),
        [](const Event& e) { return e.isCritical(); });

    cout << "\n-- Critical Events (severity >= " << CRITICAL_SEVERITY_THRESHOLD
        << ") --  [" << critical.size() << " of " << allEvents.size() << " total]\n";
    for (size_t i = 0; i < critical.size(); ++i) {
        critical[i].display();
    }
}

void CitySystem::eventMenu() {
    int choice = -1;
    while (choice != 0) {
        cout << "\n-- Event Processing --\n";
        cout << " 1. Generate a random city event\n";
        cout << " 2. Process next queued event (FIFO)\n";
        cout << " 3. Resolve top emergency override (LIFO)\n";
        cout << " 4. Filter critical events (<algorithm>)\n";
        cout << " 5. Show queue / stack sizes\n";
        cout << " 0. Back\n";
        cout << "Choice: ";
        choice = readIntChoice();

        switch (choice) {
        case 1: generateRandomEvent(); break;
        case 2: processNextQueuedEvent(); break;
        case 3: resolveTopEmergency(); break;
        case 4: filterCriticalEvents(); break;
        case 5:
            cout << "Event queue size: " << eventQueue.size()
                << " | Emergency stack size: " << emergencyStack.size() << "\n";
            break;
        case 0: break;
        default: cout << "Invalid choice.\n";
        }
    }
}

// ============================================================================
//  SECTION 4 -- Object-Oriented City Architecture
// ============================================================================
void CitySystem::displayComponentStatuses() {
    cout << "\n-- City Component Status (polymorphic getStatus()) --\n";
    for (size_t i = 0; i < components.size(); ++i) {
        // Same call, different underlying object -- resolved via the vtable.
        cout << "  " << components[i]->getStatus() << endl;
    }

    PowerSystem* power = dynamic_cast<PowerSystem*>(components[0]);
    TransportSystem* transport = dynamic_cast<TransportSystem*>(components[1]);
    SecuritySystem* security = dynamic_cast<SecuritySystem*>(components[3]);
    if (power)     cout << "  Power level: " << power->getPowerLevel() << "%\n";
    if (transport) cout << "  Traffic flow: " << transport->getTrafficFlow() << "%\n";
    if (security)  cout << "  Threat level: " << security->getThreatLevel() << "\n";
}

// ============================================================================
//  SECTION 5 -- STL Algorithms & Performance Optimisation
// ============================================================================
void CitySystem::algorithmDemoMenu() {
    if (sensorReadings.empty()) {
        cout << "No sensor readings available.\n";
        return;
    }

    // -- sort -- O(n log n), a hybrid divide-and-conquer sort under the hood ------
    vector<SensorReading> byEnergy = sensorReadings;
    sort(byEnergy.begin(), byEnergy.end(),
        [](const SensorReading& a, const SensorReading& b) { return a.energyUsage < b.energyUsage; });
    cout << "\n[sort]  O(n log n) -- sensor readings ordered by energy usage:\n";
    for (size_t i = 0; i < byEnergy.size(); ++i) byEnergy[i].display();

    // -- find -- O(n), a single linear pass looking for an exact match -----------
    cout << "\nEnter a date to find (DD/MM/YYYY): ";
    string searchDate;
    getline(cin, searchDate);
    vector<SensorReading>::iterator found = find_if(
        sensorReadings.begin(), sensorReadings.end(),
        [&searchDate](const SensorReading& r) { return r.date == searchDate; }
    );
    cout << "[find]  O(n) -- ";
    if (found != sensorReadings.end()) { cout << "match: "; found->display(); }
    else cout << "no reading found for that date.\n";

    // -- min_element / max_element -- O(n) each, one pass tracking the extreme ----
    vector<SensorReading>::iterator minIt = min_element(
        sensorReadings.begin(), sensorReadings.end(),
        [](const SensorReading& a, const SensorReading& b) { return a.energyUsage < b.energyUsage; });
    vector<SensorReading>::iterator maxIt = max_element(
        sensorReadings.begin(), sensorReadings.end(),
        [](const SensorReading& a, const SensorReading& b) { return a.energyUsage < b.energyUsage; });
    cout << "\n[min_element]  O(n) -- lowest energy usage day: ";
    minIt->display();
    cout << "[max_element]  O(n) -- highest energy usage day: ";
    maxIt->display();

    // -- count_if -- O(n), one pass with a predicate ------------------------------
    int highAlertDays = count_if(
        sensorReadings.begin(), sensorReadings.end(),
        [](const SensorReading& r) { return r.alertCount >= 3; });
    cout << "\n[count_if]  O(n) -- days with 3+ alerts: " << highAlertDays << "\n";
}

// ============================================================================
//  SECTION 6 -- System Reports & Analytics
// ============================================================================
void CitySystem::generateSystemReport() {
    cout << "\n============== SYSTEM REPORT ==============\n";
    cout << "Total events processed: " << totalEventsProcessed << "\n";

    // -- Most common emergency type -- iterate the tally map with an iterator,
    //    then max_element with a comparator over the {type, count} pairs.
    if (!eventTypeTally.empty()) {
        map<EventType, int>::iterator best = eventTypeTally.begin();
        for (map<EventType, int>::iterator it = eventTypeTally.begin();
            it != eventTypeTally.end(); ++it) {
            if (it->second > best->second) best = it;
        }
        cout << "Most common emergency type: " << Event::typeToString(best->first)
            << " (" << best->second << " occurrences)\n";
    }
    else {
        cout << "Most common emergency type: n/a (no events processed yet)\n";
    }

    // -- Average response time (in simulated ticks) --------------------------------
    if (totalEventsProcessed > 0) {
        double avg = static_cast<double>(totalResponseTicks) / totalEventsProcessed;
        cout << "Average response time: " << avg << " simulated tick(s)\n";
    }
    else {
        cout << "Average response time: n/a\n";
    }

    // -- System load summary --------------------------------------------------------
    int activeComponents = static_cast<int>(count_if(
        components.begin(), components.end(),
        [](CityComponent* c) { return c->isActive(); }));

    cout << "\n-- System Load Summary --\n";
    cout << "  Pending queued events:      " << eventQueue.size() << "\n";
    cout << "  Pending emergency overrides: " << emergencyStack.size() << "\n";
    cout << "  Sensor readings held:        " << sensorReadings.size() << "\n";
    cout << "  City log entries held:       " << cityLogs.getCount() << "\n";
    cout << "  Active components:           " << activeComponents << " / " << components.size() << "\n";
    cout << "=============================================\n";
}

// ============================================================================
//  Main run loop
// ============================================================================
void CitySystem::run() {
    printBanner();

    if (!loginMenu()) return;

    int choice = -1;
    while (choice != 0) {
        printMainMenu();
        choice = readIntChoice();

        switch (choice) {
        case 1: cityDataMenu(); break;
        case 2: eventMenu(); break;
        case 3: displayComponentStatuses(); pauseScreen(); break;
        case 4: algorithmDemoMenu(); pauseScreen(); break;
        case 5: searchComplexityDemo(); pauseScreen(); break;
        case 6: generateSystemReport(); pauseScreen(); break;
        case 7:
            FileManager::exportCityLogsToCSV();
            FileManager::exportEventsToCSV();
            pauseScreen();
            break;
        case 0: cout << "Logging out. City systems remain running.\n"; break;
        default: cout << "Invalid choice.\n";
        }
    }
}
