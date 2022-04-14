#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


int main()
{   
    // buffer for arguments
    char *args[10] = {};
    
    // allocate memory for command string
    char *str = malloc(100 * sizeof(char));
    strcpy(str, "ls --all -l --human-readable");

    // store command & options in `args`
    int i = 0;
    char *tok = strtok(str, " ");
    do {
        args[i++] = tok;   
    } while((tok = strtok(NULL, " ")) != NULL);

    // demonstrate that the first token returned by strtok
    // matches the original string
    // same address
    printf("String original: %p\n", str);
    printf("Primeiro token: %p\n", args[0]);

    // **************************************
    //free(str); // uncomment
    // **************************************

    // print all arguments
    int j = 0;
    while(args[j] != NULL) {
        printf("arg %d: '%s'\n", j, args[j]);
        j++;
    }

    printf("--- Run command ---\n");
    execvp(args[0], args);
    perror("execvp: ");

    return 0;
}
