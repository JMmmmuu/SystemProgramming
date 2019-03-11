/*************************************************
 ************** System Programming ***************
 ******************** Yuseok *********************
 ******************** 190325 *********************
 *************************************************/
#include "header.h"

int main() {
    init();
    char* input = (char*)malloc(COMMAND_SIZE * sizeof(char));
    char* input_formed = (char*)malloc(COMMAND_SIZE * sizeof(char));
    //int cmdNum;

    
    while (1) {
        // get command
        fflush(stdin);
        fgets(input, COMMAND_SIZE, stdin);
        //strcpy(input_formed, input);

        // refine into form
        strcpy(input_formed, removeSpace(input));
        printf("JUST : %s\n", input);
        printf("FORM : %s\n", input_formed);


        char* cmd = strtok(input_formed, " ");
        printf("%x\n", findCmd(cmd));
        switch (findCmd(cmd)) {
            case 0x00:
                printf("0x00\n");
                break;
            case 0x01:
                printf("0x01\n");
                break;
            case 0x02:
                printf("0x02\n");
                break;
            case 0x03:
                printf("0x03\n");
                break;
            case 0x10: case 0x11: case 0x12:
                printf("0x10\n");
                break;
            case 0x13: case 0x14:
                printf("0x13\n");
                break;
            case 0x15:
                printf("0x15\n");
                break;
            case 0x16:
                printf("0x16\n");
                break;
            case 0x20:
                printf("0x20\n");
                break;
            case 0x21:
                printf("0x21\n");
                break;

            case 0x30:
                continue;
        }


        printf("sicsim> ");

    }

    quit();
    return 0;
}

void init() {
    // allocate memories


    printf("sicsim> ");
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

int findCommand() {


    return 0;
}

char* removeSpace(char* input) {
    // remove Spaces at front & at the end
    int i;
    for (i = 0; input[i] == ' '; i++) ;
    input = input + i;
 
    i = 0;
    while (input[strlen(input) - i - 1] == ' ' || input[strlen(input) - i - 1] == '\n') i++;
    input[strlen(input) - i] = '\0';

    return input;
}
