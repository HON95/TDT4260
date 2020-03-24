#pragma once

// Table size in number of entries
#define TABLE_SIZE 256
// Number of delta values
#define DELTA_COUNT 16
// Number of bits in each delta value
#define DELTA_WIDTH 8
// Number of uncompleted prefetches to monitor
#define MAX_UNCOMPLETED_PREFETCH 32

typedef Addr BlockAddr;
