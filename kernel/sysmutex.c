#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "file.h"
#include "memlayout.h"
#include "proc.h"
#include "sleeplock.h"

uint64
sys_mutex(void)
{
    struct file *f = mutexalloc();
    int fd = fdalloc(f);
    if (f == 0)
        return -1;
    if (fd < 0) {
        fileclose(f);
        return -1;
    }
    return fd;
}

uint64 
sys_mutex_lock(void)
{
    int fd;
    struct file *f;
    argint(0, &fd);
    if (fd < 0 || (f = myproc()->ofile[fd]) == 0 || f->type != FD_MUTEX)
        return -1;
    acquiresleep(f->mutex);
    return 0;
}

uint64 
sys_mutex_unlock(void)
{
    int fd;
    struct file *f;
    argint(0, &fd);
    if (fd < 0 || (f = myproc()->ofile[fd]) == 0 || f->type != FD_MUTEX)
        return -1;
    if (f->mutex->pid != myproc()->pid)
        return -1;
    releasesleep(f->mutex);
    return 0;
}