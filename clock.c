////////////////////////////////////////////////////////////////////////////////
// Main File: runner.c
// This File: clock.c
//
// Authors:   Shaurya Kethireddy
// Email:     skethireddy@wisc.edu
// CS Login:  shaurya
//
////////////////////////////////////////////////////////////////////////////////
#include "pageAlgo.h"
#include <stdlib.h>
#include "stdio.h"

// page replacement algo for fifo
void pageReplace(unsigned long pid, unsigned long vpn, unsigned long ptr, void **tlb_root, struct Queue* physicalMemory, struct procInfo* newProc, int pageCount) {
    // init global memory clock data
    if(physicalMemory->data == NULL) {
        struct clock_data* global_clock = malloc(sizeof(struct clock_data));
        if(global_clock == NULL){
            fprintf(stderr, "Error: malloc failed\n");
        }
        global_clock->last_used = NULL;
        physicalMemory->data = global_clock;
    }

    // check if room in memory
    if(physicalMemory->pageCount >= pageCount) {
        struct QueuePage* lastUsed = ((struct clock_data*)physicalMemory->data)->last_used->next;
        //struct QueuePage* lastUsed = physicalMemory->front;
        if(lastUsed == NULL) {
            lastUsed = physicalMemory->front;
        }
        while (((struct clock_data*)lastUsed->data)->referenceBit != 0) {
            ((struct clock_data*)lastUsed->data)->referenceBit = 0;
            if(lastUsed->next == NULL) {
                lastUsed = physicalMemory->front;
            }
            else {
                lastUsed = lastUsed->next;
            }
        }

        // delete and free page from tlb
        tdelete(lastUsed->memory_ref, tlb_root, compare_pages);
        free((struct MemoryPage*)lastUsed->memory_ref);

        // add new page to tlb
        struct MemoryPage* new_tlb_page = init_page(tlb_root, pid, vpn, physicalMemory->back);

        // set link to tlb
        lastUsed->memory_ref = new_tlb_page;

        // reset ref bit
        ((struct clock_data*)lastUsed->data)->referenceBit = 0;

        // set link to physical memory
        new_tlb_page->ref = lastUsed;

        // set last used
        ((struct clock_data*)physicalMemory->data)->last_used = lastUsed;

        return;
    }

    // add new page
    struct clock_data* clock_init = malloc(sizeof(struct clock_data));
    if(clock_init == NULL){
        fprintf(stderr, "Error: malloc failed\n");
    }
    clock_init->referenceBit = 0;
    enqueue(physicalMemory, clock_init);

    // add new page to tlb
    struct MemoryPage* new_tlb_page = init_page(tlb_root, pid, vpn, physicalMemory->back);

    // set link to tlb
    physicalMemory->back->memory_ref = new_tlb_page;

    // set last used
    ((struct clock_data*)physicalMemory->data)->last_used = physicalMemory->back;
}

// used if page replace algo needs to preform an update when mem is accessed
void accessMemory(struct MemoryPage* page, struct procInfo* currentProc, struct Queue* physicalMemory) {
    struct QueuePage* physicalPage = page->ref;
    // reset ref bit
    ((struct clock_data*)physicalPage->data)->referenceBit = 1;

    // set last used physical page
    //((struct clock_data*)physicalMemory->data)->last_used = physicalPage;
}

void freeExtraMemory(struct QueuePage* page) {
    if(page->data != NULL)
        free(page->data);
}