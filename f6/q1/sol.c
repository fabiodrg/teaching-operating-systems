#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define READ_END 0
#define WRITE_END 1
#define BUFSIZE 256

/**
 * @brief Reads the file contents and writes to pipe
 *
 * @param filename Filename to be read
 * @param pipe_fds Descriptors for the pipe
 * @retval EXIT_FAILURE - Error while processing file or writing to pipe
 * @retval EXIT_SUCCESS - OK
 */
int parent(char *filename, int pipe_fds[2]) {
    /* close pipe reading end */
    close(pipe_fds[READ_END]);

    /* open the file in read mode */
    int file_fd = open(filename, O_RDONLY);
    if (file_fd == -1) {
        fprintf(stderr, "Failed to open file '%s'. Cause: %s\n", filename,
                strerror(errno));
        close(pipe_fds[WRITE_END]);
        return EXIT_FAILURE;
    }

    /* read file in chunks of BUFSIZE and write to pipe */
    char buf[BUFSIZE];
    int bytes; // store number of bytes read from file
    while ((bytes = read(file_fd, buf, BUFSIZE)) > 0) {
        // note: only write as many bytes as returned by 'read'
        // which may be less than the buffer size
        if (write(pipe_fds[WRITE_END], buf, bytes) == -1) {
            fprintf(stderr, "Failed to write to pipe. Cause: %s\n",
                    strerror(errno));
            close(file_fd);
            close(pipe_fds[WRITE_END]);
            return EXIT_FAILURE;
        }
    }

    if (bytes == -1) { // handle errors while reading file
        fprintf(stderr, "Error while reading '%s'. Cause: %s\n", filename,
                strerror(errno));
        close(file_fd);
        close(pipe_fds[WRITE_END]);
        return EXIT_FAILURE;
    }

    /* close pipe and file descriptor */
    close(pipe_fds[WRITE_END]);
    close(file_fd);
    return EXIT_SUCCESS;
}

/**
 * @brief Reads the contents of the pipe and prints to `stdout`
 *
 * @param pipe_fds Array of pipe file descriptors
 * @retval EXIT_FAILURE - Error while reading from pipe
 * @retval EXIT_SUCCESS - OK
 */
int child(int pipe_fds[2]) {
    /* close pipe writing end */
    close(pipe_fds[WRITE_END]);

    /* read file in chunks of BUFSIZE and write to pipe */
    char buf[BUFSIZE];
    int bytes;
    while ((bytes = read(pipe_fds[READ_END], buf, BUFSIZE)) > 0) {
        write(STDOUT_FILENO, buf, bytes); // write to STDOUT
    }

    if (bytes == -1) { // handle errors while reading from pipe
        fprintf(stderr, "Error while reading from pipe. Cause: %s\n",
                strerror(errno));
        close(pipe_fds[READ_END]);
        return EXIT_FAILURE;
    }

    /* close pipe */
    close(pipe_fds[READ_END]);
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    /* validate arguments */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* open pipe */
    int pipe_fds[2];
    if (pipe(pipe_fds) < 0) {
        perror("pipe error");
        exit(EXIT_FAILURE);
    }

    /* create process */
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        /* parent */
        int r = parent(argv[1], pipe_fds);

        /* wait for child and exit */
        if (waitpid(pid, NULL, 0) < 0) {
            fprintf(stderr, "Cannot wait for child: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }

        return r;
    } else {
        /* child */
        return child(pipe_fds);
    }
}
