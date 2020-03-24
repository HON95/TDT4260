// Prefetcher using the Delta Correlattion Prediction Table (DCPT) algorithm.

#include <algorithm>
#include <iostream>
#include <list>
#include <stack>
#include <string>
#include <sstream>
#include <math.h>

#include "interface.hh"
#include "prefetcher.hh"
#include "table.hh"

// Prototypes
static void run_dcpt(AccessStat stat);
static std::list<Addr> *run_delta_correlation(Entry *entry);
static std::list<Addr> *run_prefetch_filter(Entry *entry, std::list<Addr> *candidates);

// Globals
static uint64_t block_address_mask;
static int64_t delta_min_value;
static int64_t delta_max_value;
static Table *table;
static std::list<Addr> *uncompleted_prefetches;

// Called when initializing the prefetcher
void prefetch_init(void) {
    block_address_mask = ~((uint32_t) 0) << ((uint32_t) log2(BLOCK_SIZE));
    delta_min_value = -(1 << (DELTA_WIDTH - 1));
    delta_max_value = (1 << (DELTA_WIDTH - 1)) - 1;
    table  = new Table();
    uncompleted_prefetches = new std::list<Addr>();
    DPRINTF(HWPrefetch, "Initialized DCPT prefetcher.\n");
}

// Called on cache access
void prefetch_access(AccessStat stat) {
    run_dcpt(stat);
}

// Called after a prefetch is completed
void prefetch_complete(Addr addr) {
    uncompleted_prefetches->remove(addr);
}

static void run_dcpt(AccessStat stat) {
    // Nullable
    Entry *entry = table->find_entry(stat.pc);

    // Not found, create new
    if (entry == NULL){
        // Make space by removing oldest entry
        if (table->entries.size() >= TABLE_SIZE) {
            delete table->entries.back();
            table->entries.pop_back();
        }

        // Add new entry
        Entry *new_entry = new Entry();
        new_entry->pc = stat.pc;
        new_entry->last_address = stat.mem_addr;
        table->entries.push_front(new_entry);
    }

    // Found with different access address (non-zero delta)
    else if (stat.mem_addr != entry->last_address) {
        // Make space by removing oldest delta
        if (entry->deltas.size() == DELTA_COUNT){
            entry->deltas.pop_front();
        }

        // Add new delta
        Delta delta = stat.mem_addr - entry->last_address;
        // Check if underflow or overflow
        if (delta < delta_min_value || delta > delta_max_value) {
            delta = 0;
        }
        entry->deltas.push_back(delta);
        entry->last_address = stat.mem_addr;

        // Run delta correlation, prefetch filter and issue prefetches
        std::list<Addr> *candidates = run_delta_correlation(entry);
        std::list<Addr> *prefetches = run_prefetch_filter(entry, candidates);
        for(std::list<Addr>::iterator iter = prefetches->begin(); iter != prefetches->end(); iter++){
            // Check if prefetch queue is full
            if (current_queue_size() == MAX_QUEUE_SIZE) {
                break;
            }
            issue_prefetch(*iter);
        }
        delete candidates;
        delete prefetches;
    }
}

// Based on the Delta Correlation algorithm in the paper
static std::list<Addr> *run_delta_correlation(Entry *entry) {
    std::list<Addr> *candidates = new std::list<Addr>();
    size_t const delta_count = entry->deltas.size();

    // At least three deltas are required to find a pattern
    if (delta_count >= 3) {
        Delta last_delta = entry->deltas[delta_count - 1];
        Delta sec_last_delta = entry->deltas[delta_count - 2];
        Addr address = entry->last_address;

        // Try to find a match for the two last deltas
        // Start from the back, excluding the last delta
        // Signed iteration variable to prevent underflow
        for (int32_t i = delta_count - 3; i >= 0; i--) {
            Delta first_delta = entry->deltas[i];
            Delta second_delta = entry->deltas[i + 1];

            // TODO Ignore overflows? Test.

            // Is this a pattern?
            if(first_delta == sec_last_delta && second_delta == last_delta){
                // For each delta after the match, add a prefetch candidate
                for (size_t j = i + 2; j < delta_count; j++){
                    Delta delta = entry->deltas[j];
                    // Ignore overflows
                    if (delta == 0) {
                        continue;
                    }
                    address += entry->deltas[j];
                    // Check that it's (still) within the memory size
                    if (address > MAX_PHYS_MEM_ADDR) {
                        break;
                    }
                    candidates->push_back(address);
                }
                // TODO Continue or break? Test? Depends on which end is traversed from too.
                break;
            }
        }
    }

    return candidates;
}

// Based on the Prefetch Filter algorithm in the paper
static std::list<Addr> *run_prefetch_filter(Entry *entry, std::list<Addr> *candidates) {
    std::list<Addr> *prefetches = new std::list<Addr>();

    for(std::list<Addr>::iterator cand_iter = candidates->begin(); cand_iter != candidates->end(); cand_iter++){
        Addr address = *cand_iter;
        BlockAddr block_address = address & block_address_mask;

        // Discard all up to this point if same as last prefetch
        // FIXME Is this correct?
        if (address == entry->last_prefetch) {
            prefetches->clear();
        }

        // Add prefetch if not in flight, MSHR queue or cache
        bool is_uncompleted = std::find(uncompleted_prefetches->begin(), uncompleted_prefetches->end(), block_address) != uncompleted_prefetches->end();
        if (!is_uncompleted && !in_mshr_queue(block_address) && !in_cache(block_address)) {
            // Make space by removing oldest prefetch
            if (uncompleted_prefetches->size() == MAX_UNCOMPLETED_PREFETCH) {
                uncompleted_prefetches->pop_front();
            }

            prefetches->push_back(block_address);
            uncompleted_prefetches->push_back(block_address);
            entry->last_prefetch = address;
        }
    }

    return prefetches;
}
