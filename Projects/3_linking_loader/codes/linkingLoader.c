/*************************************************
 ************** System Programming ***************
 **************** linkingLoader.c  ***************
 ******************** Yuseok *********************
 ******************* ~ 190506 ********************
 *************************************************/
#include "20171690.h"
#include "linkingLoader.h"

int progaddr(char* addr) {
    PROGADDR = strToHex(addr, 1);
    if (PROGADDR == -1) {
        // not a corect hexa decimal num
        PROGADDR = 0;
        return 0;
    }
    if ( !validAddr(PROGADDR) ) {
        // out of valid address range
        PROGADDR = 0;
        printf("Invalid Address: out of range\n");
        return 0;
    }

    printf("PROGADDR SET\n");
    return 1;
}
