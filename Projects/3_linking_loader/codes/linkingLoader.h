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


int PROGADDR;
EShead* ESTAB;

int progaddr(char* addr);
int loader(char* param);

void loadMap(int objCnt);

void addES(EShead* ES, char* name, char* loc);
void haltLinkingLoader(char** objFile, FILE** objFP, EShead* ESTAB, char* tmp);



int isObjFile(char* file);
void printES(int objCnt);
