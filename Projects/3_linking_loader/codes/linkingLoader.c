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
    char tmp1[7], tmp2[7];
    int charNum;
    int refAddr;
    for (int CS = 0; CS < objCnt; CS++) {
        fgets(line, MAX_LINE_LEN, objFP[CS]);

        if (line[0] != 'H') {
            // wrong obj file
            printf("Error occured in [%s] - NO H RECORD\n", objFile[CS]);
            haltLinkingLoader(objFile, objFP, ESTAB);
            return 0;
        }
        strncpy(ESTAB[CS].CSname, line + 1, 6);
        strncpy(tmp2, line + 7, 6);
        ESTAB[CS].CSaddr = strToHex(tmp2, 0) + PROGADDR;
        for (i = 0; i < CS; i++)
            ESTAB[CS].CSaddr += ESTAB[i].CSlength;
        strncpy(tmp2, line + 13, 6);
        ESTAB[CS].CSlength = strToHex(tmp2, 0);
        ESTAB[CS].link = NULL;

        if ( !validAddr(ESTAB[CS].CSaddr) ) {
            printf("Invalid Address. Please set PROGADDR\n");
            return 0;
        }

        memset(line, '\0', sizeof(line));
        fgets(line, MAX_LINE_LEN, objFP[CS]);
        while (line[0] == 'D') {
            charNum = 1;

            while (charNum < (int)strlen(line) - 1) {
                strncpy(tmp1, line + charNum, 6);
                charNum += 6;
                strncpy(tmp2, line + charNum, 6);
                tmp1[6] = tmp2[6] = '\0';
                charNum += 6;

                addES(&(ESTAB[CS]), tmp1, tmp2);
            }

            memset(line, '\0', sizeof(line));
            fgets(line, MAX_LINE_LEN, objFP[CS]);
        }
    }


    // PASS 2
    int currentAddr = 0;
    int tLen, mLen;
    int memVal, mVal, prevVal;
    int mAddr;
    for (int CS = 0; CS < objCnt; CS++) {
        fseek(objFP[CS], 0, SEEK_SET);
        addRN("01", ESTAB[CS].CSaddr);

        // skip for H record & D record
        do {
            fgets(line, MAX_LINE_LEN, objFP[CS]);
        } while (line[0] == 'H' || line[0] == 'D');

        do {
            switch (line[0]) {
                case 'R':
                    if ( !RRecord(line, objCnt) )
                        return 0;
                    break;
                case 'T':
                    if ( !TRecord(line, ESTAB[CS]) )
                        return 0;
                    break;
                case 'M':

                case 'E':

                default:
                    printf("Error occured in OBJ file\n");
                    haltLinkingLoader(objFile, objFP, ESTAB);
                    return 0;
            }

            memset(line, '\0', sizeof(line));
            fgets(line, MAX_LINE_LEN, objFP[CS]);
        } while (1);
        

        if (line[0] == 'M') {
            charNum = 1;
            strncpy(tmp1, line + charNum, 6);       // addr to modify
            charNum += 6;
            mAddr = strToHex(tmp1, 0);
            memset(tmp1, '\0', sizeof(tmp1));

            strncpy(tmp1, line + charNum, 2);       // length to modify
            charNum += 2;
            mLen = strToHex(tmp1, 0);

            strncpy(tmp2, line + charNum + 1, 2);       // Reference number

            mVal = searchRN(tmp2);
            if (mVal == -1) {
                printf("Cannot find reference number at %06X - %s\n", mAddr, tmp2);
                return 0;
            }

            prevVal = 0;
            prevVal += (*(MEMORY + mAddr) << 16);
            prevVal += (*(MEMORY + mAddr + 1) << 8);
            prevVal += (*(MEMORY + mAddr + 2));

            mVal = (line[charNum] == '+') ? prevVal + mVal : prevVal - mVal;

            setMem(ESTAB[CS].CSaddr + mAddr, mVal >> 16);
            setMem(ESTAB[CS].CSaddr + mAddr + 1, (mVal >> 8) % (2 << 8));
            setMem(ESTAB[CS].CSaddr + mAddr + 2, mVal % (2 << 8));
        }


        


        freeRN();
    }
    






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

int RRecord(char* line, int objCnt) {
    // ACTION for R Record
    // build Reference Number list
    // if error occured, return 0
    // else, return 1
    int charNum = 1;
    int refAddr;
    char referenceNum[3], referenceName[7];
    while (charNum < (int)strlen(line) - 1) {
        memset(referenceNum, '\0', sizeof(referenceNum));
        memset(referenceName, '\0', sizeof(referenceName));
        strncpy(referenceNum, line + charNum, 2);
        charNum += 2;
        strncpy(referenceName, line + charNum, 6);
        charNum += 6;

        refAddr = (searchESTAB(referenceName, objCnt));
        if (refAddr == -1) {
            printf("No External Refernece defined\n");
            return 0;
        }

        addRN(referenceNum, refAddr);
    }

    return 1;
}

int TRecord(char* line, EShead CShead) {
    // ACTION for T Record

    int currentAddr, charNum;
    char strAddr[7], tmp[3];
    int tLen, memVal;

    memset(strAddr, '\0', sizeof(strAddr));
    memset(tmp, '\0', sizeof(tmp));

    currentAddr = CShead.CSaddr;
    charNum = 1;
    strncpy(strAddr, line + charNum, 6);       // start addr of T record
    charNum += 6;
    currentAddr += strToHex(strAddr, 0);

    strncpy(tmp, line + charNum, 2);       // length of T record
    tLen = strToHex(tmp, 0);
    charNum += 2;

    while (charNum < tLen) {
        memset(tmp, '\0', sizeof(tmp));
        strncpy(tmp, line + charNum, 2);       // get one byte
        charNum +=2;

        memVal = strToHex(tmp, 0);
        if ( !setMem(currentAddr + charNum - 9, memVal) )
            return 0;
    }

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
