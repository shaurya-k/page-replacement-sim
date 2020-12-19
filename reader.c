////////////////////////////////////////////////////////////////////////////////
// Main File: runner.c
// This File: reader.c
//
// Authors:   Shaurya Kethireddy
// Email:     skethireddy@wisc.edu
// CS Login:  shaurya
//
////////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "reader.h"
#include "queue.h"
#include "stats.h"
#include "stdbool.h"

#define PAGE_SIZE 4096
#define MEM_SIZE 1048576
#define CACHE_SIZE 5

static char* fileName = NULL;
static FILE *file = NULL;


bool isPowerOfTwo(int num) {
    int count = 0;
    while (num != 0) {
        int temp = num % 2;
        if (temp == 1){
            count++;
        }
        num = num / 2;
    }
    if(count == 1){
        return true;
    }
    else {
        return false;
    }

}


struct Queue* parseFile(){
    // open file
    if(file == NULL) {
        file = fopen(fileName, "r");
        if(file == NULL){
            fprintf(stderr, "Error: Failed to open file\n");
            exit(1);
        }
    }

    int placed[100] = {0};
	unsigned long currLineIndex = 0;
    int prevPid = 0;

    //first loop, get all pids and start and end of each pid
    struct Queue* queue = createQueue();
    // for every line
    fseek(file, 0, SEEK_SET);

	while(!feof(file)) {
		int currPid;
		int vpn;
        if(fscanf(file, "%d %d", &currPid, &vpn) != 2)
            continue;

        //check if pid has already been parsed through once
		if(placed[currPid] == 0) {
            	struct procInfo* info = malloc(sizeof(struct procInfo));
		if(info == NULL){
            		fprintf(stderr, "Error: malloc failed\n");
        	}
                info->start = createQueue();
                info->stop = createQueue();

                // stop lines
                struct start_data* newStopLine = malloc(sizeof(struct start_data));
		if(newStopLine == NULL){
            		fprintf(stderr, "Error: malloc failed\n");
        	}

                //enqueue(info->start, newStartLine);
                enqueue(info->stop, newStopLine);
            	info->ptr = currLineIndex;
           		info->blocked = 0; //false
           		info->pid = currPid;
           		info->cache = NULL;

			enqueue(queue, info);
			placed[currPid] = 1;
		}
		else{
		    // get proc from current line
            struct QueuePage* temp = queue->front;
			while(((struct procInfo*)temp->data)->pid != currPid){
                temp = temp->next;
            }
			// if the last pid was different
            if(prevPid - currPid != 0){
                // new start
                struct start_data* newStartLine = malloc(sizeof(struct start_data));
		if(newStartLine == NULL){
            		fprintf(stderr, "Error: malloc failed\n");
        	}
                newStartLine->ptr = currLineIndex;
                enqueue((struct Queue*)((struct procInfo*)temp->data)->start, newStartLine);

                // new stop line mem created
                struct start_data* newStopLine = malloc(sizeof(struct start_data));
		if(newStopLine == NULL){
            		fprintf(stderr, "Error: malloc failed\n");
        	}
                enqueue(((struct procInfo*)(temp->data))->stop, newStopLine);
            }

            // set pids stop point stop point
            ((struct start_data*)((struct procInfo*)temp->data)->stop->back->data)->ptr = currLineIndex;

		}
       	currLineIndex = ftell(file);
        prevPid = currPid;
	}
	return queue;
}


void setCache(struct procInfo* proc, long index){
    if(proc->cache == NULL){
        proc->cache = malloc(sizeof(struct pid_vpn*) * CACHE_SIZE);
        if(proc->cache == NULL){
            fprintf(stderr, "Error: malloc failed\n");
        }
    }

    // seek for next cache
    fseek(file, index, SEEK_SET);
    int pid;
    int vpn;
    long last_ptr = index;
    for (int i = 0; i < CACHE_SIZE; i++) {
        proc->cache[i] = malloc(sizeof(struct pid_vpn));
        if(fscanf(file, "%d %d", &pid, &vpn) != 2) {
            // reached EOF
            proc->cache[i]->ptr = EOF;
            proc->cache[i]->pid = -1;
            proc->cache[i]->vpn = -1;
            break;
        }
        else {
            // point is good -> set cache
            proc->cache[i]->ptr = last_ptr;
            proc->cache[i]->pid = pid;
            proc->cache[i]->vpn = vpn;

            // set the next ptr
            last_ptr = ftell(file);

        }
    }
}


struct pid_vpn* searchCache(struct procInfo* proc, long index) {
    // does cache exist
    if(proc->cache == NULL) {
        setCache(proc, proc->ptr);
        return proc->cache[0];
    }
    // check if line is in cache
    short found = 0;
    int i;
    for (i = 0; i < CACHE_SIZE; i++) {
        if(proc->cache[i]->ptr == EOF && index >= proc->cache[0]->ptr) {
            found = 1;
            break;
        }
        if(proc->cache[i]->ptr == index) {
            found = 1;
            break;
        }
    }
    if(found) {
        return proc->cache[i];
    }
    else {
        setCache(proc, index);
        return proc->cache[0];
    }
}


struct pid_vpn* getLine(struct procInfo* proc, long index){
    // open file
    if(file == NULL) {
        file = fopen(fileName, "r");
        if(file == NULL){
            fprintf(stderr, "Error: Failed to open file\n");
            exit(1);
        }
    }

    // if index is EOF
    if(index == EOF) {
        struct pid_vpn* eof_line = malloc(sizeof(struct pid_vpn));
	    if(eof_line == NULL){
            fprintf(stderr, "Error: malloc failed\n");
        }
        eof_line->ptr = EOF;
        return eof_line;
    }


