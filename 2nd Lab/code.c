#include <unistd.h>
#include <stdio.h>

int main()
{
        char *command[] = {"ls", "../1st\ Lab/", NULL};
        if (fork() == 0)
        execvp(command[0], command);
        printf("Done.\n");
        return 0;       
}