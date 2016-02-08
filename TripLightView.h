// Copyleft 2015 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      25dec15	initial version

		TripLight view
 
*/

// TripLightView.h : interface of the CTripLightView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRIPLIGHTVIEW_H__229B8900_907C_499F_8C06_042D393A65C0__INCLUDED_)
#define AFX_TRIPLIGHTVIEW_H__229B8900_907C_499F_8C06_042D393A65C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ArrayEx.h"
#include "MidiWrap.h"

class CTripLightView : public CView, protected CTripLightParams
{
protected: // create from serialization only
	CTripLightView();
	DECLARE_DYNCREATE(CTripLightView)

// Constants
	enum {	// pattern bitmasks
		DIAG_RL		= 0x1,		// diagonal top right to bottom left
		ALT_ROWS	= 0x2,		// reverse diagonal for each row
		ALT_COLS	= 0x4,		// reverse diagonal for each column
		PATTERNS	= 8,		// total number of patterns
	};

// Attributes
public:
	CTripLightDoc* GetDocument();
	void	GetParams(CTripLightParams& Params) const;
	CSize	GetGridSize() const;
	void	SetGridSize(CSize Size);
	int		GetColorSpeed() const;
	void	SetColorSpeed(int Speed);
	int		GetColorSpeedVariance() const;
	void	SetColorSpeedVariance(int Variance);
	bool	GetPause() const;
	bool	SetPause(bool Enable);
	bool	GetBackward() const;
	void	SetBackward(bool Enable);

// Operations
public:
	void	CreateMesh();
	void	UpdateMesh();
	void	ResizeMesh(CSize WndSize);
	void	ResizeMesh();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTripLightView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTripLightView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTripLightView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnImageColsLess();
	afx_msg void OnImageColsMore();
	afx_msg void OnImageRowsLess();
	afx_msg void OnImageRowsMore();
	afx_msg void OnImageColorSpeedFaster();
	afx_msg void OnImageColorSpeedSlower();
	afx_msg void OnImagePause();
	afx_msg void OnUpdateImagePause(CCmdUI* pCmdUI);
	afx_msg void OnImageColorSpeedVarianceMore();
	afx_msg void OnImageColorSpeedVarianceLess();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnImageColorJump();
	afx_msg void OnImageShowMesh();
	afx_msg void OnUpdateImageShowMesh(CCmdUI* pCmdUI);
	afx_msg void OnImageBackward();
	afx_msg void OnUpdateImageBackward(CCmdUI* pCmdUI);
	afx_msg void OnImageSingleStep();
	afx_msg void OnUpdateImageSingleStep(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnImagePattern(UINT nID);
	afx_msg void OnUpdateImagePattern(CCmdUI* pCmdUI);
	afx_msg LRESULT OnFrameTimer(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

// Constants
	enum {
		TIMER_PERIOD = 10,		// timer period, in milliseconds
		TIMER_RESOLUTION = 1,	// timer resolution, in milliseconds
		ROWS_MAX = SHRT_MAX,	// maximum number of rows
		COLS_MAX = SHRT_MAX,	// maximum number of columns
		SPEED_MIN = 0,			// minimum color speed
		SPEED_MAX = 0x4000,		// maximum color speed
		COLOR_MAX = USHRT_MAX,	// maximum color value
	};

// Types
	struct VERTEX_INFO {
		int		DeltaRed;
		int		DeltaGreen;
		int		DeltaBlue;
	};
	typedef CArrayEx<TRIVERTEX, TRIVERTEX&> CTriVertexArray;
	typedef CArrayEx<GRADIENT_TRIANGLE, GRADIENT_TRIANGLE&> CGradientTriangleArray;
	typedef CArrayEx<VERTEX_INFO, VERTEX_INFO&> CVertexInfoArray;

// Data members
	bool	m_Pause;			// true if paused
	bool	m_PrePrintPause;	// true if paused before printing started
	bool	m_ShowMesh;			// true if showing mesh
	bool	m_Backward;			// true if going backward
	int		m_ColorSpeedVarDelta;	// color speed variation, in color delta per tick
	CMMTimerPeriod	m_TimerPeriod;	// multimedia timer period
	CMMTimer	m_FrameTimer;	// multimedia timer	
	CTriVertexArray	m_Vert;		// array of vertices
	CGradientTriangleArray	m_Tri;	// array of triangles
	CVertexInfoArray	m_VertInfo;	// array of vertex info

// Helpers
	bool	CreateTimer(bool Enable);
	void	UpdateColorDelta(int& Delta) const;
	void	UpdateColorDeltas();
	void	ReverseColorDeltas();
	void	UpdateColorSpeedVariance();
	void	DumpState();
	static	void	CALLBACK TimerCallback(UINT uID, UINT uMsg, W64ULONG dwUser, W64ULONG dw1, W64ULONG dw2);
	static	int		Rand(int Vals);
	static	double	frand();
	static	COLOR16	RandColor();
	static	void	UpdateColor(COLOR16& Color, int& Delta);
};

#ifndef _DEBUG  // debug version in TripLightView.cpp
inline CTripLightDoc* CTripLightView::GetDocument()
   { return (CTripLightDoc*)m_pDocument; }
#endif

inline void CTripLightView::GetParams(CTripLightParams& Params) const
{
	Params = *this;
}

inline CSize CTripLightView::GetGridSize() const
{
	return(CSize(m_Cols, m_Rows));
}

inline int CTripLightView::GetColorSpeed() const
{
	return(m_ColorSpeed);
}

inline int CTripLightView::GetColorSpeedVariance() const
{
	return(m_ColorSpeedVar);
}

inline bool CTripLightView::GetPause() const
{
	return(m_Pause);
}

inline bool CTripLightView::GetBackward() const
{
	return(m_Backward);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRIPLIGHTVIEW_H__229B8900_907C_499F_8C06_042D393A65C0__INCLUDED_)
