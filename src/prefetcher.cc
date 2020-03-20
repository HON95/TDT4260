/*
 * Prefetcher using the Delta Correlattion Prediction Table (DCPT) Algorith,
 * 
 */

#include "interface.hh"
#include <algorithm>
#include <iostream>
#include <stack>
#include <string>
#include <sstream>
#include <list>

using namespace std;
typedef uint64 Delta;

//Declarations:
std::list <Addr> prefetches;
std::list <Addr> candidates;


//This is the struct that represents an enty in the DCPT table. Struct instead of class to have all member be public.
struct entry{
    entry(Addr pc); //constructor
    Addr PC;
    Addr lastAddr;
    Addr lastPrefetch;
    deque <Delta> deltas; 
};

//This struct is the table that holds all the table entries of the DCPT. Struct instead of class to have all member be public.

struct dcptable {
    dcptable(); //constructor
    entry find_entry(Addr pc);
    list<entry *> entries;
    int num_of_entries;
};

static dcptable *table; //allocate static memory for the table 

void prefetch_init(void){
    /* Called before any calls to prefetch_access. */
    /* This is the place to initialize data structures. */
    table  = new dcptable;
    DPRINTF(HWPrefetch, "Initialized sequential-on-access prefetcher\n");
}

//Function to find an entry in the table from a search with PC
entry *dcptable::find_entry (Addr pc){
    for (std::list<entry *>::iterator it = entries.begin(); i != entries.end(); i++){
        entry *temp = *i; //convert type
        if(pc == temp->pc){
            return temp;
        }
    }
return NULL; //If no entry is found with the current PC
}

int DeltaCorrelation(entry entry){ //From paper on DCPT
//TODO: fill inn this function acording to paper psaudocode 
// (Delta ias typedefined at the top of this file!)
return candidates[];
}

int PrefetchFilter(entry entry, candidates[]){ //From paper on DCPT
//TODO: fill inn this function acording to paper psaudocode
return prefetches[];
}

void prefetch_access(AccessStat stat){ 
//Try to implement Algorithm 1, Main flow from "Storage Efficient Hardware
//Prefetching using Delta-Correlating Prediction Tables" ("DCPT paper")

/*Original code: 
Addr pf_addr = stat.mem_addr + BLOCK_SIZE;
    if (stat.miss && !in_cache(pf_addr)) {
        issue_prefetch(pf_addr);
    }
*/
  
    entry *curr_e = table->find_entry(stat.pc); //See if the PC corresponds to an existing entry
    
    if (curr_e == NULL){  //make new entry 
        if (dcptable.num_of_entries >= MAX_num_of_entries){
            // TODO: --> Remove oldest entry
        }
    curr_e.PC = stat.pc;
    curr_e.lastAddr = stat.addr;
    curr_e.deltas = NULL;
    curr_e.lastPrefetch = 0;
    }
    else if((stat.addr - curr_e.lastAddr) != 0){
        curr_e.deltas = stat.addr - curr_e.lastAddr;
        curr_e.lastAddr = stat.addr;
    }

    Addr canditates[] = DeltaCorrelation(entry);
    Addr prefetches[] = Prefetchfilter(entry, candidates[]);
    
    for(int i = 0; i < length(prefetches[]); i++){ // Do the actual prefetching for all addresses found by the DCPT and  prefetchj filter. 
        Addr pf_addr = prefetchers[i];
        issue_prefetch(pf_addr);
    }


}

void prefetch_complete(Addr addr) {
    /*
     * Called when a block requested by the prefetcher has been loaded.
     */
}
