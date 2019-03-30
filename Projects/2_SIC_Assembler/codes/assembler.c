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

    // initialize Registers
    A = 0; X = 0; L = 0; PC = 0;
    SW = 0; B = 0; S = 0; T = 0; F = 0;
    tRHead = NULL;
    tRTail = NULL;
    
    if (pass1(asmFP)) {
        printf("pass1 completed\n");
        //printSymbol();
        //{printNums();

        //*
        if (pass2(asmFP, filename)) 
            printf("pass2 completed\n");
            //*/
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
            if ( (startingAddr = strToHex(token[i+1], 0)) == -1 ) {
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
            if (instructionSize == 0)
                pLast = addNum(lineNum, -1, pLast, 3);
            else 
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
    addNum(lineNum, LOCCTR, pLast, 2);

    return 1;
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

        fprintf(LF, "\t  %d\t%04X\t", pCurrent->lineNum * 5, pCurrent->LOC);
        for (i = 0; i < tokenNum; i++)
            fprintf(LF, "\t%s", token[i]);
        fprintf(LF, "\n");

        startingAddr = strToHex(token[2], 1);
        numNode* pMove;
        for (pMove = numHead; pMove->link; pMove = pMove->link) ;
        endAddr = pMove->LOC;
        fprintf(OF, "H%-6s%06X%06X\n", token[0], startingAddr, endAddr - startingAddr);
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
            fprintf(LF, "\t  %d\t\t\t%s\n", pCurrent->lineNum * 5, removeSpace(line));
            pCurrent = pCurrent->link;
            continue;
        }
        if (pCurrent->e_flag) {
            // END directive
            fprintf(OF, "E%06X\n", numHead->LOC);
            fprintf(LF, "\t  %d\t\t\t\t%s\n", pCurrent->lineNum * 5, removeSpace(line));
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
            // no label
            format = opcode(token[0], 3);
            if ( (token[0])[0] == '+') {
                format = 4;
                token[0] += 1;
            }
            if ( format ) {
                // There exist matching operation
                objCode = getObjCode(token, format, 0);

                fprintf(LF, "\t  %d\t%04X\t\t",  pCurrent->lineNum * 5, pCurrent->LOC);
                for (i = 0; i < tokenNum; i++)
                    fprintf(LF, "\t%s", token[i]);
                fprintf(LF, "\t\t");
                printObjCode(format, objCode, LF);
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
                        fprintf(LF, "\t  %d\t%04X\t\t", pCurrent->lineNum * 5, pCurrent->LOC);
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
            if ( (token[1])[0] == '+' ) {
                format = 4;
                token[1] += 1;
            }
            if ( format ) {
                // exist matching operation
                objCode = getObjCode(&(token[1]), format, 0);

                fprintf(LF, "\t  %d\t%04X\t", pCurrent->lineNum * 5, pCurrent->LOC);
                for (i = 0; i < tokenNum; i++)
                    fprintf(LF, "\t%s", token[i]);
                fprintf(LF, "\t\t");
                printObjCode(format, objCode, LF);
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
                        objCode = getObjCode(&(token[1]), 5, 1);
                        fprintf(LF, "\t  %d\t%04X\t", pCurrent->lineNum * 5, pCurrent->LOC);
                        for (i = 0; i < tokenNum; i++)
                            fprintf(LF, "\t%s", token[i]);
                        fprintf(LF,  "\t\t\t");
                        printObjCode(3, objCode, LF);
                        fprintf(LF, "\n");
                        enqueue(objCode, 3, pCurrent->LOC, OF);

                        break;
                    case 4:     // WORD
                        objCode = getObjCode(&(token[1]), 5, 2);
                        fprintf(LF, "\t  %d\t%04X\t", pCurrent->lineNum * 5, pCurrent->LOC);
                        for (i = 0; i < tokenNum; i++)
                            fprintf(LF, "\t%s", token[i]);
                        fprintf(LF,  "\t\t");
                        printObjCode(3, objCode, LF);
                        fprintf(LF, "\n");
                        enqueue(objCode, 3, pCurrent->LOC, OF);

                        break;
                    case 5:     // RESB
                    case 6:     // RESW
                        fprintf(LF, "\t  %d\t%04X\t", pCurrent->lineNum * 5, pCurrent->LOC);
                        for (i = 0; i < tokenNum; i++)
                            fprintf(LF, "\t%s", token[i]);
                        fprintf(LF, "\n");

                        dequeue(OF);
                        break;

                    case 7:
                        // No need to create opcode
                        fprintf(LF, "\t  %d\t%04X\t", pCurrent->lineNum * 5, pCurrent->LOC);
                        for (i = 0; i < tokenNum; i++)
                            fprintf(LF, "\t%s", token[i]);
                        fprintf(LF, "\n");
                        
                        pCurrent = pCurrent->link;
                        continue;
                }
            }
            

        }

        pCurrent = pCurrent->link;
    }

    fclose(LF); fclose(OF);
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

