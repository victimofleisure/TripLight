#include <FFGL.h>
#include <FFGLLib.h>
#include "TripLightFF.h"
#include <math.h>

#define MAX_COLOR_DELTA 0.25f

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Plugin information
////////////////////////////////////////////////////////////////////////////////////////////////////

static CFFGLPluginInfo PluginInfo (
	FFGLGradients::CreateInstance,	// Create method
	"TRLT",								// Plugin unique ID
	"TripLight",						// Plugin name
	1,									// API major version number
	000,								// API minor version number
	1,									// Plugin major version number
	000,								// Plugin minor version number
	FF_SOURCE,							// Plugin type
	"TripLight plugin",					// Plugin description
	"by Chris Korda whorld.org"			// About
);

const LPCTSTR FFGLGradients::m_ParamName[FFPARAMS] = {
	"Rows",
	"Columns",
	"Pattern",
	"Speed",
	"Variance",
	"Scale",
	"Jump",
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Constructor and destructor
////////////////////////////////////////////////////////////////////////////////////////////////////

FFGLGradients::FFGLGradients()
:CFreeFrameGLPlugin()
{
	SetMinInputs(0);
	SetMaxInputs(0);
	// Parameters
	m_Rows = 3;
	m_Cols = 4;
	m_Pattern = 2;
	m_ColorSpeedFine = 0.1f;
	m_ColorSpeedVariance = 0.06f;
	m_ColorSpeedScale = MAX_COLOR_DELTA / 4;
	m_ColorSpeed = 0;
	m_Update = UPDATE_MESH;

//	Win32Console::Create();

	for (int iParam = 0; iParam < FFPARAMS; iParam++) {
		float	fParam = GetNormParameter(iParam);
		SetParamInfo(iParam, m_ParamName[iParam], FF_TYPE_STANDARD, fParam);
	};
	srand(GetTickCount());
}

DWORD FFGLGradients::InitGL(const FFGLViewportStruct *vp)
{
	return FF_SUCCESS;
}

DWORD FFGLGradients::DeInitGL()
{
	return FF_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Methods
////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD FFGLGradients::ProcessOpenGL(ProcessOpenGLStruct *pGL)
{
	if (m_Update) {
		if (m_Update & UPDATE_MESH)
			UpdateMesh();
		else if (m_Update & RESIZE_MESH)
			ResizeMesh();
		m_Update = 0;
	}
	glShadeModel(GL_SMOOTH);
	int	nTris = m_Tri.size();
	for (int iTri = 0; iTri < nTris; iTri++) {
		TRI&	tri = m_Tri[iTri];
		glBegin(GL_POLYGON);
		for (int iPt = 0; iPt < _countof(tri.vi); iPt++) {
			int	iVert = tri.vi[iPt];
			VERT&	vert = m_Vert[iVert];
			glColor3f(vert.color.r, vert.color.g, vert.color.b);
			glVertex2f(vert.pt.x, vert.pt.y);
		}
		glEnd();
	}
	int	nVerts = m_Vert.size();
	for (int iVert = 0; iVert < nVerts; iVert++) {
		VERT&	vert = m_Vert[iVert];
		for (int iChan = 0; iChan < COLOR_CHANS; iChan++) {
			UpdateColor(vert.color.c[iChan], vert.delta.c[iChan]);
		}
	}
	return FF_SUCCESS;
}

DWORD FFGLGradients::GetParameter(DWORD dwIndex)
{
	if (dwIndex >= FFPARAMS)
		return FF_FAIL;
	return m_Param[dwIndex];
}

float FFGLGradients::GetNormParameter(DWORD dwIndex) const
{
	switch (dwIndex) {
	case FFPARAM_ROWS:
		return EnumToNorm(m_Rows - 1, MAX_ROWS);
	case FFPARAM_COLS:
		return EnumToNorm(m_Cols - 1, MAX_COLS);
	case FFPARAM_PATTERN:
		return EnumToNorm(m_Pattern, PATTERNS);
	case FFPARAM_SPEED:
		return m_ColorSpeedFine;
	case FFPARAM_VARIANCE:
		return m_ColorSpeedVariance;
	case FFPARAM_SCALE:
		return m_ColorSpeedScale / MAX_COLOR_DELTA;
	case FFPARAM_JUMP:
		return 0;
	default:
		return 0;
	}
}

DWORD FFGLGradients::SetParameter(const SetParameterStruct* pParam)
{
	if (pParam == NULL)
		return FF_FAIL;
	DWORD	iParam = pParam->ParameterNumber;
	DWORD	dwParam = pParam->NewParameterValue;
	switch (iParam) {
	case FFPARAM_ROWS:
		m_Rows = NormToEnum(UnpackParam(dwParam), MAX_ROWS) + 1;
		m_Update |= UPDATE_MESH;
		break;
	case FFPARAM_COLS:
		m_Cols = NormToEnum(UnpackParam(dwParam), MAX_COLS) + 1;
		m_Update |= UPDATE_MESH;
		break;
	case FFPARAM_PATTERN:
		m_Pattern = NormToEnum(UnpackParam(dwParam), PATTERNS);
		m_Update |= RESIZE_MESH;
		break;
	case FFPARAM_SPEED:
		m_ColorSpeedFine = UnpackParam(dwParam);
		UpdateColorDeltas();
		break;
	case FFPARAM_VARIANCE:
		m_ColorSpeedVariance = UnpackParam(dwParam);
		UpdateColorDeltas();
		break;
	case FFPARAM_SCALE:
		m_ColorSpeedScale = UnpackParam(dwParam) * MAX_COLOR_DELTA;
		UpdateColorDeltas();
		break;
	case FFPARAM_JUMP:
		if (dwParam && !m_Param[iParam])	// if positive transition
			RandomizeColors();
		break;
	default:
		return FF_FAIL;
	}
	m_Param[iParam] = dwParam;	// update shadow value
	return FF_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Utils
////////////////////////////////////////////////////////////////////////////////////////////////////

inline int FFGLGradients::round(double x)
{
	if (x > 0)
		return int(x + 0.5);
	else
		return int(x - 0.5);
}

inline int FFGLGradients::trunc(double x)
{
	return int(x);
}

int FFGLGradients::Rand(int Vals)
{
	if (Vals <= 0)
		return -1;
	int	i = trunc(rand() / double(RAND_MAX) * Vals);
	return min(i, Vals - 1);
}

inline float FFGLGradients::FltRand()
{
	return float(rand()) / RAND_MAX;
}

inline float FFGLGradients::UnpackParam(DWORD dwParam)
{
	return *((float *)(unsigned)&(dwParam));
}

inline DWORD FFGLGradients::PackParam(float fParam)
{
	return *((DWORD *)(unsigned)&(fParam));
}

inline int FFGLGradients::NormToEnum(float fVal, int nVals)
{
	int	iVal = trunc(fVal * nVals);
	return min(iVal, nVals - 1);
}

inline float FFGLGradients::EnumToNorm(int iVal, int nVals)
{
	return (iVal + 0.5f) / nVals;
}

inline void FFGLGradients::SetVertices(TRI& Tri, int i0, int i1, int i2)
{
	Tri.vi[0] = i0;
	Tri.vi[1] = i1;
	Tri.vi[2] = i2;
}

inline void FFGLGradients::UpdateColor(float& Chan, float& Delta)
{
	float	val = Chan + Delta;
	if (val < 0 || val > 1) {	// if new color value out of range
		Delta = -Delta;	// invert delta
		val = Chan + Delta;	// reverse course
	}
	Chan = val;
}

inline void FFGLGradients::UpdateColorDelta(float& Delta) const
{
	float	NewDelta = m_ColorSpeed + m_ColorSpeed * m_ColorSpeedVariance * FltRand();
	if (Delta < 0)	// retain previous delta's sign
		Delta = -NewDelta;
	else
		Delta = NewDelta;
}

void FFGLGradients::UpdateColorDeltas()
{
	m_ColorSpeed = m_ColorSpeedFine * m_ColorSpeedScale;
	int	nVerts = m_Vert.size();
	for (int iVert = 0; iVert < nVerts; iVert++) {	// for each vertex
		VERT&	vert = m_Vert[iVert];
		for (int iChan = 0; iChan < COLOR_CHANS; iChan++) {	// for each channel
			UpdateColorDelta(vert.delta.c[iChan]);
		}
	}
}

void FFGLGradients::RandomizeColors()
{
	int	nVerts = m_Vert.size();
	for (int iVert = 0; iVert < nVerts; iVert++) {	// for each vertex
		VERT&	vert = m_Vert[iVert];
		for (int iChan = 0; iChan < COLOR_CHANS; iChan++) {	// for each channel
			vert.color.c[iChan] = FltRand();
		}
	}
}

void FFGLGradients::CreateMesh()
{
	m_Vert.resize(0);
	UpdateMesh();
}

void FFGLGradients::UpdateMesh()
{
	int	nOldVerts = m_Vert.size();
	int	nVerts = (m_Cols + 1) * (m_Rows + 1);
	m_Vert.resize(nVerts);	// resize vertex array
	int	nTris = m_Cols * m_Rows * 2;
	m_Tri.resize(nTris);	// resize triangle array
	for (int iVert = nOldVerts; iVert < nVerts; iVert++) {	// for each vertex
		VERT&	vert = m_Vert[iVert];
		for (int iChan = 0; iChan < COLOR_CHANS; iChan++) {	// for each channel
			vert.color.c[iChan] = FltRand();	// random value
			vert.delta.c[iChan] = float(Rand(2) - 1);	// random direction
			UpdateColorDelta(vert.delta.c[iChan]);
		}
	}
	ResizeMesh();
}

void FFGLGradients::ResizeMesh()
{
	if (!m_Vert.size())
		return;
	int	iVert = 0;
	int	iRow;
	int	nRows = m_Rows;
	int	nCols = m_Cols;
	double	dx = 2.0 / nCols;
	double	dy = 2.0 / nRows;
	for (iRow = 0; iRow <= nRows; iRow++) {	// for each row
		for (int iCol = 0; iCol <= nCols; iCol++) {	// for each column
			VERT&	vert = m_Vert[iVert];
			double	x = dx * iCol - 1;
			double	y = dy * iRow - 1;
			vert.pt.x = float(x);
			vert.pt.y = float(y);
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
			int	tl = iRow * (nCols + 1) + iCol;	// top left
			int	tr = tl + 1;	// top right
			int	bl = (iRow + 1) * (nCols + 1) + iCol;	// bottom left
			int	br = bl + 1;	// bottom right
			if (ColDiagRL) {	// if diagonal right to left
				SetVertices(m_Tri[iTri], bl, br, tl);
				iTri++;
				SetVertices(m_Tri[iTri], br, tr, tl);
				iTri++;
			} else {	// diagonal left to right
				SetVertices(m_Tri[iTri], bl, br, tr); 
				iTri++;
				SetVertices(m_Tri[iTri], bl, tr, tl);
				iTri++;
			}
			if (AltCols)
				ColDiagRL ^= 1;
		}
		if (AltRows)
			RowDiagRL ^= 1;
	}
//	DumpState();
}

void FFGLGradients::DumpState()
{
	int	nVerts = m_Vert.size();
	for (int iVert = 0; iVert < nVerts; iVert++) {	// for each vertex
		VERT&	vert = m_Vert[iVert];
		printf("%d:(%.3f %.3f)(%.3f %.3f %.3f)(%.3f %.3f %.3f)\n", 
			iVert, vert.pt.x, vert.pt.y, 
			vert.color.r, vert.color.g, vert.color.b,
			vert.delta.r, vert.delta.g, vert.delta.b);
	}
	printf("\n");
}
