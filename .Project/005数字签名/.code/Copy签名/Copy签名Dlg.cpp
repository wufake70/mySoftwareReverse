
// Copy签名Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Copy签名.h"
#include "Copy签名Dlg.h"
#include "afxdialogex.h"
#include <Windows.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// msvc v140版本(vs2015)编译

CString NewFileFullPath;
CString fileSuffix;


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCopy签名Dlg 对话框



CCopy签名Dlg::CCopy签名Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_COPY_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCopy签名Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCopy签名Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CCopy签名Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CCopy签名Dlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CCopy签名Dlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CCopy签名Dlg 消息处理程序

BOOL CCopy签名Dlg::OnInitDialog()
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCopy签名Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCopy签名Dlg::OnPaint()
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
HCURSOR CCopy签名Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCopy签名Dlg::OnBnClickedButton1()
{
	BOOL isOpen = TRUE;		//是否打开(否则为保存)
	CString defaultDir = "";	//默认打开的文件路径
	CString fileName = "";			//默认打开的文件名
	CString filter = "文件 (*.dll *.exe *.sys)|*.dll;*.exe;*.sys|";	//文件过虑的类型
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
	INT_PTR result = openFileDlg.DoModal();
	CString filePath = "";
	if (result == IDOK) {
		filePath = openFileDlg.GetPathName();
	}
	CWnd::SetDlgItemTextA(IDC_EDIT1, filePath);
}


void CCopy签名Dlg::OnBnClickedButton2()
{
	BOOL isOpen = TRUE;		//是否打开(否则为保存)
	CString defaultDir;	//默认打开的文件路径
	CString fileName;			//默认打开的文件名
	CString filter = "文件 (*.dll *.exe *.sys)|*.dll;*.exe;*.sys|";	//文件过虑的类型
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
	INT_PTR result = openFileDlg.DoModal();
	CString filePath ="";
	if (result == IDOK) {
		filePath = openFileDlg.GetPathName();
		fileSuffix = openFileDlg.GetFileName();
		// 无数字签名pe文件的全路径
		NewFileFullPath = filePath;
		
	}
	CWnd::SetDlgItemTextA(IDC_EDIT2, filePath);
}


