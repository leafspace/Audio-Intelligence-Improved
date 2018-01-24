#include "../AudioStruct/WAVStruct.h"

bool WavFile_Initial::Frame_Energy(void)                                     //用于求短时帧能量
{
	cout << "TIP : Calculate Energy ..." << endl;
	maxEnergy = 0;                                                           //最大短时帧能量置0
	minEnergy = 1000000;                                                     //最小短时帧能量置1000000
	double sum = 0;
	for (unsigned long i = 0; i < this->Get_dataNumber() - N; i += WavFile_Initial::FrameShift) { //这是所有短时帧能量数据的个数
		for (unsigned long j = i; j < i + N; ++j) {                          //遍历窗中的每一个数据
			sum += pow(dataDouble[j] * Hamming_window(i + N - 1 - j), 2);    //求每一个数据的能量
		}
		if (sum > maxEnergy) {                                               //求取最大短时帧能量
			maxEnergy = sum;
		}
		if (sum < minEnergy) {                                               //求取最大短时帧能量
			minEnergy = sum;
		}
		dataEnergy.push_back(sum);                                           //将此帧的短时帧能量保存
		sum = 0;
	}

	return true;
}

bool WavFile_Initial::Frame_ZCR(void)                                        //用于求短时过零率
{
	cout << "TIP : Calculate ZCR ..." << endl;
	maxZCR = 0;                                                              //最大短时过零率置0
	minZCR = 1000000;                                                        //最小短时过零率置1000000
	double sum = 0;
	for (unsigned long i = 0; i < this->Get_dataNumber() - N; i += WavFile_Initial::FrameShift) {//这是所有短时帧过零率数据的个数
		for (unsigned long j = i; j < i + N; ++j) {                          //遍历窗中的每一个数据
			sum += abs(Sign_Function(dataDouble[j]) - Sign_Function(dataDouble[j - 1]))//过零率中的绝对值部分
				*Hamming_window(i + N - 1 - j);
		}
		sum /= 2;
		if (sum > maxZCR) {                                                  //求取最大短时过零率
			maxZCR = sum;
		}
		if (sum < minZCR) {                                                  //求取最大短时过零率
			minZCR = sum;
		}
		dataZCR.push_back(sum);
		sum = 0;
	}
	return true;
}

bool WavFile_Initial::Frame_EnergyZcr(void)                                  //用于同时求取短时帧能量与短时过零率
{
	cout << "TIP : Calculate Engry and ZCR ..." << endl;
	maxEnergy = 0;                                                           //最大短时帧能量置0
	minEnergy = 1000000;                                                     //最小短时帧能量置1000000
	maxZCR = 0;                                                              //最大短时过零率置0
	minZCR = 1000000;                                                        //最小短时过零率置1000000
	double sumEnergy = 0;
	double sumZcr = 0;
	double hanming = 0;
	for (unsigned long i = 0; i < this->Get_dataNumber() - N; i += WavFile_Initial::FrameShift) {//这是所有短时帧能量数据的个数
		for (unsigned long j = i; j < i + N; ++j) {                          //遍历窗中的每一个数据
			hanming = Hamming_window(i + N - 1 - j);
			sumEnergy += pow(dataDouble[j] * hanming, 2);                    //求每一个数据的能量
			sumZcr += abs(Sign_Function(dataDouble[j]) - Sign_Function(dataDouble[j - 1]))       //过零率中的绝对值部分
				*hanming;
		}
		sumZcr /= 2;

		if (sumEnergy > maxEnergy) {                                         //求取最大短时帧能量
			maxEnergy = sumEnergy;
		}
		if (sumEnergy < minEnergy) {                                         //求取最大短时帧能量
			minEnergy = sumEnergy;
		}
		if (sumZcr > maxZCR) {                                               //求取最大短时过零率
			maxZCR = sumZcr;
		}
		if (sumZcr < minZCR) {                                               //求取最大短时过零率
			minZCR = sumZcr;
		}
		dataEnergy.push_back(sumEnergy);                                     //将此帧的短时帧能量保存
		dataZCR.push_back(sumZcr);
		sumEnergy = 0;
		sumZcr = 0;

	}

	return true;
}

