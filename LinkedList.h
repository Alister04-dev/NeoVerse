// =============================================================================
// LinkedList.h - Hand-built singly linked list ADT, used for the city's
// unbounded historical log (city_logs.dat backs this at runtime).
// Template so the same list could hold any record type, same idea as the STL
// containers, but built from scratch here to show the underlying mechanics.
// =============================================================================
#pragma once
#include <vector>
using namespace std;

template <class Type>
struct LogNode {
    Type data;
    LogNode<Type>* next;
};

template <class Type>
class LinkedListType {
private:
    LogNode<Type>* first;  // pointer to the oldest entry
    LogNode<Type>* last;   // pointer to the newest entry -- keeps insertLast O(1)
    int count;

public:
    LinkedListType() {
        first = nullptr;
        last = nullptr;
        count = 0;
    }

    ~LinkedListType() {
        LogNode<Type>* current = first;
        while (current != nullptr) {
            LogNode<Type>* temp = current;
            current = current->next;
            delete temp; // free every node before the list itself dies
        }
    }

    bool isEmpty() const {
        return (first == nullptr);
    }

    int getCount() const {
        return count;
    }

    // Append at the back -- O(1) because we keep a last pointer instead of
    // walking the whole chain to find the end every time.
    void insertLast(const Type& value) {
        LogNode<Type>* newNode = new LogNode<Type>;
        newNode->data = value;
        newNode->next = nullptr;

        if (isEmpty()) {
            first = newNode;
            last = newNode;
        }
        else {
            last->next = newNode;
            last = newNode;
        }
        count++;
    }

    // Remove the oldest entry (the front node) -- O(1), no shifting required.
    bool removeOldest() {
        if (isEmpty()) return false; // guardrail

        LogNode<Type>* temp = first;
        first = first->next;
        if (first == nullptr) last = nullptr; // list just became empty
        delete temp;
        count--;
        return true;
    }

    // Traverse every node exactly once, from oldest to newest -- O(n).
    void traverse(void (*visit)(const Type&)) const {
        LogNode<Type>* current = first;
        while (current != nullptr) {
            visit(current->data);
            current = current->next;
        }
    }

    // Snapshot into a vector so STL algorithms (sort/find/etc.) can be run
    // against the log without changing the linked list itself.
    vector<Type> toVector() const {
        vector<Type> result;
        result.reserve(count);
        LogNode<Type>* current = first;
        while (current != nullptr) {
            result.push_back(current->data);
            current = current->next;
        }
        return result;
    }
};
