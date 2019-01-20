// ===============================================
// System call functions.
// ===============================================

            EXTERN exit,wait,fork,write

sys_exit    IS 0
sys_wait    IS 1
sys_fork    IS 2
sys_write   IS 3

// ===============================================
// exit
//
// Exits the program.
// On entry ax=exit code of the program.
// ===============================================

exit        FUNC
            TRAP 0,sys_exit,0
exit        ENDF

// ===============================================
// wait
//
// Waits any child process to terminate.
// On entry ax=address of a ushort variable that 
// will contain the exit code of the child.
// ax can be 0, if not interested in the exit
// code. Returns the PID of the child exited in 
// ax, or -1 if the call failed.
// ===============================================

wait        FUNC
            TRAP 0,sys_wait,0
            RET
wait        ENDF

// ===============================================
// fork
//
// Clones a process.
// Returns 0 to the child process in ax, and
// PID of the child process to the parent.
// ===============================================

fork        FUNC
            TRAP 0,sys_fork,0
            RET
fork        ENDF

// ===============================================
// write
//
// Writes a buffer of bytes to a stream.
// On entry ax=file descriptor of the stream to 
// write to, bx=address of the buffer, and 
// cx=number of bytes to write.
// Returns the number of bytes written in ax, or 
// -1 if an error occurred.
// ===============================================


write       FUNC
            TRAP 0,sys_write,0
            RET
write       ENDF