    struct pid_vpn* p = searchCache(proc, index);

    if(proc->pid != p->pid)
        printf("");

    return p;
}


struct pid_vpn* getNextLine(struct procInfo* proc, long index) {
    // open file
    if(file == NULL) {
        file = fopen(fileName, "r");
        if(file == NULL){
            fprintf(stderr, "Error: Failed to open file\n");
            exit(1);
        }
    }

    // get current line
    struct pid_vpn* currentLine = getLine(proc, index);
    if(proc->pid != currentLine->pid)
        printf("");

    // get the next line
    // if current line is EOF
    if(currentLine->ptr == EOF) {
        return currentLine;
    }
    // if current line is last in cache
    else if(currentLine->ptr == proc->cache[CACHE_SIZE - 1]->ptr) {
        // refill cache
        setCache(proc, index);
        return proc->cache[1];
    }
    else {
        // find the cache entry after the current line
        for (int i = 0; i < CACHE_SIZE; i++) {
            if(proc->cache[i]->ptr == currentLine->ptr)
                return proc->cache[i+1];
        }
    }
}


struct pid_vpn* getNextLine_slow(long index) {
    // open file
    if (file == NULL) {
        file = fopen(fileName, "r");
        if (file == NULL) {
            fprintf(stderr, "Error: Failed to open file\n");
            exit(1);
        }
    }

    struct pid_vpn *p = malloc(sizeof(struct pid_vpn));

    // seek for next line
    fseek(file, index, SEEK_SET);
    int pid;
    int vpn;
    if (fscanf(file, "%d %d", &pid, &vpn) != 2) {
        // reached EOF
        p->ptr = -1;
        p->vpn = -1;
        p->pid = -1;
        return p;
    }
    long ptr = ftell(file);
    if (fscanf(file, "%d %d", &pid, &vpn) != 2) {
        // reached EOF
        p->ptr = -1;
        p->vpn = -1;
        p->pid = -1;
        return p;
    }
    p->ptr = ptr;
    p->vpn = vpn;
    p->pid = pid;
    return p;
}


struct pid_vpn* getLine_slow(long index) {
    // open file
    if (file == NULL) {
        file = fopen(fileName, "r");
        if (file == NULL) {
            fprintf(stderr, "Error: Failed to open file\n");
            exit(1);
        }
    }

    struct pid_vpn *p = malloc(sizeof(struct pid_vpn));

    // seek for next line
    fseek(file, index, SEEK_SET);
    int pid;
    int vpn;
    if (fscanf(file, "%d %d", &pid, &vpn) != 2) {
        // reached EOF
        p->ptr = -1;
        p->vpn = -1;
        p->pid = -1;
        return p;
    }

    p->ptr = index;
    p->vpn = vpn;
    p->pid = pid;
    return p;
}


void closeFile() {
    fclose(file);
    file = NULL;
}


 int parseCmdLine(int argc, char* argv[]){
     int pageSize = PAGE_SIZE;
     int mem = MEM_SIZE;
     if(argc <= 1){
         fprintf(stderr, "Error: not enough args\n");
 		exit(1);
     }else if(argc == 2){
         fileName = argv[1];
         FILE *file = fopen(fileName, "r");
         if(file == NULL){
 		    fprintf(stderr, "Error: Failed to open file\n");
 		    exit(1);
 	    }
         fclose(file);
     }else if(argc == 4){
         if(strcmp(argv[1], "-p") == 0){
             int num = atoi(argv[2]);
 			if(num == 0){
 				fprintf(stderr, "Error: unreadable arg after -p\n");
 				exit(1);
 			}
             bool check = isPowerOfTwo(num);
             if(!check){
                 fprintf(stderr, "Error: pageSize isn't power of 2\n");
 		        exit(1);
             }
 			pageSize = num;
         }else if(strcmp(argv[1], "-m") == 0){
             int num = atoi(argv[2]);
 			if(num == 0){
 				fprintf(stderr, "Error: unreadable arg after -m\n");
 				exit(1);
 			}
 			mem = num * mem;
         }else{
             fprintf(stderr, "Error: incorrect args passed\n");
 		    exit(1);
         }
         fileName = argv[3];
         FILE *file = fopen(fileName, "r");
         if(file == NULL){
 		    fprintf(stderr, "Error: Failed to open file\n");
 		    exit(1);
 	    }
         fclose(file);
     }else if(argc == 6){
         for(int i = 1; i < argc; i++) {
            	if(strcmp(argv[i], "-p") == 0 && argv[i+1] != NULL){
 				if(strcmp(argv[i+1], "-m") == 0){
 					continue;
 				}
 				i+=1;
                 int num = atoi(argv[i]);
 				if(num == 0){
 					fprintf(stderr, "Error: unreadable arg after -p\n");
 					exit(1);
 				}
                 bool check = isPowerOfTwo(num);
                 if(!check){
                     fprintf(stderr, "Error: pageSize isn't power of 2\n");
 		        	exit(1);
                 }
 				pageSize = num;
 			}else if(strcmp(argv[i], "-m") == 0 && argv[i+1] != NULL){
 				if(i+2 >= argc){
 					continue;
 				}
 				i+=1;
                	int num = atoi(argv[i]);
 				if(num == 0){
 					fprintf(stderr, "Error: unreadable arg after -m\n");
 					exit(1);
 				}
 				mem = num * mem;
 			}else {
 				fileName = argv[i];
 			}
 		}
     }else{
         fprintf(stderr, "Error: wrong number of args\n");
 		exit(1);
     }
     int amount = mem/pageSize;
     setPagesAmount(amount);
 	return amount;
 }
