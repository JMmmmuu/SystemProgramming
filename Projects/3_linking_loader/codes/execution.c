/*************************************************
 ************** System Programming ***************
 ***************** execution.c *******************
 ******************** Yuseok *********************
 ******************* ~ 190506 *********************
 *************************************************/
#include "20171690.h"
#include "execution.h"

int executeProg() {
    // execute last linked & loaded program
    // its starting addr & len is stored in EXEC_ADDR & EXEC_LEN
    // if execute successfully, return 1
    // else, return 0

    if (EXEC_LEN == -1) {
        printf("Please load the program first\n");
        return 1;
    }

    int currentLOC = EXEC_ADDR;








    return 1;
}

void printReg() {
    printf("\t\tA : %012X X : %08X\n", A, X);
    printf("\t\tL : %012X PC: %012X\n", L, PC);
    printf("\t\tB : %012X S : %012X\n", B, S);
    printf("\t\tT : %012X\n", T);
}

int setBP(char* addr) {
    // set BreakPoint
    // if success, return 1
    // else, return 0
    int bpAddr = strToHex(addr, 0);
    if (bpAddr == -1)   // wrong parameter
        return 0;

    BPNode* pNew = (BPNode*)malloc(sizeof(BPNode));
    pNew->bp = bpAddr; pNew->link = NULL;

    if (!BPHead) {
        BPHead = pNew;
        BPTail = pNew;
        return 1;
    }
    BPTail->link = pNew;
    BPTail = pNew;
    return 1;
}

void clearBP() {
    if (!BPHead) return ;

    BPNode* pFree;
    while (BPHead) {
        pFree = BPHead;
        BPHead = BPHead->link;
        free(pFree);
    }
    BPHead = BPTail = NULL;

    printf("Breakpoint Cleared!\n");
}

void printBP() {
    if (!BPHead) {
        printf("\t    No Breakpoint Set\n");
        return ;
    }
    printf("\t    breakpoint\n");
    printf("\t    ----------\n");
    BPNode* ptmp = BPHead;
    for ( ; ptmp; ptmp = ptmp->link)
        printf("\t    %04X\n", ptmp->bp);
}
