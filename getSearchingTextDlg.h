#pragma once

#include"MFCBMP4dlg.h"

// getSearchingTextDlg 对话框

class getSearchingTextDlg : public CDialogEx
{
	DECLARE_DYNAMIC(getSearchingTextDlg)

public:
	getSearchingTextDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~getSearchingTextDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString TextDOI;
	CString TextCaption;
	CString TextAuthor;
	CString TextJournal;
	CString TextKey;
	afx_msg void OnBnClickedOk();

	int read(CString filename, int ct, CString& str);
};
