#include <sys/types.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/module.h>
#include <sys/sysent.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/malloc.h>

struct kmalloc_args {
    unsigned long size;
    unsigned long *addr;
};

static int kmalloc(struct thread *td, void *args)
{
    struct kmalloc_args *uap;
    uap = (struct kmalloc_args *)args;

    int error;
    unsigned long addr;

    MALLOC(addr, unsigned long, uap->size, M_TEMP, M_NOWAIT);
    error = copyout(&addr, uap->addr, sizeof(addr));

    return error;
}

static struct sysent kmalloc_sysent = {
    2,
    kmalloc
};

static int offset = NO_SYSCALL;

static int load(struct module *module, int cmd, void *arg)
{
    int error = 0;

    switch (cmd) {
        case MOD_LOAD:
            printf("System call kmalloc loaded at %d.\n", offset);
            break;
        case MOD_UNLOAD:
            printf("System call kmalloc unloaded from %d\n", offset);
            break;
         default:
            error = EOPNOTSUPP;
            break;
    }

    return error;
}

SYSCALL_MODULE(kmalloc, &offset, &kmalloc_sysent, load, NULL);
