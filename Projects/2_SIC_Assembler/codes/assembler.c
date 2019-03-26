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
    if (!SYMTAB) {
        // no symbol table
        printf("assemble file first!\n");
        return 0;
    }


    return 1;
}

int pass1(FILE* fp) {
    // Assign addresses to all statments in the program
    // Save the values (addrs) assigned to all labels for use in Pass 2
    // Perform some processing of assembler assignment, such as determining the
    // length of data areas defined by BYTE, RESW, etc
    
    // make symtab, assign LOC
    int LOCCTR, startingAddr;
    char* line = (char*)malloc(30 * sizeof(char));
    char symbol[10], opcode[10], target[10];
    fseek(fp, 0, SEEK_SET);     // move to the first
    fgets(line, 30, fp);

    if ( !isStr(fgets(line, 30, fp)) ) {
            // file with no text
            printf("No content in the file\n");
            return 0;
    }
    
    line = removeSpace(line);
    while (line[0] == '.') {
        // skip comment lines
        if ( !isStr(fgets(line, 30, fp)) ) {
            return 1;
        }
        line = removeSpace(line);
    }

    // First line
    char** token = (char**)malloc(5 * sizeof(char));
    tokenizeAsmFile(&token, line);


    if (strcmp(opcode, "START") == 0) {
        LOCCTR = strToHex(target);

    }
    else LOCCTR = 0;

    line = removeSpace(fgets(line, 30, fp));
    sscanf(line, "%s %s %s", symbol, opcode, target);


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
    int cnt = 0;
    char* tmp;
    
    *token[cnt] = strtok(input, " \t");
    while (cnt < 5 && *token[cnt]) {
        tmp = strtok(NULL, "\0");
        if (!tmp) break;
        *token[++cnt] = strtok(removeSpace(tmp), " \t");
        *token[cnt] = removeSpace(strtok(tmp, " \t"));
    }
    



    return 1;
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
