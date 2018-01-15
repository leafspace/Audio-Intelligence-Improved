#pragma once
#include <mmsystem.h>
#pragma comment ( lib, "winmm.lib" )

class CHwMCI
{
private:
	char m_szErrorStr[256];
	MCIDEVICEID m_wDeviceID;
	MCIERROR hwMciSendCommand(MCIDEVICEID IDDevice, UINT uMsg, DWORD fdwCommand, DWORD dwParam);

public:
	CHwMCI();
	virtual ~CHwMCI();

	BOOL DeviceIDValid();
	LPSTR GetErrorStr();
	BOOL Record(int nChannels = 2, WORD nSamplesPerSec = 11025, int wBitsPerSample = 16);
	BOOL Play(DWORD dwFrom = 0, DWORD dwTo = 0);
	BOOL Pause();
	BOOL Stop();
	BOOL Save(CString Filename);
	BOOL Open(CString lpszWaveFileName = NULL);
	void Close();
	BOOL SetPara(
		WORD  wFormatTag,			                                        // format type
		WORD  nChannels,			                                        // 纪录声音的通道数
		DWORD nSamplesPerSec,		                                        // 记录每秒取样数
		DWORD nAvgBytesPerSec,	                                            // 记录每秒的数据量
		WORD  nBlockAlign,		                                            // 记录区块的对齐单位
		WORD  wBitsPerSample		                                        // 记录每个取样所需的位元数
	);
};
