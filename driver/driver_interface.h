#include <unistd.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>  
#include <errno.h>  
#include <sys/msg.h>  
#include <signal.h>
#include <fnmatch.h> 
#include <termios.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <dirent.h>

#define PERM S_IRUSR|S_IWUSR 
#define TYPE_MAIN_TO_DRIVER 				0x06
#define TYPE_DRIVER_TO_MAIN 				0x05

#define MAIN_TO_DRIVER 					0x05
#define DRIVER_TO_MAIN 					0x09
#define LOG_PREFX						"[DriverSubSystem]:"

struct msg_st  
{  
	long int msg_type; 
	int id;
	char text[512];  
}; 

