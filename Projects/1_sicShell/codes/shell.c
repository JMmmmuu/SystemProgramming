/*************************************************
 ************** System Programming ***************
 ******************** Yuseok *********************
 ******************** 190000 *********************
 *************************************************/
#include "header.h"

void help() {
    printf("\t\th[elp]\n\t\td[ir]\n\t\tq[uit]\n\t\thi[story]\n");
    printf("\t\tdu[mp] [start, end]\n");
    printf("\t\te[dit] address, value\n");
    printf("\t\tf[ill] start, end, value\n");
    printf("\t\treset\n");
    printf("\t\topcode mnemonic\n\t\topcodelist\n");
}

void addHistory(char* input) {
    HISTORY* pNew = (HISTORY*)malloc(sizeof(HISTORY));
    pNew->hist = (char*)malloc(COMMAND_SIZE * sizeof(char));
    strcpy(pNew->hist, input);
    pNew->link = NULL;

    if (!hisHead) {
        hisHead = pNew;
        pNew->num = 1;
        return ;
    }
    HISTORY* pMove;
    for (pMove = hisHead; pMove->link; pMove = pMove->link) ;
    pMove->link = pNew;
    pNew->num = pMove->num + 1;
}

void history() {
    HISTORY* pMove;
    for (pMove = hisHead; pMove; pMove = pMove->link) {
        printf("\t%d\t%s\n", pMove->num, pMove->hist);
    }
}

void quit() {
    // free memories
    HISTORY* pFree;

    while (hisHead) {
        pFree = hisHead;
        hisHead = hisHead->link;
        free(pFree);
    }

    printf("exit shell\n");
}
