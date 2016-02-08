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

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "TripLight.h"
#include "MainFrm.h"

#include "TripLightDoc.h"
#include "TripLightView.h"
#include "Persist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_COLS,
	ID_INDICATOR_ROWS,
	ID_INDICATOR_COLOR_SPEED,
	ID_INDICATOR_COLOR_SPEED_VAR,
	ID_INDICATOR_PAUSE,
	ID_INDICATOR_BACKWARD,
};

#define RK_MAIN_FRAME		_T("MainFrame")

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_WasShown = FALSE;
	m_FullScreen = FALSE;
	m_WasZoomed = FALSE;
	m_ShowToolBar = FALSE;
	m_ShowStatusBar = FALSE;
	m_PrevCursorPos = CPoint(0, 0);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	theApp.m_pMainWnd = this;	// so view's OnCreate can call SetActiveView
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_pView = STATIC_DOWNCAST(CTripLightView, m_pViewActive);
	ASSERT(m_pView != NULL);
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	if (!m_StatusBar.Create(this) ||
		!m_StatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_ToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_ToolBar);
	DragAcceptFiles();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

void CMainFrame::FullScreen(bool Enable)
{
	if (Enable == m_FullScreen)
		return;
	if (Enable) {	// if entering full screen
		m_WasZoomed = IsZoomed() != 0;	// save maximize state
		m_ShowToolBar = m_ToolBar.IsWindowVisible() != 0;	// save control bar visibility
		m_ShowStatusBar = m_StatusBar.IsWindowVisible() != 0;
		SetMenu(NULL);	// remove menus
		ShowControlBar(&m_StatusBar, FALSE, TRUE);	// hide control bars
		ShowControlBar(&m_ToolBar, FALSE, TRUE);
		ModifyStyle(WS_CAPTION | WS_THICKFRAME | WS_TABSTOP, 0);	// remove frame
		m_pView->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);	// remove view's client edge
		UpdateWindow();	// reduce glitch by applying above changes before resizing
		if (m_WasZoomed)	// if maximized
			ShowWindow(SW_NORMAL);	// must show normal before full screen maximize
		ShowWindow(SW_MAXIMIZE);	// enter full screen
		GetCursorPos(&m_PrevCursorPos);	// save cursor position
		SetCursorPos(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));	// hide cursor
	} else {	// exiting full screen
		ShowWindow(SW_NORMAL);	// exit full screen
		if (m_WasZoomed)	// if previously maximized
			ShowWindow(SW_MAXIMIZE);	// restore maximize state
		ModifyStyle(0, WS_CAPTION | WS_THICKFRAME | WS_TABSTOP);	// restore frame
		m_pView->ModifyStyleEx(0, WS_EX_CLIENTEDGE);	// restore view's client edge
		::SetMenu(m_hWnd, m_hMenuDefault);	// restore menus
		ShowControlBar(&m_StatusBar, m_ShowStatusBar, TRUE);	// restore control bars
		ShowControlBar(&m_ToolBar, m_ShowToolBar, TRUE);
		CPoint	CursorPos;
		GetCursorPos(&CursorPos);
		if (CursorPos.x == GetSystemMetrics(SM_CXSCREEN) - 1	// if cursor still hidden
		&& CursorPos.y == GetSystemMetrics(SM_CYSCREEN) - 1)
			SetCursorPos(m_PrevCursorPos.x, m_PrevCursorPos.y);	// restore cursor position
	}
	m_FullScreen = Enable;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message map

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_FULL_SCREEN, OnWindowFullscreen)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_COLS, OnUpdateIndicatorCols)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ROWS, OnUpdateIndicatorRows)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_COLOR_SPEED, OnUpdateIndicatorColorSpeed)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_COLOR_SPEED_VAR, OnUpdateIndicatorColorSpeedVariance)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_PAUSE, OnUpdateIndicatorPause)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_BACKWARD, OnUpdateIndicatorBackward)
	ON_COMMAND(ID_WINDOW_ESCAPE, OnWindowEscape)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CFrameWnd::OnShowWindow(bShow, nStatus);
	if (!m_WasShown && !IsWindowVisible()) {
		m_WasShown = TRUE;
		CPersist::LoadWnd(REG_SETTINGS, this, RK_MAIN_FRAME, CPersist::NO_MINIMIZE);
	}
}

void CMainFrame::OnClose() 
{
	FullScreen(FALSE);
	CFrameWnd::OnClose();
}

void CMainFrame::OnDestroy() 
{
	CPersist::SaveWnd(REG_SETTINGS, this, RK_MAIN_FRAME);
	CFrameWnd::OnDestroy();
}

void CMainFrame::OnUpdateIndicatorCols(CCmdUI *pCmdUI)
{
	CString	s;
	s.Format(_T("%d"), m_pView->GetGridSize().cx);
	pCmdUI->SetText(s);
}

void CMainFrame::OnUpdateIndicatorRows(CCmdUI *pCmdUI)
{
	CString	s;
	s.Format(_T("%d"), m_pView->GetGridSize().cy);
	pCmdUI->SetText(s);
}

void CMainFrame::OnUpdateIndicatorColorSpeed(CCmdUI *pCmdUI)
{
	CString	s;
	s.Format(_T("%d"), m_pView->GetColorSpeed());
	pCmdUI->SetText(s);
}

void CMainFrame::OnUpdateIndicatorColorSpeedVariance(CCmdUI *pCmdUI)
{
	CString	s;
	s.Format(_T("%d"), m_pView->GetColorSpeedVariance());
	pCmdUI->SetText(s);
}

void CMainFrame::OnUpdateIndicatorPause(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pView->GetPause());
}

void CMainFrame::OnUpdateIndicatorBackward(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pView->GetBackward());
}

void CMainFrame::OnWindowFullscreen() 
{
	FullScreen(!m_FullScreen);
}

void CMainFrame::OnWindowEscape() 
{
	FullScreen(FALSE);
}
