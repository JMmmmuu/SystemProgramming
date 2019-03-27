/*************************************************
 *************************************************
 ******************** Yuseok *********************
 ******************** 190000 *********************
 *************************************************/
#include "20171690.h"

int A, X, L, PC, SW, B, S, T, F;

int assemble(char* filename) {
    FILE* asmFP = fopen(filename, "r");
    if (!asmFP) {
        // No input file
        printf("%s: No such file in the directory\n", filename);
        return 0;
    }
    
    pass1(asmFP);

    return 1;
}

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

int pass1(FILE* fp) {
    // Assign addresses to all statments in the program
    // Save the values (addrs) assigned to all labels for use in Pass 2
    // Perform some processing of assembler assignment, such as determining the
    // length of data areas defined by BYTE, RESW, etc
    
    // make symtab, assign LOC
    int LOCCTR, startingAddr = 0, i;
    int lineNum = 0;
    int flag = 0;
    char* line = (char*)malloc(30 * sizeof(char));
    char filename[10];
    fseek(fp, 0, SEEK_SET);     // move to the first

    if ( !isStr(fgets(line, 30, fp)) ) {
            // file with no content
            printf("No content in the file\n");
            return 0;
    }
    
    lineNum++;
    line = removeSpace(line);
    while (line[0] == '.') {
        // skip comment lines
        memset(line, '\0', (int)sizeof(line));
        if ( !isStr(fgets(line, 30, fp)) ) {
            return 1;
        }
        line = removeSpace(line);
        lineNum++;
    }

    // First line
    char** token = (char**)malloc(MAX_TOKEN_NUM * sizeof(char*));
    int num = tokenizeAsmFile(&token, line);
    for (i = 0; i < num; i++) {
        if (isDirective(token[i]) == 1) {
            // if token[i] == "START"
            if (i+1 >= num || num == 1) {
                printf("No name or starting address at [%d] line\n", lineNum);
                return 0;
            }
            if ( (startingAddr = strToHex(token[i+1])) == -1 ) {
                printf("Error occured in .asm file\nWrong Starting Address at [%d] line\n", lineNum);
                return 0;
            }
            flag = 1;
            break;
        }
    }
    if (flag) {
        // if there's START directive, get next line
        memset(line, '\0', (int)sizeof(line));
        for (i = 0; i < MAX_TOKEN_NUM; i++) token[i] = NULL;
        if ( !isStr(fgets(line, 30, fp)) ) return 1;

        lineNum++;
        num = tokenizeAsmFile(&token, line);

    }


    while ( !feof(fp) ) {
        memset(line, '\0', (int)sizeof(line));
        for (i = 0; i < MAX_TOKEN_NUM; i++) token[i] = NULL;
        fgets(line, 30, fp);
        lineNum++;

        num = tokenizeAsmFile(&token, line);

        if (strcmp(token[0], "END") == 0) break;

        if (num >= 2) {
            if (opcode(token[1], 0)) {
                //addSym(token[0], LOCCTR);
            }
        }

        

    }




    return 1;
}

int pass2(FILE* fp) {
    // Assemble instructions (translating opcodes and looking up addrs)
    // Generate data values defined by BYTE, WORD, etc
    // Perform processing of assembler directives not done during Pass 1
    // Write obj program and assembly listing.

    fseek(fp, 0, SEEK_SET);     // move to the first



    return 1;
}

int isStr(char* Str) {
    // if str contains no character, return 0. else 1
    return Str ? 1 : 0;
}

int tokenizeAsmFile(char*** token, char* input) {
    // tokenize all the possible options into token
    // return number of strings
    int cnt = 0;
    char* tmp;
    int commaFlag = removeSpaceAroundComma(input);
    
    *token[cnt] = strtok(input, " \t");
    while (cnt < 5 && *token[cnt]) {
        tmp = strtok(NULL, "\0");
        if (!tmp) break;
        *token[++cnt] = strtok(removeSpace(tmp), " \t");
    }
    cnt++;

    if (commaFlag) {
        // contain comma
        strtok(*token[cnt], ",");
        *token[++cnt] = strtok(NULL, "\0");
    }

    return cnt;
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

    for (int i = 0; i < strlen(label); i++)
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

int removeSpaceAroundComma(char* input) {
    // if comma included, return 1, else 0
    int flag = 0;
    int cnt;
    for (int i = 1; input[i]; i++) {
        if (input[i] == ',') {
            if ( isWhiteSpace(input[i-1]) ) {
                cnt = 0;    // num of spaces
                while ( i - 1 - cnt >= 0 && isWhiteSpace(input[i - 1 - cnt]) ) cnt++;
                for (int j = 0; j < (int)strlen(input) - i; j++)
                    input[i - cnt + j] = input[i + j];
                input[(int)strlen(input) - cnt] = '\0';
                i -= cnt;
            }
            if ( isWhiteSpace(input[i+1]) ) {
                cnt = 0;
                while ( input[i + 1 + cnt] && isWhiteSpace(input[i + 1 + cnt]) ) cnt++;
                for (int j = 0; j < (int)strlen(input) - i  - 1 - cnt; j++)
                    input[i + 1 + j] = input[i + 1 + cnt + j];
                input[(int)strlen(input) - cnt] = '\0';
                i -= cnt;
            }
        }
    }
    printf("%s\n", input);
    return flag ? 1 : 0;
}

int isWhiteSpace(char ch) {
    return (ch == ' ' || ch == '\n' || ch == '\t') ? 1 : 0;
}

int isDirective(char* token){
    // check if the token is a directive
    // if it is, return corresponding number. else 0
    // START, END, BYTE, WORD, RESB, RESW
    if (!token) return 0;

    if (strcmp(token, "START") == 0) return 1;
    if (strcmp(token, "END") == 0) return 2;
    if (strcmp(token, "BYTE") == 0) return 3;
    if (strcmp(token, "WORD") == 0) return 4;
    if (strcmp(token, "RESB") == 0) return 5;
    if (strcmp(token, "RESW") == 0) return 6;

    return 0;
}

char toUpper(char ch){
    if (ch >= 'a' && ch <= 'z') ch -= 'a' - 'A';
    return ch;
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
