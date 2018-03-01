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

// TripLightView.cpp : implementation of the CTripLightView class
//

#include "stdafx.h"
#include "TripLight.h"

#include "TripLightDoc.h"
#include "TripLightView.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTripLightView

IMPLEMENT_DYNCREATE(CTripLightView, CView)

/////////////////////////////////////////////////////////////////////////////
// CTripLightView construction/destruction

CTripLightView::CTripLightView()
{
	m_Pause = FALSE;
	m_PrePrintPause = FALSE;
	m_ShowMesh = FALSE;
	m_Backward = FALSE;
	m_ColorSpeedVarDelta = 0;
}

CTripLightView::~CTripLightView()
{
}

BOOL CTripLightView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

int CTripLightView::Rand(int Vals)
{
	if (Vals <= 0)
		return(-1);
	int	i = trunc(rand() / double(RAND_MAX) * Vals);
	return(min(i, Vals - 1));
}

inline double CTripLightView::frand()
{
	return(double(rand()) / RAND_MAX);
}

inline COLOR16 CTripLightView::RandColor()
{
	return(static_cast<COLOR16>(Rand(COLOR_MAX)));
}

inline void CTripLightView::UpdateColor(COLOR16& Chan, int& Delta)
{
	int	val = Chan + Delta;
	if (val < 0 || val > COLOR_MAX) {
		Delta = -Delta;
		val = Chan + Delta;
	}
	Chan = static_cast<COLOR16>(val);
}

inline void CTripLightView::UpdateColorSpeedVariance()
{
	// convert color speed variance from percentage of color speed to color delta
	m_ColorSpeedVarDelta = round(double(m_ColorSpeed) * m_ColorSpeedVar / 100);
}

inline void CTripLightView::UpdateColorDelta(int& Delta) const
{
	int	NewDelta = m_ColorSpeed + Rand(m_ColorSpeedVarDelta + 1);
	if (Delta < 0)
		Delta = -NewDelta;
	else
		Delta = NewDelta;
}

void CTripLightView::UpdateColorDeltas()
{
	UpdateColorSpeedVariance();
	int	nVerts = m_Vert.GetSize();
	for (int iVert = 0; iVert < nVerts; iVert++) {	// for each vertex
		VERTEX_INFO&	info = m_VertInfo[iVert];
		UpdateColorDelta(info.DeltaRed);
		UpdateColorDelta(info.DeltaGreen);
		UpdateColorDelta(info.DeltaBlue);
	}
}

void CTripLightView::CreateMesh() 
{
	m_Vert.RemoveAll();
	UpdateMesh();
}

void CTripLightView::SetGridSize(CSize Size)
{
	m_Cols = Size.cx;
	m_Rows = Size.cy;
	UpdateMesh();
}

void CTripLightView::UpdateMesh()
{
	int	nOldVerts = m_Vert.GetSize();
	int	nVerts = (m_Cols + 1) * (m_Rows + 1);
	m_Vert.SetSize(nVerts);
	m_VertInfo.SetSize(nVerts);
	int	nTris = m_Cols * m_Rows * 2;
	m_Tri.SetSize(nTris);
	for (int iVert = nOldVerts; iVert < nVerts; iVert++) {	// for each vertex
		TRIVERTEX&	vert = m_Vert[iVert];
		VERTEX_INFO&	info = m_VertInfo[iVert];
		vert.Red = RandColor();
		vert.Green = RandColor();
		vert.Blue = RandColor();
		vert.Alpha = 0;
		info.DeltaRed = Rand(2) - 1;
		info.DeltaGreen = Rand(2) - 1;
		info.DeltaBlue = Rand(2) - 1;
		UpdateColorDelta(info.DeltaRed);
		UpdateColorDelta(info.DeltaGreen);
		UpdateColorDelta(info.DeltaBlue);
	}
	ResizeMesh();
}

void CTripLightView::ResizeMesh()
{
	CRect	rc;
	GetClientRect(rc);
	ResizeMesh(rc.Size());
}

