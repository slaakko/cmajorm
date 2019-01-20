// ========================
// String utilities and I/O
// ========================

        EXTERN strlen,puts,putsf,putnl,putnlf,newline

.CODE

// ===============================================
// strlen
//
// Returns the length of a null-terminated string.
// On entry ax=address of the string.
// Returns the length of the string in ax.
// ===============================================
        
strlen  FUNC
        STO fp,sp,0
        SET fp,sp
        INCL sp,1*8
        SET $0,ax
        SET ax,0
@1      LDB $1,$0,0
        BZ $1,@2
        ADD ax,ax,1
        ADD $0,$0,1
        JMP @1
@2      SET sp,fp
        LDO fp,sp,0
        RET
strlen  ENDF

// ===============================================
// puts
//
// Writes a null-terminated string to the standard
// output.
// On entry ax=address of the string.
// Returns the number of characters written in ax, 
// or -1 if an error occurred.
// ===============================================

puts    FUNC
        STO fp,sp,0
        SET fp,sp
        INCL sp,1*8
        SET bx,1
        CALL $0,putsf
        SET sp,fp
        LDO fp,sp,0
        RET
puts    ENDF

// ===============================================
// putsf
//
// Writes a null-terminated string to a stream.
// On entry ax=address of the string,
// bx=file descriptor of the stream.
// Returns the number of characters written in ax,
// or -1 if an error occurred.
==================================================

putsf   FUNC
        STO fp,sp,0
        SET fp,sp
        INCL sp,1*8
        SET $1,bx
        SET $0,ax
        CALL $2,strlen
        SET cx,ax
        SET bx,$0
        SET ax,$1
        CALL $2,write
        SET sp,fp
        LDO fp,sp,0
        RET
putsf   ENDF

// ===============================================
// putnl
//
// Writes a newline to the standard output.
// Returns the number of characters written in ax,
// or -1 if an error occurred.
// ===============================================
 

putnl   FUNC
        STO fp,sp,0
        SET fp,sp
        INCL sp,1*8
        SET ax,1
        CALL $0,putnlf
        SET sp,fp
        LDO fp,sp,0
        RET
putnl   ENDF

// ===============================================
// putnlf
//
// Writes a newline to a stream.
// On entry ax=file descriptor of the stream.
// Returns the number of characters written in ax,
// or -1 if an error occurred.
// ===============================================

putnlf  FUNC
        STO fp,sp,0
        SET fp,sp
        INCL sp,1*8
        SET bx,ax
        LDOU ax,newline
        CALL $0,putsf
        SET sp,fp
        LDO fp,sp,0
        RET
putnlf  ENDF

.DATA

// ===============================================
// newline
//
// Contains the address of a string containing a 
// newline character.
// ===============================================


newline OCTA nl
nl      BYTE 10,0
