#include "WAVStruct.h"

WAV::WAV()
{

}

WAV::WAV(FILE *fp)
{
	fread(this->waveChunk.RIFF, sizeof(char), 4, fp);                       // 读取'RIFF'
	fread(&this->waveChunk.fileLength, sizeof(unsigned int), 1, fp);        // 读取文件的大小
	fread(this->waveChunk.WAVE, sizeof(char), 4, fp);                       // 读取'WAVE'

	fread(this->formatChunk.FMT, sizeof(char), 4, fp);                      // 读取'FMT'
	fread(&this->formatChunk.fChunkLength, sizeof(unsigned int), 1, fp);    // 读取块大小
	fread(&this->formatChunk.formatCategory, sizeof(unsigned short), 1, fp);// 读取文件标签
	fread(&this->formatChunk.channelNumber, sizeof(unsigned short), 1, fp); // 读取声道数
	fread(&this->formatChunk.sampleFrequency, sizeof(unsigned int), 1, fp); // 读取采样频率
	fread(&this->formatChunk.transferRate, sizeof(unsigned int), 1, fp);    // 读取数据传送速率
	fread(&this->formatChunk.sampleBytes, sizeof(unsigned short), 1, fp);   // 读取样本字节数
	fread(&this->formatChunk.sampleBits, sizeof(unsigned short), 1, fp);    // 读取样本位数
	if (this->formatChunk.fChunkLength > 16) {                              // 含有附加块的情况
		fread(&this->formatChunk.extraInfo, sizeof(unsigned short), 1, fp); // 读取附加信息
		this->factChunk = new FactChunk();
		fread(this->factChunk->FACT, sizeof(char), 4, fp);                  // 读取'FACT'
		fread(&this->factChunk->eChunkLength, sizeof(unsigned int), 1, fp); // 读取块大小
		fread(&this->factChunk->eChunk, sizeof(unsigned int), 1, fp);       // 读取附加块
	}

	DataType::setSizeFlag(this->formatChunk.sampleBytes);

	fread(this->dataChunk.DATA, sizeof(char), 4, fp);                       // 读取'DATA'
	fread(&this->dataChunk.dataLength, sizeof(unsigned int), 1, fp);        // 读取数据大小
	double dataNumber = this->dataChunk.dataLength / this->formatChunk.sampleBytes;
	if (dataNumber != this->getDataNumber()
		|| this->dataChunk.dataLength > this->waveChunk.fileLength) {       // 数值跟设置不吻合
		this->isWAV = false;
		return;
	}
	this->dataChunk.dataList = new DataType[(unsigned int)dataNumber];
	fread(this->dataChunk.dataList, this->formatChunk.sampleBytes, (unsigned int)dataNumber, fp);         //读取数据
}

WAV::WAV(ifstream fin)
{
	fin.read(this->waveChunk.RIFF, sizeof(char) * 4);
	fin.read(this->waveChunk.RIFF, sizeof(char) * 4);                       // 读取'RIFF'
	fin.read(reinterpret_cast<char*>(this->waveChunk.fileLength), sizeof(unsigned int) * 1);              // 读取文件的大小
	fin.read(this->waveChunk.WAVE, sizeof(char) * 4);                       // 读取'WAVE'

	fin.read(this->formatChunk.FMT, sizeof(char) * 4);                      // 读取'FMT'
	fin.read(reinterpret_cast<char*>(this->formatChunk.fChunkLength), sizeof(unsigned int) * 1);          // 读取块大小
	fin.read(reinterpret_cast<char*>(this->formatChunk.formatCategory), sizeof(unsigned short) * 1);      // 读取文件标签
	fin.read(reinterpret_cast<char*>(this->formatChunk.channelNumber), sizeof(unsigned short) * 1);       // 读取声道数
	fin.read(reinterpret_cast<char*>(this->formatChunk.sampleFrequency), sizeof(unsigned int) * 1);       // 读取采样频率
	fin.read(reinterpret_cast<char*>(this->formatChunk.transferRate), sizeof(unsigned int) * 1);          // 读取数据传送速率
	fin.read(reinterpret_cast<char*>(this->formatChunk.sampleBytes), sizeof(unsigned short) * 1);         // 读取样本字节数
	fin.read(reinterpret_cast<char*>(this->formatChunk.sampleBits), sizeof(unsigned short) * 1);          // 读取样本位数
	if (this->formatChunk.fChunkLength > 16) {                              // 含有附加块的情况
		fin.read(reinterpret_cast<char*>(this->formatChunk.extraInfo), sizeof(unsigned short) * 1);       // 读取附加信息
		this->factChunk = new FactChunk();
		fin.read(this->factChunk->FACT, sizeof(char) * 4);                  // 读取'FACT'
		fin.read(reinterpret_cast<char*>(this->factChunk->eChunkLength), sizeof(unsigned int) * 1);       // 读取块大小
		fin.read(reinterpret_cast<char*>(this->factChunk->eChunk), sizeof(unsigned int) * 1);             // 读取附加块
	}

	DataType::setSizeFlag(this->formatChunk.sampleBytes);

	fin.read(this->dataChunk.DATA, sizeof(char) * 4);                       // 读取'DATA'
	fin.read(reinterpret_cast<char*>(this->dataChunk.dataLength), sizeof(unsigned int) * 1);              // 读取数据大小
	double dataNumber = this->dataChunk.dataLength / this->formatChunk.sampleBytes;
	if (dataNumber != this->getDataNumber()
		|| this->dataChunk.dataLength > this->waveChunk.fileLength) {       // 数值跟设置不吻合
		this->isWAV = false;
		return;
	}
	this->dataChunk.dataList = new DataType[(unsigned int)dataNumber];
	fin.read(reinterpret_cast<char*>(this->dataChunk.dataList), this->formatChunk.sampleBytes * (unsigned int)dataNumber);
}

WAV::~WAV()
{
	delete factChunk;
	factChunk = NULL;
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
	} while (0);
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

unsigned int WAV::getDataNumber()
{
	return (unsigned int)(this->dataChunk.dataLength / this->formatChunk.sampleBytes);
}

int WAV::getData(const unsigned int index)
{
	return this->dataChunk.dataList[index];
}