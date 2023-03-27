// Copyleft 2015 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      25dec15	initial version
 		01		15mar23	add MIDI support

		TripLight application
 
*/

// TripLight.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "TripLight.h"

#include "MainFrm.h"
#include "TripLightDoc.h"
#include "TripLightView.h"

#include "Win32Console.h"
#include "VersionInfo.h"
#include "PathStr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CHECK_MIDI(x) { MMRESULT nResult = x; if (MIDI_FAILED(nResult)) { OnMidiError(nResult); return false; } }

/////////////////////////////////////////////////////////////////////////////
// CTripLightApp

BEGIN_MESSAGE_MAP(CTripLightApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTripLightApp construction

CTripLightApp::CTripLightApp()
{
	// Place all significant initialization in InitInstance
	m_iMidiDevIn = -1;
	m_iMidiDevOut = -1;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTripLightApp object

CTripLightApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTripLightApp initialization

BOOL CTripLightApp::InitInstance()
{
	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER < 0x0700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

#ifdef _DEBUG
	Win32Console::Create();
#endif

#ifdef PORTABLE_APP	// if building portable application
	// Set profile name to INI file in application folder
	free((void*)m_pszProfileName);
	CPathStr	IniPath(GetAppFolder());
	IniPath.Append(CString(m_pszAppName) + _T(".ini"));
	m_pszProfileName = _tcsdup(IniPath);
#else
	// Change the registry key under which our settings are stored.
	SetRegistryKey(_T("Anal Software"));
#endif

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)
	m_settings.Load();

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CTripLightDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CTripLightView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

int CTripLightApp::ExitInstance()
{
	if (m_iMidiDevOut >= 0) {
		m_midiOut.Reset();
	}
	m_settings.Store();
	return CWinApp::ExitInstance();
}

CString CTripLightApp::GetAppPath()
{
	CString	s = GetCommandLine();
	s.TrimLeft();	// trim leading whitespace just in case
	if (s[0] == '"')	// if first char is a quote
		s = s.Mid(1).SpanExcluding(_T("\""));	// span to next quote
	else
		s = s.SpanExcluding(_T(" \t"));	// span to next whitespace
	return s;
}

CString CTripLightApp::GetAppFolder()
{
	CPathStr	path(GetAppPath());
	path.RemoveFileSpec();
	return path;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	// ClassWizard generated virtual function overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

	CStatic	m_AboutText;
	CEdit	m_License;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ABOUT_LICENSE, m_License);
	DDX_Control(pDX, IDC_ABOUT_TEXT, m_AboutText);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString	s;
#ifdef _WIN64
	GetWindowText(s);
	s += _T(" (x64)");
	SetWindowText(s);
#endif
	VS_FIXEDFILEINFO	AppInfo;
	CVersionInfo::GetFileInfo(AppInfo, NULL);
	s.Format(IDS_APP_ABOUT_TEXT, theApp.m_pszAppName,
		HIWORD(AppInfo.dwFileVersionMS), LOWORD(AppInfo.dwFileVersionMS),
		HIWORD(AppInfo.dwFileVersionLS), LOWORD(AppInfo.dwFileVersionLS));
	m_AboutText.SetWindowText(s);
	m_License.SetWindowText(LDS(IDS_APP_LICENSE));
	return TRUE;
}

// App command to run the dialog
void CTripLightApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// By default, CWinApp::OnIdle is called after WM_TIMER messages.  This isn't
// normally a problem, but if the application uses a short timer, OnIdle will
// be called frequently, seriously degrading performance.  Performance can be
// improved by overriding IsIdleMessage to return FALSE for WM_TIMER messages,
// which prevents them from triggering OnIdle.  This technique can be applied
// to any idle-triggering message that repeats frequently, e.g. WM_MOUSEMOVE.
//
BOOL CTripLightApp::IsIdleMessage(MSG* pMsg)
{
	if (CWinApp::IsIdleMessage(pMsg)) {
		switch (pMsg->message) {	// don't call OnIdle after these messages
		case UWM_FRAME_TIMER:
			return FALSE;
		default:
			return TRUE;
		}
	} else
		return FALSE;
}

bool CTripLightApp::OpenMidiInputDevice(int iDevIn)
{
	if (iDevIn == m_iMidiDevIn)	// if already in requested state
		return true;	// nothing to do
	if (m_iMidiDevIn >= 0) {	// if device open
		CHECK_MIDI(m_midiIn.Close());
		m_iMidiDevIn = -1;
	}
	if (iDevIn >= 0) {	// if opening device
		CHECK_MIDI(m_midiIn.Open(iDevIn, reinterpret_cast<W64UINT>(MidiInProc), reinterpret_cast<W64UINT>(this), CALLBACK_FUNCTION));
		CHECK_MIDI(m_midiIn.Start());
		m_iMidiDevIn = iDevIn;
	}
	return true;
}

bool CTripLightApp::OpenMidiOutputDevice(int iDevOut)
{
	if (iDevOut == m_iMidiDevOut)	// if already in requested state
		return true;	// nothing to do
	if (m_iMidiDevOut >= 0) {	// if device open
		CHECK_MIDI(m_midiOut.Close());
		m_iMidiDevOut = -1;
	}
	if (iDevOut >= 0) {	// if opening device
		CHECK_MIDI(m_midiOut.Open(iDevOut, NULL, NULL, 0));
		m_iMidiDevOut = iDevOut;
	}
	return true;
}

void CALLBACK CTripLightApp::MidiInProc(HMIDIIN hMidiIn, UINT wMsg, W64UINT dwInstance, W64UINT dwParam1, W64UINT dwParam2)
{
	// this callback function runs in a worker thread context; 
	// data shared with main thread may require serialization
	static CDWordArrayEx	arrMappedEvent;
	UNREFERENCED_PARAMETER(hMidiIn);
	UNREFERENCED_PARAMETER(dwInstance);
//	_tprintf(_T("MidiInProc %d %d\n"), GetCurrentThreadId(), ::GetThreadPriority(GetCurrentThread()));
	switch (wMsg) {
	case MIM_DATA:
		{
			CMainFrame	*pFrame = theApp.GetMain();
			if (pFrame != NULL) {
				CTripLightView	*pView = theApp.GetMain()->GetView();
				if (pView != NULL) {
					pView->PostMessage(UWM_MAPPING_CHANGE, dwParam1);
				}
			}
		}
		break;
	}
}

void CTripLightApp::OnMidiError(MMRESULT nResult)
{
	AfxMessageBox(CMidiOut::GetErrorString(nResult));
}

/////////////////////////////////////////////////////////////////////////////
// CTripLightApp message handlers
