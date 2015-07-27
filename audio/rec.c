#include "mediastreamer2/msfilter.h"
#include "mediastreamer2/mssndcard.h"
#include "mediastreamer2/msticker.h"
#include <ortp/ortp.h>
#include <unistd.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>  
#include <errno.h>  
#include <sys/msg.h>  
#include <signal.h>
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
	 msgid = msgget((key_t)1234, 0666 | IPC_CREAT);  
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
		return -1;
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
int main(int argc, char *argv[])
{
	
	pid_t fpid;	
	int msgid = -1;  
	signal(SIGINT,stop);
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
		printf("fork failed\n"");
	else if(fpid==0)
	{
			//child process
			rec((char *)arv[1]);
	}
	else
	{
		//father process
		msgrcv(msgid, (void*)&data_r, 512, msgtype, 0);
		printf("data_r id %d,text %s\n",data_r.msg_type,data_r.text);
	}
	return 0;
}