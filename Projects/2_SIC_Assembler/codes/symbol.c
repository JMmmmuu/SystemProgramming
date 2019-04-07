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

    int idx[4];
    int ptIdx, cnt, sign;

    symNode* ptPtr[28];
    for (int i = 0; i < 28; i++)
        ptPtr[i] = SYMTAB[i];

    for (char ch = 'Z'; ch >= 'A'; ch--) {
        ptIdx = -1;
        cnt = 0;        // num of head which has no corresponding char out of 4
        sign = 0;
        for (int i = 0; i < 4; i++) {
            idx[i] = (ch - 'A') % 7 + i * 7;
            if (!ptPtr[idx[i]]) {
                cnt++; 
                sign += 1 << i;
            }
            else if ( (ptPtr[idx[i]]->symbol)[0] != ch ) {
                cnt++;
                sign += 1 << i;
            }
        }

        if (cnt == 0) {
            // all four heads have chars
            while (ptPtr[idx[0]] && ptPtr[idx[1]] && ptPtr[idx[2]] && ptPtr[idx[3]]) {
                if ( (ptIdx = getMaxofFour(ptPtr[idx[0]]->symbol, ptPtr[idx[1]]->symbol, ptPtr[idx[2]]->symbol, ptPtr[idx[3]]->symbol)) == -1) return 0;
                printf("\t%s\t%06X\n", ptPtr[idx[ptIdx]]->symbol, ptPtr[idx[ptIdx]]->Loc);
                ptPtr[idx[ptIdx]] = ptPtr[idx[ptIdx]]->link;
                if (!ptPtr[idx[ptIdx]]) break;
                if ( (ptPtr[idx[ptIdx]]->symbol)[0] != ch ) break;
            }
        }

        if (cnt <= 1) {
            if (ptIdx == -1) {
                switch (sign) {
                    case 4: idx[2] = idx[3]; break;
                    case 2: idx[1] = idx[3]; break;
                    case 1: idx[0] = idx[3]; break;
                }
            }
            else 
                idx[ptIdx] = idx[3];

            while (ptPtr[idx[0]] && ptPtr[idx[1]] && ptPtr[idx[2]]) {
                if ( (ptIdx = getMaxofThree(ptPtr[idx[0]]->symbol, ptPtr[idx[1]]->symbol, ptPtr[idx[2]]->symbol)) == -1) return 0;;
                printf("\t%s\t%06X\n", ptPtr[idx[ptIdx]]->symbol, ptPtr[idx[ptIdx]]->Loc);
                ptPtr[idx[ptIdx]] = ptPtr[idx[ptIdx]]->link;
                if (!ptPtr[idx[ptIdx]]) break;
                if ( (ptPtr[idx[ptIdx]]->symbol)[0] != ch ) break;
            }
        }

        if (cnt <= 2) {
            if (ptIdx == -1) {
                switch (sign) {
                    case 12: break;
                    case 10: idx[1] = idx[2]; break;
                    case 6: idx[1] = idx[3]; break;
                    case 9: idx[0] = idx[2]; break;
                    case 5: idx[0] = idx[3]; break;
                    case 3: idx[0] = idx[2]; idx[1] = idx[3]; break;
                }
            }
            else
                idx[ptIdx] = idx[2];

            while (ptPtr[idx[0]] && ptPtr[idx[1]]) {
                ptIdx = getBiggerStr(ptPtr[idx[0]]->symbol, ptPtr[idx[1]]->symbol);
                printf("\t%s\t%06X\n", ptPtr[idx[ptIdx]]->symbol, ptPtr[idx[ptIdx]]->Loc);
                ptPtr[idx[ptIdx]] = ptPtr[idx[ptIdx]]->link;
                if (!ptPtr[idx[ptIdx]]) break;
                if ( (ptPtr[idx[ptIdx]]->symbol)[0] != ch ) break;
            }
        }

        if (cnt <= 3) {
            if (ptIdx == -1) {
                switch (sign) {
                    case 7: idx[0] = idx[3]; break;
                    case 11: idx[0] = idx[2]; break;
                    case 13: idx[0] = idx[1]; break;
                }
            }
            else
                idx[ptIdx] = idx[1];

            while (ptPtr[idx[0]]) {
                if ( (ptPtr[idx[0]]->symbol)[0] != ch ) break;
                if (!ptPtr[idx[0]]) break;
                printf("\t%s\t%06X\n", ptPtr[idx[0]]->symbol, ptPtr[idx[0]]->Loc);
                ptPtr[idx[0]] = ptPtr[idx[0]]->link;
            }
        }
    }

    return 1;
}

