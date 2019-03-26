/*************************************************
 ************** System Programming ***************
 ****************** 20171690.h *******************
 ******************** Yuseok *********************
 ******************* ~ 190408 ********************
 *************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

// MACROs
#define MEMORY_SIZE 1<<20
#define COMMAND_SIZE 100
#define OPCODE "opcode.txt"
#define ONE_BYTE (unsigned char)0xFF

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

typedef struct symT {
    int addr;
    char symbol[10];
    struct symT* link;
} symNode;

typedef struct __token {
    char* r1, *r2;
    char* symbol;
    char* operation;
    char* target;
    char* tmp;
} asmToken;

// GLOBAL VARIANTs
HISTORY* hisHead;
opNode** opTable;
symNode** SYMTAB;
unsigned char* MEMORY;
int END_ADDR;


/**************************************************
 ******************* FUNCTIONS ********************
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
int type(char*);                // 0x04

void addHistory(char*);



/**************************************************
 **************** MEMORY COMMANDS *****************
 **************************************************/
int dump(char* start, char* end, int type);                  // 0x10 ~ 0x12
int edit(char* address, char *value);              // 0x13 ~ 0x14
int fill(char* start, char* end, char *value);     // 0x15
void reset();                                       // 0x16

int validAddr(int addr);
int validAddrRange(int start, int end);
int strToHex(char* param);
int isHex(char ch);

/**************************************************
 ************* OPCODE TABLE COMMANDS **************
 **************************************************/
int opcode(char* mnemonic);                  // 0x20
int opcodeList();              // 0x21

int readOpTable();
int hashFunction(char* op);

/**************************************************
 ******************** ASSEMBLER *******************
 **************************************************/
int assemble(char* filename);               // 0x30
int symbol();                               // 0x31

int pass1(FILE* fp);
int pass2(FILE* fp);
int isStr(char* Str);
int tokenizeAsmFile(char*** token, char* input);

int removeSpaceAroundComma(char* input);
int isWhiteSpace(char ch);
