#include "audio_interface.h"

int cancle_rec=0;
char GrammarID[128];
unsigned char g_audio_state=STATUS_STOP_RECORD;
int run_asr(const char* asrfile ,  const char* param, unsigned char **rec_result)
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
	
	sessionID = QISRSessionBegin(GrammarID, param, &ret); //asr
	if(ret !=0)
	{
		printf(LOG_PREFX"QISRSessionBegin Failed,ret=%d\n",ret);
		result=0;
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
			result=0;
		}
	}
	return result;
}

int get_from_server(char *file,unsigned char **rec_result)
{
	const char* login_config = "appid = 55801297,work_dir =   .  ";
	const char* param = "rst=plain,rse=utf8,sub=asr,aue=speex-wb,auf=audio/L16;rate=16000,ent=sms16k";    //注意sub=asr,16k音频aue=speex-wb，8k音频识别aue=speex，
	int ret = 0 ;
	int result=1;
	ret = MSPLogin(NULL, NULL, login_config);
	if ( ret != MSP_SUCCESS )
	{
		printf(LOG_PREFX"MSPLogin failed , Error code %d.\n",ret);
		return 0 ;
	}
	else
	{
		strcpy(GrammarID, "e7eb1a443ee143d5e7ac52cb794810fe");
		result = run_asr(file, param, rec_result);
		if(result == 0)
		{
			printf(LOG_PREFX"run_asr with errorCode: %d \n", ret);
		}
		MSPLogout();
	}
	return result;
}

void rec(int msgid,char *filename)
{
	MSFilter *f1_r,*f1_w,*record;
	MSSndCard *card_capture1;
	MSSndCard *card_playback1;
	MSTicker *ticker1;
	struct msg_st data;
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
		msgrcv(msgid, (void*)&data, sizeof(struct msg_st)-sizeof(long int), TYPE_LOCAL_STOP_RECORD, 0);
		g_audio_state=STATUS_START_RECORD;

		ms_filter_call_method(record,MS_FILE_REC_STOP,NULL);
		ms_filter_call_method(record,MS_FILE_REC_CLOSE,NULL);
		if(ticker1) ms_ticker_detach(ticker1,f1_r);
		if(f1_r&&record) ms_filter_unlink(f1_r,0,record,0);
		if(ticker1) ms_ticker_destroy(ticker1);
		if(f1_r) ms_filter_destroy(f1_r);
		if(record) ms_filter_destroy(record);		
		g_audio_state=STATUS_STOP_RECORD;
	}	
}
static void fileplay_eof(void *user_data, MSFilter *f, unsigned int event, void *event_data) {
	if (event == MS_FILE_PLAYER_EOF) {
		int *done = (int *)user_data;
		*done = TRUE;
	}
	MS_UNUSED(f), MS_UNUSED(event_data);
}

int playback(char *filename)
{
	MSFilter *f1_w,*play;
	MSSndCard *card_playback1;
	MSTicker *ticker1;
	int i,ret=0;
	int done = FALSE;
	struct msg_st data_w;
	long int msgtype = 0;
	int rate = 16000;
	int nchan=2;
	ortp_init();
	ortp_set_log_level_mask(/*ORTP_MESSAGE|ORTP_WARNING|ORTP_ERROR|*/ORTP_FATAL);
	ms_init();

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
		printf("play open file %s failed\n",filename);
		return 0;
	}
	ms_filter_set_notify_callback(play, fileplay_eof, &done);
	f1_w=ms_snd_card_create_writer(card_playback1);
	if(f1_w!=NULL&&play!=NULL)
	{
		ms_filter_call_method(play, MS_FILTER_GET_SAMPLE_RATE, &rate);
		ms_filter_call_method(play, MS_FILTER_GET_NCHANNELS, &nchan);
		if(ms_filter_call_method(f1_w, MS_FILTER_SET_SAMPLE_RATE,	&rate)!=0)
			printf("set sample rate f1_r failed\n");
		if(ms_filter_call_method(f1_w, MS_FILTER_SET_NCHANNELS,&nchan)!=0)
			printf("set sample rate record failed\n");
		ms_filter_call_method_noarg(play,MS_FILE_PLAYER_START);
		ticker1=ms_ticker_new();
		ms_ticker_set_name(ticker1,"card1 to card2");
		ms_filter_link(play,0,f1_w,0);		
		ms_ticker_attach(ticker1,play);
		while (done != TRUE) {
			ms_usleep(10000);
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
		printf("params is null!\n");
		return -1;
	}
	text_len = (unsigned int)strlen(src_text);
	fp = fopen(des_path,"wb");
	if (NULL == fp)
	{
		printf("open file %s error\n",des_path);
		return -1;
	}
	sess_id = QTTSSessionBegin(params, &ret);
	if ( ret != MSP_SUCCESS )
	{
		printf("QTTSSessionBegin: qtts begin session failed Error code %d.\n",ret);
		return ret;
	}

	ret = QTTSTextPut(sess_id, src_text, text_len, NULL );
	if ( ret != MSP_SUCCESS )
	{
		printf("QTTSTextPut: qtts put text failed Error code %d.\n",ret);
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
		printf("QTTSSessionEnd: qtts end failed Error code %d.\n",ret);
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
		printf("MSPLogin failed , Error code %d.\n",ret);
		return ret;
	}
	ret = text_to_speech(string,filename,param);
	if ( ret != MSP_SUCCESS )
	{
		printf("text_to_speech: failed , Error code %d.\n",ret);
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

	msgid = msgget((key_t)1234, 0666 | IPC_CREAT);  
	if(msgid == -1)  
	{  
		fprintf(stderr, LOG_PREFX"msgget failed with error: %d\n", errno);  
		exit(-1);  
	}
	if(argv[1]==NULL)
		strcpy(record_file,"/tmp/rec.wav");
	else
		strcpy(record_file,argv[1]);
	if(argv[2]==NULL)
		strcpy(record_file,"/tmp/3.wav");
	else
		strcpy(record_file,argv[2]);
	while(1)
	{		
		static int audio_system_state=STATE_NULL;
		char err_msg[256]={0};
		printf(LOG_PREFX"waiting MainCtlSystem cmd...\n");
		msgrcv(msgid, (void*)&data, sizeof(struct msg_st)-sizeof(long int), TYPE_MAIN_TO_AUDIO , 0);
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
								send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,ACK_RECORD_STOP);
								ms_sleep(100);
								if((fpid=fork())==0)
								{
									get_from_server(record_file,&rec_result);
									if(rec_result!=NULL)
									{
										char *tmp=strrchr(rec_result,'=');
										if(tmp!=NULL)
											strcpy(res,tmp+1);
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
									exit(0);
								}
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
							if((fpid=fork())==0)
							{
								if(playback(play_file)==0)
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
							strcat(err_msg,"filename is null");
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
							if((fpid=fork())==0)
							{
								if(play(audio_string,playback_file)==0)
								{
									send_msg(msgid,TYPE_AUDIO_TO_MAIN,AUDIO_TO_MAIN,ACK_PLAYBACK_XF_STOP);
									if(playback(playback_file)==0)
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
	waitpid(fpid, &status, 0);
	return 0;
}

