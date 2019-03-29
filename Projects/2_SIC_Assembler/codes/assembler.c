/*************************************************
 ************** System Programming ***************
 ****************** assember.c *******************
 ******************** Yuseok *********************
 ******************* ~ 190408 ********************
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
    
    if (pass1(asmFP)) {
        printf("pass1 completed\n");
        printSymbol();
        printNums();
    }
    if (pass2(asmFP)) {
        printf("pass2 completed\n");
    }

    fclose(asmFP);
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
    int instructionSize;
    char* line = (char*)malloc(MAX_ASM_LINE * sizeof(char));
    char programName[10];
    fseek(fp, 0, SEEK_SET);     // move to the first

    if ( !isStr(fgets(line, MAX_ASM_LINE, fp)) ) {
            // file with no content
            printf("No content in the file\n");
            return 0;
    }
    
    lineNum++;
    if (!numHead) freeNums();
    numNode* pLast = numHead;

    while (line[0] == '.' || isBlankLine(line)) {
        // skip comment lines & blank lines
        memset(line, '\0', (int)sizeof(line));
        if ( !isStr(fgets(line, MAX_ASM_LINE, fp)) ) {
            // .asm file includes only comments
            // do not create any symbol table
            return 1;
        }
        lineNum++;
        pLast = addNum(lineNum, -1, pLast);
    }

    // First line except comments & blank line
    char** token = (char**)malloc(MAX_TOKEN_NUM * sizeof(char*));
    line = toUpperCase(line);
    int tokenNum = tokenizeAsmFile(&token, line);

    /*
    printf("%d\n", tokenNum);
    for (i = 0; i < tokenNum; i++) {
        printf("%s\n", token[i]);
    }*/


    for (i = 0; i < tokenNum; i++) {
        if (isDirective(token[i]) == 1) {
            // if token[i] == "START"
            if (i+1 >= tokenNum || tokenNum == 1) {
                printf("Error occured at [%d] line: No name or starting address\n", lineNum);
                return 0;
            }
            if ( (startingAddr = strToHex(token[i+1])) == -1 ) {
                printf("Error occured at [%d] line: Wrong Starting Address\n", lineNum);
                startingAddr = 0;
                return 0;
            }
            flag = 1;
            break;
        }
    }
    if (flag) {
        // if there's START directive, save program name
        // and get next line
        strcpy(programName, token[0]);

        memset(line, '\0', (int)sizeof(line));
        for (i = 0; i < MAX_TOKEN_NUM; i++) token[i] = NULL;
        if ( !isStr(fgets(line, MAX_ASM_LINE, fp)) ) return 1;

        lineNum++;
        line = toUpperCase(line);
        tokenNum = tokenizeAsmFile(&token, line);
    }
    else strcpy(programName, "\0");
    LOCCTR = startingAddr;
    pLast = addNum(lineNum, LOCCTR, pLast);

    if (SYMTAB) freeSymTab();
    SYMTAB = (symNode**)malloc(SYMTAB_SIZE * sizeof(symNode*));
    for (i = 0; i < SYMTAB_SIZE; i++) SYMTAB[i] = NULL;
    flag = 0;

    while ( !feof(fp) ) { 
        // while not end of file
        if (line[0] == '.' || isBlankLine(line)) {
            // input is comment line or blank line
            instructionSize = 0;
            pLast = addNum(lineNum, -1, pLast);
        }
        else {
            line = toUpperCase(line);
            tokenNum = tokenizeAsmFile(&token, line);
            if (isDirective(token[0]) == 2) {
                // if END
                flag = 1;
                break;
            }
            if (line[0] == ' ' || line[0] == '\t') {
                // no label
                instructionSize = getInstructionSize(token, lineNum, 0);
                if (instructionSize == -1) return 0;
            }
            else {
                // contains label, instruction, notes, ...
                // add to symbol table
                instructionSize = getInstructionSize(token, lineNum, 1);
                if (instructionSize == -1) return 0;

                if( !addSym(token[0], LOCCTR) ) { 
                    printf("Wrong\n");
                    return 0;
                }
            }
            pLast = addNum(lineNum, LOCCTR, pLast);
        }

        LOCCTR += instructionSize;
        //printf("%d\ttokenNum: %d - ", lineNum, tokenNum);
        //for (i = 0; i < tokenNum; i++) printf("\t%s ", token[i]);
        //printf("\n\n");
        memset(line, '\0', (int)sizeof(line));
        for (i = 0; i < MAX_TOKEN_NUM; i++) token[i] = NULL;
        fgets(line, MAX_ASM_LINE, fp);
        lineNum++;

    }

    if (!flag) {
        printf("No END Directive\n");
        return 0;
    }

    return 1;
}

