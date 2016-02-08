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

// TripLightDoc.cpp : implementation of the CTripLightDoc class
//

#include "stdafx.h"
#include "TripLight.h"

#include "TripLightDoc.h"
#include "TripLightView.h"
#include "MainFrm.h"
#include "IniFile.h"
#include "RegWrap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTripLightDoc

IMPLEMENT_DYNCREATE(CTripLightDoc, CDocument)

BEGIN_MESSAGE_MAP(CTripLightDoc, CDocument)
	//{{AFX_MSG_MAP(CTripLightDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTripLightDoc construction/destruction

#define RK_FILE_VERSION _T("FileVersion")
#define FILE_VERSION 0

CTripLightDoc::CTripLightDoc()
{
	m_FileVersion = FILE_VERSION;
}

CTripLightDoc::~CTripLightDoc()
{
}

BOOL CTripLightDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	CTripLightParams	DefaultParams;
	CTripLightParams&	params = *this;
	params = DefaultParams;

	return TRUE;
}

CTripLightParams::CTripLightParams()
{
	#define TLPARAMDEF(type, name, init) m_##name = init;
	#include "TripLightParams.h"	// generate code to initialize members
}

/////////////////////////////////////////////////////////////////////////////
// CTripLightDoc serialization

void CTripLightDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTripLightDoc diagnostics

#ifdef _DEBUG
void CTripLightDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTripLightDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTripLightDoc commands

BOOL CTripLightDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	CIniFile	file;
	file.Open(lpszPathName, CFile::modeRead);
	m_FileVersion = theApp.GetProfileInt(theApp.m_pszAppName, RK_FILE_VERSION, FILE_VERSION);
	#define TLPARAMDEF(type, name, init) RdReg(theApp.m_pszAppName, _T(#name), m_##name);
	#include "TripLightParams.h"	// generate code to read members from INI file
	return TRUE;
}

BOOL CTripLightDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{	
	if (!CDocument::OnSaveDocument(lpszPathName))
		return FALSE;
	CIniFile	file;
	file.Open(lpszPathName, CFile::modeCreate | CFile::modeWrite);
	CTripLightParams&	params = *this;
	theApp.GetMain()->GetView()->GetParams(params);
	WrReg(theApp.m_pszAppName, RK_FILE_VERSION, FILE_VERSION);
	#define TLPARAMDEF(type, name, init) WrReg(theApp.m_pszAppName, _T(#name), m_##name);
	#include "TripLightParams.h"	// generate code to write members to INI file
	return TRUE;
}
