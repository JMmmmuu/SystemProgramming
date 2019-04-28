/*************************************************
 ************** System Programming ***************
 **************** linkingLoader.h  ***************
 ******************** Yuseok *********************
 ******************* ~ 190506 ********************
 *************************************************/
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

void haltLinkingLoader(char** objFile, FILE** objFP, EShead* ESTAB, char* tmp);



int isObjFile(char* file);
