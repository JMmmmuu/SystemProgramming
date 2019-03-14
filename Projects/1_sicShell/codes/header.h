/*************************************************
 ************** System Programming ***************
 ******************** Yuseok *********************
 ******************* ~ 190325 ********************
 *************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

// MACROs
#define MEMORY_SIZE 1<<10
#define COMMAND_SIZE 100
#define OPCODE "opcode.txt"

// STRUCTUREs
typedef struct hist {
    int num;
    char* hist;
    struct hist* link;
} HISTORY;

typedef struct opT {
    int opcode;
    char operation[7];
    int format;
    struct opT* link;
} opNode;

// GLOBAL VARIANTs
HISTORY* hisHead;
opNode** opTable;
char* MEMORY;






/**************************************************
 **************************************************
 **************************************************/
void init();
int findCmd(char*);
char* removeSpace(char*);


/**************************************************
 ***************** SHELL COMMANDS *****************
 **************************************************/
void help();                    // 0x00
void directory();               // 0x01
void history();                 // 0x02
void quit();                    // 0x03

void addHistory(char*);



/**************************************************
 **************** MEMORY COMMANDS *****************
 **************************************************/
int dump(char* start, char* end, int type);                  // 0x10 ~ 0x12
int edit(char* address, char *value);              // 0x13 ~ 0x14
int fill(char* start, char* end, char *value);     // 0x15
void reset();                                       // 0x16

int validAddr(char* addr);
int validAddrRange(char* start, char* end);
int strToHex(char* param);
int isHex(char ch);

/**************************************************
 ************* OPCODE TABLE COMMANDS **************
 **************************************************/
int opcode(char* mnemonic);                  // 0x20
void opcodeList();              // 0x21

int readOpTable();
int hashFunction(char* op);
