        .LINK

        EXTERN   Main

        .CODE

Main FUNC
        STO      fp,sp
        SET      fp,sp
        INCL     sp,#0020
@0      LDOU     $0,fp,#08
        LDOU     $1,fp,#10
        LDOU     $2,fp,#18
        STOU     $0,fp,#48
        STOU     $1,fp,#50
        STOU     $2,fp,#58
        CALL     1,main
        STOU     ax,fp,#48
		CALL     1,exit
        SET      sp,fp
        LDO      fp,sp
        RET     
Main ENDF
