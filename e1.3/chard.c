#include <sys/param.h>
#include <sys/proc.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/conf.h>
#include <sys/uio.h>

d_open_t open;
d_close_t close;
d_read_t read;
d_write_t write;

static struct cdevsw chard_cdevsw = {
    .d_version = D_VERSION,
    .d_open = open,
    .d_close = close,
    .d_read = read,
    .d_write = write,
    .d_name = "chard"
};

static char buf[512+1];
static size_t len;

int open(struct cdev *dev, int flag, int otyp, struct thread *td)
{
    memset(&buf, '\0', 513);
    len = 0;
    return 0;
}

int close(struct cdev *dev, int flag, int otyp, struct thread *td)
{
    return 0;
}

int write(struct cdev *dev, struct uio *uio, int ioflag)
{
    int error = 0;
    error = copyinstr(uio->uio_iov->iov_base, &buf, 512, &len);

    if (error)
        printf("Write to chard failed.");

    return error;
}

int read(struct cdev *dev, struct uio *uio, int ioflag)
{
    int error = 0;

    if (len <= 0)
        error = -1;
    else
        copystr(&buf, uio->uio_iov->iov_base, 513, &len);

    return error;
}

static struct cdev *sdev;

static int load(struct module *module, int cmd, void *arg)
{
    int error = 0;

    switch(cmd) {
    case MOD_LOAD:
        sdev = make_dev(&chard_cdevsw, 0, UID_ROOT, GID_WHEEL, 0600, "chard");
        printf("Character device loaded.");
        break;
    case MOD_UNLOAD:
        destroy_dev(sdev);
        printf("Character device unloaded.");
        break;
    default:
        error = EOPNOTSUPP;
        break;
    }

    return error;
}

DEV_MODULE(chard, &load, NULL);
