#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define START_POINT 1970

#define DAY_YR 365
#define HR_DAY 24
#define MIN_HR 60
#define SEC_MIN 60
#define NS_MS 1000000
#define NS_SEC 1000000000

int days_mth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, };

int leap_year(int year) {
    if (year % 400 == 0) 
        return 1;
    if (year % 100 == 0) 
        return 0;
    if (year % 4 == 0) 
        return 1;
    return 0;
}

int main(int argc, char *argv[]) {
    uint64 time = rtc();
    uint32 nsecs = time % NS_SEC;
    uint32 msecs = nsecs / NS_MS;
    uint64 secs = time / NS_SEC % SEC_MIN;
    uint32 mins = time / SEC_MIN / NS_SEC % MIN_HR;
    uint32 hrs = time / MIN_HR / SEC_MIN / NS_SEC % HR_DAY;
    uint32 days = time / HR_DAY / MIN_HR / SEC_MIN / NS_SEC;
    uint32 yr = START_POINT;
    uint32 mth = 0;

    while (days >= DAY_YR + leap_year(yr)) {
        ++yr;
        days -= DAY_YR + leap_year(yr);
    }
    uint32 feb29 = 0;
    while (1) {
        if (mth == 1 && leap_year(yr) == 1) 
            feb29 = 1;
        if (days >= days_mth[mth] + feb29){
            days -= days_mth[mth] + feb29;
            mth++;
        }
        else break;
        feb29 = 0;
    }

    printf("Date: %d.%d.%d\n", days + 1, mth + 1, yr);
    printf("Time: %d:%d:%ld.", hrs, mins, secs);

    char msecs_str[4];
    msecs_str[3] = '\0';
    int i = 2;
    while (i >= 0) {
        msecs_str[i] = '0' + msecs % 10;
        msecs /= 10;
        i--;
    }
    printf("%s\n", msecs_str);
}