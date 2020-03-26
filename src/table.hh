#pragma once

#include <deque>
#include <list>

#include "interface.hh"

typedef int64_t Delta;

// DCPT table entry
struct Entry {
    Addr pc;
    Addr last_address;
    Addr last_prefetch;
    std::deque<Delta> deltas;
};

// DCPT table
struct Table {
    std::list<Entry *> entries;
    // Find the entry for a PC in the table
    Entry *find_entry(Addr pc);
};
