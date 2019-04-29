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

    // PASS 1
    ESTAB = (EShead*)malloc(objCnt * sizeof(EShead));
    char line[MAX_LINE_LEN];
    for (int CS = 0; CS < objCnt; CS++) {
        fgets(line, MAX_LINE_LEN, objFP[CS]);       // get first line

        if (line[0] != 'H') {
            // wrong obj file
            printf("Error occured in file [%s] - NO H RECORD\n", objFile[CS]);
            haltLinkingLoader(objFile, objFP, ESTAB);
            return 0;
        }
        if ( !HRecord(line, CS, objFile[CS]) ) {
            haltLinkingLoader(objFile, objFP, ESTAB);
            return 0;
        }

        flag = 0;
        while (1) {
            memset(line, '\0', sizeof(line));
            fgets(line, MAX_LINE_LEN, objFP[CS]);
            switch (line[0]) {
                case 'H':
                    printf("Error occured in file [%s] - Multiple H Records\n", objFile[CS]);
                    haltLinkingLoader(objFile, objFP, ESTAB);
                    return 0;
                case 'R': case 'T': case 'M': case '.':
                    break;
                case 'E':
                    flag = 1;
                    break;

                case 'D':
                    if ( !DRecord(line, CS, objFile[CS]) ) {
                        haltLinkingLoader(objFile, objFP, ESTAB);
                        return 0;
                    }
                    break;
                default:
                    printf("Error occured in file [%s] - Wrong format\n", objFile[CS]);
                    haltLinkingLoader(objFile, objFP, ESTAB);
                    return 0;
            }
            if (flag) break;
        }
    }

/*
    // PASS 2
    for (int CS = 0; CS < objCnt; CS++) {
        fseek(objFP[CS], 0, SEEK_SET);
        addRN("01", ESTAB[CS].CSaddr);

        while (1) {
            memset(line, '\0', sizeof(line));
            fgets(line, MAX_LINE_LEN, objFP[CS]);
            switch (line[0]) {
                case 'H': case 'D':
                    // skip for H record & D records
                    break;
                case 'R':        // REFER Record
                    if ( !RRecord(line, objCnt, objFile[CS]) )
                        return 0;
                    break;
                case 'T':       // TEXT Record
                    if ( !TRecord(line, ESTAB[CS], objFile[CS]) )
                        return 0;
                    break;
                case 'M':       // MODIFICATION Record
                    if ( !MRecord(line, ESTAB[CS], objFile[CS]) )
                        return 0;
                    break;
                case 'E':       // END Record

                default:
                    printf("Error occured in OBJ file\n");
                    haltLinkingLoader(objFile, objFP, ESTAB);
                    return 0;
            }
        }
        
        


        freeRN();
    }
    */






    loadMap(objCnt);


    haltLinkingLoader(objFile, objFP, ESTAB);
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

int HRecord(char* line, int currentCS, char* file) {
    // ACTION for H Record
    // save informations about Control Section
    // if success, return 1
    // else, return 0;
    if ( (int)strlen(line) < 19 ) {
        printf("Error occured in file [%s] - Wrong format in H Record\n", file);
        return 0;
    }

    int charPtr = 1;
    strncpy(ESTAB[currentCS].CSname, line + charPtr, 6);        // get Control Section Name
    charPtr += 6;

    char tmp[7];

    strncpy(tmp, line + charPtr, 6);            // get starting address of current Control Section
    charPtr += 6;
    int csAddr = strToHex(tmp, 0);
    if (csAddr == -1) {
        printf("Error occured in file [%s] - wrong Starting Address in H Record\n", file);
        return 0;
    }
    ESTAB[currentCS].CSaddr = csAddr + PROGADDR;
    for (int i = 0; i < currentCS; i++)
        ESTAB[currentCS].CSaddr += ESTAB[i].CSlength;

    memset(tmp, '\0', sizeof(tmp));
    strncpy(tmp, line + charPtr, 6);            // get length of obj file
    ESTAB[currentCS].CSlength = strToHex(tmp, 0);
    ESTAB[currentCS].link = NULL;

    if ( !validAddr(ESTAB[currentCS].CSaddr) ) {
        printf("Error occured in file [%s] - Invalid Address in H Record. Please set PROGADDR\n", file);
        return 0;
    }

    return 1;
}

int DRecord(char* line, int currentCS, char* file) {
    if ( (int)strlen(line) < 19 ) {
        printf("Error occured in file [%s] - Wrong format in D Record\n", file);
        return 0;
    }
    int charPtr = 1;
    char name[7], addr[7];

    while (charPtr <= (int)strlen(line) - 6 - 1) {
        memset(name, '\0', sizeof(name));
        memset(addr, '\0', sizeof(addr));

        strncpy(name, line + charPtr, 6);
        charPtr += 6;
        strncpy(addr, line + charPtr, 6);
        charPtr += 6;

        addES(&(ESTAB[currentCS]), name, addr);
    }
    return 1;
}

