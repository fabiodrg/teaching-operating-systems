#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

void run_command(char *command);

int main(int argc, char *argv[])
{
    char buf[1024];
    char *command;
    pid_t pid;
    /* do this until you get a ^C or a ^D */
    for (;;)
    {
        /* give prompt, read command and null terminate it */
        fprintf(stdout, "$ ");
        if ((command = fgets(buf, sizeof(buf), stdin)) == NULL)
            break;
        command[strlen(buf) - 1] = '\0';
        /* call fork and check return value */
        if ((pid = fork()) == -1)
        {
            fprintf(stderr, "%s: can't fork command: %s\n",
                    argv[0], strerror(errno));
            continue;
        }
        else if (pid == 0)
        {
            // launch the command
            run_command(command);
            /* if I get here "execlp" failed */
            fprintf(stderr, "%s: couldn't exec %s: %s\n",
                    argv[0], buf, strerror(errno));
            /* terminate with error to be caught by parent */
            exit(EXIT_FAILURE);
        }
        /* shell waits for command to finish before giving prompt again */
        if ((pid = waitpid(pid, NULL, 0)) < 0)
            fprintf(stderr, "%s: waitpid error: %s\n",
                    argv[0], strerror(errno));
    }
    exit(EXIT_SUCCESS);
}

void run_command(char *command)
{
    size_t cap = 0; // current array capacity
    size_t size = 0; // current array size
    char **args = NULL; // array of arguments for the command

    /* process input */
    char *tok = strtok(command, " ");
    do
    {
        // if args array hasnt capacity for more strings, realloc
        if (size == cap) {
            cap += 5;
            args = realloc(args, sizeof(char *) * cap);
        }

        // allocate memory for the arg string
        args[size++] = tok;
    } while ((tok = strtok(NULL, " ")) != NULL);

    // set last argument to NULL
    args[size] = NULL;

    // for debugging array of arguments
    /*for (size_t i = 0; i < cap; i++)
    {
        printf("%d: '%s'\n", i, args[i]);
    }*/

    /* exec command */
    execvp(command, args);
}
