// ===================================================
// hello
//
// Writes text "Hello, world!" to the standard output.
// On entry ax=argc and bx=argv.
// On exit ax contains the exit code of the program.
// ===================================================

            EXTERN main
     
.CODE
     
main        FUNC
            STO fp,sp,0
            SET fp,sp
            INCL sp,1*8
            LDOU ax,hello
            CALL $0,puts
            BN ax,@0
            SET ax,0
            JMP @1
@0          SET ax,1
@1          SET sp,fp
            LDO fp,sp,0
            RET
main        ENDF

.DATA

hello       OCTA message
message     BYTE "Hello, world!",10,0
