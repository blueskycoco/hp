#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "alsa/qisr.h"
#include "alsa/msp_cmn.h"
#include "alsa/msp_errors.h"
#define MAX_KEYWORD_LEN 4096

int main(int argc, char *argv[])
{
	int ret = MSP_SUCCESS;
	const char * sessionID = NULL;
	char UserData[MAX_KEYWORD_LEN];
	unsigned int len = 0;
	const char* testID = NULL;
	char GrammarID[128];
	FILE *fp = NULL;
	memset(UserData, 0, MAX_KEYWORD_LEN);
	if(argv[1]==NULL)
	{
		printf("please set argv[1] to your keywords file\n");
		return 0;
	}
	fp = fopen(argv[1], "rb");
	if (fp == NULL)
	{
		printf("keyword file cannot open\n");
		return -1;
	}
	len = (unsigned int)fread(UserData, 1, MAX_KEYWORD_LEN, fp);
	UserData[len] = 0;
	fclose(fp);
       testID = MSPUploadData("userword", UserData, len, "dtt = userword, sub = asr", &ret);
	if(ret != MSP_SUCCESS)
	{
		printf("UploadData with errorCode: %d \n", ret);
		return ret;
	}
	memcpy((void*)GrammarID, testID, strlen(testID));
	printf("*************************************************************\n");
	printf("GrammarID: \"%s\" \n", GrammarID);
	printf("*************************************************************\n");
	return 0;
}

