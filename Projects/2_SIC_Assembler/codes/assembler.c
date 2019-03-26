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


