#if !defined(AFX_FILEVIEW_H__A28E840B_949B_4251_8A14_ADEFB3BE0736__INCLUDED_)
#define AFX_FILEVIEW_H__A28E840B_949B_4251_8A14_ADEFB3BE0736__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileView view
#include <AFXCVIEW.H>

class CFileView : public CTreeView
{
protected:
	CFileView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CFileView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CFileView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CFileView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnAfxIdPreviewClose();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEVIEW_H__A28E840B_949B_4251_8A14_ADEFB3BE0736__INCLUDED_)
