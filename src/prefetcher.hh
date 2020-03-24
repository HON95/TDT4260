#pragma once

// Number of uncompleted prefetches to monitor (don't modify)
#define MAX_UNCOMPLETED_PREFETCH 32
// Table size in number of entries (tunable)
#define TABLE_SIZE 128
// Number of delta values (tunable)
#define DELTA_COUNT 12
// Number of bits in each delta value (tunable)
#define DELTA_WIDTH 16

typedef Addr BlockAddr;
