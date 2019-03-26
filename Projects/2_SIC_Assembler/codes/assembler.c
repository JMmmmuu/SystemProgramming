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
    int LOCCTR, startingAddr;
    char line[30];
    fseek(fp, 0, SEEK_SET);     // move to the first
    fgets(line, 30, fp);
    while (line[0] == '.') {
        fgets(line, 30, fp);
    }

    char symbol[10], opcode[10], target[10];
    sscanf(line, "%s %s %s", symbol, opcode, target);


    return 1;
}

int pass2(FILE* fp) {

    fseek(fp, 0, SEEK_SET);     // move to the first



    return 1;
}
