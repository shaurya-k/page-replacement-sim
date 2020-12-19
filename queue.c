////////////////////////////////////////////////////////////////////////////////
// Main File: runner.c
// This File: queue.c
//
// Authors:   Shaurya Kethireddy
// Email:     skethireddy@wisc.edu
// CS Login:  shaurya
//
////////////////////////////////////////////////////////////////////////////////
#include "queue.h"
#include <stddef.h>
#include <stdlib.h>
#include "stdio.h"


// function creates a new queue in memory that holds the back and front
struct Queue* createQueue() {
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    if(queue == NULL){
            fprintf(stderr, "Error: malloc failed\n");
    }
    queue->front = NULL;
    queue->back = NULL;
    queue->pageCount = 0;
    queue->data = NULL;
    return queue;
}

// add page to back of queue
void enqueue(struct Queue* queue, void* data) {
    queue->pageCount += 1;

    // Create new page
    struct QueuePage* page = (struct QueuePage*)malloc(sizeof(struct QueuePage));
    if(page == NULL){
            fprintf(stderr, "Error: malloc failed\n");
    }
    page->data = data;
    page->next = NULL;
    page->previous = NULL;

    // If queue is empty
    if (queue->back == NULL) {
        queue->front = page;
        queue->back = page;
        return;
    }
    else {
        // add page to back of queue
        page->previous = queue->back;
        queue->back->next = page;
        queue->back = page;
    }
}

// pop page off queue
struct QueuePage* dequeue(struct Queue* queue) {
    // if queue is empty
    if (queue->front == NULL)
        return NULL;

    queue->pageCount -= 1;

    // store the dequeued page
    struct QueuePage* popped = queue->front;
    queue->front = queue->front->next;

    // If it was the last element
    if (queue->front == NULL) {
        queue->back = NULL;
    }
    else {
        queue->front->previous = NULL;
    }

    return popped;
}

void delete_page(struct QueuePage* page, struct Queue* queue) {
    if(page->previous != NULL) {
        page->previous->next = page->next;
    }
    else {
        queue->front = page->next;
    }
    if(page->next != NULL) {
        page->next->previous = page->previous;
    }
    else {
        queue->back = page->previous;
    }


    queue->pageCount -= 1;
}


struct QueuePage* peek(struct Queue* queue) {
    return queue->front;
}

void freeQueue(struct Queue* queue){
    struct QueuePage* toFree = queue->front;
    while (toFree != NULL) {
        // free proc page
        if(toFree->data != NULL) {
            free(toFree->data);
        }
        struct QueuePage* next = toFree->next;
        free(toFree);
        toFree = NULL;

        toFree = next;
    }
    if(queue->data != NULL) {
        free(queue->data);
    }
    free(queue);
}