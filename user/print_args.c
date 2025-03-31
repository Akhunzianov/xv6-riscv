#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(int argc, char *argv[]) {
    int mut = mutex();
    if (mut < 0) {
        fprintf(2, "Error: mutex create\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "Error: fork\n");
        exit(1);
    }
    char tmp[2];
    tmp[1] = '\0';
    printf("Output without mutex:\n");
    for (int i = 1; i < argc; i++) {
        for (int j = 0; argv[i][j] != '\0'; j++) {
            tmp[0] = argv[i][j];
            printf("%d: arg %d, char '%s'\n", getpid(), i, tmp);
        }
    }
    if (pid == 0)
        exit(0);
    wait(0);
    
    printf("Output with mutex:\n");
    pid = fork();
    if (pid < 0) {
        fprintf(2, "Error: fork\n");
        exit(1);
    }
    for (int i = 1; i < argc; i++) {
        for (int j = 0; argv[i][j] != '\0'; j++) {
            if (mutex_lock(mut) < 0) {
                fprintf(2, "Error: locking mutex\n");
                exit(1);
            }
            tmp[0] = argv[i][j];
            printf("%d: arg %d, char '%s'\n", getpid(), i, tmp);
            if (mutex_unlock(mut) < 0) {
                fprintf(2, "Error: unlocking mutex\n");
                exit(1);
            }
        }
    }
    if(pid > 0)
        wait(0);

    exit(0);
}