// TextRecognizeBase.h: interface for the CTextRecognizeBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTRECOGNIZEBASE_H__2742212D_B229_447D_A11F_CFCC05C592F7__INCLUDED_)
#define AFX_TEXTRECOGNIZEBASE_H__2742212D_B229_447D_A11F_CFCC05C592F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef int STYLE;
typedef struct 
{
	CRect rect;
	STYLE style;
} LAYOUT;

class CDib;

namespace NCUT_OCR
{

class CTextRecognizeBase  
{
public:
	CTextRecognizeBase();
	virtual ~CTextRecognizeBase();

	virtual BOOL Do(CDib& ) = 0;
	virtual BOOL Do(const CString& ) = 0;
	virtual BOOL Initialize(HWND ) = 0;
	virtual void Uninitialize() = 0;  // 2012.01.12
	virtual double GetRecoRate() = 0; // 2010.06.17 获得识别率
	virtual BOOL Layout(const CString& strFileName, int nMaxRgnNum, int &nImgRgnNum, LAYOUT *pLayout) = 0;
	virtual BOOL Layout(const CDib& dib, int nMaxRgnNum, int &nImgRgnNum, LAYOUT *pLayout) = 0;

	virtual void SetCorrect(BOOL bCorrect)  // 2012.08.05
	{
		m_bCorrect = bCorrect;
	}
	virtual BOOL GetCorrect() const // 2012.08.05
	{
		return m_bCorrect;
	}

	BOOL IsInitialized() const 
	{
		return m_bInitialized;
	}
	CString GetText() const 
	{
		return m_strText;
	}
	CString GetError() const 
	{
		return m_strError;
	}

	int GetTotal() const
	{
		return m_nTotal;
	}
	int GetDubiousCount() const
	{
		return m_nDubiousCount;
	}

protected:
	BOOL m_bInitialized;
	BOOL m_bCorrect;  // 2012.08.05
	CString m_strText;
	CString m_strError;

	int m_nTotal;
	int m_nDubiousCount;

};

}

#endif // !defined(AFX_TEXTRECOGNIZEBASE_H__2742212D_B229_447D_A11F_CFCC05C592F7__INCLUDED_)