int getObjCode(char** token, int format, int type) {
    // format: operation code format / or size
    // type:    if 0, operation
    //          if 1, BYTE Const
    //          if 2, WORD Const
    unsigned char n, i, x, b, p, e;
    unsigned char r1, r2;
    int reg;
    int b1, b2, b3, b4;
    int disp, addr;
    int opCode, operand;
    int objCode = 0;
    int tokenNum, lp, target;
    for (tokenNum = 0; token[tokenNum]; tokenNum++) ;

    if (type == 0) {
        opCode = opcode(token[0], 2);

        switch (format) {
            case 1:
                printf("%02X", objCode);
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
                reg = (r1 << 2) + r2;

                printf("%04X\n", objCode);
                return objCode;
            case 3:
                // operand
                // get nixbpe
                if (token[1]) {
                    // n & i bits
                    if (token[1][0] == '#') {
                        // immediate
                        n = 0; i = 1;
                        token[1] = &(token[1][1]);
                    }
                    else if (token[1][0] == '@') {
                        // indirect
                        n = 1; i = 0;
                        token[1] = &(token[1][1]);
                    }
                    else {
                        // simple
                        n = 1; i = 1;
                    }

                    // x bit
                    x = 0;
                    if (token[2])
                        if (strcmp(token[2], "X") == 0)
                            x = 1;

                    // b & p bits
                    operand = strToHex(token[1], 0);
                    if (operand == -1) {
                        // find symbol
                        target = findSym(token[1]);
                        if (target == -1) {
                            printf("Error!!\n");
                            return 0;
                        }
                        disp = target - PC;
                        if ((signed int)disp >= (signed int)0xFFF && disp < 0x1000) {
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
                    }
                    else {
                        b = 0; p = 0;
                        disp = operand;
                    }
                    // e bit
                    e = 0;
                }
                else {
                    // no operand
                    n = 1; i = 1;
                    x = 0; b = 0; p = 0; e = 0;
                    disp = 0;
                }

                b1 = opCode + n * 2 + i;
                b2 = (x << 7) + (b << 6) + (p << 5) + (e << 4) + disp / (1<<8);
                b3 = disp % (1 << 8);
                objCode = (b1 << 16) + (b2 << 8) + b3;

                printf("%06X\n", objCode);
                return objCode;
            case 4:
                // get nixbpe
                if (token[1]) {
                    // operand exist
                    // n & i bits
                    if (token[1][0] == '#') {
                        // immdeditae
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

                    // x bit
                    x = 0;
                    if (token[2])
                        if (strcmp(token[2], "X") == 0)
                            x = 1;

                    // b & p bits
                    b = 0;
                    p = 0;

                    // e bits
                    e = 1;
                    
                    operand = strToHex(token[1], 0);
                    if (operand == -1) {
                        // find symbol
                        target = findSym(token[1]);
                        if (target == -1) {
                            printf("no Symbol Found!\n");
                            return 0;
                        }
                        addr = target;
                        if (target < 0 || target > 0xFFFFFF) return 0;
                    }
                    else return 0;

                    b1 = opCode + n * 2 + i;
                    b2 = (x << 7) + (b << 6) + (p << 5) + (e << 4) + addr / (1<<16);
                    b3 = addr % (1 << 16) - ((addr / (1 << 16)) << 4);
                    b4 = addr % (1 << 8);
                    objCode = (b1 << 24) + (b2 << 16) + (b3 <<8 ) + b4;
                }
                printf("%08X\n", objCode);
                break;
        }
    }
    else if (type == 1) {
        // BYTE CONST
        int size = 0, i;
        int tmp;
        char* strHex = (char*)malloc(3 * sizeof(char));
        if (token[1][0] == 'C') {
            for (i = 2; i < (int)strlen(token[1]) - 1; i++) ;
            size = i;

            for (i = 0; i < size; i++)
                objCode += ((int)token[1][i+2]) << (4 * (size - i - 1));
        }
        else if (token[1][0] == 'X') {
            for (i = 2; i < (int)strlen(token[1]) - 1; i++) ;
            size = (i % 2 == 0) ? i / 2 : i / 2 + 1;
            
            for (i = 0; i < size; i++) {
                strHex[0] = token[1][i+2];
                strHex[1] = token[1][i+3];
                strHex[2] = '\0';
                tmp = strToHex(strHex, 1);

                objCode += (tmp << (size - i - 1));

            }
        }
    }
    else if (type == 2) {
        // WORD CONST

        operand = strToHex(token[1], 1);
        b1 = operand / (1 << 16);
        b2 = operand / (1 << 8) - (b1 << 8);
        b3 = operand % (1 << 8);
        objCode = (b1 << 16) + (b2 << 8) + b3;
    }

    
    return objCode;
}

void printObjCode(int size, int objCode, FILE* fp) {
    int hex;
    for (int i = size-1; i >= 0; i--) {
        hex = (objCode / 1<<(4*i)) - (objCode / 1<<(4*(i+1)) * 1<<4);
        hex &= ONE_BYTE;
        fprintf(fp, "%02X", hex);
    }
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

    if (commaFlag) {
        /*
        // contain comma
        printf("\n");
        int tp;
        while ((*token)[cnt]) {
            tp = cnt;
            (*token)[++cnt] = strtok((*token)[tp], ",");
        }
        */
    }
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
