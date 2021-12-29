        .LINK

        EXTERN   Main

        .CODE

Main FUNC
        STO      fp,sp
        SET      fp,sp
        INCL     sp,48
@0      LDOU     $0,fp,0
        LDOU     $1,fp,8
        LDOU     $2,fp,16
        STT      $0,fp,24
        STOU     $1,fp,32
        STOU     $2,fp,40
        CALL     1,main
        STB      ax,fp,24
		CALL     1,exit
        SET      sp,fp
        LDO      fp,sp
        RET     
Main ENDF
