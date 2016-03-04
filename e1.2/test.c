#include <stdio.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/module.h>

int main(int argc, char *argv[])
{
    int syscall_num;
    struct module_stat stat;

    if (argc != 2) {
        printf("Usage:\n%s <string>\n", argv[0]);
        exit(1);
    }

    stat.version = sizeof(stat);
    modstat(modfind("my_sc"), &stat);
    syscall_num = stat.data.intval;

    return syscall(syscall_num, argv[1]);
}
