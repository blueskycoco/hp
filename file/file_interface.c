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
FILE *open_file(char *file_name)
{	
	return fopen(file_name, "rb");
}
void close_file(FILE *fp)
{
	fclose(fp);
}
int read_file_line(FILE *fp,int line_addr,char prv,char *out)
{
	char *line = NULL;
	size_t len = 0;
	int result=0;
	int line_cnt=0;
	ssize_t read;

	fseek(fp,0L,SEEK_SET);
	while ((read = getline(&line, &len, fp)) != -1) 
	//while (fgets(line, 512, fp) != NULL) 
	{
	   if(line_cnt==line_addr)
	   {
			memset(out,'\0',sizeof(out));
	   		if(prv)
			{
				printf(LOG_PREFX"get %s",line);
				strcpy(out,strchr(line,';')+1);
				printf(LOG_PREFX"len %ld\n",strlen(out));
				if(out[strlen(out)-1]=='\n')
				out[strlen(out)-2]='\0';
	   		}
			else
			{
				int i=0;
				while(line[i]!='\r' && line[i]!=';'&& line[i]!='\0')
				{
					out[i]=line[i];
					i++;
				}
				
			}
			printf(LOG_PREFX"rget %s\n",out);
			result=1;
			break;
	   }
	   line_cnt++;
	}
	
	free(line);
	line=NULL;

	return result;
}

