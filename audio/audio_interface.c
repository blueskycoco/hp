#include "audio_interface.h"

int cancle_rec=0;
char GrammarID[128];
int run_asr(char *grammar_id,const char* asrfile ,  const char* param, unsigned char **rec_result)
{
	int result=1;
	const char *sessionID;
	FILE *f_pcm = NULL;
	char *pPCM = NULL;
	int lastAudio = 0 ;
	int audStat = 2 ;
	int epStatus = 0;
	int recStatus = 0 ;
	long pcmCount = 0;
	long pcmSize = 0;
	int ret = 0 ;
	*rec_result=NULL;
	
	sessionID = QISRSessionBegin(grammar_id, param, &ret); //asr
	if(ret !=0)
	{
		printf(LOG_PREFX"QISRSessionBegin Failed,ret=%d\n",ret);
		result=1;
	}
	else
	{
		f_pcm = fopen(asrfile, "rb");
		if (NULL != f_pcm) 
		{
			fseek(f_pcm, 0, SEEK_END);
			pcmSize = ftell(f_pcm);
			fseek(f_pcm, 0, SEEK_SET);
			pPCM = (char *)malloc(pcmSize);
			fread((void *)pPCM, pcmSize, 1, f_pcm);
			fclose(f_pcm);
			f_pcm = NULL;
		}
		else
			return 0;
		while (1) 
		{
			unsigned int len = 6400;
			unsigned int audio_len = 6400;
			if (pcmSize < 12800) 
			{
				len = pcmSize;
				lastAudio = 1;
			}
			audStat = 2;
			if (pcmCount == 0)
				audStat = 1;
			if (0)
			{
				if (audStat == 1)
					audStat = 5;
				else
					audStat = 4;
			}
			if (len<=0)
			{
				break;
			}
			//printf("\ncsid=%s,count=%d,aus=%d,",sessionID,pcmCount/audio_len,audStat);
			ret = QISRAudioWrite(sessionID, (const void *)&pPCM[pcmCount], len, audStat, &epStatus, &recStatus);
			//printf("eps=%d,rss=%d,ret=%d",epStatus,recStatus,ret);
			if (ret != 0)
				break;
			pcmCount += (long)len;
			pcmSize -= (long)len;
			if (recStatus == 0) 
			{
				const char *rslt = QISRGetResult(sessionID, &recStatus, 0, &ret);
				if (ret !=0)
				{
					printf(LOG_PREFX"QISRGetResult Failed,ret=%d\n",ret);
					break;
				}
				if (NULL != rslt)
				{
					if(*rec_result==NULL)
					{
						*rec_result=(char *)malloc(4096);
						memset(*rec_result,'\0',4096);
					}
					strcat(*rec_result,rslt);
				}
			}
			if (epStatus == MSP_EP_AFTER_SPEECH)
				break;
			usleep(150);
		}
		ret=QISRAudioWrite(sessionID, (const void *)NULL, 0, 4, &epStatus, &recStatus);
		if (ret !=0)
		{
			printf(LOG_PREFX"QISRAudioWrite Failed,ret=%d\n",ret);
		}
		free(pPCM);
		pPCM = NULL;
		while (recStatus != 5 && ret == 0) 
		{
			const char *rslt = QISRGetResult(sessionID, &recStatus, 0, &ret);
			if (NULL != rslt)
			{
				if(*rec_result==NULL)
				{
					*rec_result=(char *)malloc(4096);
					memset(*rec_result,'\0',4096);
				}
				strcat(*rec_result,rslt);
			}
			usleep(150);
		}
		ret=QISRSessionEnd(sessionID, NULL);
		if(ret !=MSP_SUCCESS)
		{
			printf(LOG_PREFX"QISRSessionEnd Failed, ret=%d\n",ret);
			result=1;
		}
	}
	printf(LOG_PREFX"result %s\n",*rec_result);
	return result;
}

