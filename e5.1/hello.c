#include <sys/types.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/module.h>
#include <sys/sysent.h>
#include <sys/kernel.h>
#include <sys/systm.h>

static int hello(struct thread *td, void *args)
{
    int i;
    for (i = 0; i < 10; ++i)
        printf("Bunnies are the best.\n");

    return 0;
}

static struct sysent hello_sysent = {
    0,
    hello
};

static int offset = NO_SYSCALL;

static int load(struct module *module, int cmd, void *arg)
{
    int error = 0;

    switch (cmd) {
        case MOD_LOAD:
            printf("System call hello loaded at %d\n", offset);
            break;
        case MOD_UNLOAD:
            printf("System call hello unloaded from %d\n", offset);
            break;
        default:
            error = EOPNOTSUPP;
            break;
    }

    return error;
}

SYSCALL_MODULE(hello, &offset, &hello_sysent, load, NULL);
