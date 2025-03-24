#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(int argc, char *argv[]) {
    int fd, pid, ret;

    printf("---- Test 1: Read/Write ----\n");
    fd = mutex();
    if (fd < 0) {
        fprintf(2, "Error: mutex creation failed\n");
        exit(1);
    }
    char buf[10];
    ret = read(fd, buf, sizeof(buf));
    if (ret >= 0)
        printf("Error: read mutex succseded\n");
    else
        printf("Success: read mutex failed as expected\n");
    ret = write(fd, buf, sizeof(buf));
    if (ret >= 0)
        printf("Error: write mutex succseded\n");
    else
        printf("Success: write mutex failed as expected\n");
    close(fd);

    printf("\n---- Test 2: Closing locked mutex ----\n(a)\n");
    fd = mutex();
    if (fd < 0) {
        fprintf(2, "Error: mutex creation failed\n");
        exit(1);
    }
    if (mutex_lock(fd) < 0) {
        fprintf(2, "Error: mutex_lock failed\n");
        exit(1);
    }
    if (close(fd) < 0)
        printf("Error: close failed on locked mutex (self)\n");
    else
        printf("Success: locked mutex closed by owner\n");
    printf("\n---- Test 2: Closing locked mutex ----\n(b)\n");
    fd = mutex();
    if (fd < 0) {
        fprintf(2, "Error: mutex creation failed\n");
        exit(1);
    }
    if (mutex_lock(fd) < 0) {
        fprintf(2, "Error: mutex_lock failed\n");
        exit(1);
    }
    pid = fork();
    if (pid < 0) {
        fprintf(2, "Error: fork failed\n");
        exit(1);
    } 
    else if (pid == 0) {
        printf("Child proc attempts to close mutex made locked by parent\n");
        if (close(fd) < 0)
            printf("Error: descriptor close failed\n");
        else
            printf("If there is no kfree in log in test2 (b) section before this message, then success\n");
        exit(0);
    } 
    else {
        sleep(20);
        if (mutex_unlock(fd) < 0) {
            fprintf(2, "Error: mutex_unlock failed in parent\n");
            exit(1);
        }
        if (close(fd) < 0)
            printf("Error: parent close failed after unlocking mutex\n");
        else
            printf("Parent: Successfully unlocked and closed mutex\n");
        wait(0);
    }

    printf("\n---- Test 3: Process termination with open (locked) mutex ----\n");
    pid = fork();
    if (pid < 0) {
        fprintf(2, "Error: fork failed\n");
        exit(1);
    } 
    else if (pid == 0) {
        int fd2 = mutex();
        if (fd2 < 0) {
            fprintf(2, "Child Error: mutex creation failed\n");
            exit(1);
        }
        if (mutex_lock(fd2) < 0) {
            fprintf(2, "Child Error: mutex_lock failed\n");
            exit(1);
        }
        printf("Child: Exiting with locked mutex (should be auto-closed by exit)\n");
        exit(0);
    } 
    else {
        wait(0);
        printf("Parent: Child exited. If 2 previous messages in log about kfree then succsess.\n");
    }

    printf("\n---- Test 4: Unlocking locked mutex by another proc ----\n");
    fd = mutex();
    if (fd < 0) {
        fprintf(2, "Error: mutex creation failed\n");
        exit(1);
    }
    if (mutex_lock(fd) < 0) {
        fprintf(2, "Error: mutex_lock failed\n");
        exit(1);
    }
    pid = fork();
    if (pid < 0) {
        fprintf(2, "Error: fork failed\n");
        exit(1);
    } 
    else if (pid == 0) {
        printf("Child: trying to unlock mutex locked by parent\n");
        if (mutex_unlock(fd) < 0)
            printf("Success: mutex remained locked\n");
        else
            printf("Error: mutex was unlocked\n");
        exit(0);
    } 
    else {
        sleep(20);
        printf("Parent: unlocking and closing mutex\n");
        if (mutex_unlock(fd) < 0) {
            fprintf(2, "Error: mutex_unlock failed for locking proc\n");
            exit(1);
        }
        if (close(fd) < 0) {
            fprintf(2, "Error: mutex close failed\n");
            exit(1);
        }
        wait(0);
    }

    printf("\nAll tests completed.\n");
    exit(0);
}