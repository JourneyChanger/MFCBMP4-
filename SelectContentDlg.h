#pragma once


// SelectContentDlg 对话框

class SelectContentDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SelectContentDlg)

public:
	SelectContentDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~SelectContentDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_SelcetDlg;
	virtual BOOL OnInitDialog();
	int getCurSel();
	afx_msg void OnBnClickedOk();
	int m_nIndex;
	int getSelIndex();
};
