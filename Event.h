// =============================================================================
// Event.h - City events (queue) and emergency overrides (stack)
// =============================================================================
#pragma once
#include "Globals.h"
#include <string>
using namespace std;

// ============================================================================
//  Event - one occurrence in the city, processed in arrival order (FIFO)
// ============================================================================
class Event {
protected:
    string eventID;
    EventType   type;
    string description;
    string timestamp;
    int         severity;      // 1 (minor) .. 5 (critical)
    bool        resolved;
    int         creationTick;  // simulated clock tick when the event was generated
    int         resolvedTick;  // simulated clock tick when it was resolved, -1 until then

public:
    // -- Constructor ------------------------------------------------------------
    Event(const string& id = "", EventType t = EventType::TRAFFIC_ACCIDENT,
        const string& desc = "", const string& time = "",
        int sev = 1, int tick = 0);

    virtual ~Event() = default;

    // -- Getters ------------------------------------------------------------------
    string getEventID()     const { return eventID; }
    EventType   getType()        const { return type; }
    string getDescription() const { return description; }
    string getTimestamp()   const { return timestamp; }
    int         getSeverity()    const { return severity; }
    bool        isResolved()     const { return resolved; }
    int         getCreationTick() const { return creationTick; }
    int         getResolvedTick() const { return resolvedTick; }
    bool        isCritical()     const { return severity >= CRITICAL_SEVERITY_THRESHOLD; }

    // -- Mutators -------------------------------------------------------------------
    void resolve(int tick) { resolved = true; resolvedTick = tick; }

    // -- Display ----------------------------------------------------------------------
    virtual void display() const;

    // -- Conversion helpers (for binary file I/O) --------------------------------------
    EventRecord toRecord(bool wasEmergency = false) const;
    static Event fromRecord(const EventRecord& rec);

    // -- Free helpers ---------------------------------------------------------------------
    static string typeToString(EventType t);
    static string generateEventID(int tick);
};

// ============================================================================
//  EmergencyEvent - a higher-priority override, resolved LIFO (most recent first)
// ============================================================================
class EmergencyEvent : public Event {
private:
    int overridePriority;  // higher = more urgent within the override stack

public:
    EmergencyEvent(const string& id = "", EventType t = EventType::POWER_FAILURE,
        const string& desc = "", const string& time = "",
        int sev = 5, int tick = 0, int priority = 1);

    int getOverridePriority() const { return overridePriority; }

    void display() const override;
};
