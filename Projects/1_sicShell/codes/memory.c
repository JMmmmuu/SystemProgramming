/*************************************************
 ************** System Programming ***************
 ******************** Yuseok *********************
 ******************** 190000 *********************
 *************************************************/
#include "header.h"

int dump(char* start, char* end, int type) {
    // du[mp] [start, end]
    int s = strToHex(start);
    int e = strToHex(end);
    if (!validAddrRange(s, e)) {
        printf("segmentation fault\n");
        return 0;
    }

    
    return 1;
}

int edit(char* address, char* value) {
    // e[dit] address, value
    unsigned int addr = strToHex(address);
    unsigned int val = strToHex(value);

    // incorrect input or addr and val are not hex
    if (addr == -1 || val == -1) return 0;

    if (val > 0xFF) {
        return 0;
    }
    // then edit memory

    printf("in edit: %X %X\n", addr, val);
    unsigned char realValue = val & ONE_BYTE;
    //snprintf(&realValue, 1, "%X", val);
    //memcpy(&realValue, &val+3,1); 

    *(MEMORY + addr) = realValue;


    return 1;
}

int fill(char* start, char* end, char* value) {
    // f[ill] start, end, value
    int s, e, v;
    s = strToHex(start);
    e = strToHex(end);
    v = strToHex(value);
    if (s == -1 || e == -1 || v == -1) return 0;

    
    if (!validAddrRange(s, e)) {
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

int validAddr(int addr) {
    // if given address is vaild, return 1, else return 0
    if (addr >= 0 && addr <= 0xFFFFF)
        return 1;
    else
        return 0;
}

int validAddrRange(int start, int end) {
    if (validAddr(start) && validAddr(end) && end >= start)
        return 1;
    else
        return 0;
}

int strToHex(char* param) {
    // if parameter is not a hex, or incorrect parser return -1
    for (int i = strlen(param) - 1; i >= 0; i--) {
        if ( !isHex(param[i]) ) {
            printf("not a Hexadecimal!\n");
            return -1;
        }
    }
    
    // else, return proper hexa decimal number
    int hex;
    int res = sscanf(param, "%x", &hex);
    if (res == 0) return -1;

    printf("strToHex: %X\n", hex);

    return hex;
}

int isHex(char ch) {
    if ( (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f') || (ch >= '0' && ch <= '9') )
        return 1;
    else
        return 0;
}
