// BmpVsJpg.h : main header file for the BMPVSJPG application
//

#if !defined(AFX_BMPVSJPG_H__0849D558_B799_4E3A_853E_1E2677198C54__INCLUDED_)
#define AFX_BMPVSJPG_H__0849D558_B799_4E3A_853E_1E2677198C54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CBmpVsJpgApp:
// See BmpVsJpg.cpp for the implementation of this class
//

class CBmpVsJpgApp : public CWinApp
{
public:
	CBmpVsJpgApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBmpVsJpgApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBmpVsJpgApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMPVSJPG_H__0849D558_B799_4E3A_853E_1E2677198C54__INCLUDED_)
