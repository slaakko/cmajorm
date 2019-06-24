// ===============================================
// Main
//
// Runtime library 'Main' function is the real 
// entry point for a cmsx program. Main calls user 
// supplied 'main' function, and then the exit 
// system call function to exit the program.
//
// On entry ax=argc, bx=argv, cx=envp
// User supplied main function should return the 
// exit code of the program in ax.
// ===============================================

        EXTERN Main

Main    FUNC
        STO fp,sp,0
        SET fp,sp
        INCL sp,#8
        CALL $0,main
        CALL $0,exit
        SET sp,fp
        LDO fp,sp,0
        RET
Main    ENDF
