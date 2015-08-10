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
#include <fnmatch.h>

typedef int SR_DWORD;
typedef short int SR_WORD ;
#define TYPE_MAIN_TO_AUDIO 				0x08
#define TYPE_AUDIO_TO_MAIN 				0x07

#define CMD_00_MUSIC_FIND				"r;00;?;01"
#define CMD_03_MUSIC_CHOOSE				"w;03;?;04"
#define CMD_04_MUSIC_LIKE				"w;04;?;02"
#define CMD_04_MUSIC_UNLIKE				"w;05;?;02"
#define CMD_07_LIGHT_MANUAL				"w;07;?;00"
#define CMD_07_LIGHT_MANUAL_2			"a;07;?;05"
#define CMD_08_LIGHT_VOICE				"r;08;a;"
#define CMD_08_LIGHT_VOICE_2			"a;08;a;05"
#define CMD_10_LIGHT_LIGHT_OFF			"r;10;?;07"
#define CMD_10_LIGHT_LIGHT_OFF_2		"a;10;?;05"


#define MAIN_TO_AUDIO 					0x0f
#define AUDIO_TO_MAIN 					0x10
struct msg_st  
{  
	long int msg_type; 
	int id;
	char text[512];  
}; 

