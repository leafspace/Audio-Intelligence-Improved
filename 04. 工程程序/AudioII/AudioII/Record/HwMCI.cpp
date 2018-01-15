#include "../stdafx.h"
#include "HwMCI.h"

MCIERROR CHwMCI::hwMciSendCommand(MCIDEVICEID IDDevice, UINT uMsg, DWORD fdwCommand, DWORD dwParam)
{
	MCIERROR mciError = ::mciSendCommand(IDDevice, uMsg, fdwCommand, dwParam);
	if (mciError != 0) {
		memset(this->m_szErrorStr, 0, sizeof(this->m_szErrorStr));
		mciGetErrorStringA(mciError, this->m_szErrorStr, sizeof(this->m_szErrorStr));
	}
	return mciError;
}

CHwMCI::CHwMCI() : m_wDeviceID(NULL)
{
	memset(this->m_szErrorStr, 0, sizeof(this->m_szErrorStr));
}

CHwMCI::~CHwMCI()
{
	this->Close();
}

BOOL CHwMCI::DeviceIDValid()
{
	return (this->m_wDeviceID != NULL);
}

LPSTR CHwMCI::GetErrorStr()
{
	return this->m_szErrorStr;
}

BOOL CHwMCI::Record(int nChannels, WORD nSamplesPerSec, int wBitsPerSample)
{
	if (nChannels != 1 && nChannels != 2) {
		return FALSE;
	}

	this->Close();	                                                        // 为成功打开设备，录制前关闭一次设备
	if (!this->Open()) {
		return FALSE;
	}
	SetPara(WAVE_FORMAT_PCM, nChannels, nSamplesPerSec,
		nSamplesPerSec*nChannels*wBitsPerSample / 8, nChannels*wBitsPerSample / 8, wBitsPerSample);

	MCI_RECORD_PARMS RecordParms = { 0 };
	return (hwMciSendCommand(this->m_wDeviceID, MCI_RECORD, NULL, (DWORD)(LPVOID)&RecordParms) == 0);
}

BOOL CHwMCI::Play(DWORD dwFrom, DWORD dwTo)                                 // 播放wave文件
{
	this->Stop();
	MCI_PLAY_PARMS PlayParms = { 0 };
	PlayParms.dwFrom = dwFrom;		                                        // 这是为了指定从什么地方（时间）播放WAVE文件
	PlayParms.dwTo = dwTo;
	if (hwMciSendCommand(this->m_wDeviceID, MCI_PLAY, MCI_FROM, (DWORD)(LPVOID)&PlayParms) != 0) {
		return FALSE;
	}
	return TRUE;
}

BOOL CHwMCI::Pause()
{
	if (this->m_wDeviceID == NULL) {
		return FALSE;
	}
	return (hwMciSendCommand(this->m_wDeviceID, MCI_PAUSE, NULL, NULL) == 0);
}

BOOL CHwMCI::Stop()
{
	if (this->m_wDeviceID == NULL) {
		return FALSE;
	}
	return (hwMciSendCommand(this->m_wDeviceID, MCI_STOP, NULL, NULL) == 0);
}

BOOL CHwMCI::Save(CString Filename)
{
	this->Stop();
	MCI_SAVE_PARMS SaveParms = { 0 };
	SaveParms.lpfilename = Filename;
	return (hwMciSendCommand(m_wDeviceID, MCI_SAVE, MCI_SAVE_FILE | MCI_WAIT, (DWORD)(LPVOID)&SaveParms) == 0);
}


BOOL CHwMCI::Open(CString lpszWaveFileName)
{
	MCI_OPEN_PARMS OpenParms = { 0 };
	OpenParms.lpstrDeviceType = (LPCWSTR)(lpszWaveFileName ? NULL : MCI_DEVTYPE_WAVEFORM_AUDIO);
	OpenParms.lpstrElementName = (lpszWaveFileName ? lpszWaveFileName : _T(""));                          // 打开的声音文件名
	this->m_wDeviceID = NULL;	                                                                          // 打开的音频设备的ID
	if (hwMciSendCommand(NULL, MCI_OPEN,
		(lpszWaveFileName ? 0 : (MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID)) | MCI_WAIT | MCI_OPEN_ELEMENT, (DWORD)(LPVOID)&OpenParms) != 0) {
		return FALSE;
	}

	this->m_wDeviceID = OpenParms.wDeviceID;
	return TRUE;
}

void CHwMCI::Close()
{
	if (this->m_wDeviceID != NULL) {
		hwMciSendCommand(this->m_wDeviceID, MCI_CLOSE, NULL, NULL);
	}
	this->m_wDeviceID = NULL;                                               // 打开的音频设备的ID
}

BOOL CHwMCI::SetPara(WORD wFormatTag, WORD nChannels, DWORD nSamplesPerSec, DWORD nAvgBytesPerSec,
	WORD nBlockAlign, WORD wBitsPerSample)                                  // 设置参数
{
	if (this->m_wDeviceID == NULL)
		return FALSE;

	MCI_WAVE_SET_PARMS  SetParms = { 0 };
	SetParms.wFormatTag = wFormatTag;
	SetParms.nChannels = nChannels;
	SetParms.nSamplesPerSec = nSamplesPerSec;
	SetParms.nAvgBytesPerSec = nAvgBytesPerSec;
	SetParms.nBlockAlign = nBlockAlign;
	SetParms.wBitsPerSample = wBitsPerSample;

	return (hwMciSendCommand(this->m_wDeviceID, MCI_SET, MCI_WAIT |
		MCI_WAVE_SET_FORMATTAG | MCI_WAVE_SET_CHANNELS | MCI_WAVE_SET_SAMPLESPERSEC | MCI_WAVE_SET_AVGBYTESPERSEC |
		MCI_WAVE_SET_BLOCKALIGN | MCI_WAVE_SET_BITSPERSAMPLE,
		(DWORD)(LPVOID)&SetParms) == 0);
}