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



int opAct(int opcode, int format, int target, int flags);


void printReg();

int executeProg();
int setBP(char* addr);
void clearBP();
void printBP();
int* getRegPtr(unsigned char reg);

int read_2B_float(int LOC, int memVal);
float getFloat(int f);
