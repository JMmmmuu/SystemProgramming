/*************************************************
 *************************************************
 ******************** Yuseok *********************
 ******************** 190000 *********************
 *************************************************/
#include "20171690.h"

int assemble(char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        // No input file
        printf("%s: No such file in the directory\n", filename);
        return 0;
    }

    return 1;
}

int symbol() {

    return 1;
}
