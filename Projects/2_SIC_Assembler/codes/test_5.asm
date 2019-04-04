.       This is veryvery veryveryveryvery long code!!
COPY    START   0
FIRST   STL     RETADR
        LDB     #FIRST
        BASE   LENGTH
CLOOP   +JSUB  RDREC
        LDA    LENGTH
        COMP   #0
        JEQ    ENDFIL
        +JSUB  WRREC
        J      CLOOP
ENDFIL  LDA    EOF
        STA    BUFFER
        LDA    #3
        STA    LENGTH
        +JSUB  WRREC
        J      @RETADR
EOF     BYTE   C'EOF'
RETADR  RESW   1
LENGTH  RESW   1
BUFFER  RESB   4096
.
.       SUBROUTINE TO READ RECORD INTO BUFFER
.
RDREC   CLEAR   X
        CLEAR   A
        CLEAR   S
        +LDT    #4096
RLOOP   TD      INPUT
        JEQ     RLOOP
        RD      INPUT
        COMPR   A, S
        JEQ     EXIT
        STCH    BUFFER, X
        TIXR    T
        JLT     RLOOP
EXIT    STX     LENGTH
        RSUB
INPUT   BYTE    X'F1'
.
.       SUBROUTINE TO WRITE RECORD FROM BUFFER
.
WRREC   CLEAR   X
        LDT     LENGTH
WLOOP   TD       OUTPUT
        JEQ     WLOOP
        LDCH    BUFFER, X
        WD      OUTPUT
        TIXR    T
        JLT     WLOOP
        RSUB
OUTPUT  BYTE    X'05'


.
.       MY COCE
.  
ADD1    ADD     WLOOP
ADD2    ADD    BUFFER
ADD3    ADDF   LENGTH
        ADDR   A,T
        FLOAT
        HIO
YEAH    FIX
GOOOD   AND     EXIT
JJUM    JEQ     #RLOOP
JJUM2   +JLT    ENDFIL
LOOAD   LDL     @YEAH
        NORM

        MUL     RETADR
        MULF    CLOOP

        SIO

        RMO     S, T
        RSUB
SYM1    STCH    LOOAD
GNU     SSK     GOOOD
QUEU    STT     ADD1

FFF     RMO     A
ELAP    STX     OUTPUT
UNIT    +STF    CLOOP
KKK     SIO

TRYY    SHIFTL  A, 4
XOXO    DIV     SYM1

SMALL   RESB    4
YEEAH   BYTE    X'1359'
LOL     WORD    49902
ZEAT    LDT     @10A2
MACHINE MULR    T, A
FUUQ    OR      LOOAD
NET     STSW    JJUM2
XSKO    TIO
QUUO    RMO     T, PC
SSIXE   STA     FFF
WOOW    WORD    99383
JUNG    FLOAT
YU      JGT     YEEAH
SEOK    RD      GOOOD
        END     FIRST

