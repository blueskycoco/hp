#include "driver_interface.h"
int send_msg(int msgid,unsigned char msg_type,unsigned char id,unsigned char *text)
{
	struct msg_st data;
	data.msg_type = msg_type;
	data.id=id;
	memset(data.text,'\0',512);
	printf(LOG_PREFX"send msg\n");
	printf(LOG_PREFX"MSG_TYPE %d\n",msg_type);
	printf(LOG_PREFX"MSG_ID %d\n",id);
	if(text!=NULL)
	{
		memcpy(data.text,text,strlen(text));
		printf(LOG_PREFX"MSG_TEXT %s\n",text);
	}
	if(msgsnd(msgid, (void*)&data, sizeof(struct msg_st)-sizeof(long int), IPC_NOWAIT) == -1)  
	{  
		fprintf(stderr, LOG_PREFX"msgsnd failed %s\n",strerror(errno));
		system("ipcs -q");
	}
	printf(LOG_PREFX"send msg done\n");
}
int read_uart(int fd, char *rcv_buf,int data_len)
{
    int len,fs_sel,i=0,get_start=0,get_stop=0;
	char ch;
    fd_set fs_read;
   
    struct timeval time;
   
    FD_ZERO(&fs_read);
    FD_SET(fd,&fs_read);
   
    time.tv_sec = 10;
    time.tv_usec = 0;
    while(1)
    { 
    	//fs_sel = select(fd+1,&fs_read,NULL,NULL,&time);
    	//if(fs_sel)
   		{
   			if(read(fd,&ch,1)!=0)
   			{
				if(get_start)
				{
					rcv_buf[i++]=ch;					
					if(ch=='#')
					{						
						printf(LOG_PREFX"get stop %s\n",rcv_buf);
						break;
					}
				}
				else
				{
					if(ch=='*')
					{
						get_start=1;
						i=1;
						rcv_buf[0]=ch;
						printf(LOG_PREFX"get start %c\n",ch);
					}
				}
   			}
          /* len=read(fd,rcv_buf+i,512);
	   printf(LOG_PREFX"==>%s %d %c\n",rcv_buf,i,rcv_buf[i+len-1]);
	   if(get_start==1)
	{
		if(rcv_buf[i+len-1]=='#')
			break;
		else
			i+=len;
	}
	   if(rcv_buf[0]=='*')
	   {
		get_start=1;
		i++;
	   }*/
	   

   		}
    }
    return i;
}

int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
	struct termios newtio,oldtio;
	if  ( tcgetattr( fd,&oldtio)  !=  0) { 
		perror("SetupSerial 1");
		return -1;
	}
	bzero( &newtio, sizeof( newtio ) );
	newtio.c_cflag  |=  CLOCAL | CREAD; 
	newtio.c_cflag &= ~CSIZE; 

	switch( nBits )
	{
		case 7:
			newtio.c_cflag |= CS7;
			break;
		case 8:
			newtio.c_cflag |= CS8;
			break;
	}

	switch( nEvent )
	{
		case 'O':
			newtio.c_cflag |= PARENB;
			newtio.c_cflag |= PARODD;
			newtio.c_iflag |= (INPCK | ISTRIP);
			break;
		case 'E': 
			newtio.c_iflag |= (INPCK | ISTRIP);
			newtio.c_cflag |= PARENB;
			newtio.c_cflag &= ~PARODD;
			break;
		case 'N':  
			newtio.c_cflag &= ~PARENB;
			break;
	}

	switch( nSpeed )
	{
		case 2400:
			cfsetispeed(&newtio, B2400);
			cfsetospeed(&newtio, B2400);
			break;
		case 4800:
			cfsetispeed(&newtio, B4800);
			cfsetospeed(&newtio, B4800);
			break;
		case 9600:
			cfsetispeed(&newtio, B9600);
			cfsetospeed(&newtio, B9600);
			break;
		case 115200:
			cfsetispeed(&newtio, B115200);
			cfsetospeed(&newtio, B115200);
			break;
		default:
			cfsetispeed(&newtio, B9600);
			cfsetospeed(&newtio, B9600);
			break;
	}
	if( nStop == 1 )
		newtio.c_cflag &=  ~CSTOPB;
	else if ( nStop == 2 )
		newtio.c_cflag |=  CSTOPB;
	newtio.c_cc[VTIME]  = 0;
	newtio.c_cc[VMIN] = 0;
	tcflush(fd,TCIFLUSH);
	if((tcsetattr(fd,TCSANOW,&newtio))!=0)
	{
		perror("com set error");
		return -1;
	}
	printf("set done!\n");
	return 0;
}

int open_com_port()
{
	int fd;
	long  vdisable;
		
	fd = open( "/dev/ttySAC3", O_RDWR|O_NOCTTY|O_NDELAY);
	if (-1 == fd){
		perror("Can't Open Serial Port0");
		return(-1);
	}
	else 
		printf("open tts/0 .....\n");

	if(fcntl(fd, F_SETFL, FNDELAY)<0)
		printf("fcntl failed!\n");
	else
		printf("fcntl=%d\n",fcntl(fd, F_SETFL,FNDELAY));
	if(isatty(STDIN_FILENO)==0)

		printf("standard input is not a terminal device\n");
	else
		printf("isatty success!\n");
	printf("fd-open=%d\n",fd);
	return fd;
}

int main(int argc, char *argv[])
{

	pid_t fpid;	
	int status;
	int fd_com=0;
	int msgid = -1;
	unsigned char *rec_result=NULL;
	char res[256]={0};
	struct msg_st data;

	msgid = msgget((key_t)1234, 0666 | IPC_CREAT);  
	if(msgid == -1)  
	{  
		fprintf(stderr, LOG_PREFX"msgget failed with error: %d\n", errno);  
		exit(-1);  
	}
	else
		printf(LOG_PREFX"msgid %d\n",msgid);	
	if((fd_com=open_com_port())<0)
	{
		perror("open_port error");
		return -1;
	}
	if(set_opt(fd_com,115200,8,'N',1)<0)
	{
		perror(" set_opt error");
		return -1;
	}
	fpid=fork();
	if(fpid==0)
	{
		char rcv_buf[512];
		while(1)
		{//mon uart 
			memset(rcv_buf,'\0',512);
			if(read_uart(fd_com,rcv_buf,512)!=0)
			{
				printf(LOG_PREFX"MCU==>%s\n",rcv_buf);
				send_msg(msgid,TYPE_DRIVER_TO_MAIN,DRIVER_TO_MAIN,rcv_buf);
			}
		}
	}
	else if(fpid>0)
	{
		while(1)
		{		
			printf(LOG_PREFX"waiting MainCtlSystem cmd...\n");
			if(msgrcv(msgid, (void*)&data, sizeof(struct msg_st)-sizeof(long int), TYPE_MAIN_TO_DRIVER , 0)>=0)
			{
				printf(LOG_PREFX"msgtype %ld ,data id %d,text %s\n",data.msg_type,data.id,data.text);
				if(data.id==MAIN_TO_DRIVER)
				{
					printf(LOG_PREFX"Driver==>%s\n",data.text);
					write(fd_com,data.text,strlen(data.text));
				}
			}
			else
			{
				msgid = msgget((key_t)1234, 0666 | IPC_CREAT);  
				if(msgid == -1)  
				{  
					fprintf(stderr, LOG_PREFX"msgget failed with error: %d\n", errno);  
					//exit(-1);  
				}
				else
				printf(LOG_PREFX"msgid %d\n",msgid);			
				sleep(1);
			}
		}
	}
	waitpid(fpid, &status, 0);
	return 0;
}

