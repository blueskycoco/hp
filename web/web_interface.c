#include "web_interface.h"
#include "weblib.h"

char g_url[10][256]={0};
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
int send_web(char *url,char *commandid,char *message,int timeout)
{
	char *request[1024]={0};
	int result=0;
	sprintf(request,"%s?%s&%s",url,commandid,message);
	char *rcv=http_get(request,timeout);
	if(rcv!=NULL)
	{
		printf("%s\n",rcv);
		char *res=doit(rcv,"code");
		if(res)
		{
			printf(LOG_PREFX"code is %s\n",res);
			if(atoi(res)==0)
				result=1;
			free(res);
		}
		free(rcv);
	}
	return result;
}
void get_param(char *input,char *message,char *commandid,int *websiteid,int *timeout)
{
	int j=0;
	int i=strlen(input)-1;
	memset(message,'\0',sizeof(message));
	memset(commandid,'\0',sizeof(commandid)); 				
	strcpy(message,"message=");
	strcpy(commandid,"commandID=");
	*timeout=-1;
	*websiteid=-1;
	*timeout=atoi(strrchr(input,';')+1);
	 while(input[i]!=';' && i=0)
	 	i--;
	 i--;
	 *websiteid=input[i]-48;
	 i=i-2;
	 j=i;
	 while(input[i]!=';' && i=0)
	 	i--;
	 i--;
	 memcpy(commandid+strlen("commandID="),input+i,j-i+1);
	 memcpy(message+strlen("message="),input,i-1);
}
int get_server_cmd(char *url)
{
	int result=0;
	char *text_out[512]={0};
	char *rcv=http_get(url,3);
	if(rcv!=NULL)
	{
		printf("%s\n",rcv);
		char *res=doit(rcv,"code");
		if(res)
		{
			printf(LOG_PREFX"code is %s\n",res);
			if(atoi(res)==0)
				result=1;
			free(res);
		}
		if(result)
		{
			char *lampCode=doit(rcv,"commandid");
			char *commandid=doit(rcv,"message");
			char *commandCode=doit(rcv,"commandCode");
			memset(text_out,'\0',sizeof(text_out));
			if(commandCode)
			{
				memcpy(text_out,commandCode,3);
				strcat(text_out,"w;");			
				strcat(text_out,commandCode+3);
			}
			if(lampCode)
			{
				strcat(text_out,";");
				strcat(text_out,lampCode);
			}
			if(commandid)
			{
				strcat(text_out,";");
				strcat(text_out,commandid);
			}
			if(strlen(text_out)!=0)
				send_msg(msgid,TYPE_WEB_TO_MAIN,WEB_TO_MAIN,text_out);
		}
		free(rcv);
	}
	return result;
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
		get_server_cmd(g_url[0]);
	}
	else if(fpid>0)
	{
		char lampcode[256]={0};
		char commandid[256]={0};
		char errorMsg[256]={0};
		int websiteid=0;
		int timeout=0;
		int i=0,j=0;
		char message[256]={0};
		while(1)
		{
			printf(LOG_PREFX"waiting MainCtlSystem cmd...\n");
			if(msgrcv(msgid, (void*)&data, sizeof(struct msg_st)-sizeof(long int), TYPE_MAIN_TO_WEB , 0)>=0)
			{			
				memset(errorMsg,'\0',256);
				strcpy(errorMsg,data.text);
				if(data.id==MAIN_TO_WEB)
				{
					if(data.text[0]=='f' && data.text[1]=='f')
					{
						int m=3;
						for(i=0;i<10;i++)
						{
							j=m;
							while(data.text[m]!=';' && data.text[m]!='\0')
								m++;
							if(data.text[m]=='\0')
								break;
							else
							{								
								memset(g_url[i],'\0',256);
								memcpy(g_url[i],data.text+j,m-j);
								m++;
							}
						}
						strcat(errorMsg,";done");
						send_msg(msgid,TYPE_WEB_TO_MAIN,WEB_TO_MAIN,errorMsg);
					}
					else
					{
						get_param(data.text,message,commandid,&websiteid,&timeout);
						printf(LOG_PREFX"input %s\nmessage %s\ncommandid %s\nwebsiteid %d\ntimeout %d\n",
							data.text,message,commandid,websiteid,timeout);
						if(send_web(g_url[websiteid],commandid,message,timeout))
							strcat(errorMsg,";done");
						else
							strcat(errorMsg,";failed");
							send_msg(msgid,TYPE_WEB_TO_MAIN,WEB_TO_MAIN,errorMsg);
					}
				}
				else
				{
					strcat(errorMsg,";errorID");
					send_msg(msgid,TYPE_WEB_TO_MAIN,WEB_TO_MAIN,errorMsg);
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

