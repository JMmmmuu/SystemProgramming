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
        return 1;
    }

    A = 0; X = 0; L = 0; PC = 0; SW = 0; B = 0; S = 0; T = 0; F = 0;
    PC = EXEC_ADDR;
    int opcode;
    unsigned char flags, reg, ni;//, x, b, p, e;
    int format;
    int target;

    if (EXEC_LEN + EXEC_ADDR >= 0x100000) {
        printf("Segmentation Fault!\n");
        return 0;
    }
    while (PC < EXEC_ADDR + EXEC_LEN) {
        if ( !validAddr(PC + 3) ) {
            printf("Segmentation Fault!\n - %06X\n", PC);
            return 0;
        }
        opcode = *(MEMORY + PC) << 4;
        opcode += *(MEMORY + ++PC);
        ni = opcode & (unsigned char)0x03;
        opcode &= (unsigned char)0xFC;

        if ( (format = searchWithOpcode(opcode)) ) {
            // if opcode exists
            switch (format) {
                case 1:
                    PC++;
                    opAct(opcode, format, 0, 0);
                    break;
                case 2:
                    reg = *(MEMORY + ++PC);
                    opAct(opcode, format, reg, 0);
                    break;
                case 3:
                    flags = *(MEMORY + ++PC);
                    if (flags / 2 == 1) format = 4;
                    if (format == 3) {
                        for (int i = 2; i >= 0; i--) {
                            if ( !validAddr(PC) ) {
                                printf("Segmentation Fault! - %06X\n", PC);
                                return 0;
                            }
                            target = *(MEMORY + ++PC) << (i * 4);
                        }
                    }
                    else {
                        for (int i = 4; i >= 0; i--) {
                            if ( !validAddr(PC) ) {
                                printf("Segmentation Fault! - %06X\n", PC);
                                return 0;
                            }
                            target = *(MEMORY + ++PC) << (i * 4);
                        }
                    }

                    flags += (ni << 4);
                    if ( !opAct(opcode, format, target, flags) )
                        return 0;

                    break;
            }
        }
        else {
            // WORD CONST
            // no change in REG or MEM
            PC++;
        }
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
