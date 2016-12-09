// BmpVsJpgDlg.h : header file
//

#if !defined(AFX_BMPVSJPGDLG_H__E78EA6FB_DCB0_4CDC_B763_A51128457498__INCLUDED_)
#define AFX_BMPVSJPGDLG_H__E78EA6FB_DCB0_4CDC_B763_A51128457498__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CBmpVsJpgDlg dialog

class CBmpVsJpgDlg : public CDialog
{
// Construction
public:
	CBmpVsJpgDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBmpVsJpgDlg)
	enum { IDD = IDD_BMPVSJPG_DIALOG };
	CButton	m_CJpgToBmp;
	CButton	m_CBmpToJpg;
	CString	m_strDest;
	CString	m_strSrc;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBmpVsJpgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CBmpVsJpgDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonfinds();
	afx_msg void OnButtondest();
	afx_msg void OnButtonbegin();
	virtual void OnCancel();
	afx_msg void OnRadiobmptojpg();
	afx_msg void OnRadiojpgtobmp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMPVSJPGDLG_H__E78EA6FB_DCB0_4CDC_B763_A51128457498__INCLUDED_)
