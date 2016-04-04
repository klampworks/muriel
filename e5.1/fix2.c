#include <fcntl.h>
#include <kvm.h>
#include <limits.h>
#include <nlist.h>
#include <stdio.h>
#include <sys/types.h>

#define SIZE 0x30

unsigned char nop_code[] = "\x90\x90";

int main()
{
    int i, jns_offset, call_offset;
    char errbuf[_POSIX2_LINE_MAX];
    kvm_t *kd;
    struct nlist nl[] = {{NULL}, {NULL}, {NULL}};
    unsigned char hello_code[SIZE], call_operand[4];

    kd = kvm_openfiles(NULL, NULL, NULL, O_RDWR, errbuf);
    if (!kd) {
        fprintf(stderr, "ERROR, %s\n", errbuf);
        exit(1);
    }

    n1[0].n_name = "hello";
    n1[1].n_name = "uprintf";

    if (kvm_nlist(kd, nl) < 0) {
        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        exit(1);
    }

    if (!n1[0].n_value) {
        fprintf(stderr, "ERROR: Symbol %s not found.\n", n1[0].n_name);
        exit(1);
    }

    if (!n1[1].n_value) {
        fprintf(stderr, "ERROR: Symbol %s not found.\n", n1[1].n_name);
        exit(1);
    }

    if (kvm_read(kd, n1[0].n_value, hello_code, SIZE) < 0) {
        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        exit(1);
    }

    const unsigned char jns = 0x79;
    const unsigned char call = 0xe8;
    for (i = 0; i < SIZE; ++i) {
        if (hello_code[i] == jns)
            jns_offset = i;
        if (hello_code[i] == call)
            call_offset = i;
    }

    *(unsigned long *)&call_operand[0] = n1[1].n_value -
        (n1[0].n_value + call_offset + 5);

    if (kvm_write(kd, n1[0].n_value + jns_offset, nop_code,
        sizeof(nop_code) - 1) < 0) {
        
        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        exit(1);
    }

    if (kvm_write(kd, n1[0].n_value + call_offset + 1, call_operand,
        sizeof(call_operand)) < 0) {

        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        exit(1);
    }

    if (kvm_close(kd) < 0) {
        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        exit(1);
    }

    exit(0);
}
