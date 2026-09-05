// =============================================================================
// Event.cpp - Event / EmergencyEvent implementation
// =============================================================================
#define _CRT_SECURE_NO_WARNINGS
#include "Event.h"
#include <cstring>
#include <sstream>
#include <iostream>
using namespace std;

Event::Event(const string& id, EventType t, const string& desc,
    const string& time, int sev, int tick)
    : eventID(id), type(t), description(desc), timestamp(time),
    severity(sev), resolved(false), creationTick(tick), resolvedTick(-1) {
}

void Event::display() const {
    cout << "  [" << eventID << "] " << typeToString(type)
        << " (severity " << severity << ") - " << description
        << " @ " << timestamp
        << (resolved ? "  [RESOLVED]" : "  [PENDING]") << endl;
}

EventRecord Event::toRecord(bool wasEmergency) const {
    EventRecord rec{};
    memset(&rec, 0, sizeof(rec));
    strncpy(rec.eventID, eventID.c_str(), EVENT_ID_LEN - 1);
    rec.type = static_cast<int>(type);
    strncpy(rec.description, description.c_str(), EVENT_DESC_LEN - 1);
    strncpy(rec.timestamp, timestamp.c_str(), TIMESTAMP_LEN - 1);
    rec.severity = severity;
    rec.resolved = resolved;
    rec.creationTick = creationTick;
    rec.resolvedTick = resolvedTick;
    rec.wasEmergencyOverride = wasEmergency;
    return rec;
}

Event Event::fromRecord(const EventRecord& rec) {
    Event e(string(rec.eventID), static_cast<EventType>(rec.type),
        string(rec.description), string(rec.timestamp),
        rec.severity, rec.creationTick);
    if (rec.resolved) e.resolve(rec.resolvedTick);
    return e;
}

string Event::typeToString(EventType t) {
    switch (t) {
    case EventType::TRAFFIC_ACCIDENT:  return "Traffic Accident";
    case EventType::POWER_FAILURE:     return "Power Failure";
    case EventType::NETWORK_OVERLOAD:  return "Network Overload";
    case EventType::WEATHER_ALERT:     return "Weather Alert";
    default:                           return "Unknown Event";
    }
}

string Event::generateEventID(int tick) {
    ostringstream oss;
    oss << "EVT" << tick;
    return oss.str();
}

// ============================================================================
//  EmergencyEvent
// ============================================================================
EmergencyEvent::EmergencyEvent(const string& id, EventType t, const string& desc,
    const string& time, int sev, int tick, int priority)
    : Event(id, t, desc, time, sev, tick), overridePriority(priority) {
}

void EmergencyEvent::display() const {
    cout << "  [OVERRIDE p" << overridePriority << "] ";
    Event::display();
}