void CTripLightView::ResizeMesh(CSize WndSize)
{
	if (!m_Vert.GetSize())
		return;
	int	iVert = 0;
	int	iRow;
	int	nRows = m_Rows;
	int	nCols = m_Cols;
	double	dx = double(WndSize.cx) / nCols;
	double	dy = double(WndSize.cy) / nRows;
	for (iRow = 0; iRow <= nRows; iRow++) {	// for each row
		for (int iCol = 0; iCol <= nCols; iCol++) {	// for each column
			double	x = dx * iCol;
			double	y = dy * iRow;
			const VERTEX_INFO&	info = m_VertInfo[iVert];
			m_Vert[iVert].x = round(x);
			m_Vert[iVert].y = round(y);
			iVert++;
		}
	}
	bool	AltRows = (m_Pattern & ALT_ROWS) != 0;
	bool	AltCols = (m_Pattern & ALT_COLS) != 0;
	bool	RowDiagRL = (m_Pattern & DIAG_RL) != 0;
	int	iTri = 0;
	for (iRow = 0; iRow < nRows; iRow++) {	// for each row
		int	ColDiagRL = RowDiagRL;
		for (int iCol = 0; iCol < nCols; iCol++) {	// for each column
			int	tl = iRow * (nCols + 1) + iCol;
			int	tr = tl + 1;
			int	bl = (iRow + 1) * (nCols + 1) + iCol;
			int	br = bl + 1;
			if (ColDiagRL) {	// if diagonal right to left
				m_Tri[iTri].Vertex1 = tl;
				m_Tri[iTri].Vertex2 = tr;
				m_Tri[iTri].Vertex3 = bl;
				iTri++;
				m_Tri[iTri].Vertex1 = tr;
				m_Tri[iTri].Vertex2 = br;
				m_Tri[iTri].Vertex3 = bl;
				iTri++;
			} else {	// diagonal left to right
				m_Tri[iTri].Vertex1 = tl;
				m_Tri[iTri].Vertex2 = tr;
				m_Tri[iTri].Vertex3 = br;
				iTri++;
				m_Tri[iTri].Vertex1 = tl;
				m_Tri[iTri].Vertex2 = br;
				m_Tri[iTri].Vertex3 = bl;
				iTri++;
			}
			if (AltCols)
				ColDiagRL ^= 1;
		}
		if (AltRows)
			RowDiagRL ^= 1;
	}
	Invalidate();
}

bool CTripLightView::CreateTimer(bool Enable)
{
	if (Enable == m_FrameTimer.IsCreated())
		return(TRUE);
	if (Enable) {
		if (!m_FrameTimer.Create(TIMER_PERIOD, TIMER_RESOLUTION, TimerCallback, W64ULONG(this), TIME_PERIODIC))
			return(FALSE);
	} else {
		if (MIDI_FAILED(m_FrameTimer.Destroy()))
			return(FALSE);
	}
	return(TRUE);
}

LRESULT CTripLightView::OnFrameTimer(WPARAM wParam, LPARAM lParam) 
{
	if (m_Pause)
		return(0);	// nothing to do
	int	nVerts = m_Vert.GetSize();
	for (int iVert = 0; iVert < nVerts; iVert++) {	// for each vertex
		TRIVERTEX&	vert = m_Vert[iVert];
		VERTEX_INFO&	info = m_VertInfo[iVert];
		UpdateColor(vert.Red, info.DeltaRed);
		UpdateColor(vert.Green, info.DeltaGreen);
		UpdateColor(vert.Blue, info.DeltaBlue);
	}
//	ResizeMesh();
	Invalidate();
//	DumpState();
	return(0);
}

bool CTripLightView::SetPause(bool Enable)
{
	if (!CreateTimer(!Enable))
		return(FALSE);
	m_Pause = Enable;
	return(TRUE);
}

void CTripLightView::DumpState()
{
	int	nVerts = m_Vert.GetSize();
	for (int iVert = 0; iVert < nVerts; iVert++) {	// for each vertex
		VERTEX_INFO&	info = m_VertInfo[iVert];
		printf("(%d %d %d)", info.DeltaRed, info.DeltaGreen, info.DeltaBlue);
	}
	printf("\n");
}

void CTripLightView::SetColorSpeed(int Speed)
{
	Speed = CLAMP(Speed, SPEED_MIN, SPEED_MAX);
	if (Speed == m_ColorSpeed)
		return;	// nothing to do
	m_ColorSpeed = Speed;
	UpdateColorDeltas();
}

void CTripLightView::SetColorSpeedVariance(int Variance)
{
	Variance = CLAMP(Variance, SPEED_MIN, SPEED_MAX);
	if (Variance == m_ColorSpeedVar)
		return;	// nothing to do
	m_ColorSpeedVar = Variance;
	UpdateColorDeltas();
}

