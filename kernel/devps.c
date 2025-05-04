#include "types.h"
#include "riscv.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "defs.h"
#include "file.h"

#define NULL 0
#define ZERO 1
#define URANDOM 2
#define NULLSTAT 3

#define BUF_SIZE 512
char null_buf[BUF_SIZE];

struct pseudo {
  struct spinlock lock;
  uint64 seed;
  uint64 cnt;
};

static struct pseudo devps;

uint8 urandom_next() {
  devps.seed = devps.seed * 6364136223846793005ull + 1;
  return (uint8)(devps.seed >> 56);
}

int 
ps_read(int user_dst, uint64 dst, int n, short minor) 
{
  int r = -1;
  switch (minor) {
    case NULL:
      r = 0;
      break;
    case ZERO:
      int cpy_n = n;
      uint64 cur_dst = dst;
      while (cpy_n > 0) {
        int chunk = cpy_n < BUF_SIZE ? cpy_n : BUF_SIZE;
        if (either_copyout(user_dst, cur_dst, null_buf, chunk) < 0) {
          r = -1;
          break;
        }
        cur_dst += chunk;
        cpy_n -= chunk;
      }
      r = n;
      break;
    case URANDOM:
      acquire(&devps.lock);
      for (int i = 0; i < n; i++) {
        uint8 new = urandom_next();
        if (either_copyout(user_dst, dst + i, &new, 1) < 0) {
          r = -1;
          release(&devps.lock);
          break;
        }
      }
      release(&devps.lock);
      r = n;
      break;
    case NULLSTAT:
      if (n != sizeof(uint64)) {
        r = -1;
        break;
      }
      acquire(&devps.lock);
      if (either_copyout(user_dst, dst, &devps.cnt, n) < 0) {
        r = -1;
        release(&devps.lock);
        break;
      }
      release(&devps.lock);
      r = n;
      break;
    default:
      break;
  }
  
  return r;
}

int 
ps_write(int user_src, uint64 src, int n, short minor)
{
  int r = -1;
  switch (minor) {
    case NULL:
      r = n;
      break;
    case ZERO:
      r = -1;
      break;
    case URANDOM:
      if (n != sizeof(uint64)) {
        r = -1;
        break;
      }
      uint64 nseed;
      if (either_copyin(&nseed, user_src, src, n) < 0) {
        r = -1;
        break;
      }
      acquire(&devps.lock);
      devps.seed = nseed;
      release(&devps.lock);
        r = n;
        break;
    case NULLSTAT:
      acquire(&devps.lock);
      devps.cnt += n;
      release(&devps.lock);
      r = n;
      break;
    default:
      break;
  }

  return r;
}

void 
devps_init(void) 
{
    initlock(&devps.lock, "dev");
    for (int i = 0; i < BUF_SIZE; ++i)
      null_buf[i] = 0;
    devps.seed = 1337; devps.cnt = 0;
    devsw[PS_DEV].read = ps_read;
    devsw[PS_DEV].write = ps_write;
}