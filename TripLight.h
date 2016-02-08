// Copyleft 2015 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      25dec15	initial version

		TripLight application
 
*/

// TripLight.h : main header file for the TRIPLIGHT application
//

#if !defined(AFX_TRIPLIGHT_H__FEF11610_D445_4930_999B_3E6CA90EC20B__INCLUDED_)
#define AFX_TRIPLIGHT_H__FEF11610_D445_4930_999B_3E6CA90EC20B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CTripLightApp:
// See TripLight.cpp for the implementation of this class
//

class CMainFrame;

class CTripLightApp : public CWinApp
{
public:
	CTripLightApp();

// Attributes
	CMainFrame	*GetMain();
	CString	GetAppPath();
	CString	GetAppFolder();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTripLightApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL IsIdleMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CTripLightApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

inline CMainFrame *CTripLightApp::GetMain()
{
	return((CMainFrame *)m_pMainWnd);
}

extern CTripLightApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRIPLIGHT_H__FEF11610_D445_4930_999B_3E6CA90EC20B__INCLUDED_)
