#include "file_interface.h"

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
int read_file_line(char *file,int line_addr,char prv,char *out)
{
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	int result=0;
	int line_cnt=0;
	ssize_t read;

	fp = fopen(file, "r");
	if (fp == NULL)
	   exit(-1);

	while ((read = getline(&line, &len, fp)) != -1) {
	   printf("Retrieved line of length %zu :\n", read);
	   printf("%s", line);
	   if(line_cnt==line_addr)
	   {
	   		if(prv)
				strcpy(out,strchr(line,';')+1);
			else
			{
				int i=0;
				while(line[i]!='\n' && line[i]!=';')
				{
					out[i]=line[i];
					i++;
				}
				
			}
			printf(LOG_PREFX"get %s\n",out);
			result=1;
			free(line);
			break;
	   }
	   else
	   		free(line);
	   line_cnt++;
	}

	fclose(fp);
	if (line)
	   free(line);
	return result;
}
int set_music_like(char *file,char *music_id,int like)
{
	char buf[256]={0};
	int result=0;
	char * line = NULL;
	size_t len = 0;
	int i;
	int write_pos=0,read=0,found=0;
	FILE *fp=fopen(file,"r");
	fseek(fp,0L,SEEK_END);
	int flen=ftell(fp);
	char *file_write;
	file_write=(char *)malloc(flen);	
	while ((read = getline(&line, &len, fp)) != -1) 
	{	
		if(strncmp(line,music_id,strlen(music_id))==0)
		{		 
		  found=1;
		  i=0;
	   	}
		if(found==1)
			i++;
		if(i==4)
		{
			//set like or unlike
			char *str=strchr(line,';');
			if(like)
				str[1]='1';
			else
				str[1]='2';
			found=0;
			i=0;
		}
	   memcpy(file_write+write_pos,line,read);
 	   write_pos=write_pos+read;
	   free(line);
	}
	fclose(fp);
	fp=fopen(file,"w");
	fwrite(file_write,write_pos,1,fp);	
	fclose(fp);
	result=1;
	
	return result;	
}
int set_light_use(char *file,char *mode_id,int like)
{
	char buf[256]={0};
	int result=0;
	char * line = NULL;
	size_t len = 0;
	int i;
	int write_pos=0,read=0,found=0;
	FILE *fp=fopen(file,"r");
	fseek(fp,0L,SEEK_END);
	int flen=ftell(fp);
	char *file_write;
	file_write=(char *)malloc(flen);	
	while ((read = getline(&line, &len, fp)) != -1) 
	{	
		if(strncmp(line,mode_id,strlen(mode_id))==0)
		{		 
		  found=1;
		  i=0;
	   	}
		if(found==1)
			i++;
		if(i==6)
		{
			//set use or unuse
			char *str=strchr(line,';');
			if(like)
				str[1]='0';
			else
				str[1]='1';
			found=0;
			i=0;
		}
	   memcpy(file_write+write_pos,line,read);
 	   write_pos=write_pos+read;
	   free(line);
	}
	fclose(fp);
	fp=fopen(file,"w");
	fwrite(file_write,write_pos,1,fp);
	fclose(fp);
	result=1;
	
	return result;	
}
int set_year_time(char *file,char *year,char *time)
{
	char buf[256]={0};
	int result=0;
	char * line = NULL;
	size_t len = 0;
	int i;
	int write_pos=0,read=0,found=0;
	FILE *fp=fopen(file,"r");
	fseek(fp,0L,SEEK_END);
	int flen=ftell(fp);
	char *file_write;
	file_write=(char *)malloc(flen);	
	while ((read = getline(&line, &len, fp)) != -1) 
	{	
		if(i==7)
		{
			strcpy(line+4,time);
		}
		if(i==8)
		{
			strcpy(line+4,year);
		}
    	i++;		
	   memcpy(file_write+write_pos,line,read);
 	   write_pos=write_pos+read;
	   free(line);
	}
	fclose(fp);
	fp=fopen(file,"w");
	fwrite(file_write,write_pos,1,fp);
	fclose(fp);
	result=1;
	
	return result;	
}

int set_alarm_on(char *file,char *alarm_id,int like)
{
	char buf[256]={0};
	int result=0;
	char * line = NULL;
	size_t len = 0;
	int i;
	int write_pos=0,read=0,found=0;
	FILE *fp=fopen(file,"r");
	fseek(fp,0L,SEEK_END);
	int flen=ftell(fp);
	char *file_write;
	file_write=(char *)malloc(flen);	
	while ((read = getline(&line, &len, fp)) != -1) 
	{	
		if(strncmp(line,alarm_id,strlen(alarm_id))==0)
		{		 
		  found=1;
		  i=0;
	   	}
		if(found==1)
			i++;
		if(i==3)
		{
			//set use or unuse
			char *str=strchr(line,';');
			if(like)
				str[1]='a';
			else
				str[1]='b';
			found=0;
			i=0;
		}
	   memcpy(file_write+write_pos,line,read);
 	   write_pos=write_pos+read;
	   free(line);
	}
	fclose(fp);
	fp=fopen(file,"w");
	fwrite(file_write,write_pos,1,fp);
	fclose(fp);
	result=1;
	
	return result;	
}

