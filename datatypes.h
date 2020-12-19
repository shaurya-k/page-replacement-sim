////////////////////////////////////////////////////////////////////////////////
// Main File: runner.c
// This File: datatypes.h
//
// Authors:   Shaurya Kethireddy
// Email:     skethireddy@wisc.edu
// CS Login:  shaurya
//
////////////////////////////////////////////////////////////////////////////////

#ifndef PROJ4_CS537_DATATYPES_H
#define PROJ4_CS537_DATATYPES_H

struct pid_vpn {
    int pid;
    int vpn;
    long ptr;
};

struct start_data{
    long ptr;
};

struct procInfo {
    struct Queue* start;
    struct Queue* stop;
    long ptr;
    int blocked;
    int pid;
    struct pid_vpn** cache;
};

#endif //PROJ4_CS537_DATATYPES_H
