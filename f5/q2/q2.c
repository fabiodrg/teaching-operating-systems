#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{   
    int fork_counter = 0;
    for (int i = 0; i < 4; i++) {
        fork_counter++;
        int pid = fork();
        // if child process, reset its own counter
        if (pid == 0) {
            fork_counter = 0;
        }
    }
    printf("pid '%d' forked %d times\n", getpid(), fork_counter);
    return EXIT_SUCCESS;
}