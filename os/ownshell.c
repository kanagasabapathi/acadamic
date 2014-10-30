#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <stdint.h>
#include <fcntl.h>
#define INPUT_STRING_SIZE 80
#define NORMAL 00
#define OUTPUT_REDIRECTION 11
#define INPUT_REDIRECTION 22
#define PIPELINE 33
#define OUTPUT_APP 55
#define FALSE 0
#define TRUE !FALSE
#define MAX_BUF 1024
#define INT_SIZE_BUF 6
#define PID_LIST_BLOCK 32
#define UP_TIME_SIZE 10
 
 
 
void execute(char **, int, char **);
void handle_signal(int);
int parse(char *, char **, char **, int *);
void chop(char *);

 
typedef void (*sighandler_t)(int);
 
 
//.......check the pipeline function
 
int parse(char *inputString, char *cmdArgv[], char **supplementPtr, int *modePtr){
	int cmdArgc = 0, terminate = 0;
	char *srcPtr = inputString;
	while(*srcPtr != '\0' && terminate == 0){
		*cmdArgv = srcPtr;
		cmdArgc++;
		while(*srcPtr != ' ' && *srcPtr != '\t' && *srcPtr != '\0' && *srcPtr != '\n' && terminate == 0){
			switch(*srcPtr){
			case '>':
				*modePtr = OUTPUT_REDIRECTION;
				*cmdArgv = '\0';
				srcPtr++;
				if(*srcPtr == '>'){
					*modePtr = OUTPUT_APP;
					srcPtr++;
				}
				while(*srcPtr == ' ' || *srcPtr == '\t')
					srcPtr++;
				*supplementPtr = srcPtr;
				chop(*supplementPtr);
				terminate = 1;
				break;
			case '|':
				*modePtr = PIPELINE;
				*cmdArgv = '\0';
				srcPtr++;
				while(*srcPtr == ' ' || *srcPtr == '\t')
					srcPtr++;
				*supplementPtr = srcPtr;
				terminate = 1;
				break;
			}
			srcPtr++;
		}
	
		while((*srcPtr == ' ' || *srcPtr == '\t' || *srcPtr == '\n') && terminate == 0){
			*srcPtr = '\0';
			srcPtr++;
		}
		cmdArgv++;
	}
	*cmdArgv = '\0';
	return cmdArgc;
}


 
void chop(char *srcPtr){
	while(*srcPtr != ' ' && *srcPtr != '\t' && *srcPtr != '\n'){
		srcPtr++;
	}
	*srcPtr = '\0';
}


 
void execute(char **cmdArgv, int mode, char **supplementPtr){
	pid_t pid, pid2;
	FILE *fp;
	int mode2 = NORMAL, cmdArgc, status1, status2;
	char *cmdArgv2[INPUT_STRING_SIZE], *supplement2 = NULL;
	int myPipe[2];
	if(mode == PIPELINE){
		if(pipe(myPipe)){	//create pipe
			fprintf(stderr, "Pipe failed!");
			exit(-1);
		}
		parse(*supplementPtr, cmdArgv2, &supplement2, &mode2);
	}
	
	pid = fork();

	if( pid < 0){
		printf("Error occured");
		exit(-1);
	}

	else if(pid == 0){
		switch(mode){
			case OUTPUT_REDIRECTION:
				fp = fopen(*supplementPtr, "w+");
				dup2(fileno(fp), 1);
				break;
			case OUTPUT_APP:
				fp = fopen(*supplementPtr, "a");
				dup2(fileno(fp), 1);
				break;
			case PIPELINE:
				close(myPipe[0]);	//close input of pipe
				dup2(myPipe[1], fileno(stdout));
				close(myPipe[1]);
				break;
		}
		execvp(*cmdArgv, cmdArgv);
	}
	
}

// ls function--------------------------------------------------------------------------------------------

extern int alphasort(); //Inbuilt sorting function
static char perms_buff[30];
char pathname[MAXPATHLEN];
void die(char *msg){
	perror(msg);
	exit(0);
}

