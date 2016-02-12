#ifndef TRIPLIGHTFF_H
#define TRIPLIGHTFF_H

#include "FFGLPluginSDK.h"
#include "FFGLExtensions.h"
#include <vector>


class FFGLGradients : public CFreeFrameGLPlugin
{
public:
	FFGLGradients();
  ~FFGLGradients() {}

	///////////////////////////////////////////////////
	// FreeFrame plugin methods
	///////////////////////////////////////////////////
	
	DWORD	SetParameter(const SetParameterStruct* pParam);		
	DWORD	GetParameter(DWORD dwIndex);					
	DWORD	ProcessOpenGL(ProcessOpenGLStruct* pGL);
	DWORD   InitGL(const FFGLViewportStruct *vp);
	DWORD   DeInitGL();

	///////////////////////////////////////////////////
	// Factory method
	///////////////////////////////////////////////////

	static DWORD __stdcall CreateInstance(CFreeFrameGLPlugin **ppOutInstance)
  {
  	*ppOutInstance = new FFGLGradients();
	  if (*ppOutInstance != NULL)
      return FF_SUCCESS;
	  return FF_FAIL;
  }


protected:	
// Constants
	enum {	// freeframe parameters
		FFPARAM_ROWS,			// number of rows
		FFPARAM_COLS,			// number of columns
		FFPARAM_PATTERN,		// pattern; see enum below
		FFPARAM_SPEED,			// fine color cycling rate, as percentage of coarse rate
		FFPARAM_VARIANCE,		// percentage of random variation in color cycling rate
		FFPARAM_SCALE,			// coarse color cycling rate; upper limit of fine rate
		FFPARAM_JUMP,			// on positive transition, jump to random colors
		FFPARAMS
	};
	enum {	// pattern bitmasks
		DIAG_RL		= 0x1,		// diagonal top right to bottom left
		ALT_ROWS	= 0x2,		// reverse diagonal for each row
		ALT_COLS	= 0x4,		// reverse diagonal for each column
		PATTERNS	= 8,		// total number of patterns
	};
	enum {
		MAX_ROWS = 20,			// maximum number of rows
		MAX_COLS = 20,			// maximum number of columns
		COLOR_CHANS = 3,		// number of color channels
	};
	enum {	// update flags
		RESIZE_MESH = 0x01,		// next frame should resize mesh
		UPDATE_MESH = 0x02,		// next frame should update mesh; implies resize
	};

// Types
	union COLOR {	// float color
		struct {
			float	r;		// red channel
			float	g;		// green channel
			float	b;		// blue channel
		};
		float	c[COLOR_CHANS];	// array of channels
	};
	union FPOINT {	// float 2D point
		struct {
			float	x;		// x-coordinate
			float	y;		// y-coordinate
		};
		float	a[2];	// array of coordinates
	};
	struct VERT {	// vertex info
		FPOINT	pt;			// vertex coordinates
		COLOR	color;		// vertex color
		COLOR	delta;		// vertex color delta
	};
	struct TRI {	// triangle info
		int		vi[3];		// array of vertex indices
	};

// Data members
	std::vector<VERT>	m_Vert;	// array of vertices
	std::vector<TRI>	m_Tri;	// array of triangles
	int		m_Rows;				// number of rows
	int		m_Cols;				// number of columns
	int		m_Pattern;			// pattern bitmask; see enum
	float	m_ColorSpeedFine;	// fine color speed, multiplied with coarse speed
	float	m_ColorSpeedVariance;	// percentage of random variation in color speed
	float	m_ColorSpeedScale;	// coarse color speed, multiplied with fine speed
	float	m_ColorSpeed;		// product of coarse and fine color speeds
	int		m_Update;			// update bitmask indicating pending tasks; see enum
	static const LPCTSTR m_ParamName[FFPARAMS];	// array of parameter names
	DWORD	m_Param[FFPARAMS];	// array of parameter shadow values

// Helpers
	static	int		round(double x);
	static	int		trunc(double x);
	static	int		Rand(int Vals);
	static	float	FltRand();
	static	float	UnpackParam(DWORD dwParam);
	static	DWORD	PackParam(float fParam);
	static	int		NormToEnum(float fVal, int nVals);
	static	float	EnumToNorm(int iVal, int nVals);
	static	void	SetVertices(TRI& Tri, int i0, int i1, int i2);
	float	GetNormParameter(DWORD dwIndex) const;
	void	UpdateColor(float& Chan, float& Delta);
	void	UpdateColorDelta(float& Delta) const;
	void	UpdateColorDeltas();
	void	RandomizeColors();
	void	CreateMesh();
	void	UpdateMesh();
	void	ResizeMesh();
	void	DumpState();
};

#endif	// TRIPLIGHTFF_H
