////////////////////////////////////////////////////////////////////////////////
// Main File: runner.c
// This File: queue.h
//
// Authors:   Shaurya Kethireddy
// Email:     skethireddy@wisc.edu
// CS Login:  shaurya
//
////////////////////////////////////////////////////////////////////////////////
#ifndef PROJ4_CS537_QUEUE_H
#define PROJ4_CS537_QUEUE_H


struct QueuePage {
    struct QueuePage* next;
    struct QueuePage* previous;
    void* data;
    void* memory_ref;
};

struct Queue {
    struct QueuePage *front;
    struct QueuePage *back;
    int pageCount;
    void* data;
};

struct Queue* createQueue();

void enqueue(struct Queue* queue, void* data);
struct QueuePage* dequeue(struct Queue* queue);
void delete_page(struct QueuePage* page, struct Queue* queue);
struct QueuePage* peek(struct Queue* queue);
void freeQueue(struct Queue* queue);


#endif //PROJ4_CS537_QUEUE_H
