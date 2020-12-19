////////////////////////////////////////////////////////////////////////////////
// Main File: runner.c
// This File: memoryTree.c
//
// Authors:   Shaurya Kethireddy
// Email:     skethireddy@wisc.edu
// CS Login:  shaurya
//
////////////////////////////////////////////////////////////////////////////////
#define _GNU_SOURCE
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "memoryTree.h"


int compare_pages(const void *_left, const void *_right) {
    struct MemoryPage *left = (struct MemoryPage*)_left;
    struct MemoryPage *right = (struct MemoryPage*)_right;

    if (left->pid > right->pid) {
        return 1;
    }
    else if (left->pid < right->pid){
        return -1;
    }
    else {
        if (left->vpn > right->vpn) {
            return 1;
        }
        else if (left->vpn < right->vpn){
            return -1;
        }
    }
    return 0;
}

struct MemoryPage* init_page(void **root_page, unsigned int pid, unsigned int vpn, struct QueuePage* ref) {
    struct MemoryPage *page = malloc(sizeof(*page));
    if (page == NULL) {
        fprintf(stderr, "Page memory allocation error.\n");
        exit(1);
    }
    page->pid = pid;
    page->vpn = vpn;
    page->ref = ref;


    tsearch(page, root_page, compare_pages);

    return page;
}

// Find a node in the binary tree
// Returns NULL if no node is found
struct MemoryPage *find_page(void **root_page, unsigned int pid, unsigned int vpn) {
    void *result;
    struct MemoryPage *page;
    struct MemoryPage search_page;

    search_page.pid = pid;
    search_page.vpn = vpn;

    // page not found
    if ((result = tfind(&search_page, root_page, compare_pages)) == NULL) {
        page = NULL;
    }
    // page found
    else {
        page = *(struct MemoryPage**)result;
    }

    return page;
}