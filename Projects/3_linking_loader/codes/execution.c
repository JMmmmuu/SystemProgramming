/*************************************************
 ************** System Programming ***************
 ***************** execution.c *******************
 ******************** Yuseok *********************
 ******************* ~ 190506 *********************
 *************************************************/
#include "20171690.h"
#include "execution.h"
#include "linkingLoader.h"


int executeProg() {
    // execute last linked & loaded program
    // its starting addr & len is stored in EXEC_ADDR & EXEC_LEN
    // if execute successfully, return 1
    // else, return 0

    if (EXEC_LEN == -1) {
        printf("Please load the program first\n");
        return 0;
    }

    /** A = 0; X = 0; L = 0; SW = 0; B = 0; S = 0; T = 0; F = 0; CC = 0; */
    PC = EXEC_ADDR;
    int opcode;
    unsigned char flags, reg, ni;//, x, b, p, e;
    int format;
    int target;

    newTR* addr_of_new_TRecord = TRHead;

    int cnt = 0;
    int MAX_CNT = (PROG_END - PROG_START) * 1000;
    int endFlag = 0;

    if (EXEC_LEN + EXEC_ADDR >= 0x100000) {
        printf("Segmentation Fault!\n");
        return 0;
    }
    /** while (PC < EXEC_ADDR + EXEC_LEN && PC >= EXEC_ADDR) { */
    if (!continuing) {
        L = PROG_END;
    }
    while (PC >= PROG_START && PC < PROG_END) {
        if ( !validAddr(PC + 2) ) {
            printf("Segmentation Fault!\n - %06X\n", PC);
            return 0;
        }
        opcode = *(MEMORY + PC);
        ni = opcode & (unsigned char)0x03;
        opcode &= (unsigned char)0xFC;

        format = searchWithOpcode(opcode);
        int byteSize;
        if (addr_of_new_TRecord) {
            byteSize = addr_of_new_TRecord->addr - PC;
            if (byteSize == 0)
                addr_of_new_TRecord = addr_of_new_TRecord->link;
        }

        if ( format == 0 || (format == 3 && ni == 0) ) {
            // WORD or BYTE CONST
            // no change of REG or MEM
            if (addr_of_new_TRecord) {
                byteSize = addr_of_new_TRecord->addr - PC;
                if (byteSize >= 1 && byteSize <= 4) {
                    /** printf("%06X %02X\n", addr_of_new_TRecord->addr, byteSize); */
                    PC += byteSize;
                    addr_of_new_TRecord = addr_of_new_TRecord->link;
                }
                else PC += 3;
            }
            else PC += 3;
            /** printf("\n"); */
        }
        else {
            // if opcode exists
            /** printf("[PC %06X], ", PC); */
            PC++;
            switch (format) {
                case 1:
                    opAct(opcode, format, 0, 0);
                    /** printf("\n"); */
                    break;
                case 2:
                    reg = *(MEMORY + PC++);
                    opAct(opcode, format, reg, 0);
                    /** printf("\n"); */
                    break;
                case 3:
                    if (ni == 0) {
                        PC += 2;
                        /** printf("\n"); */
                        if ( ++cnt == MAX_CNT) { 
                            printf("Program Halted at PC: [%05X] - please set progaddr again, or correct input file\n", PC);
                            return 0;
                        }
                        break;
                    }
                    flags = *(MEMORY + PC++);
                    target = (flags & (unsigned char)0x0F) << 8;
                    flags = (flags >> 4) + (ni << 4);

                    if (flags % 2) {
                        // if e flag set
                        format = 4;
                        target = (target << 8);
                    }
                    for (int i = (format == 3) ? 0 : 1; i >= 0; i--) {
                        if ( !validAddr(PC) ) {
                            printf("Segmentation Fault! - %06X\n", PC);
                            return 0;
                        }
                        target += (*(MEMORY + PC++) << (i * 8));
                    }

                    /** printf("\t[opcode %02X], [format %d], [flags %01X], [target %05X]\n", opcode, format, flags, target); */
                    if ( !opAct(opcode, format, target, flags) )
                        return 0;

                    break;
            }
            /** printReg(); */
        }

        if ( ++cnt == MAX_CNT) {
            printf("Program Halted at PC: [%05X] - please set progaddr again, or correct input file\n", PC);
            return 0;
        }
        if (BPHead) {
            //if (nextBP->bp == PC) {
            if (searchBP(PC)) {
                printReg();
                endFlag = 1;
                printf("\t     Stop at checkpoint [%05X]\n", PC);
                EXEC_LEN -= (PC - EXEC_ADDR);
                EXEC_ADDR = PC;
                continuing = 1;
                break;
            }
        }
    }
    if (endFlag == 0) {
        printReg();
        continuing = 0;
    }

    return 1;
}

void printReg() {
    printf("\t\tA : %06X X : %06X\n", A, X);
    printf("\t\tL : %06X PC: %06X\n", L, PC);
    printf("\t\tB : %06X S : %06X\n", B, S);
    printf("\t\tT : %06X\n", T);
}

int setBP(char* addr) {
    // set BreakPoint
    // if success, return 1
    // else, return 0
    int bpAddr = strToHex(addr, 0);
    if (bpAddr == -1 || !validAddr(bpAddr)) {
        // wrong parameter
        printf("\t    invalid break point\n");
        return 0;
    }

    BPNode* pNew = (BPNode*)malloc(sizeof(BPNode));
    pNew->bp = bpAddr; pNew->link = NULL;

    printf("\t    [ok] create breakpoint %05X\n", bpAddr);
    if (!BPHead) {
        BPHead = pNew;
        return 1;
    }
    if (bpAddr < BPHead->bp) {
        // head - pNew - ...
        pNew->link = BPHead;
        BPHead = pNew;
    }
    else if (bpAddr == BPHead->bp) {
        printf("\t     bp already set\n");
        return 0;
    }
    else {
        // head - node - pNew - ...
        pNew->link = BPHead->link;
        BPHead->link = pNew;

        // head - pMv1 - pNew - pMv2
        BPNode* pMv1 = BPHead;
        BPNode* pMv2 = pNew->link;
        while (pMv2) {
            if (pMv2->bp == bpAddr) {
                printf("\t     bp already set\n");
                return 0;
            }
            else if (pMv2->bp < bpAddr) {
                pMv1->link = pMv2;
                pNew->link = pMv2->link;
                pMv2->link = pNew;

                pMv1 = pMv1->link;
                pMv2 = pNew->link;
            }
            else break;
        }
    }

    return 1;
}

int clearBP() {
    if (!BPHead) {
        printf("\t    BP Already Cleared!\n");
        return 0;
    }

    BPNode* pFree;
    while (BPHead) {
        pFree = BPHead;
        BPHead = BPHead->link;
        free(pFree);
    }
    BPHead =  NULL;

    printf("Breakpoints Cleared!\n");
    return 1;
}

void printBP() {
    if (!BPHead) {
        printf("\t    No Breakpoint Set\n");
        return ;
    }
    printf("\t    breakpoint\n");
    printf("\t    ----------\n");
    BPNode* ptmp = BPHead;
    for ( ; ptmp; ptmp = ptmp->link)
        printf("\t    %05X\n", ptmp->bp);
}

int searchBP(int PC) {
    BPNode* pMv = BPHead;
    while (pMv) {
        if (pMv->bp == PC)
            return 1;
        else
            pMv = pMv->link;
    }
    return 0;
}
