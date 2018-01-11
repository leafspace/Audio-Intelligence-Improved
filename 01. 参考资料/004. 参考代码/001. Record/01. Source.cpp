#define _CRT_SECURE_NO_WARNINGS
/*
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <errno.h>
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "mmsystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#pragma comment(lib, "winmm.lib")//必须包含这个lib

#define BUFFSIZE 1024 * 1024 //环形缓冲区的大小，你可以定义大一些

//WAV音频文件头


#define LENGTH    10   //录音时间,秒
#define RATE    16000 //采样频率
#define SIZE    16   //量化位数
#define CHANNELS 1   //声道数目
#define RSIZE    8    //buf的大小，
#define	BUFFER_SIZE	4096
#define FRAME_LEN	640 
#define HINTS_SIZE  100
//#define min(x, y) ((x) < (y) ? (x) : (y))//这个函数在VS中有一个同样的宏，所以注释掉了。
typedef int SR_DWORD;
typedef short int SR_WORD ;
typedef long long SR_LWORD;
//wav的音频头
typedef struct wave_pcm_hdr
 {
                char                riff[4];               // = "RIFF"
                SR_DWORD			size_8;              // = FileSize - 8
                char                wave[4];            // = "WAVE"
                char                fmt[4];          // = "fmt "
                SR_DWORD			dwFmtSize;        // = 下一个结构体的大小 : 16

                SR_WORD				format_tag;        // = PCM : 1
                SR_WORD				channels;               // = 通道数 : 1
                SR_DWORD			samples_per_sec;        // = 采样率
                SR_DWORD			avg_bytes_per_sec;        // = 每秒字节数
                SR_WORD				block_align;              // = 每采样点字节数
                SR_WORD				bits_per_sample;        // = 量化比特数: 8 | 16

                char                data[4];                    // = "data";
                SR_DWORD			data_size;        // = 纯数据长度 : FileSize - 44 
} wave_header;

//默认音频头部数据
struct wave_pcm_hdr wav_hdr= 
{
        { 'R', 'I', 'F', 'F' },
        LENGTH*RATE*CHANNELS*SIZE/8+36,
        {'W', 'A', 'V', 'E'},
        {'f', 'm', 't', ' '},
        16,
        1,                
        CHANNELS,                                
        RATE,                                
        RATE*CHANNELS*SIZE/8,                
        CHANNELS*SIZE/8,                                                                
        SIZE,                                        
        {'d', 'a', 't', 'a'},
        LENGTH*RATE*CHANNELS*SIZE/8
};

//环形缓冲区的的数据结构
struct cycle_buffer {  
    char *buf;  
    unsigned int size;  
    unsigned int in;  
    unsigned int out;  
};  

static struct cycle_buffer *fifo = NULL;//定义全局FIFO
FILE *fp; 
CRITICAL_SECTION cs;

//初始化环形缓冲区
static int init_cycle_buffer(void)  
{
    int size = BUFFSIZE, ret;  

    ret = size & (size - 1);  
    if (ret)  
        return ret;
    fifo = (struct cycle_buffer *) malloc(sizeof(struct cycle_buffer));  
    if (!fifo)  
        return -1;  

    memset(fifo, 0, sizeof(struct cycle_buffer));  
    fifo->size = size;  
    fifo->in = fifo->out = 0;  
    fifo->buf = (char *) malloc(size);  
    if (!fifo->buf)
        free(fifo);
    else
        memset(fifo->buf, 0, size);  
    return 0;  
}  

unsigned int fifo_get(char *buf, unsigned int len)  //从环形缓冲区中取数据
{  
    unsigned int l;  
    len = min(len, fifo->in - fifo->out);  
    l = min(len, fifo->size - (fifo->out & (fifo->size - 1)));  
    memcpy(buf, fifo->buf + (fifo->out & (fifo->size - 1)), l);  
    memcpy(buf + l, fifo->buf, len - l);  
    fifo->out += len;  
    return len;  
}  

unsigned int fifo_put(char *buf, unsigned int len) //将数据放入环形缓冲区
{  
    unsigned int l;  
    len = min(len, fifo->size - fifo->in + fifo->out);  
    l = min(len, fifo->size - (fifo->in & (fifo->size - 1)));  
    memcpy(fifo->buf + (fifo->in & (fifo->size - 1)), buf, l);  
    memcpy(fifo->buf, buf + l, len - l);  
    fifo->in += len;  
    return len;  
}  

void WaveInitFormat(LPWAVEFORMATEX m_WaveFormat, WORD nCh,DWORD nSampleRate,WORD BitsPerSample)//初始化音频格式
{
	m_WaveFormat->wFormatTag = WAVE_FORMAT_PCM;
	m_WaveFormat->nChannels = nCh;
	m_WaveFormat->nSamplesPerSec = nSampleRate;
	m_WaveFormat->nAvgBytesPerSec = nSampleRate * nCh * BitsPerSample/8;
	m_WaveFormat->nBlockAlign = m_WaveFormat->nChannels * BitsPerSample/8;
	m_WaveFormat->wBitsPerSample = BitsPerSample;
	m_WaveFormat->cbSize = 0;
}

DWORD CALLBACK MicCallback(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)//回调函数当数据缓冲区慢的时候就会触发，回调函数，执行下面的RecordWave函数之后相当于创建了一个线程
{
	int len=0;
	switch(uMsg)
	{
		case WIM_OPEN://打开设备时这个分支会执行。
			printf("\n设备已经打开...\n");
			break;
		case WIM_DATA://当缓冲区满的时候这个分支会执行，不要再这个分支中出现阻塞语句，会丢数据	，waveform audio本身没有缓冲机制。		
			printf("\n缓冲区%d存满...\n",((LPWAVEHDR)dwParam1)->dwUser);
			waveInAddBuffer(hwavein, (LPWAVEHDR)dwParam1, sizeof(WAVEHDR));

			EnterCriticalSection(&cs); //进入临界区
			len=fifo_put(((LPWAVEHDR)dwParam1)->lpData, 10240);//将缓冲区的数据写入环形fifo			
			LeaveCriticalSection(&cs);//退出临界区
			//fwrite(((LPWAVEHDR)dwParam1)->lpData,10240, 1, fp);
			//printf("lens=%d", len);
			break;
		case WIM_CLOSE:
			printf("\n设备已经关闭...\n");
			break;
		default:
			break;
	}
	return 0;
}
void RecordWave()
{
	
	HWAVEIN phwi;
	WAVEINCAPS waveIncaps;
	int count=0;
	MMRESULT mmResult;
	count= waveInGetNumDevs();//获取系统有多少个声卡
	
	mmResult = waveInGetDevCaps(0, &waveIncaps, sizeof(WAVEINCAPS));//查看系统声卡设备参数，不用太在意这两个函数。

	printf("\ncount = %d\n", count);
	printf("\nwaveIncaps.szPname=%s\n",waveIncaps.szPname);
	
	if(MMSYSERR_NOERROR==mmResult)
	{
		WAVEFORMATEX pwfx;
		WaveInitFormat(&pwfx,1,16000,16);
		printf("\n请求打开音频输入设备");
		printf("\n采样参数：单声道 8kHz 8bit\n");
		mmResult=waveInOpen(&phwi,WAVE_MAPPER,&pwfx,(DWORD)(MicCallback),NULL,CALLBACK_FUNCTION);//打开音频设备，设置回调函数
		
		if(MMSYSERR_NOERROR==mmResult)
		{
			WAVEHDR pwh1;
			char buffer1[10240];
			WAVEHDR pwh2;
			char buffer2[10240];

			pwh1.lpData = buffer1;
			pwh1.dwBufferLength = 10240;
			pwh1.dwUser = 1;
			pwh1.dwFlags = 0;
			mmResult = waveInPrepareHeader(phwi,&pwh1,sizeof(WAVEHDR));//准备缓冲区
			printf("\n准备缓冲区1");
			
			pwh2.lpData=buffer2;
			pwh2.dwBufferLength=10240;
			pwh2.dwUser=2;
			pwh2.dwFlags=0;
			mmResult=waveInPrepareHeader(phwi,&pwh2,sizeof(WAVEHDR));//
			printf("\n准备缓冲区2\n");
			
			if(MMSYSERR_NOERROR==mmResult)
			{
				mmResult=waveInAddBuffer(phwi,&pwh1,sizeof(WAVEHDR));//添加缓冲区
				mmResult=waveInAddBuffer(phwi,&pwh2,sizeof(WAVEHDR));
				
				printf("\n将缓冲区2加入音频输入设备\n");
				if(MMSYSERR_NOERROR==mmResult)
				{
					mmResult=waveInStart(phwi);
					printf("\n请求开始录音\n");						
					
					Sleep(10000);
					waveInStop(phwi);//停止录音
					waveInReset(phwi);
					waveInClose(phwi);//关闭音频设备
					waveInUnprepareHeader(phwi,&pwh1, sizeof(WAVEHDR));//释放buffer
					waveInUnprepareHeader(phwi,&pwh2, sizeof(WAVEHDR));
					printf("stop capture!\n");
					fflush(stdout);
					
				}
			}
		}
	}
}



DWORD WINAPI forRec(void * ptr)//新建的另一个线程用于将数据写入文件
{
	char buff[10240]={0};
	int len=0;
	while(1)
	{
			//printf("sdafsadf");
		//if()
			EnterCriticalSection(&cs); //进入临界区			
			len=fifo_get(buff, 10240);//从fifo中获取数据			
			LeaveCriticalSection(&cs);//离开临界区
			printf("len=%d\n", len);
			fwrite(buff,len, 1, fp);//将音频数据写入音频文件				
			Sleep(100);
	}
}

int main(int argc, char* argv[])
{
	int len=0;
	char buff[10240]={0};
	InitializeCriticalSection(&cs);//初始化临界区
	init_cycle_buffer();//初始化缓冲区
	
	fp = fopen("test.wav","wb");//打开音频文件
	if (NULL == fp)
	{
		printf("open %s error.\n", "test.wav");
		return 1;
	}
        fwrite(&wav_hdr, sizeof(wav_hdr) ,1, fp); //添加写入wav音频头，使用采样率为16000

		HANDLE handle = CreateThread(NULL, 0, forRec, NULL, 0, NULL);
	//_beginthread(forRec, 0, NULL);//创建线程
	RecordWave();//开启录音，一旦录音数据buffer满，就会触发回调函数，所以下面要有while阻塞不然程序就会结束掉。
		
	while(1)
	{
	}
	

	return 0;
}
*/

