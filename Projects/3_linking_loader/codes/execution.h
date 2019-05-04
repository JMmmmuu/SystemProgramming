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
int EXEC_ADDR;
int EXEC_LEN;
int PROG_START;
int PROG_END;
int continuing;



int opAct(int opcode, int format, int target, int flags);


void printReg();

int executeProg();
int setBP(char* addr);
int clearBP();
void printBP();
int searchBP(int PC);
int* getRegPtr(unsigned char reg);

int read_2B_float(int LOC, int memVal);
float getFloat(int f);
int write_to_memory(int LOC, int memVal);