int file_selecto(struct direct *entry){ /*function to select other than hidden files ex: .c, .h, .o */
char *ptr;
char *rindex(const char *s, int c );
if ((strcmp(entry->d_name, ".")== 0) ||(strcmp(entry->d_name, "..") == 0)) return (FALSE);
	ptr = rindex(entry->d_name, '.');
if ((ptr != NULL) && ((strcmp(ptr, ".c") == 0) ||(strcmp(ptr, ".h") == 0) ||(strcmp(ptr, ".o") == 0) ))
	return (TRUE);
else
	return(FALSE);
}


const char *get_perms(mode_t mode){ /* mode type */
	char ftype = '?';
	if (S_ISREG(mode)) ftype = '-'; /*checking the flags type */
	if (S_ISLNK(mode)) ftype = 'l';
	if (S_ISDIR(mode)) ftype = 'd';
	if (S_ISBLK(mode)) ftype = 'b';
	if (S_ISCHR(mode)) ftype = 'c';
	if (S_ISFIFO(mode)) ftype = '|';
	sprintf(perms_buff, "%c%c%c%c%c%c%c%c%c%c %c%c%c", ftype,
	mode & S_IRUSR ? 'r' : '-', /* readable */
	mode & S_IWUSR ? 'w' : '-', /* writable */
	mode & S_IXUSR ? 'x' : '-', /* excutable */
	mode & S_IRGRP ? 'r' : '-',
	mode & S_IWGRP ? 'w' : '-',
	mode & S_IXGRP ? 'x' : '-',
	mode & S_IROTH ? 'r' : '-',
	mode & S_IWOTH ? 'w' : '-',
	mode & S_IXOTH ? 'x' : '-',
	mode & S_ISUID ? 'U' : '-',
	mode & S_ISGID ? 'G' : '-',
	mode & S_ISVTX ? 'S' : '-');
	return (const char *)perms_buff;
}


static int
one (const struct dirent *unused)
{
	return 1;
}


int file_select(struct direct *entry){ /*selection of hidden files */
	if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))
		return (FALSE);
	else
		return (TRUE);
}


int excecute_a(){ /* function for excecuting ls-a */
int count,i;
struct direct **files;
if(!getcwd(pathname, sizeof(pathname)))
	die("Error getting pathname\n");
printf("Current Working Directory = %s\n",pathname);
count = scandir(pathname, &files, file_select, alphasort);
/* If no files found, make a non-selectable menu item */
if(count <= 0)
die("No files in this directory\n");
printf("Number of files = %d\n",count);
for (i=1; i<count+1; ++i)
	printf("%s\n ",files[i-1]->d_name);
printf("\n"); /* flush buffer */
exit(0);
}



int excecute_l(){ /* function for excecuting ls -l */
int count,i;
struct direct **files;
struct stat statbuf;
char datestring[256];
struct passwd pwent;
struct passwd *pwentp;
struct group grp;
struct group *grpt;
struct tm time;
char buf[1024];
count = scandir(pathname, &files, file_selecto, alphasort);
if(count > 0){
	printf("total %d\n",count);
	printf("pathname name is = %s\n" , pathname);
	for (i=0; i<count; ++i){
		if (!getpwuid_r(statbuf.st_uid, &pwent, buf, sizeof(buf), &pwentp))
			printf(" %s", pwent.pw_name);
		else
			printf(" %d", statbuf.st_uid); /* User ID of the file's owner */
		if (!getgrgid_r (statbuf.st_gid, &grp, buf, sizeof(buf), &grpt))
			printf(" %s", grp.gr_name);
		else
			printf(" %d", statbuf.st_gid); /* Group ID of the file's group */
		printf(" %5d", (int)statbuf.st_size); /* Print size of file. */
		localtime_r(&statbuf.st_mtime, &time); /* Time of last data modification */
		strftime(datestring, sizeof(datestring), "%F %T", &time); /* Get localized date string. */
		printf(" %s %s\n", datestring, files[i]->d_name);
		free (files[i]);
	}
	free(files);
}
exit(0);
}


