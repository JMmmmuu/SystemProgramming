/*************************************************
 ************** System Programming ***************
 ******************* memory.c ********************
 ******************** Yuseok *********************
 ******************* ~ 190408 ********************
 *************************************************/
#include "20171690.h"

int dump(char* start, char* end, int type) {
    // du[mp] [start, end]
    int s, e;
    int tmp = END_ADDR;
    switch (type) {
        case 0:         // du[mp]
            if (END_ADDR > 0xFFFFF) END_ADDR = 0;
            s = END_ADDR;
            e = ((END_ADDR + 0x9F) > 0xFFFFF) ? 0xFFFFF : END_ADDR + 0x9F;

            END_ADDR = e + 0x1;
            break;  
        case 1:         // du[mp] start
            s = strToHex(start, 1);
            e = ((s + 0x9F) > 0xFFFFF) ? 0xFFFFF : s + 0x9F;

            END_ADDR = e + 0x1;
            break;
        case 2:         // du[mp] start, end
            s = strToHex(start, 1);
            e = strToHex(end, 1);

            END_ADDR = e + 0x1;
            break;
    }

    // incorrect input or addr and val are not hex
    if (s == -1 || e == -1) {
        printf("Incorrect input. Please text again\n");
        END_ADDR = tmp;
        return 0;
    }

    if (!validAddrRange(s, e)) {
        END_ADDR = tmp;
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
    int addr = strToHex(address, 1);
    int val = strToHex(value, 1);

    // incorrect input or addr and val are not hex
    if (addr == -1 || val == -1) {
        printf("Incorrect input. Please text again\n");
        return 0;
    }

    if (val > 0xFF) {
        printf("Invalid value. Too big!\n");
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
    s = strToHex(start, 1);
    e = strToHex(end, 1);
    v = strToHex(value, 1);

    // incorrect input or addr and val are not hex
    if (s == -1 || e == -1 || v == -1) {
        printf("Incorrect input. Please text again\n");
        return 0;
    }

    if (!validAddrRange(s, e)) {
        return 0;
    }

    if (v > 0xFF) {
        printf("Invalid value. Too big!\n");
        return 0;
    }
    unsigned char realValue = v & ONE_BYTE;
    for (int i = s; i <= e; i++)
        memcpy(MEMORY + i, &realValue, 1);

    return 1;
}

void reset() {
    // reset
    for (int i = 0; i < MEMORY_SIZE; i++)
        MEMORY[i] = 0x00;
}

int validAddr(int addr) {
    // if given address is vaild, return 1, else return 0
    if (addr >= 0 && addr <= 0xFFFFF)
        return 1;
    else
        return 0;
}

int validAddrRange(int start, int end) {
    if (!validAddr(start)) { 
        printf("Segmentation fault: Cannot access %05X\n", start);
        return 0;
    }
    else if (!validAddr(end)) {
        printf("Segmentation fault: Cannot access %05X\n", end);
        return 0;
    }
    else if (end < start) {
        printf("Incorrect Address Range. End address must be bigger than Start address.\n");
        return 0;
    }
    return 1;
}

int strToHex(char* param, int type) {
    // if parameter is not a hex, or incorrect parser return -1
    // if type == 1, print error message
    for (int i = 0; i < (int)strlen(param); i++) {
        if ( !isHex(param[i]) ) {
            if (type) printf("Address is not a Hexadecimal!\n");
            return -1;
        }
    }
    
    // else, return proper hexa decimal number
    int hex;
    int res = sscanf(param, "%x", &hex);
    if (res == 0) {
        if (type)printf("error occured. Please type again\n");
        return -1;
    }

    return hex;
}

int isHex(char ch) {
    if ( (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f') || (ch >= '0' && ch <= '9') )
        return 1;
    else
        return 0;
}

int strToDecimal(char* param) {
    // if parameter is not a hex, or incorrect parser return -1
    for (int i = 0; i < (int)strlen(param); i++) {
        if ( !isHex(param[i]) ) {
            return -1;
        }
    }
    
    // else, return proper hexa decimal number
    int dec;
    int res = sscanf(param, "%d", &dec);
    if (res == 0) {
        printf("error occured. Please type again\n");
        return -1;
    }

    return dec;
}

