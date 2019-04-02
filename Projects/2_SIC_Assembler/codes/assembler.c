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
        printf("output file : [%s], [%s]\n", nameToListing(filename), nameToObj(filename));
        /** printSymbol(); */
        /** printNums(); */

        if (!pass2(asmFP, filename)) {
            printf("Program halted in Pass 2\n");
            return 0;
        }
    }
    else {
        printf("program halted in Pass 1\n");
        return 0;
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

    for (i = 0; i < tokenNum; i++) {
        if (isDirective(token[i]) == 1) {
            // if token[i] == "START"
            if (i+1 >= tokenNum || tokenNum == 2) {
                printf("Error occured at [%d] line: No name or starting address\n", lineNum);
                return 0;
            }
            if ( (startingAddr = strToHex(token[i+1], 0)) == -1 ) {
                printf("Error occured at [%d] line: Wrong Starting Address\n", lineNum);
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
                    printf("Error occured at [%d] line: use label already declared - %s\n", lineNum, token[0]);
                    return 0;
                }
            }
            if (instructionSize == 0)
                pLast = addNum(lineNum, -1, pLast, 3);
            else 
                pLast = addNum(lineNum, LOCCTR, pLast, 0);
        }

        LOCCTR += instructionSize;

        memset(line, '\0', (int)sizeof(line));
        for (i = 0; i < MAX_TOKEN_NUM; i++) token[i] = NULL;
        fgets(line, MAX_ASM_LINE, fp);
        lineNum++;

    }

    free(line);
    free(token);

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
    int i, tokenNum, startingAddr, endAddr, size, tmp;
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

    while (pCurrent->skip_flag) {
        // while comment line or blank line
        fgets(line, MAX_ASM_LINE, fp);
        fprintf(LF, "\t  %s\n", line);
        pCurrent = pCurrent->link;
        if (!pCurrent) {
            printf("Warning! - no content except comment / blank lines\n");
            return 1;
        }
    }
    if (pCurrent->s_flag) {
        // START directive
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
            fprintf(LF, "\t  %d\t\t\t\t%s\n", pCurrent->lineNum * 5, removeSpace(line));
            pCurrent = pCurrent->link;
            continue;
        }
        if (pCurrent->e_flag) {
            // END directive
            if (tRHead) dequeue(OF);
            fprintf(OF, "E%06X\n", startingAddr);
            fprintf(LF, "\t  %d\t\t\t\t%s\n", pCurrent->lineNum * 5, removeSpace(line));
            break;
        }

        // TEXT RECORD
        line = toUpperCase(line);
        tokenNum = tokenizeAsmFile(&token, line);
        int format, directiveNum;
        int objCode;

        // set Program Counter
        numNode* pMove = pCurrent->link;
        while (pMove->link) {
            if (pMove->LOC == -1) {
                // comment line or blank line
                pMove = pMove->link;
                continue;
            }
            break;
        }
        PC = pMove->LOC;

        if (line[0] == ' ' || line[0] == '\t') {
            // no label
            format = opcode(token[0], 3);
            if ( (token[0])[0] == '+') {
                format = 4;
                token[0] += 1;
            }
            if ( format ) {
                // There exist matching operation
                objCode = getObjCode(token, format, 0, pCurrent);
                if (objCode == -1) return 0;
                if (opcode(token[0], 4) == 0x68) {
                    // Load operand value in the B register
                    if (token[1][0] == '#' || token[1][0] == '@')
                        B = findSym(token[1]+1);
                    else if ( (tmp = strToHex(token[1], 0)) != -1)
                        B = tmp;

                    if (B == -1) {
                        if ( (tmp = strToHex(token[1]+1, 0)) != -1)
                            B = tmp;
                        else {
                            printf("Error occured at [%d] line: incorrect use of undeclared label - %s\n", pCurrent->lineNum, token[1]+1);
                            return 0;
                        }
                    }
                }

                fprintf(LF, "\t  %d\t%04X\t\t",  pCurrent->lineNum * 5, pCurrent->LOC);
                if (format == 4) token[0] -= 1;
                for (i = 0; i < tokenNum; i++)
                    fprintf(LF, "\t%s", token[i]);
                if (tokenNum == 1) fprintf(LF, "\t\t\t");
                else {
                    if ((int)strlen(token[1]) >= 8) fprintf(LF, "\t");
                    else fprintf(LF, "\t\t");
                }
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
                objCode = getObjCode(&(token[1]), format, 0, pCurrent);
                if (objCode == -1) return 0;
                if (opcode(token[1], 4) == 0x68) {
                    // LOAD operand value to B register
                    if (token[2][0] == '#' || token[2][0] == '@')
                        B = findSym(token[2]+1);
                    else if ( (tmp = strToHex(token[2], 0)) != -1)
                        B = tmp;

                    if (B == -1) {
                        if ( (tmp = strToHex(token[2]+1, 0)) != -1)
                            B = tmp;
                        else {
                            printf("Error occured at [%d] line: incorrect use of undeclared label - %s\n", pCurrent->lineNum, token[2]+1);
                            return 0;
                        }

                    }
                }

                fprintf(LF, "\t  %d\t%04X\t", pCurrent->lineNum * 5, pCurrent->LOC);
                if (format == 4) token[1] -= 1;
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
                        objCode = getObjCode(&(token[1]), 5, 1, pCurrent);
                        if (objCode == -1) return 0;
                        fprintf(LF, "\t  %d\t%04X\t", pCurrent->lineNum * 5, pCurrent->LOC);
                        for (i = 0; i < tokenNum; i++)
                            fprintf(LF, "\t%s", token[i]);
                        size = (int)strlen(token[2]) - 3;
                        size = (token[2][0] == 'X') ? size / 2 : size;
                        fprintf(LF,  "\t\t");
                        printObjCode(size, objCode, LF);
                        fprintf(LF, "\n");
                        enqueue(objCode, size, pCurrent->LOC, OF);

                        break;
                    case 4:     // WORD
                        objCode = getObjCode(&(token[1]), 5, 2, pCurrent);
                        if (objCode == -1) return 0;
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
    free(line);
    free(token);
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

int getObjCode(char** token, int format, int type, numNode* pCurrent) {
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
    int lc;
    int tokenNum, target;
    for (tokenNum = 0; token[tokenNum]; tokenNum++) ;
    char* operandStr[5];
    char* tmp;
    char* sym;
    for (lc = 0; lc < 5; lc++) operandStr[lc] = NULL;

    if (type == 0) {
        opCode = opcode(token[0], 4);

        int commaFlag = isComma(token[1]);
        if (commaFlag) {
            tmp = (char*)malloc(50 * sizeof(char));
            strcpy(tmp, token[1]);
            lc = 0;
            operandStr[lc] = strtok(tmp, ",");
            while (operandStr[lc++] && lc <= 5)
                operandStr[lc] = strtok(NULL, ",");
        }

        switch (format) {
            case 1:
                return objCode;
            case 2:
                // get register code
                if (commaFlag) {
                    r1 = getRegNum(operandStr[0]);
                    r2 = getRegNum(operandStr[1]);
                    if (r1 == 0xFF) r1 = strToDecimal(operandStr[0]) - 1;
                    if (r2 == 0xFF) r2 = strToDecimal(operandStr[1]) - 1;
                }
                else {
                    r1 = getRegNum(token[1]);
                    if (r1 == 0xFF) r1 = strToDecimal(token[1]);
                    r2 = 0x00;
                }
                reg = (r1 << 4) + r2;
                objCode = (opCode * (1 << 8)) + reg;

                return objCode;
            case 3:
                // operand
                // get nixbpe
                
                if (token[1]) {
                    sym = token[1];
                    // x bit
                    x = 0;
                    if (commaFlag) {
                        sym = operandStr[0];
                        if (strcmp(operandStr[1], "X") == 0)
                            x = 1;
                    }

                    // n & i bits
                    if (token[1][0] == '#') {
                        // immediate
                        n = 0; i = 1;
                        sym += 1;
                    }
                    else if (token[1][0] == '@') {
                        // indirect
                        n = 1; i = 0;
                        sym += 1;
                    }
                    else {
                        // simple
                        n = 1; i = 1;
                    }

                    // b & p bits
                    operand = strToHex(sym, 0);
                    if (operand == -1) {
                        // find symbol
                        target = findSym(sym);
                        if (target == -1) {
                            printf("Error occured at [%d] line: incorrect use of undeclared label - %s\n", pCurrent->lineNum, sym);
                            return -1;
                        }

                        disp = target - PC;
                        if (disp >= -0xFFF && disp < 0x1000) {
                            b = 0; p = 1;
                        }
                        else {
                            disp = target - B;
                            if (disp >= 0 && disp < 0x1000) {
                                b = 1; p = 0;
                            }
                            else {
                                format = 4;
                                printf("Error at [%d] line: format 4 with no + sign\n", pCurrent->lineNum);
                                return -1;
                            }
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

                if (disp < 0)
                    disp = disp & 0x0FFF;
                b1 = opCode + n * 2 + i;
                b2 = (x << 7) + (b << 6) + (p << 5) + (e << 4) + (disp >> 8);
                b3 = disp % (1 << 8);
                objCode = (b1 << 16) + (b2 << 8) + b3;

                return objCode;
            case 4:
                // get nixbpe
                if (token[1]) {
                    // operand exist
                    sym = token[1];

                    // x bit
                    x = 0;
                    if (commaFlag) {
                        sym = operandStr[0];
                        if (strcmp(operandStr[1], "X") == 0)
                            x = 1;
                    }

                    // n & i bits
                    if (token[1][0] == '#') {
                        // immdeditae
                        n = 0; i = 1;
                        sym += 1;
                    }
                    else if (token[1][0] == '@') {
                        // indirect
                        n = 1; i = 0;
                        sym += 1;
                    }
                    else {
                        // simple
                        n = 1; i = 1;
                    }

                    // b & p bits
                    b = 0;
                    p = 0;

                    // e bits
                    e = 1;
                    
                    operand = strToDecimal(sym);
                    if (operand == -1) {
                        // find symbol
                        target = findSym(sym);
                        if (target == -1) {
                            printf("Error occured at [%d] line: incorrect use of undeclared label - %s\n", pCurrent->lineNum, sym);
                            return -1;
                        }
                        addr = target;
                        if (target < 0 || target > 0xFFFFFF) {
                            printf("incorrect addr value\n");
                            return -1;
                        }
                    }
                    else
                        addr = operand;

                    b1 = opCode + n * 2 + i;
                    b2 = (x << 7) + (b << 6) + (p << 5) + (e << 4) + (addr >> 16);
                    b3 = (addr >> 8) - ((addr / (1 << 16)) << 8);
                    b4 = addr % (1 << 8);
                    objCode = (b1 << 24) + (b2 << 16) + (b3 << 8) + b4;
                }
                break;
        }
    }
    else if (type == 1) {
        // BYTE CONST
        int size = 0;
        int hex;
        char* strHex = (char*)malloc(3 * sizeof(char));
        if (token[1][0] == 'C') {
            size = (int)strlen(token[1]) - 3;

            for (lc = 0; lc < size; lc++)
                objCode += (((int)token[1][lc+2]) << (8 * (size - lc - 1)));
        }
        else if (token[1][0] == 'X') {
            size = ((int)strlen(token[1]) - 3) / 2;
            
            for (lc = 0; lc < size; lc++) {
                strHex[0] = token[1][lc+2];
                strHex[1] = token[1][lc+3];
                strHex[2] = '\0';
                hex = strToHex(strHex, 1);

                objCode += (hex << (8 * (size - lc - 1)));

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
    for (int i = size-1; i >= 1; i--) {
        hex = (objCode >> (8 * i)) - ((objCode >> (8 * i)) << (8 * i));
        hex &= ONE_BYTE;
        fprintf(fp, "%02X", hex);
    }
    hex = objCode % (1 << 8);
    fprintf(fp, "%02X", hex);
}

int tokenizeAsmFile(char*** token, char* input) {
    // tokenize all the possible options into token
    // return number of strings
    int cnt = 0;
    char* tmp;
    removeSpaceAroundComma(input);
    input = removeSpace(input);

    (*token)[cnt] = strtok(input, " \t");
    while (cnt < MAX_TOKEN_NUM && (*token)[cnt]) {
        tmp = strtok(NULL, "\0");
        if (!tmp) break;
        (*token)[++cnt] = strtok(removeSpace(tmp), " \t");
    }
    cnt++;

/**     printf("\t  "); */
    /** for (int i = 0; i < cnt; i++) */
    /**     printf("\t%s", (*token)[i]); */
    /** printf("\n"); */

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

    return isComma;
}