int excecute_lp(){ /* function for excecuting ls -l */
	int count,i;
	struct direct **files;
	struct stat statbuf;
	char datestring[256];
	struct passwd pwent;
	struct passwd *pwentp;
	struct group grp;
	struct group *grpt;
	struct tm time;
	char buf[1024];
	if(!getcwd(pathname, sizeof(pathname)))
		die("Error getting pathnamen");
	count = scandir(pathname, &files, file_selecto, alphasort);
	if(count > 0){
		printf("total %d\n",count);
		for (i=0; i<count; ++i){
			if (stat(files[i]->d_name, &statbuf) == 0){	/* Print out type, permissions, and number of links. */
				printf("%10.10s", get_perms(statbuf.st_mode)); /* File mode (type, perms) */
				printf(" %d", statbuf.st_nlink); /* Number of links */
				if (!getpwuid_r(statbuf.st_uid, &pwent, buf, sizeof(buf), &pwentp))
					printf(" %s", pwent.pw_name);
				else
					printf(" %d", statbuf.st_uid); /* User ID of the file's owner */
				if (!getgrgid_r (statbuf.st_gid, &grp, buf, sizeof(buf), &grpt))
					printf(" %s", grp.gr_name);
				else
					printf(" %d", statbuf.st_gid); /* Group ID of the file's group */
				printf(" %5d", (int)statbuf.st_size); /* Print size of file. */
				localtime_r(&statbuf.st_mtime, &time); /* Time of last data modification */
				strftime(datestring, sizeof(datestring), "%F %T", &time); /* Get localized date string. */
				printf(" %s %s\n", datestring, files[i]->d_name);
			}
			free (files[i]);
		}
		free(files);
	}
exit(0);
}



// ls function finish--------------------------------------------------------------------------------


//ps function starts --------------------------------------------------------------------------------

int check_if_number (char *str){
	int i;
	for (i=0; str[i] != '\0'; i++){
		if (!isdigit (str[i])){
			return 0;
		}
	}
	return 1;
}


const char *getUserName(int uid){
	struct passwd *pw = getpwuid(uid);
	if (pw){
		return pw->pw_name;
	}
	return "";
}


