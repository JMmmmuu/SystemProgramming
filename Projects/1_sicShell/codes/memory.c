/*************************************************
 ************** System Programming ***************
 ******************** Yuseok *********************
 ******************** 190000 *********************
 *************************************************/
#include "header.h"

int dump(char* start, char* end, int type) {
    // du[mp] [start, end]
    /*switch (type) {
        case 0:         // du[mp]
            
        case 1:         // du[mp] start
        case 2:         // du[mp] start, end
    }*/
    int s, e;
    if (type == 0) {
        s = 0;
        e = 0x9F;
    }
    else if (type == 1) {
        s = strToHex(start);
        e = s + 0x9F;
    }
    else {
        s = strToHex(start);
        e = strToHex(end);
    }

    // incorrect input or addr and val are not hex
    if (s == -1 || e == -1) {
        printf("incorrect input. Please text again\n");
        return 0;
    }

    if (!validAddrRange(s, e)) {
        printf("segmentation fault\n");
        return 0;
    }

    int i, j;
    int startLineAddr = s / 16 * 16;
    int endLineAddr = e / 16 * 16;
    int line = (endLineAddr - startLineAddr) / 16 + 1;
    int addr;
    char ASCIIcode[17];
    ASCIIcode[16] = '\0';

    for (i = 0; i < line; i++) {
        printf("%05X ", startLineAddr + i * 16);
        for (j = 0; j < 16; j++) {
            addr = startLineAddr + i * 16 + j;
            if (addr < s) {
                printf("   ");
                ASCIIcode[j] = '.';
            }
            else if (addr > e) {
                printf("   ");
                ASCIIcode[j] = '.';
            }
            else {
                printf("%02X ", *(MEMORY + addr));
                if (*(MEMORY + addr) >= 0x20 && *(MEMORY + addr) <= 0x7E) ASCIIcode[j] = *(MEMORY + addr);
                else ASCIIcode[j] = '.';
            }
        }
        printf("; %s\n", ASCIIcode);
    }


    
    return 1;
}

int edit(char* address, char* value) {
    // e[dit] address, value
    int addr = strToHex(address);
    int val = strToHex(value);

    // incorrect input or addr and val are not hex
    if (addr == -1 || val == -1) return 0;

    if (val > 0xFF) {
        return 0;
    }
    // then edit memory
    unsigned char realValue = val & ONE_BYTE;
    memcpy(MEMORY + addr, &realValue, 1);


    return 1;
}

int fill(char* start, char* end, char* value) {
    // f[ill] start, end, value
    int s, e, v;
    s = strToHex(start);
    e = strToHex(end);
    v = strToHex(value);

    // incorrect input or addr and val are not hex
    if (s == -1 || e == -1 || v == -1) {
        printf("incorrect input. Please text again\n");
        return 0;
    }

    if (!validAddrRange(s, e)) {
        printf("segmentation fault\n");
        return 0;
    }

    unsigned char realValue = v & ONE_BYTE;
    for (int i = s; i <= e; i++)
        memcpy(MEMORY + i, &realValue, 1);

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

    return hex;
}

int isHex(char ch) {
    if ( (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f') || (ch >= '0' && ch <= '9') )
        return 1;
    else
        return 0;
}
