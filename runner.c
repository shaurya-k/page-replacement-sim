////////////////////////////////////////////////////////////////////////////////
// Main File: runner.c
// This File: runner.c
//
// Authors:   Shaurya Kethireddy
// Email:     skethireddy@wisc.edu
// CS Login:  shaurya
//
////////////////////////////////////////////////////////////////////////////////
#define DISK_TIME 2000000

#include <stdlib.h>
#include "reader.h"
#include "queue.h"
#include "memoryTree.h"
#include "pageAlgo.h"
#include "datatypes.h"
#include "stats.h"
#include <stdio.h>
#include <time.h>


struct QueuePage* getNextProc(struct Queue* runningProcs ){
    struct QueuePage* currentProc = runningProcs->front;
    struct QueuePage* oldest = NULL;
    int all_blocked = 1;
    while (currentProc != NULL) {
        // and if not blocked or complete
        if (((struct procInfo *) currentProc->data)->blocked == 0) {
            all_blocked = 0;
            // if older then set new oldest
            if (oldest == NULL || ((struct procInfo *) currentProc->data)->ptr < ((struct procInfo *) oldest->data)->ptr) {
                oldest = currentProc;
            }
        }
        currentProc = currentProc->next;
    }

    if(all_blocked){
        return NULL;
    }
    return oldest;
}


struct QueuePage* getProc(struct Queue* runningProcs, int pid) {
    struct QueuePage *currentProc = runningProcs->front;
    while (currentProc != NULL) {
        if(((struct procInfo*)currentProc->data)->pid == pid) {
            return currentProc;
        }
        currentProc = currentProc->next;
    }
    return NULL;
}

void removeProc(struct QueuePage* proc, struct Queue* physicalMemory, void** tlb_root) {
    struct QueuePage* currentPage = physicalMemory->front;
    unsigned long pidToCheck = ((struct procInfo*)(proc->data))->pid;

    // del all mem with certain pid
    // iterate through physical mem
    do {
        // if memory link is same pid
        if(((struct MemoryPage*)currentPage->memory_ref)->pid == pidToCheck) {
            // set temp to del and forward to next page
            struct QueuePage* page_to_delete = currentPage;
            currentPage = currentPage->next;

            // del and free tlb page
            freeExtraMemory(page_to_delete);
            tdelete(((struct MemoryPage*)page_to_delete->memory_ref), tlb_root, compare_pages);
            free((struct MemoryPage*)page_to_delete->memory_ref);

            // del and free physical mem page
            delete_page(page_to_delete, physicalMemory);
            free(page_to_delete);
        }
        else {
            // next page
            currentPage = currentPage->next;
        }

    } while(currentPage != NULL);

    // del proc info from running procs
    ((struct procInfo*)proc->data)->blocked = -1;
}


long moveToNextRunnable(struct pid_vpn* line, struct Queue* runningProcs) {
    struct QueuePage* proc = getProc(runningProcs, line->pid);

    struct pid_vpn* l = getLine(proc->data, line->ptr);
    if(l == NULL) {
        return EOF;
    }

    // get procs next stopping point
    struct QueuePage* currentStop = ((struct procInfo*)proc->data)->stop->front;
    while(currentStop != NULL) {
        if(((struct start_data*)currentStop->data)->ptr >= line->ptr)
            break;
        currentStop = currentStop->next;
    }

    // check if procs pointer is greater or equal to the block's (that line is in) stop point
    if (((struct start_data*)currentStop->data)->ptr == EOF ||
            ((struct procInfo*)proc->data)->ptr > ((struct start_data*)currentStop->data)->ptr ||
            (((struct procInfo*)proc->data)->ptr == ((struct start_data*)currentStop->data)->ptr && ((struct procInfo*)proc->data)->blocked == -1)) {

        // then block is already complete so move to next runnable
        struct pid_vpn* nextLine = getNextLine(proc->data, ((struct start_data*)currentStop->data)->ptr);
        if(nextLine->ptr == EOF) {
            return EOF;
        }
        return moveToNextRunnable(nextLine, runningProcs);
    }

    return ((struct procInfo*)proc->data)->ptr;
}


long incrementGlobalLine(struct procInfo* global_line, struct QueuePage* currentProc, struct Queue* runningProcs) {
    // get proc at next global line
    struct pid_vpn* nextLine = getNextLine(global_line, global_line->ptr);
    if(nextLine->ptr == EOF) {
        return EOF;
    }
    struct pid_vpn* line = getLine(getProc(runningProcs, nextLine->pid)->data, nextLine->ptr);

    // get next lines proc
    struct QueuePage* nextProc = getProc(runningProcs, line->pid);

    // if next and current procs have different pids
    if(((struct procInfo*)currentProc->data)->pid != ((struct procInfo*)nextProc->data)->pid) {
        // skip to next proc
        // get next runnable proc
        global_line->ptr = moveToNextRunnable(line, runningProcs);
        return global_line->ptr;
    }
    return line->ptr;
}


