// =============================================================================
// CityLogEntry.h - One historical log record, held in the CityLogList linked list
// A linked list is used here rather than a vector because the log grows for as
// long as the city runs (unbounded), and every new entry is only ever appended
// or trimmed from the oldest end -- it never needs random access by index, so
// the O(1) insertion a linked list gives up front outweighs vector's O(1) access.
// =============================================================================
#pragma once
#include <string>
#include <iostream>
using namespace std;

struct CityLogEntry {
    string timestamp;
    string sourceComponent;
    string description;

    void display() const {
        cout << "  [" << timestamp << "] " << sourceComponent
            << ": " << description << endl;
    }
};
