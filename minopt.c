////////////////////////////////////////////////////////////////////////////////
// Main File: runner.c
// This File: minopt.c
//
// Authors:   Shaurya Kethireddy
// Email:     skethireddy@wisc.edu
// CS Login:  shaurya
//
////////////////////////////////////////////////////////////////////////////////
#include "pageAlgo.h"
#include <stdlib.h>
#include "reader.h"
#include <stdio.h>

struct next_ptr* getNextRef(unsigned long pid, unsigned long vpn, unsigned long ptr, struct procInfo* newProc) {
    // get next ref for new page
    struct next_ptr* nptr = malloc(sizeof(struct next_ptr));
    nptr->ptr = -2;
    struct pid_vpn* currentLine = getNextLine_slow((long)ptr);

    // iterate until next ref is found or eof
    while(nptr->ptr == -2) {
        if(currentLine->pid == pid && currentLine->vpn == vpn) {
            nptr->ptr = (int)currentLine->ptr;
        }
        else if (currentLine->ptr == EOF) {
            nptr->ptr = -1;
        }
        else if (currentLine->pid != pid) {
            // if there is no new start -> must be done
            struct QueuePage* nextStart = newProc->start->front;
            while (nextStart != NULL) {
                if(((struct start_data*)nextStart->data)->ptr >= currentLine->ptr)
                    break;
                nextStart = nextStart->next;
            }

            if(nextStart == NULL) {
                nptr->ptr = -1;
            }
            else {
                free(currentLine);
                currentLine = NULL;
                currentLine = getLine_slow(((struct start_data*)nextStart->data)->ptr);
            }
        }
        else {
            struct pid_vpn* old_mem = currentLine;
            currentLine = getNextLine_slow(currentLine->ptr);
            free(old_mem);
        }
    }

    return nptr;
}

// page replacement algo for fifo
void pageReplace(unsigned long pid, unsigned long vpn, unsigned long ptr, void **tlb_root, struct Queue* physicalMemory, struct procInfo* newProc, int pageCount) {
    // check if room in memory
    if(physicalMemory->pageCount >= pageCount) {
        struct QueuePage* max = physicalMemory->front;
        struct QueuePage* current = physicalMemory->front;
        // get memory page with furthest next ref (-1 = no future ref)
        while(current->next != NULL) {
            if(((struct next_ptr*)current->data)->ptr == -1){
                max = current;
                break;
            }
            else if(((struct next_ptr*)current->data)->ptr > ((struct next_ptr*)max->data)->ptr) {
                max = current;
            }
            current = current->next;
        }

        // delete and free page from tlb
        tdelete(max->memory_ref, tlb_root, compare_pages);
        free((struct MemoryPage*)max->memory_ref);

        // delete page from physical memory
        delete_page(max, physicalMemory);
        if((struct next_ptr*)max->data == NULL)
            free((struct next_ptr*)max->data);
        free(max);
    }


    // add new page
    enqueue(physicalMemory, getNextRef(pid, vpn, ptr, newProc));

    // add new page to tlb
    struct MemoryPage* new_tlb_page = init_page(tlb_root, pid, vpn, physicalMemory->back);

    // set link to tlb
    physicalMemory->back->memory_ref = (struct MemoryPage*)new_tlb_page;

}

// used if page replace algo needs to preform an update when mem is accessed
void accessMemory(struct MemoryPage* page, struct procInfo* currentProc, struct Queue* physicalMemory) {
    struct next_ptr* nextRef = getNextRef(page->pid, page->vpn, currentProc->ptr, currentProc);
    if(page->ref->data != NULL) {
        free((struct next_ptr*)page->ref->data);
    }
    page->ref->data = nextRef;
}

void freeExtraMemory(struct QueuePage* page) { }