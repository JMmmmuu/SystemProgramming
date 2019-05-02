/*************************************************
 ************** System Programming ***************
 ***************** execution.c *******************
 ******************** Yuseok *********************
 ******************* ~ 190506 *********************
 *************************************************/
#include "20171690.h"
#include "execution.h"

int executeProg() {
    // execute last linked & loaded program
    // its starting addr & len is stored in EXEC_ADDR & EXEC_LEN
    // if execute successfully, return 1
    // else, return 0

    if (EXEC_LEN == -1) {
        printf("Please load the program first\n");
        return 0;
    }

    A = 0; X = 0; L = 0; SW = 0; B = 0; S = 0; T = 0; F = 0; CC = 0;
    PC = EXEC_ADDR;
    int opcode;
    unsigned char flags, reg, ni;//, x, b, p, e;
    int format;
    int target;

    int cnt = 0;

    if (EXEC_LEN + EXEC_ADDR >= 0x100000) {
        printf("Segmentation Fault!\n");
        return 0;
    }
    while (PC < EXEC_ADDR + EXEC_LEN && PC >= EXEC_ADDR) {
        if ( !validAddr(PC + 2) ) {
            printf("Segmentation Fault!\n - %06X\n", PC);
            return 0;
        }
        opcode = *(MEMORY + PC++);
        ni = opcode & (unsigned char)0x03;
        opcode &= (unsigned char)0xFC;

        if ( (format = searchWithOpcode(opcode)) != 0 ) {
            // if opcode exists
            printf("[PC %06X], ", PC-1);
            switch (format) {
                case 1:
                    opAct(opcode, format, 0, 0);
            printf("\n");
                    break;
                case 2:
                    reg = *(MEMORY + PC++);
                    opAct(opcode, format, reg, 0);
            printf("\n");
                    break;
                case 3:
                    if (ni == 0) {
                        PC += 2;
                        printf("\n");
                        /** if ( ++cnt == 40) return 0; */
                        continue;
                    }
                    flags = *(MEMORY + PC++);
                    target = (flags & (unsigned char)0x0F) << 8;
                    flags = (flags >> 4) + (ni << 4);

                    if (flags % 2) format = 4;     // if e flag set
                    for (int i = (format == 3) ? 0 : 1; i >= 0; i--) {
                        if ( !validAddr(PC) ) {
                            printf("Segmentation Fault! - %06X\n", PC);
                            return 0;
                        }
                        target += (*(MEMORY + PC++) << (i * 8));
                    }

                    printf("\t[opcode %02X], [format %d], [flags %01X], [target %05X]\n", opcode, format, flags, target);
                    if ( !opAct(opcode, format, target, flags) )
                        return 0;

                    break;
            }
        }
        else {
            // WORD CONST
            // no change in REG or MEM
            PC += 2;
            printf("\n");
        }

        printReg();
        /** if ( ++cnt == 40) return 0; */
    }

    return 1;
}

void printReg() {
    printf("\t\tA : %012X X : %08X\n", A, X);
    printf("\t\tL : %012X PC: %012X\n", L, PC);
    printf("\t\tB : %012X S : %012X\n", B, S);
    printf("\t\tT : %012X\n", T);
}

int setBP(char* addr) {
    // set BreakPoint
    // if success, return 1
    // else, return 0
    int bpAddr = strToHex(addr, 0);
    if (bpAddr == -1)   // wrong parameter
        return 0;

    BPNode* pNew = (BPNode*)malloc(sizeof(BPNode));
    pNew->bp = bpAddr; pNew->link = NULL;

    if (!BPHead) {
        BPHead = pNew;
        BPTail = pNew;
        return 1;
    }
    BPTail->link = pNew;
    BPTail = pNew;
    return 1;
}

void clearBP() {
    if (!BPHead) return ;

    BPNode* pFree;
    while (BPHead) {
        pFree = BPHead;
        BPHead = BPHead->link;
        free(pFree);
    }
    BPHead = BPTail = NULL;

    printf("Breakpoint Cleared!\n");
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
        printf("\t    %04X\n", ptmp->bp);
}
