#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main(int argc, char *argv[]) {
  if (argc > 2)
    fprintf(2, "Invalid args");
  if (argc == 2) {
    int n = atoi(argv[1]);
    struct procinfo *buf = malloc(sizeof(struct procinfo) * n);
    if (!buf) {
      printf("test_manual: malloc error\n");
      exit(1);
    }
    int rc = ps_listinfo(buf, n);
    if (rc < 0) 
      printf("test_manual: call failed, rc=%d\n", rc);
    else {
      printf("test_manual: first call success, copied=%d\n", rc);
      for (int i = 0; i < rc; i++) {
        printf("pid=%d ppid=%d name=%s parent=%s state=%d\n",
               buf[i].pid, buf[i].ppid, buf[i].name, buf[i].pname, buf[i].state);
      }
    }
    free(buf);
    exit(0);
  }

  // число процессов
  int n = ps_listinfo(0, 0);
  if (n < 0) {
    printf("Error at ps_listinfo(NULL,0)\t(err)\n");
    exit(1);
  }
  printf("test0: Number of processes: %d\t(ok)\n", n);

  // маленький буфер
  struct procinfo smallbuf[2];
  int rc = ps_listinfo(smallbuf, 2);
  if (rc < 0) 
    printf("test1: buffer too small? rc=%d\t(ok)\n", rc);
  else 
    printf("test1: unexpected success, copied=%d\t(err)\n", rc);

  // невалидный адресс
  rc = ps_listinfo((struct procinfo*)0xFFFFFFFF, 10);
  if (rc < 0) 
    printf("test2: invalid address error = %d\t(ok)\n", rc);
  else 
    printf("test2: unexpected success with invalid address\t(err)\n");

  // реальный вызов с буфером по количеству процессов, если к тому моменту процессов стало больше, он вернёт -1, придётся увеличить
  if (n < 1) 
    n = 1; 
  struct procinfo *buf = malloc(sizeof(struct procinfo) * n);
  if (!buf) {
    printf("test3: malloc error\n");
    exit(1);
  }
  rc = ps_listinfo(buf, n);
  if (rc < 0) 
    printf("test3: first call fail, rc=%d\n", rc);
  else {
    printf("test3: first call success, copied=%d\n", rc);
    for (int i = 0; i < rc; i++) {
      printf("pid=%d ppid=%d name=%s parent=%s state=%d\n",
             buf[i].pid, buf[i].ppid, buf[i].name, buf[i].pname, buf[i].state);
    }
  }
  free(buf);

  exit(0);
}