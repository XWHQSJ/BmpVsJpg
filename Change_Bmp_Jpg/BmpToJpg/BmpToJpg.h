// BmpToJpg.h : main header file for the BMPTOJPG application
//

#if !defined(AFX_BMPTOJPG_H__539339D1_1359_442C_9989_00602FBE96AE__INCLUDED_)
#define AFX_BMPTOJPG_H__539339D1_1359_442C_9989_00602FBE96AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CBmpToJpgApp:
// See BmpToJpg.cpp for the implementation of this class
//

class CBmpToJpgApp : public CWinApp
{
public:
	CBmpToJpgApp();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBmpToJpgApp)
public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	// Implementation

	//{{AFX_MSG(CBmpToJpgApp)
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMPTOJPG_H__539339D1_1359_442C_9989_00602FBE96AE__INCLUDED_)