void CCopy签名Dlg::OnBnClickedButton3()
{
	CString sigFileName;
	CString notSigFileName;
	GetDlgItemTextA(IDC_EDIT1, sigFileName);
	GetDlgItemTextA(IDC_EDIT2, notSigFileName);
	notSigFileName.Trim();
	sigFileName.Trim();
	if (sigFileName.IsEmpty() || notSigFileName.IsEmpty())
	{
		AfxMessageBox(_T("文件路径不能为空!!!"));
		return;
	}

	//打开文件
	PCHAR sigPath = sigFileName.GetBuffer();
	PCHAR notsigPath = notSigFileName.GetBuffer();

	

	do 
	{
		char * sigMem = NULL;
		char * notSigMem = NULL;
		FILE * sigFile = NULL;
		FILE * notSigFile = NULL;
		fopen_s(&sigFile, sigPath, "rb");
		if (!sigFile)
		{
			AfxMessageBox(_T("打开签名文件失败!!!"));
			break;
		}

		fseek(sigFile, 0, SEEK_END);
		size_t sigFileLength = ftell(sigFile);
		rewind(sigFile);

		sigMem = (char *)malloc(sigFileLength);
		memset(sigMem, 0, sigFileLength);
		fread(sigMem, sigFileLength, 1, sigFile);
		fclose(sigFile);

		fopen_s(&notSigFile, notsigPath, "rb");
		if (!notSigFile)
		{
			free(sigMem);
			AfxMessageBox(_T("打开未签名文件失败!!!"));
			break;
		}

		//获取未签名数据
		fseek(notSigFile, 0, SEEK_END);
		size_t notSigFileLength = ftell(notSigFile);
		rewind(notSigFile);
		
		notSigMem = (char *)malloc(notSigFileLength);
		
		memset(notSigMem, 0, notSigFileLength);
		fread(notSigMem, notSigFileLength, 1, notSigFile);
		fclose(notSigFile);

		//开始获取签名目录
		PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)sigMem;
		PIMAGE_NT_HEADERS pNts = (PIMAGE_NT_HEADERS)(sigMem+ pDos->e_lfanew);
		
		PIMAGE_DOS_HEADER pDos1 = (PIMAGE_DOS_HEADER)notSigMem;
		PIMAGE_NT_HEADERS pNts1 = (PIMAGE_NT_HEADERS)(notSigMem + pDos1->e_lfanew);

		if (pNts1->Signature != 0x4550 || pNts->Signature != 0x4550)
		{
			free(sigMem);
			free(notSigMem);
			AfxMessageBox(_T("请打开PE格式的文件!!!"));
			break;
		}

		/*
		if (pNts1->FileHeader.Machine != pNts->FileHeader.Machine)
		{
			free(sigMem);
			free(notSigMem);
			AfxMessageBox(_T("两个PE格式不一致，存在一个是X86 一个是X64 文件"));
			break;
		}
		*/
		PUCHAR fw = NULL;
		ULONG fwisze = NULL;
		ULONG SigVirtualAddress = 0;
		ULONG sigSize = 0;
		if (pNts->OptionalHeader.Magic == 0x20B)
		{
			PIMAGE_NT_HEADERS64 pNts = (PIMAGE_NT_HEADERS64)(sigMem + pDos->e_lfanew);
			PIMAGE_DATA_DIRECTORY Pdir = &pNts->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY];
			SigVirtualAddress = Pdir->VirtualAddress;
			sigSize = Pdir->Size;

			fwisze = notSigFileLength + sigSize;
			fw = (PUCHAR)malloc(fwisze);
			memset(fw, 0, fwisze);
			memcpy(fw, notSigMem, notSigFileLength);

			
		
		}
		else 
		{
			PIMAGE_DOS_HEADER pDos32 = (PIMAGE_DOS_HEADER)sigMem;
			PIMAGE_NT_HEADERS32 pNts32 = (PIMAGE_NT_HEADERS32)(sigMem + pDos32->e_lfanew);
			PIMAGE_DATA_DIRECTORY Pdir = &pNts32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY];
			SigVirtualAddress = Pdir->VirtualAddress;
			sigSize = Pdir->Size;

			fwisze = notSigFileLength + sigSize;
			fw = (PUCHAR)malloc(fwisze);
			memset(fw, 0, fwisze);
			memcpy(fw, notSigMem, notSigFileLength);

			
		}
		
		if (pNts1->OptionalHeader.Magic == 0x20B)
		{

			PIMAGE_DOS_HEADER pDos64w = (PIMAGE_DOS_HEADER)fw;
			PIMAGE_NT_HEADERS64 pNts64w = (PIMAGE_NT_HEADERS64)(fw + pDos64w->e_lfanew);
			PIMAGE_DATA_DIRECTORY Pdirw = &pNts64w->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY];
			PIMAGE_SECTION_HEADER pImageSection = IMAGE_FIRST_SECTION(pNts64w);
			pImageSection += (pNts64w->FileHeader.NumberOfSections - 1);

			ULONG VirtualAddressW = (pImageSection->PointerToRawData + pImageSection->SizeOfRawData);
			memcpy(fw + VirtualAddressW, sigMem + SigVirtualAddress, sigSize);
			Pdirw->VirtualAddress = VirtualAddressW;
			Pdirw->Size = sigSize;
		}
		else 
		{


			PIMAGE_DOS_HEADER pDos32w = (PIMAGE_DOS_HEADER)fw;
			PIMAGE_NT_HEADERS pNts32w = (PIMAGE_NT_HEADERS)(fw + pDos32w->e_lfanew);
			PIMAGE_DATA_DIRECTORY Pdirw = &pNts32w->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY];
			PIMAGE_SECTION_HEADER pImageSection = IMAGE_FIRST_SECTION(pNts32w);
			pImageSection += (pNts32w->FileHeader.NumberOfSections - 1);

			ULONG VirtualAddressW = (pImageSection->PointerToRawData + pImageSection->SizeOfRawData);
			memcpy(fw + VirtualAddressW, sigMem + SigVirtualAddress, sigSize);
			Pdirw->VirtualAddress = VirtualAddressW;
			Pdirw->Size = sigSize;
		}

		free(notSigMem);
		free(sigMem);

		if (fw)
		{
			FILE * file = NULL;
			fileSuffix.Delete(0, fileSuffix.GetLength() - 4);
			NewFileFullPath.Delete(NewFileFullPath.GetLength()-4,4);
			NewFileFullPath.Append("-sign");
			NewFileFullPath.Append(fileSuffix);
			fopen_s(&file, NewFileFullPath,"wb");
			if (!file)
			{
				free(fw);
				break;
			}

			fwrite(fw, fwisze, 1, file);
			fclose(file);
			free(fw);
			AfxMessageBox(_T("借签名成功\r\n"));
		}

	} while (0);

	sigFileName.ReleaseBuffer();
	notSigFileName.ReleaseBuffer();
}
