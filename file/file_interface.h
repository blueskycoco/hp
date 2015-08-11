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

#define TYPE_MAIN_TO_FILE 				0x08
#define TYPE_FILE_TO_MAIN 				0x07
#define MAIN_TO_FILE 					0x0f
#define FILE_TO_MAIN 					0x10
#define LOG_PREFX 						"[FileSubSystem]:"
#define CMD_00_MUSIC_FIND				"r;00;?;01"
#define CMD_03_MUSIC_CHOOSE				"w;03;?;04;"
#define CMD_04_MUSIC_LIKE				"w;04;?;02;"
#define CMD_05_MUSIC_UNLIKE				"w;05;?;02;"
#define CMD_07_LIGHT_MANUAL				"w;07;?;00;"
#define CMD_07_LIGHT_MANUAL_2			"a;07;?;05;"
#define CMD_08_LIGHT_VOICE				"r;08;a;"
#define CMD_08_LIGHT_VOICE_2			"a;08;a;05;"
#define CMD_10_LIGHT_LIGHT_OFF			"r;10;?;07;"
#define CMD_10_LIGHT_LIGHT_OFF_2		"a;10;?;05;"
#define CMD_11_LIGHT_MODE_FIND			"r;11;?;00"
#define CMD_12_LIGHT_MODE_CHECKs		"r;12;?;00;"
#define CMD_13_LIGHT_MODE_CHOOSE		"r;13;?;00"
#define CMD_13_LIGHT_MODE_CHOOSE_2		"a;13;?;05;"
#define CMD_14_LIGHT_MODE				"r;14;?;00;"
#define CMD_14_LIGHT_MODE_2				"w;14;?;"
#define CMD_15_SAVE_MODE_ON				"r;15;?;00;"
#define CMD_15_SAVE_MODE_ON_2			"a;15;?;05;"
#define CMD_16_SAVE_MODE_OFF			"r;16;?;00;"
#define CMD_16_SAVE_MODE_OFF_2			"a;16;?;05;"
#define CMD_17_RING_SET					"w;17;?;04;?"//?
#define CMD_17_RING_SET_2				"w;17;?;"//?
#define CMD_18_RING_ON					"w;18;?;04;"
#define CMD_19_RING_OFF					"w;19;?;04;"
#define CMD_20_RING_DELAY				"a;20;m;05;"
#define CMD_21_RING_NOW_ARM				"r;21;00;"
#define CMD_21_RING_NOW_ARM_2			"r;21;04;"
#define CMD_21_RING_NOW_ARM_3			"a;21;05;"
#define CMD_23_TIME_UPDATE_ARM			"a;23;05;"
#define CMD_24_ADD_REMINDER				"w;24;?;03;?"//?
#define CMD_24_ADD_REMINDER_2			"w;24;?;03;"//?
#define CMD_25_REMINDER_ON_ARM			"r;25;03;"
#define FILE_PATH_NAME "/home/file/"

#define FILE_MUSIC_NAME_LINE	1
#define FILE_MUSIC_SINGER_LINE	2
#define FILE_MUSIC_LIKE_LINE	4


struct msg_st
{  
	long int msg_type; 
	int id;
	char text[512];  
}; 

