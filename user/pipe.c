#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        fprintf(2, "Error pipe\n");
        exit(1);
    }
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "Error fork\n");
        exit(1);
    }

    if (pid == 0) { 
        close(pipefd[1]); 
        close(0);
        if (dup(pipefd[0]) < 0) {
            fprintf(2, "Error dup\n");
            exit(1);
        }
        close(pipefd[0]);
        char *wc_argv[] = { "wc", 0 };
        exec("wc", wc_argv);
        fprintf(2, "Error exec\n");
        exit(1);
    }
    else { 
        close(pipefd[0]); 
        for (int i = 1; i < argc; i++) {
            if (write(pipefd[1], argv[i], strlen(argv[i])) < 0) {
                fprintf(2, "Error writing arguments\n");
                exit(1);
            }
            if (write(pipefd[1], "\n", 1) < 0) {
                fprintf(2, "Error write newline\n");
                exit(1);
            }
        }
        close(pipefd[1]); 
        wait(0);
        exit(0);
    }
}