int set_music_like(char *file,char *music_id,int like)
{
	char buf[256]={0};
	int result=0;
	char * line = NULL;
	size_t len = 0;
	int i=0;
	int write_pos=0,read=0,found=0;
	FILE *fp=fopen(file,"r");
	fseek(fp,0L,SEEK_END);
	int flen=ftell(fp);
	fseek(fp,0L,SEEK_SET);
	char *file_write;
	file_write=(char *)malloc(flen);	
	while ((read = getline(&line, &len, fp)) != -1) 
	{	
		if(strncmp(line,music_id,strlen(music_id))==0)
		{		 
		  found=1;
		  result=1;
	   	}
		/*if(i==3)
		{
			int j=0;
			char *buf=(char *)malloc(strlen(line)+1);
			memset(buf,'\0',strlen(line)+1);
			while(line[j]!=';' && line[j]!='\0')
				j++;
			memcpy(buf,line,j+1);
			strcat(buf,str);
			strcat(buf,"\r\n");
			printf("new %s",buf);
			memcpy(file_write+write_pos,buf,strlen(buf));
			write_pos=write_pos+strlen(buf);
			free(buf);
		}
		else */if(i==4)
		{
			//set like or unlike
			int j=0;
			while(line[j]!=';' && line[j]!='\0')
				j++;
			if(like)
				line[j+1]='1';
			else
				line[j+1]='2';
			memcpy(file_write+write_pos,line,read);
			write_pos=write_pos+read;
		}
		else
		{
			memcpy(file_write+write_pos,line,read);
			  write_pos=write_pos+read;
		}
		if(found==1)
			i++;
	}
	if(line)
		free(line);
	fclose(fp);
	fp=fopen(file,"w");
	fwrite(file_write,write_pos,1,fp);	
	fclose(fp);
	
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
	fseek(fp,0L,SEEK_SET);
	char *file_write;
	file_write=(char *)malloc(flen);	
	while ((read = getline(&line, &len, fp)) != -1) 
	{	
		if(strncmp(line,mode_id,strlen(mode_id))==0)
		{		 
		  found=1;
		  i=0;
	   	}
		if(i==6)
		{
			//set use or unuse
			char *str=strchr(line,';');
			if(like)
				str[1]='0';
			else
				str[1]='1';
			str[2]='\r';
			str[3]='\n';
			str[4]='\0';
			found=0;
			i=0;
			read=7;
		}
		if(found==1)
			i++;
	   memcpy(file_write+write_pos,line,read);
 	   write_pos=write_pos+read;
	  // free(line);
	}
	if(line)
		free(line);
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
	int i=0;
	int write_pos=0,read=0,found=0;
	FILE *fp=fopen(file,"r");
	fseek(fp,0L,SEEK_END);
	int flen=ftell(fp);
	fseek(fp,0L,SEEK_SET);
	char *file_write;
	file_write=(char *)malloc(flen);	
	while ((read = getline(&line, &len, fp)) != -1) 
	{	
		if(i==7)
		{
			//strcpy(line+4,time);
			memcpy(line+4,time,4);
			line[strlen(time)+4]='\r';
			line[strlen(time)+5]='\n';
			line[strlen(time)+6]='\0';
		}
		if(i==8)
		{
			//strcpy(line+4,year);
			memcpy(line+4,year,8);
			line[strlen(year)+4]='\r';
			line[strlen(year)+5]='\n';
			line[strlen(year)+6]='\0';
		}
    	   i++;		
	   memcpy(file_write+write_pos,line,strlen(line));
 	   write_pos=write_pos+strlen(line);
	}
	printf(LOG_PREFX"%s",file_write);
	   free(line);
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
	fseek(fp,0L,SEEK_SET);
	char *file_write;
	file_write=(char *)malloc(flen+1);	
	while ((read = getline(&line, &len, fp)) != -1) 
	{	
		if(strncmp(line,alarm_id,strlen(alarm_id))==0)
		{		 
		  found=1;
		  i=0;
	   	}
		if(i==3)
		{
			//set use or unuse
			char *str=strchr(line,';');
			if(like)
				str[1]='a';
			else
				str[1]='b';
			str[2]='\r';
			str[3]='\n';
			str[4]='\0';
			found=0;
			i=0;
		}
		if(found==1)
			i++;
	   memcpy(file_write+write_pos,line,strlen(line));
 	   write_pos=write_pos+strlen(line);
	}
	free(line);
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
	int i=0;
	char last[3]="\r\n";
	char tmp=';';
	int write_pos=0,read=0,found=0;
	FILE *fp=fopen(file,"r");
	fseek(fp,0L,SEEK_END);
	int flen=ftell(fp);
	fseek(fp,0L,SEEK_SET);
	char *file_write;
	file_write=(char *)malloc(flen+strlen(str)+1);	
	while ((read = getline(&line, &len, fp)) != -1) 
	{	
	   if(strncmp(line,alarm_id,strlen(alarm_id))==0)
	   {
		  found=1;
	   }
	   if(i==2)
	   {
	   		int j=0;
	   		char *buf=(char *)malloc(strlen(line)+1);
			memset(buf,'\0',strlen(line)+1);
			while(line[j]!=';' && line[j]!='\0')
				j++;
			memcpy(buf,line,j+1);
			strcat(buf,str);
			strcat(buf,"\r\n");
			printf("new %s",buf);
			memcpy(file_write+write_pos,buf,strlen(buf));
			write_pos=write_pos+strlen(buf);
	   }
	   else
	   	{
			memcpy(file_write+write_pos,line,read);
	 	   write_pos=write_pos+read;
	   	}
	   if(found==1)
	   	i++;
	   
	}
	if(line)		
	   free(line);
	   line=NULL;
	fclose(fp);
	fp=fopen(file,"w");
	fwrite(file_write,write_pos,1,fp);	
	if(found==0)
	{
		fwrite(alarm_id,strlen(alarm_id),1,fp);
		fwrite(&tmp,1,1,fp);
		fwrite(str,strlen(str),1,fp);
	}
	fclose(fp);
	result=1;
	
	return result;	
}
int write_alert(char *file,int found,char *alert_id,char *alert_name,char *alert_date,char *alert_time,char *alert_eraly,char *alert_once,char *alert_year)
{
	char buf[256]={0};
	int result=0;
	char * line = NULL;
	size_t len = 0;
	char last[3]="\r\n";
	int write_pos=0,read=0;
	FILE *fp=fopen(file,"r");
	fseek(fp,0L,SEEK_END);
	int flen=ftell(fp);
	fseek(fp,0L,SEEK_SET);
	char *file_write;
	if(found)		
	{
		file_write=(char *)malloc(flen+strlen(alert_id)+strlen(alert_name)+strlen(alert_date)+strlen(alert_time)+strlen(alert_eraly)+strlen(alert_once)+strlen(alert_year)+1);		
		memset(file_write,'\0',flen+strlen(alert_id)+strlen(alert_name)+strlen(alert_date)+strlen(alert_time)+strlen(alert_eraly)+strlen(alert_once)+strlen(alert_year)+1);	
	}	
	else
	{
		file_write=(char *)malloc(flen+5*strlen(alert_id)+strlen(alert_name)+strlen(alert_date)+strlen(alert_time)+strlen(alert_eraly)+strlen(alert_once)+strlen(alert_year)+1);		
		memset(file_write,'\0',flen+5*strlen(alert_id)+strlen(alert_name)+strlen(alert_date)+strlen(alert_time)+strlen(alert_eraly)+strlen(alert_once)+strlen(alert_year)+1);		
	}
	while ((read = getline(&line, &len, fp)) != -1) 
	{	
		if(found && strncmp(line,alert_id,strlen(alert_id))==0)
		{
		  int a_id=atoi(alert_id);
		  char str_a_id[4]={0};
		  //name
		  sprintf(str_a_id,"%03d",a_id);
		  memcpy(file_write+write_pos,str_a_id,strlen(str_a_id));
		  file_write[write_pos+strlen(str_a_id)]=';';
		  memcpy(file_write+write_pos+strlen(str_a_id)+1,alert_name,strlen(alert_name));
		  memcpy(file_write+write_pos+strlen(str_a_id)+1+strlen(alert_name),last,2);
		  write_pos=write_pos+strlen(str_a_id)+1+2+strlen(alert_name);
		  //date 
		  a_id=a_id+1;
		  sprintf(str_a_id,"%03d",a_id);
		  memcpy(file_write+write_pos,str_a_id,strlen(str_a_id));
		  file_write[write_pos+strlen(str_a_id)]=';';
		  memcpy(file_write+write_pos+strlen(str_a_id)+1,alert_date,strlen(alert_date));
		  memcpy(file_write+write_pos+strlen(str_a_id)+1+strlen(alert_date),last,2);
		  write_pos=write_pos+strlen(str_a_id)+1+2+strlen(alert_date);
	          //time
		  a_id=a_id+1;
		  sprintf(str_a_id,"%03d",a_id);
		  memcpy(file_write+write_pos,str_a_id,strlen(str_a_id));
		  file_write[write_pos+strlen(str_a_id)]=';';
		  memcpy(file_write+write_pos+strlen(str_a_id)+1,alert_time,strlen(alert_time));
		  memcpy(file_write+write_pos+strlen(str_a_id)+1+strlen(alert_time),last,2);
		  write_pos=write_pos+strlen(str_a_id)+1+2+strlen(alert_time);
		  //eraly	
		  a_id=a_id+1;
		  sprintf(str_a_id,"%03d",a_id);
		  memcpy(file_write+write_pos,str_a_id,strlen(str_a_id));
		  file_write[write_pos+strlen(str_a_id)]=';';
		  memcpy(file_write+write_pos+strlen(str_a_id)+1,alert_eraly,strlen(alert_eraly));
		  memcpy(file_write+write_pos+strlen(str_a_id)+1+strlen(alert_eraly),last,2);
		  write_pos=write_pos+strlen(str_a_id)+1+2+strlen(alert_eraly);
                  //once
		  a_id=a_id+1;
		  sprintf(str_a_id,"%03d",a_id);
		  memcpy(file_write+write_pos,str_a_id,strlen(str_a_id));
		  file_write[write_pos+strlen(str_a_id)]=';';
		  memcpy(file_write+write_pos+strlen(str_a_id)+1,alert_once,strlen(alert_once));
		  memcpy(file_write+write_pos+strlen(str_a_id)+1+strlen(alert_once),last,2);
		  write_pos=write_pos+strlen(str_a_id)+1+2+strlen(alert_once);
		  a_id=a_id+1;
		  sprintf(str_a_id,"%03d",a_id);
		  memcpy(file_write+write_pos,str_a_id,strlen(str_a_id));
		  file_write[write_pos+strlen(str_a_id)]=';';
		  memcpy(file_write+write_pos+strlen(str_a_id)+1,alert_year,strlen(alert_year));
		  memcpy(file_write+write_pos+strlen(str_a_id)+1+strlen(alert_year),last,2);
		  write_pos=write_pos+strlen(str_a_id)+1+2+strlen(alert_year);
		  printf(LOG_PREFX"\n%s",file_write);
 		 getline(&line, &len, fp);
		  getline(&line, &len, fp);
		  getline(&line, &len, fp);
		  getline(&line, &len, fp);
		  getline(&line, &len, fp);
	   }
	   else
	   {
		   memcpy(file_write+write_pos,line,read);
	 	   write_pos=write_pos+read;
	   }
	}
	free(line);
	fclose(fp);
	
	fp=fopen(file,"w");
	fwrite(file_write,write_pos,1,fp);	
	if(found==0)
	{	
		char line[64]={0};
		int a_id=atoi(alert_id);
		char str_a_id[4]={0};
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,alert_name);
		strcat(line,"\r\n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+1;
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,alert_date);
		strcat(line,"\r\n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+1;
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,alert_time);
		strcat(line,"\r\n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+1;
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,alert_eraly);
		strcat(line,"\r\n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+1;
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,alert_once);
		strcat(line,"\r\n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+1;
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,alert_year);
		fwrite(line,strlen(line),1,fp);
	}
	fclose(fp);
	result=1;
	
	return result;	
}
int write_lamp(char *file,int found,char *lamp_id,char *lamp_code,char *lamp_name,char *rom_position)
{
	char buf[256]={0};
	int result=0;
	char * line = NULL;
	size_t len = 0;
	char last[3]="\r\n";
	int write_pos=0,read=0;
	FILE *fp=fopen(file,"r");
	fseek(fp,0L,SEEK_END);
	int flen=ftell(fp);
	fseek(fp,0L,SEEK_SET);
	char *file_write;
	if(found)		
	{
		file_write=(char *)malloc(flen+strlen(lamp_id)+strlen(lamp_code)+strlen(lamp_name)+strlen(rom_position)+1);		
		memset(file_write,'\0',flen+strlen(lamp_id)+strlen(lamp_code)+strlen(lamp_name)+strlen(rom_position)+1);	
	}	
	else
	{
		file_write=(char *)malloc(flen+3*strlen(lamp_id)+strlen(lamp_code)+strlen(lamp_name)+strlen(rom_position)+1);		
		memset(file_write,'\0',flen+3*strlen(lamp_id)+strlen(lamp_code)+strlen(lamp_name)+strlen(rom_position)+1);		
	}
	while ((read = getline(&line, &len, fp)) != -1) 
	{	
		if(found && strncmp(line,lamp_id,strlen(lamp_id))==0)
		{
		  int a_id=atoi(lamp_id);
		  char str_a_id[4]={0};
		  sprintf(str_a_id,"%03d",a_id);
		  memcpy(file_write+write_pos,str_a_id,strlen(str_a_id));
		  file_write[write_pos+strlen(str_a_id)]=';';
		  memcpy(file_write+write_pos+strlen(str_a_id)+1,lamp_code,strlen(lamp_code));
		  memcpy(file_write+write_pos+strlen(str_a_id)+1+strlen(lamp_code),last,2);
		  write_pos=write_pos+strlen(str_a_id)+1+2+strlen(lamp_code);
		  a_id=a_id+1;
		  sprintf(str_a_id,"%03d",a_id);
		  memcpy(file_write+write_pos,str_a_id,strlen(str_a_id));
		  file_write[write_pos+strlen(str_a_id)]=';';
		  memcpy(file_write+write_pos+strlen(str_a_id)+1,lamp_name,strlen(lamp_name));
		  memcpy(file_write+write_pos+strlen(str_a_id)+1+strlen(lamp_name),last,2);
		  write_pos=write_pos+strlen(str_a_id)+1+2+strlen(lamp_name);
		  a_id=a_id+1;
		  sprintf(str_a_id,"%03d",a_id);
		  memcpy(file_write+write_pos,str_a_id,strlen(str_a_id));
		  file_write[write_pos+strlen(str_a_id)]=';';
		  memcpy(file_write+write_pos+strlen(str_a_id)+1,rom_position,strlen(rom_position));
		  memcpy(file_write+write_pos+strlen(str_a_id)+1+strlen(rom_position),last,2);
		  write_pos=write_pos+strlen(str_a_id)+1+2+strlen(rom_position);
		  printf(LOG_PREFX"\n%s",file_write);
		  getline(&line, &len, fp);
		  getline(&line, &len, fp);
	   }
	   else
	   {
		   memcpy(file_write+write_pos,line,read);
	 	   write_pos=write_pos+read;
	   }
	}
	free(line);
	fclose(fp);
	printf(LOG_PREFX"w %s",file_write);	
	fp=fopen(file,"w");
	fwrite(file_write,write_pos,1,fp);	
	if(found==0)
	{	
		char line[64]={0};
		int a_id=atoi(lamp_id);
		char str_a_id[4]={0};
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,lamp_code);
		strcat(line,"\r\n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+1;
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,lamp_name);
		strcat(line,"\r\n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+1;
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,rom_position);
		strcat(line,"\r\n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+1;
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,"unknown");
		strcat(line,"\r\n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+1;
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,"unknown");
		strcat(line,"\r\n");
		fwrite(line,strlen(line),1,fp);
	}
	fclose(fp);
	result=1;
	
	return result;	
}
int write_switch(char *file,int found,char *switch_id,char *switch_code,char *switch_name,char *rom_position)
{
	char buf[256]={0};
	int result=0;
	char * line = NULL;
	size_t len = 0;
	char last[3]="\r\n";
	int write_pos=0,read=0;
	FILE *fp=fopen(file,"r");
	fseek(fp,0L,SEEK_END);
	int flen=ftell(fp);
	fseek(fp,0L,SEEK_SET);
	char *file_write;
	if(found)		
	{
		file_write=(char *)malloc(flen+strlen(switch_id)+strlen(switch_code)+strlen(switch_name)+strlen(rom_position)+1);		
		memset(file_write,'\0',flen+strlen(switch_id)+strlen(switch_code)+strlen(switch_name)+strlen(rom_position)+1);	
	}	
	else
	{
		file_write=(char *)malloc(flen+3*strlen(switch_id)+strlen(switch_code)+strlen(switch_name)+strlen(rom_position)+1);		
		memset(file_write,'\0',flen+3*strlen(switch_id)+strlen(switch_code)+strlen(switch_name)+strlen(rom_position)+1);		
	}
	while ((read = getline(&line, &len, fp)) != -1) 
	{	
		if(found && strncmp(line,switch_id,strlen(switch_id))==0)
		{
		  int a_id=atoi(switch_id);
		  char str_a_id[4]={0};
		  sprintf(str_a_id,"%03d",a_id);
		  memcpy(file_write+write_pos,str_a_id,strlen(str_a_id));
		  file_write[write_pos+strlen(str_a_id)]=';';
		  memcpy(file_write+write_pos+strlen(str_a_id)+1,switch_code,strlen(switch_code));
		  memcpy(file_write+write_pos+strlen(str_a_id)+1+strlen(switch_code),last,2);
		  write_pos=write_pos+strlen(str_a_id)+1+2+strlen(switch_code);
		  a_id=a_id+1;
		  sprintf(str_a_id,"%03d",a_id);
		  memcpy(file_write+write_pos,str_a_id,strlen(str_a_id));
		  file_write[write_pos+strlen(str_a_id)]=';';
		  memcpy(file_write+write_pos+strlen(str_a_id)+1,switch_name,strlen(switch_name));
		  memcpy(file_write+write_pos+strlen(str_a_id)+1+strlen(switch_name),last,2);
		  write_pos=write_pos+strlen(str_a_id)+1+2+strlen(switch_name);
		  a_id=a_id+1;
		  sprintf(str_a_id,"%03d",a_id);
		  memcpy(file_write+write_pos,str_a_id,strlen(str_a_id));
		  file_write[write_pos+strlen(str_a_id)]=';';
		  memcpy(file_write+write_pos+strlen(str_a_id)+1,rom_position,strlen(rom_position));
		  memcpy(file_write+write_pos+strlen(str_a_id)+1+strlen(rom_position),last,2);
		  write_pos=write_pos+strlen(str_a_id)+1+2+strlen(rom_position);
		  printf(LOG_PREFX"\n%s",file_write);
		  getline(&line, &len, fp);
		  getline(&line, &len, fp);
	   }
	   else
	   {
		   memcpy(file_write+write_pos,line,read);
	 	   write_pos=write_pos+read;
	   }
	}
	free(line);
	fclose(fp);
	
	fp=fopen(file,"w");
	fwrite(file_write,write_pos,1,fp);	
	if(found==0)
	{	
		char line[64]={0};
		int a_id=atoi(switch_id);
		char str_a_id[4]={0};
		sprintf(str_a_id,"%03d",a_id-3);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,"unknown");
		strcat(line,"\r\n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		sprintf(str_a_id,"%03d",a_id-2);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,"unknown");
		strcat(line,"\r\n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		sprintf(str_a_id,"%03d",a_id-1);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,"unknown");
		strcat(line,"\r\n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,switch_code);
		strcat(line,"\r\n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+1;
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,switch_name);
		strcat(line,"\r\n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+1;
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,rom_position);
		strcat(line,"\r\n");
		fwrite(line,strlen(line),1,fp);
	}
	fclose(fp);
	result=1;
	
	return result;	
}
int write_wifi(char *file,char *lamp_code,char *wifi_ap,char *wifi_pwd)
{
	char last[3]="\r\n";
	char *buf=(char *)malloc(strlen(lamp_code)+2+strlen(wifi_ap)+2+strlen(wifi_pwd)+2+1);
	memset(buf,'\0',strlen(lamp_code)+2+strlen(wifi_ap)+2+strlen(wifi_pwd)+2+1);
	strcpy(buf,lamp_code);
	strcat(buf,"\r\n");
	strcat(buf,wifi_ap);
	strcat(buf,"\r\n");	
	strcat(buf,wifi_pwd);
	strcat(buf,"\r\n");
	FILE *fp=fopen(file,"w");
	fwrite(buf,strlen(buf),1,fp);	
	fclose(fp);
	return 1;
}
int write_alarm(char *file,int found,char *alarm_id,char *alarm_time,char *alarm_freq,char *alarm_vol,char *alarm_blance,char *alarm_change,char *alarm_change_freq)
{
	char buf[256]={0};
	int result=0;
	char * line = NULL;
	size_t len = 0;
	char last[3]="\r\n";
	int write_pos=0,read=0;
	FILE *fp=fopen(file,"r");
	fseek(fp,0L,SEEK_END);
	int flen=ftell(fp);
	fseek(fp,0L,SEEK_SET);
	char *file_write;
	if(found)		
	{
		file_write=(char *)malloc(flen+strlen(alarm_id)+strlen(alarm_time)+strlen(alarm_freq)+strlen(alarm_vol)+strlen(alarm_blance)+strlen(alarm_change)+strlen(alarm_change_freq)+1);		
		memset(file_write,'\0',flen+strlen(alarm_id)+strlen(alarm_time)+strlen(alarm_freq)+strlen(alarm_vol)+strlen(alarm_blance)+strlen(alarm_change)+strlen(alarm_change_freq)+1);	
	}	
	else
	{
		file_write=(char *)malloc(flen+5*strlen(alarm_id)+strlen(alarm_time)+strlen(alarm_freq)+strlen(alarm_vol)+strlen(alarm_blance)+strlen(alarm_change)+strlen(alarm_change_freq)+1);		
		memset(file_write,'\0',flen+5*strlen(alarm_id)+strlen(alarm_time)+strlen(alarm_freq)+strlen(alarm_vol)+strlen(alarm_blance)+strlen(alarm_change)+strlen(alarm_change_freq)+1);		
	}
	while ((read = getline(&line, &len, fp)) != -1) 
	{	
		if(found && strncmp(line,alarm_id,strlen(alarm_id))==0)
		{
		  int a_id=atoi(alarm_id);
		  char str_a_id[4]={0};
		  //time
		  sprintf(str_a_id,"%03d",a_id);
		  memcpy(file_write+write_pos,str_a_id,strlen(str_a_id));
		  file_write[write_pos+strlen(str_a_id)]=';';
		  memcpy(file_write+write_pos+strlen(str_a_id)+1,alarm_time,strlen(alarm_time));
		  memcpy(file_write+write_pos+strlen(str_a_id)+1+strlen(alarm_time),last,2);
		  write_pos=write_pos+strlen(str_a_id)+1+2+strlen(alarm_time);
		  //freq 
		  a_id=a_id+1;
		  sprintf(str_a_id,"%03d",a_id);
		  memcpy(file_write+write_pos,str_a_id,strlen(str_a_id));
		  file_write[write_pos+strlen(str_a_id)]=';';
		  memcpy(file_write+write_pos+strlen(str_a_id)+1,alarm_freq,strlen(alarm_freq));
		  memcpy(file_write+write_pos+strlen(str_a_id)+1+strlen(alarm_freq),last,2);
		  write_pos=write_pos+strlen(str_a_id)+1+2+strlen(alarm_freq);
		  getline(&line, &len, fp);
		  getline(&line, &len, fp);
		  memcpy(file_write+write_pos,line,strlen(line));
	 	  write_pos=write_pos+strlen(line);
		  getline(&line, &len, fp);
		  memcpy(file_write+write_pos,line,strlen(line));
	 	  write_pos=write_pos+strlen(line);
	          //vol
		  a_id=a_id+3;
		  sprintf(str_a_id,"%03d",a_id);
		  memcpy(file_write+write_pos,str_a_id,strlen(str_a_id));
		  file_write[write_pos+strlen(str_a_id)]=';';
		  memcpy(file_write+write_pos+strlen(str_a_id)+1,alarm_vol,strlen(alarm_vol));
		  memcpy(file_write+write_pos+strlen(str_a_id)+1+strlen(alarm_vol),last,2);
		  write_pos=write_pos+strlen(str_a_id)+1+2+strlen(alarm_vol);
		  //blance	
		  a_id=a_id+1;
		  sprintf(str_a_id,"%03d",a_id);
		  memcpy(file_write+write_pos,str_a_id,strlen(str_a_id));
		  file_write[write_pos+strlen(str_a_id)]=';';
		  memcpy(file_write+write_pos+strlen(str_a_id)+1,alarm_blance,strlen(alarm_blance));
		  memcpy(file_write+write_pos+strlen(str_a_id)+1+strlen(alarm_blance),last,2);
		  write_pos=write_pos+strlen(str_a_id)+1+2+strlen(alarm_blance);
                  //change
		  a_id=a_id+1;
		  sprintf(str_a_id,"%03d",a_id);
		  memcpy(file_write+write_pos,str_a_id,strlen(str_a_id));
		  file_write[write_pos+strlen(str_a_id)]=';';
		  memcpy(file_write+write_pos+strlen(str_a_id)+1,alarm_change,strlen(alarm_change));
		  memcpy(file_write+write_pos+strlen(str_a_id)+1+strlen(alarm_change),last,2);
		  write_pos=write_pos+strlen(str_a_id)+1+2+strlen(alarm_change);
                  //change freq
		  a_id=a_id+1;
		  sprintf(str_a_id,"%03d",a_id);
		  memcpy(file_write+write_pos,str_a_id,strlen(str_a_id));
		  file_write[write_pos+strlen(str_a_id)]=';';
		  memcpy(file_write+write_pos+strlen(str_a_id)+1,alarm_change_freq,strlen(alarm_change_freq));
		  memcpy(file_write+write_pos+strlen(str_a_id)+1+strlen(alarm_change_freq),last,2);
		  write_pos=write_pos+strlen(str_a_id)+1+2+strlen(alarm_change_freq);
		  getline(&line, &len, fp);
		  getline(&line, &len, fp);
		  getline(&line, &len, fp);
		  getline(&line, &len, fp);
		  getline(&line, &len, fp);
		  memcpy(file_write+write_pos,line,strlen(line));
	 	  write_pos=write_pos+strlen(line);
		  printf(LOG_PREFX"\n%s",file_write);
	   }
	   else
	   {
		   memcpy(file_write+write_pos,line,read);
	 	   write_pos=write_pos+read;
	   }
	}
	free(line);
	fclose(fp);
	
	fp=fopen(file,"w");
	fwrite(file_write,write_pos,1,fp);	
	if(found==0)
	{	
		char line[64]={0};
		int a_id=atoi(alarm_id);
		char str_a_id[4]={0};
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,"\r\n");
		strcat(line,str_a_id);
		strcat(line,";");
		strcat(line,alarm_time);
		strcat(line,"\r\n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+1;
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,alarm_freq);
		strcat(line,"\r\n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+1;
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,"unknown\r\n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+1;
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,"unknown\r\n");
		fwrite(line,strlen(line),1,fp);
		a_id=a_id+1;
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,alarm_vol);
		strcat(line,"\r\n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+1;
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,alarm_blance);
		strcat(line,"\r\n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+1;
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,alarm_change);
		strcat(line,"\r\n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+1;
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,alarm_change_freq);
		strcat(line,"\r\n");
		fwrite(line,strlen(line),1,fp);
		memset(line,'0',64);
		a_id=a_id+1;
		sprintf(str_a_id,"%03d",a_id);
		strcpy(line,str_a_id);
		strcat(line,";");
		strcat(line,"unknown");
		fwrite(line,strlen(line),1,fp);
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
	   //printf(LOG_PREFX"%s", line);
	   line_cnt++;
	}
	
	free(line);
	line=NULL;

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
						strcat(text_out,";unknown");
						strcat(text_out,";unknown");
						strcat(text_out,";unknown");
						strcat(text_out,";unknown");
					}
					else
					{
						int i;
						FILE *fp=open_file(file_name);
						if(fp!=NULL)
						{
							for(i=0;i<file_line;i=i+FILE_MUSIC_LINES_RECORD)
							{
								strcat(text_out,";");
								if(read_file_line(fp,i+FILE_MUSIC_NAME_LINE,0,buf))
									strcat(text_out,buf);
								else
									strcat(text_out,"unknown");
								memset(buf,'\0',25);
								strcat(text_out,";");
								if(read_file_line(fp,i+FILE_MUSIC_NAME_LINE,1,buf))
									strcat(text_out,buf);
								else
									strcat(text_out,"unknown");
								memset(buf,'\0',25);
								strcat(text_out,";");
								if(read_file_line(fp,i+FILE_MUSIC_SINGER_LINE,1,buf))
									strcat(text_out,buf);
								else
									strcat(text_out,"unknown");
								memset(buf,'\0',25);
								strcat(text_out,";");
								if(read_file_line(fp,i+FILE_MUSIC_LIKE_LINE,1,buf))
									strcat(text_out,buf);
								else
									strcat(text_out,"unknown");
							}
							close_file(fp);
						}
						else
							strcat(text_out,"can not open file");
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
					if(data.text[5]!='w')
					{
						printf(LOG_PREFX"in normal read\n");
						memcpy(text_out,data.text+2,4);
						strcat(text_out,strrchr(data.text,';'));						
						strcat(text_out,";");
						strcpy(music_id,strrchr(data.text,';')+1);
					}
					else
					{	
						printf(LOG_PREFX"in web read\n");
						strcpy(text_out,"01;w;");
						int i=10;
						while(data.text[i]!=';' && data.text[i]!='\0')
						{
							music_id[i-10]=data.text[i];
							i++;
						}
						strcat(text_out,music_id);
						strcat(text_out,";");
						printf(LOG_PREFX"music_id %s , text_out %s\n",music_id,text_out);
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
						FILE *fp=open_file(file_name);
						if(fp!=NULL)
						{
							for(i=0;i<file_line;i=i+FILE_MUSIC_LINES_RECORD)
							{
								read_file_line(fp,i+FILE_MUSIC_NAME_LINE,0,buf);
								if(strncmp(buf,music_id,strlen(buf))==0)
								{
									read_file_line(fp,i+FILE_MUSIC_PATH_LINE,1,buf);
									strcat(text_out,buf);								
									read_file_line(fp,i+FILE_MUSIC_FILE_NAME_LINE,1,buf);
									strcat(text_out,buf);
									if(data.text[0]=='r' &&data.text[5]=='w')
									{
										strcat(text_out,";");
										strcat(text_out,data.text+web_pos);
									}
									printf(LOG_PREFX"get music %s\n",text_out);
									break;
								}
							}
							close_file(fp);
							if(i==file_line)
							{
								strcat(text_out,"unknown");
								printf(LOG_PREFX"cant not find music by id %s\n",strrchr(data.text,';')+1);
							}
						}
						else
							strcat(text_out,"can not open file");
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
					memcpy(alarm_id,data.text+10,i-10);
					if(data.text[5]!='w')						
						strcpy(str,strrchr(data.text,';')+1);
					else
					{
						i++;
	                    int j=i;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(str,data.text+j,i-j);
					}
					printf(LOG_PREFX"alarm_id %s, str %s \n",alarm_id,str);
					write_file_line(file_name,alarm_id,str);
					if(data.text[5]!='w')
						strcpy(text_out,"g;03;b;0");
					else
					{
						strcpy(text_out,"g;03;w;0;");
						strcat(text_out,data.text+i+1);
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
					memcpy(music_id,data.text+10,i-10);
					printf(LOG_PREFX"music_id %s\n",music_id);
					if(set_music_like(file_name,music_id,1))
					{
						if(data.text[5]!='w')
						{
							strcpy(text_out,"g;04;b;0");
							text_out[5]=data.text[5];
						}
						else
						{
							strcpy(text_out,"g;04;w;0;");
							strcat(text_out,data.text+i+1);
						}
					}
					else
					{
						if(data.text[5]!='w')
						strcpy(text_out,"g;04;b;1");
						else
						{
							strcpy(text_out,"g;04;w;1;");
							strcat(text_out,data.text+i+1);
						}
					}
					printf(LOG_PREFX"text_out is %s\n",text_out);
				}
				else if(data.text[2]=='0' && data.text[3]=='5')
				{//05
					char music_id[10]={0};
					len=strlen(FILE_PATH_NAME);
					file_name[len]=data.text[7];
					file_name[len+1]=data.text[8];
					strcat(file_name,".txt");
					printf(LOG_PREFX"cmd 05 to open %s\n",file_name);
					int i=10;
					while(data.text[i]!='\0' && data.text[i]!=';')
						i++;
					memcpy(music_id,data.text+10,i-10);
					
					printf(LOG_PREFX"music_id %s\n",music_id);
					if(set_music_like(file_name,music_id,0))
					{
						if(data.text[5]!='w')
						{
							strcpy(text_out,"g;05;b;0");
							text_out[5]=data.text[5];
						}
						else
						{
							strcpy(text_out,"g;05;w;0;");
							strcat(text_out,data.text+i+1);
						}
					}
					else
					{
							if(data.text[5]!='w')
							strcpy(text_out,"g;05;b;1");
						else
						{
							strcpy(text_out,"g;05;w;1;");
							strcat(text_out,data.text+i+1);
						}
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
						if(data.text[2]=='1' && data.text[3]=='2')
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
									strcpy(text_out,"d;14;b");
								else
									strcpy(text_out,"d;14;w");
							}
							else
							{
								
								strcpy(text_out,"d;21;00");											
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
						memcpy(light_id,data.text+10,j-10);
					}
					if(data.text[3]!='4' && data.text[3]!='1')
					strcat(text_out,light_id);
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
						FILE *fp=open_file(file_name);
						if(fp!=NULL)
						{
							for(i=0;i<file_line;i=i+FILE_LIGHT_LINES_RECORD)
							{
								if(read_file_line(fp,i,0,buf));
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
								strcat(text_out,";");
								if(read_file_line(fp,i+1,1,buf))
									strcat(text_out,buf);
								else
									strcat(text_out,"unknown");
								strcat(text_out,";");
								memset(buf,'\0',25);
								if(read_file_line(fp,i+2,2,buf))
									strcat(text_out,buf);
								else
									strcat(text_out,"unknown");
								strcat(text_out,";");
								memset(buf,'\0',25);
								if(read_file_line(fp,i+3,1,buf))
									strcat(text_out,buf);
								else
									strcat(text_out,"unknown");
								strcat(text_out,";");
								memset(buf,'\0',25);
								if(read_file_line(fp,i+4,1,buf))
									strcat(text_out,buf);
								else
									strcat(text_out,"unknown");	
								strcat(text_out,";");
								memset(buf,'\0',25);
								if(read_file_line(fp,i+5,1,buf))
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
							close_file(fp);
						}
						else
							strcat(text_out,"can not open file");
						if(data.text[5]=='w' &&((data.text[2]=='0' && data.text[3]=='7')||(data.text[2]=='1' && data.text[3]=='4')||(data.text[2]=='1' && data.text[3]=='2')))
							strcat(text_out,data.text+j);
						}
						printf(LOG_PREFX"light list %s\n",text_out);
					
				}
				else if(data.text[2]=='1' && data.text[3]=='1')
				{//11
					char buf[128]={0};
					strcat(file_name,data.text+7);
					strcat(file_name,".txt");
					printf(LOG_PREFX"cmd 11 to open %s\n",file_name);
					operation=0;
					memcpy(text_out,data.text,6);
					text_out[0]='d';
					int file_line=get_file_lines(file_name);
					if(file_line==0 || ((file_line%FILE_LIGHT_LINES_RECORD)!=0))
					{
						strcat(text_out,";unknown");
						strcat(text_out,";unknown");
						strcat(text_out,";unknown");
					}
					else
					{
						int i;
						FILE *fp=open_file(file_name);
						if(fp!=NULL)
						{
							for(i=0;i<file_line;i=i+FILE_LIGHT_LINES_RECORD)
							{
								memset(buf,'\0',128);
								strcat(text_out,";");
								if(read_file_line(fp,i+FILE_LIGHT_NAME_LINE,0,buf))
									strcat(text_out,buf);
								else
									strcat(text_out,"unknown");
								memset(buf,'\0',128);
								strcat(text_out,";");
								if(read_file_line(fp,i+FILE_LIGHT_NAME_LINE,1,buf))
									strcat(text_out,buf);
								else
									strcat(text_out,"unknown");
								//memset(buf,'\0',128);
								//strcat(text_out,";");
								//if(read_file_line(fp,i+FILE_LIGHT_MODE_USE,1,buf))
								//	strcat(text_out,buf);
								//else
								//	strcat(text_out,"unknown");
							}
							close_file(fp);
						}
						else
							strcat(text_out,"can not open file");
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
						strcpy(text_out,"d;13;b;");
						strcat(text_out,mode_id);
					}
					else
					{
						strcpy(text_out,"d;13;w;");
						strcat(text_out,mode_id);
						strcat(text_out,";");
						strcat(text_out,data.text+i+3);
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
						strcat(text_out,data.text+i);
					}
					printf(LOG_PREFX"text_out is %s\n",text_out);

				}
				else if(data.text[2]=='1' && data.text[3]=='7')
				{//17
					char buf[25]={0};
					len=strlen(FILE_PATH_NAME);
					printf(LOG_PREFX"cmd 17 to open %s\n",file_name);
					operation=0;
					if(strrchr(data.text,'?')!=NULL)
					{						
						file_name[len]=data.text[7];
						file_name[len+1]=data.text[8];
						strcat(file_name,".txt");
						int file_line=get_file_lines(file_name);
						if(file_line!=0 && ((file_line%FILE_ALARM_LINES_RECORD)==0))
						{
							FILE *fp=open_file(file_name);
							if(fp!=NULL)
							{
								read_file_line(fp,file_line-FILE_ALARM_LINES_RECORD,0,buf);
								if(data.text[5]=='b')
								{
									strcpy(text_out,"g;17;b;");
									strcat(text_out,buf);
								}	
								else
								{
									strcpy(text_out,"g;17;w;");
									strcat(text_out,buf);
									strcat(text_out,";");
									strcat(text_out,strrchr(data.text,';')+1);
								}
								close_file(fp);
							}
							else
								strcat(text_out,"can not open file");
						}
						else
						{
							strcat(text_out,"unknown");
							printf(LOG_PREFX"04 file lines %d\n",file_line);
						}
					}
					else
					{//change alarm setting
						int found=0;
						char alarm_id[10]={0};
						char alarm_time[10]={0};
						char alarm_freq[10]={0};
						char alarm_vol[10]={0};
						char alarm_blance[10]={0};
						char alarm_change[10]={0};
						char alarm_vol_change_freq[10]={0};
						int i=7,j=0,commandid_pos=0;
						file_name[len]='0';
						file_name[len+1]='4';
						strcat(file_name,".txt");
						int file_line=get_file_lines(file_name);
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alarm_id,data.text+7,i-7);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alarm_time,data.text+j+1,i-j-1);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alarm_time+2,data.text+j+1,i-j-1);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alarm_freq,data.text+j+1,i-j-1);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alarm_vol,data.text+j+1,i-j-1);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alarm_blance,data.text+j+1,i-j-1);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alarm_change,data.text+j+1,i-j-1);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						commandid_pos=i;
						memcpy(alarm_vol_change_freq,data.text+j+1,i-j-1);
						FILE *fp=open_file(file_name);
						if(fp!=NULL)
						{
							for(i=0;i<file_line;i=i+FILE_ALARM_LINES_RECORD)
							{
								if(read_file_line(fp,i,0,buf));
								{
										if(strncmp(buf,alarm_id,strlen(buf))==0)
										{
											found=1;
											break;
										}
								}
							}
							close_file(fp);
						}
						else
							strcat(text_out,"can not open file");
						if(data.text[5]=='b')
							strcpy(text_out,"g;17;b;0");
						else
						{
							strcpy(text_out,"g;17;w;0");
							strcat(text_out,data.text+commandid_pos);
						}
						printf(LOG_PREFX"\nalarm_id %s \nalarm_time %s \nalarm_freq %s \nalarm_vol %s \nalarm_blance %s \nalarm_change %s \nalarm_vol_change_freq %s\n",alarm_id,alarm_time,alarm_freq,alarm_vol,alarm_blance,alarm_change,alarm_vol_change_freq);
						if(strlen(alarm_id)!=0 &&
							strlen(alarm_time)!=0 &&
							strlen(alarm_freq)!=0 &&
							strlen(alarm_vol)!=0 &&
							strlen(alarm_blance)!=0 &&
							strlen(alarm_change)!=0 &&
							strlen(alarm_vol_change_freq)!=0)
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
					if(data.text[5]!='w')
					{
						strcpy(text_out,"g;18;b;0");
						text_out[5]=data.text[5];
					}
					else
					{
						strcpy(text_out,"g;18;w;0");
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
					if(data.text[5]!='w')
					{
						strcpy(text_out,"g;19;b;0");
						text_out[5]=data.text[5];
					}
					else
					{
						strcpy(text_out,"g;19;w;0");
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
						FILE *fp=open_file(file_name);
						if(fp!=NULL)
						{
							for(i=0;i<file_line;i=i+FILE_ALARM_LINES_RECORD)
							{
								if(read_file_line(fp,i,0,buf));
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
								if(read_file_line(fp,i+8,1,buf))
									strcat(text_out,buf);
								else
									strcat(text_out,";unknown");
							}
							else
							{
								strcat(text_out,";unknown");
							}
							close_file(fp);
						}
						else
							strcat(text_out,"can not open file");
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
					strcpy(text_out,"d;21;04");
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
						FILE *fp=open_file(file_name);
						if(fp!=NULL)
						{
							for(i=0;i<file_line;i=i+FILE_ALARM_LINES_RECORD)
							{
								if(read_file_line(fp,i,0,buf));
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
								strcat(text_out,";");
								if(read_file_line(fp,i+2,1,buf))
									strcat(text_out,buf);
								else
									strcat(text_out,"unknown");
								strcat(text_out,";");
								if(read_file_line(fp,i+3,1,buf))
									strcat(text_out,buf);
								else
									strcat(text_out,"unknown");							
								strcat(text_out,";");
								if(read_file_line(fp,i+4,1,buf))
									strcat(text_out,buf);
								else
									strcat(text_out,"unknown");
								strcat(text_out,";");
								if(read_file_line(fp,i+5,1,buf))
									strcat(text_out,buf);
								else
									strcat(text_out,"unknown");
								strcat(text_out,";");
								if(read_file_line(fp,i+6,1,buf))
									strcat(text_out,buf);
								else
									strcat(text_out,"unknown");							
								strcat(text_out,";");
								if(read_file_line(fp,i+7,1,buf))
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
							close_file(fp);
						}
						else
							strcat(text_out,"can not open file");
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
						strcpy(text_out,"g;23;1");
					}
					else
					{//a;23;05;2015;0807;12;21
						memcpy(date_id,data.text+8,4);
						memcpy(date_id+4,data.text+13,4);						
						memcpy(time_id,data.text+18,2);
						memcpy(time_id+2,data.text+21,2);
						strcpy(text_out,"g;23;0");
						printf(LOG_PREFX"date %s,time %s\n",date_id,time_id);
						set_year_time(file_name,date_id,time_id);
					}
					printf(LOG_PREFX"text_out is %s\n",text_out);
				}
				else if(data.text[2]=='2' && data.text[3]=='4')
				{//24
					char buf[25]={0};
					len=strlen(FILE_PATH_NAME);
					printf(LOG_PREFX"cmd 24 to open %s\n",file_name);
					operation=0;
					if(strrchr(data.text,'?')!=NULL)
					{						
						file_name[len]=data.text[7];
						file_name[len+1]=data.text[8];
						strcat(file_name,".txt");
						int file_line=get_file_lines(file_name);
						if(file_line!=0 && ((file_line%FILE_ALERT_LINES_RECORD)==0))
						{
							FILE *fp=open_file(file_name);
							if(fp!=NULL)
							{
								read_file_line(fp,file_line-FILE_ALERT_LINES_RECORD,0,buf);
								if(data.text[5]=='b')
								{
									strcpy(text_out,"g;24;b;");
									strcat(text_out,buf);
								}	
								else
								{
									strcpy(text_out,"g;24;w;");
									strcat(text_out,buf);
									strcat(text_out,";");
									strcat(text_out,strrchr(data.text,'?')+2);
								}
								close_file(fp);
							}
							else
								strcat(text_out,"can not open file");
						}
						else
						{
							strcat(text_out,"unknown");
							printf(LOG_PREFX"04 file lines %d\n",file_line);
						}
					}
					else
					{//change alarm setting
						int found=0;
						char alert_id[10]={0};
						char alert_name[10]={0};
						char alert_date[10]={0};
						char alert_time[10]={0};
						char alert_eraly[10]={0};
						char alert_once[10]={0};
						char alert_year[10]={0};
						int i=10,j=0,commandid_pos=0;
						file_name[len]='0';
						file_name[len+1]='3';
						strcat(file_name,".txt");
						int file_line=get_file_lines(file_name);
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alert_id,data.text+10,i-10);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alert_name,data.text+j+1,i-j-1);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alert_date,data.text+j+1,i-j-1);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alert_date+2,data.text+j+1,i-j-1);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alert_time,data.text+j+1,i-j-1);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alert_time+2,data.text+j+1,i-j-1);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alert_eraly,data.text+j+1,i-j-1);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alert_once,data.text+j+1,i-j-1);
						j=i++;
						while(data.text[i]!='\0' && data.text[i]!=';')
							i++;
						memcpy(alert_year,data.text+j+1,i-j-1);
						commandid_pos=i;
						FILE *fp=open_file(file_name);
						if(fp!=NULL)
						{
							for(i=0;i<file_line;i=i+FILE_ALERT_LINES_RECORD)
							{
								if(read_file_line(fp,i,0,buf));
								{
										if(strncmp(buf,alert_id,strlen(buf))==0)
										{
											found=1;
											break;
										}
								}
							}
							close_file(fp);
						}
						else
							strcat(text_out,"can not open file");
						if(data.text[5]=='b')
							strcpy(text_out,"g;24;b;0");
						else
						{
							strcpy(text_out,"g;24;w;0");
							strcat(text_out,data.text+commandid_pos);
						}
						printf(LOG_PREFX"\nalert_id %s \nalert_name %s \nalert_date %s \nalert_time %s \nalert_eraly %s \nalert_once %s \nalert_year %s\n",alert_id,alert_name,alert_date,alert_time,alert_eraly,alert_once,alert_year);
						if(strlen(alert_id)!=0 &&
							strlen(alert_name)!=0 &&
							strlen(alert_date)!=0 &&
							strlen(alert_time)!=0 &&
							strlen(alert_eraly)!=0 &&
							strlen(alert_once)!=0 &&
							strlen(alert_year)!=0)
						write_alert(file_name,found,alert_id,alert_name,alert_date,alert_time,alert_eraly,alert_once,alert_year);
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
					if(file_line==0 || ((file_line%FILE_ALERT_LINES_RECORD)!=0))
					{
						strcat(text_out,"unknown");
						strcat(text_out,";unknown");
					}
					else
					{
						FILE *fp=open_file(file_name);
						if(fp!=NULL)
						{
							for(i=0;i<file_line;i=i+FILE_ALERT_LINES_RECORD)
							{
								if(read_file_line(fp,i,0,buf));
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
								if(read_file_line(fp,i,1,buf))
									strcat(text_out,buf);
								else
									strcat(text_out,"unknown");
								strcat(text_out,";");
								if(read_file_line(fp,i+1,1,buf))
									strcat(text_out,buf);
								else
									strcat(text_out,"unknown"); 		
							}
							else
							{
								strcat(text_out,";unknown");
								strcat(text_out,";unknown");
							}
							close_file(fp);
						}
						else
							strcat(text_out,"can not open file");
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
						strcat(text_out,";unknown");
						strcat(text_out,";unknown");
						strcat(text_out,";unknown");
						strcat(text_out,";unknown");
					}
					else
					{
						int i;
						FILE *fp=open_file(file_name);
						if(fp!=NULL)
						{
							for(i=0;i<file_line;i=i+FILE_ALARM_LINES_RECORD)
							{
								strcat(text_out,";");
								if(read_file_line(fp,i+0,0,buf))
									strcat(text_out,buf);
								else
									strcat(text_out,"unknown");
								memset(buf,'\0',25);
								strcat(text_out,";");
								if(read_file_line(fp,i+0,1,buf))
									strcat(text_out,buf);
								else
									strcat(text_out,"unknown");
								memset(buf,'\0',25);
								if(data.text[5]!='m')
								{
									strcat(text_out,";");
									if(read_file_line(fp,i+1,1,buf))
										strcat(text_out,buf);
									else
										strcat(text_out,"unknown");
									memset(buf,'\0',25);
								}
								strcat(text_out,";");
								if(read_file_line(fp,i+3,1,buf))
									strcat(text_out,buf);
								else
									strcat(text_out,"unknown");
							}
							close_file(fp);
						}
						else
							strcat(text_out,"can not open file");
						printf(LOG_PREFX"audio list %s\n",text_out);
					}
					memset(file_name,'\0',256);
				}
				else if(data.text[2]=='3' && data.text[3]=='5')
				{//35
					char buf[25]={0};
					char alarm_id[10]={0};
					len=strlen(FILE_PATH_NAME);
					file_name[len]=data.text[5];
					file_name[len+1]=data.text[6];
					strcat(file_name,".txt");
					printf(LOG_PREFX"cmd 25 to open %s\n",file_name);
					int found=0;
					strcpy(text_out,"d;35;m;");
					int i;
					int file_line=get_file_lines(file_name);
					if(file_line==0 || ((file_line%FILE_ALERT_LINES_RECORD)!=0))
					{
						strcat(text_out,"unknown");
						strcat(text_out,"unknown");
					}
					else
					{
						FILE *fp=open_file(file_name);
						if(fp!=NULL)
						{
							for(i=0;i<file_line;i=i+FILE_ALERT_LINES_RECORD)
							{
								if(read_file_line(fp,i+1,1,buf))
								{
									strcat(text_out,buf);
									strcat(text_out,";");
								}
								else
									strcat(text_out,"unknown;");
							}
							close_file(fp);
						}
						else
							strcat(text_out,"can not open file");
					}
					printf(LOG_PREFX"text out %s\n",text_out);
			}
			else if(data.text[2]=='3' && data.text[3]=='1')
			{
				char buf[25]={0};
				char alarm_id[10]={0};
				len=strlen(FILE_PATH_NAME);
				file_name[len]=data.text[7];
				file_name[len+1]=data.text[8];
				strcat(file_name,".txt");
				printf(LOG_PREFX"cmd 25 to open %s\n",file_name);
				int found=0;
				strcpy(text_out,"d;31;w");
				int i;
				int file_line=get_file_lines(file_name);
				if(file_line==0 || ((file_line%FILE_LAMP_INFO_LINES_RECORD)!=0))
				{
					strcat(text_out,"unknown");
				}
				else
				{
					FILE *fp=open_file(file_name);
					if(fp!=NULL)
					{
						for(i=0;i<file_line;i=i+FILE_LAMP_INFO_LINES_RECORD)
						{
							strcat(text_out,";");
							if(read_file_line(fp,i,1,buf))
							{
								strcat(text_out,buf);
								strcat(text_out,";");
							}
							else
								strcat(text_out,"unknown;");
							if(read_file_line(fp,i+1,1,buf))
							{
								strcat(text_out,buf);
								strcat(text_out,";");
							}
							else
								strcat(text_out,"unknown;");
							if(read_file_line(fp,i+3,1,buf))
							{
								strcat(text_out,buf);
								strcat(text_out,";");
							}
							else
								strcat(text_out,"unknown;");
							if(read_file_line(fp,i+4,1,buf))
							{
								strcat(text_out,buf);
							}
							else
								strcat(text_out,"unknown;");
						}
						close_file(fp);
					}
					else
						strcat(text_out,"can not open file");
				}
				printf(LOG_PREFX"text out %s\n",text_out);
			}
			else if(data.text[2]=='3' && data.text[3]=='2')
			{
				char buf[25]={0};
				char alarm_id[10]={0};
				len=strlen(FILE_PATH_NAME);
				file_name[len]=data.text[7];
				file_name[len+1]=data.text[8];
				strcat(file_name,".txt");
				printf(LOG_PREFX"cmd 25 to open %s\n",file_name);
				int found=0;
				strcpy(text_out,"d;32;w");
				int i;
				int file_line=get_file_lines(file_name);
				if(file_line==0 || ((file_line%FILE_LAMP_STATUS_LINES_RECORD)!=0))
				{
					strcat(text_out,"unknown");
				}
				else
				{
					FILE *fp=open_file(file_name);
					if(fp!=NULL)
					{
						for(i=0;i<file_line;i=i+FILE_LAMP_STATUS_LINES_RECORD)
						{
							strcat(text_out,";");
							if(read_file_line(fp,i,1,buf))
							{
								strcat(text_out,buf);
								strcat(text_out,";");
							}
							else
								strcat(text_out,"unknown;");
							if(read_file_line(fp,i+2,1,buf))
							{
								strcat(text_out,buf);
								strcat(text_out,";");
							}
							else
								strcat(text_out,"unknown;");
							if(read_file_line(fp,i+3,1,buf))
							{
								strcat(text_out,buf);
								strcat(text_out,";");
							}
							else
								strcat(text_out,"unknown;");
							if(read_file_line(fp,i+4,1,buf))
							{
								strcat(text_out,buf);
							}
							else
								strcat(text_out,"unknown;");
						}
						close_file(fp);
					}
					else
						strcat(text_out,"can not open file");
				}
				printf(LOG_PREFX"text out %s\n",text_out);
			}
			else if(data.text[2]=='3' && data.text[3]=='3')
			{
				int found=0;
				char buf[100]={0};
				char lamp_id[20]={0};
				char lamp_code[20]={0};
				char lamp_name[20]={0};
				char rom_position[20]={0};
				char wifi_ap[20]={0};
				char wifi_pwd[20]={0};
				char date_timeset[20]={0};
				int i=8,j=0,commandid_pos=0;
				len=strlen(FILE_PATH_NAME);
				file_name[len]=data.text[5];
				file_name[len+1]=data.text[6];
				strcat(file_name,".txt");
				printf("try to open %s\n",file_name);
				int file_line=get_file_lines(file_name);
				while(data.text[i]!='\0' && data.text[i]!=';')
					i++;
				memcpy(lamp_id,data.text+8,i-8);
				j=i++;
				while(data.text[i]!='\0' && data.text[i]!=';')
					i++;
				memcpy(lamp_code,data.text+j+1,i-j-1);
				j=i++;
				while(data.text[i]!='\0' && data.text[i]!=';')
					i++;
				memcpy(lamp_name,data.text+j+1,i-j-1);
				j=i++;
				while(data.text[i]!='\0' && data.text[i]!=';')
					i++;
				memcpy(rom_position,data.text+j+1,i-j-1);
				j=i++;
				while(data.text[i]!='\0' && data.text[i]!=';')
					i++;
				memcpy(wifi_ap,data.text+j+1,i-j-1);
				j=i++;
				while(data.text[i]!='\0' && data.text[i]!=';')
					i++;
				memcpy(wifi_pwd,data.text+j+1,i-j-1);
				j=i++;
				while(data.text[i]!='\0' && data.text[i]!=';')
					i++;
				memcpy(date_timeset,data.text+j+1,i-j-1);
				commandid_pos=i;
				FILE *fp=open_file(file_name);
				if(fp!=NULL)
				{
					for(i=0;i<file_line;i=i+FILE_LAMP_INFO_LINES_RECORD)
					{
						if(read_file_line(fp,i,0,buf));
						{
								if(strncmp(buf,lamp_id,strlen(buf))==0)
								{
									found=1;
									break;
								}
						}
					}
					close_file(fp);
				}
				else
					strcat(text_out,"can not open file");
				if(data.text[5]=='b')
					strcpy(text_out,"g;33;b;0");
				else
				{
					strcpy(text_out,"g;33;w;0");
					strcat(text_out,data.text+commandid_pos);
				}
				printf(LOG_PREFX"\nlamp_id %s \nlamp_code %s \nlamp_name %s \nrom_position %s \nwifi_ap %s \nwifi_pwd %s \ndatetime_sets %s \n",lamp_id,lamp_code,lamp_name,rom_position,wifi_ap,wifi_pwd,date_timeset);
				if(strlen(lamp_code)!=0 &&
					strlen(lamp_name)!=0 &&
					strlen(rom_position)!=0 &&
					strlen(wifi_ap)!=0 &&
					strlen(wifi_pwd)!=0 &&
					strlen(date_timeset)!=0)
				{
					printf(LOG_PREFX"going to writ_lamp %s\n",file_name);
					write_lamp(file_name,found,lamp_id,lamp_code,lamp_name,rom_position);
				}
				file_name[len]='1';
				file_name[len+1]='0';
				write_wifi(file_name,lamp_code,wifi_ap,wifi_pwd);
			}
			else if(data.text[2]=='3' && data.text[3]=='4')
			{
				int found=0;
				char buf[100]={0};
				char switch_id[20]={0};
				char switch_code[20]={0};
				char switch_name[20]={0};
				char rom_position[20]={0};
				int i=8,j=0,commandid_pos=0;
				len=strlen(FILE_PATH_NAME);
				file_name[len]=data.text[5];
				file_name[len+1]=data.text[6];
				strcat(file_name,".txt");
				int file_line=get_file_lines(file_name);
				while(data.text[i]!='\0' && data.text[i]!=';')
					i++;
				memcpy(switch_id,data.text+8,i-8);
				j=i++;
				while(data.text[i]!='\0' && data.text[i]!=';')
					i++;
				memcpy(switch_code,data.text+j+1,i-j-1);
				j=i++;
				while(data.text[i]!='\0' && data.text[i]!=';')
					i++;
				memcpy(switch_name,data.text+j+1,i-j-1);
				j=i++;
				while(data.text[i]!='\0' && data.text[i]!=';')
					i++;
				memcpy(rom_position,data.text+j+1,i-j-1);
				commandid_pos=i;
				FILE *fp=open_file(file_name);
				if(fp!=NULL)
				{
					for(i=0;i<file_line;i=i+FILE_LAMP_INFO_LINES_RECORD)
					{
						if(read_file_line(fp,i+3,0,buf));
						{
								if(strncmp(buf,switch_id,strlen(buf))==0)
								{
									found=1;
									break;
								}
						}
					}
					close_file(fp);
				}
				else
					strcat(text_out,"can not open file");
				if(data.text[5]=='b')
					strcpy(text_out,"g;34;b;0");
				else
				{
					strcpy(text_out,"g;34;w;0");
					strcat(text_out,data.text+commandid_pos);
				}
				printf(LOG_PREFX"\nswitch_code %s \nswitch_name %s \nrom_position %s \n",switch_code,switch_name,rom_position);
				if(strlen(switch_code)!=0 &&
					strlen(switch_name)!=0 &&
					strlen(rom_position)!=0)
				write_switch(file_name,found,switch_id,switch_code,switch_name,rom_position);
			}
			else if(data.text[0]=='a' && data.text[2]=='x' && data.text[3]=='x')
			{//35 tmp data to store
				char *tmp_buf=NULL;
				int flen=0;
				FILE *fp=fopen("/tmp/store.txt", "rb");
				if(fp!=NULL)
				{
					fseek(fp,0L,SEEK_END);
					flen=ftell(fp);
					fseek(fp,0L,SEEK_SET);
					tmp_buf=(char *)malloc(flen+1+strlen(data.text+5)+2);
					memset(tmp_buf,'\0',flen+1+2+strlen(data.text+5));
					fread(tmp_buf,flen,1,fp);
					fclose(fp);
					printf(LOG_PREFX"ori \n%s",tmp_buf);
					strcat(tmp_buf,data.text+5);
				}
				else
				{
					flen=0;
					tmp_buf=(char *)malloc(1+strlen(data.text+5)+2);
					memset(tmp_buf,'\0',1+2+strlen(data.text+5));
					strcpy(tmp_buf,data.text+5);
				}
				fp=fopen("/tmp/store.txt", "wb");
				strcat(tmp_buf,"\r\n");
				printf(LOG_PREFX"to store \n%s",tmp_buf);
				fwrite(tmp_buf,strlen(data.text+5)+2+flen,1,fp);
				fclose(fp);
				//memcpy(text_out,data.text,4);
			}
			else if(data.text[0]=='b' && data.text[2]=='x' && data.text[3]=='x')
			{//37 read tmp data from store
				char *tmp_buf=NULL;
				FILE *fp=fopen("/tmp/store.txt", "rb");
				fseek(fp,0L,SEEK_END);
				int flen=ftell(fp);
				fseek(fp,0L,SEEK_SET);
				strcpy(text_out,"e;");
				if(flen!=0)
				{
					char *line=NULL;
					size_t len;
					tmp_buf=(char *)malloc(flen+1);				
					memset(tmp_buf,'\0',flen+1);
					getline(&line, &len, fp);
					line[strlen(line)-2]='\0';
					strcat(text_out,line);
					fread(tmp_buf,flen,1,fp);
					fclose(fp);
					fp=fopen("/tmp/store.txt", "wb");
					fwrite(tmp_buf,strlen(tmp_buf),1,fp);
				}
				fclose(fp);
			}
			else if(data.text[2]=='6' && data.text[3]=='2')
			{//62
				char buf[25]={0};
				len=strlen(FILE_PATH_NAME);
				file_name[len]=data.text[7];
				file_name[len+1]=data.text[8];
				strcat(file_name,".txt");
				printf(LOG_PREFX"cmd 62 to open %s\n",file_name);
				operation=0;
				memcpy(text_out,data.text,6);
				text_out[0]='d';
				int file_line=get_file_lines(file_name);
				if(file_line==0 || ((file_line%FILE_11_LINES_RECORD)!=0))
				{
					strcat(text_out,";unknown");
					strcat(text_out,";unknown");
					strcat(text_out,";unknown");
					strcat(text_out,";unknown");
				}
				else
				{
					int i;
					FILE *fp=open_file(file_name);
					if(fp!=NULL)
					{
						for(i=0;i<file_line;i=i+FILE_11_LINES_RECORD)
						{
							strcat(text_out,";");
							if(read_file_line(fp,i,0,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
							memset(buf,'\0',25);
							strcat(text_out,";");
							if(read_file_line(fp,i+1,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
							memset(buf,'\0',25);
							strcat(text_out,";");
							if(read_file_line(fp,i+2,1,buf))
								strcat(text_out,buf);
							else
								strcat(text_out,"unknown");
						}
						close_file(fp);
					}
					else
						strcat(text_out,"can not open file");
					printf(LOG_PREFX"11 list %s\n",text_out);
				}
				memset(file_name,'\0',256);
			}
			else
				strcpy(text_out,"msg.text is wrong,please add \" \"");
			}		
			if(!(data.text[0]=='a' && data.text[2]=='x' && data.text[3]=='x'))
			send_msg(msgid,TYPE_FILE_TO_MAIN,FILE_TO_MAIN,text_out);
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

