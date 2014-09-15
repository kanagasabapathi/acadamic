#include <stdio.h>
 
main()
{
  FILE *fptr;
  char line[130]; /* line from unix command*/
   
  fptr = popen("ls -l","r"); /* Issue the ls command. */
 
/* Read a line */
  while ( fgets( line, sizeof line, fptr))
  {
    printf("%s", line);
 
 /* or process the 'line' as needed */
  }
  pclose(fptr);
}
