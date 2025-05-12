#include "types.h"
#include "memlayout.h"
#include "riscv.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "defs.h"
#include "file.h"

#define RTC_REG(addr) (*(volatile uint32 *)(addr))

static struct spinlock rtc_lock;

void
rtcinit(void) 
{
  initlock(&rtc_lock, "rtc");
}

uint32
rtc_low(void)
{
    return RTC_REG(RTC_LOW);
}

uint32
rtc_high(void)
{
    return RTC_REG(RTC_HIGH);
}

uint64
rtc_read(void)
{
    acquire(&rtc_lock);
    uint32 low = rtc_low();
    uint32 high = rtc_high();
    release(&rtc_lock);
    return ((uint64)high << 32) + low;
}