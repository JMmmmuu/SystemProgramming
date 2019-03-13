/*************************************************
 ************** System Programming ***************
 ******************** Yuseok *********************
 ******************** 190000 *********************
 *************************************************/
#include "header.h"

int opcode(char* mnemonic) {
    // if found correct mnemonic, return 1, else return 0
    int idx = hashFunction(mnemonic);
    opNode* pMove;

    for (pMove = opTable[idx]; pMove; pMove = pMove->link) {
        if (strcmp(pMove->operation, mnemonic) == 0) {
            printf("opcode is %X\n", pMove->opcode);
            return 1;
        }
    }
    printf("Wrong mnemonic\n");
    return 0;
}
void opcodeList() {
    //if (readOpTable()) return ;

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
        return 0;
    }

    int idx;
    int opcode;
    char operation[7];
    char formatStr[4];

    while (fscanf(fp, "%X %s %s\n", &opcode, operation, formatStr) != EOF) {
        opNode* pNew = (opNode*)malloc(sizeof(opNode));
        pNew->opcode = opcode;
        strcpy(pNew->operation, operation);
        if (strcmp(formatStr, "3/4") == 0) pNew->format = 3;
        else if (strcmp(formatStr, "1") == 0) pNew->format = 1;
        else if (strcmp(formatStr, "2") == 0) pNew->format = 2;
        else pNew->format = 0;
        pNew->link = NULL;

        idx = hashFunction(operation);
        if (!opTable[idx])
            opTable[idx] = pNew;
        else {
            opNode* pMove;
        
            for (pMove = opTable[idx]; pMove->link; pMove = pMove->link) ;
            pMove->link = pNew;
        }
    }
    return 1;
}

int hashFunction(char* op) {
    int idx = 0;
    for (int i = 0; i < (int)strlen(op); i++)
        idx += op[i];

    return idx % 20;
}
