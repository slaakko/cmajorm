// =================================================
// echo
//
// Prints program arguments to the standard output.
// On entry ax=argc and bx=argv.
// On exit ax contains the exit code of the program.
// =================================================

        EXTERN main

.CODE

main    FUNC
        STO fp,sp,0
        SET fp,sp
        INCL sp,1*8
        SET $0,ax
        SET $1,bx
        JMP @1
@0      BNP $0,@2
        LDOU ax,$1,0
        CALL $2,puts
        BN ax,@3
        CALL $2,putnl
        BN ax,@3
@1      ADDU $1,$1,8
        SUB $0,$0,1
        JMP @0
@2      SET ax,0
        JMP @4
@3      SET ax,1
@4      SET sp,fp
        LDO fp,sp,0
        RET
main    ENDF
