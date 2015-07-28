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
#include "alsa/qtts.h"


typedef int SR_DWORD;
typedef short int SR_WORD ;

//音频头部格式
struct wave_pcm_hdr
{
	char            riff[4];                        // = "RIFF"
	SR_DWORD        size_8;                         // = FileSize - 8
	char            wave[4];                        // = "WAVE"
	char            fmt[4];                         // = "fmt "
	SR_DWORD        dwFmtSize;                      // = 下一个结构体的大小 : 16

	SR_WORD         format_tag;              // = PCM : 1
	SR_WORD         channels;                       // = 通道数 : 1
	SR_DWORD        samples_per_sec;        // = 采样率 : 8000 | 6000 | 11025 | 16000
	SR_DWORD        avg_bytes_per_sec;      // = 每秒字节数 : dwSamplesPerSec * wBitsPerSample / 8
	SR_WORD         block_align;            // = 每采样点字节数 : wBitsPerSample / 8
	SR_WORD         bits_per_sample;         // = 量化比特数: 8 | 16

	char            data[4];                        // = "data";
	SR_DWORD        data_size;                // = 纯数据长度 : FileSize - 44 
} ;

//默认音频头部数据
struct wave_pcm_hdr default_pcmwavhdr = 
{
	{ 'R', 'I', 'F', 'F' },
	0,
	{'W', 'A', 'V', 'E'},
	{'f', 'm', 't', ' '},
	16,
	1,
	1,
	16000,
	32000,
	2,
	16,
	{'d', 'a', 't', 'a'},
	0  
};

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
	int nchan=2;
	int i;
	const char *alsadev=NULL;
	int  msgid = msgget((key_t)1234, 0666 | IPC_CREAT);  
	if(msgid == -1)  
	{  
		fprintf(stderr, "msgget failed with error: %d\n", errno);  
		exit(-1);  
	}  

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
		//return -1;
	}
	else
	{
		ms_warning("card_playback1 %s|%s|%s|%d|%d|%d",card_playback1->name,card_playback1->id,card_playback1->desc->driver_type,
				card_playback1->capabilities,card_playback1->latency,card_playback1->preferred_sample_rate);
	}
	record=ms_filter_new(MS_FILE_REC_ID);
	if(ms_filter_call_method(record,MS_FILE_REC_OPEN,(void*)filename)!=0)
		printf("record open file %s failed\n",filename);
	f1_r=ms_snd_card_create_reader(card_capture1);
	//f1_w=ms_snd_card_create_writer(card_playback1);
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
		ms_sleep(2);
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

	printf("\nbegin to synth...\n");
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
              printf("\nget audio...\n");
              usleep(150000);//建议可以sleep下，因为只有云端有音频合成数据，audioget都能获取到音频。
		if (synth_status == 2 || ret != 0) 
		break;
	}

	//修正pcm文件头数据的大小
	pcmwavhdr.size_8 += pcmwavhdr.data_size + 36;

	//将修正过的数据写回文件头部
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
       printf("\nTTS end...\n");
	return ret;
}

void play(const char *string,const char *filename)
{
	///APPID请勿随意改动
	const char* login_configs = " appid = 55801297, work_dir =	 .	";
	const char* text  = "科大讯飞作为中国最大的智能语音技术提供商，在智能语音技术领域有着长期的研究积累，并在中文语音合成、语音识别、口语评测等多项技术上拥有国际领先的成果。";
	//const char*  filename = "text_to_speech_test.wav";//因加wav音频头，生成的是wav音频，不加头，是pcm音频.
	const char* param = "vcn=xiaoyan,aue = speex-wb,auf=audio/L16;rate=16000,spd = 5,vol = 5,tte = utf8";//8k音频合成参数：aue=speex,auf=audio/L16;rate=8000,其他参数意义参考参数列表
	int ret = 0;
	char key = 0;

	//用户登录
	ret = MSPLogin(NULL, NULL, login_configs);
	if ( ret != MSP_SUCCESS )
	{
		printf("MSPLogin failed , Error code %d.\n",ret);
	}
	//音频合成
	ret = text_to_speech(text,filename,param);
	if ( ret != MSP_SUCCESS )
	{
		printf("text_to_speech: failed , Error code %d.\n",ret);
	}
	//退出登录
	MSPLogout();
}
char *run_asr(const char* asrfile ,  const char* param)
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
	return rec_result;
}
char *get_from_server(char *file)
{
	const char* login_config = "appid = 55801297,work_dir =   .  ";
	const char* param = "rst=plain,rse=utf8,sub=asr,aue=speex-wb,auf=audio/L16;rate=16000,ent=sms16k";    //注意sub=asr,16k音频aue=speex-wb，8k音频识别aue=speex，
	int ret = 0 ;
	char *result;
	char key = 0 ;
	int grammar_flag = 0;//0:不上传词表；1：上传词表
	ret = MSPLogin(NULL, NULL, login_config);
	if ( ret != MSP_SUCCESS )
	{
		printf("MSPLogin failed , Error code %d.\n",ret);
		return 0 ;
	}
	
	strcpy(GrammarID, "e7eb1a443ee143d5e7ac52cb794810fe");
 	result = run_asr(file, param);
	if(result == NULL)
	{
		printf("run_asr with errorCode: %d \n", ret);
		MSPLogout();
		return 0;
	}
	MSPLogout();
	return result;
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
		play("tech","/tmp/3.wav");
	}
	return 0;
}
