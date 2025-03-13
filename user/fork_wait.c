#include "kernel/types.h"
#include "user.h"

int main(int argc, char *argv[]) {
    int pid, ret, status;
    pid = fork();
    if (pid < 0) {
        fprintf(2, "Error fork\n");
        exit(1);
    }
    if (pid == 0) {
        int sleep_time = 100;
        sleep(sleep_time); 
        exit(1);
    } 
    else {
        printf("Parent process: pid = %d\n", getpid());
        printf("Child process created: pid = %d\n", pid);
        ret = wait(&status);
        if (ret > 0)
            printf("Child process %d finished with exit code %d\n", ret, status);
        else 
            fprintf(2, "Error: wait failed\n");
        exit(0);
    }
}