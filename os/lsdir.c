#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
int main(int argc, char* argv[])
{
        DIR *mydir;
        struct dirent *myfile;
        struct stat mystat;
        mydir = opendir(argv[1]);
        while((myfile = readdir(mydir)) != NULL){
                        stat(myfile->d_name, &mystat);
                        if(myfile->d_name[0]!='.')
                        printf(" %s\n", myfile->d_name);
        }
        closedir(mydir);
}
