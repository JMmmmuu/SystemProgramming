/*************************************************
 ************** System Programming ***************
 ******************** Yuseok *********************
 ******************** 190000 *********************
 *************************************************/
#include "header.h"

int dump(char* start, char* end, int type) {
    // du[mp] [start, end]
    if (!validAddrRange(start, end)) {
        printf("segmentation fault\n");
        return 0;
    }

    
    return 1;
}

int edit(char* address, char value) {
    // e[dit] address, value
    if (!validAddr(address)) {
        printf("segmentation fault\n");
        return 0;
    }

    *address = value;
    return 1;
}

int fill(char* start, char* end, char value) {
    // f[ill] start, end, value
    if (!validAddrRange(start, end)) {
        printf("segmentation fault\n");
        return 0;
    }
    char* tmp = start;
    while (tmp <= end) {
        *tmp = value;
    }
    return 1;
}

void reset() {
    // reset
    for (int i = 0; i < MEMORY_SIZE; i++) {
        MEMORY[i] = 0x00;
    }
}

int validAddr(char* addr) {
    // if given address is vaild, return 1, else return 0
    if (addr >= MEMORY && addr - MEMORY < MEMORY_SIZE)
        return 1;
    else
        return 0;
}

int validAddrRange(char* start, char* end) {
    if (validAddr(start) && validAddr(end) && end >= start)
        return 1;
    else
        return 0;
}
