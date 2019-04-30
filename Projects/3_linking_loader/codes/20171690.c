/*************************************************
 ************** System Programming ***************
 ****************** 20171690.c *******************
 ******************** Yuseok *********************
 ******************* ~ 190506 ********************
 *************************************************/
#include "20171690.h"
#include "linkingLoader.h"
#include "execution.h"

int main() {
    init();
    char* input;
    char* input_formed;

    
    while (1) {
        printf("sicsim> ");
        // allocate memories
        input = (char*)malloc(COMMAND_SIZE * sizeof(char));
        input_formed = (char*)malloc(COMMAND_SIZE * sizeof(char));

        // get input
        fflush(stdin);
        fgets(input, COMMAND_SIZE, stdin);
        if ((int)strlen(input) > COMMAND_SIZE) {
            printf("Too long command!\n");
            continue;
        }

        // remove white spaces 
        strcpy(input_formed, removeSpace(input));

        char* cmd = strtok(input_formed, " \t");
        char* params;
        char* start, *end, *addr, *val;
        int before, after;

        // if input is white spaces, continue;
        if (cmd) {
            switch (findCmd(cmd)) {
                case 0x00:  // h[elp]       done
                    if ( (params = strtok(NULL, "\0")) ) {
                        printf("h[elp]: '%s' is an invalid option. See 'h[elp]'\n", removeSpace(params));
                        break;
                    }

                    addHistory(input);
                    help();
                    break;
                case 0x01:  // d[ir]        done
                    if ( (params = strtok(NULL, "\0")) ) {
                        printf("d[ir]: '%s' is an invalid option. See 'h[elp]'\n", removeSpace(params));
                        break;
                    }

                    directory();
                    addHistory(input);

                    break;
                case 0x02:  // hi[story]    done
                    if ( (params = strtok(NULL, "\0")) ) {
                        printf("hi[story]: '%s' is an invalid option. See 'h[elp]'\n", removeSpace(params));
                        break;
                    }

                    addHistory(input);
                    history();
                    break;
                case 0x03:  // q[uit]       done
                    if ( (params = strtok(NULL, "\0")) ) {
                        printf("q[uit]: '%s' is an invalid option. See 'h[elp]'\n", removeSpace(params));
                        break;
                    }

                    free(input);
                    free(input_formed);
                    quit();
                    return 0;
                case 0x04:  // type filename
                    params = strtok(NULL, "\0");
                    if (!params) {
                        // no filename
                        printf("Invalid Syntax. See 'h[elp]'\n");
                        break;
                    }

                    params = removeSpace(params);
                    if (type(params))
                        addHistory(input);
                    break;

                case 0x10: case 0x11: case 0x12:    // du[mp] [start, end]
                    params = strtok(NULL, "\0");
                    if (!params) {
                        // "dump"
                        addHistory(input);
                        dump(NULL, NULL, 0);
                        break;
                    }

                    // parameter(s) follow(s)
                    before = (int)strlen(params);
                    start = removeSpace(strtok(params, ","));
                    after = (int)strlen(params);
                    end = strtok(NULL, "\0");
                    if (!end) {
                        if (before != after) {
                            // "dump start, "
                            printf("Invalid Syntax. See 'h[elp]'\n");
                            break;
                        }
                        if (!dump(start, NULL, 1)) {
                            // inavalid addr
                            break;
                        }
                        // "dump start"
                        addHistory(input);

                        break;
                    }

                    // "dump start, end"
                    end = removeSpace(end);
                    if (!dump(start, end, 2)) {
                        // invalid addr
                        break;
                    }
                    addHistory(input);
                    
                    break;
                case 0x13: case 0x14:               // e[dit] address, value
                    params = strtok(NULL, "\0");
                    if (!params) {
                        // no parameters
                        printf("Syntax Error. See 'h[elp]'\n");
                        break;
                    }

                    addr = removeSpace(strtok(params, ","));
                    val = strtok(NULL, "\0");
                    if (!val) {
                        // no value + invalid sytax
                        printf("Syntax Error. See 'h[elp]'\n");
                        break;
                    }

                    val = removeSpace(val);
                    if (!edit(addr, val)) {
                        // invalid addr & val
                        break;
                    }
                    addHistory(input);

                    break;

                case 0x15:          // f[ill] start, end, value
                    params = strtok(NULL, "\0");
                    if (!params) {
                        // No parameters at all
                        printf("Syntax Error. See 'h[elp]'\n");
                        break;
                    }

                    start = removeSpace(strtok(params, ","));
                    end = strtok(NULL, ",");
                    if (!end) {
                        // "fill start, "
                        printf("Syntax Error. See 'h[elp]'\n");
                        break;
                    }

                    end = removeSpace(end);
                    val = strtok(NULL, "\0");
                    if (!val) {
                        // "fill start, end, "
                        printf("Syntax Error. See 'h[elp]'\n");
                        break;
                    }

                    val = removeSpace(val);
                    if (!fill(start, end, val)) {
                        // invalid addr & val
                        break;
                    }
                    addHistory(input);

                    break;
                case 0x16:          // reset
                    if ( (params = strtok(NULL, "\0")) ) {
                        printf("reset: '%s' is an invalid option. See 'h[elp]'\n", removeSpace(params));
                        break;
                    }

                    reset();
                    addHistory(input);
                    printf("Reset Successfully\n");

                    break;

                case 0x20:  // opcode mnemonic      //done
                    params = strtok(NULL, "\0");
                    if (!params) {
                        // no mnemonic
                        printf("Invalid Syntax. See 'h[elp]'\n");
                        break;
                    }

                    params = removeSpace(params);
                    if (opcode(params, 1))
                        addHistory(input);
                    else 
                        printf("%s: No matching mnemonic\n", params);

                    break;
                case 0x21:  // opcodelist       done
                    if ( (params = strtok(NULL, "\0")) ) {
                        printf("opcodelist: '%s' is an invalid option. See 'h[elp]'\n", removeSpace(params));
                        break;
                    }

                    if ( !opcodeList() )
                        printf("There's no opcode.txt in the directory\n");
                    else
                        addHistory(input);
                        
                    break;

                case 0x30:          // assemble filename
                    params = strtok(NULL, "\0");
                    if (!params) {
                        // no filename
                        printf("Invalid Syntax. See 'h[elp]'\n");
                        break;
                    }

                    params = removeSpace(params);
                    if (assemble(params))
                            addHistory(input);
                    break;
                case 0x31:          // symbol
                    if ( (params = strtok(NULL, "\0")) ) {
                        printf("symbol: '%s' is an invalid option. See 'h[elp]'\n", removeSpace(params));
                        break;
                    }
                    if ( symbol() )
                        addHistory(input);
                    
                    break;

                case 0x40:
                    printSymbol();
                    break;

                case 0x50:          // progaddr address
                    params = strtok(NULL, "\0");
                    if (!params) {
                        PROGADDR = 0;
                        printf("PROGADDR RESET\n");
                        break;
                    }

                    params = removeSpace(params);
                    if ( progaddr(params) )
                        addHistory(input);
                    
                    break;

                case 0x51:          // loader obj obj obj
                    params = strtok(NULL, "\0");
                    if (!params) {
                        // No parameters at all
                        printf("Syntax Error. See 'h[elp]'\n");
                        break;
                    }

                    params = removeSpace(params);
                    if ( loader(params) )
                        addHistory(input);
                    break;

                case 0x52:          // run
                    if ( executeProg() ) {
                        printReg();
                        addHistory(input);
                    }

                    break;

                case 0x53:
                    params = strtok(NULL, "\0");
                    if (!params) {
                        // printf BP
                        printBP();
                        addHistory(input);
                        break;
                    }
                    
                    params = removeSpace(params);
                    if (strcmp(params, "clear") == 0) {
                        // clear all BPs
                        clearBP();
                        addHistory(input);
                        break;
                    }
                    if ( setBP(params) )
                        addHistory(input);
                    else 
                        printf("Syntax Error. See 'h[elp]'\n");
                    break;

                case 0xA0:
                    printf("command not found: %s\n", cmd);
                    break;
            }
        }

        free(input);
        free(input_formed);
    }

    return 0;
}

