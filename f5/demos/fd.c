#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define CHUNK 40

int main()
{
    int parent_pid = getpid();
    printf("Parent process pid: %d\n", parent_pid);

    // open file "random.txt"
    int fd = open("random.txt", O_RDONLY);
    printf("fd: %d\n", fd);

    // fork
    int child_pid = fork();

    char buf[CHUNK]; // buffer for chunks of text from file
    char msg[CHUNK + 100]; // buffer to prepare message to stdout
    int bytes; // number of bytes read from file
    
    // read file in chunks of size CHUNK - 1
    while ((bytes = read(fd, buf, CHUNK - 1)))
    {
        // insert the \0 explicitly, so that buf is a valid string
        buf[bytes] = '\0';
        // prepare the message to be printed
        // show the process ID and the chunk retrieved
        int msg_size;
        if (getpid() == parent_pid) {
            msg_size = sprintf(msg, "Parent: %s\n", buf);
        } else {
            msg_size = sprintf(msg, "Child: %s\n", buf);
        }
        // print message on STDOUT
        write(STDOUT_FILENO, msg, msg_size);

        sleep(1); // just to give more chances of both parent and child running before EOF
    }

    if (child_pid > 0)
    {
        waitpid(child_pid, NULL, 0);
    }

    return 0;
}
