#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

extern int printf(char *fmt, ...);

const int isLog = 1;

struct file*
mutexalloc(void)
{
    struct file *f = filealloc();
    if (f == 0) {
        if (isLog)
            printf("log: Proc with pid %d failed at filealloc for mutex\n", myproc()->pid);
        return 0;
    }
    if (isLog)
        printf("log: Proc with pid %d succseded at filealloc at 0x%lx for mutex\n", myproc()->pid, (uint64)f);
    f->type = FD_MUTEX;
    f->mutex = (struct sleeplock *)kalloc();
    if (f->mutex == 0) {
        fileclose(f);
        if (isLog)
            printf("log: Proc with pid %d failed at kalloc for mutex file at 0x%lx\n", myproc()->pid, (uint64)f);
        return 0;
    }
    if (isLog)
        printf("log: Proc with pid %d kalloced mutex at addr 0x%lx, file at 0x%lx\n", myproc()->pid, (uint64)f->mutex, (uint64)f);
    initsleeplock(f->mutex, "mutex");
    return f;
}

void
mutexclose(struct file *f) 
{
    if (f == 0 || f->type != FD_MUTEX) {
        if (isLog)
            printf("log: tried closing not a mutex file at 0x%lx as mutex file\n", (uint64)f);
        return;
    }
    if (f->mutex->locked && f->mutex->pid == myproc()->pid) 
        releasesleep(f->mutex);
    if (isLog)
        printf("log: Proc with pid %d unlocked mutex, starting kfree for mutex at 0x%lx file at 0x%lx\n", myproc()->pid, (uint64)f->mutex, (uint64)f);
    kfree((char *)f->mutex);
    if (isLog)
        printf("log: Proc with pid %d kfreed mutex file at 0x%lx\n", myproc()->pid, (uint64)f);
    f->mutex = 0;
}