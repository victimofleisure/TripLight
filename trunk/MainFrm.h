// Copyleft 2015 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      25dec15	initial version

		TripLight main window
 
*/

// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__76E6CCA4_E826_4A48_986D_F427C5F09B6D__INCLUDED_)
#define AFX_MAINFRM_H__76E6CCA4_E826_4A48_986D_F427C5F09B6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTripLightView;

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CTripLightView	*GetView();

// Operations
public:
	void	FullScreen(bool Enable);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_StatusBar;
	CToolBar    m_ToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowFullscreen();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnUpdateIndicatorCols(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIndicatorRows(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIndicatorColorSpeed(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIndicatorColorSpeedVariance(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIndicatorPause(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIndicatorBackward(CCmdUI *pCmdUI);
	afx_msg void OnWindowEscape();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Constants
	enum {	// status bar panes
		SBP_HINT,
		SBP_ROWS,
		SBP_COLS,
		SBP_COLOR_SPEED,
		SBP_COLOR_SPEED_VAR,
		SBP_PAUSE,
		SBP_BACKWARD,
		SBP_FRAME_NUMBER,
	};

// Data members
	CTripLightView	*m_pView;	// one and only view
	bool	m_WasShown;			// true if window was shown
	bool	m_FullScreen;		// true if full screen
	bool	m_WasZoomed;		// true if maximized prior to going full screen
	bool	m_ShowToolBar;		// true if tool bar was showing prior to going full screen
	bool	m_ShowStatusBar;	// true if status bar was showing prior to going full screen
	CPoint	m_PrevCursorPos;	// cursor position prior to going full screen
};

inline CTripLightView *CMainFrame::GetView()
{
	return(m_pView);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__76E6CCA4_E826_4A48_986D_F427C5F09B6D__INCLUDED_)
