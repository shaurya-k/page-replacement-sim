////////////////////////////////////////////////////////////////////////////////
// Main File: runner.c
// This File: lru.c
//
// Authors:   Shaurya Kethireddy
// Email:     skethireddy@wisc.edu
// CS Login:  shaurya
//
////////////////////////////////////////////////////////////////////////////////
#include "pageAlgo.h"
#include <stdlib.h>

// page replacement algo for fifo
void pageReplace(unsigned long pid, unsigned long vpn, unsigned long ptr, void **tlb_root, struct Queue* physicalMemory, struct procInfo* newProc, int pageCount) {
    // check if room in memory
    if(physicalMemory->pageCount >= pageCount) {
        // remove page
        struct QueuePage* oldPage = dequeue(physicalMemory);

        // delete page from tlb
        tdelete(oldPage->memory_ref, tlb_root, compare_pages);

        // free mem
        free((struct MemoryPage*)oldPage->memory_ref);
        free(oldPage);
    }

    // add new page
    enqueue(physicalMemory, NULL);

    // add new page to tlb
    struct MemoryPage* new_tlb_page = init_page(tlb_root, pid, vpn, physicalMemory->back);

    // set link to tlb
    physicalMemory->back->memory_ref = new_tlb_page;
}

// used if page replace algo needs to preform an update when mem is accessed
void accessMemory(struct MemoryPage* page, struct procInfo* currentProc, struct Queue* physicalMemory) {
    struct QueuePage* physicalPage = page->ref;

    // delete physical memory page
    delete_page(physicalPage, physicalMemory);

    //add physical page back
    enqueue(physicalMemory, NULL);
    physicalMemory->back->memory_ref = physicalPage->memory_ref;

    free(physicalPage);

    //reset ref
    page->ref = physicalMemory->back;
}

void freeExtraMemory(struct QueuePage* page) { }