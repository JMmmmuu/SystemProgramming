/*************************************************
 ************** System Programming ***************
 ************* assember_functions.c **************
 ******************** Yuseok *********************
 ******************* ~ 190408 ********************
 *************************************************/
#include "20171690.h"

unsigned char getRegNum(char* reg) {
    if (strcmp(reg, "A") == 0) return 0x00;
    if (strcmp(reg, "X") == 0) return 0x01;
    if (strcmp(reg, "L") == 0) return 0x02;
    if (strcmp(reg, "B") == 0) return 0x03;
    if (strcmp(reg, "S") == 0) return 0x04;
    if (strcmp(reg, "T") == 0) return 0x05;
    if (strcmp(reg, "F") == 0) return 0x06;
    if (strcmp(reg, "PC") == 0) return 0x08;
    if (strcmp(reg, "SW") == 0) return 0x09;

    return 0xFF;
}

int isWhiteSpace(char ch) {
    return (ch == ' ' || ch == '\n' || ch == '\t') ? 1 : 0;
}

int isBlankLine(char* input) {
    for (int i = 0; i < (int)strlen(input); i++)
        if (!isWhiteSpace(input[i])) return 0;
    return 1;
}

int isDirective(char* token){
    // check if the token is a directive
    // if it is, return corresponding number. else 0
    // START, END, BYTE, WORD, RESB, RESW
    // BASE
    if (!token) return 0;

    if (strcmp(token, "START") == 0) return 1;
    if (strcmp(token, "END") == 0) return 2;
    if (strcmp(token, "BYTE") == 0) return 3;
    if (strcmp(token, "WORD") == 0) return 4;
    if (strcmp(token, "RESB") == 0) return 5;
    if (strcmp(token, "RESW") == 0) return 6;

    if (strcmp(token, "BASE") == 0) return 7;

    return 0;
}

char* nameToListing(char* filename) {
    char* lstName = (char*)malloc(50 * sizeof(char));

    for (int i = 0; i < (int)strlen(filename); i++) {
        if (filename[i] != '.') 
            lstName[i] = filename[i];
        else break;
    }

    strcat(lstName, ".lst");
    
    return lstName;
}

char* nameToObj(char* filename) {
    char* objName = (char*)malloc(50 * sizeof(char));

    for (int i = 0; i < (int)strlen(filename); i++) {
        if (filename[i] != '.') 
            objName[i] = filename[i];
        else break;
    }

    strcat(objName, ".obj");
    
    return objName;
}

char toUpper(char ch){
    if (ch >= 'a' && ch <= 'z') ch -= 'a' - 'A';
    return ch;
}

char* toUpperCase(char* input) {
    for (int i = 0; i < (int)strlen(input); i++)
        if (input[i] >= 'a' && input[i] <= 'z') input[i] -= 'a' - 'A';
    return input;
}

int byteSize(char* input) {
    if (!input) return -1;
    int size;

    if (input[0] == 'C') {
        if (input[1] == input[(int)strlen(input)-1] && input[1] == '\'') {
            size = (int)strlen(input) - 3;
        }
        else return -1;
    }
    if (input[0] == 'X') {
        if (input[1] == input[(int)strlen(input)-1] && input[1] == '\'') {
            size = (int)strlen(input) - 3;
            size = (size % 2 == 0) ? size / 2 : size / 2 + 1;
        }
        else return -1;
    }

    return size;
}

int wordSize(char* input) {
    if (!input) return -1;
    int size = WORD_SIZE;

    return size;
}

int resbSize(char* input) {
    if (!input) return -1;
    int size;
    if ( (size = strToDecimal(input)) == -1 ) return -1;

    return size;
}

int reswSize(char* input) {
    if (!input) return -1;
    int size;
    if ( (size = strToHex(input)) == -1 ) return -1;

    return size * WORD_SIZE;
}

numNode* addNum(int lineNum, int LOC, numNode* pLast, int flag_type) {
    numNode* pNew = (numNode*)malloc(sizeof(numNode));
    pNew->lineNum = lineNum; pNew->LOC = LOC;
    switch (flag_type) {
        case 0:
            pNew->s_flag = 0; pNew->e_flag = 0; pNew->skip_flag = 0;
            break;
        case 1:
            pNew->s_flag = 1; pNew->e_flag = 0; pNew->skip_flag = 0;
            break;
        case 2:
            pNew->s_flag = 0; pNew->e_flag = 1; pNew->skip_flag = 0;
            break;
        case 3:
            pNew->s_flag = 0; pNew->e_flag = 0; pNew->skip_flag = 1;
            break;
    }
    pNew->link = NULL;

    if (!numHead) {
        numHead = pNew;
        return pNew;
    }
    pLast->link = pNew;
    return pNew;
}

void freeNums() {
    if (!numHead) return ;
    numNode* pFree = numHead;
    while (numHead) {
        pFree = numHead;
        numHead = numHead->link;
        free(pFree);
    }
    numHead = NULL;
}

void printNums() {
    numNode* pMove;
    for (pMove = numHead; pMove; pMove = pMove->link)
        printf("\t%d\t%06X\n", pMove->lineNum, pMove->LOC);
}

void enqueue(unsigned char* objCode, int size, int LOC, FILE* OF) {
    tRecord* pNew = (tRecord*)malloc(sizeof(tRecord));
    memcpy(pNew->objCode, objCode, size);
    pNew->size = size;
    pNew->LOC = LOC;
    pNew->link = NULL;

    if ( !tRHead ) {
        tRHead = pNew;
        tRTail = pNew;
        return ;
    }

    if (LOC - tRHead->LOC > MAX_OBJ_TRECORD) {
        dequeue(OF);
        tRHead = pNew;
        tRTail= pNew;
        return ;
    }

    tRTail->link = pNew;
    tRTail = pNew;
}

void dequeue(FILE* OF) {
    // pop node from the queue
    // and print obj codes in the .obj file
    if (!tRHead) return ;
        // print T, starting LOC, and Length of the record
        fprintf(OF, "T%06X%02X", tRHead->LOC, tRTail->LOC - tRHead->LOC);

        while (tRHead) {
            printObjCode(tRHead->size, tRHead->objCode, OF);
            tRecord* pFree = tRHead;
            tRHead = tRHead->link;
            free(pFree);
        }
        tRHead = NULL;
        tRTail = NULL;
}

int isComma(char* input) {
    for (int i = 0; i < (int)strlen(input); i++)
        if (input[i] == ',') return 1;

    return 0;
}
