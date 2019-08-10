
// MFCBMP4View.h: CMFCBMP4View 类的接口
//

#pragma once

#include <atlimage.h>
#include "MFCBMP4Doc.h"
#include "Dib.h"
#include "project.h"

class CMFCBMP4View : public CScrollView
{
protected: // 仅从序列化创建
	CMFCBMP4View() noexcept;
	DECLARE_DYNCREATE(CMFCBMP4View)

// 特性
public:
	CMFCBMP4Doc* GetDocument() const;

// 操作
public:

	// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);


// 实现
public:
	virtual ~CMFCBMP4View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CString m_strWorkDir;
	CString m_strFileName;
	CString m_strFileType;

	int m_nFileCt;
	Analysis m_analy;
	//bool bMutiDisplay;
// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
private:
	CImage image;
public:
	afx_msg void OnFlush();
	afx_msg void OnRecognize();
	afx_msg void OnSearch();
};

#ifndef _DEBUG  // MFCBMP4View.cpp 中的调试版本
inline CMFCBMP4Doc* CMFCBMP4View::GetDocument() const
   { return reinterpret_cast<CMFCBMP4Doc*>(m_pDocument); }
#endif

