#include "mediastreamer2/msfilter.h"
#include "mediastreamer2/mssndcard.h"
#include "mediastreamer2/msticker.h"
#include "mediastreamer2/msfileplayer.h"
#include "mediastreamer2/msfilerec.h"
#include <ortp/ortp.h>
#include <unistd.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>  
#include <errno.h>  
#include <sys/msg.h>  
#include <signal.h>
#include "alsa/qisr.h"
#include "alsa/msp_cmn.h"
#include "alsa/msp_errors.h"



#define MAX_KEYWORD_LEN 4096
char GrammarID[128];
struct msg_st  
{  
	long int msg_type;  
	char text[512];  
}; 
void rec(char *filename)
{
	MSFilter *f1_r,*f1_w,*record;
	MSSndCard *card_capture1;
	MSSndCard *card_playback1;
	MSTicker *ticker1;
	struct msg_st data_w;
	long int msgtype = 0;
	char *capt_card1=NULL,*play_card1=NULL;
	int rate = 8000;
	int i;
	const char *alsadev=NULL;
	int  msgid = msgget((key_t)1234, 0666 | IPC_CREAT);  
	if(msgid == -1)  
	{  
		fprintf(stderr, "msgget failed with error: %d\n", errno);  
		exit(-1);  
	}  

	ortp_init();
	ortp_set_log_level_mask(/*ORTP_MESSAGE|ORTP_WARNING|*/ORTP_ERROR|ORTP_FATAL);
	ms_init();

	card_capture1 = ms_snd_card_manager_get_default_capture_card(ms_snd_card_manager_get());
	card_playback1 = ms_snd_card_manager_get_default_playback_card(ms_snd_card_manager_get());
	if (card_playback1==NULL || card_capture1==NULL)
	{
		if(card_playback1==NULL)
			ms_error("No card. card_playback1 %s",capt_card1);
		if(card_capture1==NULL)
			ms_error("No card. card_capture1 %s",capt_card1);
		//return -1;
	}
	else
	{
		ms_warning("card_playback1 %s|%s|%s|%d|%d|%d",card_playback1->name,card_playback1->id,card_playback1->desc->driver_type,
				card_playback1->capabilities,card_playback1->latency,card_playback1->preferred_sample_rate);
	}
	record=ms_filter_new(MS_FILE_REC_ID);
	ms_filter_call_method(record,MS_FILE_REC_OPEN,(void*)filename);
	f1_r=ms_snd_card_create_reader(card_capture1);
	//f1_w=ms_snd_card_create_writer(card_playback1);
	if(f1_r!=NULL&&record!=NULL)
	{
		ms_filter_call_method (f1_r, MS_FILTER_SET_SAMPLE_RATE,	&rate);
		//ms_filter_call_method (f1_w, MS_FILTER_SET_SAMPLE_RATE,	&rate);
		ms_filter_call_method(record,MS_FILE_REC_START,NULL);
		ticker1=ms_ticker_new();
		ms_ticker_set_name(ticker1,"card1 to card2");
		ms_filter_link(f1_r,0,record,0);	 	
		ms_ticker_attach(ticker1,f1_r);
		ms_sleep(30);
		ms_filter_call_method(record,MS_FILE_REC_STOP,NULL);
		ms_filter_call_method(record,MS_FILE_REC_CLOSE,NULL);
		if(ticker1) ms_ticker_detach(ticker1,f1_r);
		if(f1_r&&record) ms_filter_unlink(f1_r,0,record,0);
		if(ticker1) ms_ticker_destroy(ticker1);
		if(f1_r) ms_filter_destroy(f1_r);
		if(record) ms_filter_destroy(record);
		data_w.msg_type = 1;    //注意2  
		strcpy(data_w.text, filename);  
		//向队列发送数据  
		if(msgsnd(msgid, (void*)&data_w, 512, IPC_NOWAIT) == -1)  
		{  
			fprintf(stderr, "msgsnd failed\n");  
			exit(1);  
		}  
	}	
}
int run_asr(const char* asrfile ,  const char* param)
{
	char rec_result[1024*4] = {0};
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
	sessionID = QISRSessionBegin(GrammarID, param, &ret); //asr
	if(ret !=0)
	{
		printf("QISRSessionBegin Failed,ret=%d\n",ret);
	}

	f_pcm = fopen(asrfile, "rb");
	if (NULL != f_pcm) {
		fseek(f_pcm, 0, SEEK_END);
		pcmSize = ftell(f_pcm);
		fseek(f_pcm, 0, SEEK_SET);
		pPCM = (char *)malloc(pcmSize);
		fread((void *)pPCM, pcmSize, 1, f_pcm);
		fclose(f_pcm);
		f_pcm = NULL;
	}
	while (1) {
		unsigned int len = 6400;
              unsigned int audio_len = 6400;
		if (pcmSize < 12800) {
			len = pcmSize;
			lastAudio = 1;
		}
		audStat = 2;
		if (pcmCount == 0)
			audStat = 1;
		if (0) {
			if (audStat == 1)
				audStat = 5;
			else
				audStat = 4;
		}
		if (len<=0)
		{
			break;
		}
		printf("\ncsid=%s,count=%d,aus=%d,",sessionID,pcmCount/audio_len,audStat);
		ret = QISRAudioWrite(sessionID, (const void *)&pPCM[pcmCount], len, audStat, &epStatus, &recStatus);
		printf("eps=%d,rss=%d,ret=%d",epStatus,recStatus,ret);
		if (ret != 0)
			break;
		pcmCount += (long)len;
		pcmSize -= (long)len;
		if (recStatus == 0) {
			const char *rslt = QISRGetResult(sessionID, &recStatus, 0, &ret);
			if (ret !=0)
			{
				printf("QISRGetResult Failed,ret=%d\n",ret);
				break;
			}
			if (NULL != rslt)
				strcat(rec_result,rslt);
		}
		if (epStatus == MSP_EP_AFTER_SPEECH)
			break;
		usleep(150000);
	}
	ret=QISRAudioWrite(sessionID, (const void *)NULL, 0, 4, &epStatus, &recStatus);
	if (ret !=0)
	{
		printf("QISRAudioWrite Failed,ret=%d\n",ret);
	}
	free(pPCM);
	pPCM = NULL;
	while (recStatus != 5 && ret == 0) {
		const char *rslt = QISRGetResult(sessionID, &recStatus, 0, &ret);
		if (NULL != rslt)
		{
			strcat(rec_result,rslt);
		}
		usleep(150000);
	}
	ret=QISRSessionEnd(sessionID, NULL);
	if(ret !=MSP_SUCCESS)
	{
		printf("QISRSessionEnd Failed, ret=%d\n",ret);
	}
	printf("\n=============================================================\n");
	printf("The result is: %s\n",rec_result);
	printf("=============================================================\n");
	usleep(100000);
}
int get_from_server(char *file)
{
	const char* login_config = "appid = 55801297,work_dir =   .  ";
	const char* param = "rst=plain,rse=utf8,sub=asr,aue=speex-wb,auf=audio/L16;rate=16000,ent=sms16k";    //注意sub=asr,16k音频aue=speex-wb，8k音频识别aue=speex，
	int ret = 0 ;
	char key = 0 ;
	int grammar_flag = 0;//0:不上传词表；1：上传词表
	ret = MSPLogin(NULL, NULL, login_config);
	if ( ret != MSP_SUCCESS )
	{
		printf("MSPLogin failed , Error code %d.\n",ret);
		return 0 ;
	}
	
	strcpy(GrammarID, "e7eb1a443ee143d5e7ac52cb794810fe");
 	ret = run_asr(file, param);
	if(ret != MSP_SUCCESS)
	{
		printf("run_asr with errorCode: %d \n", ret);
		return 0;
	}
	MSPLogout();
}
int main(int argc, char *argv[])
{

	pid_t fpid;	
	int msgid = -1;  
	//signal(SIGINT,stop);
	struct msg_st data_r;
	long int msgtype = 1;

	//建立消息队列  
	msgid = msgget((key_t)1234, 0666 | IPC_CREAT);  
	if(msgid == -1)  
	{  
		fprintf(stderr, "msgget failed with error: %d\n", errno);  
		exit(-1);  
	}  
	fpid=fork();
	if(fpid<0)
		printf("fork failed\n");
	else if(fpid==0)
	{
		//child process
		rec((char *)argv[1]);
	}
	else
	{
		//father process
		msgrcv(msgid, (void*)&data_r, 512, msgtype, 0);
		printf("data_r id %d,text %s\n",data_r.msg_type,data_r.text);
		get_from_server(data_r.text);
	}
	return 0;
}
