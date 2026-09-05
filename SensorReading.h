// =============================================================================
// SensorReading.h - One day's worth of city sensor data
// Stored in a vector: readings are appended daily and accessed by index/date,
// so vector's fast random access and contiguous memory suit it well.
// =============================================================================
#pragma once
#include <string>
#include <iostream>
using namespace std;

struct SensorReading {
    string date;
    int         populationStat;
    double      energyUsage;    // MWh
    int         trafficDensity; // vehicles counted
    int         alertCount;     // alerts raised that day

    void display() const {
        cout << "  " << date
            << "  | Pop: " << populationStat
            << "  | Energy: " << energyUsage << " MWh"
            << "  | Traffic: " << trafficDensity
            << "  | Alerts: " << alertCount << endl;
    }
};