void CTripLightView::ReverseColorDeltas()
{
	int	nVerts = m_Vert.GetSize();
	for (int iVert = 0; iVert < nVerts; iVert++) {	// for each vertex
		VERTEX_INFO&	info = m_VertInfo[iVert];
		info.DeltaRed = -info.DeltaRed;
		info.DeltaGreen = -info.DeltaGreen;
		info.DeltaBlue = -info.DeltaBlue;
	}
}

void CTripLightView::SetBackward(bool Enable)
{
	if (Enable == m_Backward)
		return;
	ReverseColorDeltas();
	m_Backward = Enable;
}

/////////////////////////////////////////////////////////////////////////////
// CTripLightView message map

BEGIN_MESSAGE_MAP(CTripLightView, CView)
	//{{AFX_MSG_MAP(CTripLightView)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_IMAGE_COLS_LESS, OnImageColsLess)
	ON_COMMAND(ID_IMAGE_COLS_MORE, OnImageColsMore)
	ON_COMMAND(ID_IMAGE_ROWS_LESS, OnImageRowsLess)
	ON_COMMAND(ID_IMAGE_ROWS_MORE, OnImageRowsMore)
	ON_COMMAND(ID_IMAGE_SPEED_FASTER, OnImageColorSpeedFaster)
	ON_COMMAND(ID_IMAGE_SPEED_SLOWER, OnImageColorSpeedSlower)
	ON_COMMAND(ID_IMAGE_PAUSE, OnImagePause)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_PAUSE, OnUpdateImagePause)
	ON_COMMAND(ID_IMAGE_VARIANCE_MORE, OnImageColorSpeedVarianceMore)
	ON_COMMAND(ID_IMAGE_VARIANCE_LESS, OnImageColorSpeedVarianceLess)
	ON_WM_CREATE()
	ON_COMMAND(ID_IMAGE_COLOR_JUMP, OnImageColorJump)
	ON_COMMAND(ID_IMAGE_SHOW_MESH, OnImageShowMesh)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_SHOW_MESH, OnUpdateImageShowMesh)
	ON_COMMAND(ID_IMAGE_BACKWARD, OnImageBackward)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_BACKWARD, OnUpdateImageBackward)
	ON_COMMAND(ID_IMAGE_SINGLE_STEP, OnImageSingleStep)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_SINGLE_STEP, OnUpdateImageSingleStep)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ID_IMAGE_PATTERN1, ID_IMAGE_PATTERN8, OnImagePattern) 
	ON_UPDATE_COMMAND_UI_RANGE(ID_IMAGE_PATTERN1, ID_IMAGE_PATTERN8, OnUpdateImagePattern) 
	ON_MESSAGE(UWM_FRAME_TIMER, OnFrameTimer)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTripLightView drawing

void CTripLightView::OnDraw(CDC* pDC)
{
	CTripLightDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (m_ShowMesh) {	// if showing mesh
		int	nTris = m_Tri.GetSize();
		for (int iTri = 0; iTri < nTris; iTri++) {	// for each triangle
			const GRADIENT_TRIANGLE& tri = m_Tri[iTri];
			CPoint	pa[3] = {
				CPoint(m_Vert[tri.Vertex1].x, m_Vert[tri.Vertex1].y),
				CPoint(m_Vert[tri.Vertex2].x, m_Vert[tri.Vertex2].y),
				CPoint(m_Vert[tri.Vertex3].x, m_Vert[tri.Vertex3].y),
			};
			pDC->Polygon(pa, _countof(pa));
		}
	} else {	// not showing mesh; fill triangles
		GradientFill(*pDC, m_Vert.GetData(), m_Vert.GetSize(), 
			m_Tri.GetData(), m_Tri.GetSize(), GRADIENT_FILL_TRIANGLE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTripLightView printing

BOOL CTripLightView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CTripLightView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	m_PrePrintPause = m_Pause;
	m_Pause = TRUE;
}

void CTripLightView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	m_Pause = m_PrePrintPause;
	GetParentFrame()->RecalcLayout();
}

void CTripLightView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	MoveWindow(pInfo->m_rectDraw);
	CView::OnPrint(pDC, pInfo);
}

/////////////////////////////////////////////////////////////////////////////
// CTripLightView diagnostics

#ifdef _DEBUG
void CTripLightView::AssertValid() const
{
	CView::AssertValid();
}

