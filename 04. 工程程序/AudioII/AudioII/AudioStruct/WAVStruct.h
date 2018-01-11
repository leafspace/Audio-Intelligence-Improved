#pragma once
#include "Common.h"

typedef struct WaveChunk
{
    char RIFF[4] = { 0 };
    unsigned int fileLength = 0;
    char WAVE[4] = { 0 };
} WaveChunk;

typedef struct FormatChunk
{
    char FMT[4] = { 0 };
    unsigned int fChunkLength = 0;
    unsigned short formatCategory = 0;
    unsigned short channelNumber = 0;
    unsigned int sampleFrequency = 0;
    unsigned int transferRate = 0;
    unsigned short sampleBytes = 0;
    unsigned short sampleBits = 0;
    
    unsigned short extraInfo = 0;
} FormatChunk;

typedef struct FactChunk
{
    char FACT[4] = { 0 };
    unsigned int eChunkLength = 0;
    unsigned int eChunk = 0;
} FactChunk;

typedef struct DataChunk
{
    char DATA[4] = { 0 };
    unsigned int dataLength = 0;
    DataType *dataList = NULL;
} DataChunk;

class WAV 
{
private:
    bool isWAV = true;
    WaveChunk waveChunk;
    FormatChunk formatChunk;
    FactChunk *factChunk = NULL;
    DataChunk dataChunk;
public:
    WAV();
    WAV(FILE fp);
    WAV(ifstream fin);
    ~WAV();

    bool isWAV();
    unsigned short getChannelNumber();
    unsigned int getSampleFrequency();
    unsigned short getSampleBytes();
    unsigned int getDataLength();
    int getData(const unsigned int index);
};