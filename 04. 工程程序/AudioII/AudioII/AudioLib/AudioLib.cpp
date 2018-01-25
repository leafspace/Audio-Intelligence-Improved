#include "../stdafx.h"
#include "../AudioLib/AudioLib.h"

short Sign_Function(double data)                                            // 求短时过零率的辅助符号函数
{
	if (data >= 0) {
		return 1;
	}
	else {
		return 0;
	}
}

double Hamming_window(double data)                                          // 汉明窗函数
{
	if (data >= 0 && data <= (FRAMESIZE - 1)) {                             // 0<= n <= N-1的情况
		return 0.54 - 0.46*cos(2 * PI*data / (FRAMESIZE - 1));              // 返回数值
	}
	else {
		return 0;
	}
}

bool Frame_Energy(WAV* const wavFile, CharacteristicParameters *prameters)  // 用于求短时帧能量
{
	prameters->maxEnergy = 0;                                               // 最大短时帧能量置0
	prameters->minEnergy = 1000000;                                         // 最小短时帧能量置1000000

	double sum = 0;
	for (unsigned int i = 0; i < wavFile->getDataNumber() - FRAMESIZE; i += FRAMESHIFT) {                 // 这是所有短时帧能量数据的个数
		for (unsigned int j = i; j < i + FRAMESIZE; ++j) {                  // 遍历窗中的每一个数据
			sum += pow(wavFile->getData(j) * Hamming_window(i + FRAMESIZE - 1 - j), 2);                   //求每一个数据的能量
		}
		if (sum > prameters->maxEnergy) {                                   // 求取最大短时帧能量
			prameters->maxEnergy = sum;
		}
		if (sum < prameters->minEnergy) {                                   // 求取最大短时帧能量
			prameters->minEnergy = sum;
		}
		prameters->dataEnergy.push_back(sum);                               // 将此帧的短时帧能量保存
		sum = 0;
	}

	return true;
}

bool Frame_ZCR(WAV* const wavFile, CharacteristicParameters *prameters)     // 用于求短时过零率
{
	prameters->maxZCR = 0;                                                  // 最大短时过零率置0
	prameters->minZCR = 1000000;                                            // 最小短时过零率置1000000
	double sum = 0;
	for (unsigned int i = 0; i < wavFile->getDataNumber() - FRAMESIZE; i += FRAMESHIFT) {                 // 这是所有短时帧过零率数据的个数
		for (unsigned int j = i; j < i + FRAMESHIFT; ++j) {                 // 遍历窗中的每一个数据
			sum += abs(Sign_Function(wavFile->getData(j)) - Sign_Function(wavFile->getData(j - 1)))       // 过零率中的绝对值部分
				*Hamming_window(i + FRAMESHIFT - 1 - j);
		}
		sum /= 2;
		if (sum > prameters->maxZCR) {                                      // 求取最大短时过零率
			prameters->maxZCR = sum;
		}
		if (sum < prameters->minZCR) {                                      // 求取最大短时过零率
			prameters->minZCR = sum;
		}
		prameters->dataZCR.push_back(sum);
		sum = 0;
	}
	return true;
}

bool Endpoint_Detection(WAV* const wavFile, CharacteristicParameters *prameters)                          // 端点检测函数
{

	double energyHigh = 10;                                                 // 初始化短时帧能量高门限
	double energyLow = 2;                                                   // 初始化短时帧能量低门限
	double zcrHigh = 10;                                                    // 初始化短时过零率高门限
	double zcrLow = 5;                                                      // 初始化短时过零率低门限

	energyHigh = min(energyHigh, prameters->maxEnergy / 4);                 // 调整短时帧能量高门限
	energyLow = min(energyLow, prameters->maxEnergy / 8);                   // 调整短时帧能量低门限
	zcrHigh = min(zcrHigh, prameters->maxZCR / 4);                          // 调整短时帧能量高门限
	zcrLow = min(zcrLow, prameters->maxZCR / 8);                            // 调整短时帧能量低门限

	int statusFlag = 0;                                                     // 设置语音状态标志
	unsigned long begin = 0;                                                // 语音段落的起点
	unsigned long end = 0;                                                  // 语音段落的终点
	unsigned long voiceLength = 0;                                          // 语音段落的长度
	unsigned long silence = 0;                                              // 静音段落的长度
	prameters->voiceNumber = 0;

	for (unsigned long i = 0, frame = 0; i < wavFile->getDataNumber() - FRAMESIZE; ++i) {                 // 遍历每一帧
		frame = (i - FRAMESIZE) / FRAMESHIFT + 1;
		if (i <= FRAMESIZE) {
			frame = 0;
		}
		switch (statusFlag)
		{
		case MUTEPARAGRAPH:
		case INTERIMPARAGRAPH:
			if (prameters->dataEnergy[frame] > energyHigh) {                // 帧能量大于能量高门限,进入语音段
				begin = (unsigned long)max((int)(i - voiceLength - 1), 0);
				statusFlag = VOICEPARAGRAPH;
				voiceLength++;
				silence = 0;
			}
			else if (prameters->dataEnergy[frame] > energyLow || prameters->dataZCR[frame] > zcrLow) {    // 过渡段
				statusFlag = INTERIMPARAGRAPH;
				voiceLength++;
			}
			else {                                                          // 静音段
				statusFlag = MUTEPARAGRAPH;
				voiceLength = 0;
			}
			break;
		case VOICEPARAGRAPH:
			if (prameters->dataEnergy[frame] > prameters->minEnergy || 
				prameters->dataZCR[frame]  > prameters->minZCR) {           // 保持在语音段
				voiceLength++;
			}
			else {                                                          // 语音将结束
				silence++;
				if (silence < MINSILENCE) {                                 // 静音还不够长，尚未结束
					voiceLength++;
				}
				else {
					if (voiceLength < MINVOICELENGTH) {                     // 语音段长度太短，认为是噪声
						statusFlag = MUTEPARAGRAPH;
						silence = 0;
						voiceLength = 0;
					}
					else {                                                  // 语音结束
						statusFlag = OVERPARAGRAPH;
						end = max(begin + voiceLength, 0);
					}
				}
			}
			break;
		case OVERPARAGRAPH:
			prameters->voiceParagraph.push_back(VoiceParagraph(begin, end, voiceLength));                 // 保存语音段落信息
			voiceLength = 0;
			prameters->voiceNumber++;                                       // 语音段落+1
			statusFlag = MUTEPARAGRAPH;
			break;
		default:
			return false;
			break;
		}
	}

	if (statusFlag == VOICEPARAGRAPH) {                                     // 说明语音信号还没有结束，以当前记录下的最后一个点为终点保存语音段
		end = begin + voiceLength;
		prameters->voiceParagraph.push_back(VoiceParagraph(begin, end, voiceLength));
		++prameters->voiceNumber;
	}

	if (prameters->voiceNumber == 0 && prameters->voiceParagraph.size() == 0) {                           // 说明没有检测到语音段落，直接将整段语义合成为一个语音段落
		end = 0 + voiceLength;
		prameters->voiceParagraph.push_back(VoiceParagraph(0, end, voiceLength));
		++prameters->voiceNumber;
	}

	return true;
}