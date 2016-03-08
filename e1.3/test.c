#include <stdio.h>
#include <fcntl.h>
#include <paths.h>
#include <string.h>
#include <sys/types.h>

static char buf[512+1];

int main(int argc, char **argv)
{
    int kernel_fd;
    int len;

    if (argc != 2) {
        printf("Usage:\n%s <string>\n", argv[0]);
        exit(1);
    }

    if ((kernel_fd = open("/dev/chard", O_RDWR)) == -1) {
        perror("/dev/chard");
        exit(1);
    }

    if ((len = strlen(argv[1]) + 1) > 512) {
        printf("ERROR: String too long.\n");
        exit(1);
    }

    if (write(kernel_fd, argv[1], len) == -1) {
        perror("write()");
    } else {
        printf("Write '%s' to device /dev/chard.\n", argv[1]);
    }

    if (read(kernel_fd, buf, len) == -1) {
        perror("read()");
    } else {
        printf("Read '%s' from device /dev/chard.\n", buf);
    }

    if ((close(kernel_fd)) == -1) {
        perror("close()");
        exit(1);
    }

    return 0;
}
