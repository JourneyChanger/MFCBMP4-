#pragma once


// MFCBMP4dlg 对话框

#include "project.h"
#include "getSearchingTextDlg.h"

class MFCBMP4dlg : public CDialogEx
{
	DECLARE_DYNAMIC(MFCBMP4dlg)

public:
	MFCBMP4dlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~MFCBMP4dlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString name;
	CString title;
	CString author;
	CString company;
	CString keyword;
	CString time;
	CString en_title;
	CString en_author;
	CString en_company;
	CString en_keyword;
	CString Reference;
	CString DOI;
	CString CNAbstract;
	CString ENAbstract;


	bool setData(Analysis& A);
	int addData(Analysis& A);
public:
	bool saveData();
private:
	//创建索引
	bool save(CString filename, CString& str1, CString& str2);//中英文
	bool save(CString filename, CString& str);//中或英
	bool save(int ct, CString& str);//reference
	bool save(int ct);//保存全部数据

public:
	bool pull(int index);//拉取检索内容
private:
	void GetLine(FILE* p, char* str);//pull所用,按行读取
	void GetReference(int index);
};