int get_from_server(char *grammar_id,char *file,unsigned char **rec_result)
{
	const char* login_config = "appid = 55801297,work_dir =   .  ";
	const char* param = "rst=plain,rse=utf8,sub=asr,aue=speex-wb,auf=audio/L16;rate=16000,ent=sms16k";    //注意sub=asr,16k音频aue=speex-wb，8k音频识别aue=speex，
	int ret = 0 ;
	int result=1;
	ret = MSPLogin(NULL, NULL, login_config);
	if ( ret != MSP_SUCCESS )
	{
		printf(LOG_PREFX"MSPLogin failed , Error code %d.\n",ret);
		return 1 ;
	}
	else
	{
		//strcpy(GrammarID, "e7eb1a443ee143d5e7ac52cb794810fe");
		result = run_asr(grammar_id,file, param, rec_result);
		if(result == 1)
		{
			printf(LOG_PREFX"run_asr with errorCode: %d \n", ret);
		}
		MSPLogout();
	}
	return result;
}

void rec(/*int msgid,*/char *filename)
{
	MSFilter *f1_r,*f1_w,*record;
	MSSndCard *card_capture1;
	MSSndCard *card_playback1;
	MSTicker *ticker1;
	//struct msg_st data;
	long int msgtype = 0;
	char *capt_card1=NULL,*play_card1=NULL;
	int rate = 8000;
	int nchan=2;
	int i;
	const char *alsadev=NULL;

	ortp_init();
	ortp_set_log_level_mask(/*ORTP_MESSAGE|ORTP_WARNING|ORTP_ERROR|*/ORTP_FATAL);
	ms_init();

	card_capture1 = ms_snd_card_manager_get_default_capture_card(ms_snd_card_manager_get());
	card_playback1 = ms_snd_card_manager_get_default_playback_card(ms_snd_card_manager_get());
	if (card_playback1==NULL || card_capture1==NULL)
	{
		if(card_playback1==NULL)
			ms_error(LOG_PREFX"No card. card_playback1 %s",capt_card1);
		if(card_capture1==NULL)
			ms_error(LOG_PREFX"No card. card_capture1 %s",capt_card1);
		return ;
	}
	else
	{
		ms_warning(LOG_PREFX"card_playback1 %s|%s|%s|%d|%d|%d",card_playback1->name,card_playback1->id,card_playback1->desc->driver_type,
				card_playback1->capabilities,card_playback1->latency,card_playback1->preferred_sample_rate);
	}
	record=ms_filter_new(MS_FILE_REC_ID);
	if(ms_filter_call_method(record,MS_FILE_REC_OPEN,(void*)filename)!=0)
	{
		printf(LOG_PREFX"record open file %s failed\n",filename);
		return ;
	}
	f1_r=ms_snd_card_create_reader(card_capture1);
	if(f1_r!=NULL&&record!=NULL)
	{
		if(ms_filter_call_method(f1_r, MS_FILTER_SET_SAMPLE_RATE,	&rate)!=0)
			printf(LOG_PREFX"set sample rate f1_r failed\n");
		if(ms_filter_call_method(record, MS_FILTER_SET_SAMPLE_RATE,&rate)!=0)
			printf(LOG_PREFX"set sample rate record failed\n");
		if(ms_filter_call_method(f1_r, MS_FILTER_SET_NCHANNELS,	&nchan)!=0)
			printf(LOG_PREFX"set nchan f1_r failed\n");
		if(ms_filter_call_method(record, MS_FILTER_SET_NCHANNELS,&nchan)!=0)
			printf(LOG_PREFX"set nchan record failed\n");
		ms_filter_call_method_noarg(record,MS_FILE_REC_START);
		ticker1=ms_ticker_new();
		ms_ticker_set_name(ticker1,"card1 to card2");
		ms_filter_link(f1_r,0,record,0);	 	
		ms_ticker_attach(ticker1,f1_r);
		//msgrcv(msgid, (void*)&data, sizeof(struct msg_st)-sizeof(long int), TYPE_LOCAL_STOP_RECORD, 0);
		ms_sleep(3);
		ms_filter_call_method(record,MS_FILE_REC_STOP,NULL);
		ms_filter_call_method(record,MS_FILE_REC_CLOSE,NULL);
		if(ticker1) ms_ticker_detach(ticker1,f1_r);
		if(f1_r&&record) ms_filter_unlink(f1_r,0,record,0);
		if(ticker1) ms_ticker_destroy(ticker1);
		if(f1_r) ms_filter_destroy(f1_r);
		if(record) ms_filter_destroy(record);		
	}	
}
static void fileplay_eof(void *user_data, MSFilter *f, unsigned int event, void *event_data) {
	if (event == MS_FILE_PLAYER_EOF) {
		int *done = (int *)user_data;
		*done = TRUE;
	}
	MS_UNUSED(f), MS_UNUSED(event_data);
}
void set_vol(int vol)
{
	char cmd[256]={0};
	sprintf(cmd,"amixer cset numid=3,iface=MIXER,name=\'Headphone Playback Volume\' %d", vol);
	printf(LOG_PREFX"cmd is %s",cmd);
	system(cmd);
}
int playback(int msgid,char *filename,int vol,int step)
{
	MSFilter *f1_w,*play;
	MSSndCard *card_playback1;
	MSTicker *ticker1;
	int i=0,ret=0;
	struct msg_st data;
	int done = FALSE;
	struct msg_st data_w;
	long int msgtype = 0;
	int rate = 16000;
	int nchan=2;
	ortp_init();
	ortp_set_log_level_mask(/*ORTP_MESSAGE|ORTP_WARNING|ORTP_ERROR|*/ORTP_FATAL);
	ms_init();
	if(vol!=0)
	{
		set_vol(vol);
	}
	card_playback1 = ms_snd_card_manager_get_default_playback_card(ms_snd_card_manager_get());
	if (card_playback1==NULL)
	{
		if(card_playback1==NULL)
			ms_error("No card. card_playback");
		return 0;
	}
	else
	{
		ms_warning("card_playback1 %s|%s|%s|%d|%d|%d",card_playback1->name,card_playback1->id,card_playback1->desc->driver_type,
				card_playback1->capabilities,card_playback1->latency,card_playback1->preferred_sample_rate);
	}
	play=ms_filter_new(MS_FILE_PLAYER_ID);
	if(ms_filter_call_method(play,MS_FILE_PLAYER_OPEN,(void*)filename)!=0)
	{
		printf(LOG_PREFX"play open file %s failed\n",filename);
		return 0;
	}
	ms_filter_set_notify_callback(play, fileplay_eof, &done);
	f1_w=ms_snd_card_create_writer(card_playback1);
	if(f1_w!=NULL&&play!=NULL)
	{
		ms_filter_call_method(play, MS_FILTER_GET_SAMPLE_RATE, &rate);
		ms_filter_call_method(play, MS_FILTER_GET_NCHANNELS, &nchan);
		if(ms_filter_call_method(f1_w, MS_FILTER_SET_SAMPLE_RATE,	&rate)!=0)
			printf(LOG_PREFX"set sample rate f1_r failed\n");
		if(ms_filter_call_method(f1_w, MS_FILTER_SET_NCHANNELS,&nchan)!=0)
			printf(LOG_PREFX"set sample rate record failed\n");
		ms_filter_call_method_noarg(play,MS_FILE_PLAYER_START);
		ticker1=ms_ticker_new();
		ms_ticker_set_name(ticker1,"card1 to card2");
		ms_filter_link(play,0,f1_w,0);		
		ms_ticker_attach(ticker1,play);
		while (done != TRUE) 
		{
			if(msgrcv(msgid, (void*)&data, sizeof(struct msg_st)-sizeof(long int), TYPE_LOCAL_STOP_PLAYBACK, IPC_NOWAIT)>0)
				break;
			if(step!=0)
			{
				set_vol(vol+step*i);
			}
			ms_usleep(10000);
			i++;
		}

		ms_filter_call_method_noarg(play, MS_FILE_PLAYER_CLOSE);
		if(ticker1) ms_ticker_detach(ticker1,play);
		if(f1_w&&play) ms_filter_unlink(play,0,f1_w,0);
		if(ticker1) ms_ticker_destroy(ticker1);
		if(f1_w) ms_filter_destroy(f1_w);
		if(play) ms_filter_destroy(play);
		ret=1;
	}	
	return ret;
}

