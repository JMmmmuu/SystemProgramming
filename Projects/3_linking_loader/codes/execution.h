/*************************************************
 ************** System Programming ***************
 ***************** execution.h *******************
 ******************** Yuseok *********************
 ******************* ~ 190506 *********************
 *************************************************/

typedef struct _BP {
    int bp;
    struct _BP* link;
} BPNode;


BPNode* BPHead;
BPNode* BPTail;
int EXEC_ADDR;
int EXEC_LEN;






void printReg();

int executeProg();
int setBP(char* addr);
void clearBP();
void printBP();

