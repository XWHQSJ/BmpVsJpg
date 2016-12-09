
// Change_Bmp_JpgDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Change_Bmp_Jpg.h"
#include "Change_Bmp_JpgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CChange_Bmp_JpgDlg 对话框




CChange_Bmp_JpgDlg::CChange_Bmp_JpgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChange_Bmp_JpgDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChange_Bmp_JpgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FILEPATH, m_editFilePath);
	DDX_Control(pDX, IDC_COMBO_CHANGEMODE, m_boxChangeMode);
	DDX_Control(pDX, IDC_EDIT_DESTPATH, m_editOutFilePath);
	DDX_Control(pDX, IDC_COMBO_VALUE, m_qualityValue);
}

BEGIN_MESSAGE_MAP(CChange_Bmp_JpgDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_CHOOSE, &CChange_Bmp_JpgDlg::OnBnClickedButtonChoose)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE, &CChange_Bmp_JpgDlg::OnBnClickedButtonChange)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSEOUT, &CChange_Bmp_JpgDlg::OnBnClickedButtonChooseout)
	ON_CBN_SELCHANGE(IDC_COMBO_CHANGEMODE, &CChange_Bmp_JpgDlg::OnCbnSelchangeComboChangemode)
END_MESSAGE_MAP()


// CChange_Bmp_JpgDlg 消息处理程序

BOOL CChange_Bmp_JpgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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




	m_boxChangeMode.AddString(_T("JPG转换为BMP"));
	m_boxChangeMode.AddString(_T("BMP转换为JPG"));
	m_boxChangeMode.SetCurSel(0);

	CString strText;

	for(int i = 0; i < 101; i++)
	{
		strText.Format(_T("%d"), i);		//将数字强制转换为字符串，方便后面的输出
		m_qualityValue.AddString(strText);
	}
	m_qualityValue.SetCurSel(100);

	GetDlgItem(IDC_STATIC_TEXT)->ShowWindow(SW_HIDE);
	m_qualityValue.ShowWindow(SW_HIDE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CChange_Bmp_JpgDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CChange_Bmp_JpgDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CChange_Bmp_JpgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CChange_Bmp_JpgDlg::OnBnClickedButtonChoose()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE, _T("bin"),NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T(" Files (*.jpg)|*.jpg|(*.bmp)|*.bmp|All Files (*.*)|*.*||"),this);
	if (dlg.DoModal() == IDOK)
	{
		m_strPath = dlg.GetPathName();  //获取文件名称与路径
		m_editFilePath.SetWindowText(m_strPath);
	}
	else
	{
		return;
	}
}

void CChange_Bmp_JpgDlg::OnBnClickedButtonChooseout()
{
	// TODO: 在此添加控件通知处理程序代码

	CFileDialog dlg(TRUE, _T("bin"),NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T(" Files (*.jpg)|*.jpg|(*.bmp)|*.bmp|All Files (*.*)|*.*||"),this);
	if (dlg.DoModal() == IDOK)
	{
		m_strOutPath = dlg.GetPathName();  //获取文件名称与路径
		m_editOutFilePath.SetWindowText(m_strOutPath);
	}
	else
	{
		return;
	}
}

void CChange_Bmp_JpgDlg::OnBnClickedButtonChange()
{
	// TODO: 在此添加控件通知处理程序代码

	int iSelect = m_boxChangeMode.GetCurSel();

	CString strInPath; 
	m_editFilePath.GetWindowText(strInPath);
	CString strOutPath;
	m_editOutFilePath.GetWindowText(strOutPath);
	CString strCheck1 = strInPath.Right(3);
	CString strCheck2 = strOutPath.Right(3);
	strCheck1.MakeUpper();
	strCheck2.MakeUpper();

	int iRet = 0;
	if(strInPath == _T("") || strOutPath == _T(""))
	{
		AfxMessageBox(_T("文件路径不能为空"));
		return;
	}
	if(0 == iSelect && strCheck1 == _T("JPG") && strCheck2 == _T("BMP")) //并判断文件后缀
	{
		//jpg转bmp
		iRet = m_jpgToBmp.LoadJpegFile(strInPath.GetBuffer(0), strOutPath.GetBuffer(0));
	}
	else if(1 == iSelect && strCheck1 == _T("BMP") && strCheck2 == _T("JPG")) //并判断文件后缀
	{
		//bmp转jpg
		int iValue = m_qualityValue.GetCurSel();
		iRet = m_bmpToJpg.BMPToJPG(strInPath.GetBuffer(0), strOutPath.GetBuffer(0), iValue);
	}
	else
	{
		AfxMessageBox(_T("请检查转换类型是否正确"));
		return;
	}


	strInPath.ReleaseBuffer();
	strOutPath.ReleaseBuffer();

	if(iRet == 1)
	{
		AfxMessageBox(_T("转换成功！"));
	}
	else
	{
		AfxMessageBox(_T("转换失败！"));
	}
}



void CChange_Bmp_JpgDlg::OnCbnSelchangeComboChangemode()
{
	// TODO: 在此添加控件通知处理程序代码
	int iSelect = m_boxChangeMode.GetCurSel();
	
	if(0 == iSelect)
	{
		GetDlgItem(IDC_STATIC_TEXT)->ShowWindow(SW_HIDE);
		m_qualityValue.ShowWindow(SW_HIDE);
	}
	else if(1 == iSelect)
	{
		GetDlgItem(IDC_STATIC_TEXT)->ShowWindow(SW_SHOW);
		m_qualityValue.ShowWindow(SW_SHOW);
	}

}
