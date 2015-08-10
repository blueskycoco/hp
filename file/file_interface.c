#include "audio_interface.h"

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
		char err_msg[256]={0};
		printf(LOG_PREFX"waiting MainCtlSystem cmd...\n");
		if(msgrcv(msgid, (void*)&data, sizeof(struct msg_st)-sizeof(long int), TYPE_MAIN_TO_AUDIO , 0)>=0)
		{
			if (fnmatch(PATTERN, entry->d_name, FNM_PATHNAME) == 0) {}
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
	#if 0
		static int audio_system_state=STATE_NULL;
		char err_msg[256]={0};
		printf(LOG_PREFX"waiting MainCtlSystem cmd...\n");
		if(msgrcv(msgid, (void*)&data, sizeof(struct msg_st)-sizeof(long int), TYPE_MAIN_TO_AUDIO , 0)>=0)
		{
		printf(LOG_PREFX"msgtype %d ,data id %d,text %s\n",data.msg_type,data.id,data.text);
		if(data.id==MAIN_TO_AUDIO)
		{
			switch (data.text[XXX_OFS])
			{
				case PROC_RECORD://start rec
				{
					switch (data.text[YYY_OFS])
					{
						case CMD_START_RECORD:
						{
							strcpy(err_msg,ACK_XF_RECORD_RESULT);
							if((!(audio_system_state&STATE_START_RECORD)))
							{
								audio_system_state|=STATE_START_RECORD;
								audio_system_state&=~STATE_STOP_RECORD;
								if((fpid=fork())==0)
								{
									send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,ACK_RECORD_START);
									rec(msgid,record_file);
									exit(0);
								}
							}
							else
							{
								strcat(err_msg,"already in recording ...");
								send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,err_msg);
							}
						}
						break;
						case CMD_STOP_RECORD:
						{
							strcpy(err_msg,ACK_XF_RECORD_RESULT);
							if(!(audio_system_state&STATE_STOP_RECORD)&&(audio_system_state&STATE_START_RECORD))
							{
								send_msg(msgid,TYPE_LOCAL_STOP_RECORD,0,NULL);
								ms_sleep(1);
								//if((fpid=fork())==0)
								{
									get_from_server(record_file,&rec_result);
									if(rec_result!=NULL)
									{
										char *tmp=strrchr(rec_result,'=');
										if(tmp!=NULL)
										{
											strcpy(res,tmp+1);
											printf(LOG_PREFX"res %s\n",res);
										}
										else
											memset(res,'\0',256);
										free(rec_result);
										rec_result=NULL;
									}
									char cmd[256];
									strcpy(cmd,"rm ");
									strcat(cmd,record_file);
									system(cmd);
									audio_system_state|=STATE_STOP_RECORD;
									audio_system_state&=~STATE_START_RECORD;
									//exit(0);
								}
								send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,ACK_RECORD_STOP);
							}
							else
							{
								strcat(err_msg,"already in stoped ...");
								send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,err_msg);
							}
						}
						break;
						case CMD_CHECK_RECORD:
						{
							char result[256]={0};
							strcpy(result,ACK_XF_RECORD_RESULT);
							printf(LOG_PREFX"res is %s\n",res);
							if(strlen(res)!=0)
								strcat(result,res);
							else
								strcat(result,"0");
							memset(res,'\0',256);
							send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,result);
						}
						break;
						default:
							break;
					}
				}
				break;
				case PROC_PLAYBACK:
				{
					int len=strlen(data.text)-2;
					strcpy(err_msg,ACK_PLAYBACK_LOCAL_FAILED);
					if(!((audio_system_state&STATE_XF_PLAYBACK_BEGIN)||(audio_system_state&STATE_PLAYBACK_LOCAL_BEGIN)))
					{
						if(len!=0)
						{						
							char *play_file=(char *)malloc(len+1);
							memset(play_file,'\0',len+1);
							memcpy(play_file,data.text+2,len);
							audio_system_state|=STATE_PLAYBACK_LOCAL_BEGIN;
							audio_system_state&=~STATE_PLAYBACK_LOCAL_END;							
							send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,ACK_PLAYBACK_LOCAL_START);
							//if((fpid=fork())==0)
							{
								if(playback(play_file)==1)
									send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,ACK_PLAYBACK_LOCAL_STOP);
								else
								{
									strcat(err_msg,"play back local file failed");
									send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,err_msg);
								}
								free(play_file);
							}						
							audio_system_state&=~STATE_PLAYBACK_LOCAL_BEGIN;
							audio_system_state|=STATE_PLAYBACK_LOCAL_END;							
						}
						else
						{
							if(audio_system_state&STATE_PLAYBACK_LOCAL_BEGIN)
								send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,ACK_PLAYBACK_LOCAL_START);
							else
								send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,ACK_PLAYBACK_LOCAL_STOP);
						}
						
					}
					else
					{
						strcat(err_msg,"already in xf playcking or local playbacking...");
						send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,err_msg);
					}	
					
				}
				break;
				case XF_PLAYBACK:
				{
					int len=strlen(data.text)-2;
					strcpy(err_msg,ACK_PLAYBACK_XF_FAILED);
					if(!((audio_system_state&STATE_XF_PLAYBACK_BEGIN)||(audio_system_state&STATE_PLAYBACK_LOCAL_BEGIN)))
					{
						if(len!=0)
						{
							char *audio_string=(char *)malloc(len+1);
							memset(audio_string,'\0',len+1);
							memcpy(audio_string,data.text+2,len);
							audio_system_state|=STATE_XF_PLAYBACK_BEGIN;
							audio_system_state&=~STATE_XF_PLAYBACK_END;							
							send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,ACK_PLAYBACK_XF_START);
							//if((fpid=fork())==0)
							{
								if(play(audio_string,playback_file)==0)
								{
									send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,ACK_PLAYBACK_XF_STOP);
									if(playback(playback_file)==1)
										send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,ACK_PLAYBACK_XF_OK);
									else
									{
										strcat(err_msg,"play back xf file failed");
										send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,err_msg);
									}
									send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,ACK_PLAYBACK_XF_UNDO);
								}
								else
								{
									strcat(err_msg,"server do xf failed, ");
									strcat(err_msg,audio_string);
									send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,err_msg);
								}
								free(audio_string);
								audio_system_state&=~STATE_XF_PLAYBACK_BEGIN;
								audio_system_state|=STATE_XF_PLAYBACK_END; 						
							}
							
						}
						else
						{
							strcat(err_msg,"audio_string is null");
							send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,err_msg);
						}
					}
					else
					{
						strcat(err_msg,"already in xf playcking or local playbacking...");
						send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,err_msg);
					}
					
				}
				break;
				default:
					break;
			}
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
			ms_sleep(1);
		}
		#endif
	}
	waitpid(fpid, &status, 0);
	
	return 0;
}

