#include "table.hh"

Entry *Table::find_entry(Addr pc){
    // Linear search, which is kinda bad, but this could be implemented as content-addressable memory in real hardware
    for (std::list<Entry *>::iterator iter = entries.begin(); iter != entries.end(); iter++){
        if(pc == (*iter)->pc){
            return *iter;
        }
    }
    return NULL;
}
