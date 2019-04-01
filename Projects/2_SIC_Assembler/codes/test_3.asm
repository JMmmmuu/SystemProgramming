. This is Comment
. Hi there It's yuseok

. Blank line

FIRST   STL     RETADR
        LDB     #LENGTH
        BASE    LENGTH

        COMPR   A, S
        +JSUB   WRREC
        J       @RETADR

RETADR  RESW    1
LENGTH  RESW    3
EOF     BYTE    C'EOFG'
CONST   WORD    498
ONE     WORD    1
BUFFER  RESB    4096

.   comment
.   good

RDREC   CLEAR   X
RLOOP   TD      INPUT
        COMP    ONE
        COMP    #293
        HIO


WRREC   CLEAR   X


        END     FIRST