void WavFile_Initial::Pre_emphasis(VoiceParagraph voiceParagraph, double *dataDouble)  //对一个段落内的数据进行预加重处理 
{
	for (unsigned long i = 0; i < voiceParagraph.voiceLength; ++i) {
		unsigned long dataIndex = voiceParagraph.begin + i;
		if (dataIndex == 0 || dataIndex == this->Get_dataNumber()) {
			continue;
		}
		dataDouble[dataIndex] = dataDouble[dataIndex] - WavFile_Initial::preCoefficient * dataDouble[dataIndex - 1]; //加一阶数字滤波器
	}
}

bool WavFile_Initial::Frame_Data(double *data, unsigned long index, double* dataSpace, int dataSpaceSize)                      //获取端点检测后第index帧的分帧加窗操作
{
	if (dataSpaceSize < WavFile_Initial::N) {                                //预分配的空间不足一帧时
		return false;
	}

	VoiceParagraph voiceParagraph(-1, -1, -1);
	for (unsigned long i = 0; i < this->voiceParagraph.size(); ++i) {
		if ((int)(index - this->Get_frameNumber(this->voiceParagraph[i])) <= 0) {      //如果减去此段落的帧数，数据小于0，则此帧为当前数据段
			voiceParagraph = this->voiceParagraph[i];
			break;
		}
		index = index - this->Get_frameNumber(this->voiceParagraph[i]);
	}

	if (voiceParagraph.begin == -1 || voiceParagraph.end == -1 || voiceParagraph.voiceLength == -1) {
		return false;
	}

	unsigned long begin = voiceParagraph.begin + (index - 1) * WavFile_Initial::FrameShift;
	unsigned long end = begin + WavFile_Initial::N - 1;
	unsigned long voiceLength = WavFile_Initial::N;

	if (end >= voiceParagraph.end) {
		end = voiceParagraph.end;
		voiceLength = end - begin + 1;
	}

	voiceParagraph.begin = begin;
	voiceParagraph.end = end;
	voiceParagraph.voiceLength = voiceLength;

	for (unsigned long i = voiceParagraph.begin; i <= voiceParagraph.end; ++i) {
		dataSpace[i - voiceParagraph.begin] = data[i] * this->Hamming_window(i - voiceParagraph.begin);    //加窗功能
	}

	return true;
}

bool WavFile_Initial::Frame_Data(double *data, double dataSize, unsigned long index, double* dataSpace, int dataSpaceSize)     //对部分数据进行分帧加窗操作
{
	if (dataSpaceSize < WavFile_Initial::N) {                                //预分配的空间不足一帧时
		return false;
	}

	unsigned long frameNumber = this->Get_frameNumber(dataSize);
	if (index < 1 || index > frameNumber) {                                  //帧位不属于数据段允许范围内
		return false;
	}

	unsigned long begin = (index - 1) * WavFile_Initial::FrameShift;
	unsigned long end = begin + WavFile_Initial::N - 1;
	unsigned long voiceLength = WavFile_Initial::N;

	if (index == frameNumber && end != dataSize) {                           //如果长度不为整帧
		if (end < dataSize) {                                                //全部数据多余
		}
		else if (end > dataSize) {                                         //全部数据缺少
			end = (unsigned long)(dataSize - 1);
			voiceLength = (unsigned long)(dataSize - begin);
		}
	}
	VoiceParagraph voiceParagraph(begin, end, voiceLength);

	for (unsigned long i = voiceParagraph.begin; i <= voiceParagraph.end; ++i) {
		dataSpace[i - voiceParagraph.begin] = data[i] * this->Hamming_window(i - voiceParagraph.begin);    //加窗功能
	}
	return true;
}

