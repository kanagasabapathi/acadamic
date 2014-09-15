#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>


int
main(int argc, char **argv)
{
    pid_t pid = getpid();

    fprintf(stdout, "Path to current process: '/proc/%d/'\n", (int)pid);

    return EXIT_SUCCESS;
}
