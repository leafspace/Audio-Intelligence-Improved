#pragma once
#define FRAMESIZE	256                                                     // 每个窗的窗长
#define FRAMESHIFT	125                                                     // 窗函数的帧移
#define PI			3.14159

#define MUTEPARAGRAPH		0                                               // 静音段
#define INTERIMPARAGRAPH	1                                               // 过渡段
#define VOICEPARAGRAPH		2                                               // 语音段
#define OVERPARAGRAPH		3                                               // 语音结束

#define MINSILENCE 6                                                        // 最短静音长度
#define MINVOICELENGTH 15                                                   // 最短语音长度

#include "../AudioStruct/WAVStruct.h"

typedef struct VoiceParagraph
{
	unsigned long begin;                                                    // 语音段落开始点
	unsigned long end;                                                      // 语音段落结束点
	unsigned long voiceLength;                                              // 语音段落长度
	VoiceParagraph(unsigned long be, unsigned long en, unsigned long vo) {  // 构造初始化
		begin = be;
		end = en;
		voiceLength = vo;
	}
} VoiceParagraph;

typedef struct CharacteristicParameters
{
	double maxZCR;
	double minZCR;
	double maxEnergy;
	double minEnergy;
	vector<double> dataZCR;
	vector<double> dataEnergy;
	unsigned long voiceNumber;                                              // 语音段落个数
	vector<VoiceParagraph> voiceParagraph;                                  // 保存这个语音文件中所有要处理的语音段落
} CharacteristicParameters;

short Sign_Function(double data);
double Hamming_window(double data);

bool frameEnergy(WAV* const wavFile, CharacteristicParameters *prameters);
bool frameZCR(WAV* const wavFile, CharacteristicParameters *prameters);

bool endpointDetection(WAV* const wavFile, CharacteristicParameters *prameters);