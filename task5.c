#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#define FIFO_PATH "/Users/renat/xv6-riscv/fifo"
#define LOG_PATH  "/Users/renat/xv6-riscv/fifo.log"
#define BUF_SIZE 512
#define N 30

volatile sig_atomic_t exit_flag = 0; 
volatile sig_atomic_t alarm_flag = 0;

unsigned long total_messages = 0;
unsigned long total_bytes = 0;
unsigned long total_alarms = 0;

FILE *log_stream = NULL;
int run_as_daemon = 0;
int is_switched_to_daemon = 0;

void print_stats() {
    fprintf(log_stream, "\n---- Stats ----\n");
    fprintf(log_stream, "Messages: %lu\n", total_messages);
    fprintf(log_stream, "Bytes: %lu\n", total_bytes);
    fprintf(log_stream, "Alarms: %lu\n\n", total_alarms);
    fflush(log_stream);
}

void daemonize_on_sighup() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    if (setsid() < 0) {
        perror("setsid");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    fclose(stdin);
    fclose(stdout);
    fclose(stderr);
    freopen(LOG_PATH, "a", stdout);
    freopen(LOG_PATH, "a", stderr);

    log_stream = fopen(LOG_PATH, "a");
    if (!log_stream) {
        perror("log reopen");
        exit(EXIT_FAILURE);
    }

    fprintf(log_stream, "\nДемонизация по SIGHUP.\n");
    print_stats();
    run_as_daemon = 1;
    alarm(N);
}

void handle_signal(int sig) {
    switch (sig) {
        case SIGINT:
            exit_flag = 1;
            break;
        case SIGTERM:
            exit_flag = 2;
            break;
        case SIGALRM:
            alarm_flag = 1;
            break;
        case SIGUSR1:
            print_stats();
            break;
        case SIGHUP:
            if (!run_as_daemon && !is_switched_to_daemon) 
                is_switched_to_daemon = 1; 
            break;
        default:
            break;
    }
}

void exec_alarm() {
    fprintf(log_stream, "Диагностика. Программа ждет ввода...\n");
    total_alarms++;
    alarm_flag = 0;
    alarm(N);
}

int create_fifo(const char *fifo_path) {
    if (mkfifo(fifo_path, 0600) == -1) {
        if (errno == EEXIST) {
            struct stat st;
            if (stat(fifo_path, &st) == -1) {
                perror("stat");
                return -1;
            }
            if (!S_ISFIFO(st.st_mode)) {
                fprintf(stderr, "%s существует, но это не FIFO\n", fifo_path);
                return -1;
            }
        } 
        else {
            perror("mkfifo");
            return -1;
        }
    }
    return 0;
}

int open_fifo(const char *fifo_path) {
    int fd;
    while (1) {
        fd = open(fifo_path, O_RDONLY);
        if (fd == -1) {
            if (errno == EINTR) {
                if (exit_flag == 2) {
                    fprintf(log_stream, "Получен SIGTERM. open прерван.\n");
                    return -1;
                } 
                else if (exit_flag == 1) {
                    fprintf(log_stream, "SIGINT. open прерван.\n");
                    return -1;
                }
                if (is_switched_to_daemon && !run_as_daemon)
                    daemonize_on_sighup();
                if (alarm_flag)
                    exec_alarm();
                continue;
            } 
            else {
                perror("open FIFO");
                return -1;
            }
        }
        break;
    }
    return fd;
}

int read_from_fifo(int fd) {
    char buf[BUF_SIZE];
    ssize_t n;
    int newline_needed = 0;
    total_messages++;
    while (1) {
        n = read(fd, buf, BUF_SIZE - 1);
        if (n == -1) {
            if (errno == EINTR) {
                if (exit_flag == 2) {
                    fprintf(log_stream, "Получен SIGTERM. Прерываю чтение.\n");
                    return -1;
                } 
                else if (exit_flag == 1) {
                    fprintf(log_stream, "SIGINT: дочитываю FIFO...\n");
                    continue;
                }
                if (is_switched_to_daemon && !run_as_daemon)
                    daemonize_on_sighup();
                if (alarm_flag)
                    exec_alarm();
                continue;
            } 
            else {
                perror("read");
                return -1;
            }
        } 
        else if (n == 0) 
            break;

        buf[n] = '\0';
        total_bytes += n;
        if (buf[n - 1] != '\n') 
            newline_needed = 1;
        else 
            newline_needed = 0;

        fputs(buf, log_stream);
        if (newline_needed) 
            fputc('\n', log_stream);
        fflush(log_stream);
    }

    return 0;
}

void setup_signals() {
    struct sigaction sa = {0};
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGALRM, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGHUP, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL); 
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "-d") == 0) 
        run_as_daemon = 1;
    if (run_as_daemon) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid > 0) {
            exit(EXIT_SUCCESS);
        }
        
        if (setsid() < 0) {
            perror("setsid");
            exit(EXIT_FAILURE);
        }
        
        pid = fork();
        if (pid < 0) 
            exit(EXIT_FAILURE);
        if (pid > 0) 
            exit(EXIT_SUCCESS);
        
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);
        freopen(LOG_PATH, "a", stdout);
        freopen(LOG_PATH, "a", stderr);
    }

    log_stream = run_as_daemon ? fopen(LOG_PATH, "a") : stdout;
    if (!log_stream) {
        perror("log open");
        exit(EXIT_FAILURE);
    }
    setup_signals();
    alarm(N);
    if (create_fifo(FIFO_PATH) == -1) 
        exit(EXIT_FAILURE);
    fprintf(log_stream, "Echo server started. FIFO: %s\n", FIFO_PATH);
    fflush(log_stream);

    while (!exit_flag) {
        int fd = open_fifo(FIFO_PATH);
        if (fd == -1) 
            break;
        if (is_switched_to_daemon && !run_as_daemon)
            daemonize_on_sighup();
        read_from_fifo(fd);
        if (close(fd) < 0) {
            perror("fifo close");
            exit(EXIT_FAILURE);
        }
        if (is_switched_to_daemon && !run_as_daemon)
            daemonize_on_sighup();
        if (alarm_flag) 
            exec_alarm();
    }

    fprintf(log_stream, "Завершение по сигналу %d\n", exit_flag);
    print_stats();
    unlink(FIFO_PATH);
    if (log_stream != stdout) {
        if (fclose(log_stream) < 0) {
            perror("log close");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}