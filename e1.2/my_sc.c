#include <sys/types.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/module.h>
#include <sys/sysent.h>
#include <sys/kernel.h>
#include <sys/systm.h>

struct my_sc_args {
    char *str;
};

static int my_sc(struct thread *td, void *syscall_args)
{
    struct my_sc_args *uap;
    uap = (struct my_sc_args *)syscall_args;

    printf("%s\n", uap->str);

    return 0;
}

static struct sysent my_sc_sysent = {
    1,
    &my_sc
};

static int offset = NO_SYSCALL;

static int load(struct module *module, int cmd, void *arg)
{
    int error = 0;

    switch (cmd) {
        case MOD_LOAD:
            printf("my_sc module loaded at offset %d\n", offset);
            break;
        case MOD_UNLOAD:
            printf("my_sc module unloaded at offset %d\n", offset);
            break;
        default:
            error = EOPNOTSUPP;
            break;
    }

    return error;
}

SYSCALL_MODULE(my_sc, &offset, &my_sc_sysent, &load, NULL);
