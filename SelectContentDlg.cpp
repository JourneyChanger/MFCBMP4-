// SelectContentDlg.cpp: 实现文件
//

#include "pch.h"
#include "MFCBMP4.h"
#include "SelectContentDlg.h"
#include "afxdialogex.h"


// SelectContentDlg 对话框

IMPLEMENT_DYNAMIC(SelectContentDlg, CDialogEx)

SelectContentDlg::SelectContentDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{

}

SelectContentDlg::~SelectContentDlg()
{
}

void SelectContentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_SelcetDlg);
}


BEGIN_MESSAGE_MAP(SelectContentDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &SelectContentDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// SelectContentDlg 消息处理程序


BOOL SelectContentDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_SelcetDlg.SetCurSel(1);
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


int SelectContentDlg::getCurSel()
{
	// TODO: 在此处添加实现代码.

	//int a = m_SelcetDlg.GetCurSel();
	return 0; 
}


void SelectContentDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();

	m_nIndex = ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel();//当前选中的行。
}


int SelectContentDlg::getSelIndex()
{
	// TODO: 在此处添加实现代码.
	return m_nIndex;
}
