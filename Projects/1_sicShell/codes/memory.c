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

int edit(char* addr, char* val) {
    // e[dit] address, value
    strToHex(addr);
    strToHex(val);


    int address = 0, value = 0;


    return 1;
}

int fill(char* start, char* end, char* val) {
    // f[ill] start, end, value
    if (!validAddrRange(start, end)) {
        printf("segmentation fault\n");
        return 0;
    }
    char* tmp = start;
    while (tmp <= end) {
        //*tmp = val;
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

int strToHex(char* param) {
    // if parameter is not a hex, return -1
    
    for (int i = strlen(param) - 1; i >= 0; i--) {
        if ( !isHex(param[i]) ) {
            printf("not a Hexadecimal!\n");
            return -1;
        }
    }
    
    // else, return proper hexa decimal number
    int hex;
    sscanf(param, "%x", &hex);
    printf("%X\n", hex);

    return hex;
}

int isHex(char ch) {
    if ( (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f') || (ch >= '0' && ch <= '9') )
        return 1;
    else
        return 0;
}
