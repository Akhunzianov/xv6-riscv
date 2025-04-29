#include "types.h"
#include "memlayout.h"
#include "riscv.h"

#define RTC_REG(addr) (*(volatile uint32 *)(addr))

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
    uint32 low = rtc_low();
    uint32 high = rtc_high();
    return ((uint64)high << 32) + low;
}