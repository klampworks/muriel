#include <fcntl.h>
#include <kvm.h>
#include <limits.h>
#include <nlist.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/sysent.h>

void usage();

int main(int argc, char **argv)
{
    char errbuf[_POSIX2_LINE_MAX];
    kvm_t *kd;
    struct nlist n1[3] = {};

    unsigned long addr;
    int callnum;
    struct sysent call;

    if (argc < 3) {
        usage();
        return 1;
    }

    n1[0].n_name = "sysent";
    n1[1].n_name = argv[1];

    callnum = strtol(argv[2], NULL, 10);

    printf("Checking system call %d: %s\n", callnum, argv[1]);

    kd = kvm_openfiles(NULL, NULL, NULL, O_RDWR, errbuf);
    if (!kd) {
        fprintf(stderr, "ERROR: %s\n", errbuf);
        return 1;
    }

    if (kvm_nlist(kd, n1) < 0) {
        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        return 1;
    }

    if (n1[0].n_value) {
        printf("%s[] is 0x%x at 0x%1x\n", n1[0].n_name, n1[0].n_type,
            n1[0].n_name);
    } else {
        fprintf(stderr, "ERROR: %s not found (ver weird...)\n", n1[0].n_name);
        return 1;
    }

    if (!n1[1].n_value) {
        fprintf(stderr, "ERROR: %s not found\n", n1[1].n_name);
        return 1;
    }

    addr = n1[0].n_value + callnum * sizeof(struct sysent);

    if (kvm_read(kd, addr, &call, sizeof(struct sysent)) < 0) {
        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        return 1;
    }

    printf("sysent[%d] is at 0x%1x and its sy_call member points to "
        "%p\n", callnum, addr, call.sy_call);

    if ((uintptr_t)call.sy_call != n1[1].n_value) {
        printf("ALERT! It should point to 0x%1x instead\n", n1[1].n_value);

        if (argv[3] &&strncmp(argv[3], "fix", 3) == 0) {
            printf("Fixing...");

            call.sy_call = n1[1].n_value;
            if (kvm_write(kd, addr, &call, sizeof(struct sysent)) < 0) {
                fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
                return 1;
            }

            printf("done");
        }
    }

    if (kvm_close(kd) < 0) {
        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        return 1;
    }

    return 0;
}

void usage()
{
    fprintf(stderr, "Usage:\ncheckcall [system call function] "
        "[call number] <fix>\n\n");
    fprintf(stderr, "For a list of system call numbers see "
        "/sys/sys/syscall.h\n");
}
