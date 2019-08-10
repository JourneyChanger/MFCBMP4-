// FileView.cpp : implementation file
//

#include "afx.h"
#include "MFCBMP4.h"
#include "FileView.h"
#include "pch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileView

IMPLEMENT_DYNCREATE(CFileView, CTreeView)

CFileView::CFileView()
{
}

CFileView::~CFileView()
{
}


BEGIN_MESSAGE_MAP(CFileView, CTreeView)
	//{{AFX_MSG_MAP(CFileView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_COMMAND(AFX_ID_PREVIEW_CLOSE, &CFileView::OnAfxIdPreviewClose)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileView drawing

void CFileView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CFileView diagnostics

#ifdef _DEBUG
void CFileView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CFileView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFileView message handlers


void CFileView::OnAfxIdPreviewClose()
{
	// TODO: 在此添加命令处理程序代码
}
