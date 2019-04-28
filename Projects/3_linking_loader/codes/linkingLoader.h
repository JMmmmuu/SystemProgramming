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
EShead ESTAB[3];