int write_file_line(char *file,char *alarm_id,char *str)
{
	char buf[256]={0};
	int result=0;
	char * line = NULL;
	size_t len = 0;
	char last[3]="/r/n";
	int write_pos=0,read=0,found=0;
	FILE *fp=fopen(file,"r");
	fseek(fp,0L,SEEK_END);
	int flen=ftell(fp);
	char *file_write;
	file_write=(char *)malloc(flen+strlen(str)+1);	
	while ((read = getline(&line, &len, fp)) != -1) 
	{	
		if(strncmp(line,alarm_id,strlen(alarm_id))==0)
		{
		  memcpy(file_write+write_pos,alarm_id,strlen(alarm_id));
		  file_write[write_pos+strlen(alarm_id)]=';';
		  memcpy(file_write+write_pos+stlen(alarm_id)+1,str,strlen(str));
		  memcpy(file_write+write_pos+stlen(alarm_id)+1+strlen(str),last,2);
		  write_pos=write_pos+strlen(str)+strlen(alarm_id)+1+2;
		  found=1;
	   }
	   else
	   {
		   memcpy(file_write+write_pos,line,read);
	 	   write_pos=write_pos+read;
	   }
	   free(line);
	}
	fclose(fp);
	fp=fopen(file,"w");
	fwrite(file_write,write_pos,1,fp);	
	if(found==0)
	{
		fwrite(alarm_id,strlen(alarm_id),1,fp);
		fwrite(';',1,1,fp);
		fwrite(str,strlen(str),1,fp);
	}
	fclose(fp);
	result=1;
	
	return result;	
}
int write_alert(char *file,int found,char *alarm_id,char *alarm_name,char *alarm_day,char *alarm_time,char *alarm_up,char *alarm_freq)
{
	char buf[256]={0};
	int result=0;
	char * line = NULL;
	size_t len = 0;
	char last[3]="/r/n";
	int write_pos=0,read=0;
	FILE *fp=fopen(file,"r");
	fseek(fp,0L,SEEK_END);
	int flen=ftell(fp);
	char *file_write;
	if(found)		
		file_write=(char *)malloc(flen+strlen(alarm_name)+strlen(alarm_day)+strlen(alarm_time)+strlen(alarm_up)+strlen(alarm_freq)+1);		
	else
		file_write=(char *)malloc(flen+5*strlen(alarm_id)+strlen(alarm_name)+strlen(alarm_day)+strlen(alarm_time)+strlen(alarm_up)+strlen(alarm_freq)+1);		
	while ((read = getline(&line, &len, fp)) != -1) 
	{	
		if(found && strncmp(line,alarm_id,strlen(alarm_id))==0)
		{
			char *a_id=strdup(alarm_id);
		  memcpy(file_write+write_pos,a_id,strlen(a_id));
		  file_write[write_pos+strlen(a_id)]=';';
		  memcpy(file_write+write_pos+stlen(a_id)+1,alarm_name,strlen(alarm_name));
		  memcpy(file_write+write_pos+stlen(a_id)+1+stlen(alarm_name),last,2);
		  write_pos=write_pos+strlen(a_id)+1+2+stlen(alarm_name);
		  a_id=a_id+'1';
		  memcpy(file_write+write_pos,a_id,strlen(a_id));
		  file_write[write_pos+strlen(a_id)]=';';
		  memcpy(file_write+write_pos+stlen(a_id)+1,alarm_day,strlen(alarm_day));
		  memcpy(file_write+write_pos+stlen(a_id)+1+stlen(alarm_day),last,2);
		  write_pos=write_pos+strlen(a_id)+1+2+stlen(alarm_day);
		  a_id=a_id+'1';
		  memcpy(file_write+write_pos,a_id,strlen(a_id));
		  file_write[write_pos+strlen(a_id)]=';';
		  memcpy(file_write+write_pos+stlen(a_id)+1,alarm_time,strlen(alarm_time));
		  memcpy(file_write+write_pos+stlen(a_id)+1+stlen(alarm_time),last,2);
		  write_pos=write_pos+strlen(a_id)+1+2+stlen(alarm_time);
		  a_id=a_id+'1';
		  memcpy(file_write+write_pos,a_id,strlen(a_id));
		  file_write[write_pos+strlen(a_id)]=';';
		  memcpy(file_write+write_pos+stlen(a_id)+1,alarm_up,strlen(alarm_up));
		  memcpy(file_write+write_pos+stlen(a_id)+1+stlen(alarm_up),last,2);
		  write_pos=write_pos+strlen(a_id)+1+2+stlen(alarm_up);
		  a_id=a_id+'1';
		  memcpy(file_write+write_pos,a_id,strlen(a_id));
		  file_write[write_pos+strlen(a_id)]=';';
		  memcpy(file_write+write_pos+stlen(a_id)+1,alarm_freq,strlen(alarm_freq));
		  memcpy(file_write+write_pos+stlen(a_id)+1+stlen(alarm_freq),last,2);
		  write_pos=write_pos+strlen(a_id)+1+2+stlen(alarm_freq);
		  free(a_id);
	   }
	   else
	   {
		   memcpy(file_write+write_pos,line,read);
	 	   write_pos=write_pos+read;
	   }
	   free(line);
	}
	fclose(fp);
	
	fp=fopen(file,"w");
	fwrite(file_write,write_pos,1,fp);	
	if(found==0)
	{	
		char line[64]={0};
		char *a_id=strdup(alarm_id);
		strcpy(line,a_id);
		strcat(line,";");
		strcat(line,alarm_name);
		strcat(line,"/r/n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+'1';
		strcpy(line,a_id);
		strcat(line,";");
		strcat(line,alarm_day);
		strcat(line,"/r/n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+'1';
		strcpy(line,a_id);
		strcat(line,";");
		strcat(line,alarm_time);
		strcat(line,"/r/n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+'1';
		strcpy(line,a_id);
		strcat(line,";");
		strcat(line,alarm_up);
		strcat(line,"/r/n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+'1';
		strcpy(line,a_id);
		strcat(line,";");
		strcat(line,alarm_freq);
		strcat(line,"/r/n");
		fwrite(line,strlen(line),1,fp);
		free(a_id);
	}
	fclose(fp);
	result=1;
	
	return result;	
}
int write_alarm(char *file,int found,char *alarm_id,char *alarm_time,char *alarm_freq,char *alarm_vol,char *alarm_blance,char *alarm_change,char *alarm_change_freq)
{
	char buf[256]={0};
	int result=0;
	char * line = NULL;
	size_t len = 0;
	char last[3]="/r/n";
	int write_pos=0,read=0;
	FILE *fp=fopen(file,"r");
	fseek(fp,0L,SEEK_END);
	int flen=ftell(fp);
	char *file_write;
	if(found)		
		file_write=(char *)malloc(flen+strlen(alarm_id)+strlen(alarm_time)+strlen(alarm_freq)+strlen(alarm_vol)+strlen(alarm_blance)+strlen(alarm_change)+strlen(alarm_change_freq)+1);		
	else
		file_write=(char *)malloc(flen+5*strlen(alarm_id)+strlen(alarm_time)+strlen(alarm_freq)+strlen(alarm_vol)+strlen(alarm_blance)+strlen(alarm_change)+strlen(alarm_change_freq)+1);		
	while ((read = getline(&line, &len, fp)) != -1) 
	{	
		if(found && strncmp(line,alarm_id,strlen(alarm_id))==0)
		{
			char *a_id=strdup(alarm_id);
		  memcpy(file_write+write_pos,a_id,strlen(a_id));
		  file_write[write_pos+strlen(a_id)]=';';
		  memcpy(file_write+write_pos+stlen(a_id)+1,alarm_time,strlen(alarm_time));
		  memcpy(file_write+write_pos+stlen(a_id)+1+2+stlen(alarm_time),last,2);
		  write_pos=write_pos+strlen(a_id)+1+2+stlen(alarm_time);
		  a_id=a_id+'1';
		  memcpy(file_write+write_pos,a_id,strlen(a_id));
		  file_write[write_pos+strlen(a_id)]=';';
		  memcpy(file_write+write_pos+stlen(a_id)+1,alarm_freq,strlen(alarm_freq));
		  memcpy(file_write+write_pos+stlen(a_id)+1+2+stlen(alarm_freq),last,2);
		  write_pos=write_pos+strlen(a_id)+1+2+stlen(alarm_freq);
		  a_id=a_id+'1';
		  memcpy(file_write+write_pos,a_id,strlen(a_id));
		  file_write[write_pos+strlen(a_id)]=';';
		  memcpy(file_write+write_pos+stlen(a_id)+1,alarm_vol,strlen(alarm_vol));
		  memcpy(file_write+write_pos+stlen(a_id)+1+2+stlen(alarm_vol),last,2);
		  write_pos=write_pos+strlen(a_id)+1+2+stlen(alarm_vol);
		  a_id=a_id+'1';
		  memcpy(file_write+write_pos,a_id,strlen(a_id));
		  file_write[write_pos+strlen(a_id)]=';';
		  memcpy(file_write+write_pos+stlen(a_id)+1,alarm_blance,strlen(alarm_blance));
		  memcpy(file_write+write_pos+stlen(a_id)+1+2+stlen(alarm_blance),last,2);
		  write_pos=write_pos+strlen(a_id)+1+2+stlen(alarm_blance);
		  a_id=a_id+'1';
		  memcpy(file_write+write_pos,a_id,strlen(a_id));
		  file_write[write_pos+strlen(a_id)]=';';
		  memcpy(file_write+write_pos+stlen(a_id)+1,alarm_change,strlen(alarm_change));
		  memcpy(file_write+write_pos+stlen(a_id)+1+2+stlen(alarm_change),last,2);
		  write_pos=write_pos+strlen(a_id)+1+2+stlen(alarm_change);
		  a_id=a_id+'1';
		  memcpy(file_write+write_pos,a_id,strlen(a_id));
		  file_write[write_pos+strlen(a_id)]=';';
		  memcpy(file_write+write_pos+stlen(a_id)+1,alarm_change_freq,strlen(alarm_change_freq));
		  memcpy(file_write+write_pos+stlen(a_id)+1+2+stlen(alarm_change_freq),last,2);
		  write_pos=write_pos+strlen(a_id)+1+2+stlen(alarm_change_freq);
		  free(a_id);
	   }
	   else
	   {
		   memcpy(file_write+write_pos,line,read);
	 	   write_pos=write_pos+read;
	   }
	   free(line);
	}
	fclose(fp);
	
	fp=fopen(file,"w");
	fwrite(file_write,write_pos,1,fp);	
	if(found==0)
	{	
		char line[64]={0};
		char *a_id=strdup(alarm_id);
		strcpy(line,a_id);
		strcat(line,";");
		strcat(line,alarm_time);
		strcat(line,"/r/n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+'1';
		strcpy(line,a_id);
		strcat(line,";");
		strcat(line,alarm_freq);
		strcat(line,"/r/n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+'1';
		strcpy(line,a_id);
		strcat(line,";");
		strcat(line,alarm_vol);
		strcat(line,"/r/n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+'1';
		strcpy(line,a_id);
		strcat(line,";");
		strcat(line,alarm_blance);
		strcat(line,"/r/n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+'1';
		strcpy(line,a_id);
		strcat(line,";");
		strcat(line,alarm_change);
		strcat(line,"/r/n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+'1';
		strcpy(line,a_id);
		strcat(line,";");
		strcat(line,alarm_change_freq);
		strcat(line,"/r/n");
		fwrite(line,strlen(line),1,fp);
		free(a_id);
	}
	fclose(fp);
	result=1;
	
	return result;	
}

int get_file_lines(char *file)
{
	FILE *fp;
	int line_cnt=0;
	char *line=NULL;
	size_t len = 0;
	fp = fopen(file, "r");
	if (fp == NULL)
	   return line_cnt;

	while (getline(&line, &len, fp) != -1) {
	   printf("%s", line);
	   line_cnt++;
	   free(line);
	}

	fclose(fp);
	return line_cnt;
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
	
	while(1)
	{
		memset(err_msg,'\0',256);
		memset(text_out,'\0',256);
		memset(file_name,'\0',256);
		strcpy(file_name,FILE_PATH_NAME);
		printf(LOG_PREFX"waiting MainCtlSystem cmd...\n");
		if(msgrcv(msgid, (void*)&data, sizeof(struct msg_st)-sizeof(long int), TYPE_MAIN_TO_FILE , 0)>=0)
		{
			if(data.id==MAIN_TO_FILE)
			{
				if(data.text[2]=='0' && data.text[3]=='0')
				{//00
					char buf[25]={0};
					strcat(file_name,data.text+7);
					strcat(file_name,".txt");
					printf(LOG_PREFX"cmd 00 to open %s\n",file_name);
					operation=0;
					memcpy(text_out,data.text,6);
					text_out[0]='d';
					int file_line=get_file_lines(file_name);
					if(file_line==0 || ((file_line%FILE_MUSIC_LINES_RECORD)!=0))
					{
						strcat(text_out,";music-id ");
						strcat(text_out,"unknown");
						strcat(text_out,";music-name ");
						strcat(text_out,"unknown");
						strcat(text_out,";singer ");
						strcat(text_out,"unknown");
						strcat(text_out,";music-like/unlike ");
						strcat(text_out,"unknown");
					}
					else
					{
						int i;
						for(i=0;i<file_line;i=i+FILE_MUSIC_LINES_RECORD)
						{
							strcat(text_out,";music-id ");
							if(read_file_line(file_name,i+FILE_MUSIC_NAME_LINE,0,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
							memset(buf,'\0',25);
							strcat(text_out,";music-name ");
							if(read_file_line(file_name,i+FILE_MUSIC_NAME_LINE,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
							memset(buf,'\0',25);
							strcat(text_out,";singer ");
							if(read_file_line(file_name,i+FILE_MUSIC_SINGER_LINE,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
							memset(buf,'\0',25);
							strcat(text_out,";music-like/unlike ");
							if(read_file_line(file_name,i+FILE_MUSIC_LIKE_LINE,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
						}
						printf(LOG_PREFX"audio list %s\n",text_out);
					}
					memset(file_name,'\0',256);
				}
				else if(data.text[2]=='0' && data.text[3]=='1')
				{//01
					char buf[25]={0};
					char music_id[15]={0};
					len=strlen(FILE_PATH_NAME);
					int web_pos=0;
					file_name[len]=data.text[2];
					file_name[len+1]=data.text[3];
					strcat(file_name,".txt");
					printf(LOG_PREFX"cmd 01 to open %s\n",file_name);
					operation=0;	
					if(fnmatch(CMD_01_MUSIC_PLAY, data.text, FNM_PATHNAME) == 0)
					{
						printf(LOG_PREFX"in normal read\n");
						strcpy(text_out,data.text+2);						
						strcat(text_out,";");
						strcpy(music_id,strrchr(data.text,';')+1);
					}
					else
					{	
						printf(LOG_PREFX"in web read\n");
						strcpy(text_out,"01;w;");
						int i=10;
						while(data.text[i]!=';' && data.text[i]!='\0')
							music_id[i-10]=data.text[i];
						strcat(text_out,music_id);
						strcat(text_out,";");
						web_pos=i+1;
					}
					
					int file_line=get_file_lines(file_name);
					if(file_line<FILE_MUSIC_LINES_RECORD)
					{
						strcat(text_out,"unknown");
					}
					else
					{
						int i;
						for(i=0;i<file_line;i=i+FILE_MUSIC_LINES_RECORD)
						{
							read_file_line(file_name,i+FILE_MUSIC_NAME_LINE,0,buf);
							if(strncmp(buf,music_id,strlen(buf))==0)
							{
								read_file_line(file_name,i+FILE_MUSIC_PATH_LINE,1,buf);
								strcat(text_out,buf);								
								read_file_line(file_name,i+FILE_MUSIC_NAME_LINE,1,buf);
								strcat(text_out,buf);
								if(data.text[0]=='r' &&data.text[5]=='w')
									strcat(text_out,data.text+web_pos);
								printf(LOG_PREFX"get music %s\n",text_out);
								break;
							}
						}
						if(i==file_line)
						{
							strcat(text_out,"unknown");
							printf(LOG_PREFX"cant not find music by id %s\n",strrchr(data.text,';')+1);
						}
					}
				}
				else if(data.text[2]=='0' && data.text[3]=='3')
				{//03
					char alarm_id[10]={0};
					char str[256]={0};
					len=strlen(FILE_PATH_NAME);
					file_name[len]=data.text[7];
					file_name[len+1]=data.text[8];
					strcat(file_name,".txt");
					printf(LOG_PREFX"cmd 03 to open %s\n",file_name);
					int i=10;
					while(data.text[i]!='\0' && data.text[i]!=';')
						i++;
					memcpy(alarm_id,data.text[10],i-10);
					if(data.text[5]!='w')						
						strcpy(str,strrchr(data.text,';')+1);
					else
					{
	                    int j=i;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(str,data.text[j+1],i-j);
					}
					printf(LOG_PREFX"alarm_id %s, str %s \n",alarm_id,str);
					write_file_line(file_name,alarm_id,str);
					if(data.text[5]!='w')
						strcpy(text_out,"g;03;b;0");
					else
					{
						strcpy(text_out,"g;03;w;0;");
						strcat(text_out,data.text+i);
					}
					printf(LOG_PREFX"text_out is %s\n",text_out);
				}				
				else if(data.text[2]=='0' && data.text[3]=='4')
				{//04
					char music_id[10]={0};
					char str[256]={0};
					len=strlen(FILE_PATH_NAME);
					file_name[len]=data.text[7];
					file_name[len+1]=data.text[8];
					strcat(file_name,".txt");
					printf(LOG_PREFX"cmd 04 to open %s\n",file_name);
					int i=10;
					while(data.text[i]!='\0' && data.text[i]!=';')
						i++;
					memcpy(music_id,data.text[10],i-10);
					printf(LOG_PREFX"music_id %s\n",music_id);
					set_music_like(file_name,music_id,1);
					if(data.text[5]!='w')
						strcpy(text_out,"g;04;b;0");
					else
					{
						strcpy(text_out,"g;04;w;0;");
						strcat(text_out,data.text+i);
					}
					printf(LOG_PREFX"text_out is %s\n",text_out);
				}
				else if(data.text[2]=='0' && data.text[3]=='5')
				{//05
					char music_id[10]={0};
					char str[256]={0};
					len=strlen(FILE_PATH_NAME);
					file_name[len]=data.text[7];
					file_name[len+1]=data.text[8];
					strcat(file_name,".txt");
					printf(LOG_PREFX"cmd 05 to open %s\n",file_name);
					int i=10;
					while(data.text[i]!='\0' && data.text[i]!=';')
						i++;
					memcpy(music_id,data.text[10],i-10);
					printf(LOG_PREFX"music_id %s\n",music_id);
					set_music_like(file_name,music_id,0);
					if(data.text[5]!='w')
						strcpy(text_out,"g;05;b;0");
					else
					{
						strcpy(text_out,"g;05;w;0;");
						strcat(text_out,data.text+i);
					}
					printf(LOG_PREFX"text_out is %s\n",text_out);

				}				
				else if((data.text[2]=='0' && data.text[3]=='7')||(data.text[2]=='1' && data.text[3]=='2')||(data.text[2]=='1' && data.text[3]=='4')||(data.text[2]=='2' && data.text[3]=='1' &&data.text[5]=='0' && data.text[6]=='0'))
				{//07
					char buf[25]={0};
					char light_id[10]={0};
					int j=10,found=0;
					len=strlen(FILE_PATH_NAME);
					file_name[len]=data.text[7];
					file_name[len+1]=data.text[8];
					strcat(file_name,".txt");
					printf(LOG_PREFX"cmd 07 or 12 or 14 or 21 to open %s\n",file_name);
					operation=0;
					if(data.text[2]=='0' && data.text[3]=='7')
					{
						if(data.text[5]!='w')
							strcpy(text_out,"d;07;b;");
						else
							strcpy(text_out,"d;07;w;");
					}
					else
					{
						if(data.text[2]=='0' && data.text[3]=='7')
						{
							if(data.text[5]!='w')
								strcpy(text_out,"d;12;b;");
							else
								strcpy(text_out,"d;12;w;");
						}
						else
						{
							if(data.text[2]=='1' && data.text[3]=='4')
							{	
								if(data.text[5]!='w')
									strcpy(text_out,"d;14;b;");
								else
									strcpy(text_out,"d;14;w;");
							}
							else
							{
								
								strcpy(text_out,"d;21;00;");											
								file_name[len]=data.text[5];
								file_name[len+1]=data.text[6];
							}
						}
					}
					if(data.text[5]=='b' || data.text[5]!='w')
						strcpy(light_id,strrchr(data.text,';')+1);
					else
					{
						while(data.text[j]!='\0' && data.text[j]!=';')
							j++;
						memcpy(light_id,data.text[10],j-10);
					}
					int file_line=get_file_lines(file_name);
					if(file_line==0 || ((file_line%FILE_LIGHT_LINES_RECORD)!=0))
					{
						strcat(text_out,";unknown");
						strcat(text_out,";unknown");
						strcat(text_out,";unknown");
						strcat(text_out,";unknown");
						strcat(text_out,";unknown");
					}
					else
					{
						int i;
						for(i=0;i<file_line;i=i+FILE_LIGHT_LINES_RECORD)
						{
							if(read_file_line(file_name,i,0,buf));
							{
									if(strncmp(buf,light_id,strlen(buf))==0)
									{
										found=1;
										break;
									}
							}
						}
						if(found)
						{
							if(read_file_line(file_name,i+1,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
							memset(buf,'\0',25);
							if(read_file_line(file_name,i+2,2,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
							memset(buf,'\0',25);
							if(read_file_line(file_name,i+3,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
							memset(buf,'\0',25);
							if(read_file_line(file_name,i+4,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");	
							memset(buf,'\0',25);
							if(read_file_line(file_name,i+5,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
						}
						else
						{
							strcat(text_out,";unknown");
							strcat(text_out,";unknown");
							strcat(text_out,";unknown");
							strcat(text_out,";unknown");
							strcat(text_out,";unknown");
						}
						if(data.text[5]=='w' &&((data.text[2]=='0' && data.text[3]=='7')||(data.text[2]=='1' && data.text[3]=='4')))
							strcat(text_out,data.text+j);
						}
						printf(LOG_PREFX"light list %s\n",text_out);
					
				}
				else if(data.text[2]=='1' && data.text[3]=='1')
				{//11
					char buf[25]={0};
					strcat(file_name,data.text+7);
					strcat(file_name,".txt");
					printf(LOG_PREFX"cmd 11 to open %s\n",file_name);
					operation=0;
					memcpy(text_out,data.text,6);
					text_out[0]='d';
					int file_line=get_file_lines(file_name);
					if(file_line==0 || ((file_line%FILE_LIGHT_LINES_RECORD)!=0))
					{
						strcat(text_out,";mode-id ");
						strcat(text_out,"unknown");
						strcat(text_out,";mode-name ");
						strcat(text_out,"unknown");
						strcat(text_out,";mode-use ");
						strcat(text_out,"unknown");
					}
					else
					{
						int i;
						for(i=0;i<file_line;i=i+FILE_LIGHT_LINES_RECORD)
						{
							strcat(text_out,";mode-id ");
							if(read_file_line(file_name,i+FILE_LIGHT_NAME_LINE,0,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
							memset(buf,'\0',25);
							strcat(text_out,";mode-name ");
							if(read_file_line(file_name,i+FILE_LIGHT_NAME_LINE,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
							memset(buf,'\0',25);
							strcat(text_out,";mode-use ");
							if(read_file_line(file_name,i+FILE_LIGHT_MODE_USE,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
						}
						printf(LOG_PREFX"light list %s\n",text_out);
					}
					memset(file_name,'\0',256);
				}
				//else if(data.text[2]='1' && data.text[3]='2')
				//{//12
				//
				//}
				else if(data.text[2]=='1' && data.text[3]=='3')
				{//13
					char mode_id[10]={0};
					char str[256]={0};
					len=strlen(FILE_PATH_NAME);
					file_name[len]=data.text[7];
					file_name[len+1]=data.text[8];
					strcat(file_name,".txt");
					printf(LOG_PREFX"cmd 13 to open %s\n",file_name);
					int i=10;
					while(data.text[i]!='\0' && data.text[i]!=';')
						i++;
					memcpy(mode_id,data.text+10,i-10);
					printf(LOG_PREFX"mode_id %s\n",mode_id);
					set_light_use(file_name,mode_id,1);
					if(data.text[5]!='w')
					{
						strcpy(text_out,"d;13;b;mode-id ");
						strcat(text_out,mode_id);
					}
					else
					{
						strcpy(text_out,"d;13;w;mode-id ");
						strcat(text_out,mode_id);
						strcat(text_out,";");
						strcat(text_out,data.text+i);
					}
					printf(LOG_PREFX"text_out is %s\n",text_out);
				}
				///else if(data.text[2]='1' && data.text[3]='4')
				//{//14
				//
				//}
				else if(data.text[2]=='1' && data.text[3]=='5')
				{//15					
					char mode_id[10]={0};
					char str[256]={0};
					len=strlen(FILE_PATH_NAME);
					file_name[len]=data.text[7];
					file_name[len+1]=data.text[8];
					strcat(file_name,".txt");
					printf(LOG_PREFX"cmd 15 to open %s\n",file_name);
					int i=10;
					while(data.text[i]!='\0' && data.text[i]!=';')
						i++;
					memcpy(mode_id,data.text+10,i-10);
					printf(LOG_PREFX"mode_id %s\n",mode_id);
					set_light_use(file_name,mode_id,1);
					if(data.text[5]!='w')
					{
						if(data.text[5]=='b')
							strcpy(text_out,"g;15;b;0");
						else
							strcpy(text_out,"g;15;m;0");
					}
					else
					{
						strcpy(text_out,"g;15;w;0");
						strcat(text_out,";");
						strcat(text_out,data.text+i);
					}
					printf(LOG_PREFX"text_out is %s\n",text_out);
				}
				else if(data.text[2]=='1' && data.text[3]=='6')
				{//16
					char mode_id[10]={0};
					char str[256]={0};
					len=strlen(FILE_PATH_NAME);
					file_name[len]=data.text[7];
					file_name[len+1]=data.text[8];
					strcat(file_name,".txt");
					printf(LOG_PREFX"cmd 16 to open %s\n",file_name);
					int i=10;
					while(data.text[i]!='\0' && data.text[i]!=';')
						i++;
					memcpy(mode_id,data.text+10,i-10);
					printf(LOG_PREFX"mode_id %s\n",mode_id);
					set_light_use(file_name,mode_id,0);
					if(data.text[5]!='w')
					{
						if(data.text[5]=='b')
							strcpy(text_out,"g;16;b;0");
						else
							strcpy(text_out,"g;16;m;0");
					}
					else
					{
						strcpy(text_out,"g;16;w;0");
						strcat(text_out,";");
						strcat(text_out,data.text+i);
					}
					printf(LOG_PREFX"text_out is %s\n",text_out);

				}
				else if(data.text[2]=='1' && data.text[3]=='7')
				{//17
					char buf[25]={0};
					len=strlen(FILE_PATH_NAME);
					file_name[len]=data.text[7];
					file_name[len+1]=data.text[8];
					strcat(file_name,".txt");
					printf(LOG_PREFX"cmd 11 to open %s\n",file_name);
					operation=0;
					int file_line=get_file_lines(file_name);
					if(strrchr(data.text,'?')!=NULL)
					{						
						strcpy(text_out,"g;17;b;");
						if(file_line==0 || ((file_line%FILE_ALARM_LINES_RECORD)!=0))
						{
							read_file_line(file_name,file_line-FILE_ALARM_LINES_RECORD,0,buf);
							strcat(text_out,buf);
						}
						else
							strcat(text_out,"unknown");
					}
					else
					{//change alarm setting
						int i,found=0;
						char alarm_id[10]={0};
						char alarm_time[10]={0};
						char alarm_freq[10]={0};
						char alarm_vol[10]={0};
						char alarm_blance[10]={0};
						char alarm_change[10]={0};
						char alarm_vol_change_freq[10]={0};
						int i=10,j=0;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alarm_id,data.text+10,i-10);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alarm_time,data.text+j,i-j);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alarm_time+2,data.text+j,i-j);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alarm_freq,data.text+j,i-j);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alarm_vol,data.text+j,i-j);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alarm_blance,data.text+j,i-j);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alarm_change,data.text+j,i-j);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alarm_vol_change_freq,data.text+j,i-j);
						for(i=0;i<file_line;i=i+FILE_ALARM_LINES_RECORD)
						{
							if(read_file_line(file_name,i,0,buf));
							{
									if(strncmp(buf,alarm_id,strlen(buf))==0)
									{
										found=1;
										break;
									}
							}
						}					
						if(data.text[5]=='b')
							strcpy(text_out,"g;17;b;0");
						else
						{
							strcpy(text_out,"g;17;w;0;");
							strcat(text_out,data.text+i);
						}
						printf(LOG_PREFX"alarm_id %s \nalarm_time %s \nalarm_freq %s \nalarm_vol %s \nalarm_blance %s \nalarm_change %s \nalarm_vol_change_freq\n"alarm_id,alarm_time,alarm_freq,alarm_vol,alarm_blance,alarm_change,alarm_vol_change_freq);
						write_alarm(file_name,found,alarm_id,alarm_time,alarm_freq,alarm_vol,alarm_blance,alarm_change,alarm_vol_change_freq);
					}
				}
				else if(data.text[2]=='1' && data.text[3]=='8')
				{//18
					char alarm_id[10]={0};
					char str[256]={0};
					len=strlen(FILE_PATH_NAME);
					file_name[len]=data.text[7];
					file_name[len+1]=data.text[8];
					strcat(file_name,".txt");
					printf(LOG_PREFX"cmd 18 to open %s\n",file_name);
					int i=10;
					while(data.text[i]!='\0' && data.text[i]!=';')
						i++;
					memcpy(alarm_id,data.text+10,i-10);
					printf(LOG_PREFX"alarm_id %s\n",alarm_id);
					set_alarm_on(file_name,alarm_id,1);
					if(data.text[5]=='b')
						strcpy(text_out,"g;18;b;0");
					else
					{
						strcpy(text_out,"g;18;w;0;");
						strcat(text_out,data.text+i);
					}
					printf(LOG_PREFX"text_out is %s\n",text_out);

				}
				else if(data.text[2]=='1' && data.text[3]=='9')
				{//19
					char alarm_id[10]={0};
					char str[256]={0};
					len=strlen(FILE_PATH_NAME);
					file_name[len]=data.text[7];
					file_name[len+1]=data.text[8];
					strcat(file_name,".txt");
					printf(LOG_PREFX"cmd 19 to open %s\n",file_name);
					int i=10;
					while(data.text[i]!='\0' && data.text[i]!=';')
						i++;
					memcpy(alarm_id,data.text+10,i-10);
					printf(LOG_PREFX"alarm_id %s\n",alarm_id);
					set_alarm_on(file_name,alarm_id,0);
					if(data.text[5]=='b')
						strcpy(text_out,"g;19;b;0");
					else
					{
						strcpy(text_out,"g;19;w;0;");
						strcat(text_out,data.text+i);
					}
					printf(LOG_PREFX"text_out is %s\n",text_out);

				}
				else if(data.text[2]=='2' && data.text[3]=='0')
				{//20
					char buf[25]={0};
					char alarm_id[10]={0};
					len=strlen(FILE_PATH_NAME);
					file_name[len]=data.text[5];
					file_name[len+1]=data.text[6];
					strcat(file_name,".txt");
					printf(LOG_PREFX"cmd 20 to open %s\n",file_name);
					int found=0;
					strcpy(text_out,"d;20;");
					int i;
					strcpy(alarm_id,strrchr(data.text,';')+1);
					int file_line=get_file_lines(file_name);
					if(file_line==0 || ((file_line%FILE_ALARM_LINES_RECORD)!=0))
					{
						strcat(text_out,";unknown");
					}
					else
					{
						for(i=0;i<file_line;i=i+FILE_ALARM_LINES_RECORD)
						{
							if(read_file_line(file_name,i,0,buf));
							{
									if(strncmp(buf,alarm_id,strlen(buf))==0)
									{
										found=1;
										break;
									}
							}
						}
						if(found)
						{
							if(read_file_line(file_name,i+8,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,";unknown");
						}
						else
						{
							strcat(text_out,";unknown");
						}
					}
					printf(LOG_PREFX"text out %s\n",text_out);
				}
				else if(data.text[2]=='2' && data.text[3]=='1'&& data.text[5]=='0' && data.text[6]=='4')
				{//21
					char buf[25]={0};
					char alarm_id[10]={0};
					len=strlen(FILE_PATH_NAME);
					file_name[len]=data.text[5];
					file_name[len+1]=data.text[6];
					strcat(file_name,".txt");
					printf(LOG_PREFX"cmd 21 to open %s\n",file_name);
					int found=0;
					strcpy(text_out,"d;21;04;");
					int i;
					strcpy(alarm_id,strrchr(data.text,';')+1);
					int file_line=get_file_lines(file_name);
					if(file_line==0 || ((file_line%FILE_ALARM_LINES_RECORD)!=0))
					{
						strcat(text_out,";unknown");
						strcat(text_out,";unknown");
						strcat(text_out,";unknown");
						strcat(text_out,";unknown");
						strcat(text_out,";unknown");
						strcat(text_out,";unknown");
					}
					else
					{
						for(i=0;i<file_line;i=i+FILE_ALARM_LINES_RECORD)
						{
							if(read_file_line(file_name,i,0,buf));
							{
									if(strncmp(buf,alarm_id,strlen(buf))==0)
									{
										found=1;
										break;
									}
							}
						}
						if(found)
						{
							if(read_file_line(file_name,i+2,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
							if(read_file_line(file_name,i+3,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");							
							if(read_file_line(file_name,i+4,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
							if(read_file_line(file_name,i+5,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
							if(read_file_line(file_name,i+6,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");							
							if(read_file_line(file_name,i+7,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
						}
						else
						{
							strcat(text_out,";unknown");
							strcat(text_out,";unknown");
							strcat(text_out,";unknown");
							strcat(text_out,";unknown");
							strcat(text_out,";unknown");
							strcat(text_out,";unknown");
						}
					}
					printf(LOG_PREFX"text out %s\n",text_out);
				}
				else if(data.text[2]=='2' && data.text[3]=='3')
				{//23
					char date_id[10]={0};
					char time_id[5]={0};
					len=strlen(FILE_PATH_NAME);
					file_name[len]=data.text[5];
					file_name[len+1]=data.text[6];
					strcat(file_name,".txt");
					printf(LOG_PREFX"cmd 23 to open %s\n",file_name);
					if(strlen(data.text)!=23)
					{
						strcpu(text_out,"g;23;1");
					}
					else
					{//a;23;05;2015;0807;12;21
						memcpy(date_id,data.text+8,4);
						memcpy(date_id+4,data.text+13,4);						
						memcpy(time_id,data.text+18,2);
						memcpy(time_id+2,data.text+21,2);
						strcpu(text_out,"g;23;0");
						set_year_time(file_name,date_id,time_id);
					}
					printf(LOG_PREFX"text_out is %s\n",text_out);
				}
				else if(data.text[2]=='2' && data.text[3]=='4')
				{//24
					char buf[25]={0};
					len=strlen(FILE_PATH_NAME);
					file_name[len]=data.text[7];
					file_name[len+1]=data.text[8];
					strcat(file_name,".txt");
					printf(LOG_PREFX"cmd 24 to open %s\n",file_name);
					operation=0;
					int file_line=get_file_lines(file_name);
					if(strrchr(data.text,'?')!=NULL)
					{						
						strcpy(text_out,"g;24;b;");
						if(file_line==0 || ((file_line%FILE_ALERT_LINES_RECORD)!=0))
						{
							read_file_line(file_name,file_line-FILE_ALARM_LINES_RECORD,0,buf);
							strcat(text_out,buf);
						}
						else
							strcat(text_out,"unknown");
					}
					else
					{//change alarm setting
						int i,found=0;
						char alert_id[10]={0};
						char alert_name[10]={0};
						char alert_day[10]={0};
						char alert_time[10]={0};
						char alert_up[10]={0};
						char alert_freq[10]={0};
						int i=10,j=0;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alert_id,data.text+10,i-10);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alert_name,data.text+j,i-j);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alert_day,data.text+j,i-j);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alert_day+4,data.text+j,i-j);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alert_time,data.text+j,i-j);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alert_time+2,data.text+j,i-j);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alert_up,data.text+j,i-j);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alert_freq,data.text+j,i-j);
						for(i=0;i<file_line;i=i+FILE_ALERT_LINES_RECORD)
						{
							if(read_file_line(file_name,i,0,buf));
							{
									if(strncmp(buf,alert_id,strlen(buf))==0)
									{
										found=1;
										break;
									}
							}
						}					
						if(data.text[5]=='b')
							strcpy(text_out,"g;24;b;0");
						else
						{
							strcpy(text_out,"g;24;w;0;");
							strcat(text_out,data.text+i);
						}
						printf(LOG_PREFX"alert_id %s \nalert_name %s \nalert_day %s \nalert_time %s \nalert_up %s \nalert_freq\n",alert_id,alert_name,alert_day,alert_time,alert_up,alert_freq);
						write_alert(file_name,found,alert_id,alert_name,alert_day,alert_time,alert_up,alert_freq);
					}

				}
				else if(data.text[2]=='2' && data.text[3]=='5')
				{//25
					char buf[25]={0};
					char alarm_id[10]={0};
					len=strlen(FILE_PATH_NAME);
					file_name[len]=data.text[5];
					file_name[len+1]=data.text[6];
					strcat(file_name,".txt");
					printf(LOG_PREFX"cmd 25 to open %s\n",file_name);
					int found=0;
					strcpy(text_out,"d;25;");
					int i;
					strcpy(alarm_id,strrchr(data.text,';')+1);
					int file_line=get_file_lines(file_name);
					if(file_line==0 || ((file_line%FILE_ALARM_LINES_RECORD)!=0))
					{
						strcat(text_out,";unknown");
						strcat(text_out,";unknown");
					}
					else
					{
						for(i=0;i<file_line;i=i+FILE_ALARM_LINES_RECORD)
						{
							if(read_file_line(file_name,i,0,buf));
							{
									if(strncmp(buf,alarm_id,strlen(buf))==0)
									{
										found=1;
										break;
									}
							}
						}
						if(found)
						{
							if(read_file_line(file_name,i,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
							if(read_file_line(file_name,i+1,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown"); 		
						}
						else
						{
							strcat(text_out,";unknown");
							strcat(text_out,";unknown");
						}
					}
					printf(LOG_PREFX"text out %s\n",text_out);

				}
				else if(data.text[2]=='2' && data.text[3]=='8')
				{//28
					char buf[25]={0};
					strcat(file_name,data.text+7);
					strcat(file_name,".txt");
					printf(LOG_PREFX"cmd 28 to open %s\n",file_name);
					operation=0;
					memcpy(text_out,data.text,6);
					text_out[0]='d';
					int file_line=get_file_lines(file_name);
					if(file_line==0 || ((file_line%FILE_ALARM_LINES_RECORD)!=0))
					{
						strcat(text_out,";music-id ");
						strcat(text_out,"unknown");
						strcat(text_out,";music-name ");
						strcat(text_out,"unknown");
						strcat(text_out,";singer ");
						strcat(text_out,"unknown");
						strcat(text_out,";music-like/unlike ");
						strcat(text_out,"unknown");
					}
					else
					{
						int i;
						for(i=0;i<file_line;i=i+FILE_ALARM_LINES_RECORD)
						{
							strcat(text_out,";ring-id ");
							if(read_file_line(file_name,i+0,0,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
							memset(buf,'\0',25);
							strcat(text_out,";");
							if(read_file_line(file_name,i+0,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
							memset(buf,'\0',25);
							strcat(text_out,";");
							if(read_file_line(file_name,i+1,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
							memset(buf,'\0',25);
							strcat(text_out,";");
							if(read_file_line(file_name,i+3,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
						}
						printf(LOG_PREFX"audio list %s\n",text_out);
					}
					memset(file_name,'\0',256);

				}
				send_msg(msgid,TYPE_FILE_TO_MAIN,FILE_TO_MAIN,text_out);
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
	waitpid(fpid, &status, 0);
	
	return 0;
}