int addSym(char* label, int LOC) {
    // add symbol to the SYMTAB
    // if label already exist, return 0
    // if add successfully, return 1

    if (findSym(label) != -1) {
        // label exist
        return 0;
    }
    if (label[0] < 'A' || label[0] > 'Z') return 0;

    // create new node
    symNode* pNew = (symNode*)malloc(sizeof(symNode));
    pNew->Loc = LOC; 
    strcpy(pNew->symbol,label);
    pNew->link = NULL;

    int idx = symHashFunc(label);
    if (!SYMTAB[idx]) {
        // no node at the header
        SYMTAB[idx] = pNew;
        return 1;
    }

    symNode* pMove = SYMTAB[idx];
    if ( getBiggerStr(pNew->symbol, pMove->symbol) == 0 ) {
        // header - pNew - pMove - ...
        pNew->link = pMove;
        SYMTAB[idx] = pNew;
        return 1;
    }
    else {
        // header - pMove - pNew - ...
        pNew->link = pMove->link;
        pMove->link = pNew;

        if ( !(pNew->link) ) return 1;
    }

    // if the header has more than two nodes
    symNode* ptmp = pNew->link;
    while (ptmp) {
        // header - ... - pMove - pNew - ptmp - ...
        if ( getBiggerStr(pNew->symbol, ptmp->symbol) == 0)
            return 1;

        pMove->link = ptmp;
        pNew->link = ptmp->link;
        ptmp->link = pNew;
        // ... - pMove - ptmp - pNew - ...

        pMove = pMove->link;
        ptmp = pNew->link;
    }

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
    SYMTAB = NULL;
}

int symHashFunc(char* label) {
    // Assum proper label input
    int idx = 0;
    int rmd = (label[0]  - 'A') % 7;

    for (int i = 0; i < (int)strlen(label); i++)
        idx += label[i];

    int quo = idx % 4;
    idx = quo * 7 + rmd;

    return idx;
}

int findSym(char* label) {
    // if found, return Loc of the label. else -1
    if (label[0] < 'A' || label[0] > 'Z') return -1;
    if (!SYMTAB) {
        // no Symbol Table
        printf("No Symbol Table. Please assemble file first\n");
        return -1;
    }
    if (!isStr(label)) {
        printf("Wrong Label\n");
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

int isStr(char* Str) {
    // if str contains no character, return 0. else 1
    return Str ? 1 : 0;
}

int getBiggerStr(char* str1, char* str2){
    // Bigger means closer to Z
    // return 0 : str1 > str2
    // return 1 : str1 < str2
    int len1 = (int)strlen(str1);
    int len2 = (int)strlen(str2);
    int shorterLen = (len1 > len2) ? len2 : len1;
    // if flag set, str1 > str2
    int i;

    for (i = 0; i < shorterLen; i++) {
        if (str1[i] > str2[i])
            return 0;
        else if (str1[i] < str2[i])
            return 1;
    }

    // longer one is bigger
    if (shorterLen == len1) return 1;
    return 0;
}

int getMaxofFour(char* str1, char* str2, char* str3, char* str4) {
    if ( getBiggerStr(str2, str1) * getBiggerStr(str3, str1) * getBiggerStr(str4, str1) ) return 0;
    if ( getBiggerStr(str1, str2) * getBiggerStr(str3, str2) * getBiggerStr(str4, str2) ) return 1;
    if ( getBiggerStr(str1, str3) * getBiggerStr(str2, str3) * getBiggerStr(str4, str3) ) return 2;
    if ( getBiggerStr(str1, str4) * getBiggerStr(str2, str4) * getBiggerStr(str3, str4) ) return 3;

    return -1;
}

int getMaxofThree(char* str1, char* str2, char* str3) {
    if ( getBiggerStr(str2, str1) * getBiggerStr(str3, str2) ) return 0;
    if ( getBiggerStr(str1, str2) * getBiggerStr(str3, str2) ) return 1;
    if ( getBiggerStr(str1, str3) * getBiggerStr(str2, str3) ) return 2;

    return -1;
}

void printSymbol() {
    if (!SYMTAB) {
        printf("No SYMTAB\n");
        return ;
    }

    symNode* pMove;
    char ch[4];
    for (int i = 0; i < SYMTAB_SIZE; i++) {
        if (i % 7 == 5 || i % 7 == 6) {
            for (int j = 0; j < 3; j++)
                ch[j] = 'A' + i % 7 + j * 7;
            printf("%d [%c %c %c  ]: ", i, ch[0], ch[1], ch[2]);
        }
        else {
            for (int j = 0; j < 4; j++)
                ch[j] = 'A' + i % 7 + j * 7;
            printf("%d [%c %c %c %c]: ", i, ch[0], ch[1], ch[2], ch[3]);
        }
        pMove = SYMTAB[i];
        if (pMove) {
            for ( ; pMove->link; pMove = pMove->link)
                printf("[%s, %X] -> ", pMove->symbol, pMove->Loc);
            printf("[%s, %X]", pMove->symbol, pMove->Loc);
        }
        if (i % 7 == 6) printf("\n");
        printf("\n");
    }
}
