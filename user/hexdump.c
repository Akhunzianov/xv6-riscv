#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define BUF_SIZE 512

static void
print_byte(unsigned char b) {
  static char *hex = "0123456789abcdef";
  write(1, &hex[b >> 4], 1);
  write(1, &hex[b & 0x0F], 1);
}

int
main(int argc, char *argv[])
{
  if (argc != 3) {
    printf("Usage: hexdump <file> <count>\n");
    exit(1);
  }

  char *path = argv[1];
  int to_print = atoi(argv[2]);
  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    printf("hexdump: cannot open %s\n", path);
    exit(1);
  }

  unsigned char buf[BUF_SIZE];
  int printed = 0;
  int global_idx = 0;
  while (printed < to_print) {
    int chunk = to_print - printed;
    if (chunk > BUF_SIZE) 
      chunk = BUF_SIZE;
    int n = read(fd, buf, chunk);
    if (n <= 0)
      break;
    for (int i = 0; i < n; i++) {
      print_byte(buf[i]);
      write(1, " ", 1);
      if ((global_idx & 0xF) == 0xF)
        write(1, "\n", 1);
      global_idx++;
    }
    printed += n;
  }
  if ((global_idx & 0xF) != 0)
    write(1, "\n", 1);

  close(fd);
  exit(0);
}