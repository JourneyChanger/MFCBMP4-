
// MFCBMP4.h: MFCBMP4 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"       // 主符号


// CMFCBMP4App:
// 有关此类的实现，请参阅 MFCBMP4.cpp
//

class CMFCBMP4App : public CWinApp
{
public:
	CMFCBMP4App() noexcept;

	CString GetWorkDirectory() const
	{
		return m_strWorkDir;
	}

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

protected:
	CString m_strWorkDir;
};

extern CMFCBMP4App theApp;
