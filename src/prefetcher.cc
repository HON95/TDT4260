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
typedef uint64_t Delta;

//Define:
#define MAX_NUM_OF_ENTRIES 100
#define MAX_DELTAS 6

//Declarations:
std::list <Addr> prefetches;
std::list <Addr> candidates;
deque<Addr> inFlight;

//This is the class that represents an enty in the DCPT table. All members are public.
class Entry{
    public:
    Entry(Addr pc); //constructor
    Addr pc;
    Addr lastAddr;
    Addr lastPrefetch;
    deque <Delta> deltas; 
};

Entry::Entry(Addr pc) {
    pc = 0;
    lastAddr = 0;
    lastPrefetch = 0;
}

//This calss is the table that holds all the table entries of the DCPT. All members are public
class Dcptable{
    public:
    Dcptable(); //constructor
    Entry *find_entry(Addr pc);
    list<Entry *> entries;
    int num_of_entries;
};

Dcptable::Dcptable(){
    num_of_entries = 0;
}

static Dcptable *table; //allocate static memory for the table 

void prefetch_init(void){
    /* Called before any calls to prefetch_access. */
    /* This is the place to initialize data structures. */
    table  = new Dcptable;
    DPRINTF(HWPrefetch, "Initialized sequential-on-access prefetcher\n");
}

//Function to find an entry in the table from a search with PC
Entry *Dcptable::find_entry(Addr pc){
    for (std::list<Entry *>::iterator it = entries.begin(); it != entries.end(); it++){
        Entry *temp = *it; //convert type
        if(pc == temp->pc){
            return temp;
        }
    }
return NULL; //If no entry is found with the current PC
}

void DeltaCorrelation(Entry *entry){ //From paper on DCPT
    candidates.clear();
    deque<Delta>::iterator del_it = entry->deltas.end(); //Create an delta iterator at the end of the delta deque
    del_it -= 1; //push it back to the last entry
    Delta last_delta = *del_it; 
    del_it -= 1; //push it back ti the second to last entry
    Delta sec_last_delta = *del_it;
    Addr address = entry->lastAddr; 
    del_it = entry->deltas.begin();
    
    while(del_it != entry->deltas.end()){
        Delta temp1 = *del_it;
        del_it++;
        Delta temp2 = *del_it;
        
        if((temp1 == sec_last_delta) && (temp2 == last_delta)){ //If a pattern is recognized
            
            for(del_it++; del_it != entry->deltas.end(); ++del_it){ //for each remaining delta
                address += *del_it * BLOCK_SIZE;
                candidates.push_back(address);
            }
        }
    }
}

void PrefetchFilter(Entry *entry){ //From paper on DCPT
    prefetches.clear();
    list<Addr>::iterator cand_it; //Make a candidate iterator

    for(cand_it = candidates.begin(); cand_it != candidates.end(); cand_it++){
        // See if candidate is in inFlight or MSHR or cache
        if(std::find(inFlight.begin(), inFlight.end(), *cand_it) == inFlight.end() && !in_mshr_queue(*cand_it) && !in_cache(*cand_it)){
            prefetches.push_back(*cand_it);
            
            if(inFlight.size() == 32){ //number of inFlight emelents
                inFlight.pop_front();
            }

            inFlight.push_back(*cand_it); 
            entry->lastPrefetch = *cand_it;
        }
    }
}

void prefetch_access(AccessStat stat){ 
    //Try to implement Algorithm 1, Main flow from "Storage Efficient Hardware
    //Prefetching using Delta-Correlating Prediction Tables" ("DCPT paper")
    Entry *curr_e = table->find_entry(stat.pc); //See if the PC corresponds to an existing entry

    if (curr_e == NULL){  //make new entry 
        if (table->num_of_entries >= MAX_NUM_OF_ENTRIES){
            table->entries.pop_back(); //remove oldeset entry
        }

        Entry *new_e = new Entry(stat.pc); //create new entry
        new_e->pc = stat.pc;
        new_e->lastAddr = stat.mem_addr;
        new_e->lastPrefetch = 0;
        new_e->deltas.push_front(1);//insert new element
        table->entries.push_front(new_e);
        curr_e = new_e;

        if(table->num_of_entries < MAX_NUM_OF_ENTRIES){
            ++table->num_of_entries; //update number of entries
        }
            cout << "Number of entries is: " << table->num_of_entries<<endl;
    }
    else if((stat.mem_addr - curr_e->lastAddr) != 0){ //If the addresses are not the same
       Delta curr_delta = (stat.mem_addr - curr_e->lastAddr);
       if(curr_e->deltas.size() == MAX_DELTAS){
            curr_e->deltas.pop_front();
       }
       curr_e->deltas.push_back(curr_delta); 
        //update the delta deque entry for the current entry to have the new delta value.
       curr_e->lastAddr = stat.mem_addr; //update last address
    }

    DeltaCorrelation(curr_e);
    PrefetchFilter(curr_e);
    
    for(list<Addr>::iterator pre_it = prefetches.begin(); pre_it !=prefetches.end(); pre_it++){ 
        // Do the actual prefetching for all addresses found by the DCPT and prefetch filter. 
        issue_prefetch(*pre_it);
    }
}

void prefetch_complete(Addr addr) {
    /*
     * Called when a block requested by the prefetcher has been loaded.
     */
}
