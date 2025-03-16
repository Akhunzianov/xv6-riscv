#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/procinfo.h"

int main(void) {
  int n = ps_listinfo(0, 0); 
  if (n < 0) {
    fprintf(2, "ps: cannot get number of processes\n");
    exit(1);
  }
  if (n == 0) {
    fprintf(2, "ps: no processes?\n");
    exit(1);
  }

  struct procinfo *buf = malloc(n * sizeof(struct procinfo));
  if (!buf) {
    fprintf(2, "ps: malloc error\n");
    exit(1);
  }

  int rc = ps_listinfo(buf, n);
  while (rc < 0) { 
    free(buf);
    int bigger = n * 2;
    buf = malloc(bigger * sizeof(struct procinfo));
    if (!buf) {
      fprintf(2, "ps: malloc error\n");
      exit(1);
    }
    rc = ps_listinfo(buf, bigger);
  }

  for (int i = 0; i < rc; i++) {
    printf("pid=%d ppid=%d name=%s parent=%s state=%d\n",
           buf[i].pid, buf[i].ppid, buf[i].name, buf[i].pname, buf[i].state);
  }
  free(buf);
  exit(0);
}