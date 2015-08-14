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
#include <sys/stat.h>
#include <fnmatch.h> 
#include <signal.h>
#include <sys/ipc.h>  
#include <sys/shm.h>  
#include "alsa/qisr.h"
#include "alsa/msp_cmn.h"
#include "alsa/msp_errors.h"
#include "alsa/qtts.h"
typedef int SR_DWORD;
typedef short int SR_WORD ;
#define PERM S_IRUSR|S_IWUSR  
#define TYPE_MAIN_TO_AUDIO 				0x04
#define TYPE_AUDIO_TO_MAIN 				0x03
#define TYPE_LOCAL_STOP_RECORD			0x1f
#define TYPE_LOCAL_STOP_PLAYBACK		0x1e

#define MAIN_TO_AUDIO 					0x0d
#define AUDIO_TO_MAIN 					0x0e

#define MUSIC_PLAY_START				0x01
#define MUSIC_PLAY_STOP					0x02
#define MUSIC_RECORD_START				0x04
#define MUSIC_RECORD_STOP				0x08
#define MUSIC_XF_START					0x10
#define MUSIC_XF_STOP					0x20

#define CMD_01_MUSIC_PLAY					"b;01;?;"
#define CMD_02_MUSIC_STOP					"b;02;?;"
#define CMD_50_GET_VOL						"50"
#define CMD_51_SET_VOL						"51;"
#define CMD_21_RING_NOW_ARM					"21;"
#define CMD_START_RECORD					"c;1"
#define CMD_RCV_GRAMMAR_ID					"c;2;"

#define LOG_PREFX						"[AudioSubSystem]:"
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
struct msg_st  
{  
	long int msg_type; 
	int id;
	char text[512];  
}; 