int RRecord(char* line, int objCnt, char* file) {
    // ACTION for R Record
    // build Reference Number list
    // if error occured, return 0
    // else, return 1
    if ( (int)strlen(line) < 4 ) {
        printf("Error occured in file [%s] - Wrong format in R Record\n", file);
        return 0;
    }
    int charPtr = 1;
    int refAddr;
    char referenceNum[3], referenceName[7];
    while (charPtr < (int)strlen(line) - 1) {
        memset(referenceNum, '\0', sizeof(referenceNum));
        memset(referenceName, '\0', sizeof(referenceName));
        strncpy(referenceNum, line + charPtr, 2);
        charPtr += 2;
        strncpy(referenceName, line + charPtr, 6);
        charPtr += 6;

        refAddr = (searchESTAB(referenceName, objCnt));
        if (refAddr == -1) {
            printf("Error occured in file [%s] - No External Refernece defined\n", file);
            return 0;
        }

        addRN(referenceNum, refAddr);
    }

    return 1;
}

int TRecord(char* line, EShead CShead, char* file) {
    // ACTION for T Record
    // Set Memory Value
    // if error occured, return 0
    // else, return 1
    if ( (int)strlen(line) < 11 ) {
        printf("Error occured in file [%s] - Wrong format in T Record\n", file);
        return 0;
    }
    int currentAddr, charPtr = 1;
    char strAddr[7], tmp[3];
    int tLen, memVal;

    currentAddr = CShead.CSaddr;
    strncpy(strAddr, line + charPtr, 6);       // start addr of T record
    charPtr += 6;
    currentAddr += strToHex(strAddr, 0);

    strncpy(tmp, line + charPtr, 2);       // length of T record
    tLen = strToHex(tmp, 0);
    charPtr += 2;

    while (charPtr < tLen) {
        memset(tmp, '\0', sizeof(tmp));
        strncpy(tmp, line + charPtr, 2);       // get one byte
        charPtr +=2;

        memVal = strToHex(tmp, 0);
        if ( !setMem(currentAddr + charPtr - 9, memVal) )
            return 0;
    }

    return 1;
}

int MRecord(char* line, EShead CShead, char* file) {
    // ACTION for M Record
    // Modify Memory value
    // if error occured, return 0
    // else, return 1
    if ( (int)strlen(line) < 12 ) {
        printf("Error occured in file [%s] - Wrong format in M Record\n", file);
        return 0;
    }
    int charPtr = 1;
    char _mAddr[7], tmp[3];
    int mAddr, mLen, mVal;

    strncpy(_mAddr, line + charPtr, 6);       // addr to modify
    charPtr += 6;
    mAddr = strToHex(_mAddr, 0);

    strncpy(tmp, line + charPtr, 2);       // length to modify
    charPtr += 2;
    mLen = strToHex(tmp, 0);

    memset(tmp, '\0', sizeof(tmp));
    strncpy(tmp, line + charPtr + 1, 2);       // Reference number
    mVal = searchRN(tmp);       // value to be either added or subtracted

    if (mVal == -1) {
        printf("Error occured in file [%s] - Cannot find reference number at %06X:  %s\n", file, mAddr, tmp);
        return 0;
    }

    int prevVal = 0;
    prevVal += (*(MEMORY + mAddr) << 16);
    prevVal += (*(MEMORY + mAddr + 1) << 8);
    prevVal += (*(MEMORY + mAddr + 2));

    mVal = (line[charPtr] == '+') ? prevVal + mVal : prevVal - mVal;

    setMem(CShead.CSaddr + mAddr, mVal >> 16);
    setMem(CShead.CSaddr + mAddr + 1, (mVal >> 8) % (2 << 8));
    setMem(CShead.CSaddr + mAddr + 2, mVal % (2 << 8));

    return 1;
}

int setMem(int addr, int val) {
    // set Memory to value
    // simailar with EDIT func in MEMORY.c
    // parameter as int
    // if success, return 1
    // else, return 0
    if (val == -1) {
        // in case OBJ FILE has error
        printf("Error occured in obj file\n");
        return 0;
    }

    if ( !validAddr(addr) ) {
        printf("Segmentation falut!\n");
        return 0;
    }

    unsigned char value = val & ONE_BYTE;
    memcpy(MEMORY + addr, &value, 1);

    return 1;
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

void addRN(char* ref, int addr) {
    int refNum = strToDecimal(ref);
    referNode* pNew = (referNode*)malloc(sizeof(referNode));
    pNew->ref = refNum; pNew->addr = addr;
    pNew->link = NULL;

    pNew->link = referHead;
    referHead = pNew;
}

int searchESTAB(char* name, int objCnt) {
    // search ESTAB
    // parameter: External Symbol name
    // return LOC
    ESnode* ptmp;
    for (int i = 0; i < objCnt; i++) {
        ptmp = ESTAB[i].link;
        while (ptmp) {
            if (strcmp(ptmp->name, name) == 0) return ptmp->LOC;
            ptmp = ptmp->link;
        }
    }
    
    return -1;
}

int searchRN(char* ref) {
    // saerch Reference number
    // return address if success
    // else, return -1
    referNode* ptmp = referHead;
    int refNum = strToDecimal(ref);
    while (ptmp) {
        if (ptmp->ref == refNum)
            return ptmp->addr;
    }

    return -1;
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

void haltLinkingLoader(char** objFile, FILE** objFP, EShead* ESTAB) {
    free(objFile); free(objFP);
    free(ESTAB);
}

void freeRN() {
    referNode* pFree;
    while (referHead) {
        pFree = referHead;
        referHead = referHead->link;
        free(pFree);
    }
}
