// Copyleft 2015 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      24mar15	initial version

        define registry templates
 
*/

#pragma once

#include "Persist.h"

template<class T>
void	RdReg(LPCTSTR Key, T& Value)
{
	DWORD	Size = sizeof(T);
	CPersist::GetBinary(REG_SETTINGS, Key, &Value, &Size);
}
template<class T>	
void	WrReg(LPCTSTR Key, const T& Value)
{
	CPersist::WriteBinary(REG_SETTINGS, Key, &Value, sizeof(T));
}
#define	REG_TYPE_DEF(T, prefix) \
	void	RdReg(LPCTSTR Key, T& Value)	\
	{	\
		Value = static_cast<T>(prefix theApp.GetProfileInt(REG_SETTINGS, Key, Value));	\
	}
#include "RegTypes.h"	// specialize RdReg for numeric types
#define	REG_TYPE_DEF(T, prefix) \
	void	WrReg(LPCTSTR Key, const T& Value)	\
	{	\
		theApp.WriteProfileInt(REG_SETTINGS, Key, Value);	\
	}
#include "RegTypes.h"	// specialize WrReg for numeric types
void	RdReg(LPCTSTR Key, CString& Value)
{
	Value = theApp.GetProfileString(REG_SETTINGS, Key, Value);
}
void	WrReg(LPCTSTR Key, const CString& Value)
{
	theApp.WriteProfileString(REG_SETTINGS, Key, Value);
}

// Persistence in specified section
template<class T>
void	RdReg(LPCTSTR Section, LPCTSTR Key, T& Value)
{
	DWORD	Size = sizeof(T);
	CPersist::GetBinary(Section, Key, &Value, &Size);
}
template<class T>	
void	WrReg(LPCTSTR Section, LPCTSTR Key, const T& Value)
{
	CPersist::WriteBinary(Section, Key, &Value, sizeof(T));
}
#define	REG_TYPE_DEF(T, prefix) \
	void	RdReg(LPCTSTR Section, LPCTSTR Key, T& Value)	\
	{	\
		Value = static_cast<T>(prefix theApp.GetProfileInt(Section, Key, Value));	\
	}
#include "RegTypes.h"	// specialize RdReg for numeric types
#define	REG_TYPE_DEF(T, prefix) \
	void	WrReg(LPCTSTR Section, LPCTSTR Key, const T& Value)	\
	{	\
		theApp.WriteProfileInt(Section, Key, Value);	\
	}
#include "RegTypes.h"	// specialize WrReg for numeric types
void	RdReg(LPCTSTR Section, LPCTSTR Key, CString& Value)
{
	Value = theApp.GetProfileString(Section, Key, Value);
}
void	WrReg(LPCTSTR Section, LPCTSTR Key, const CString& Value)
{
	theApp.WriteProfileString(Section, Key, Value);
}
