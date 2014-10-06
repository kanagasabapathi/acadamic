#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
int main() {
  DIR *dir;
  struct dirent *dirent;
  char *c = NULL;
  c = get_current_dir_name();
  if ((dir = opendir(c))==NULL) {
    fprintf(stderr,"%d (%s) opendir %s failed\n", errno, strerror(errno), c);
    return 2;
  }
  while ((dirent = readdir(dir))!=NULL) {
    if(dirent->d_name[0]!='.')
    printf("%s\n", dirent->d_name);
  }
  closedir(dir);
  return 0;
}
 
