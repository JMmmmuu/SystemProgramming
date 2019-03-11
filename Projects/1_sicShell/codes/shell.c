/*************************************************
 *************************************************
 ******************** Yuseok *********************
 ******************** 190000 *********************
 *************************************************/
#include "header.h"

void help() {
    printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\n");
    printf("du[mp] [start, end]\n");
    printf("e[dit] address, value\n");
    printf("f[ill] start, end, value\n");
    printf("reset\n");
    printf("opcode mnemonic\nopcodelist\n");
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
    pNew->num = pMove->num;
}

void history() {
    HISTORY* pMove;
    for (pMove = hisHead; pMove; pMove = pMove->link) {
        printf("%d\t%s\n", pMove->num, pMove->hist);
    }
}
