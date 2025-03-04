#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUFFER_SIZE 100

int main(int argc, char *argv[]) {
  char buf[BUFFER_SIZE];
  int n, i = 0;
  char c;
  int errCode = 0;
  while (1) {
    n = read(0, &c, 1);
    if (n < 0) {
      fprintf(2, "sum: read error\n");
      exit(1); 
    }
    if (n == 0)
      break;
    if (c == '\n') 
      break;
    if ((c != ' ') && (c > '9' || c < '0')) 
      errCode = 1;
    buf[i] = c;
    i++;
    if (i > BUFFER_SIZE - 1) {
      fprintf(2, "sum: input too long\n");
      exit(1); 
    }
  }
  if (errCode == 1) {
    fprintf(2, "sum: invalid argument\n");
    exit(1);
  }
  buf[i] = '\0';
  printf("|%s|\n", buf);
  char* space = 0;
  space = strchr(buf, ' ');
  if (!space) {
    fprintf(2, "sum: no space in input\n");
    exit(1);
  }
  *space = '\0';
  char* sndNumPtr = space + 1;
  char* fstNumPtr = buf;
  if (*sndNumPtr == '\0') {
    fprintf(2, "sum: no snd number\n");
    exit(1);
  }
  if (*buf == '\0') {
    fprintf(2, "sum: no fst number\n");
    exit(1);
  }
  char* newSpace1 = 0;
  char* newSpace2 = 0;
  newSpace1 = strchr(buf, ' ');
  newSpace2 = strchr(sndNumPtr, ' ');
  if (newSpace1 || newSpace2) {
    fprintf(2, "sum: too many spaces\n");
    exit(1);
  }
  int num1 = atoi(fstNumPtr);
  int num2 = atoi(sndNumPtr);
  int sum = add(num1, num2);
  printf("%d\n", sum);
  exit(0);
}
