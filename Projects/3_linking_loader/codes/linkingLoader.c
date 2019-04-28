/*************************************************
 ************** System Programming ***************
 **************** linkingLoader.c  ***************
 ******************** Yuseok *********************
 ******************* ~ 190506 ********************
 *************************************************/
#include "20171690.h"
#include "linkingLoader.h"

int progaddr(char* addr) {
    PROGADDR = strToHex(addr, 1);
    if (PROGADDR == -1) {
        // not a corect hexa decimal num
        PROGADDR = 0;
        return 0;
    }
    if ( !validAddr(PROGADDR) ) {
        // out of valid address range
        PROGADDR = 0;
        printf("Invalid Address: out of range\n");
        return 0;
    }

    printf("PROGADDR SET\n");
    return 1;
}

int loader(char* param) {
    // Link & Load obj files
    // if fail, return 0.
    // else, 1
    char** objFile = (char**)malloc(3 * sizeof(char*));
    int objNum = 0;
    objFile[objNum++] = strtok(param, " \t");
    do {
        objFile[objNum] = strtok(NULL, " \t");
    } while (objFile[objNum++]);
    objNum--;

    if (objNum > 3) {
        // consider upto 3 files
        printf("Too many .obj file. Available only up to 3, you have %d\n", objNum);
        return 0;
    }

    int i, flag = 0;
    FILE** objFP = (FILE**)malloc(objNum * sizeof(FILE*));
    
    for (i = 0; i < objNum; i++) {
        // wrong file name
        /** if ( !isObjFile(objFile[i]) ) { */
        /**     printf("invaild file name - %s\n", objFile[i]); */
        /**     free(objFile); */
        /**     free(objFP); */
        /**     return 0; */
        /** } */

        objFP[i] = fopen(objFile[i], "r");
        if ( !objFP[i] ) {
            printf("FILE NOT FOUND - %s\n", objFile[i]);
            flag = 1;
        }
    }
    if (flag) {
        free(objFile);
        free(objFP);
        return 0;
    }






    return 1;
}


int isObjFile(char* file) {
    // chech the file name
    // it must be "*.obj" form
    int ptr;
    if (!file) return 0;
    ptr = strlen(file) - 4;
    if (ptr <= 0) return 0;

    if ( strcmp(file + ptr, ".obj") != 0 ) return 0;

    return 1;
}
