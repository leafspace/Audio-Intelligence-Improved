#pragma once
#include "Common.h"

typedef struct WaveChunk                                                    // RIFF块结构
{
	char RIFF[4] = { 0 };
	unsigned int fileLength = 0;                                            // 文件长度
	char WAVE[4] = { 0 };
} WaveChunk;

typedef struct FormatChunk                                                  // 格式块结构
{
	char FMT[4] = { 0 };
	unsigned int fChunkLength = 0;                                          // 格式块长度
	unsigned short formatCategory = 0;                                      // 格式类别
	unsigned short channelNumber = 0;                                       // 声道数
	unsigned int sampleFrequency = 0;                                       // 采样频率
	unsigned int transferRate = 0;                                          // 数据传送速率
	unsigned short sampleBytes = 0;                                         // 样本字节数
	unsigned short sampleBits = 0;                                          // 样本位数

	unsigned short extraInfo = 0;                                           // 附加信息
} FormatChunk;

typedef struct FactChunk                                                    // 附加块结构
{
	char FACT[4] = { 0 };
	unsigned int eChunkLength = 0;                                          // 块长度
	unsigned int eChunk = 0;
} FactChunk;

typedef struct DataChunk                                                    // 数据块结构
{
	char DATA[4] = { 0 };
	unsigned int dataLength = 0;                                            // 数据长度
	DataType *dataList = NULL;                                              // 数据列表
} DataChunk;

class WAV
{
private:
	bool isWAV = true;
	WaveChunk waveChunk;                                                    // RIFF块
	FormatChunk formatChunk;                                                // 格式块
	FactChunk *factChunk = NULL;                                            // 附加块
	DataChunk dataChunk;                                                    // 数据块
public:
	WAV();
	WAV(FILE *fp);
	WAV(ifstream fin);
	~WAV();

	bool isWAVE();
	unsigned short getChannelNumber();
	unsigned int getSampleFrequency();
	unsigned short getSampleBytes();
	unsigned int getDataNumber();
	int getData(const unsigned int index);
};