int text_to_speech(const char* src_text ,const char* des_path ,const char* params)
{
	struct wave_pcm_hdr pcmwavhdr = default_pcmwavhdr;
	const char* sess_id = NULL;
	int ret = 0;
	unsigned int text_len = 0;
	char* audio_data;
	unsigned int audio_len = 0;
	int synth_status = 1;
	FILE* fp = NULL;
	if (NULL == src_text || NULL == des_path)
	{
		printf(LOG_PREFX"params is null!\n");
		return -1;
	}
	text_len = (unsigned int)strlen(src_text);
	fp = fopen(des_path,"wb");
	if (NULL == fp)
	{
		printf(LOG_PREFX"open file %s error\n",des_path);
		return -1;
	}
	sess_id = QTTSSessionBegin(params, &ret);
	if ( ret != MSP_SUCCESS )
	{
		printf(LOG_PREFX"QTTSSessionBegin: qtts begin session failed Error code %d.\n",ret);
		return ret;
	}

	ret = QTTSTextPut(sess_id, src_text, text_len, NULL );
	if ( ret != MSP_SUCCESS )
	{
		printf(LOG_PREFX"QTTSTextPut: qtts put text failed Error code %d.\n",ret);
		QTTSSessionEnd(sess_id, "TextPutError");
		return ret;
	}
	fwrite(&pcmwavhdr, sizeof(pcmwavhdr) ,1, fp);
	while (1) 
	{
		const void *data = QTTSAudioGet(sess_id, &audio_len, &synth_status, &ret);
		if (NULL != data)
		{
			fwrite(data, audio_len, 1, fp);
			pcmwavhdr.data_size += audio_len;//修正pcm数据的大小
		}
		//printf("\nget audio...\n");
		usleep(1500);//建议可以sleep下，因为只有云端有音频合成数据，audioget都能获取到音频。
		if (synth_status == 2 || ret != 0) 
			break;
	}
	pcmwavhdr.size_8 += pcmwavhdr.data_size + 36;
	fseek(fp, 4, 0);
	fwrite(&pcmwavhdr.size_8,sizeof(pcmwavhdr.size_8), 1, fp);
	fseek(fp, 40, 0);
	fwrite(&pcmwavhdr.data_size,sizeof(pcmwavhdr.data_size), 1, fp);
	fclose(fp);

	ret = QTTSSessionEnd(sess_id, NULL);
	if ( ret != MSP_SUCCESS )
	{
		printf(LOG_PREFX"QTTSSessionEnd: qtts end failed Error code %d.\n",ret);
	}
	//printf("\nTTS end...\n");
	return ret;
}

