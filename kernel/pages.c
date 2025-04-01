#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

void print_pte(pagetable_t paget, int lvl, uint64 vab, int mask, uint64 buf_start, uint64 buf_end) {
    uint64 mult = 4096;
    if (lvl == 1)
        mult *= 512;
    if (lvl == 2) 
        mult *= 512 * 512;
    for (int i = 0; i < 512; ++i) {
        pte_t pte = paget[i];
        if (!(pte & PTE_V)) 
            continue;

        uint64 va = vab | ((uint64)i << PXSHIFT(lvl));
        if (va >= buf_end || (va + mult) <= buf_start)
            continue;
        int ni = 2, na = 15;
        uint64 di = 16;
        while (i / di != 0) {
            ni--;
            di *= 16;
        }
        di = 10;
        while (PTE2PA(pte) / di != 0) {
            na--;
            di *= 16;
        }

        char flags[8] = "_______\0";
        char flagsLits[8] = "RWXUGAD\0";
        uint64 pte_flags = PTE_FLAGS(pte);
        for (int j = 1; j < 8; ++j) {
            if (pte_flags & (1L << j)) 
                flags[j - 1] = flagsLits[j - 1];
        }
        
        if (lvl == 0 && (mask & 1) && (flags[6] != 'D'))
            continue;
        if (lvl == 0 && (mask & 2) && (flags[5] != 'A'))
            continue;

        for (int j = 0; j < 9 * (2 - lvl); ++j)
            printf(".");
        printf("0x");
        for (int j = 0; j < ni; ++j) printf("0");
        printf("%x -> ", i);
        printf("0x");
        for (int j = 0; j < na; ++j) printf("0");
        printf("%lx ", PTE2PA(pte));
        printf("%s\n", flags);

        if (lvl != 0) 
            print_pte((pagetable_t)PTE2PA(pte), lvl - 1, va, mask, buf_start, buf_end);
    }
}

void strip_flags(pagetable_t paget, int lvl, uint64 vab, int mask, uint64 buf_start, uint64 buf_end) {
    uint64 mult = 4096;
    if (lvl == 1)
        mult *= 512;
    if (lvl == 2) 
        mult *= 512 * 512;
    for (int i = 0; i < 512; ++i) {
        pte_t pte = paget[i];
        if (!(pte & PTE_V)) 
            continue;

        uint64 va = vab | ((uint64)i << PXSHIFT(lvl));
        if (va >= buf_end || (va + mult) <= buf_start)
            continue;

        if (lvl == 0 && (mask & 1) && (pte & (1L << 7))) 
            paget[i] &= ~(1L << 7);
        if (lvl == 0 && (mask & 2) && (pte & (1L << 6))) 
            paget[i] &= ~(1L << 6);

        if (lvl != 0) 
            strip_flags((pagetable_t)PTE2PA(pte), lvl - 1, va, mask, buf_start, buf_end);
    }
}


uint64
sys_pages(void)
{
  uint64 buf, len;
  int mask;

  argaddr(0, &buf);
  argaddr(1, &len);
  argint(2, &mask);
  if (mask > 3 || mask < 0)
    return -1;
  
  if (buf == 0 || len == 0) {
    buf = 0;
    len = MAXVA - 1;
  }

  pte_t *pte = myproc()->pagetable;
  printf("PAGETABLE %p\n", pte);
  print_pte(pte, 2, 0, mask, buf, buf + len);
  return 0;
}

uint64
sys_strip_flags(void)
{
    uint64 buf, len;
    int mask;
  
    argaddr(0, &buf);
    argaddr(1, &len);
    argint(2, &mask);
    if (mask > 3 || mask < 0)
      return -1;
    
    if (buf == 0 || len == 0) {
      buf = 0;
      len = MAXVA - 1;
    }

    pte_t *pte = myproc()->pagetable;
    strip_flags(pte, 2, 0, mask, buf, buf + len);
    return 0;
}