// BmpVsJpgDlg.cpp : implementation file
//
#include "stdafx.h"
#include "BmpVsJpg.h"
#include "BmpVsJpgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "Jpeg.h"
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBmpVsJpgDlg dialog

CBmpVsJpgDlg::CBmpVsJpgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBmpVsJpgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBmpVsJpgDlg)
	m_strDest = _T("");
	m_strSrc = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBmpVsJpgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBmpVsJpgDlg)
	DDX_Control(pDX, IDC_RADIOJPGTOBMP, m_CJpgToBmp);
	DDX_Control(pDX, IDC_RADIOBMPTOJPG, m_CBmpToJpg);
	DDX_Text(pDX, IDC_EDITD, m_strDest);
	DDX_Text(pDX, IDC_EDITS, m_strSrc);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBmpVsJpgDlg, CDialog)
	//{{AFX_MSG_MAP(CBmpVsJpgDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTONFINDS, &CBmpVsJpgDlg::OnButtonfinds)
	ON_BN_CLICKED(IDC_BUTTONDEST, &CBmpVsJpgDlg::OnButtondest)
	ON_BN_CLICKED(IDC_BUTTONBEGIN, &CBmpVsJpgDlg::OnButtonbegin)
	ON_BN_CLICKED(IDC_RADIOBMPTOJPG, &CBmpVsJpgDlg::OnRadiobmptojpg)
	ON_BN_CLICKED(IDC_RADIOJPGTOBMP, &CBmpVsJpgDlg::OnRadiojpgtobmp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBmpVsJpgDlg message handlers

BOOL CBmpVsJpgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_CBmpToJpg.SetCheck(1);
	m_CJpgToBmp.SetCheck(0);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBmpVsJpgDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBmpVsJpgDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBmpVsJpgDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CBmpVsJpgDlg::OnButtonfinds() 
{//查找源文件
	// TODO: Add your control notification handler code here
	if(m_CBmpToJpg.GetCheck())
	{
		CFileDialog fileDialog(TRUE,NULL,NULL,NULL,"BMP文件(*.bmp)|*.bmp|");
		
		if (fileDialog.DoModal() == IDOK) 
		{
			m_strSrc = fileDialog.GetPathName(); 
			UpdateData(FALSE);
		}
	}
	else
	{
		CFileDialog fileDialog(TRUE,NULL,NULL,NULL,"JPG文件(*.jpg)|*.jpg|");
		
		if (fileDialog.DoModal() == IDOK) 
		{
			m_strSrc = fileDialog.GetPathName(); 
			UpdateData(FALSE);
		}
	}
}

void CBmpVsJpgDlg::OnButtondest() 
{//查找目标文件
	// TODO: Add your control notification handler code here
	if(m_CBmpToJpg.GetCheck())
	{
		CFileDialog fileDialog(FALSE,NULL,NULL,NULL,"JPG文件(*.jpg)|*.jpg|");
		
		if (fileDialog.DoModal() == IDOK) 
		{
			m_strDest = fileDialog.GetPathName(); 
			CString str = m_strDest.Right(4);
			str.MakeUpper();
			if(str != ".JPG")
			{
				m_strDest +=".jpg";
			}
			UpdateData(FALSE);
		}
	}
	else
	{
		CFileDialog fileDialog(FALSE,NULL,NULL,NULL,"BMP文件(*.bmp)|*.bmp|");
		
		if (fileDialog.DoModal() == IDOK) 
		{
			m_strDest = fileDialog.GetPathName(); 
			CString str = m_strDest.Right(4);
			str.MakeUpper();
			if(str != ".BMP")
			{
				m_strDest +=".bmp";	
			}
			UpdateData(FALSE);
		}
		
	}
}

void CBmpVsJpgDlg::OnButtonbegin() 
{//开始转换
	// TODO: Add your control notification handler code here
	UpdateData();
	if(m_strSrc == "")
	{
		MessageBox("Please Input The Source File!");
		return;
	}
	if(m_strDest == "")
	{
		MessageBox("Please Input The Destination File!");
		return;
	}
	if(m_CBmpToJpg.GetCheck())
	{//BmpToJpg
		
		BOOL bt;
		CJpeg jj;
		CDib * pDib;
		pDib=new CDib;
		
		int iQuality=100;
		
		// 读取Bmp信息
		CFile file(m_strSrc,CFile::modeRead|CFile::shareDenyNone);
		pDib->Read(&file);
		
		// 保存为Jpg格式
		bt=jj.Save(m_strDest, pDib, TRUE, iQuality);
		delete pDib;
		pDib = NULL;
		
		
	}
	else
	{//JpgToBmp
		
		BOOL bt;
		CJpeg jj;
		CDib * pDib;
		pDib=new CDib;
		
		// 创建CFile对象
		CFile file(m_strDest,CFile::modeCreate | CFile::modeWrite);
		
		// 加载JPG文件内容
		jj.Load(m_strSrc);
		// 获得JPG文件句柄
		HDIB hDIB = CopyHandle(jj.GetDib()->GetHandle());
		if (hDIB == NULL)
			return ;
		// 将DIB对象与JPG文件句柄关联起来
		pDib->Attach(hDIB);

		int iColorBit = 24;//BMP颜色位数 ，24位Bmp / 可以选择1、4、8位	
		pDib->ConvertFormat(iColorBit);	// 24位Bmp
		
		
		// 写BMP文件数据
		bt=pDib->Write(&file);
		
		delete pDib;
		pDib = NULL;
	}
	Beep(1000,100);
}

void CBmpVsJpgDlg::OnCancel() 
{//退出
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}


void CBmpVsJpgDlg::OnRadiobmptojpg() 
{//Bmp转Jpg
	// TODO: Add your control notification handler code here
	m_CBmpToJpg.SetCheck(1);
	m_CJpgToBmp.SetCheck(0);
	m_strDest = "";
	m_strSrc = "";
	UpdateData(FALSE);
}

void CBmpVsJpgDlg::OnRadiojpgtobmp() 
{//Jpg转Bmp
	// TODO: Add your control notification handler code here
	m_CBmpToJpg.SetCheck(0);
	m_CJpgToBmp.SetCheck(1);
	m_strDest = "";
	m_strSrc = "";
	UpdateData(FALSE);
}
