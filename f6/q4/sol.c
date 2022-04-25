#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SOCK_PARENT 0
#define SOCK_CHILD 1

#define BUFSIZE 256

/**
 * @brief Reads the contents of a file and writes it in `socket[SOCK_PARENT]`
 *
 * @param filename The filename
 * @param sockets A pair of connected sockets file descriptors
 */
void parent(char *filename, int sockets[2]) {
    /* close childs socket */
    close(sockets[SOCK_CHILD]);

    /* open the file in read mode */
    int file_fd = open(filename, O_RDONLY);
    if (file_fd == -1) {
        fprintf(stderr, "Failed to open file '%s'. Cause: %s\n", filename,
                strerror(errno));
        close(sockets[SOCK_PARENT]);
        exit(EXIT_FAILURE);
    }

    /* read file in chunks of BUFSIZE and write to pipe */
    char buf[BUFSIZE];
    int bytes; // auxiliar, store how many bytes were read from file on each
               // 'read' call
    while ((bytes = read(file_fd, buf, BUFSIZE)) > 0) {
        // read another chunk successfully, write to socket to send it to child
        if (write(sockets[SOCK_PARENT], buf, bytes) == -1) {
            // note: use 'bytes' as third argument because only that amount of
            // bytes was read, can be less than BUFSIZE
            fprintf(stderr, "Failed to write to pipe. Cause: %s\n",
                    strerror(errno));
            close(file_fd);
            close(sockets[SOCK_PARENT]);
            exit(EXIT_FAILURE);
        }
    }

    if (bytes == -1) { // handle errors while reading file
        fprintf(stderr, "Error while reading '%s'. Cause: %s\n", filename,
                strerror(errno));
        close(file_fd);
        close(sockets[SOCK_PARENT]);
        exit(EXIT_FAILURE);
    }

    /* close socket and file descriptor */
    close(sockets[SOCK_PARENT]);
    close(file_fd);
}

/**
 * @brief Reads data from `sockets[SOCK_CHILD]` and prints in stdout
 *
 * @param sockets A pair of connected sockets file descriptors
 */
void child(int sockets[2]) {
    /* close pipe writing end */
    close(sockets[SOCK_PARENT]);

    /* read file in chunks of BUFSIZE and write to pipe */
    char buf[BUFSIZE];
    int bytes;
    while ((bytes = read(sockets[SOCK_CHILD], buf, BUFSIZE)) > 0) {
        write(STDOUT_FILENO, buf, bytes); // write to STDOUT
    }

    if (bytes == -1) { // handle errors while reading from pipe

        fprintf(stderr, "Error while reading from pipe. Cause: %s\n",
                strerror(errno));
        close(sockets[SOCK_CHILD]);
        exit(EXIT_FAILURE);
    }

    /* close pipe and file descriptor */
    close(sockets[SOCK_CHILD]);
}

int main(int argc, char *argv[]) {
    /* validate arguments */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* create a pair of connected UNIX sockets */
    int sockets[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0) {
        perror("opening stream socket pair");
        return EXIT_FAILURE;
    }

    /* create process */
    pid_t pid;
    if ((pid = fork()) < 0) { // error
        perror("fork");
        return EXIT_FAILURE;
    } else if (pid == 0) { // child
        child(sockets);
        return EXIT_SUCCESS;
    } else { // parent
        parent(argv[1], sockets);

        // wait for child and exit
        if (waitpid(pid, NULL, 0) < 0) {
            perror("did not catch child exiting");
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
}
