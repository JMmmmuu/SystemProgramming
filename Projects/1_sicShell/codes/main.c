/*************************************************
 ************** System Programming ***************
 ******************** Yuseok *********************
 ******************** 190325 *********************
 *************************************************/
#include "header.h"

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
        if (strlen(input) > COMMAND_SIZE) {
            printf("Too long command!\n");
            continue;
        }


        // remove white spaces 
        strcpy(input_formed, removeSpace(input));
        //printf("%s\n", input_formed);

        char* cmd = strtok(input_formed, " \t");
        char* params;
        int res;
        int start, end, addr, val;
        //char* left = removeSpace(strtok(NULL, "\0"));
        //printf("%s\n", cmd);
        //printf("%s\n", cmd+strlen(cmd) + 1);

        /*
        printf("cmd: %s\n", cmd);
        char* tmp = input_formed + strlen(cmd) + 1;
        printf("2_%s\n", tmp);
        char* op1 = strtok(NULL, ",");
        if (op1) {
            printf("3_%s\n", removeSpace(op1));
            char* op2 = strtok(NULL, ",");
            if (op2) {
                printf("4_%s\n", removeSpace(op2));
            }
        }*/
        //char* op = strtok(NULL, "\0");
        //printf("op: %s\n", op);


        // if input is white spaces, continue;
        if (cmd) {
            switch (findCmd(cmd)) {
                case 0x00:  // h[elp]       done
                    if ( (cmd = strtok(NULL, " ")) ) {
                        printf("h[elp]: '%s' is not a correct option. See 'h[elp]'\n", cmd);
                        break;
                    }

                    addHistory(input);
                    help();
                    break;
                case 0x01:  // d[ir]        done
                    if ( (cmd = strtok(NULL, " ")) ) {
                        printf("d[ir]: '%s' is not a correct option. See 'h[elp]'\n", cmd);
                        break;
                    }

                    addHistory(input);
                    struct dirent *dirEntry;      // directory entry Pointer
                    struct stat fileInfo;
                    DIR* dr = opendir(".");
                    while ( (dirEntry = readdir(dr)) != NULL) {
                        printf("\t\t%s", dirEntry->d_name);
                        stat(dirEntry->d_name, &fileInfo);
                        if (S_ISDIR(fileInfo.st_mode)) printf("/");
                        else if (fileInfo.st_mode & S_IXUSR) printf("*");
                        printf("\n");
                    }

                    closedir(dr);
                    break;
                case 0x02:  // hi[story]    done
                    if ( (cmd = strtok(NULL, " ")) ) {
                        printf("hi[story]: '%s' is not a correct option. See 'h[elp]'\n", cmd);
                        break;
                    }

                    addHistory(input);
                    history();
                    break;
                case 0x03:  // q[uit]       done
                    if ( (cmd = strtok(NULL, " ")) ) {
                        printf("q[uit]: '%s' is not a correct option. See 'h[elp]'\n", cmd);
                        break;
                    }

                    free(input);
                    free(input_formed);
                    quit();
                    return 0;
                case 0x10: case 0x11: case 0x12:    // du[mp] [start, end]
                    params = strtok(NULL, "\0");
                    if (!params) {
                        // dump 10 lines
                        addHistory(input);

                        break;
                    }
                    
                    res = scanf(params, " %x , %x", &start, &end);
                    if (res == 1) {
                        // if there's comma, print error
                        
                        // else, dump from start
                        addHistory(input);

                        break;
                    }
                    if (res == 2) {
                        // if there's another char except parameters, print error
                        
                        // else, dump from start to end
                        addHistory(input);

                        break;
                    }
                    break;
                case 0x13: case 0x14:               // e[dit] address, value
                    params = strtok(NULL, "\0");
                    if (!params) {
                        // No parameters at all
                        printf("Not enough parameters. See 'h[elp]'\n");
                        break;
                    }

                    res = scanf(params, " %x , %x", &addr, &val);
                    if (res != 2) {
                        // Not enough parameters
                        printf("Not enough parameters. See 'h[elp]'\n");
                        break;
                    }

                    addHistory(input);
                    break;
                case 0x15:          // f[ill] start, end, value
                    params = strtok(NULL, "\0");
                    if (!params) {
                        // No parameters at all
                        printf("Not enough parameters. See 'h[elp]'\n");
                        break;
                    }

                    printf("%s\n", params);
                    res = sscanf(params, " %x , %x , %x", &start, &end, &val);
                    if (res != 3) {
                        // Not enough parameters
                        printf("Not enough parameters. See 'h[elp]'\n");
                        break;
                    }

                    addHistory(input);
                    

                    break;
                case 0x16:          // reset
                    if ( !(cmd = strtok(NULL, " ")) ) {
                        printf("reset: '%s' is not a correct option. See 'h[elp]'\n", cmd);
                        break;
                    }

                    addHistory(input);
                    reset();
                    break;
                case 0x20:  // opcode mnemonic
                    printf("%s\n", input_formed);
                    cmd = strtok(NULL, " ");
                    printf("%s\n", cmd);
                    printf("0x20\n");
                    break;
                case 0x21:  // opcodelist       done
                    if ( !(cmd = strtok(NULL, " ")) ) {
                        printf("opcodelist: '%s' is not a correct option. See 'h[elp]'\n", cmd);
                        break;
                    }

                    addHistory(input);
                    opcodeList();
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

    MEMORY = malloc(MEMORY_SIZE);
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
