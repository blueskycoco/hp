#include "web_interface.h"

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
int send_web(char *url,char *commandid,char *lampcode,int timeout)
{
	char *rcv=http_get(argv[2]/*"http://101.200.236.69:8080/lamp/lamp/commond/wait?lampCode=aaaa"*/,atoi(argv[1]));
	if(rcv!=NULL)
	{
		printf("%s\n",rcv);
		res=doit(rcv,"code");
		if(res)
		{
			printf(LOG_PREFX"code is %s\n",res);
			free(res);
		}
		free(rcv);
	}
}
int main(int argc, char *argv[])
{

	pid_t fpid;	
	int status;
	int msgid = -1;
	unsigned char *rec_result=NULL;
	char res[256]={0};
	char record_file[256]={0};
	char playback_file[256]={0};
	struct msg_st data;	
	char err_msg[256]={0};
	char text_out[256]={0};
	char last_check=0;
	char file_name[256]={0};
	char operation=0;//0 for read ,1 for write
	int offs=0;
	int len;

	msgid = msgget((key_t)1234, 0666 | IPC_CREAT);  
	if(msgid == -1)  
	{  
		fprintf(stderr, LOG_PREFX"msgget failed with error: %d\n", errno);  
		exit(-1);  
	}
	else
		printf(LOG_PREFX"msgid %d\n",msgid);		
	fpid=fork();
	if(fpid==0)
	{

	}
	else if(fpid>0)
	{
		char lampcode[256]={0};
		char commandid[256]={0};
		int websiteid=0;
		int timeout=0;
		char key[256]={0};
		char value[256]={0};
		while(1)
		{
			printf(LOG_PREFX"waiting MainCtlSystem cmd...\n");
			if(msgrcv(msgid, (void*)&data, sizeof(struct msg_st)-sizeof(long int), TYPE_MAIN_TO_FILE , 0)>=0)
			{
				if(data.id==MAIN_TO_WEB)
				{
					if(data.text[0]=='0' && data.text[1]=='1')
					{
						
					}
					else if(data.text[0]=='0' && data.text[1]=='2')
				}
				
			}
			else
			{
				msgid = msgget((key_t)1234, 0666 | IPC_CREAT);  
				if(msgid == -1)  
				{  
					fprintf(stderr, LOG_PREFX"msgget failed with error: %d\n", errno);  
					sleep(1);
				}
				else
					printf(LOG_PREFX"msgid %d\n",msgid);
			}	
		}
	}
	waitpid(fpid, &status, 0);	
	return 0;
}

