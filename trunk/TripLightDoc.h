// Copyleft 2015 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      25dec15	initial version

		TripLight document
 
*/

// TripLightDoc.h : interface of the CTripLightDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRIPLIGHTDOC_H__5E9D9690_8D02_4BBD_98D8_B64C80AAA711__INCLUDED_)
#define AFX_TRIPLIGHTDOC_H__5E9D9690_8D02_4BBD_98D8_B64C80AAA711__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTripLightParams {
public:
	CTripLightParams();
	#define TLPARAMDEF(type, name, init) type m_##name;
	#include "TripLightParams.h"	// generate code to define members
};

class CTripLightDoc : public CDocument, public CTripLightParams
{
protected: // create from serialization only
	CTripLightDoc();
	DECLARE_DYNCREATE(CTripLightDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTripLightDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTripLightDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTripLightDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	int		m_FileVersion;	// file version number
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRIPLIGHTDOC_H__5E9D9690_8D02_4BBD_98D8_B64C80AAA711__INCLUDED_)
