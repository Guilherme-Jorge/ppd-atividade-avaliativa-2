#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig)
{
    printf("\nCaught SIGINT (Ctrl+C)! Cleaning up...\n");
    keep_running = 0;
}

void handle_sigterm(int sig)
{
    printf("\nCaught SIGTERM! Performing graceful shutdown...\n");
    keep_running = 0;
}

void handle_sigusr1(int sig)
{
    printf("\nCaught SIGUSR1! Performing custom action...\n");
}

int main()
{
    struct sigaction sa_int, sa_term, sa_usr1;
    int counter = 0;

    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);

    sa_term.sa_handler = handle_sigterm;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    sigaction(SIGTERM, &sa_term, NULL);

    sa_usr1.sa_handler = handle_sigusr1;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;
    sigaction(SIGUSR1, &sa_usr1, NULL);

    printf("Process ID: %d\n", getpid());
    printf("Try these commands in another terminal:\n");
    printf("kill -TERM %d  # Send SIGTERM\n", getpid());
    printf("kill -USR1 %d  # Send SIGUSR1\n", getpid());
    printf("Or press Ctrl+C in this terminal\n\n");

    while (keep_running)
    {
        printf("Working... (count: %d)\r", counter++);
        fflush(stdout);
        sleep(1);
    }

    printf("\nProgram terminated gracefully\n");
    return 0;
}