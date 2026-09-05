# NeoVerse: AI City Survival System

A city survival console based game (Programming Assignment as a Year 2,
Semester 2 student in completion of his Diploma in Information Technology).

A console-based C++ simulation built for the PROGRAMMING 622 assignment. It models
an AI-managed city (NeoVerse Labs, 2035) using STL containers, a hand-built linked
list, object-oriented design, and file persistence.

## How to run

**Visual Studio:** open `NeoVerse.slnx`, set the configuration to Debug or Release
(x64 or x86), and run (F5). All source files are already included in `NeoVerse.vcxproj`.

**Command line (MinGW/g++):**
```
g++ -std=c++17 -o NeoVerse.exe main.cpp CitySystem.cpp CityComponent.cpp Engineer.cpp Event.cpp Encryption.cpp FileManager.cpp
NeoVerse.exe
```

On first run the program creates `engineers.dat`, `events.dat`, `city_logs.dat`
and `config.txt` in the working directory and seeds two default engineer logins.
Every subsequent run reloads from those same files, so nothing is lost between
sessions.

## Sample login credentials

| Engineer ID | Username  | Password    | Clearance |
|-------------|-----------|-------------|-----------|
| ENG001      | jsmith    | password1   | High      |
| ENG002      | tmokoena  | password2   | Medium    |

Login allows 3 attempts (configurable in `config.txt` via `MaxLoginAttempts`).

## Menu overview

1. **City Data Management** — add/remove/display sensor readings (vector) and
   city log entries (hand-built linked list).
2. **Event Processing** — generate a random city event, process the next queued
   event (FIFO), resolve the top emergency override (LIFO), or filter critical
   events.
3. **City Component Status** — displays each `CityComponent`'s status via the
   polymorphic `getStatus()`/`processEvent()` calls.
4. **STL Algorithm Demonstrations** — runs `sort`, `find`, `min_element`,
   `max_element`, and `count_if` against the sensor readings, printing the
   complexity of each as it runs.
5. **Login Search Complexity Demo** — runs a linear search and a binary search
   for the same engineer ID side by side, printing the comparison count for each.
6. **System Reports & Analytics** — total events processed, most common event
   type, average response time, and current system load.
7. **Export Logs & Events to CSV** — writes `city_logs_export.csv` and
   `events_export.csv`.

## Containers used, and why

| Data | Container | Reasoning |
|---|---|---|
| Engineer roster | `std::vector<Engineer>` | Small, mostly-read list; needs to be sortable for the binary-search demo and iterated for login lookup. |
| Daily sensor readings | `std::vector<SensorReading>` | Accessed by recency and iterated in bulk for the STL algorithm demos — vector's contiguous storage and fast append suit that. |
| Historical city logs | Hand-built `LinkedListType<CityLogEntry>` (`LinkedList.h`) | The log grows for as long as the city runs and is never accessed by index, only appended to or trimmed from the oldest end. A linked list gives O(1) insertion at both operations without ever needing to shift existing entries, which is what an ever-growing, unbounded log needs. Built from scratch (node + first/last/count) rather than using `std::list`, to demonstrate the underlying mechanics as required. |
| Incoming events | `std::queue<Event>` | Events must be handled strictly in arrival order (FIFO) — a queue enforces that directly instead of relying on manual bookkeeping. |
| Emergency overrides | `std::stack<EmergencyEvent>` | The most recently raised emergency must be resolved first (LIFO) — a stack enforces that directly. |
| Event-type tally (reports) | `std::map<EventType, int>` | Needs to look up and increment a count by event type; a map gives that in O(log n) and keeps the tally naturally organised by key, and its iterators are used explicitly in the report (Section 6's "Use: Iterators" requirement). |

## Big-O decisions

- **Login search (Section 1):** the live login itself uses `std::find_if`
  (linear, O(n)) over the unsorted engineer vector — appropriate because the
  roster is small and insertion order shouldn't matter for day-to-day login.
  The separate **Search Complexity Demo** menu additionally sorts a copy of
  the roster and runs a hand-written binary search (O(log n)), printing the
  comparison count for both so the difference is visible, not just asserted.
  At real city scale (thousands of engineers) binary search would be the
  better default, provided the roster is kept sorted.
- **Sensor reading insert/remove (Section 2):** `push_back` on the vector is
  O(1) amortised; removing the oldest reading (`erase(begin())`) is O(n)
  because every remaining element shifts down. This trade-off was accepted
  because readings are added far more often than the oldest one is manually
  trimmed.
- **City log insert/remove (Section 2):** both `insertLast` and `removeOldest`
  on the hand-built linked list are O(1) — the list keeps a `last` pointer so
  appending never has to walk the chain, and removing the front node needs no
  shifting. Displaying/traversing the whole log is O(n), which is unavoidable
  for a structure with no random access.
- **Event queue/stack operations (Section 3):** `push`/`pop`/`front`/`top` on
  `std::queue`/`std::stack` are all O(1) — both are container adapters over a
  `std::deque` by default.
- **Filtering critical events (Section 3):** `std::copy_if` over the full
  event log is a single O(n) pass.
- **STL algorithm demo (Section 5):** `sort` is O(n log n); `find`,
  `min_element`, `max_element`, and `count_if` are each a single O(n) pass.
  These are printed alongside their results in the program itself.
- **Report generation (Section 6):** scanning the event-type tally map for the
  most common type is O(k) where k is the number of distinct event types (at
  most 4 here) — negligible regardless of how many events have been processed,
  since the map only ever holds one entry per `EventType`.

## File persistence

- `engineers.dat`, `events.dat`, `city_logs.dat` — fixed-size binary records
  (`EngineerRecord`, `EventRecord`, `CityLogRecord` in `Globals.h`), written
  and read with `ofstream`/`ifstream` in `std::ios::binary` mode. Engineer
  passwords are never written in plain text — they're encrypted first
  (`Encryption.h`, an XOR-with-rolling-key + Caesar-shift cipher) and only the
  cipher text is compared on login.
- `config.txt` — plain text `key=value` pairs (max login attempts, critical
  severity threshold, max sensor readings kept), human-readable and editable
  without recompiling.
- `events_export.csv` / `city_logs_export.csv` — comma-separated exports of
  the two `.dat` logs, generated on demand from the main menu.

No `.dat`/`.csv`/`config.txt` files are included in this folder — they're
created automatically the first time the program runs (see "How to run"
above).
