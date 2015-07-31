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
		printf("QISRSessionBegin Failed,ret=%d\n",ret);
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
					printf("QISRGetResult Failed,ret=%d\n",ret);
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
			printf("QISRAudioWrite Failed,ret=%d\n",ret);
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
			printf("QISRSessionEnd Failed, ret=%d\n",ret);
			result=0;
		}
	}
	return result;
}

int get_from_server(char *file,char **rec_result)
{
	const char* login_config = "appid = 55801297,work_dir =   .  ";
	const char* param = "rst=plain,rse=utf8,sub=asr,aue=speex-wb,auf=audio/L16;rate=16000,ent=sms16k";    //注意sub=asr,16k音频aue=speex-wb，8k音频识别aue=speex，
	int ret = 0 ;
	char *result;
	ret = MSPLogin(NULL, NULL, login_config);
	if ( ret != MSP_SUCCESS )
	{
		printf("MSPLogin failed , Error code %d.\n",ret);
		return 0 ;
	}
	else
	{
		strcpy(GrammarID, "e7eb1a443ee143d5e7ac52cb794810fe");
		result = run_asr(file, param, rec_result);
		if(result == 0)
		{
			printf("run_asr with errorCode: %d \n", ret);
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
			ms_error("No card. card_playback1 %s",capt_card1);
		if(card_capture1==NULL)
			ms_error("No card. card_capture1 %s",capt_card1);
		return ;
	}
	else
	{
		ms_warning("card_playback1 %s|%s|%s|%d|%d|%d",card_playback1->name,card_playback1->id,card_playback1->desc->driver_type,
				card_playback1->capabilities,card_playback1->latency,card_playback1->preferred_sample_rate);
	}
	record=ms_filter_new(MS_FILE_REC_ID);
	if(ms_filter_call_method(record,MS_FILE_REC_OPEN,(void*)filename)!=0)
	{
		printf("record open file %s failed\n",filename);
		return ;
	}
	f1_r=ms_snd_card_create_reader(card_capture1);
	if(f1_r!=NULL&&record!=NULL)
	{
		if(ms_filter_call_method(f1_r, MS_FILTER_SET_SAMPLE_RATE,	&rate)!=0)
			printf("set sample rate f1_r failed\n");
		if(ms_filter_call_method(record, MS_FILTER_SET_SAMPLE_RATE,&rate)!=0)
			printf("set sample rate record failed\n");
		if(ms_filter_call_method(f1_r, MS_FILTER_SET_NCHANNELS,	&nchan)!=0)
			printf("set nchan f1_r failed\n");
		if(ms_filter_call_method(record, MS_FILTER_SET_NCHANNELS,&nchan)!=0)
			printf("set nchan record failed\n");
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
		/*take it out*/
		data.msg_type = TYPE_AUDIO_TO_MAIN;
		data.id=AUDIO_TO_MAIN;
		char *tmp=strrchr(get_from_server(filename),'=');
		if(tmp==NULL)
			strcpy(data_w.text,"can not find result from server");
		else
			strcpy(data_w.text, tmp+1);  
		if(msgsnd(msgid, (void*)&data_w, 512, IPC_NOWAIT) == -1)  
		{  
			fprintf(stderr, "msgsnd failed\n");  
			//exit(1);  
		}  
	}	
}
int main(int argc, char *argv[])
{

	pid_t fpid;	
	int msgid = -1;
	struct msg_st *data;

	msgid = msgget((key_t)1234, 0666 | IPC_CREAT);  
	if(msgid == -1)  
	{  
		fprintf(stderr, "msgget failed with error: %d\n", errno);  
		exit(-1);  
	}
	while(1)
	{
		printf("waiting audio capt cmd...\n");
		data=(struct msg_st *)malloc(sizeof(struct msg_st));
		msgrcv(msgid, (void*)data, sizeof(struct msg_st)-sizeof(long int), TYPE_MAIN_TO_AUDIO , 0);
		printf("msgtype %d ,data id %d,text xxx %c;yyy %c\n",data->msg_type,data->id,data->text[XXX_OFS],data->text[YYY_OFS]);
		if(data->id==MAIN_TO_AUDIO)
		{
			switch (data->text[XXX_OFS])
			{
				case PROC_RECORD://start rec
				{
					switch (data->text[YYY_OFS])
					{
						case CMD_START_RECORD:
						{
							if(fork()==0)
							{
								rec(msgid,"/tmp/rec.avi");
								exec(0);
							}
						}
						break;
						case CMD_STOP_RECORD:
						{
							data=(struct msg_st *)malloc(sizeof(struct msg_st));
							data->msg_type = TYPE_LOCAL_STOP_RECORD;
							printf("stop record\n");
							if(msgsnd(msgid, (void*)data, sizeof(struct msg_st)-sizeof(long int), IPC_NOWAIT) == -1)  
							{  
								fprintf(stderr, "msgsnd failed %s\n",strerror(errno));
								system("ipcs -q");
							}
							data=(struct msg_st *)malloc(sizeof(struct msg_st));
							data->msg_type = TYPE_AUDIO_TO_MAIN;
							data->id=AUDIO_TO_MAIN;
							printf("stop record\n");
							if(msgsnd(msgid, (void*)data, sizeof(struct msg_st)-sizeof(long int), IPC_NOWAIT) == -1)  
							{  
								fprintf(stderr, "msgsnd failed %s\n",strerror(errno));
								system("ipcs -q");
							}
						}
						break;
						case CMD_CHECK_RECORD:
						{
						
						}
						break;
						default:
							break;
					}
				}
				break;
				case PROC_PLAYBACK://start transfer string and play
				{
					play(data_r.text,"/tmp/3.wav");
					playback("/tmp/3.wav");
				}
				break;
				case XF_PLAYBACK:
				{
					
				}
				break;
				default:
					break;
			}
		}
	}



	
	fpid=fork();
	if(fpid<0)
		printf("fork failed\n");
	else if(fpid==0)
	{
		//child process,capture audio when main process send start cmd, finish when receive stop cmd
		//rec((char *)argv[1]);		
		long int msgtype = 8;//MAIN_TO_AUDIO 8
		struct msg_st data_r;
		while(1)
		{
			printf("waiting audio capt cmd...\n");
			msgrcv(msgid, (void*)&data_r, sizeof(struct msg_st)-sizeof(long int), msgtype, 0);
			printf("data_r id %d,text %d\n",data_r.msg_type,data_r.id);
			switch (data_r.id)
			{
				case 0://start rec
				{
					rec("/tmp/rec.avi");
				}
				break;
				case 2://start transfer string and play
				{
					play(data_r.text,"/tmp/3.wav");
					playback("/tmp/3.wav");
				}
				break;
				default:
					break;
			}
		}
	}
	else
	{
		//father process
		long int msgtype = 8;//MAIN_TO_AUDIO 8
		struct msg_st data_r;
		int status;
		while(1)
		{
			ms_sleep(2);
			data_r.msg_type = 8;
			data_r.id=0;
			printf("start record\n");
			if(msgsnd(msgid, (void*)&data_r, sizeof(struct msg_st)-sizeof(long int), IPC_NOWAIT) == -1)  
			{  
				fprintf(stderr, "msgsnd failed %s msgid %d\n",strerror(errno),msgid);  
				exit(1);  
			}  
			ms_sleep(3);
			data_r.msg_type = 8;
			data_r.id=1;  
			printf("stop record\n");
			if(msgsnd(msgid, (void*)&data_r, sizeof(struct msg_st)-sizeof(long int), IPC_NOWAIT) == -1)  
			{  
				fprintf(stderr, "msgsnd failed %s\n",strerror(errno));  
				exit(1);  
			} 
			printf("waiting message 7\n"); 
			msgtype=7;
			msgrcv(msgid, (void*)&data_r, sizeof(struct msg_st)-sizeof(long int), msgtype, 0);

			printf("Get Result %s\n",data_r.text);
			
			data_r.msg_type = 8;    //注意2
			data_r.id=2;  
			printf("start transfer and playback\n");
			if(msgsnd(msgid, (void*)&data_r, sizeof(struct msg_st)-sizeof(long int), IPC_NOWAIT) == -1)  
			{  
				fprintf(stderr, "msgsnd failed %s\n",strerror(errno));  
				exit(1);  
			}
			system("rm /tmp/rec.avi");
			//system("ipcs -q");
		}
		waitpid(fpid, &status, 0);
	}
	return 0;
}

