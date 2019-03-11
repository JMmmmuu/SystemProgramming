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

// MACROs
#define MEMORY_SIZE 1<<10
#define COMMAND_SIZE 30

// STRUCTUREs
typedef struct hist {
    char* hist;
    struct hist* link;
} HISTORY;




// GLOBAL VARIANTs





/**************************************************
 **************************************************
 **************************************************/
void init();
int findCmd(char*);
char* removeSpace(char*);


/**************************************************
 ***************** SHELL COMMANDS *****************
 **************************************************/
void help();        // 0x00
void directory();   // 0x01
void history();     // 0x02
void quit();        // 0x03

void addHistory(char*);



/**************************************************
 **************** MEMORY COMMANDS *****************
 **************************************************/
void dump();        // 0x10 ~ 0x12
void edit();        // 0x13 ~ 0x14
void fill();        // 0x15
void reset();       // 0x16


/**************************************************
 ************* OPCODE TABLE COMMANDS **************
 **************************************************/
void opcode();      // 0x20
void opcodeList();  // 0x21
