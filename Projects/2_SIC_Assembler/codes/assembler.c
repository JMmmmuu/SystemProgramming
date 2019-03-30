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
    //if (pass2(asmFP, filename)) {
    //    printf("pass2 completed\n");
    //}

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
    // initialize global vars
    if (numHead) freeNums();
    if (SYMTAB) freeSymTab();
    numNode* pLast = numHead;

    if (line[0] == '.' || isBlankLine(line))
        pLast = addNum(lineNum, -1, pLast, 3);

    while (line[0] == '.' || isBlankLine(line)) {
        // skip comment lines & blank lines
        memset(line, '\0', (int)sizeof(line));
        if ( !isStr(fgets(line, MAX_ASM_LINE, fp)) ) {
            // .asm file includes only comments
            // do not create any symbol table
            return 1;
        }
        lineNum++;
        pLast = addNum(lineNum, -1, pLast, 3);
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
        pLast = addNum(lineNum, startingAddr, pLast, 1);

        memset(line, '\0', (int)sizeof(line));
        for (i = 0; i < MAX_TOKEN_NUM; i++) token[i] = NULL;
        if ( !isStr(fgets(line, MAX_ASM_LINE, fp)) ) return 1;

        lineNum++;
    }
    else strcpy(programName, "\0");
    LOCCTR = startingAddr;

    SYMTAB = (symNode**)malloc(SYMTAB_SIZE * sizeof(symNode*));
    for (i = 0; i < SYMTAB_SIZE; i++) SYMTAB[i] = NULL;
    flag = 0;

    while ( !feof(fp) ) { 
        // while not end of file
        if (line[0] == '.' || isBlankLine(line)) {
            // input is comment line or blank line
            instructionSize = 0;
            pLast = addNum(lineNum, -1, pLast, 3);
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
            pLast = addNum(lineNum, LOCCTR, pLast, 0);
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
    addNum(lineNum, LOCCTR, pLast, 0);

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

int pass2(FILE* fp, char* filename) {
    // Assemble instructions (translating opcodes and looking up addrs)
    // Generate data values defined by BYTE, WORD, etc
    // Perform processing of assembler directives not done during Pass 1
    // Write obj program and assembly listing.

    fseek(fp, 0, SEEK_SET);     // move to the start
    int i, tokenNum, startingAddr, endAddr;
    numNode* pCurrent = numHead;
    char* line = (char*)malloc(MAX_ASM_LINE * sizeof(char));
    char** token = (char**)malloc(MAX_TOKEN_NUM * sizeof(char*));
    for (i = 0; i < MAX_TOKEN_NUM; i++) token[i] = NULL;
    tRHead = NULL; tRTail = NULL;

    FILE* LF = fopen(nameToListing(filename), "w");     // listing file pointer
    FILE* OF = fopen(nameToObj(filename), "w");         // object file pointer

    if (!pCurrent) {
        printf("No content in the [%s] file", filename);
        return 0;
    }

    if (pCurrent->s_flag) {
        // exist START directive
        fgets(line, MAX_ASM_LINE, fp);
        tokenNum = tokenizeAsmFile(&token, line);

        fprintf(LF, "\t%d\t%04X\t", pCurrent->lineNum * 5, pCurrent->LOC);
        for (i = 0; i < tokenNum; i++)
            fprintf(LF, "\t%s", token[i]);
        fprintf(LF, "\n");

        startingAddr = strToHex(token[2]);
        numNode* pMove;
        for (pMove = numHead; pMove->link; pMove = pMove->link) ;
        endAddr = pMove->LOC;
        fprintf(OF, "H%-6s%06X%06X", token[0], startingAddr, endAddr - startingAddr);
        pCurrent = pCurrent->link;
    }
    else {
        // no START directive
        // starting addr == 0. no program name
        numNode* pMove;
        for (pMove = numHead; pMove->link; pMove = pMove->link) ;
        startingAddr = 0;
        endAddr = pMove->LOC;
        fprintf(OF, "H      000000%06X", endAddr);
    }
    PC = startingAddr;

    while (pCurrent) {
        fflush(stdin);
        for (i = 0; i < MAX_TOKEN_NUM; i++) token[i] = NULL;
        memset(line, '\0', (int)sizeof(line));
        fgets(line, MAX_ASM_LINE, fp);

        if (pCurrent->skip_flag) {
            // if the  line is comment or blank
            fprintf(LF, "\t%d\t\t\t\t%s\n", pCurrent->lineNum * 5, removeSpace(line));
            pCurrent = pCurrent->link;
            continue;
        }
        if (pCurrent->e_flag) {
            // END directive
            fprintf(OF, "E%06X", numHead->LOC);
            fprintf(LF, "\t%d\t\t\t\t\t\t%s", pCurrent->lineNum * 5, removeSpace(line));
            break;
        }

        // TEXT RECORD
        line = toUpperCase(line);
        tokenNum = tokenizeAsmFile(&token, line);
        int format, directiveNum;
        int objCode;

        // set Program Counter
        if (pCurrent->link) PC = pCurrent->link->LOC;

        if (line[0] == ' ' || line[0] == '\t') {
            // no label follows
            format = opcode(token[0], 3);
            if ( (token[0])[0] == '+') format = 4;
            if ( format ) {
                // There exist matching operation
                //objCode = getObjCode(token, format, 0);

                fprintf(LF, "\t%d\t%04X\t\t\t",  pCurrent->lineNum * 5, pCurrent->LOC);
                for (i = 0; i < tokenNum; i++)
                    fprintf(LF, "\t%s", token[i]);

                fprintf(LF, "\t\t\t\t");
                //printObjCode(format, objCode, LF);
                fprintf(LF, "\n");
                enqueue(objCode, format, pCurrent->LOC, OF);
            }
            else {
                // no matching operation
                // it can be directives
                // or wrong input
                directiveNum = isDirective(token[0]);
                if ( !directiveNum ) {
                    // wrong input
                    printf("Wrong!!\n");
                    return 0;
                }
                switch (directiveNum) {
                    case 5:     // RESB
                    case 6:     // RESW
                        dequeue(OF);
                    case 7:
                        // No need to create opcode
                        fprintf(LF, "\t%d\t%04X\t\t\t", pCurrent->lineNum * 5, pCurrent->LOC);
                        for (i = 0; i < tokenNum; i++)
                            fprintf(LF, "\t%s", token[i]);
                        fprintf(LF, "\n");
                        continue;
                }
            }
        }
        else {
            // label!!
            format = opcode(token[1], 3);
            if ( (token[1])[0] == '+' ) format = 4;
            if ( format ) {
                // exist matching operation
                //objCode = getObjCode(&(token[1]), format, 0);

                fprintf(LF, "\t%d\t%04X\t", pCurrent->lineNum * 5, pCurrent->LOC);
                for (i = 0; i < tokenNum; i++)
                    fprintf(LF, "\t%s", token[i]);

                fprintf(LF, "\t\t\t\t");
                //printObjCode(format, objCode, LF);
                fprintf(LF, "\n");
                enqueue(objCode, format, pCurrent->LOC, OF);
            }
            else {
                // no matching operation
                // directives or wrong input
                directiveNum = isDirective(token[1]);
                if ( !directiveNum ) {
                    // wrong input
                    printf("Wrong!\n");
                    return 0;
                }
                // directive!
                switch (directiveNum) {
                    case 3:     // BYTE
                        //objCode = getObjCode(&(token[1]), 5, 1);
                        fprintf(LF, "\t%d\t%04X\t", pCurrent->lineNum * 5, pCurrent->LOC);
                        for (i = 0; i < tokenNum; i++)
                            fprintf(LF, "\t%s", token[i]);
                        fprintf(LF,  "\t\t\t\t");
                        //printObjCode(3, objCode, LF);
                        fprintf(LF, "\n");
                        enqueue(objCode, 3, pCurrent->LOC, OF);

                        break;
                    case 4:     // WORD
                        //objCode = getObjCode(&(token[1]), 5, 2);
                        fprintf(LF, "\t%d\t%04X\t", pCurrent->lineNum * 5, pCurrent->LOC);
                        for (i = 0; i < tokenNum; i++)
                            fprintf(LF, "\t%s", token[i]);
                        fprintf(LF,  "\t\t\t\t");
                        //printObjCode(3, objCode, LF);
                        enqueue(objCode, 3, pCurrent->LOC, OF);

                        break;
                    case 5:     // RESB
                    case 6:     // RESW
                        fprintf(LF, "\t%d\t%04X\t", pCurrent->lineNum, pCurrent->LOC);
                        for (i = 0; i < tokenNum; i++)
                            fprintf(LF, "\t%s", token[i]);
                        fprintf(LF, "\n");

                        dequeue(OF);
                        break;

                    case 7:
                        // No need to create opcode
                        fprintf(LF, "\t%d\t%04X\t", pCurrent->lineNum * 5, pCurrent->LOC);
                        for (i = 0; i < tokenNum; i++)
                            fprintf(LF, "\t%s", token[i]);
                        fprintf(LF, "\n");
                        continue;
                }
            }
            

        }




        pCurrent = pCurrent->link;
    }






    return 1;
}

unsigned char* getObjCode(char** token, int format, int type) {
    // format: operation code format / or size
    // type:    if 0, operation
    //          if 1, BYTE Const
    //          if 2, WORD Const
    unsigned char n, i, x, b, p, e;
    unsigned char r1, r2, reg;
    unsigned char b1, b2, b3, b4;
    int disp, addr;
    int opCode;
    unsigned char* objCode;
    int tokenNum, lp, target;
    for (tokenNum = 0; token[tokenNum]; tokenNum++) ;

    if (type == 0) {
        objCode = (unsigned char*)malloc(format * sizeof(unsigned char));
        opCode = opcode(token[0], 2);
        opCode = opCode & ONE_BYTE;
        memcpy(objCode, &opCode, 1);

        switch (format) {
            case 1:
                return objCode;
            case 2:
                // get register code
                if (isComma(token[tokenNum-1])) {
                    token[tokenNum] = strtok(token[tokenNum-1], ",");
                    while (token[tokenNum])
                        token[++tokenNum] = strtok(NULL, ",");
                    tokenNum--;
                }

                r1 = token[1] ? getRegNum(token[1]) : 0x00;
                r2 = token[2] ? getRegNum(token[2]) : 0x00;
                reg = (r1 << 4 & 0xF0) || (r2 & 0x0F);
                memcpy(objCode + 1, &reg, 1);
                return objCode;
            case 3:
                // operand
                // get nixbpe
                if (token[1]) {
                    e = 0;
                    if (token[1][0] == '#') {
                        // immediate
                        n = 0; i = 1;
                        token[1] += 1;
                    }
                    else if (token[1][0] == '@') {
                        // indirect
                        n = 1; i = 0;
                        token[1] += 1;
                    }
                    else {
                        // simple
                        n = 1; i = 1;
                    }

                    if (strToHex(token[1]) == -1) {
                        // find symbol
                        target = findSym(token[-1]);
                        if (target == -1) {
                            printf("Error!!\n");
                            return 0;
                        }
                        disp = target - PC;
                        if (disp >= (unsigned int)0xFFF && disp < 0x1000) {
                            b = 0; p = 1;
                        }
                        else disp = PC - B;
                        if (disp >= 0 && disp < 0x1000) {
                            b = 1; p = 0;
                        }
                        else {
                            format = 4;
                            return 0;
                        }

                        b1 = (opCode + n * 2 + i) & ONE_BYTE;
                        b2 = (((x * 8 + b * 4 + p * 2 + e) << 4 & 0xF0) | ((disp / 256) & 0x0F)) & ONE_BYTE;
                        b3 = (disp % 256) & ONE_BYTE;
                        memcpy(objCode, &b1, 1);
                        memcpy(objCode, &b2, 1);
                        memcpy(objCode, &b3, 1);
                    }




                }
                else {
                    // no operand
                    n = 1; i = 1;
                    objCode[0] += n * 2 + i;
                    b2 = 0x00;
                    for (lp = 1; lp < 3; lp++)
                        memcpy(objCode + i, &b2, 1);
                }



                // get disp

                break;
            case 4:
                break;
        }
    }

    
    return 0;
}

void printObjCode(int format, unsigned char* objCode, FILE* fp) {
    switch (format) {
        case 1:
            fprintf(fp, "%02X", (unsigned int)objCode);
        case 2:
            fprintf(fp, "%04X", (unsigned int)objCode);
        case 3:
            fprintf(fp, "%06X", (unsigned int)objCode);
        case 4:
            fprintf(fp, "%08X", (unsigned int)objCode);
    }
}

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

int tokenizeAsmFile(char*** token, char* input) {
    // tokenize all the possible options into token
    // return number of strings
    int cnt = 0;
    char* tmp;
    int commaFlag = removeSpaceAroundComma(input);
    input = removeSpace(input);

    (*token)[cnt] = strtok(input, " \t");
    while (cnt < MAX_TOKEN_NUM && (*token)[cnt]) {
        tmp = strtok(NULL, "\0");
        if (!tmp) break;
        (*token)[++cnt] = strtok(removeSpace(tmp), " \t");
    }
    cnt++;
/*
    if (commaFlag) {
        // contain comma
        printf("\n");
        int tp;
        while ((*token)[cnt]) {
            tp = cnt;
            (*token)[++cnt] = strtok((*token)[tp], ",");
        }
    }
*/
    for (int i = 0; i < cnt; i++)
        printf("\t\t%s", (*token)[i]);
    printf("\n");

    return cnt;
}

int removeSpaceAroundComma(char* input) {
    // if comma included, return 1, else 0
    int isComma = 0;
    int cnt;
    for (int i = 1; input[i]; i++) {
        if (input[i] == ',') {
            isComma = 1;
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
    return isComma;
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

void enqueue(int addr, int size, int LOC, FILE* OF) {
    tRecord* pNew = (tRecord*)malloc(sizeof(tRecord));
    pNew->addr = addr;
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
    if (!tRHead) return ;
        fprintf(OF, "T%06X%02X", tRHead->LOC, tRTail->LOC - tRHead->LOC);

        while (tRHead) {
            //printObjCode(tRHead->size, tRHead->addr, OF);

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
