// BmpToJpgDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "BmpToJpg.h"
#include "BmpToJpgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
// CBmpToJpgDlg dialog

CBmpToJpgDlg::CBmpToJpgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBmpToJpgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBmpToJpgDlg)
	m_strBmp = _T("");
	m_strJpg = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBmpToJpgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBmpToJpgDlg)
	DDX_Control(pDX, IDC_EDIT_STRJPG, m_editFilePath);
	DDX_Control(pDX, IDC_EDIT_STRBMP, m_editOutFilePath);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBmpToJpgDlg, CDialog)
	//{{AFX_MSG_MAP(CBmpToJpgDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDC_BUTTON_CHANGE, &CBmpToJpgDlg::OnBnClickedButtonChange)
	ON_BN_CLICKED(IDC_BUTTON_JPGFILE, &CBmpToJpgDlg::OnBnClickedButtonStrJpg)
	ON_BN_CLICKED(IDC_BUTTON_BMPFILE, &CBmpToJpgDlg::OnBnClickedButtonStrBmp)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, &CBmpToJpgDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBmpToJpgDlg message handlers

BOOL CBmpToJpgDlg::OnInitDialog()
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
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBmpToJpgDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CBmpToJpgDlg::OnPaint() 
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
HCURSOR CBmpToJpgDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

typedef  bool(__stdcall CHANGE)(char *,char *);



void CBmpToJpgDlg::OnBnClickedButtonChange() 
{
	UpdateData(TRUE);

	CString strInPath;
	m_editFilePath.GetWindowText(strInPath);
	CString strOutPath;
	m_editOutFilePath.GetWindowText(strOutPath);
	CString strCheck1 = strInPath.Right(3);
	CString strCheck2 = strOutPath.Right(3);
	strCheck1.MakeUpper();
	strCheck2.MakeUpper();	

	int iRet = 0;
	if (strInPath == _T("") || strOutPath == _T(""))
	{
		AfxMessageBox(_T("The filepath can't be NULL!"));
		return;
	}

	iRet = m_jpgToBmp.LoadJpegFile(strInPath.GetBuffer(0), strOutPath.GetBuffer(0));

	strInPath.ReleaseBuffer();
	strOutPath.ReleaseBuffer();

	if (iRet == 1)
	{
		AfxMessageBox(_T("Change SUCCEED!"));
	}
	else
	{
		AfxMessageBox(_T("Change FAILED!"));
	}

	//HINSTANCE hInst;
	//CHANGE *pFunc;
	//hInst = ::LoadLibrary("BMPDLL.DLL");
	//pFunc = (CHANGE*)::GetProcAddress(hInst, "BmpToJpg");
	//pFunc(m_strJpg.GetBuffer(50), m_strBmp.GetBuffer(50));
	//m_strJpg.ReleaseBuffer();
	//m_strBmp.ReleaseBuffer();
	//::FreeLibrary(hInst);
}

void CBmpToJpgDlg::OnBnClickedButtonStrJpg() 
{
	CFileDialog dlg(TRUE, "jpg", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Jpg File(*.jpg)|*.jpg||");
	if (dlg.DoModal() == IDOK) {
		m_strJpg = dlg.GetPathName();
		m_editFilePath.SetWindowText(m_strJpg);
		UpdateData(FALSE);
	}
}

void CBmpToJpgDlg::OnBnClickedButtonStrBmp() 
{
	CFileDialog dlg(FALSE, "bmp", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Bmp File(*.bmp)|*.bmp||");
	if (dlg.DoModal() == IDOK) {
		m_strBmp = dlg.GetPathName();
		m_editOutFilePath.SetWindowText(m_strBmp);
		UpdateData(FALSE);
	}
}

void CBmpToJpgDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnCancel();
}
