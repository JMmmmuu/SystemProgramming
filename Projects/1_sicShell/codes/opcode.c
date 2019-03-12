/*************************************************
 ************** System Programming ***************
 ******************** Yuseok *********************
 ******************** 190000 *********************
 *************************************************/
#include "header.h"

void opcodeList() {
    if (readOpTable()) return ;

    opNode* pMove;
    for (int i = 0; i < 20; i++) {
        printf("%d : ", i);
        pMove = opTable[i];
        if (pMove) {
            for ( ; pMove->link; pMove = pMove->link)
                printf("[%s,%X] -> ", pMove->operation, pMove->opcode);
            printf("[%s,%X]", pMove->operation, pMove->opcode);
        }
        printf("\n");
    }
}

int readOpTable() {
    FILE* fp = fopen(OPCODE, "r");
    if (!fp) {
        printf("wrong input file\n");
        return 1;
    }
    




    int i = 0;
    int opcode;
    char operation[7];
    char formatStr[5];

    while (fscanf(fp, "%X %s %s\n", &opcode, operation, formatStr) != EOF) {
        i = i % 20;

        opNode* pNew = (opNode*)malloc(sizeof(opNode));
        pNew->opcode = opcode;
        strcpy(pNew->operation, operation);
        if (strcmp(formatStr, "3/4") == 0) pNew->format = 3;
        else if (strcmp(formatStr, "1") == 0) pNew->format = 1;
        else if (strcmp(formatStr, "2") == 0) pNew->format = 2;
        else pNew->format = 0;
        pNew->link = NULL;

        if (!opTable[i])
            opTable[i] = pNew;
        else {
            opNode* pMove;
        
            for (pMove = opTable[i]; pMove->link; pMove = pMove->link) ;
            pMove->link = pNew;
        }

        i++;
    }
    return 0;
}
