/*************************************************
 ************** System Programming ***************
 **************** opcodeAction.c *****************
 ******************** Yuseok *********************
 ******************* ~ 190506 *********************
 *************************************************/
#include "20171690.h"
#include "execution.h"

int opAct(int opcode, int format, int target, int flags) {
    // operate proper action
    // format always 1 ... 4
    // target, either reg or disp/addr
    // flags get nixbpe info
    // if DONE successfully, return 1
    // else, return 0
    if (format == 1) {
        switch (opcode) {
            case 0xC4:      // FIX
                A = (int)F;
                break;
            case 0xC0:      // FLOAT
                F = (float)A;
                break;
            /** case 0xF4:      // HIO */
            /**     break; */
            /** case 0xC8:      // NORM */
            /**     break; */
            /** case 0xF0:      // SIO */
            /**     break; */
            /** case 0xF8:      // TIO */
            /**     break; */
        }
    }
    else if (format == 2) {
        unsigned char _reg1 = target >> 4;
        unsigned char _reg2 = target & (unsigned char)0x0F;

        int* r1 = getRegPtr(_reg1);
        int* r2 = getRegPtr(_reg2);
        switch (opcode) {
            case 0x90:      // ADDR r1, r2
                if (!r1 || !r2) return 0;
                *r2 += *r1;
                break;
            case 0xB4:      // CLEAR r1
                if (!r1) return 0;
                *r1 = 0;
                break;
            case 0xA0:      // COMPR r1, r2
                if (*r1 == *r2) CC = 1;
                else CC = 0;
                break;
            case 0x9C:      // DIVR r1, r2
                if (!r1 || !r2) return 0;
                *r2 /= *r1;
                break;
            case 0x98:      // MULR r1, r2
                if (!r1 || !r2) return 0;
                *r2 *= *r1;
                break;
            case 0xAC:      // RMO r1, r2
                if (!r1 || !r2) return 0;
                *r2 = *r1;
                break;
            case 0xA4:      // SHIFTL r1, n
                if (!r1) return 0;
                *r1 = (*r1 << (_reg2+1));
                break;
            case 0xA8:      // SHIFTR r1, n
                if (!r1) return 0;
                *r1 = (*r1 >> (_reg2+1));
                break;
            case 0x94:      // SUBR r1, r2
                if (!r1 || !r2) return 0;
                *r2 -= *r1;
                break;
            /** case 0xB0:      // SVC n */
            /**     break; */
            case 0xB8:      // TIXR r1
                X += 1;
                if (!r1) return 0;
                if (*r1 == X) CC = 1;
                else CC = 0;
                break;
        }

    }
    else {
        // format 3 or 4
        unsigned char ni, x, b, p, e;
        ni = flags >> 4;
        x = (flags >> 3) % 2;
        b = (flags >> 2) % 2;
        p = (flags >> 1) % 2;
        e = flags % 2;
        if (p) {
            target = (signed char)target + PC;
        }
        else if (b) {
            target += B;
        }
        if (x) {
            target += X;
        }

        printf("%02X - %d %d %d %d %d\n", flags, ni, x, b, p, e);
        int memVal = 0;
        int LOC = target;
        switch (ni) {
            case 1:         // immediate addressing
                memVal = target;
                break;
            case 2:         // indirect addressing
                for (int j = 0; j < 2; j++) {
                    // READ MEMORY
                    // READ VALUE AS ADDR OF MEMORY AGAIN!
                    memVal = 0;
                    for (int i = 2; i >= 0; i--) {
                        // READ ONE WORD SIZE from the MEMORY
                        if ( !validAddr(LOC) ) {
                            printf("Segmentation Fault!_1\n");
                            return 0;
                        }
                        memVal += (*(MEMORY + LOC++) << (i << 8));
                    }
                    if (j == 0) {
                        LOC = memVal;
                        target = memVal;
                    }
                    printf("%06X\n", memVal);
                }
                break;
            case 3:         // simple addressing
                for (int i = 2; i >= 0; i--) {
                    // READ ONE WORD SIZE from the MEMORY
                    if ( !validAddr(LOC) ) {
                        printf("Segmentation Fault!_2\n");
                        return 0;
                    }
                    memVal += ((*(MEMORY + LOC++) << (i << 8)));
                    printf("%06X ", memVal);
                }
                printf("\n");
                break;
        }

        switch (opcode) {
            // memory operand is stored in memVal variable
            case 0x18:        // ADD m
                A += memVal;
                break;
            /** case 0x58:        // ADDF m */
            /**     break; */
            case 0x40:        // AND m
                A &= memVal;
                break;
            case 0x28:        // COMP m
                if (A == memVal) CC = 1;
                else CC = 0;
                break;
            /** case 0x88:        // COMPF m */
            /**     break; */
            case 0x24:        // DIV m
                A /= memVal;
                break;
            /** case 0x64:        // DIVF m */
            /**     break; */
            case 0x3C:        // J m
                PC = target;
                break;
            case 0x30:        // JEQ m
                if (CC) PC = target;
                break;
            case 0x34:        // JGT m
                if (CC > 0) PC = target;
                break;
            case 0x38:        // JLT m
                if (CC < 0) PC = target;
                break;
            case 0x48:        // JSUB m
                L = PC;
                PC = target;
                break;
            case 0x00:        // LDA m
                A = memVal;
                break;
            case 0x68:        // LDB m
                B = memVal;
                break;
            case 0x50:        // LDCH m
                memVal = (memVal >> 16) | (unsigned char)0xFFFF00;
                A &= (unsigned char)memVal;
                break;
            /** case 0x70:        // LDF m */
            /**     break; */
            case 0x08:        // LDL m
                L = memVal;
                break;
            case 0x6C:        // LDS m
                S = memVal;
                break;
            case 0x74:        // LDT m
                T = memVal;
                break;
            case 0x04:        // LDX m
                X = memVal;
                break;
            /** case 0xD0:        // LPS m */
            /**     break; */
            case 0x20:        // MUL m
                A *= memVal;
                break;
            /** case 0x60:        // MULF m */
            /**     break; */
            case 0x44:        // OR m
                A |= memVal;
                break;
            /** case 0xD8:        // RD m */
            /**     break; */
            case 0x4C:        // RSUB
                PC = L;
                break;
            case 0xEC:        // SSK m
                break;
            case 0x0C:        // STA m
                LOC -= 3;
                if ( !write_to_memory(LOC, A) )
                    return 0;
                break;
            case 0x78:        // STB m
                LOC -= 3;
                if ( !write_to_memory(LOC, B) )
                    return 0;
                break;
            case 0x54:        // STCH m
                LOC -= 3;
                if ( !validAddr(LOC) ) {
                    printf("Segmentation Fault!_STCH\n");
                    return 0;
                }
                memVal = A & (unsigned char)0x0FF;
                memcpy(MEMORY + LOC, &memVal, 1);
                break;
            case 0x80:        // STF m
                break;
            case 0xD4:        // STI m
                break;
            case 0x14:        // STL m
                LOC -= 3;
                if ( !write_to_memory(LOC, L) )
                    return 0;
                break;
            case 0x7C:        // STS m
                LOC -= 3;
                if ( !write_to_memory(LOC, S) )
                    return 0;
                break;
            case 0xE8:        // STSW m
                LOC -= 3;
                if ( !write_to_memory(LOC, SW) )
                    return 0;
                break;
            case 0x84:        // STT m
                LOC -= 3;
                if ( !write_to_memory(LOC, T) )
                    return 0;
                break;
            case 0x10:        // STX m
                LOC -= 3;
                if ( !write_to_memory(LOC, X) )
                    return 0;
                break;
            case 0x1C:        // SUB m
                A -= memVal;
                break;
            /** case 0x5C:        // SUBF m */
            /**     break; */
            case 0xE0:        // TD m
                CC = 0;
                break;
            case 0x2C:        // TIX m
                X += 1;
                if (X == memVal) CC = 1;
                else CC = 0;
                break;
            /** case 0xDC:        // WD m */
            /**     break; */

        }

    }

    return 1;
}

int* getRegPtr(unsigned char reg) {
    switch (reg) {
        case 0x00:
            return &A;
        case 0x01:
            return &X;
        case 0x02:
            return &L;
        case 0x03:
            return &B;
        case 0x04:
            return &S;
        case 0x05:
            return &T;
        case 0x06:
            return &F;
        case 0x08:
            return &PC;
        case 0x09:
            return &SW;
        default:
            return NULL;
    }
}

int write_to_memory(int LOC, int memVal) {
    // Write ONE WORD SIZE value to memory
    // if success, return 1
    // else, return 0
    if ( !validAddr(LOC + 2) ) {
        printf("Segmentation Fault!_writing\n");
        return 0;
    }
    unsigned char b[3];
    b[0] = memVal >> 16;
    b[1] = (memVal >> 8) & (unsigned char)0x00FF;
    b[2] = memVal & (unsigned char)0x00FF;
    
    for (int i = 0; i < 3; i++) {
        b[i] &= ONE_BYTE;
        memcpy(MEMORY + LOC++, &(b[i]), 1);
    }

    return 1;
}
