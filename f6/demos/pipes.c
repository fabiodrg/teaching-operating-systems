/**
 * Example illustrating ICP via pipes
 * In the example, the parent process sends messages to child process (parent
 * writes to pipe, child reads from pipe)
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define READ_END 0
#define WRITE_END 1

int main() {
    /** prepare pipe */
    int pipe_fds[2];            // pipe file descriptors
    if (pipe(pipe_fds) == -1) { // create pipe
        fprintf(stderr, "Failed to create pipe. Cause: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    /** create process */
    int pid = fork();

    if (pid == -1) { // error
        fprintf(stderr, "Failed to create process. Cause: %s\n",
                strerror(errno));
        return EXIT_FAILURE;
    } else if (pid > 0) { // parent
        // close the unused pipe end
        close(pipe_fds[READ_END]);

        const unsigned int num_msgs = 2;
        char *msgs[] = {
            "There are only two kinds of programming languages out there. The "
            "ones people complain about and the ones no one uses.\n",
            "It's hardware that makes a machine fast. It's software that makes "
            "a fast machine slow.\n"};

        for (size_t i = 0; i < num_msgs; i++) {
            if (write(pipe_fds[WRITE_END], msgs[i], strlen(msgs[i])) == -1) {
                fprintf(stderr, "Failed to write to pipe. Cause: %s\n",
                        strerror(errno));
            }
        }
        // close write end, so that child process knows I will not send more
        // messages
        close(pipe_fds[WRITE_END]);

        /* wait for child and exit */
        if (waitpid(pid, NULL, 0) < 0) {
            fprintf(stderr, "Cannot wait for child: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
    } else if (pid == 0) { // child

        // close the unused pipe end
        close(pipe_fds[WRITE_END]);

        const int buf_size = 256; // buffer size
        char buf[buf_size];       // buffer
        int bytes;                // number of bytes returned by 'read'
        while ((bytes = read(pipe_fds[READ_END], buf, buf_size)) > 0) {
            write(STDOUT_FILENO, buf, bytes);
        }

        if (bytes == -1) { // handle error
            fprintf(stderr, "Failed to read from pipe. Cause: %s\n",
                    strerror(errno));
        }

        // if 'read' returned 0, then parent closed the pipe write end
        close(pipe_fds[WRITE_END]);
    }
    return 0;
}
