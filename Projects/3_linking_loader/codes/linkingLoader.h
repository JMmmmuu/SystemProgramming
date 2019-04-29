/*************************************************
 ************** System Programming ***************
 **************** linkingLoader.h  ***************
 ******************** Yuseok *********************
 ******************* ~ 190506 ********************
 *************************************************/
#define MAX_LINE_LEN 0xFF

typedef struct _ESnode {
    char name[9];
    int LOC;
    struct _ESnode* link;
} ESnode;

typedef struct _EShead {
    char CSname[9];
    int CSaddr;
    int CSlength;
    ESnode* link;
} EShead;

typedef struct _referNode {
    int ref;
    int addr;
    struct _referNode* link;
} referNode;

int PROGADDR;
EShead* ESTAB;
referNode* referHead;

int progaddr(char* addr);
int loader(char* param);

void loadMap(int objCnt);
int HRecord();
int DRecord();
int RRecord(char* line, int objCnt);
int TRecord(char* line, EShead CShead);
int MRecord(char* line, EShead CShead);

int setMem(int addr, int val);

void addES(EShead* ES, char* name, char* loc);
void addRN(char* ref, int addr);
int searchESTAB(char* name, int objCnt);
int searchRN(char* ref);

void haltLinkingLoader(char** objFile, FILE** objFP, EShead* ESTAB);



int isObjFile(char* file);
void printES(int objCnt);
void freeRN();
