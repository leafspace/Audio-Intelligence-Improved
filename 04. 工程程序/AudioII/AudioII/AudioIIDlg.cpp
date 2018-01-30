// AudioIIDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AudioII.h"
#include "AudioIIDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 应用于保存文件后的提示对话框
class CSaveFileDlg : public CDialogEx
{
public:
	CSaveFileDlg();
	CString filePath;
	// 对话框数据
	enum { IDD = IDD_SAVEFILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

														// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

CSaveFileDlg::CSaveFileDlg() : CDialogEx(CSaveFileDlg::IDD)
{
}

void CSaveFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	SetDlgItemText(IDC_EDIT1, this->filePath);
}

BEGIN_MESSAGE_MAP(CSaveFileDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSaveFileDlg::OnBnClickedOk)
END_MESSAGE_MAP()

void CSaveFileDlg::OnBnClickedOk()
{
	int nameLen = WideCharToMultiByte(CP_ACP, 0, this->filePath, -1, NULL, 0, NULL, NULL);
	char *filePathChar = new char[nameLen + 1];
	WideCharToMultiByte(CP_ACP, 0, this->filePath, -1, filePathChar, nameLen, NULL, NULL);                //将CString转为char*
	char *filePathTemp = new char[nameLen + 20];
	int index = 0;                                                          // 用于保存新文件名长度
	for (int i = 0; i < nameLen + 1; ++i) {                                 // 处理'\'为'\\'，若文件中的路径分隔符为'\'则无法准确定位
		filePathTemp[index++] = filePathChar[i];
		if (filePathChar[i] == '\\') {
			filePathTemp[index++] = '\\';
		}
	}
	filePathTemp[index] = 0;
	while (index >= 0) {
		if (filePathTemp[index] == '\\') {
			break;
		}
		index--;
	}
	filePathTemp[index + 1] = 0;
	CString cString(filePathTemp);
	ShellExecute(m_hWnd, _T("open"), cString, NULL, NULL, SW_SHOWNORMAL);   // 打开文件夹路径
	CDialogEx::OnOK();
}

// CAudioIIDlg 对话框


CAudioIIDlg::CAudioIIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAudioIIDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAudioIIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, mSlider);
}

BEGIN_MESSAGE_MAP(CAudioIIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CAudioIIDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CAudioIIDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CAudioIIDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CAudioIIDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CAudioIIDlg::OnBnClickedButton5)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CAudioIIDlg::OnNMCustomdrawSlider1)
END_MESSAGE_MAP()


// CAudioIIDlg 消息处理程序

BOOL CAudioIIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	this->orginWAVFile = NULL;
	this->isRecording = FALSE;
	this->isPlaying = FALSE;
	this->fileFilter.Format(_T("WAVE File(*.wav)|*.wav|ALL File(*.*)|*.*||"));
	this->mSlider.SetRange(1, 100);
	this->mSlider.SetPos(70);
	this->mSlider.SetPageSize(20);
	this->playSoundHandle = NULL;
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAudioIIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAudioIIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAudioIIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAudioIIDlg::OnBnClickedButton1()
{
	CFileDialog fileDlg(TRUE, _T("wav"), NULL, 0, this->fileFilter, this);  // 创建弹窗对象
	int chooseID = fileDlg.DoModal();                                       // 开始弹窗
	if (chooseID == IDOK) {                                                 // 用户选择了文件
		this->orginWAVFilePath = fileDlg.GetPathName();
		SetDlgItemText(IDC_EDIT1, this->orginWAVFilePath);
	}
	else if (chooseID == IDCANCEL) {                                        // 用户选择了取消
		this->orginWAVFilePath = "";
		SetDlgItemText(IDC_EDIT1, this->orginWAVFilePath);
		MessageBoxA(NULL, "Choose no file !", "Warning", MB_ICONWARNING | MB_OK);
	}
	this->SendDlgItemMessage(IDC_EDIT1, WM_VSCROLL, SB_BOTTOM, 0);          // 定位编辑框到最底端
}