void CTripLightView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTripLightDoc* CTripLightView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTripLightDoc)));
	return (CTripLightDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTripLightView message handlers

int CTripLightView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	STATIC_DOWNCAST(CFrameWnd, AfxGetMainWnd())->SetActiveView(this);
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (MIDI_FAILED(m_TimerPeriod.Create(1))) {
		AfxMessageBox(_T("Can't create timer period."));
		return FALSE;
	}
	if (!CreateTimer(TRUE)) {
		AfxMessageBox(_T("Can't create frame timer."));
		return FALSE;
	}
	srand(GetTickCount());	// set random number seed to system time so every run is different
	return 0;
}

void CTripLightView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	CTripLightDoc	*pDoc = GetDocument();
	CTripLightParams&	params = *this;	// upcast to base struct
	params = *pDoc;	// copy document's parameters to ours
	UpdateColorSpeedVariance();
	CreateMesh();
}

BOOL CTripLightView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CTripLightView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	if (m_Vert.GetSize())
		ResizeMesh(CSize(cx, cy));
}

void CALLBACK CTripLightView::TimerCallback(UINT uID, UINT uMsg, W64ULONG dwUser, W64ULONG dw1, W64ULONG dw2)
{
	CTripLightView	*pDlg = (CTripLightView *)dwUser;
	pDlg->PostMessage(UWM_FRAME_TIMER);
}

void CTripLightView::OnImageColsLess() 
{
	if (m_Cols > 1) {
		m_Cols--;
		UpdateMesh();
		GetDocument()->SetModifiedFlag();
	}
}

void CTripLightView::OnImageColsMore() 
{
	if (m_Cols < COLS_MAX) {
		m_Cols++;
		UpdateMesh();
		GetDocument()->SetModifiedFlag();
	}
}

void CTripLightView::OnImageRowsLess() 
{
	if (m_Rows > 1) {
		m_Rows--;
		UpdateMesh();
		GetDocument()->SetModifiedFlag();
	}
}

void CTripLightView::OnImageRowsMore() 
{
	if (m_Rows < ROWS_MAX) {
		m_Rows++;
		UpdateMesh();
		GetDocument()->SetModifiedFlag();
	}
}

void CTripLightView::OnImagePattern(UINT nID)
{
	int	iPat = nID - ID_IMAGE_PATTERN1;
	ASSERT(iPat >= 0 && iPat < PATTERNS);
	m_Pattern = iPat;
	ResizeMesh();
	GetDocument()->SetModifiedFlag();
}

void CTripLightView::OnUpdateImagePattern(CCmdUI* pCmdUI)
{
	int	iPat = pCmdUI->m_nID - ID_IMAGE_PATTERN1;
	ASSERT(iPat >= 0 && iPat < PATTERNS);
	pCmdUI->SetRadio(m_Pattern == iPat);
}

void CTripLightView::OnImageColorSpeedFaster() 
{
	SetColorSpeed(m_ColorSpeed + m_ColorSpeedStep);
	GetDocument()->SetModifiedFlag();
}

void CTripLightView::OnImageColorSpeedSlower() 
{
	SetColorSpeed(m_ColorSpeed - m_ColorSpeedStep);
	GetDocument()->SetModifiedFlag();
}

void CTripLightView::OnImageColorSpeedVarianceMore() 
{
	SetColorSpeedVariance(m_ColorSpeedVar + m_ColorSpeedVarStep);
	GetDocument()->SetModifiedFlag();
}

void CTripLightView::OnImageColorSpeedVarianceLess() 
{
	SetColorSpeedVariance(m_ColorSpeedVar - m_ColorSpeedVarStep);
	GetDocument()->SetModifiedFlag();
}

void CTripLightView::OnImagePause() 
{
	SetPause(!m_Pause);
}

void CTripLightView::OnUpdateImagePause(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_Pause);
}

void CTripLightView::OnImageColorJump() 
{
	int	nVerts = m_Vert.GetSize();
	for (int iVert = 0; iVert < nVerts; iVert++) {	// for each vertex
		TRIVERTEX&	vert = m_Vert[iVert];
		vert.Red = RandColor();
		vert.Green = RandColor();
		vert.Blue = RandColor();
	}
	Invalidate();
}

void CTripLightView::OnImageShowMesh() 
{
	m_ShowMesh ^= 1;
}

void CTripLightView::OnUpdateImageShowMesh(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_ShowMesh);
}

void CTripLightView::OnImageBackward() 
{
	SetBackward(!m_Backward);
}

void CTripLightView::OnUpdateImageBackward(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_Backward);
}

void CTripLightView::OnImageSingleStep() 
{
	m_Pause = FALSE;
	OnFrameTimer(0, 0);
	m_Pause = TRUE;
}

void CTripLightView::OnUpdateImageSingleStep(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_Pause);
}
