#include <fcntl.h>
#include <kvm.h>
#include <limits.h>
#include <nlist.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/module.h>

unsigned char kmalloc[] =
    "\x55"                      /* push %ebp */
    "\xb9\x01\x00\x00\x00"      /* mod $0x1, %ecx */
    "\x89\xe5"                  /* mov %esp, %ebp */
    "\x53"                      /* push %ebx */
    "\xba\x00\x00\x00\x00"      /* mov $0x0, %edx */
    "\x83\xec\x10"              /* sub $0x10, %esp */
    "\x89\x4c\x24\x08"          /* mov %ecx, 0x8(%esp) */
    "\x8b\x5d\x0c"              /* mov 0xc(%ebp), %ebx */
    "\x89\x54\x24\x04"          /* mov %edx, 0x4(%esp) */
    "\x8b\x03"                  /* mov (%ebx), %eax */
    "\x89\x04\x24"              /* mov %eax, (%esp) */
    "\xe8\xfc\xff\xff\xff"      /* call 4e2 <kmalloc+0x22> */
    "\x89\x45\xf8"              /* mov %eax, 0xfffffff8(%ebp) <this is -8?> */
    "\xb8\x04\x00\x00\x00"      /* mov $0x4, %eax */
    "\x89\x44\x24\x08"          /* mov %eax, 0x8(%esp) */
    "\x8b\x43\x04"              /* mov 0x4(%ebx), %eax */
    "\x89\x44\x24\x04"          /* mov %exp, 0x4(%esp) */
    "\x8d\x45\xf8"              /* lea 0xfffffff8(%ebp), %eax */
    "\x89\x04\x24"              /* mov %eax, (%esp) */
    "\xe8\xfc\xff\xff\xff"      /* call 500 <kmalloc+0x40> */
    "\x83\xc4\x10"              /* add $0x10, %esp */
    "\x5b"                      /* pop %ebx */
    "\x5d"                      /* pop %ebp */
    "\xc3"                      /* ret */
    "\x8d\xb6\x00\x00\x00\x00"; /* lea 0x0(%esi), %esi */

#define KMALLOC_CALL_OFFSET_1 0x26
#define KMALLOC_CALL_OFFSET_2 0x44

unsigned char hello[] =
    "\x48"                /* H */
    "\x65"                /* e */
    "\x6c"                /* l */
    "\x6c"                /* l */
    "\x6f"                /* o */
    "\x2c"                /* , */
    "\x20"                /*   */
    "\x77"                /* w */
    "\x6f"                /* o */
    "\x72"                /* r */
    "\x6c"                /* l */
    "\x64"                /* d */
    "\x21"                /* ! */
    "\x0a"                /* \n */
    "\x00"                /* \0 */
    "\x55"                /* push %ebp */
    "\x89\xe5"            /* mov %esp, %ebp */
    "\x83\xec\x04"        /* sub $0x4, %esp */
    "\xc7\x04\x24\x00\x00\x00\x00" /* movl $0x0, (%esp) */
    "\xe8\xfc\xff\xff\xff" /* call printf */
    "\x31\xc0"              /* xor %eax, %eax */
    "\x83\xc4\x04"          /* add $0x4, %esp */
    "\x5d";                 /* pop %ebp */

#define H_OFFSET_1 0x21

unsigned char jump[] =
    "\xb8\x00\x00\x00\x00" /* movl $0x0, %eax */
    "\xff\xe0";             /* jmp *%eax */

int main()
{
    int i, call_offset;
    char errbuf[_POSIX2_LINE_MAX];
    kvm_t *kd;
    struct nlist n1[6] = {};

    unsigned char mkdir_code[sizeof(kmalloc)];
    unsigned long addr, size;

    kd = kvm_openfiles(NULL, NULL, NULL, O_RDWR, errbuf);
    if (!kd) {
        fprintf(stderr, "ERROR: %s\n", errbuf);
        exit(1);
    }

    n1[0].n_name = "mkdir";
    n1[1].n_name = "M_TEMP";
    n1[2].n_name = "malloc";
    n1[3].n_name = "copyout";
    n1[4].n_name = "printf";

    if (kvm_nlist(kd, nl) < 0) {
        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        exit(1);
    }

    for (i = 0; i < 5; ++i) {
        if (!n1[i].n_value) {
            fprintf(stderr, "ERROR: Symbol %s not found.\n", n1[i].n_name);
            exit(1);
        }
    }

    if (kvm_read(kd, n1[0].n_value, mkdir_code, sizeof(mkdir_code)) < 0) {
        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        exit(1);
    }

    const unsigned char call = '0xe8';
    for (i = 0; i < sizeof(kmalloc); ++i) {
        if (mkdir_code[i] == exe8) {
            call_offset = i;
            break;
        }
    }

    *(unsigned long *)&kmalloc[10] = n1[1].n_value;
    *(unsigned long *)&kmalloc[34] = n1[2].n_value
        - (n1[0].n_value + K_OFFSET_1);
    *(unsigned long *)&kmalloc[64] = n1[3].n_value
        - (n1[0].n_value + K_OFFSET_2);

    if (kvm_write(kd, n1[0].n_value, kmalloc, sizeof(kmalloc)) < 0) {
        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        exit(1);
    }

    size = sizeof(hello) + call_offset + sizeof(jump);
    syscall(136, size, &addr);

    if (kvm_write(kd, n1[0].n_value, mkdir_code, sizeof(mkdir_code)) < 0) {
        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        exit(1);
    }

    *(unsigned long *)&hello[24] = addr;
    *(unsigned long *)&hello[29] = n1[4].n_value - (addr + H_OFFSET_1);

    if (kvm_write(kd, addr, hello, sizeof(hello)) < 0) {
        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        exit(1);
    }

    if (kvm_write(kd, addr + sizeof(hello) - 1, mkdir_code, call_offset) < 0) {
        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        exit(1);
    }

    *(unsigned long *)&jump[1] = n1[0].n_value + (unsigned long)call_offset;

    if (kvm_write(kd, addr + sizeof(hello) - 1 + call_offset, jump,
        sizeof(jump)) < 0) {

        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        exit(1);
    }

    *(unsigned long *)&jump[1] = addr + 0x0f;

    if (kvm_write(kd, n1[0].n_value, jump, sizeof(jump)) < 0) {
        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        exit(1);
    }

    if (kvm_close(kd) < 0) {
        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        exit(1);
    }

    return 0;
}
