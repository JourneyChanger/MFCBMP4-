// getSearchingTextDlg.cpp: 实现文件
//

#include "pch.h"
#include "MFCBMP4.h"
#include "getSearchingTextDlg.h"
#include "afxdialogex.h"


// getSearchingTextDlg 对话框

IMPLEMENT_DYNAMIC(getSearchingTextDlg, CDialogEx)

getSearchingTextDlg::getSearchingTextDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG3, pParent)
	, TextDOI(_T(""))
	, TextCaption(_T(""))
	, TextAuthor(_T(""))
	, TextJournal(_T(""))
	, TextKey(_T(""))
{

}

getSearchingTextDlg::~getSearchingTextDlg()
{
}

void getSearchingTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT5, TextDOI);
	DDX_Text(pDX, IDC_EDIT1, TextCaption);
	DDX_Text(pDX, IDC_EDIT2, TextAuthor);
	DDX_Text(pDX, IDC_EDIT4, TextJournal);
	DDX_Text(pDX, IDC_EDIT3, TextKey);
}


BEGIN_MESSAGE_MAP(getSearchingTextDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &getSearchingTextDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// getSearchingTextDlg 消息处理程序


void getSearchingTextDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();

	UpdateData(TRUE);

	TextAuthor.Trim();
	TextDOI.Trim();
	TextJournal.Trim();
	TextKey.Trim();

	FILE* p = fopen("data\\index.txt", "r");
	if (!p) return;

	int ct = 0;
	if (fscanf(p, "%d", &ct) == -1)
		return;
	fclose(p);

	int a = 0, b = 0;
	if (!TextCaption.Trim().IsEmpty())
	{
		a = read("title", ct, TextCaption);
	}
	if (a > 0 && !TextAuthor.Trim().IsEmpty())
	{
		b = read("author", ct, TextAuthor);
		a = (a == b ? a : -1);
	}
	if (a > 0 && !TextDOI.Trim().IsEmpty())
	{
		b = read("DOI", ct, TextDOI);
		a = (a == b ? a : -1);
	}
	if (a > 0 && !TextJournal.Trim().IsEmpty())
	{
		b = read("name", ct, TextJournal);
		a = (a == b ? a : -1);
	}
	if (a > 0 && !TextKey.Trim().IsEmpty())
	{
		b = read("key", ct, TextKey);
		a = (a == b ? a : -1);
	}

	if (a > 0)
	{
		int b = read("title", ct, TextCaption);
		////
		//CString str;
		//str.Format("%d", a);
		//AfxMessageBox(str);
		////
	
		AfxMessageBox("检索成功");
		MFCBMP4dlg dlg;
		dlg.pull(a);
		dlg.DoModal();
		return;
	}
	
	AfxMessageBox("未找到!");
}
int getSearchingTextDlg::read(CString filename,int ct, CString& str)
{
	FILE* p = fopen("data\\index_" + filename + ".txt", "r");
	if (!p) return -2;
	
	char a[5000];
	CString temp;
	for (int i = 1; i <= ct; i++)
	{
		fgets(a,5000,p);
		temp = a;
		if (temp.Find(str) != -1)
			return i;
	}

	fclose(p);
	return -1;
}
