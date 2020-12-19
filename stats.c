////////////////////////////////////////////////////////////////////////////////
// Main File: runner.c
// This File: stats.c
//
// Authors:   Shaurya Kethireddy
// Email:     skethireddy@wisc.edu
// CS Login:  shaurya
//
////////////////////////////////////////////////////////////////////////////////

#include "stats.h"

static unsigned long runTime;
static int memRef = 0;
static int pageFaults = 0;
static unsigned long pageFrames = 0;
static int processes = 0;
static int amount = 0;

void addPageFault(int toAdd) {
	pageFaults += toAdd;
}

void addMemRef(int toAdd) {
	memRef += toAdd;
}

void addRT(int toAdd) {
	runTime += toAdd;
}

void addPF(int toAdd) {
	pageFrames += toAdd;
}

void addProc(int toAdd) {
	processes += toAdd;
}

int getTMR() {
	return memRef;
}

int getTPI() {
	return pageFaults;
}

void setPagesAmount(int _amount) {
    amount = _amount;
}

double getAMU() {
    double ans = (((double)pageFrames / (double)getRT()))/(double)amount;
	return ans;
}

double getARP() {
    double ans = (double)processes/runTime;
	return ans;
}

unsigned long getRT() {
	return runTime;
}

void printStatistics(){
	printf("AMU: %f\n", getAMU());
	printf("ARP: %f\n", getARP());
	printf("TMR: %d\n", getTMR());
	printf("TPI: %d\n", getTPI());
	printf("RTime: %ld\n", getRT());
}