#include <Windows.h>
#include <stdio.h>
#include "mmsystem.h"
#pragma comment(lib, "winmm.lib")
void PlayMusi();
void WaveInitFormat(LPWAVEFORMATEX m_WaveFormat, WORD nCh, DWORD nSampleRate, WORD BitsPerSample);
DWORD CALLBACK MicCallback(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
void RecordWave();

void main()
{
	//PlayMusi();
	RecordWave();
	while (1);
}
void RecordWave()
{
	int count = waveInGetNumDevs();//1
	printf("\n音频输入数量：%d\n", count);

	WAVEINCAPS waveIncaps;
	MMRESULT mmResult = waveInGetDevCaps(0, &waveIncaps, sizeof(WAVEINCAPS));//2
	printf("\n音频输入设备：%s\n", waveIncaps.szPname);

	if (MMSYSERR_NOERROR == mmResult)
	{
		HWAVEIN phwi;
		WAVEFORMATEX pwfx;
		WaveInitFormat(&pwfx, 1, 8000, 8);
		printf("\n请求打开音频输入设备");
		printf("\n采样参数：单声道 8kHz 8bit\n");
		mmResult = waveInOpen(&phwi, WAVE_MAPPER, &pwfx, (DWORD)(MicCallback), NULL, CALLBACK_FUNCTION);//3

		if (MMSYSERR_NOERROR == mmResult)
		{
			WAVEHDR pwh1;
			char buffer1[10240];
			pwh1.lpData = buffer1;
			pwh1.dwBufferLength = 10240;
			pwh1.dwUser = 1;
			pwh1.dwFlags = 0;
			mmResult = waveInPrepareHeader(phwi, &pwh1, sizeof(WAVEHDR));//4
			printf("\n准备缓冲区1");

			WAVEHDR pwh2;
			char buffer2[10240];
			pwh2.lpData = buffer2;
			pwh2.dwBufferLength = 10240;
			pwh2.dwUser = 2;
			pwh2.dwFlags = 0;
			mmResult = waveInPrepareHeader(phwi, &pwh2, sizeof(WAVEHDR));//4
			printf("\n准备缓冲区2\n");

			if (MMSYSERR_NOERROR == mmResult)
			{
				mmResult = waveInAddBuffer(phwi, &pwh1, sizeof(WAVEHDR));//5
				printf("\n将缓冲区1加入音频输入设备");
				mmResult = waveInAddBuffer(phwi, &pwh2, sizeof(WAVEHDR));//5
				printf("\n将缓冲区2加入音频输入设备\n");

				if (MMSYSERR_NOERROR == mmResult)
				{
					mmResult = waveInStart(phwi);//6
					printf("\n请求开始录音\n");
				}
			}
		}

	}
}
DWORD CALLBACK MicCallback(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	switch (uMsg)
	{
	case WIM_OPEN:
		printf("\n设备已经打开...\n");
		break;

	case WIM_DATA:
		printf("\n缓冲区%d存满...\n", ((LPWAVEHDR)dwParam1)->dwUser);
		waveInAddBuffer(hwavein, (LPWAVEHDR)dwParam1, sizeof(WAVEHDR));
		break;

	case WIM_CLOSE:
		printf("\n设备已经关闭...\n");
		break;
	default:
		break;
	}
	return 0;
}
void WaveInitFormat(LPWAVEFORMATEX m_WaveFormat, WORD nCh, DWORD nSampleRate, WORD BitsPerSample)
{
	m_WaveFormat->wFormatTag = WAVE_FORMAT_PCM;
	m_WaveFormat->nChannels = nCh;
	m_WaveFormat->nSamplesPerSec = nSampleRate;
	m_WaveFormat->nAvgBytesPerSec = nSampleRate * nCh * BitsPerSample / 8;
	m_WaveFormat->nBlockAlign = m_WaveFormat->nChannels * BitsPerSample / 8;
	m_WaveFormat->wBitsPerSample = BitsPerSample;
	m_WaveFormat->cbSize = 0;
}