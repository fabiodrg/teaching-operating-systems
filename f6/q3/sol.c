#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define PIPE_READ 0
#define PIPE_WRITE 1

/**
 * @brief Parses a command string and creates an array of arguments. E.g.,
 * `"ls -l -a"` would be broken into an array with the strings
 * `{"ls", "-l", "-a"}`
 *
 * Usage:
 * ```
 * char **args;                     // Array of strings
 * parse_cmds("ls -l -a", &args);   // Array allocated internally and writes
 *                                  // address in `args`
 * printf("%s", args[0]);           // Output: "ls"
 * ```
 *
 * @param cmd The string command
 * @param args Return by parameter the array of arguments, null terminated
 * @return int
 */
int parse_cmds(char *cmd, char ***args) {
    // init array of arguments
    int n = 0;    // current number of elements in array
    int cap = 10; // holds current capacity
    char **aux_args = malloc(sizeof(char *) * cap);
    if (aux_args == NULL) {
        fprintf(stderr, "Failed to allocate memory for arguments array\n");
        exit(EXIT_FAILURE);
    }

    // get the command string (1st token)
    aux_args[n++] = strtok(cmd, " ");

    // get the options strings, if any (remainder tokens)
    char *tok;
    while ((tok = strtok(NULL, " ")) != NULL) {
        // if num of strings is 'capacity - 1', reallocate
        // note: we need one extra placeholder for NULL, hence 'capacity - 1'
        if (n == (cap - 1)) {
            cap += 10; // reallocate in steps of 10
            aux_args = realloc(aux_args, sizeof(char *) * cap);
            if (aux_args == NULL) {
                fprintf(stderr,
                        "Failed to reallocate memory for arguments array\n");
                exit(EXIT_FAILURE);
            }
        }
        // add token to args array
        aux_args[n++] = tok;
    }
    // ensure array is null terminated
    aux_args[n] = NULL;

    // return by parameter
    *args = aux_args;
    return 0;
}

/**
 * @brief Executes a command string
 *
 * @param cmd The string command, e.g., `ls -l -a`
 * @return int
 */
int run_cmd(char *cmd) {
    char **args;
    parse_cmds(cmd, &args);

    if (execvp(args[0], args) == -1) {
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    return 0;
}

/**
 * @brief Executes a chain of piped commands, e.g., `cat file.c | grep printf
 * | wc -l`
 *
 * @param piped_cmds The chain of piped commands
 */
void next_pipe(char *piped_cmds) {
    // get current command to be executed, e.g. `cat file.c`
    char *curr_cmd = strtok(piped_cmds, "|");
    // get the remainder list of piped commands, e.g., `grep printf | wc -l`
    char *next_piped_cmds = strtok(NULL, "");

    if (next_piped_cmds != NULL) {
        // if there is more piped commands, then create a pipe
        int pipe_fds[2];
        if (pipe(pipe_fds) == -1) {
            fprintf(stderr, "Failed to create pipe: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }

        // fork
        pid_t pid;
        if ((pid = fork()) == -1) { // error
            fprintf(stderr, "Failed to fork: %s", strerror(errno));
            exit(EXIT_FAILURE);
        } else if (pid > 0) { // child
            /*
             * the child process is responsible to process the remainder list
             * of piped commands
             */

            // close unused pipe end
            close(pipe_fds[PIPE_WRITE]);
            // set stdin to referece the pipe's read end
            dup2(pipe_fds[PIPE_READ], STDIN_FILENO);
            close(pipe_fds[PIPE_READ]);
            // process remainder list of piped commands recursively
            next_pipe(next_piped_cmds);
        } else if (pid == 0) {
            /*
             * the parent process executes the current command, extracted
             * from a list of piped commands
             */

            // close unused pipe end
            close(pipe_fds[PIPE_READ]);
            // make the stdout file descriptor point to pipes write end
            dup2(pipe_fds[PIPE_WRITE], STDOUT_FILENO);
            close(pipe_fds[PIPE_WRITE]);
            // run the command with execvp
            run_cmd(curr_cmd);
        }
    } else {
        // no more piped commands to be process, thus no need to create pipes
        // simply execute the last command of the input string
        run_cmd(curr_cmd);
    }
}

int main(int argc, char const *argv[]) {
    // validate arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s \"cmd_1 | cmd_2 -a | ... | cmd_n\"\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    // copy argv1
    char *cmd = malloc(sizeof(char) * (strlen(argv[1]) + 1));
    if (cmd == NULL) {
        fprintf(stderr, "Failed to allocate memory for command string\n");
        exit(EXIT_FAILURE);
    }
    strcpy(cmd, argv[1]);

    // run commands
    next_pipe(cmd);
    return 0;
}