int main(int argc, char* argv[]){
    clock_t start = clock();

    // set vars
    long _clock = 0;
    long inner_clock = 0;
    int page_count = parseCmdLine(argc, argv);

    // reader
    struct Queue* runningProcs = parseFile();

    // physical memory queue
    struct Queue* physicalMemory = createQueue();

    // disk  queue
    struct Queue* diskQueue = createQueue();
    long diskQueue_timer = 0;

    // tlb db
    void *tlb_root = NULL;

    // current line
    struct procInfo* global_proc = malloc(sizeof(struct procInfo));
    if(global_proc == NULL){
            fprintf(stderr, "Error: malloc failed\n");
    }
    global_proc->ptr = 0;


    struct pid_vpn* current_line = NULL;
    struct QueuePage* currentProc = NULL;

    do {
        // if first ittr
        if(currentProc == NULL) {
            currentProc = runningProcs->front;
        }

        // if first ittr
        if(current_line == NULL) {
            current_line = getLine(getProc(runningProcs, ((struct procInfo*)currentProc->data)->pid)->data , global_proc->ptr);
        }

        // if global is different
        if (global_proc->ptr != current_line->ptr) {
            current_line = getLine(getProc(runningProcs, global_proc->pid)->data, global_proc->ptr);
        }
        // otherwise continue

        global_proc->pid = current_line->pid;

        short isGlobal = 1;

        // get proc from line
        currentProc = getProc(runningProcs, current_line->pid);


        // if proc is blocked
        if(((struct procInfo*)currentProc->data)->blocked == 1) {
            isGlobal = 0;
            // check disk queue
            if(diskQueue_timer <= _clock && peek(diskQueue) != NULL) {
                currentProc = dequeue(diskQueue)->data;
                // if disk queue is not empty, then reset disk timer
                if(peek(diskQueue) != NULL) {
                    diskQueue_timer = _clock + DISK_TIME;
                }
            }
            // otherwise get other proc
            else {
                currentProc = getNextProc(runningProcs);
            }
            // fast forward in time if all blocked
            if(currentProc == NULL) {
                _clock += (DISK_TIME - inner_clock);
                addRT(DISK_TIME - inner_clock);
                addPF((int)(physicalMemory->pageCount * (DISK_TIME - inner_clock)));

                inner_clock = 0;
                continue;
            }
            // get line for other proc
            else {
                current_line = getLine(currentProc->data ,((struct procInfo*)currentProc->data)->ptr);
            }
        }

        // check tlb
        // if found in TLB
        struct MemoryPage* tlb_page = find_page(&tlb_root, current_line->pid, current_line->vpn);
        if(tlb_page != NULL) {
            accessMemory(tlb_page, currentProc->data, physicalMemory);

            //stats
            addMemRef(1);

            // if last instruction
            if(((struct procInfo*)currentProc->data)->ptr == ((struct start_data*)((struct procInfo*)currentProc->data)->stop->back->data)->ptr) {
                removeProc(currentProc, physicalMemory, &tlb_root);

                if(isGlobal)
                    global_proc->ptr = incrementGlobalLine(global_proc, currentProc, runningProcs);
            }
            // not last instruction then increment line
            else {
                // get next line
                struct pid_vpn* nextLine = getNextLine(currentProc->data, ((struct procInfo*)currentProc->data)->ptr);

                // if EOF
                if(nextLine == NULL)
                    break;

                // if context switch
                if(nextLine->pid != current_line->pid) {
                    // take next start point of the queue
                    struct QueuePage* top = dequeue(((struct procInfo*)currentProc->data)->start);
                    if(top != NULL) {
                        struct start_data* topData = (struct start_data*)top->data;
                        ((struct procInfo*)currentProc->data)->ptr = topData->ptr;
                    }
                    //}

                    // change global line too
                    if(isGlobal) {
                        global_proc->ptr = incrementGlobalLine(global_proc, currentProc, runningProcs);
                    }

                }
                // if in same pid
                else {
                    // if global
                    if(isGlobal) {
                        global_proc->ptr = incrementGlobalLine(global_proc, currentProc, runningProcs);
                    }

                    ((struct procInfo*)currentProc->data)->ptr = getNextLine(currentProc->data, ((struct procInfo*)currentProc->data)->ptr)->ptr;
                }
            }
        }
        // if not found
        else {
            // if already grabbed from disk
            if(((struct procInfo*)currentProc->data)->blocked != 0) {
                pageReplace(current_line->pid, current_line->vpn, current_line->ptr, &tlb_root, physicalMemory, currentProc->data, page_count);

                ((struct procInfo*)currentProc->data)->blocked = 0;
            }
            // page fault
            else {
                //stats
                addPageFault(1);

                ((struct procInfo*)currentProc->data)->blocked = 1;
                if(peek(diskQueue) == NULL) {
                    diskQueue_timer = _clock + DISK_TIME;
                }
                enqueue(diskQueue, currentProc);
            }
        }
        _clock += 1;
        if(peek(diskQueue) != NULL) {
            inner_clock += 1;
        }

        // stats
        addRT(1);
        addPF(physicalMemory->pageCount);

        struct QueuePage* current = runningProcs->front;
        while(current != NULL) {
            if(((struct procInfo*)current->data)->blocked == 0) {
                addProc(1);
            }
            current = current->next;
        }

    } while (global_proc->ptr != EOF);

    printStatistics();
    closeFile();

    // free memory
    struct QueuePage* toFree = runningProcs->front;
    while (toFree != NULL) {
        // free proc page
        freeQueue(((struct procInfo*)toFree->data)->stop);
        freeQueue(((struct procInfo*)toFree->data)->start);
        free(((struct procInfo*)toFree->data)->cache);

        struct QueuePage* next = toFree->next;
        free(toFree);
        toFree = NULL;

        toFree = next;
    }
    free(runningProcs);

    //free global
    free(global_proc->cache);
    free(global_proc);

    // free disk
    free(diskQueue);
    free(tlb_root);

    clock_t stop = clock();
    double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
    printf("\nTime elapsed: %.5f\n", elapsed);
}
