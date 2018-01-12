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
}

BEGIN_MESSAGE_MAP(CAudioIIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CAudioIIDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CAudioIIDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CAudioIIDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CAudioIIDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CAudioIIDlg::OnBnClickedButton5)
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
	TCHAR szFilter[] = _T("语音文件(*.wav)|*.wav|所有文件(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, _T("wav"), NULL, 0, szFilter, this);

	if (IDOK == fileDlg.DoModal()) {
		CString strFilePath = fileDlg.GetPathName();
		SetDlgItemText(IDC_EDIT1, strFilePath);
	}
	this->SendDlgItemMessage(IDC_EDIT1, WM_VSCROLL, SB_BOTTOM, 0);
}


void CAudioIIDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
}


void CAudioIIDlg::OnBnClickedButton3()
{
	CSaveFileDlg dlgSave;
	TCHAR szFilter[] = _T("语音文件(*.wav)|*.wav|所有文件(*.*)|*.*||");
	CFileDialog fileDlg(FALSE, _T("wav"), _T("outwave"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);

	if (IDOK == fileDlg.DoModal()) {
		dlgSave.filePath = fileDlg.GetPathName();
		SetDlgItemText(IDC_EDIT2, dlgSave.filePath);
	}
	this->SendDlgItemMessage(IDC_EDIT2, WM_VSCROLL, SB_BOTTOM, 0);

	// Todo save file


	dlgSave.DoModal();
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
		CString outPath(filePathP);
		fp = fopen(filePathP, "rb");
		if (fp == NULL) {
			MessageBoxA(NULL, "Have no file !", "Error", MB_ICONSTOP | MB_YESNO);
			break;
		}
		if (isPlaying) {
			PlaySound(outPath, nullptr, SND_ASYNC | SND_FILENAME);
			SetDlgItemText(IDC_BUTTON4, TEXT("Stop"));
			isPlaying = FALSE;
		}
		else {
			PlaySound(nullptr, nullptr, SND_ASYNC | SND_FILENAME);
			SetDlgItemText(IDC_BUTTON4, TEXT("Play"));
			isPlaying = TRUE;
		}
		fclose(fp);
	} while (0);
	memset(tempBuffer, 0, BUFFERSIZE);
	memset(filePathP, 0, BUFFERSIZE);
}


void CAudioIIDlg::OnBnClickedButton5()
{
	// TODO: Add your control notification handler code here
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