bool WavFile_Initial::Endpoint_Detection(void)                               //端点检测函数
{
	//this->Frame_Energy();                                                    //计算短时帧能量
	//this->Frame_ZCR();                                                       //计算短时过零率
	this->Frame_EnergyZcr();                                                 //计算短时帧能量与短时过零率

	energyHigh = 10;                                                         //初始化短时帧能量高门限
	energyLow = 2;                                                           //初始化短时帧能量低门限
	zcrHigh = 10;                                                            //初始化短时过零率高门限
	zcrLow = 5;                                                              //初始化短时过零率低门限

	energyHigh = min(energyHigh, Get_maxEnergy() / 4);                       //调整短时帧能量高门限
	energyLow = min(energyLow, Get_maxEnergy() / 8);                         //调整短时帧能量低门限
	zcrHigh = min(zcrHigh, Get_maxZCR() / 4);                                //调整短时帧能量高门限
	zcrLow = min(zcrLow, Get_maxZCR() / 8);                                  //调整短时帧能量低门限

	int statusFlag = 0;                                                      //设置语音状态标志
	unsigned long begin = 0;                                                 //语音段落的起点
	unsigned long end = 0;                                                   //语音段落的终点
	unsigned long voiceLength = 0;                                           //语音段落的长度
	unsigned long silence = 0;                                               //静音段落的长度
	voiceNumber = 0;

	for (unsigned long i = 0, frame = 0; i < this->Get_dataNumber() - N; ++i) { //遍历每一帧
		frame = (i - N) / WavFile_Initial::FrameShift + 1;
		if (i <= 256) {
			frame = 0;
		}
		switch (statusFlag)
		{
		case MUTEPARAGRAPH:
		case INTERIMPARAGRAPH:
			if (Get_DataEnergy(frame) > energyHigh) {                        //帧能量大于能量高门限,进入语音段
				begin = (unsigned long)max((int)(i - voiceLength - 1), 0);
				statusFlag = VOICEPARAGRAPH;
				voiceLength++;
				silence = 0;
			}
			else if (Get_DataEnergy(frame) > energyLow || Get_DataZCR(frame) > zcrLow) {         //过渡段
				statusFlag = INTERIMPARAGRAPH;
				voiceLength++;
			}
			else {                                                           //静音段
				statusFlag = MUTEPARAGRAPH;
				voiceLength = 0;
			}
			break;
		case VOICEPARAGRAPH:
			if (Get_DataEnergy(frame) > Get_minEnergy() || Get_DataZCR(frame) > Get_minZCR()) {  //保持在语音段
				voiceLength++;
			}
			else {                                                           //语音将结束
				silence++;
				if (silence < minSilence) {                                  //静音还不够长，尚未结束
					voiceLength++;
				}
				else {
					if (voiceLength < minVoiceLength) {                      //语音段长度太短，认为是噪声
						statusFlag = MUTEPARAGRAPH;
						silence = 0;
						voiceLength = 0;
					}
					else {                                                   //语音结束
						statusFlag = OVERPARAGRAPH;
						end = max(begin + voiceLength, 0);
					}
				}
			}
			break;
		case OVERPARAGRAPH:
			voiceParagraph.push_back(VoiceParagraph(begin, end, voiceLength));         //保存语音段落信息
			voiceLength = 0;
			voiceNumber++;                                                   //语音段落+1
			statusFlag = MUTEPARAGRAPH;
			break;
		default:
			MessageBoxA(NULL, "ERROR : Status failure !", "ERROR", MB_ICONHAND);
			return false;
			break;
		}
	}

	if (statusFlag == VOICEPARAGRAPH) {                                      //说明语音信号还没有结束，以当前记录下的最后一个点为终点保存语音段
		end = begin + voiceLength;
		voiceParagraph.push_back(VoiceParagraph(begin, end, voiceLength));
		++voiceNumber;
	}

	if (voiceNumber == 0 && voiceParagraph.size() == 0) {                    //说明没有检测到语音段落，直接将整段语义合成为一个语音段落
		end = 0 + voiceLength;
		voiceParagraph.push_back(VoiceParagraph(0, end, voiceLength));
		++voiceNumber;
	}

	cout << "TIP : Voice number is " << voiceNumber << endl;

	return true;
}