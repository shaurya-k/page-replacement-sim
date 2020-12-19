////////////////////////////////////////////////////////////////////////////////
// Main File: runner.c
// This File: reader.h
//
// Authors:   Shaurya Kethireddy
// Email:     skethireddy@wisc.edu
// CS Login:  shaurya
//
////////////////////////////////////////////////////////////////////////////////
#ifndef READER
#define READER
#include "queue.h"
#include <stdbool.h>
#include "datatypes.h"
#include "math.h"

bool isPowerOfTwo(int num);

int parseCmdLine(int argc, char* argv[]);

struct Queue* parseFile();

struct pid_vpn* getLine(struct procInfo* proc, long index);

struct pid_vpn* getNextLine(struct procInfo* proc, long index);

void closeFile();

struct pid_vpn* getNextLine_slow(long index);

struct pid_vpn* getLine_slow(long index);


#endif