void pidaux(){
	DIR *dirp;
	FILE *fp;
	struct dirent *entry;
	char path[MAX_BUF], read_buf[MAX_BUF],temp_buf[MAX_BUF];
	char uid_int_str[INT_SIZE_BUF]={0},*line;
	char uptime_str[UP_TIME_SIZE];
	char *user,*command;
	size_t len=0;
	dirp = opendir ("/proc/");
	if (dirp == NULL){
		perror ("Fail");
		exit(0);
	}
	strcpy(path,"/proc/");
	strcat(path,"uptime");
	fp=fopen(path,"r");
	if(fp!=NULL){
		getline(&line,&len,fp);
		sscanf(line,"%s ",uptime_str);
	}
	long uptime=atof(uptime_str);
	long Hertz=sysconf(_SC_CLK_TCK);
	strcpy(path,"/proc/");
	strcat(path,"meminfo");
	fp=fopen(path,"r");
	unsigned long long total_memory;
	if(fp!=NULL){
		getline(&line,&len,fp);
		sscanf(line,"MemTotal: %llu kB",&total_memory);
	}	
	while ((entry = readdir (dirp)) != NULL){
		if (check_if_number (entry->d_name)){
			strcpy(path,"/proc/");
			strcat(path,entry->d_name);
			strcat(path,"/status");
			unsigned long long memory_rss;
			fp=fopen(path,"r");
			unsigned long long vmsize;
			if(fp!=NULL){
				vmsize=0;
				getline(&line,&len,fp);
				getline(&line,&len,fp);
				getline(&line,&len,fp);
				getline(&line,&len,fp);
				getline(&line,&len,fp);
				getline(&line,&len,fp);
				getline(&line,&len,fp);
				getline(&line,&len,fp);
				sscanf(line,"Uid: %s ",uid_int_str);
				getline(&line,&len,fp);
				getline(&line,&len,fp);
				getline(&line,&len,fp);
				getline(&line,&len,fp);
				getline(&line,&len,fp);
				sscanf(line,"VmSize: %llu kB",&vmsize);
				getline(&line,&len,fp);
				getline(&line,&len,fp);
				getline(&line,&len,fp);
				getline(&line,&len,fp);
				sscanf(line,"VmRSS: %llu kB",&memory_rss);
			}
			else{
				fprintf(stdout,"FP is NULL\n");
			}
			float memory_usage=100*memory_rss/total_memory;
			strcpy(path,"/proc/");
			strcat(path,entry->d_name);
			strcat(path,"/stat");
			fp=fopen(path,"r");
			getline(&line,&len,fp);
			char comm[10],state;
			unsigned int flags;
			int pid,ppid,pgrp,session,tty_nr,tpgid;
			unsigned long minflt,cminflt,majflt,cmajflt,utime,stime;
			unsigned long long starttime;
			long cutime,cstime,priority,nice,num_threads,itreavalue;
			sscanf(line,"%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu",&pid,comm,&state,&ppid,&pgrp,&session,&tty_nr,&tpgid,&flags,&minflt,&cminflt,&majflt,&cmajflt,&utime,&stime,&cutime,&cstime,&priority,&nice,&num_threads,&itreavalue,&starttime);
			unsigned long total_time=utime+stime;
			total_time=total_time+(unsigned long)cutime+(unsigned long)cstime;
			float seconds=uptime-(starttime/Hertz);
			float cpu_usage=100*((total_time/Hertz)/seconds);
			if(isnan(cpu_usage)){
				cpu_usage=0.0;
			}
			if(isnan(memory_usage)){
				memory_usage=0.0;
			}
			strcpy (path, "/proc/");
			strcat (path, entry->d_name);
			strcat (path, "/comm");
			fp = fopen (path, "r");
			if (fp != NULL){
				fscanf (fp, "%s", read_buf);
				fclose(fp);
			}
			char *userName=getUserName(atoi(uid_int_str));
			if(strlen(userName)<9){
				user=userName;	
			}
			else{
				user=uid_int_str;
			}
			fprintf(stdout,"%s %s %0.1f %0.1f %llu %llu %c %s\n",user,entry >d_name,cpu_usage,memory_usage,vmsize,memory_rss,state,read_buf);
		}
	}
closedir (dirp);
}

//ps function end ----------------------------------------------------------------------------------------------------------



int main(int argc, char *argv[]){
	int i, mode = NORMAL, cmdArgc;
	size_t len = INPUT_STRING_SIZE;
	char *cpt, *inputString, *cmdArgv[INPUT_STRING_SIZE], *supplement = NULL;
	inputString = (char*)malloc(sizeof(char)*INPUT_STRING_SIZE);
	while(1){
		mode = NORMAL;
		printf("OSassignment_shell-> $");
		getline( &inputString, &len, stdin);
		if(strcmp(inputString, "exit\n")== 0)
			exit(0);
		cmdArgc = parse(inputString, cmdArgv, &supplement, &mode);
		if(strcmp(*cmdArgv, "cd") == 0){
			chdir(cmdArgv[1]);
		}
		else if(strcmp(*cmdArgv,"ls")==0){
				execute_ls();
		}
		else if (strcmp(*cmdArgv,"ls-a")==0){
				excecute_a();
		}
		else if(strcmp(*cmdArgv,"ls-l")==0){
				excecute_lp();
		}
		else if(strcmp(*cmdArgv,"ps")==0){
				pidaux();
		}

	
		else
			execute(cmdArgv, mode, &supplement);//cmdargv-total input string, mode-e.g -elf -l, supplement- check the pipe supplement
	}
	return 0;
}
