// ===============================================
// Main
//
// Runtime library 'Main' function is the real 
// entry point for a cmsx program. Main calls user 
// supplied 'main' function, and then the exit 
// system call function to exit the program.
//
// On entry ax=argc, bx=argv.
// User supplied main function should return the 
// exit code of the program in ax.
// ===============================================

        EXTERN Main

Main    FUNC
        CALL $0,main
        CALL $0,exit
Main    ENDF
