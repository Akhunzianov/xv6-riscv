#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int fd, ret;

  printf("\n---- Testing /dev/null ----\n");
  fd = open("/dev/null", O_RDWR);
  if (fd < 0) { 
    printf("cannot open /dev/null\n"); 
    exit(1); 
  }
  ret = write(fd, "xyz", 3);
  printf("write to null: %d bytes\n", ret);
  ret = read(fd, argv, 1);
  printf("read from null: %d bytes (should be 0)\n", ret);
  close(fd);

  printf("\n---- Testing /dev/zero with direct read ----\n");
  fd = open("/dev/zero", O_RDONLY);
  if (fd < 0) { 
    printf("cannot open /dev/zero\n"); 
    exit(1); 
  }
  char buf[4];
  ret = read(fd, buf, sizeof(buf));
  printf("zero bytes: %d %d %d %d\n", buf[0], buf[1], buf[2], buf[3]);
  close(fd);

  printf("\n---- Hexdump 4 bytes from /dev/zero ----\n");
  if (fork() == 0) {
    char *args[] = { "hexdump", "/dev/zero", "4", 0 };
    exec("hexdump", args);
    printf("exec hexdump failed\n");
    exit(1);
  }
  wait(0);

  printf("\n---- Testing /dev/urandom ----\n");
  fd = open("/dev/urandom", O_RDONLY);
  if (fd < 0) { 
    printf("cannot open /dev/urandom\n"); 
    exit(1); 
  }
  ret = read(fd, buf, 3);
  printf("random bytes: %d %d %d (read %d)\n", buf[0], buf[1], buf[2], ret);
  close(fd);

  printf("\n---- Testing /dev/nullstat via both methods ----\n");
  fd = open("/dev/nullstat", O_RDWR);
  if (fd < 0) { 
    printf("cannot open /dev/nullstat\n"); 
    exit(1); 
  }
  write(fd, "1234567", 7);
  write(fd, "6789", 4);
  uint64 cnt;
  ret = read(fd, &cnt, sizeof(cnt));
  printf("nullstat count via C: %d\n", (int)cnt);
  close(fd);

  printf("\n---- Hexdump 8 bytes from /dev/nullstat ----\n");
  if (fork() == 0) {
    char *args2[] = { "hexdump", "/dev/nullstat", "8", 0 };
    exec("hexdump", args2);
    exit(1);
  }
  wait(0);

  exit(0);
}
