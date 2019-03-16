/*************************************************
 ************** System Programming ***************
 ******************** Yuseok *********************
 ******************** 190325 *********************
 *************************************************/
#include "20171690.h"

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

        // if input is white spaces, continue;
        if (cmd) {
            switch (findCmd(cmd)) {
                case 0x00:  // h[elp]       done
                    if ( (params = strtok(NULL, " ")) ) {
                        printf("h[elp]: '%s' is an invalid option. See 'h[elp]'\n", params);
                        break;
                    }

                    addHistory(input);
                    help();
                    break;
                case 0x01:  // d[ir]        done
                    if ( (params = strtok(NULL, " ")) ) {
                        printf("d[ir]: '%s' is an invalid option. See 'h[elp]'\n", params);
                        break;
                    }

                    directory();
                    addHistory(input);

                    break;
                case 0x02:  // hi[story]    done
                    if ( (params = strtok(NULL, " ")) ) {
                        printf("hi[story]: '%s' is an invalid option. See 'h[elp]'\n", params);
                        break;
                    }

                    addHistory(input);
                    history();
                    break;
                case 0x03:  // q[uit]       done
                    if ( (params = strtok(NULL, " ")) ) {
                        printf("q[uit]: '%s' is an invalid option. See 'h[elp]'\n", params);
                        break;
                    }

                    free(input);
                    free(input_formed);
                    quit();
                    return 0;
                case 0x10: case 0x11: case 0x12:    // du[mp] [start, end]
                    params = strtok(NULL, "\0");
                    if (!params) {
                        // "dump"
                        addHistory(input);
                        dump(NULL, NULL, 0);
                        break;
                    }

                    // parameter(s) follow(s)
                    int before = (int)strlen(params);
                    start = removeSpace(strtok(params, ","));
                    int after = (int)strlen(params);
                    end = strtok(NULL, "\0");
                    if (!end) {
                        // "dump start"
                        if (before != after) {
                            // "dump start, "
                            printf("Invalid Syntax. See 'h[elp]'\n");
                            break;
                        }
                        if (!dump(start, NULL, 1)) {
                            // inavalid addr
                            break;
                        }
                        addHistory(input);

                        break;
                    }

                    // "dump start, end"
                    end = removeSpace(end);
                    if (!dump(start, end, 2)) break;
                    addHistory(input);
                    
                    break;
                case 0x13: case 0x14:               // e[dit] address, value
                    addr = removeSpace(strtok(NULL, ","));
                    if (!addr) {
                        // no comma
                        printf("Syntax Error. See 'h[elp]'\n");
                        break;
                    }

                    val = removeSpace(strtok(NULL, "\0"));
                    if (!val) {
                        // no value
                        printf("Syntax Error. See 'h[elp]'\n");
                        break;
                    }

                    if (!edit(addr, val)) {
                        // wrong syntax
                        printf("Syntax Error. See 'h[elp]'\n");
                        break;
                    }
                    addHistory(input);
                    //printf("addr: %s val: %s\n", addr, val);

                    break;

                case 0x15:          // f[ill] start, end, value
                    start = removeSpace(strtok(NULL, ","));
                    if (!start) {
                        // No parameters at all
                        printf("Syntax Error. See 'h[elp]'\n");
                        break;
                    }

                    end = removeSpace(strtok(NULL, ","));
                    if (!end) {
                        // No parameters at all
                        printf("Syntax Error. See 'h[elp]'\n");
                        break;
                    }

                    val = removeSpace(strtok(NULL, "\0"));
                    if (!val) {
                        // No parameters at all
                        printf("Syntax Error. See 'h[elp]'\n");
                        break;
                    }

                    addHistory(input);
                    fill(start, end, val);
                    

                    break;
                case 0x16:          // reset
                    if ( (params = strtok(NULL, " ")) ) {
                        printf("reset: '%s' is an invalid option. See 'h[elp]'\n", params);
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
                    if (opcode(params))
                        addHistory(input);
                    else 
                        printf("%s: No matching mnemonic\n", params);

                    break;
                case 0x21:  // opcodelist       done
                    if ( (params = strtok(NULL, " ")) ) {
                        printf("opcodelist: '%s' is an invalid option. See 'h[elp]'\n", params);
                        break;
                    }

                    if (!opcodeList())
                        printf("There's no opcode.txt in the directory\n");
                    else
                        addHistory(input);
                        
                    break;

                case 0x30:
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
    // allocate memories
    hisHead = NULL;

    opTable = (opNode**)malloc(20 * sizeof(opNode*));
    for (int i = 0; i < 20; i++)
        opTable[i] = NULL;

    MEMORY = (unsigned char*)calloc(MEMORY_SIZE, sizeof(unsigned char));
    END_ADDR = 0;

    readOpTable();
}

int findCmd(char* cmd) {
    // get command as a parameter, return proper cmd
    
    if (strcmp(cmd, "help") == 0 || strcmp(cmd, "h") == 0) return 0x00;
    if (strcmp(cmd, "dir") == 0 || strcmp(cmd, "d") == 0) return 0x01;
    if (strcmp(cmd, "history") == 0 || strcmp(cmd, "hi") == 0) return 0x02;
    if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "q") == 0) return 0x03;

    if (strcmp(cmd, "dump") == 0 || strcmp(cmd, "du") == 0) return 0x10;
    if (strcmp(cmd, "edit") == 0 || strcmp(cmd, "e") == 0) return 0x13;
    if (strcmp(cmd, "fill") == 0 || strcmp(cmd, "f") == 0) return 0x15;
    if (strcmp(cmd, "reset") == 0) return 0x16;

    if (strcmp(cmd, "opcode") == 0) return 0x20;
    if (strcmp(cmd, "opcodelist") == 0) return 0x21;

    return 0x30;
}

char* removeSpace(char* input) {
    // remove Spaces at front & at the end
    int i;
    for (i = 0; input[i] == ' ' || input[i] == '\t' || input[i] == '\n'; i++) ;
    input = input + i;
 
    i = 0;
    while (input[strlen(input) - 1 - i] == ' ' || input[strlen(input) - 1 - i] == '\n' || input[strlen(input) - 1 - i] == '\t') i++;
    input[strlen(input) - i] = '\0';

    return input;
}