int getInstructionSize(char** token, int lineNum, int isLabel) {
    // return instructionSize
    // if isLabel is on, it has label. else no
    // if error occured, return 0
    int opIdx = isLabel ? 1 : 0;
    int size = 0;

    if (!token[opIdx]) return -1;
    if (token[opIdx][0] == '+') 
        // Foramt 4
        return 4;

    size = opcode(token[opIdx], 3);
    if (size == 0) {
        // token is no operation
        // it can be directives
        // or wrong input
        switch (isDirective(token[opIdx])) {
            //case 1: case 2:
            case 3:     // BYTE
                size = byteSize(token[opIdx+1]); 
                break;
            case 4:     // WORD
                size = wordSize(token[opIdx+1]);
                break;
            case 5:     // RESB
                size = resbSize(token[opIdx+1]);
                break;
            case 6:     // RESW
                size = reswSize(token[opIdx+1]);
                break;
            case 7:     // directives without memory allocation
                return 0;

            default:
                // wrong input
                printf("Error occured at [%d] line: No matching operation code - %s\n", lineNum, token[opIdx]);
                return -1;
        }
        if (size == -1) {
            // wrong syntax. 
            // ex) no input || no hexa, ...
            printf("Error occured at [%d] line: Wrong Syntax\n", lineNum);
            return -1;
        }
    }

    return size;
}

int pass2(FILE* fp) {
    // Assemble instructions (translating opcodes and looking up addrs)
    // Generate data values defined by BYTE, WORD, etc
    // Perform processing of assembler directives not done during Pass 1
    // Write obj program and assembly listing.

    fseek(fp, 0, SEEK_SET);     // move to the first



    return 1;
}

int tokenizeAsmFile(char*** token, char* input) {
    // tokenize all the possible options into token
    // return number of strings
    int cnt = 0;
    char* tmp;
    int commaFlag = removeSpaceAroundComma(input);
    input = removeSpace(input);

    (*token)[cnt] = strtok(input, " \t");
    while (cnt < 5 && (*token)[cnt]) {
        tmp = strtok(NULL, "\0");
        if (!tmp) break;
        (*token)[++cnt] = strtok(removeSpace(tmp), " \t");
    }
    cnt++;

    if (commaFlag) {
        // contain comma
        strtok((*token)[cnt], ",");
        (*token)[++cnt] = strtok(NULL, "\0");
    }

    return cnt;
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

    //printf("%s", input);
    return flag ? 1 : 0;
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

numNode* addNum(int lineNum, int LOC, numNode* pLast) {
    numNode* pNew = (numNode*)malloc(sizeof(numNode));
    pNew->lineNum = lineNum; pNew->LOC = LOC;
    pNew->link = NULL;

    printf("%d %X\n", lineNum, LOC);
    if (!numHead) {
        numHead = pNew;
        return pNew;
    }
    pLast->link = pNew;
    return pNew;
}

void freeNums() {
    numNode* pFree;
    while (numHead) {
        pFree = numHead;
        numHead = numHead->link;
        free(pFree);
    }
}

void printNums() {
    numNode* pMove;
    for (pMove = numHead; pMove; pMove = pMove->link)
        printf("\t%d\t%d\n", pMove->lineNum, pMove->LOC);
}
