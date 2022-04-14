#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
    /* fork a child process */
    fork(); printf("after 1st fork, pid: %d. My parent is, pid: %d\n", getpid(), getppid());
    /* fork another child process */
    fork(); printf("after 2nd fork, pid: %d. My parent is, pid: %d\n", getpid(), getppid());
    /* and fork another */
    fork(); printf("after 3rd fork, pid: %d. My parent is, pid: %d\n", getpid(), getppid());

    return 0;
}
