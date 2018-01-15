// AudioIIDlg.h : 头文件
//

#pragma once
#define BUFFERSIZE 256
#include <windows.h>
#include <mmsystem.h>
#include "afxcmn.h"
#include "AudioStruct\WAVStruct.h"
#pragma comment(lib, "WINMM.LIB")

// CAudioIIDlg 对话框
class CAudioIIDlg : public CDialogEx
{
	// 构造
public:
	CAudioIIDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_AUDIOII_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);

private:
	WAV *orginWAVFile;
	bool isRecording;
	bool isPlaying;
	CString fileFilter;
	CString orginWAVFilePath;
	CString outWAVFilePath;
	void cstringToCharP(const CString cstring, char* outString);
	void charPathToOut(const char* filePath, char* outString);

	void loadFile();
	void showFile(WAV *waveFile);

	void StartDraw(int ControlID, WAV *waveFile);
public:
	CSliderCtrl mSlider;
};
