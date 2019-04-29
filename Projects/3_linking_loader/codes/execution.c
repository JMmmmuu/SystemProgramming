/*************************************************
 ************** System Programming ***************
 ***************** execution.c *******************
 ******************** Yuseok *********************
 ******************* ~ 190506 *********************
 *************************************************/
#include "20171690.h"
#include "execution.h"


int setBP(char* addr) {
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
