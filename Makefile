#
PROG=	ppiswitch	
BINDIR=	/usr/local/sbin
MAN8=ppiswitch.8

SRCS=	ppiswitch.c circuit.c 
NO_OBJ=	yes
LDADD=	-lm

BINOWN= root
BINGRP= operator
BINMODE=4550

SCRIPTS=	compreboot ppiswitch
SCRIPTSDIR=	/usr/local/sbin
SCRIPTSDIR_ppiswitch=	/usr/local/etc/rc.d
SCRIPTSOWN=	root
SCRIPTSGRP=	wheel
SCIPTSMODE=	755
CLEANFILES=	${SCRIPTS}
#LINKS=  ${DESTDIR}/bin/test ${DESTDIR}/bin/[

FILES=	compreboot.conf	
#FILES+=	circuit.txt	
FILESNAME_compreboot.conf=	compreboot.conf-sample
FILESDIR_compreboot.conf=	/usr/local/etc
FILESDIR_circuit.txt=	/usr/local/share/examples/ppiswitch
FILESNAME_circuit.txt=	circuit.txt	

.include <bsd.prog.mk>
