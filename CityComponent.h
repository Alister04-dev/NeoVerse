// =============================================================================
// CityComponent.h - Base CityComponent class and four derived subsystems
// =============================================================================
#pragma once
#include "Event.h"
#include <string>
using namespace std;

// ============================================================================
//  Base CityComponent class
// ============================================================================
class CityComponent {
protected:
    int         componentID;
    string name;
    bool        active;

public:
    CityComponent(int id = 0, const string& n = "");
    virtual ~CityComponent() = default;

    virtual void        activate();
    virtual void        deactivate();
    virtual string getStatus() const;

    // Every subsystem must react to city events in its own way -- this is
    // what each derived class overrides to demonstrate polymorphism.
    virtual void processEvent(const Event& e) = 0;

    int         getComponentID() const { return componentID; }
    string getName()        const { return name; }
    bool        isActive()       const { return active; }
};

// ============================================================================
//  PowerSystem - reacts to POWER_FAILURE events
// ============================================================================
class PowerSystem : public CityComponent {
private:
    double powerLevel; // percentage, 0-100

public:
    PowerSystem(int id, const string& n, double level = 100.0);
    void supplyPower(double amount);
    void processEvent(const Event& e) override;
    double getPowerLevel() const { return powerLevel; }
};

// ============================================================================
//  TransportSystem - reacts to TRAFFIC_ACCIDENT events
// ============================================================================
class TransportSystem : public CityComponent {
private:
    int trafficFlow; // percentage of normal flow, 0-100

public:
    TransportSystem(int id, const string& n, int flow = 100);
    void manageTraffic(int adjustment);
    void processEvent(const Event& e) override;
    int getTrafficFlow() const { return trafficFlow; }
};

// ============================================================================
//  HealthSystem - reacts to WEATHER_ALERT events (emergency response capacity)
// ============================================================================
class HealthSystem : public CityComponent {
private:
    int hospitalCnt;

public:
    HealthSystem(int id, const string& n, int hospitals = 5);
    void provideCare();
    void processEvent(const Event& e) override;
    int getHospitalCount() const { return hospitalCnt; }
};

// ============================================================================
//  SecuritySystem - reacts to NETWORK_OVERLOAD events
// ============================================================================
class SecuritySystem : public CityComponent {
private:
    int threatLevel; // 0 (calm) - 100 (critical)

public:
    SecuritySystem(int id, const string& n, int threat = 0);
    void monitorCity();
    void processEvent(const Event& e) override;
    int getThreatLevel() const { return threatLevel; }
};