void CAudioIIDlg::OnBnClickedButton2()
{
	// TODO: 编辑录音到本地
	if (isRecording) {
		// Todo 录音结束
		this->isRecording = FALSE;
		SetDlgItemText(IDC_BUTTON2, _T("Record"));
		// Todo 保存文件


	}
	else {
		CFileDialog fileDlg(FALSE, _T("wav"), _T("inputwave"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, this->fileFilter, this);
		int chooseID = fileDlg.DoModal();                                       // 开始弹窗
		if (chooseID == IDOK) {
			this->orginWAVFilePath = fileDlg.GetPathName();
			SetDlgItemText(IDC_EDIT1, this->orginWAVFilePath);
			// Todo 开始录音
			this->isRecording = TRUE;
			SetDlgItemText(IDC_BUTTON2, _T("Stop"));
			// Todo 调用库接口开始录音


		}
		else if (chooseID == IDCANCEL) {
			chooseID = MessageBoxA(NULL, "Choose no file !", "Warning", MB_ICONSTOP | MB_RETRYCANCEL);
			if (chooseID == IDRETRY) {
				this->OnBnClickedButton2();
			}
			else if (chooseID == IDCANCEL) {
				this->orginWAVFilePath = "";
				SetDlgItemText(IDC_EDIT1, this->orginWAVFilePath);
			}
		}
	}
}


void CAudioIIDlg::OnBnClickedButton3()
{
	CSaveFileDlg saveDlg;
	CFileDialog fileDlg(FALSE, _T("wav"), _T("outputwave"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, this->fileFilter, this);
	do {
		int chooseID = fileDlg.DoModal();
		if (chooseID == IDOK) {
			saveDlg.filePath = fileDlg.GetPathName();
			this->outWAVFilePath = fileDlg.GetPathName();
			SetDlgItemText(IDC_EDIT2, this->outWAVFilePath);
			this->SendDlgItemMessage(IDC_EDIT2, WM_VSCROLL, SB_BOTTOM, 0);
		}
		else if (chooseID == IDCANCEL) {
			chooseID = MessageBoxA(NULL, "Choose no file !", "Warning", MB_ICONSTOP | MB_OK);
			saveDlg.filePath = "";
			this->outWAVFilePath = "";
			SetDlgItemText(IDC_EDIT2, this->outWAVFilePath);
			break;
		}

		// 将处理后的数据保存到文件当中
		if (this->orginWAVFile != NULL) {
			FILE *fp = NULL;
			char tempBuffer[BUFFERSIZE] = { 0 }, filePathP[BUFFERSIZE] = { 0 };
			this->cstringToCharP(this->outWAVFilePath, tempBuffer);
			this->charPathToOut(tempBuffer, filePathP);
			if ((fp = fopen(filePathP, "wb")) == NULL) {
				MessageBoxA(NULL, "Create out file failured !", "Error", MB_ICONSTOP | MB_OK);
				break;
			}
			this->orginWAVFile->writeWAV(fp);
		} else {
			MessageBoxA(NULL, "Have no wave data !", "Error", MB_ICONSTOP | MB_OK);
		}

		saveDlg.DoModal();
	} while (0);
}


void CAudioIIDlg::OnBnClickedButton4()
{
	FILE *fp = NULL;
	char tempBuffer[BUFFERSIZE] = { 0 }, filePathP[BUFFERSIZE] = { 0 };
	do {
		CString filePath;
		GetDlgItemTextW(IDC_EDIT2, filePath);
		this->cstringToCharP(filePath, tempBuffer);
		this->charPathToOut(tempBuffer, filePathP);
		if ((fp = fopen(filePathP, "rb")) == NULL) {
			MessageBoxA(NULL, "Have no file !", "Error", MB_ICONSTOP | MB_OK);
			break;
		}
		fclose(fp);
		if (this->isPlaying) {
			CloseHandle(this->playSoundHandle);
			this->StopPlaySound();
		}
		else {
			this->BeginPlaySound(filePath);
		}
	} while (0);
	memset(tempBuffer, 0, BUFFERSIZE);
	memset(filePathP, 0, BUFFERSIZE);
}


void CAudioIIDlg::OnBnClickedButton5()
{
	// 将语音文件数据处理
	int nowPos = this->mSlider.GetPos();
	this->loadFile(this->orginWAVFilePath);
	CharacteristicParameters wavCParameters;
	frameEnergy(this->orginWAVFile, &wavCParameters);
	frameZCR(this->orginWAVFile, &wavCParameters);
	endpointDetection(this->orginWAVFile, &wavCParameters);
	for (unsigned short i = 0; i < wavCParameters.voiceParagraph.size(); ++i) {
		VoiceParagraph voiceParagraphT = wavCParameters.voiceParagraph[i];
		for (unsigned long j = voiceParagraphT.begin; j < voiceParagraphT.end; ++j) {
			// Todo 此处没有效果
			int indexValue = this->orginWAVFile->getData(j);
			int valueFlag = indexValue > 0 ? 1 : -1;
			int outValue = indexValue + (int)(indexValue * nowPos / 100) * valueFlag;
			this->orginWAVFile->setData(j, outValue);
		}
	}

	this->showFile(IDC_PICTURE2, this->orginWAVFile);
}

void CAudioIIDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	*pResult = 0;
	int nowPos = this->mSlider.GetPos();
	CString showPos;
	showPos.Format(_T("Process %d%%"), nowPos);
	SetDlgItemText(IDC_BUTTON5, showPos);
}

void CAudioIIDlg::cstringToCharP(const CString cstring, char* outString)
{
	int nameLen = WideCharToMultiByte(CP_ACP, 0, cstring, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, cstring, -1, outString, nameLen, NULL, NULL);                       //将CString转为char*
}

void CAudioIIDlg::charPathToOut(const char* filePath, char* outString)
{
	int index = 0;                                                          // 用于保存新文件名长度
	for (unsigned int i = 0; i < (unsigned int)strlen(filePath); ++i) {     // 处理'\'为'\\'，若文件中的路径分隔符为'\'则无法准确定位
		outString[index++] = filePath[i];
		if (filePath[i] == '\\') {
			outString[index++] = '\\';
		}
	}
	outString[index] = 0;
}

void CAudioIIDlg::loadFile(CString wavFilePath)
{
	char tempBuffer[BUFFERSIZE] = { 0 }, filePathP[BUFFERSIZE] = { 0 };
	this->cstringToCharP(wavFilePath, tempBuffer);
	this->charPathToOut(tempBuffer, filePathP);
	FILE *fp = NULL;
	if ((fp = fopen(filePathP, "rb")) == NULL) {
		MessageBoxA(NULL, "File is missing !", "Error", MB_ICONSTOP | MB_OK);
		return;
	}
	if (this->orginWAVFile != NULL) {
		// Todo 此处内存泄漏了
		//delete this->orginWAVFile;
		this->orginWAVFile = NULL;
	}

	this->orginWAVFile = new WAV(fp);
	fclose(fp);

	this->showFile(IDC_PICTURE1, this->orginWAVFile);
}

void CAudioIIDlg::showFile(int controlID, WAV *waveFile)
{
	if (waveFile == NULL) {
		MessageBoxA(NULL, "No file data !", "Error", MB_ICONSTOP | MB_OK);
		return;
	}

	this->StartDraw(controlID, waveFile);
}

void CAudioIIDlg::StartDraw(int ControlID, WAV *waveFile)
{
	CRect rc;
	CPaintDC dc(this);
	CWnd *pWnd = GetDlgItem(ControlID);
	pWnd->GetWindowRect(rc);
	CDC *pdc = pWnd->GetDC();

	CPen newPen;                                                            // 用于创建新画笔
	CPen *pOldPen;                                                          // 用于存放旧画笔
	CDC MemDC;                                                              // 首先定义一个显示设备对象
	CBitmap MemBitmap;                                                      // 定义一个位图对象

	int width = rc.Width();
	int height = rc.Height();

	MemDC.CreateCompatibleDC(NULL);                                         // 随后建立与屏幕显示兼容的内存显示设备
	MemBitmap.CreateCompatibleBitmap(pdc, width, height);                   // 下面建立一个与屏幕显示兼容的位图
	CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);                      // 将位图选入到内存显示设备中//只有选入了位图的内存显示设备才有地方绘图，画到指定的位图上
	MemDC.FillSolidRect(1, 1, width - 2, height - 2, RGB(255, 255, 255));   // 先用背景色将位图清除干净

	newPen.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));                          // 创建实心画笔，粗度为1，颜色为绿色
	pOldPen = MemDC.SelectObject(&newPen);                                  // 选择新画笔，并将旧画笔的指针保存到pOldPen

	unsigned int pitureWidth = (unsigned int)(width - 1);
	unsigned int pitureHeight = (unsigned int)(height - 1);
	unsigned int fileWidth = waveFile->getDataNumber();
	int oldMaxX = 0, oldMaxY = pitureHeight / 2;
	int oldMinX = 0, oldMinY = pitureHeight / 2;
	for (int i = 1; i < width; ++i) {
		double showMax = 0, showMin = 0;
		if (fileWidth <= (unsigned int)pitureWidth) {
			showMax = waveFile->getData(i);
			showMin = waveFile->getData(i);
		}
		else {
			unsigned int frameSize = (unsigned int)(fileWidth / width);
			unsigned int frameMaxI = (i - 1) * frameSize, frameMinI = (i - 1) * frameSize;
			for (unsigned int j = 0; j < frameSize; ++j) {
				unsigned int nowIndex = (i - 1) * frameSize + j;
				int indexTData = waveFile->getData(nowIndex);
				if (indexTData > waveFile->getData(frameMaxI)) {
					frameMaxI = nowIndex;
				}
				if (indexTData < waveFile->getData(frameMinI)) {
					frameMinI = nowIndex;
				}
			}
			int frameMax = 0, frameMin = 0;
			frameMax = waveFile->getData(frameMaxI);
			frameMin = waveFile->getData(frameMinI);
			showMax = (int)(frameMax);
			showMin = (int)(frameMin);
		}
		// 显示数值
		showMax /= pow(2, (double)(waveFile->getSampleBytes() * 8 - 1));
		showMax = (pitureHeight / 2) * showMax;
		showMin /= pow(2, (double)(waveFile->getSampleBytes() * 8 - 1));
		showMin = (pitureHeight / 2) * showMin;
		MemDC.MoveTo(oldMaxX, oldMaxY);
		MemDC.LineTo(i, (int)((pitureHeight / 2) - showMax));
		MemDC.MoveTo(oldMinX, oldMinY);
		MemDC.LineTo(i, (int)((pitureHeight / 2) - showMin));
		oldMaxX = i;
		oldMaxY = (int)((pitureHeight / 2) - showMax);
		oldMinX = i;
		oldMinY = (int)((pitureHeight / 2) - showMin);
	}

	MemDC.SelectObject(pOldPen);                                            // 恢复旧画笔
	newPen.DeleteObject();                                                  // 删除新画笔

	newPen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));                            // 创建实心画笔，粗度为1，颜色为绿色
	pOldPen = MemDC.SelectObject(&newPen);                                  // 选择新画笔，并将旧画笔的指针保存到pOldPen
	MemDC.MoveTo(0, pitureHeight / 2);
	MemDC.LineTo(pitureWidth, pitureHeight / 2);
	for (unsigned int i = 0; i < pitureWidth; i += (pitureWidth / 10)) {
		MemDC.MoveTo(i, pitureHeight / 2 + 5);
		MemDC.LineTo(i, pitureHeight / 2 - 5);
	}

	pdc->BitBlt(0, 0, width - 2, height - 2, &MemDC, 0, 0, SRCCOPY);        // 将内存中的图拷贝到屏幕上进行显示

	MemBitmap.DeleteObject();                                               // 绘图完成后的清理
	MemDC.DeleteDC();
}

