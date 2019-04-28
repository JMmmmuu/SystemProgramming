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
    int objCnt = 0;
    objFile[objCnt++] = strtok(param, " \t");
    do {
        objFile[objCnt] = strtok(NULL, " \t");
    } while (objFile[objCnt++]);
    objCnt--;

    if (objCnt > 3) {
        // consider upto 3 files
        printf("Too many .obj file. Available only up to 3, you have %d\n", objCnt);
        return 0;
    }

    int i, flag = 0;
    FILE** objFP = (FILE**)malloc(objCnt * sizeof(FILE*));
    
    for (i = 0; i < objCnt; i++) {
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
        free(objFile); free(objFP);
        return 0;
    }


    /**************************************************
     ***************** LINKING & LOADING **************
     **************************************************/
    ESTAB = (EShead*)malloc(objCnt * sizeof(EShead));
    char* tmp = (char*)malloc(10 * sizeof(char));
    char line[MAX_LINE_LEN];
    char name[7], loc[7];
    int charNum;
    for (int CS = 0; CS < objCnt; CS++) {
        fgets(line, MAX_LINE_LEN, objFP[CS]);
        if (line[0] != 'H') {
            // wrong obj file
            printf("Error occured in [%s] - NO H RECORD\n", objFile[CS]);
            haltLinkingLoader(objFile, objFP, ESTAB, tmp);
            return 0;
        }
        strncpy(ESTAB[CS].CSname, line + 1, 6);
        strncpy(loc, line + 7, 6);
        ESTAB[CS].CSaddr = strToHex(loc, 0) + PROGADDR;
        for (i = 0; i < CS; i++)
            ESTAB[CS].CSaddr += ESTAB[i].CSlength;
        strncpy(loc, line + 13, 6);
        ESTAB[CS].CSlength = strToHex(loc, 0);
        ESTAB[CS].link = NULL;

        if ( !validAddr(ESTAB[CS].CSaddr) ) {
            printf("Invalid Address. Please set PROGADDR\n");
            return 0;
        }


        
        /**  */
        /** if (fgetc(objFP[CS]) != 'H') { */
        /**     // wrong obj file */
        /**     printf("Error occured in [%s] - NO H RECORD\n", objFile[CS]); */
        /**     haltLinkingLoader(objFile, objFP, ESTAB, tmp); */
        /**     return 0; */
        /** } */
        /** // Get information from H Record */
        /** fgets(ESTAB[CS].CSname, 6, objFP[CS]); */
        /** fgets(tmp, 6, objFP[CS]);  */
        /** printf("%s\n", tmp); */
        /** ESTAB[CS].CSaddr = strToHex(tmp, 0) + PROGADDR; */
        /** for (i = 0; i < CS; i++) */
        /**     ESTAB[CS].CSaddr += ESTAB[i].CSlength; */
        /** fgets(tmp, 6, objFP[CS]); */
        /** ESTAB[CS].CSlength = strToHex(tmp, 0); */
        /** ESTAB[CS].link = NULL; */
        /**  */
        /** printf("%s %d %d\n", ESTAB[CS].CSname, ESTAB[CS].CSaddr, ESTAB[CS].CSlength); */
        /** if ( !validAddr(ESTAB[CS].CSaddr) ) { */
        /**     // invalid start address */
        /**     printf("Invalid Address. Please set PROGADDR.\n"); */
        /**     haltLinkingLoader(objFile, objFP, ESTAB, tmp); */
        /**     return 0; */
        /** } */
        /**  */

        memset(line, '\0', sizeof(line));
        fgets(line, MAX_LINE_LEN, objFP[CS]);
        while (line[0] == 'D') {
            charNum = 1;

            while (charNum < (int)strlen(line) - 1) {
                strncpy(name, line + charNum, 6);
                charNum += 6;
                strncpy(loc, line + charNum, 6);
                name[6] = loc[6] = '\0';
                charNum += 6;

                addES(&(ESTAB[CS]), name, loc);
            }

            memset(line, '\0', sizeof(line));
            fgets(line, MAX_LINE_LEN, objFP[CS]);
        }
        





    }


    //printES(objCnt);
    loadMap(objCnt);


    haltLinkingLoader(objFile, objFP, ESTAB, tmp);
    return 1;
}

void loadMap(int objCnt) {
    int len = 0;
    printf("\t\t control\tsymbol\t\taddress\t\tlength\n");
    printf("\t\t section\tname\n");
    printf("\t\t ---------------------------------------------------\n");

    for (int i = 0; i < objCnt; i++) {
        printf("\t\t %s\t\t\t\t%04X\t\t%04X\n", ESTAB[i].CSname, ESTAB[i].CSaddr, ESTAB[i].CSlength);
        len += ESTAB[i].CSlength;
        ESnode* ptmp = ESTAB[i].link;
        for ( ; ptmp; ptmp = ptmp->link) {
            printf("\t\t \t\t%s\t\t%04X\n", ptmp->name, ptmp->LOC);
        }
    }

    printf("\t\t ---------------------------------------------------\n");
    printf("\t\t \t\t\t\ttotal length\t%04X\n", len);
}

void addES(EShead* ES, char* name, char* loc) {
    ESnode* pNew = (ESnode*)malloc(sizeof(ESnode));
    strcpy(pNew->name, removeSpace(name));
    pNew->LOC = strToHex(loc, 0) + ES->CSaddr;
    pNew->link = NULL;

    //printf("%s %X\n", pNew->name, pNew->LOC);

    if ( ES->link == NULL ) {
        ES->link = pNew;
        return ;
    }
    ESnode* ptmp;
    for (ptmp = ES->link; ptmp->link; ptmp = ptmp->link) ;
    ptmp->link = pNew;
}

void printES(int objCnt) {
    for (int i = 0; i < objCnt; i++) {
        printf("[%s %06X %06X] ", ESTAB[i].CSname, ESTAB[i].CSaddr, ESTAB[i].CSlength);
        ESnode* ptmp = ESTAB[i].link;
        for ( ; ptmp; ptmp = ptmp->link) {
            printf("- [%s, %06X] ", ptmp->name, ptmp->LOC);
        }
        printf("\n");
    }

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

void haltLinkingLoader(char** objFile, FILE** objFP, EShead* ESTAB, char* tmp) {
    free(objFile); free(objFP);
    free(ESTAB); free(tmp);
}
