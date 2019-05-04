/*************************************************
 ************** System Programming ***************
 ******************** shell.c ********************
 ******************** Yuseok *********************
 ******************* ~ 190506 ********************
 *************************************************/
#include "20171690.h"

void help() {
    printf("\t\th[elp]\n\t\td[ir]\n\t\tq[uit]\n\t\thi[story]\n");
    printf("\t\tdu[mp] [start, end]\n");
    printf("\t\te[dit] address, value\n");
    printf("\t\tf[ill] start, end, value\n");
    printf("\t\treset\n");
    printf("\t\topcode mnemonic\n\t\topcodelist\n");
    printf("\t\tassemble filename\n\t\ttype filename\n\t\tsymbol\n");

    printf("\t\tprogaddr [address]\n");
    printf("\t\tloader [object filename] [object filename] [...]\n");
    printf("\t\trun\n");
    printf("\t\tbp [address]\n\t\tbp clear\n\t\tbp\n");
}

void directory() {
    struct dirent* dirEntry;        // directory entry Pointer
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
}
void addHistory(char* input) {
    HISTORY* pNew = (HISTORY*)malloc(sizeof(HISTORY));
    pNew->hist = (char*)malloc(COMMAND_SIZE * sizeof(char));
    strcpy(pNew->hist, input);
    pNew->link = NULL;

    if (!hisHead) {
        hisHead = pNew;
        pNew->num = 1;
        return ;
    }
    HISTORY* pMove;
    for (pMove = hisHead; pMove->link; pMove = pMove->link) ;
    pMove->link = pNew;
    pNew->num = pMove->num + 1;
}

void history() {
    HISTORY* pMove;
    for (pMove = hisHead; pMove; pMove = pMove->link) {
        printf("\t%d\t%s\n", pMove->num, pMove->hist);
    }
}

void quit() {
    // free memories
    HISTORY* pFree;

    while (hisHead) {
        pFree = hisHead;
        hisHead = hisHead->link;
        free(pFree);
    }

    opNode* opFree, *optmp;
    for (int i = 0; i < 20; i++) {
        optmp = opTable[i];
        while (optmp) {
            opFree = optmp;
            optmp = optmp->link;
            free(opFree);
        }
    }
    free(MEMORY);

    freeSymTab();
    freeNums();
    freeQueue();

    printf("exit shell\n");
}

int type(char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("type: print [%s]: No such file in current directory\n", filename);
        return 0;
    }

    char ch = fgetc(fp);
    while (!feof(fp)) {
        printf("%c", ch);
        ch = fgetc(fp);
    }


    return 1;
}
