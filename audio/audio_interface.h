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
#define TYPE_MAIN_TO_AUDIO 				0x04
#define TYPE_AUDIO_TO_MAIN 				0x03
#define TYPE_LOCAL_STOP_RECORD			0x1f

#define MAIN_TO_AUDIO 					0x0d
#define AUDIO_TO_MAIN 					0x0e
#define PROC_RECORD 					'c'
#define PROC_PLAYBACK 					'b'
#define XF_PLAYBACK 					'd'

#define STATUS_RECORD 					's'
#define STATUS_PLAYBACK 				'b'
#define STATUS_XF_PLAYBACK 				'd'
#define RESULT_XF_RECORD 				'r'

#define CMD_START_RECORD 				'1'
#define CMD_STOP_RECORD 				'0'
#define CMD_CHECK_RECORD 				'2'
#define STATUS_START_RECORD 			'1'
#define STATUS_STOP_RECORD 				'0'
#define STATUS_PLAYBACK_LOCAL_BEGIN 	'1'
#define STATUS_PLAYBACK_LOCAL_DONE 		'2'
#define STATUS_PLAYBACK_LOCAL_FAILED 	'0'
#define STATUS_XF_PLAYBACK_BEGIN 		'1'
#define STATUS_XF_PLAYBACK_DONE 		'2'
#define STATUS_XF_PLAYBACK_UNDO 		'0'
#define STATUS_XF_PLAYBACK_OK 			'3'
#define STATUS_XF_PLAYBACK_FAILED 		'4'

#define CMD_START_RECORD				"c;1"
#define CMD_STOP_RECORD					"c;0"
#define CMD_CHECK_RECORD				"c;2"
#define CMD_PLAYBACK_LOCAL_FILE			"b;"
#define CMD_PLAYBACK_LOCAL_CHECK		"b;"
#define CMD_XF_PLAYBACK_START			"d;"

#define ACK_RECORD_STOP 				"s;0"
#define ACK_RECORD_START 				"s;1"
#define ACK_XF_RECORD_RESULT			"r;"
#define ACK_PLAYBACK_LOCAL_START 		"b;1"
#define ACK_PLAYBACK_LOCAL_STOP 		"b;2"
#define ACK_PLAYBACK_LOCAL_FAILED 		"b;0"
#define ACK_PLAYBACK_XF_START 			"d;1"
#define ACK_PLAYBACK_XF_STOP 			"d;2"
#define ACK_PLAYBACK_XF_UNDO 			"d;0"
#define ACK_PLAYBACK_XF_OK 				"d;3"
#define ACK_PLAYBACK_XF_FAILED 			"d;4"
#define LOG_PREFX						"[AudioSubSystem]:"
#define XXX_OFS 0
#define YYY_OFS 2
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

