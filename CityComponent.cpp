// =============================================================================
// CityComponent.cpp - Base + derived subsystem implementations
// =============================================================================
#include "CityComponent.h"
#include <iostream>
#include <algorithm>
using namespace std;

// ============================================================================
//  CityComponent (base)
// ============================================================================
CityComponent::CityComponent(int id, const string& n)
    : componentID(id), name(n), active(false) {
}

void CityComponent::activate() {
    active = true;
    cout << "  [" << name << "] activated." << endl;
}

void CityComponent::deactivate() {
    active = false;
    cout << "  [" << name << "] deactivated." << endl;
}

string CityComponent::getStatus() const {
    return name + (active ? " - ONLINE" : " - OFFLINE");
}

// ============================================================================
//  PowerSystem
// ============================================================================
PowerSystem::PowerSystem(int id, const string& n, double level)
    : CityComponent(id, n), powerLevel(level) {
}

void PowerSystem::supplyPower(double amount) {
    powerLevel = min(100.0, powerLevel + amount);
}

void PowerSystem::processEvent(const Event& e) {
    if (e.getType() != EventType::POWER_FAILURE) return; // not our domain

    double drop = e.getSeverity() * 5.0;
    powerLevel = max(0.0, powerLevel - drop);
    cout << "  [PowerSystem] handling power failure -- power level now "
        << powerLevel << "%." << endl;
}

// ============================================================================
//  TransportSystem
// ============================================================================
TransportSystem::TransportSystem(int id, const string& n, int flow)
    : CityComponent(id, n), trafficFlow(flow) {
}

void TransportSystem::manageTraffic(int adjustment) {
    trafficFlow = min(100, max(0, trafficFlow + adjustment));
}

void TransportSystem::processEvent(const Event& e) {
    if (e.getType() != EventType::TRAFFIC_ACCIDENT) return; // not our domain

    int drop = e.getSeverity() * 8;
    manageTraffic(-drop);
    cout << "  [TransportSystem] rerouting around accident -- traffic flow now "
        << trafficFlow << "%." << endl;
}

// ============================================================================
//  HealthSystem
// ============================================================================
HealthSystem::HealthSystem(int id, const string& n, int hospitals)
    : CityComponent(id, n), hospitalCnt(hospitals) {
}

void HealthSystem::provideCare() {
    cout << "  [HealthSystem] " << hospitalCnt << " hospitals on standby." << endl;
}

void HealthSystem::processEvent(const Event& e) {
    if (e.getType() != EventType::WEATHER_ALERT) return; // not our domain

    cout << "  [HealthSystem] mobilising emergency response for weather alert (severity "
        << e.getSeverity() << ")." << endl;
}

// ============================================================================
//  SecuritySystem
// ============================================================================
SecuritySystem::SecuritySystem(int id, const string& n, int threat)
    : CityComponent(id, n), threatLevel(threat) {
}

void SecuritySystem::monitorCity() {
    cout << "  [SecuritySystem] current threat level: " << threatLevel << endl;
}

void SecuritySystem::processEvent(const Event& e) {
    if (e.getType() != EventType::NETWORK_OVERLOAD) return; // not our domain

    threatLevel = min(100, threatLevel + e.getSeverity() * 6);
    cout << "  [SecuritySystem] containing network overload -- threat level now "
        << threatLevel << "." << endl;
}