void init() {
    // initialize variants & allocate memories
    hisHead = NULL;
    SYMTAB = NULL;
    numHead = NULL;
    BPHead = NULL;
    BPTail = NULL;

    opTable = (opNode**)malloc(20 * sizeof(opNode*));
    for (int i = 0; i < 20; i++)
        opTable[i] = NULL;

    MEMORY = (unsigned char*)calloc(MEMORY_SIZE, sizeof(unsigned char));
    END_ADDR = 0;

    PROGADDR = 0;
    EXEC_ADDR = EXEC_LEN = -1;
    

    readOpTable();
}

int findCmd(char* cmd) {
    // get command as a parameter, return proper cmd
    
    if (strcmp(cmd, "help") == 0 || strcmp(cmd, "h") == 0) return 0x00;
    if (strcmp(cmd, "dir") == 0 || strcmp(cmd, "d") == 0) return 0x01;
    if (strcmp(cmd, "history") == 0 || strcmp(cmd, "hi") == 0) return 0x02;
    if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "q") == 0) return 0x03;
    if (strcmp(cmd, "type") == 0) return 0x04;

    if (strcmp(cmd, "dump") == 0 || strcmp(cmd, "du") == 0) return 0x10;
    if (strcmp(cmd, "edit") == 0 || strcmp(cmd, "e") == 0) return 0x13;
    if (strcmp(cmd, "fill") == 0 || strcmp(cmd, "f") == 0) return 0x15;
    if (strcmp(cmd, "reset") == 0) return 0x16;

    if (strcmp(cmd, "opcode") == 0) return 0x20;
    if (strcmp(cmd, "opcodelist") == 0) return 0x21;

    if (strcmp(cmd, "assemble") == 0) return 0x30;
    if (strcmp(cmd, "symbol") == 0) return 0x31;

    if (strcmp(cmd, "printSym") == 0) return 0x40;


    if (strcmp(cmd, "progaddr") == 0) return 0x50;
    if (strcmp(cmd, "loader") == 0) return 0x51;

    if (strcmp(cmd, "run") == 0) return 0x52;
    if (strcmp(cmd, "bp") == 0) return 0x53;

    return 0xA0;
}

char* removeSpace(char* input) {
    // remove Spaces at front & at the end
    if (!input) return input;
    int i;
    for (i = 0; input[i] == ' ' || input[i] == '\t' || input[i] == '\n'; i++) ;
    input = input + i;
 
    i = 0;
    while (input[strlen(input) - 1 - i] == ' ' || input[strlen(input) - 1 - i] == '\n' || input[strlen(input) - 1 - i] == '\t') i++;
    input[strlen(input) - i] = '\0';

    return input;
}
