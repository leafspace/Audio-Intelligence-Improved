#include "WAVStruct.h"

WAV::WAV()
{

}

WAV::WAV(FILE fp)
{
    // Todo Analysis wav file

}

WAV::WAV(ifstream fin)
{
    // Todo Analysis wav file

}

WAV::~WAV()
{
    delete factChunk;
}

bool WAV::isWAV()
{
    do {
        if (!strstr(strupr(this->waveChunk.RIFF), "RIFF")) {
            this->isWAV = false;
        }

        if (!strstr(strupr(this->waveChunk.WAVE), "WAVE")) {
            this->isWAV = false;
        }

        if (!strstr(strupr(this->formatChunk.FMT), "FMT")) {
            this->isWAV = false;
        }

        if (this->factChunk) {
            if (!strstr(strupr(this->factChunk->FACT), "FACT")) {
                this->isWAV = false;
            }
        }

        if (!strstr(strupr(this->dataChunk.DATA), "DATA")) {
            this->isWAV = false;
        }
    } while(0);
    return this->isWAV;
}

unsigned short WAV::getChannelNumber()
{
    return this->formatChunk.channelNumber;
}

unsigned int WAV::getSampleFrequency()
{
    return this->formatChunk.sampleFrequency;
}

unsigned short WAV::getSampleBytes()
{
    return this->formatChunk.sampleBytes;
}

unsigned int WAV::getDataLength()
{
    return this->dataChunk.dataLength;
}

int WAV::getData(const unsigned int index)
{
    return this->dataChunk.dataList[index];
}