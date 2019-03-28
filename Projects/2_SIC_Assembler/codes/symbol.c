/*************************************************
 ************** System Programming ***************
 ******************* symbol.c ********************
 ******************** Yuseok *********************
 ******************* ~ 190408 ********************
 *************************************************/
#include "20171690.h"

int symbol() {
    // print symbol table of recently assembled file
    if (!SYMTAB) {
        // no symbol table
        printf("assemble file first!\n");
        return 0;
    }

    symNode *tmp1, *tmp2, *tmp3, *tmp4;

    for (char ch = 'Z'; ch >= 'A'; ch--) {
        int idx = (ch - 'A') / 7;
        tmp1 = SYMTAB[idx];
        tmp2 = SYMTAB[7 + idx];
        tmp3 = SYMTAB[14 + idx];
        tmp4 = SYMTAB[21 + idx];

    }


    return 1;
}

int isStr(char* Str) {
    // if str contains no character, return 0. else 1
    return Str ? 1 : 0;
}

int addSym(char* label, int LOC) {
    // add symbol to the SYMTAB
    // if label already exist, return 0
    // if add successfully, return 1

    if (!SYMTAB) {
        // no Symbol table.
        // allocate memory and initialize the table
        SYMTAB = (symNode**)malloc(SYMTAB_SIZE * sizeof(symNode*));
        for (int i = 0; i < SYMTAB_SIZE; i++) SYMTAB[i] = NULL;
    }

    if (findSym(label) != -1) {
        // label exist
        printf("Label already exist\n");
        return 0;
    }

    // create new node
    symNode* pNew = (symNode*)malloc(sizeof(symNode));
    pNew->Loc = LOC; 
    strcpy(pNew->symbol,label);
    pNew->link = NULL;

    int idx = symHashFunc(label);
    if (!SYMTAB[idx]) {
        SYMTAB[idx] = pNew;
        return 1;
    }

    symNode* pMove = SYMTAB[idx];
    // take shorter symbol's length
    if ( getBiggerStr(pNew->symbol, pMove->symbol) ) {
        // header - pMove - pNew
        pNew->link = pMove;
        pMove->link = pNew;
    }
    else {
        // header - pNew - pMove
        pNew->link = pMove;
        SYMTAB[idx] = pNew;
    }

    int flag = 0, i;

    if (!pMove->link) 
        // if the header has only one node
        return 1;

    // if the header has more than two nodes
    symNode* ptmp = pMove->link;
    // HEADER - ... - pMove - ptmp - ...

    while (ptmp) {

        if ( getBiggerStr(pNew->symbol, ptmp->symbol) ) {
            // pMove - pNew - ptmp
            pNew->link = ptmp;
            pMove->link = pNew;
            return 1;
        }
        else {
            // pMove - ptmp - pNew
            pNew->link = ptmp;
            ptmp->link = pNew;
            return 1;
        }

        pMove = pMove->link;
        ptmp = pMove->link;
    }
    pMove->link = pNew;

    return 1;
}

void freeSymTab() {
    symNode* pFree, *ptmp;
    for (int i = 0; i < SYMTAB_SIZE; i++) {
        ptmp = SYMTAB[i];
        while (ptmp) {
            pFree = ptmp;
            ptmp = ptmp->link;
            free(pFree);
        }
    }
    free(SYMTAB);
}

int symHashFunc(char* label) {
    // Assum proper label input
    int idx = 0;
    int rmd = (toUpper(label[0])  - 'A') % 7;

    for (int i = 0; i < (int)strlen(label); i++)
        idx += label[i];

    int quo = idx % 4;
    idx = quo * 7 + rmd;

    return idx;
}

int findSym(char* label) {
    // if found, return Loc of the label. else -1
    if (!SYMTAB) {
        // no Symbol Table
        printf("No Symbol Table. Please assemble file first\n");
        return -1;
    }
    int idx = symHashFunc(label);
    symNode* pMove;
    for (pMove = SYMTAB[idx]; pMove; pMove = pMove->link) {
        if (strcmp(pMove->symbol, label) == 0) {
            return pMove->Loc;
        }
    }

    return -1;
}

int getBiggerStr(char* str1, char* str2){
    // Bigger means closer to Z
    // return 0 : str1 > str2
    // return 1 : str1 < str2
    int len1 = (int)strlen(str1);
    int len2 = (int)strlen(str2);
    int shorterLen = len1 > len2 ? len2 : len1;
    // if flag set, str1 > str2
    int flag = 0;
    int i;

    for (i = 0; i < shorterLen; i++) {
        if (str1[i] > str2[i]) {
            flag = 1;
            break;
        }
    }

    if (i == shorterLen) {
        // longer one is bigger
        flag = (shorterLen == len1) ? 0 : 1;
    }

    return flag ? 0 : 1;
}

int getMaxofFour(char* str1, char* str2, char* str3, char* str4) {
    if ( getBiggerStr(str2, str1) * getBiggerStr(str3, str1) * getBiggerStr(str4, str1) ) return 0;
    if ( getBiggerStr(str1, str2) * getBiggerStr(str3, str2) * getBiggerStr(str4, str2) ) return 0;
    if ( getBiggerStr(str1, str3) * getBiggerStr(str2, str3) * getBiggerStr(str4, str3) ) return 0;
    if ( getBiggerStr(str1, str4) * getBiggerStr(str2, str4) * getBiggerStr(str3, str4) ) return 0;

    return -1;
}
