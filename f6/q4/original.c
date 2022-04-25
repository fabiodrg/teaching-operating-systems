#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#define CHANNEL0 0
#define CHANNEL1 1
#define DATA0 "In every walk with nature..."
#define DATA1 "...one receives far more than he seeks."
/* by John Muir */
int main(int argc, char *argv[]) {
    int sockets[2];
    char buf[1024];
    pid_t pid;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0) {
        perror("opening stream socket pair");
        exit(1);
    }

    if ((pid = fork()) < 0) {
        perror("fork");
        return EXIT_FAILURE;
    } else if (pid == 0) {
        /* this is the child */
        close(sockets[CHANNEL0]);
        if (read(sockets[CHANNEL1], buf, sizeof(buf)) < 0)
            perror("reading stream message");
        if (write(sockets[CHANNEL1], DATA1, sizeof(DATA1)) < 0)
            perror("writing stream message");
        printf("message from %d-->%s\n", getppid(), buf);
        close(sockets[CHANNEL1]);
        /* leave gracefully */
        return EXIT_SUCCESS;
    } else {
        /* this is the parent */
        close(sockets[CHANNEL1]);
        if (write(sockets[CHANNEL0], DATA0, sizeof(DATA0)) < 0)
            perror("writing stream message");
        if (read(sockets[CHANNEL0], buf, sizeof(buf)) < 0)
            perror("reading stream message");
        printf("message from %d-->%s\n", pid, buf);
        close(sockets[CHANNEL0]);
        /* wait for child and exit */
        if (waitpid(pid, NULL, 0) < 0) {
            perror("did not catch child exiting");
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
}
