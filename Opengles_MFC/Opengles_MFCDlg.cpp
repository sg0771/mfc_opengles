
// Opengles_MFCDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Opengles_MFC.h"
#include "Opengles_MFCDlg.h"
#include "afxdialogex.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// COpenglesMFCDlg 对话框



COpenglesMFCDlg::COpenglesMFCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OPENGLES_MFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COpenglesMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COpenglesMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOADFILE, &COpenglesMFCDlg::OnBnClickedLoadfile)
	ON_BN_CLICKED(IDC_BUTTON1, &COpenglesMFCDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// COpenglesMFCDlg 消息处理程序

BOOL COpenglesMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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


	//_hWnd = this->GetSafeHwnd();//渲染窗口

	HWND hwnd = GetDlgItem(IDC_PIC)->GetSafeHwnd();//渲染窗口
	bool bOInit = m_inst.Init(hwnd);


//	SetTimer(1001, 40,nullptr);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void COpenglesMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void COpenglesMFCDlg::OnPaint()
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
HCURSOR COpenglesMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void COpenglesMFCDlg::OnBnClickedLoadfile()
{
	//m_inst.Draw();
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE);
	if (dlg.DoModal() == IDOK) {
		std::ifstream fin(dlg.GetPathName(), std::ios::binary);
		if (fin.is_open()) {
			// 移动文件指针到文件末尾
			fin.seekg(0, std::ios::end);
			size_t length = fin.tellg(); // 获取当前位置的偏移量，即文件长度

			// 如果需要继续读取文件，可以将指针重置到开头
			fin.seekg(0, std::ios::beg);
			char* tmp = new char[length];
			fin.read(tmp, length);
			fin.close();

			int width = 0, height = 0, channels = 0;
			unsigned char* imageData = stbi_load_from_memory((stbi_uc const* )tmp, length, &width, &height, &channels, 0);

			if (imageData) {
				m_inst.DrawRGB(imageData, width, height, channels, TRUE);
			}

			delete []tmp;
		}
	}
}


void COpenglesMFCDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

	CFileDialog dlg(TRUE);
	if (dlg.DoModal() == IDOK) {
		std::ifstream fin(dlg.GetPathName(), std::ios::binary);
		if (fin.is_open()) {
			// 移动文件指针到文件末尾
			fin.seekg(0, std::ios::end);
			size_t length = fin.tellg(); // 获取当前位置的偏移量，即文件长度

			// 如果需要继续读取文件，可以将指针重置到开头
			fin.seekg(0, std::ios::beg);
			int w = 352;
			int h = 288;
			int size = w * h * 3 / 2;
			if (size > length) {
				AfxMessageBox(L"Read cif.yuv error");

				fin.close();
				return;
			}
			char* tmp = new char[size];
			fin.read(tmp, size);
			fin.close();

			m_inst.DrawYUV((uint8_t*)tmp, w, h, TRUE);
			
			delete[]tmp;
		}
	}

}
