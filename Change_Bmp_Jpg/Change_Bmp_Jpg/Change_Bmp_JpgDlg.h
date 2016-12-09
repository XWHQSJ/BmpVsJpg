
// Change_Bmp_JpgDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "JpgToBmp.h"
#include "BmpToJpg.h"

// CChange_Bmp_JpgDlg 对话框
class CChange_Bmp_JpgDlg : public CDialog
{
// 构造
public:
	CChange_Bmp_JpgDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CHANGE_BMP_JPG_DIALOG };

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
	CEdit m_editFilePath;
	CEdit m_editOutFilePath;

	CComboBox m_boxChangeMode;


	afx_msg void OnBnClickedButtonChoose();
	afx_msg void OnBnClickedButtonChange();
	afx_msg void OnBnClickedButtonChooseout();
	


private:
	CString	m_strPath;
	CString m_strOutPath;
	CJpgToBmp	m_jpgToBmp;
	CBmpToJpg	m_bmpToJpg;
	
public:
	CComboBox m_qualityValue;
	afx_msg void OnCbnSelchangeComboChangemode();
};
