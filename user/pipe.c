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
        if (close(pipefd[1]) < 0) { 
            fprintf(2, "Error closing pipe write end in child\n");
            exit(1);
        }
        if (close(0) < 0) { 
            fprintf(2, "Error closing stdin in child\n");
            exit(1);
        }
        if (dup(pipefd[0]) < 0) {
            fprintf(2, "Error dup\n");
            exit(1);
        }
        if (close(pipefd[0]) < 0) {
            fprintf(2, "Error closing pipe read end in child\n");
            exit(1);
        }
        char *wc_argv[] = { "wc", 0 };
        exec("wc", wc_argv);
        fprintf(2, "Error exec\n");
        exit(1);
    }
    else { 
        if (close(pipefd[0]) < 0) { 
            fprintf(2, "Error closing pipe read end in parent\n");
            exit(1);
        }
        for (int i = 1; i < argc; i++) {
            char *argvi = argv[i];
            int leng = strlen(argvi), writ = 0;
            while (writ < leng) {
                int wroteBits = write(pipefd[1], argvi + writ, leng - writ);
                if (wroteBits < 0) {
                    fprintf(2, "Error writing arguments\n");
                    close(pipefd[1]);
                    exit(1);
                }
                writ += wroteBits;
            }
            if (write(pipefd[1], "\n", 1) < 0) {
                fprintf(2, "Error write newline\n");
                close(pipefd[1]);
                exit(1);
            }
        }
        if (close(pipefd[1]) < 0) { 
            fprintf(2, "Error closing pipe write end in parent\n");
            exit(1);
        }
        wait(0);
        exit(0);
    }
}