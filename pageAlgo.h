////////////////////////////////////////////////////////////////////////////////
// Main File: runner.c
// This File: pageAlgo.h
//
// Authors:   Shaurya Kethireddy
// Email:     skethireddy@wisc.edu
// CS Login:  shaurya
//
////////////////////////////////////////////////////////////////////////////////
#ifndef pageAlgo
#define pageAlgo

#include <search.h>
#include "queue.h"
#include "memoryTree.h"
#include "datatypes.h"

struct clock_data {
    unsigned short referenceBit;
    struct QueuePage* last_used;
};

struct next_ptr{
    int ptr;
};


// page replacement algo
void pageReplace(unsigned long pid, unsigned long vpn, unsigned long ptr, void **tlb_root, struct Queue* physicalMemory, struct procInfo* newProc, int pageCount);

// used if page replace algo needs to preform an update when mem is accessed
void accessMemory(struct MemoryPage* page, struct procInfo* currentProc, struct Queue* physicalMemory);

void freeExtraMemory(struct QueuePage* page);
#endif //pageAlgo