int play(const char *string,const char *filename)
{
	const char* login_configs = " appid = 55801297, work_dir =	 .	";
	const char* text  = "科大讯飞作为中国最大的智能语音技术提供商，在智能语音技术领域有着长期的研究积累，并在中文语音合成、语音识别、口语评测等多项技术上拥有国际领先的成果。";
	const char* param = "vcn=xiaoyan,aue = speex-wb,auf=audio/L16;rate=16000,spd = 5,vol = 5,tte = utf8";//8k音频合成参数：aue=speex,auf=audio/L16;rate=8000,其他参数意义参考参数列表
	int ret = 0;
	char key = 0;

	ret = MSPLogin(NULL, NULL, login_configs);
	if ( ret != MSP_SUCCESS )
	{
		printf(LOG_PREFX"MSPLogin failed , Error code %d.\n",ret);
		return ret;
	}
	ret = text_to_speech(string,filename,param);
	if ( ret != MSP_SUCCESS )
	{
		printf(LOG_PREFX"text_to_speech: failed , Error code %d.\n",ret);
	}
	MSPLogout();
	return ret;
}
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
	char *audio_system_state=NULL;
	char *grammar_id=NULL;
	int vol=0;
	char vol_str[4]={0};
	key_t shmid;  

	msgid = msgget((key_t)1234, 0666 | IPC_CREAT);  
	if(msgid == -1)  
	{  
		fprintf(stderr, LOG_PREFX"msgget failed with error: %d\n", errno);  
		exit(-1);  
	}
	else
	printf(LOG_PREFX"msgid %d\n",msgid);			
	
	if(argv[1]==NULL)
		strcpy(record_file,"/tmp/rec.wav");
	else
		strcpy(record_file,argv[1]);
	if(argv[2]==NULL)
		strcpy(playback_file,"/tmp/3.wav");
	else
		strcpy(playback_file,argv[2]);
	if((shmid = shmget(IPC_PRIVATE, 2, PERM)) == -1 )
	{
        fprintf(stderr, LOG_PREFX"Create Share Memory Error:%s/n/a", strerror(errno));  
        exit(1);  
    }  
	audio_system_state = (char *)shmat(shmid, 0, 0);
	while(1)
	{
		char text_out[256]={0};
		char err_msg[256]={0};
		printf(LOG_PREFX"waiting MainCtlSystem cmd...\n");
		if(msgrcv(msgid, (void*)&data, sizeof(struct msg_st)-sizeof(long int), TYPE_MAIN_TO_AUDIO , 0)>=0)
		{
			printf(LOG_PREFX"msgtype %d ,data id %d,text %s\n",data.msg_type,data.id,data.text);
			if(data.id==MAIN_TO_AUDIO)
			{
				if(strstr(data.text, CMD_RCV_GRAMMAR_ID)!=NULL)
				{
					//set grammarid
					if(strlen(data.text)>4)
					{
						printf(LOG_PREFX"set grammarid %s\n",data.text+4);
						if(grammar_id)
						{
							free(grammar_id);
						}
						grammar_id=(char *)malloc(strlen(data.text)-3);
						memset(grammar_id,'\0',strlen(data.text)-3);
						memcpy(grammar_id,data.text+4,strlen(data.text)-4);
						strcpy(text_out,"r;2");
					}
					else
						strcpy(text_out,"r;0");
				}
				else if(strncmp(CMD_START_RECORD, data.text, strlen(CMD_START_RECORD)) == 0)
				{
					//audio record
					if((!(*audio_system_state&MUSIC_RECORD_START)) && (grammar_id!=NULL))
					{
						*audio_system_state|=MUSIC_RECORD_START;
						strcpy(text_out,"s;1");
						if((fpid=fork())==0)
						{
							char *audio_state=(char *)shmat(shmid, 0, 0);
							rec(record_file);
							get_from_server(grammar_id,record_file,&rec_result);
							if(rec_result!=NULL)
							{
								char *tmp=strrchr(rec_result,'=');
								if(tmp!=NULL)
								{
									strcpy(res,"r;");
									strcat(res,tmp+1);
									printf(LOG_PREFX"res %s\n",res);
								}
								else
									memset(res,'\0',256);
								free(rec_result);
								rec_result=NULL;
							}
							*audio_state&=~MUSIC_RECORD_START;
							shmdt(audio_state);
							if(strlen(res)==0)
								strcat(res,"r;failed");
							send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,res);
							exit(0);
						}
					}
					else
					{
						strcpy(text_out,"s;0");
					}
				}
				else if(strstr(data.text, CMD_51_SET_VOL)!=NULL)
				{
					//set vol from main process
					printf(LOG_PREFX"set vol %s\n",data.text+3);
					vol=atoi(data.text+3);
					memcpy(vol_str,data.text+3,strlen(data.text)-3);
					//set to low layer
					set_vol(vol);
					memcpy(text_out,data.text,3);
					strcat(text_out,"0");
				}
				else if(strncmp(CMD_50_GET_VOL, data.text, strlen(CMD_50_GET_VOL)) == 0)
				{
					//get vol from audio sub system 
					printf(LOG_PREFX"get vol \n");
					memcpy(text_out,data.text,2);
					strcat(text_out,";");
					strcat(text_out,vol_str);
				}
				else if(strncmp(CMD_21_RING_NOW_ARM, data.text, strlen(CMD_21_RING_NOW_ARM)) == 0)
				{
					//ring
					if(!(*audio_system_state&MUSIC_PLAY_START))
					{
						*audio_system_state|=MUSIC_PLAY_START;
						printf(LOG_PREFX"ring coming %s\n",data.text);
						char *index=strchr(data.text,';');
						char file_name[64]={0},vol_cur[64]={0},blance[64]={0},step[64]={0};
						int i=1,j=0,vol_i,step_i;
						while(index[i]!=';' && index[i]!='\0')
						{
							file_name[j++]=index[i++];							
						}
						printf(LOG_PREFX"file name is %s\n",file_name);
						if(index[i]!='\0')
						{
							i++;
							j=0;
							while(index[i]!=';' && index[i]!='\0')
							{
								vol_cur[j++]=index[i++];
							}
							printf(LOG_PREFX"vol_cur is %s\n",vol_cur);
						}
						else
							printf(LOG_PREFX"can not get vol\n");
						
						if(index[i]!='\0')
						{
							i++;
							j=0;
							while(index[i]!=';' && index[i]!='\0')
							{
								blance[j++]=index[i++];
							}
							printf(LOG_PREFX"blance is %s\n",blance);
						}
						else
							printf(LOG_PREFX"can not get blance\n");
						if(index[i]!='\0')
						{
							i++;
							j=0;
							while(index[i]!=';' && index[i]!='\0')
							{
								step[j++]=index[i++];
							}
							printf(LOG_PREFX"step is %s\n",step);
						}
						else
							printf(LOG_PREFX"can not get step\n");
						int freq=atoi(strrchr(data.text,';')+1);
						vol_i=atoi(vol_cur);
						step_i=atoi(step);
						if((strlen(step)!=0) && (strlen(vol_cur)!=0) && (strlen(file_name)!=0))
						{
							if((fpid=fork())==0)
							{	
								for(i=0;i<freq;i++)
								playback(msgid,file_name,vol_i,step_i);
								*audio_system_state&=~MUSIC_PLAY_START;
								set_vol(vol);
								strcpy(text_out,"b;21;w;2");
								send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,text_out);
								exit(0);
							}
						}
						else
						{	
							printf(LOG_PREFX"paramter is wrong %s\n",data.text);	
							strcpy(text_out,"b;21;w;0");
							send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,text_out);
						}
					}
					else
					{
						printf(LOG_PREFX"already in music playing ,please wait %s\n",data.text);	
						strcpy(text_out,"b;21;w;0");
						send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,text_out);
					}
					
				}
				else if(strlen(data.text)>7) 
				{		
					//music play or stop
					char mach_play_stop[8]={0};
					memcpy(mach_play_stop,data.text,7);
					if(fnmatch(CMD_01_MUSIC_PLAY, mach_play_stop, FNM_PATHNAME) == 0)
					{	
						if(!(*audio_system_state&MUSIC_PLAY_START))
						{
							//play music from web,mcu,bluetooth
							printf(LOG_PREFX"play music from web,mcu,bluetooth %s\n",data.text+7);
							*audio_system_state|=MUSIC_PLAY_START;
							if((fpid=fork())==0)
							{
								char *audio_state=(char *)shmat(shmid, 0, 0);
								memcpy(text_out,mach_play_stop,7);
								if(playback(msgid,data.text+7,0,0)==1)
								{
									strcat(text_out,"1");
								}
								else
								{
									strcat(text_out,"0");
								}
								*audio_state&=~MUSIC_PLAY_START;
								shmdt(audio_state);
								send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,text_out);
								exit(0);
							}
						}
						else
						{
							printf(LOG_PREFX"already in music playing ,please wait %s\n",data.text+7);						
							memcpy(text_out,mach_play_stop,7);
							strcat(text_out,"0");
							send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,text_out);
						}
					}
					else if(fnmatch(CMD_02_MUSIC_STOP, mach_play_stop, FNM_PATHNAME) == 0)
					{
						//stop play music
						printf(LOG_PREFX"stop play music\n");
						if(*audio_system_state&MUSIC_PLAY_START)
						{
							send_msg(msgid,TYPE_LOCAL_STOP_PLAYBACK,0,NULL);
							ms_sleep(1);
							*audio_system_state&=~MUSIC_PLAY_START;
						}
						memcpy(text_out,mach_play_stop,6);
					}				
				}
				if(strlen(data.text)>4)
				{
					if(strncmp(data.text+2,"01",2)!=0)
						send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,text_out);
				}
			}
			else
				printf(LOG_PREFX"wrong id\n");
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
				printf(LOG_PREFX"new msgid %d\n",msgid);			
			ms_sleep(1);
		}
	}
	waitpid(fpid, &status, 0);
	return 0;
}

