////////////////////////////////////////////////////////////////////////////////
// Main File: runner.c
// This File: memoryTree.h
//
// Authors:   Shaurya Kethireddy
// Email:     skethireddy@wisc.edu
// CS Login:  shaurya
//
////////////////////////////////////////////////////////////////////////////////
#ifndef PROJ4_CS537_MEMORYTREE_H
#define PROJ4_CS537_MEMORYTREE_H

#include <search.h>

// memory pages
struct MemoryPage {
    unsigned long pid;
    unsigned long vpn;
    struct QueuePage* ref;
    void* data;
};

int compare_pages(const void *_left, const void *_right);

struct MemoryPage*  init_page(void **root_page, unsigned int pid, unsigned int vpn, struct QueuePage* ref);

struct MemoryPage *find_page(void **root_page, unsigned int pid, unsigned int vpn);


#endif //PROJ4_CS537_MEMORYTREE_H