void CAudioIIDlg::OnTimer(UINT_PTR nIDEvent)
{
	// Todo 播放进度绘图
	// 需要保存原先的语音数据

	CDialogEx::OnTimer(nIDEvent);
	CDialogEx::OnPaint();
}

void CAudioIIDlg::BeginPlaySound(CString filePath)
{
	struct RECVPARAM
	{
		CString filePath;
	};
	RECVPARAM revPara = { filePath };
	this->playSoundHandle = CreateThread(NULL, 0, TPlaySound, &revPara, 0, NULL);
	SetTimer(1, 100, NULL);
	this->isPlaying = TRUE;
	SetDlgItemText(IDC_BUTTON4, TEXT("Stop"));
}

void CAudioIIDlg::StopPlaySound()
{
	this->playSoundHandle = CreateThread(NULL, 0, TStopSound, NULL, 0, NULL);
	this->isPlaying = FALSE;
	SetDlgItemText(IDC_BUTTON4, TEXT("Play"));
}

DWORD WINAPI TPlaySound(LPVOID lpParam)
{
	struct RECVPARAM
	{
		CString filePath;
	};
	CString filePath = ((RECVPARAM *)lpParam)->filePath;
	CAudioIIDlg *pDlg = (CAudioIIDlg*)(AfxGetApp()->GetMainWnd());
	PlaySound(filePath, AfxGetInstanceHandle(), SND_SYNC | SND_FILENAME);
	pDlg->StopPlaySound();
	return NULL;
}

DWORD WINAPI TStopSound(LPVOID lpParam)
{
	PlaySound(NULL, AfxGetInstanceHandle(), SND_SYNC | SND_FILENAME);
	return NULL;
}
