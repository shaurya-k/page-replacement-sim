////////////////////////////////////////////////////////////////////////////////
// Main File: runner.c
// This File: stats.h
//
// Authors:   Shaurya Kethireddy
// Email:     skethireddy@wisc.edu
// CS Login:  shaurya
//
////////////////////////////////////////////////////////////////////////////////
#ifndef STATS
#define STATS

#include <time.h>
#include <stdio.h>

void addPageFault(int toAdd);
void addMemRef(int toAdd);
void addRT(int toAdd);
void addPF(int toAdd);
void addProc(int toAdd);
int getTMR();
int getTPI();
double getAMU();
double getARP();
unsigned long getRT();
void printStatistics();
void setPagesAmount(int _amount);

#endif
