/*************************************************
 ************** System Programming ***************
 ******************** Yuseok *********************
 ******************** 190000 *********************
 *************************************************/
#include "header.h"

void opcode() {
    readOpTable();

    opNode* pMove;
    for (int i = 0; i < 20; i++) {
        printf("%d : ", i);
        for (pMove = opTable[i]; pMove->link; pMove = pMove->link)
            printf("[%s,%x] -> ", pMove->operation, pMove->opcode);
        printf("[%s,%x]\n", pMove->operation, pMove->opcode);
    }
}

void readOpTable() {
    
}
