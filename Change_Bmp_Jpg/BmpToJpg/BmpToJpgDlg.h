// BmpToJpgDlg.h : header file
//

#include "StdAfx.h"
#include "BmpToJpg.h"
#include "JpgToBmp.h"

#if !defined(AFX_BMPTOJPGDLG_H__B4783D11_FCD2_4F0D_8988_AEB3F635DF97__INCLUDED_)
#define AFX_BMPTOJPGDLG_H__B4783D11_FCD2_4F0D_8988_AEB3F635DF97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CBmpToJpgDlg dialog

class CBmpToJpgDlg : public CDialog
{
	// Construction
public:
	CBmpToJpgDlg(CWnd* pParent = NULL);	// standard constructor

										// Dialog Data
										//{{AFX_DATA(CBmpToJpgDlg)
	enum { IDD = IDD_BMPTOJPG_DIALOG };

	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBmpToJpgDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
														//}}AFX_VIRTUAL

														// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CBmpToJpgDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonChange();
	afx_msg void OnBnClickedButtonChoose();
	afx_msg void OnBnClickedButtonChooseout();
	afx_msg void OnBnClickedCancel();

public:
	CEdit m_editFilePath;
	CEdit m_editOutFilePath;

	CComboBox m_boxChangeMode;

	CString m_strPath;
	CString m_strOutpath;

	CJpgToBmp	m_jpgToBmp;
	CString m_bmpToJpg;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMPTOJPGDLG_H__B4783D11_FCD2_4F0D_8988_AEB3F635DF97__INCLUDED_)
