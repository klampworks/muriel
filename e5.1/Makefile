KMOD=hello
SRCS=hello.c

.include <bsd.kmod.mk>

fix: fix.c fix2.c
	gcc fix.c -o fix -lkvm
	gcc fix2.c -o fix2 -lkvm
